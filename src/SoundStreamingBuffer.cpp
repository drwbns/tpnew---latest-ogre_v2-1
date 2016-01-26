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


#include <OgreResourceGroupManager.h>
#include "SoundStreamingBuffer.h"
#include "SoundSystem.h"

//----------------------------------------------------------------------------//
StreamingBuffer::StreamingBuffer(const Ogre::String&, SoundSource* source,  int bufferCount): mLoop(false)
{
	mBufferCount = bufferCount;
	mBuffers = new ALuint[bufferCount];
	alGenBuffers(mBufferCount, mBuffers);
	mSource = source;
	mStopRequested = false;
}
//----------------------------------------------------------------------------//
bool StreamingBuffer::play(ALuint source)
{
	ALint mState;
	alGetSourcei(source, AL_SOURCE_STATE, &mState);
	switch(mState)
	{
	case AL_INITIAL:
	case AL_STOPPED:
	case AL_PLAYING:
		seek();
		break;
	case AL_PAUSED:
		seek();
		break;
	}

	alSourcePlay(source);
	SoundSystem::checkError(__FUNCTION__);
	mStopRequested = false;
	return true;
}
//----------------------------------------------------------------------------//
bool StreamingBuffer::pause(ALuint source)
{
	alSourcePause(source);
	SoundSystem::checkError(__FUNCTION__);
	return true;
}

//----------------------------------------------------------------------------//
bool StreamingBuffer::stop(ALuint source)
{
	int queued;
	ALuint mBuffer;
	ALint mState;

	alGetSourcei(source, AL_SOURCE_STATE, &mState);
	switch(mState)
	{
		case AL_PAUSED:
		case AL_PLAYING:
			alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);
			SoundSystem::checkError(__FUNCTION__);
			alSourceStop(source);
			SoundSystem::checkError(__FUNCTION__);

			while(queued--)
			{
				alSourceUnqueueBuffers(source, 1, &mBuffer);
				SoundSystem::checkError(__FUNCTION__);
			}
			break;

		case AL_STOPPED:
		case AL_INITIAL:
			//NOOP;
			break;
	}

	mStopRequested = true;
	return true;
}
//----------------------------------------------------------------------------//
void StreamingBuffer::update()
{
	ALint mState;
	ALint processed;
	bool active;

	alGetSourcei(source, AL_SOURCE_STATE, &mState);
	if (mState == AL_STOPPED && ! mStopRequested)
	{
		seek();
		alSourcePlay(source);
		SoundSystem::checkError(__FUNCTION__);
		mState= AL_PLAYING;
	}

	if(mState != AL_PLAYING)
		return;

	alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
	SoundSystem::checkError(__FUNCTION__);
 
	while(processed--)
	{
		ALuint buffer;
        
		alSourceUnqueueBuffers(source, 1, &buffer);
		SoundSystem::checkError(__FUNCTION__);
 
		active = stream();
 
		alSourceQueueBuffers(source, 1, &buffer);
		SoundSystem::checkError(__FUNCTION__);

		if(!active)
		{
			if(mLoop)
			{
				seek();
			}
			else
			{
				stop(source);
			}
		}
	}
}
//----------------------------------------------------------------------------//
bool StreamingBuffer::stream()
{
	return false;
}

bool OgSream::stream()
{
	return false;
}

//----------------------------------------------------------------------------//
void StreamingBuffer::seek()
{
}
//----------------------------------------------------------------------------//
OgSream::OgSream(const Ogre::String& name, SoundSource* source, int bufferCount) :
	StreamingBuffer(name, source, bufferCount)
{
	ov_callbacks vorbisCallbacks;

	// read file to memory
	mStream = Ogre::ResourceGroupManager::getSingleton().openResource(name);

	// open ogg stream
	vorbisCallbacks.read_func = OggBuffer::vorbisRead;
	vorbisCallbacks.close_func = OggBuffer::vorbisClose;
	vorbisCallbacks.seek_func = OggBuffer::vorbisSeek;
	vorbisCallbacks.tell_func = OggBuffer::vorbisTell;
	if (ov_open_callbacks(&mStream, &mOgGAMESTATEream, NULL, 0, vorbisCallbacks) < 0)
	{
		throw Ogre::Exception(1, "Could not open Ogg stream.",__FUNCTION__);
	}

	mVorbisInfo = ov_info(&mOgGAMESTATEream, -1);
	mVorbisComment = ov_comment(&mOgGAMESTATEream, -1);
	
	mChannels = mVorbisInfo->channels;
	mFrequency = mVorbisInfo->rate;
	mDuration = ov_time_total(&mOgGAMESTATEream, -1);
	mBits = 16;

	if(mChannels == 1)
		mFormat = AL_FORMAT_MONO16;
	else
		mFormat = AL_FORMAT_STEREO16;
	mSize = mDuration * static_cast<float>(mBits * mFrequency * mChannels) /  8.0f;
}
//----------------------------------------------------------------------------//
OgSream::~OgSream()
{
	mStream.setNull();
}
//----------------------------------------------------------------------------//
void OgSream::seek()
{
	ALint mState;
	ALuint mBuffer;
	int queued;

	alGetSourcei(source, AL_SOURCE_STATE, &mState);
	SoundSystem::checkError(__FUNCTION__);
	alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);
	SoundSystem::checkError(__FUNCTION__);
	alSourcePlay(source);
	alSourceStop(source);
	SoundSystem::checkError(__FUNCTION__);

	while(queued--)
	{
		alSourceUnqueueBuffers(source, 1, &mBuffer);
		SoundSystem::checkError(__FUNCTION__);
	}

	ov_time_seek(&mOgGAMESTATEream, position);

	for(int i = 0; i < mBufferCount; i++)
		stream();

	alSourceQueueBuffers(source, mBufferCount, mBuffers);

	switch(mState)
	{
	case AL_PLAYING:
		alSourcePlay(source);
		break;
	case AL_PAUSED:
		alSourcePlay(source);
		alSourcePause(source);
		break;
	}
}
//----------------------------------------------------------------------------//
bool OgSream::stream()
{
	char data[SOUND_BUFFER_SIZE];
	int  size = 0;
	int  section;
	int  result;
 
	while(size < SOUND_BUFFER_SIZE)
	{
		result = ov_read(&mOgGAMESTATEream, data+size, SOUND_BUFFER_SIZE - size, 0, 2, 1, &section);
		if(result > 0)
			size += result;
		else
			if(result < 0)
				throw Ogre::Exception(6, "Ogg Error: " + SoundSystem::errorToString(result), __FUNCTION__);
			else
				break;
	}
    
	SoundSystem::checkError(__FUNCTION__);

	if(size == 0)
		return false;
 
	alBufferData(buffer, mFormat, data, size, mVorbisInfo->rate);
	SoundSystem::checkError(__FUNCTION__);
 
	return true;
}
//----------------------------------------------------------------------------//