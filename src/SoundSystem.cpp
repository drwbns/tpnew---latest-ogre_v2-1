/*
Copyright (c) 2010 Yunus Kara

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#include "SoundSystem.h"
#include "SoundStreamingBuffer.h"

#include <OgreRoot.h>
#include <OgreLogManager.h>
#include <OgreStringConverter.h>


template<> SoundSystem* Ogre::Singleton<SoundSystem>::msSingleton = NULL;


//----------------------------------------------------------------------------//
SoundSystem& SoundSystem::getSingleton(void)
{
	return *msSingleton;
}
//----------------------------------------------------------------------------//
SoundSystem* SoundSystem::getSingletonPtr(void)
{
	return msSingleton;
}
//----------------------------------------------------------------------------//
SoundSystem::SoundSystem()
{
}
//----------------------------------------------------------------------------//
bool SoundSystem::initialize()
{
	logMessage("SoundSystem Initialization:");

	mListenerFactory = OGRE_NEW SoundListenerFactory();
	mSourceFactory = OGRE_NEW SoundSourceFactory();

	Ogre::Root::getSingleton().addMovableObjectFactory(mSourceFactory);
	Ogre::Root::getSingleton().addMovableObjectFactory(mListenerFactory);

	alutInitWithoutContext(NULL, NULL);

	logMessage("Creating OpenAL");

	// Enumerate the available devices and choose one
	mDevices = OGRE_NEW SoundDevices();
	mDevices->logDevices();

	//TODO: Choose Device
	//Ogre::String mDeviceName =Application::getSingleton().getConfig(mName, "DEVICE");
	//mSelectedDevice = mDevices->getDeviceIndex(mDeviceName.c_str());
	//if (mSelectedDevice == -1)
	//{
		mSelectedDevice = mDevices->getDefaultDevice();
		Ogre::String mDeviceName = mDevices->getDeviceName(mSelectedDevice);
		//TOY::ABase::Application::getSingleton().setConfig(mName, "DEVICE", mDeviceName);
	//}
	ALCdevice *device = alcOpenDevice(mDeviceName.c_str());
	logMessage("Choosing: " + mDeviceName);

	if(device == NULL)
	{
		throw Ogre::Exception(Ogre::Exception::ERR_INTERNAL_ERROR,
			"Failed to open audio device", __FUNCTION__);
	}

	// Create OpenAL Context
	mContext = alcCreateContext(device, NULL);
	if(mContext == NULL)
	{
		throw Ogre::Exception(Ogre::Exception::ERR_INTERNAL_ERROR,
			"Failed to create OpenAL Context", __FUNCTION__);
	}

	alcMakeContextCurrent(mContext);
	checkError(__FUNCTION__);

	// Check for Supported Formats
	ALenum eBufferFormat = 0;
	eBufferFormat = alcGetEnumValue(device, "AL_FORMAT_MONO16");
	if(eBufferFormat) mSupportedFormats[MONO_CHANNEL] = 
		OGRE_NEW FormatData(eBufferFormat, "AL_FORMAT_MONO16", "Monophonic Sound");
	eBufferFormat = alcGetEnumValue(device, "AL_FORMAT_STEREO16");
	if(eBufferFormat) mSupportedFormats[STEREO_CHANNEL] = 
		OGRE_NEW FormatData(eBufferFormat, "AL_FORMAT_STEREO16", "Stereo Sound");
	eBufferFormat = alcGetEnumValue(device, "AL_FORMAT_QUAD16");
	if(eBufferFormat) mSupportedFormats[QUAD_CHANNEL] = 
		OGRE_NEW FormatData(eBufferFormat, "AL_FORMAT_QUAD16", "4 Channel Sound");
	eBufferFormat = alcGetEnumValue(device, "AL_FORMAT_51CHN16");
	if(eBufferFormat) mSupportedFormats[MULTI_CHANNEL_51] = 
		OGRE_NEW FormatData(eBufferFormat, "AL_FORMAT_51CHN16", "5.1 Surround Sound");
	eBufferFormat = alcGetEnumValue(device, "AL_FORMAT_61CHN16");
	if(eBufferFormat) mSupportedFormats[MULTI_CHANNEL_61] = 
		OGRE_NEW FormatData(eBufferFormat, "AL_FORMAT_61CHN16", "6.1 Surround Sound");
	eBufferFormat = alcGetEnumValue(device, "AL_FORMAT_71CHN16");
	if(eBufferFormat) mSupportedFormats[MULTI_CHANNEL_71] = 
		OGRE_NEW FormatData(eBufferFormat, "AL_FORMAT_71CHN16", "7.1 Surround Sound");

	FormatMapIterator itr = getSupportedFormatIterator();
	logMessage("Supported Formats");
	logMessage("-----------------");
	while(itr.hasMoreElements())
	{
		logMessage(" * " + std::string(static_cast<char*>
			(itr.peekNextValue()->formatName)) + ", " + itr.peekNextValue()->formatDescription);
		itr.getNext();
	}

	// Check for EAX Support
	std::stringbuf versionString;
	for(int version = 5; version >= 2; version--)
	{
		versionString.str("EAX"+Ogre::StringConverter::toString(version)+".0");
		if(alIsExtensionPresent(versionString.str().data()) == AL_TRUE)
		{
			mEAXSupport = true;
			mEAXVersion = version;
			versionString.str("EAX " + Ogre::StringConverter::toString(version) + ".0 Detected");
			logMessage(versionString.str());
			break;
		}
	}

	// Check for X-RAM extension
	if(alIsExtensionPresent("EAX-RAM") == AL_TRUE)
	{
		mXRAMSupport = true;
		logMessage("X-RAM Detected");

		EAXSetBufferMode setXRamMode = (EAXSetBufferMode)alGetProcAddress("EAXSetBufferMode");
		EAXGetBufferMode getXRamMode = (EAXGetBufferMode)alGetProcAddress("EAXGetBufferMode");
		mXRamSize = alGetEnumValue("AL_EAX_RAM_SIZE");
		mXRamFree = alGetEnumValue("AL_EAX_RAM_FREE");
		
		logMessage("X-RAM: " + Ogre::StringConverter::toString(mXRamSize) +
			" (" + Ogre::StringConverter::toString(mXRamFree) + " free)");
	}

	createListener();

	return true;
}
//----------------------------------------------------------------------------//
void SoundSystem::finalize()
{
	logMessage("SoundSystem Shutdown");

	// Destroy the Listener and all Sounds
	destroyAllSources();
	destroyAllBuffers();
	delete SoundListener::getSingletonPtr();

	// Unregister the Sound and Listener Factories
	Ogre::Root::getSingleton().removeMovableObjectFactory(mSourceFactory);
	Ogre::Root::getSingleton().removeMovableObjectFactory(mListenerFactory);

	delete mListenerFactory;
	delete mSourceFactory;
	delete mDevices;

	// delete all FormatData pointers in the FormatMap;
	std::for_each(mSupportedFormats.begin(), mSupportedFormats.end(), DeleteSecond());
	logMessage("Releasing OpenAL");

	// Release the OpenAL Context and the Audio device
	ALCcontext *context = alcGetCurrentContext();
	ALCdevice *device = alcGetContextsDevice(context);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);
	alutExit();
}
//----------------------------------------------------------------------------//
struct SoundSystem::UpdateSource
{
	UpdateSource(float elapsedTime)
	{
		mElapsedTime = elapsedTime;
	}

	void operator()(std::pair<std::string, SoundSource*> pair)const
	{
		pair.second->_updateSound(mElapsedTime);
	}

	float mElapsedTime;
};
//----------------------------------------------------------------------------//
struct SoundSystem::SortSources
{
	bool operator()(SoundSource* x, SoundSource* y)
	{
		if ( x->getPriority() < y->getPriority())
			return true;
		else if ( x->getPriority() > y->getPriority())
			return false;

		if(x->getEffectiveGain() > y->getEffectiveGain ())
			return true;

		return false;
	}
};
//----------------------------------------------------------------------------//
bool SoundSystem::update(float elapsedTime)
{

	// Update the Sound and Listeners if necessary	
	std::for_each(mSourceMap.begin(), mSourceMap.end(), UpdateSource(elapsedTime));
	SoundListener::getSingleton().updateListener();

	mPlayingSources.clear();

	for(SourceMap::iterator it = mSourceMap.begin(); it != mSourceMap.end(); ++it)
	{
		if (it->second->isPlaying())
			mPlayingSources.push_back(it->second);
		else if(it->second->isActive())
			it->second->activate(false);
	}

	size_t mActiveSourceCount;
	if(mPlayingSources.size() < mDevices->getMaxNumSources(mSelectedDevice))
		mActiveSourceCount = mPlayingSources.size();
	else
	{
		//TODO: sort
		std::sort(mPlayingSources.begin(), mPlayingSources.end(), SortSources());
		mActiveSourceCount = mDevices->getMaxNumSources(mSelectedDevice);
	}

	for(size_t i = mActiveSourceCount; i < mPlayingSources.size(); ++i)
		mPlayingSources[i]->activate(false);

	for(size_t i = 0; i < mActiveSourceCount; ++i)
		mPlayingSources[i]->activate(true);


	return true;
}
//----------------------------------------------------------------------------//
void SoundSystem::checkError(const Ogre::String& reference)
{
	int error = alGetError();

	if(error != AL_NO_ERROR)
	{
		throw Ogre::Exception(3, "OpenAL Error: " + errorToString(error), reference);
	}
}
//----------------------------------------------------------------------------//
Ogre::String SoundSystem::errorToString(int error)
{
	switch(error)
	{
		case AL_INVALID_VALUE:
			return Ogre::String("The value pointer given is not valid");
			break;
		case AL_INVALID_ENUM:
			return Ogre::String("The specified parameter is not valid");
			break;
		case AL_INVALID_NAME:
			return Ogre::String("The specified source name is not valid");
			break;
		case AL_INVALID_OPERATION:
			return Ogre::String("There is no current context");
			break;
		default:
			return Ogre::String("Unknown Error");
			break;
	}
}

//----------------------------------------------------------------------------//
SoundListener* SoundSystem::createListener()
{
	SoundListener *listener = SoundListener::getSingletonPtr();
	if(!listener)
	{
		listener = static_cast<SoundListener*>
			(mListenerFactory->createInstance("ListenerSingleton", NULL));
	}

	return listener;
}

//----------------------------------------------------------------------------//
SoundSource* SoundSystem::createSource(const Ogre::String& name, const Ogre::String& buffer, bool streaming)
{
	if (mSourceMap.find(name) != mSourceMap.end())
	{
		throw Ogre::Exception(Ogre::Exception::ERR_DUPLICATE_ITEM,
			"A Source with name '" + name + "' already exists.",
			__FUNCTION__);
	}

	SoundSource *newSource = static_cast<SoundSource*>(mSourceFactory->createInstance(name, NULL));
	newSource->attachBuffer(buffer, streaming);

	if (SoundSource::getActiveCount() < mDevices->getMaxNumSources(mSelectedDevice) )
		newSource->activate(true);

	mSourceMap[name] = newSource;
	return newSource;
}
//----------------------------------------------------------------------------//
void SoundSystem::destroyAllSources()
{
	std::for_each(mSourceMap.begin(), mSourceMap.end(), DeleteSecond());
	mSourceMap.clear();
}
//----------------------------------------------------------------------------//
void SoundSystem::destroySource(SoundSource* sound)
{
	SourceMap::iterator it = mSourceMap.find(sound->getName());
	if( it != mSourceMap.end())
	{
		mSourceMap.erase(it);
		delete sound;
	}

	for(SourceVector::iterator i = mPausedSources.begin(); i != mPausedSources.end(); ++i)
		if ( sound == (*i))
		{
			mPausedSources.erase(i);
			break;
		}
}
//----------------------------------------------------------------------------//
SoundBuffer* SoundSystem::createBuffer(const Ogre::String& fileName)
{
	if (mBufferMap.find(fileName) != mBufferMap.end())
	{
		throw Ogre::Exception(Ogre::Exception::ERR_DUPLICATE_ITEM,
			"A Buffer with name '" + fileName + "' already exists.",
			__FUNCTION__);
	}

	SoundBuffer* mBuffer;
	if(fileName.find(".ogg") != std::string::npos || fileName.find(".OGG") != std::string::npos)
		mBuffer = OGRE_NEW OggBuffer(fileName);
	else if(fileName.find(".wav") != std::string::npos || fileName.find(".WAV") != std::string::npos)
		mBuffer = OGRE_NEW WavBuffer(fileName);

	mBufferMap[fileName] = mBuffer;
	return mBuffer;
}
//----------------------------------------------------------------------------//
SoundBuffer* SoundSystem::createStreamingBuffer(SoundSource* mSource, const Ogre::String& fileName)
{

	StreamingBuffer* mBuffer = NULL;
	if(fileName.find(".ogg") != std::string::npos || fileName.find(".OGG") != std::string::npos)
		mBuffer = OGRE_NEW OgGAMESTATEream(fileName, mSource, 4);
	//else if(fileName.find(".wav") != std::string::npos || fileName.find(".WAV") != std::string::npos)
	//	mBuffer = new WavBuffer(fileName);

	if(mBuffer)
		mStreamingBuffers.push_back(mBuffer);

	return mBuffer;
}
//----------------------------------------------------------------------------//
SoundBuffer* SoundSystem::getBuffer(const Ogre::String& fileName)
{
	SoundBuffer* mBuffer;
	BufferMap::iterator it = mBufferMap.find(fileName);

	if(it == mBufferMap.end())
		mBuffer = createBuffer(fileName);
	else
		mBuffer = it->second;

	return mBuffer;
}
//----------------------------------------------------------------------------//
void SoundSystem::destroyAllBuffers()
{
	std::for_each(mBufferMap.begin(), mBufferMap.end(), DeleteSecond());
	for(size_t i = 0; i < mStreamingBuffers.size(); i++)
		delete mStreamingBuffers[i];

	mBufferMap.clear();
	mStreamingBuffers.clear();
}
//----------------------------------------------------------------------------//
FormatMapIterator SoundSystem::getSupportedFormatIterator()
{
	return FormatMapIterator(mSupportedFormats.begin(), mSupportedFormats.end());
}
//----------------------------------------------------------------------------//
void SoundSystem::restoreContext()
{
	alcMakeContextCurrent(mContext);
}
//----------------------------------------------------------------------------//
void SoundSystem::logMessage(const Ogre::String& msg)
{
	Ogre::LogManager::getSingleton().getDefaultLog()->logMessage(msg);
}
//----------------------------------------------------------------------------//
void SoundSystem::pauseAllSounds()
{
	for(SourceMap::iterator it = mSourceMap.begin(); it != mSourceMap.end(); ++it)
	{
		if(it->second->isPlaying())
		{
			it->second->pause();
			mPausedSources.push_back(it->second);
		}
	}
}
//----------------------------------------------------------------------------//
void SoundSystem::resumeAllSounds()
{
	for(SourceVector::iterator it = mPausedSources.begin(); it != mPausedSources.end(); ++it)
		(*it)->play();

	mPausedSources.clear();
}


//----------------------------------------------------------------------------//
SoundSource* SoundSystem::getSource(const Ogre::String& name)
{
	return mSourceMap[name];
}
//----------------------------------------------------------------------------//
SoundSystem::DistanceModel SoundSystem::getDistanceModel() const
{
	switch(alGetInteger(AL_DISTANCE_MODEL))
	{
	case AL_NONE:
		return DMNone;

	case AL_INVERSE_DISTANCE:
		return DMInverseDistance;

	case AL_INVERSE_DISTANCE_CLAMPED:
		return DMInverseDistanceClamped;

	case AL_LINEAR_DISTANCE:
		return DMLinearDistance;

	case AL_LINEAR_DISTANCE_CLAMPED:
		return DMLinearDistanceClamped;

	case AL_EXPONENT_DISTANCE:
		return DMExponentDistance;

	case AL_EXPONENT_DISTANCE_CLAMPED:
		return DMExponentDistanceClamped;

	}

}
void SoundSystem::setDistanceModel(DistanceModel model)
{
	switch(model)
	{
	case DMNone:
		alDistanceModel(AL_NONE);
		break;

	case DMInverseDistance:
		alDistanceModel(AL_INVERSE_DISTANCE);
		break;

	case DMInverseDistanceClamped:
		alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
		break;

	case DMLinearDistance:
		alDistanceModel(AL_LINEAR_DISTANCE);
		break;

	case DMLinearDistanceClamped:
		alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
		break;

	case DMExponentDistance:
		alDistanceModel(AL_EXPONENT_DISTANCE);
		break;

	case DMExponentDistanceClamped:
		alDistanceModel(AL_EXPONENT_DISTANCE_CLAMPED);
		break;
	}
}