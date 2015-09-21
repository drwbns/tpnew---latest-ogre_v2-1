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


/************************************************************************/ 
#include "SoundSource.h"
#include "SoundBuffer.h"
#include "SoundTimer.h"
#include "SoundListener.h"
#include "SoundSystem.h"

#include "OgreNode.h"

/************************************************************************/ 
Ogre::String SoundSourceFactory::FACTORY_TYPE_NAME = "SoundSource";
size_t SoundSource::mActiveCount = 0;
/************************************************************************/ 
SoundSource::SoundSource(const Ogre::String &name) :
	MovableObject(name),
	mPitch(1.0),
	mGain(1.0),
	mMaxGain(1.0),
	mMinGain(0.0),
	mMaxDistance(3400.0),
	mRolloffFactor(1.0),
	mReferenceDistance(150.0),
	mOuterConeGain(0.0),
	mInnerConeAngle(360.0),
	mOuterConeAngle(360.0),
	mPosition(Ogre::Vector3::ZERO),
	mVelocity(Ogre::Vector3::ZERO),
	mDirection(Ogre::Vector3::NEGATIVE_UNIT_Z),
	mSourceRelative(AL_FALSE),
	mDerivedPosition(Ogre::Vector3::ZERO),
	mDerivedDirection(Ogre::Vector3::NEGATIVE_UNIT_Z),
	mActive(false),
	mState(AL_INITIAL),
	mBuffer(NULL),
	mLoop(AL_FALSE),
	mSource(0),
	mPriority(0),
	mDistanceAttenuation(0.0)
{
	mParentNode = NULL;
	mTimer = new SoundTimer();
}
/************************************************************************/ 
SoundSource::~SoundSource()
{
	deinitSource();
	delete mTimer;
}
/************************************************************************/ 
bool SoundSource::play()
{
	if(mActive)
		mBuffer->play(mSource);

	mTimer->play();
	mState = AL_PLAYING;
	return true;
}
/************************************************************************/ 
bool SoundSource::isPlaying() const
{
	return (mState == AL_PLAYING);
}
/************************************************************************/ 
bool SoundSource::pause()
{
	if(mActive)
		mBuffer->pause(mSource);

	mTimer->pause();
	mState = AL_PAUSED;
	return true;
}
/************************************************************************/ 
bool SoundSource::isPaused() const
{
	return ( mState == AL_PAUSED);
}
/************************************************************************/ 
bool SoundSource::stop()
{
	if(mActive)
		mBuffer->stop(mSource);

	mTimer->stop();
	mState = AL_STOPPED;
	return true;
}
/************************************************************************/ 
bool SoundSource::isStopped() const
{
	return (mState == AL_STOPPED);
}
/************************************************************************/ 
bool SoundSource::seek(Ogre::Real position)
{
	if(mActive)
		mBuffer->seek(mSource, position);

	mTimer->seek(position);
	return true;
}
/************************************************************************/ 
bool SoundSource::isInitial() const
{
	return (mState == AL_INITIAL);
}
/************************************************************************/ 
void SoundSource::setPitch(Ogre::Real pitch)
{
	if(pitch < 0)
		pitch = 0;

	mTimer->setPitch(pitch);
	mPitch = pitch;
	if(mActive)
		alSourcef(mSource, AL_PITCH, mPitch);
	SoundSystem::checkError(__FUNCTION__);
}
/************************************************************************/ 
void SoundSource::setGain(Ogre::Real gain)
{
	if (gain < 0)
		gain = 0;

	mGain = gain;
	if(mActive)
		alSourcef(mSource, AL_GAIN, mGain);
	SoundSystem::checkError(__FUNCTION__);
}
/************************************************************************/ 
void SoundSource::setMaxGain(Ogre::Real maxGain)
{
	mMaxGain = maxGain;
	if(mActive)
		alSourcef(mSource, AL_MAX_GAIN, mMaxGain);
	SoundSystem::checkError(__FUNCTION__);
}
/************************************************************************/ 
void SoundSource::setMinGain(Ogre::Real minGain)
{
	mMinGain = minGain;
	if(mActive)
		alSourcef(mSource, AL_MIN_GAIN, mMinGain);
	SoundSystem::checkError(__FUNCTION__);
}
/************************************************************************/ 
void SoundSource::setMaxDistance(Ogre::Real maxDistance)
{
	mMaxDistance = maxDistance;
	if(mActive)
		alSourcef(mSource, AL_MAX_DISTANCE, mMaxDistance);
	SoundSystem::checkError(__FUNCTION__);
}
/************************************************************************/ 
void SoundSource::setRolloffFactor(Ogre::Real rolloffFactor)
{
	mRolloffFactor = rolloffFactor;
	if(mActive)
		alSourcef(mSource, AL_ROLLOFF_FACTOR, mRolloffFactor);
	SoundSystem::checkError(__FUNCTION__);
}
/************************************************************************/ 
void SoundSource::setReferenceDistance(Ogre::Real refDistance)
{
	mReferenceDistance = refDistance;
	if(mActive)
		alSourcef(mSource, AL_REFERENCE_DISTANCE, mReferenceDistance);
	SoundSystem::checkError(__FUNCTION__);
}
/************************************************************************/ 
void SoundSource::setVelocity(Ogre::Real x, Ogre::Real y, Ogre::Real z)
{
	mVelocity.x = x;
	mVelocity.y = y;
	mVelocity.z = z;
	if(mActive)
		alSource3f(mSource, AL_VELOCITY, mVelocity.x, mVelocity.y, mVelocity.z);
	SoundSystem::checkError(__FUNCTION__);
}
/************************************************************************/ 
void SoundSource::setVelocity(const Ogre::Vector3& vec)
{
	mVelocity = vec;
	if(mActive)
		alSource3f(mSource, AL_VELOCITY, mVelocity.x, mVelocity.y, mVelocity.z);
	SoundSystem::checkError(__FUNCTION__);
}
/************************************************************************/ 
void SoundSource::setRelativeToListener(bool relative)
{
	// Do not set to relative if it's connected to a node
	if(mParentNode) 
		return;

	mSourceRelative = relative;
	if(mActive)
		alSourcei(mSource, AL_SOURCE_RELATIVE, mSourceRelative);
	SoundSystem::checkError(__FUNCTION__);
}
/************************************************************************/ 
void SoundSource::setOuterConeGain(Ogre::Real outerConeGain)
{
	mOuterConeGain = outerConeGain;
	if(mActive)
		alSourcef(mSource, AL_CONE_OUTER_GAIN, mOuterConeGain);
	SoundSystem::checkError(__FUNCTION__);
}
/************************************************************************/ 
void SoundSource::setInnnerConeAngle(Ogre::Real innerConeAngle)
{
	mInnerConeAngle = innerConeAngle;
	if(mActive)
		alSourcef(mSource, AL_CONE_INNER_ANGLE, mInnerConeAngle);
	SoundSystem::checkError(__FUNCTION__);
}
/************************************************************************/ 
void SoundSource::setOuterConeAngle(Ogre::Real outerConeAngle)
{
	mOuterConeAngle = outerConeAngle;
	if(mActive)
		alSourcef(mSource, AL_CONE_OUTER_ANGLE, mOuterConeAngle);
	SoundSystem::checkError(__FUNCTION__);
}
/************************************************************************/ 
void SoundSource::setLoop(bool loop)
{
	mLoop = loop?AL_TRUE:AL_FALSE;
	if(mActive)
		mBuffer->setLoop(mSource, loop);
}
/************************************************************************/ 
void SoundSource::setDistanceValues(Ogre::Real maxDistance, Ogre::Real rolloffFactor, Ogre::Real refDistance)
{
	setMaxDistance(maxDistance);
	setRolloffFactor(rolloffFactor);
	setReferenceDistance(refDistance);
}
/************************************************************************/ 
void SoundSource::setPosition(Ogre::Real x, Ogre::Real y, Ogre::Real z)
{
	mPosition.x = x;
	mPosition.y = y;
	mPosition.z = z;
	mLocalTransformDirty = true;
}
/************************************************************************/ 
void SoundSource::setPosition(const Ogre::Vector3& vec)
{
	mPosition = vec;
	mLocalTransformDirty = true;
}
/************************************************************************/ 
const Ogre::Vector3& SoundSource::getPosition() const
{
	return mPosition;
}
/************************************************************************/ 
void SoundSource::setDirection(Ogre::Real x, Ogre::Real y, Ogre::Real z)
{
	mDirection.x = x;
	mDirection.y = y;
	mDirection.z = z;
	mLocalTransformDirty = true;
}
/************************************************************************/ 
void SoundSource::setDirection(const Ogre::Vector3& vec)
{
	mDirection = vec;
	mLocalTransformDirty = true;
}
/************************************************************************/ 
const Ogre::Vector3& SoundSource::getDirection() const
{
	return mDirection;
}
/************************************************************************/ 
const Ogre::Vector3& SoundSource::getDerivedPosition(void) const
{
	return mDerivedPosition;
}
/************************************************************************/ 
const Ogre::Vector3& SoundSource::getDerivedDirection(void) const
{
	return mDerivedDirection;
}
/************************************************************************/ 
void SoundSource::_update() const
{
	if (mParentNode)
	{
		if (!(mParentNode->_getDerivedOrientation() == mLastParentOrientation &&
			mParentNode->_getDerivedPosition() == mLastParentPosition)
			|| mLocalTransformDirty)
		{
			// Ok, we're out of date with SceneNode we're attached to
			mLastParentOrientation = mParentNode->_getDerivedOrientation();
			mLastParentPosition = mParentNode->_getDerivedPosition();
			mDerivedDirection = mLastParentOrientation * mDirection;
			mDerivedPosition = (mLastParentOrientation * mPosition) + mLastParentPosition;
		}
	}
	else
	{
		mDerivedPosition = mPosition;
		mDerivedDirection = mDirection;
	}

	mLocalTransformDirty = false;
}
/************************************************************************/ 
bool SoundSource::_updateSound(Ogre::Real ElapsedTime)
{
	_update();
	if(mActive)
	{
		alSource3f(mSource, AL_POSITION, mDerivedPosition.x, mDerivedPosition.y, mDerivedPosition.z);
		alSource3f(mSource, AL_DIRECTION, mDerivedDirection.x, mDerivedDirection.y, mDerivedDirection.z);
		mBuffer->update(mSource, ElapsedTime);
		alGetSourcei(mSource, AL_SOURCE_STATE, &mState);
		SoundSystem::checkError(__FUNCTION__);

		updateDistanceAttenuation();
		updateEffectiveGain();
	}
	else 
	{
		switch(mState)
		{
		case AL_PLAYING:
			//if not looping and duration has passed stop
			if (mTimer->getPosition() > mBuffer->getDuration() && ! mLoop)
			{
				mTimer->stop();
				mState = AL_STOPPED;
			}
			break;
		case AL_STOPPED:
		case AL_INITIAL:
		case AL_PAUSED:
			//NOOP;
			break;
		}
	}

	return true;
}
/************************************************************************/ 
void SoundSource::initSource()
{
	if(!mActive)
	{
		mActiveCount++;
		alGenSources(1, &mSource);
		SoundSystem::checkError(__FUNCTION__);
		alSourcef (mSource, AL_PITCH,				mPitch);
		alSourcef (mSource, AL_GAIN,				mGain);
		alSourcef (mSource, AL_MAX_GAIN,			mMaxGain);
		alSourcef (mSource, AL_MIN_GAIN,			mMinGain);
		alSourcef (mSource, AL_MAX_DISTANCE,		mMaxDistance);
		alSourcef (mSource, AL_ROLLOFF_FACTOR,		mRolloffFactor);
		alSourcef (mSource, AL_REFERENCE_DISTANCE,	mReferenceDistance);
		alSourcef (mSource, AL_CONE_OUTER_GAIN,		mOuterConeGain);
		alSourcef (mSource, AL_CONE_INNER_ANGLE,	mInnerConeAngle);
		alSourcef (mSource, AL_CONE_OUTER_ANGLE,	mOuterConeAngle);
		alSource3f(mSource, AL_POSITION,			mDerivedPosition.x, mDerivedPosition.y, mDerivedPosition.z);
		alSource3f(mSource, AL_VELOCITY,			mVelocity.x, mVelocity.y, mVelocity.z);
		alSource3f(mSource, AL_DIRECTION,			mDerivedDirection.x, mDerivedDirection.y, mDerivedDirection.z);
		alSourcei (mSource, AL_SOURCE_RELATIVE,		mSourceRelative);
		
		mBuffer->setLoop(mSource, mLoop != 0);
		SoundSystem::checkError(__FUNCTION__);
		mActive = true;
	}
}
/************************************************************************/ 
void SoundSource::deinitSource()
{
	if(mActive)
	{
		mActiveCount--;
		alSourceStop(mSource);
		alSourcei(mSource, AL_BUFFER, 0);
		alDeleteSources(1, &mSource);
		SoundSystem::checkError(__FUNCTION__);
		mActive = false;
	}
}
/************************************************************************/ 
const Ogre::String& SoundSource::getMovableType(void) const
{
	return SoundSourceFactory::FACTORY_TYPE_NAME;
}
/************************************************************************/ 
const Ogre::AxisAlignedBox& SoundSource::getBoundingBox(void) const
{
	// Null, Sounds are not visible
	static Ogre::AxisAlignedBox box;
	return box;
}
/************************************************************************/ 
void SoundSource::_updateRenderQueue(Ogre::RenderQueue* queue)
{
	// Sounds are not visible so do nothing
}
/************************************************************************/ 
void SoundSource::_notifyAttached(Ogre::Node *parent, bool isTagPoint)
{
	// Set the source not relative to the listener if it's attached to a node
	if(mSourceRelative)
	{
		mSourceRelative = false;
		if(mActive)
			alSourcei(mSource, AL_SOURCE_RELATIVE, AL_FALSE);
		SoundSystem::checkError(__FUNCTION__);
	}
	mParentNode = parent;
	_update();
}
/************************************************************************/ 
bool SoundSource::attachBuffer(const Ogre::String& bufferName, bool streaming)
{
	bool replay = false;
	if (mState == AL_PLAYING)
	{
		replay = true;
		stop();
	}

	if(streaming)
		mBuffer = SoundSystem::getSingleton().createStreamingBuffer(this, bufferName);
	else
		mBuffer = SoundSystem::getSingleton().getBuffer(bufferName);

	if(replay)
		play();

	return true;
}
/************************************************************************/ 
void SoundSource::activate(bool activate)
{
	if (activate == mActive)
		return;

	if (activate)
	{
		float position = mTimer->getPosition();
		while(position > mBuffer->getDuration())
			position -= mBuffer->getDuration();

		initSource();
		if (mState == AL_PLAYING)
			play();
		seek(position);
	}
	else
	{
		//mSecOffset = mTimer.getPosition();
		deinitSource();
	}
}
/************************************************************************/ 
Ogre::Real SoundSource::getDistanceToListener() const
{
	if (mSourceRelative)
		return  mDerivedPosition.length();
	else
		return (mDerivedPosition - SoundListener::getSingleton().getDerivedPosition()).length();
}
/************************************************************************/ 
void SoundSource::updateDistanceAttenuation()
{
	if(!mActive)
		return;

	Ogre::Real mDistance = getDistanceToListener();
	Ogre::Real critDist;

	switch(alGetInteger(AL_DISTANCE_MODEL))
	{
	case AL_NONE:
		mDistanceAttenuation = 1.0f;
		break;


	case AL_INVERSE_DISTANCE:
		if ((critDist = mReferenceDistance + (mRolloffFactor * (mDistance - mReferenceDistance))) <= 0.0)
			mDistanceAttenuation = 1.0;
		else
			mDistanceAttenuation =  mReferenceDistance / critDist;
		break;


	case AL_INVERSE_DISTANCE_CLAMPED:
		if (mMaxDistance <= mReferenceDistance)
			mDistanceAttenuation = 1.0;
		else if ((critDist = mReferenceDistance + (mRolloffFactor * (std::min(std::max(mDistance, mReferenceDistance), mMaxDistance) - mReferenceDistance))) <= 0.0)
			mDistanceAttenuation = 1.0;
		else
			mDistanceAttenuation = mReferenceDistance / critDist;
		break;


	case AL_LINEAR_DISTANCE:
		if (mMaxDistance <= mReferenceDistance)
			mDistanceAttenuation = 1.0;
		else
			mDistanceAttenuation = 1.0 - mRolloffFactor * (mDistance - mReferenceDistance) / (mMaxDistance - mReferenceDistance);
		break;


	case AL_LINEAR_DISTANCE_CLAMPED:
		if (mMaxDistance <= mReferenceDistance) 
			mDistanceAttenuation = 1.0;
		else
			mDistanceAttenuation = 1.0 - mRolloffFactor * (std::min(std::max(mDistance, mReferenceDistance), mMaxDistance) - mReferenceDistance) / (mMaxDistance - mReferenceDistance);
		break;

	case AL_EXPONENT_DISTANCE:
		if (mReferenceDistance == 0.0)
			mDistanceAttenuation = 1.0;
		else if ((critDist = mDistance / mReferenceDistance) == 0)
			mDistanceAttenuation = 1.0;
		else
			mDistanceAttenuation = Ogre::Math::Pow(critDist, -mRolloffFactor);
		break;


	case AL_EXPONENT_DISTANCE_CLAMPED:
		if ((mMaxDistance <= mReferenceDistance) || (mReferenceDistance == 0.0f)) 
			mDistanceAttenuation = 1.0;
		else if ((critDist = std::min(std::max(mDistance, mReferenceDistance), mMaxDistance) / mReferenceDistance) == 0)
			mDistanceAttenuation = 1.0;
		else
			mDistanceAttenuation = Ogre::Math::Pow(critDist, -mRolloffFactor);
		break;

	}
}
/************************************************************************/ 
void SoundSource::updateEffectiveGain()
{
	mEffectiveGain = mGain * mDistanceAttenuation;
	if ( mInnerConeAngle < mOuterConeAngle)
	{
		//TODO
	}
	mEffectiveGain = std::min(std::max(mEffectiveGain, mMinGain), mMaxGain);
}
/************************************************************************/ 
SoundSourceFactory::SoundSourceFactory()
{
}
/************************************************************************/ 
SoundSourceFactory::~SoundSourceFactory()
{
	BufferMap::iterator bufferItr = mBufferMap.begin();
	while(bufferItr != mBufferMap.end())
		delete bufferItr->second;

	mBufferMap.clear();
}
/************************************************************************/ 
const Ogre::String& SoundSourceFactory::getType(void) const
{
	return FACTORY_TYPE_NAME;
}
/************************************************************************/ 
Ogre::MovableObject* SoundSourceFactory::createInstanceImpl(const Ogre::String& name, 
			const Ogre::NameValuePairList* params)
{
	return new SoundSource(name);
}
/************************************************************************/ 
void SoundSourceFactory::destroyInstance(Ogre::MovableObject* obj)
{
	SoundSystem::getSingleton().destroySource((SoundSource*)obj);
}
/************************************************************************/ 
