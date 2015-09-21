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
#include <OgreException.h>
#include "SoundBuffer.h"
#include "SoundSystem.h"

//----------------------------------------------------------------------------//
SoundBuffer::SoundBuffer(const Ogre::String& name)
{
	mBuffers = new ALuint[1];
	alGenBuffers(1, mBuffers);
}
//----------------------------------------------------------------------------//
SoundBuffer::~SoundBuffer()
{
	alDeleteBuffers(1, mBuffers);
	delete [] mBuffers;
}
//----------------------------------------------------------------------------//
bool SoundBuffer::play(ALuint source)
{
	ALint buffer;
	alGetSourcei(source, AL_BUFFER, &buffer);
	if ( buffer != mBuffers[0])
		alSourcei(source, AL_BUFFER, mBuffers[0]);

	alSourcePlay(source);
	SoundSystem::checkError(__FUNCTION__);
	return true;
}
//----------------------------------------------------------------------------//
bool SoundBuffer::pause(ALuint source)
{
	alSourcePause(source);
	SoundSystem::checkError(__FUNCTION__);
	return true;
}
//----------------------------------------------------------------------------//
bool SoundBuffer::stop(ALuint source)
{
	alSourceStop(source);
	SoundSystem::checkError(__FUNCTION__);
	return true;
}
//----------------------------------------------------------------------------//
void SoundBuffer::update(ALuint source, Ogre::Real elapsedTime)
{
}
//----------------------------------------------------------------------------//
void SoundBuffer::setLoop(ALuint source, bool loop)
{
	alSourcei(source, AL_LOOPING, loop);
	SoundSystem::checkError(__FUNCTION__);
}
//----------------------------------------------------------------------------//
void SoundBuffer::updateInfo()
{
	alGetBufferi(mBuffers[0], AL_FREQUENCY, &mFrequency);
	alGetBufferi(mBuffers[0], AL_BITS, &mBits);
	alGetBufferi(mBuffers[0], AL_SIZE, &mSize);
	alGetBufferi(mBuffers[0], AL_CHANNELS, &mChannels);
	SoundSystem::checkError(__FUNCTION__);

	mDuration = 8.0f * (float)mSize / (float)( mBits * mFrequency * mChannels);
}

//----------------------------------------------------------------------------//
void SoundBuffer::seek(ALuint source, float position)
{
	alSourcef(source, AL_SEC_OFFSET, position);
	SoundSystem::checkError(__FUNCTION__);
}
//----------------------------------------------------------------------------//
WavBuffer::WavBuffer(const Ogre::String &name)
{
	mName = name;
	mBuffers = new ALuint[1];

	Ogre::DataStreamPtr dstream = Ogre::ResourceGroupManager::getSingleton().openResource(name);
	Ogre::MemoryDataStream stream(dstream);

//	mBuffers[0] = alutCreateBufferFromFileImage((ALbyte *)stream.getPtr(), static_cast<ALsizei>(stream.size()));
	SoundSystem::checkError(__FUNCTION__);
	updateInfo();
}
//----------------------------------------------------------------------------//
OggBuffer::OggBuffer(const Ogre::String &name) 
	: SoundBuffer(name)
{
	int currSection;
	long size = 0;
	char data[SOUND_BUFFER_SIZE];
	std::vector<char> buffer;

    OggVorbis_File  mOgGAMESTATEream;
	ov_callbacks vorbisCallbacks;
	vorbis_info* mVorbisInfo;
    vorbis_comment* mVorbisComment;
	ALenum mFormat;

	// read file to memory
	Ogre::DataStreamPtr mStream = Ogre::ResourceGroupManager::getSingleton().openResource(name);

	// open ogg stream
	vorbisCallbacks.read_func = vorbisRead;
	vorbisCallbacks.close_func = vorbisClose;
	vorbisCallbacks.seek_func = vorbisSeek;
	vorbisCallbacks.tell_func = vorbisTell;
	if (ov_open_callbacks(&mStream, &mOgGAMESTATEream, NULL, 0, vorbisCallbacks) < 0)
	{
		throw Ogre::Exception(1, "Could not open Ogg stream.",__FUNCTION__);
	}

	mVorbisInfo = ov_info(&mOgGAMESTATEream, -1);
	mVorbisComment = ov_comment(&mOgGAMESTATEream, -1);

	if(mVorbisInfo->channels == 1)
		mFormat = AL_FORMAT_MONO16;
	else
		mFormat = AL_FORMAT_STEREO16;

	do
	{
		size = ov_read(&mOgGAMESTATEream, data, sizeof(data), 0, 2, 1, &currSection);
		buffer.insert(buffer.end(), data, data + size);
	}while(size > 0);

	alBufferData(mBuffers[0], mFormat, &buffer[0], static_cast<ALsizei>(buffer.size()), mVorbisInfo->rate);
	SoundSystem::checkError(__FUNCTION__);

	ov_clear(&mOgGAMESTATEream);
	updateInfo();
}
//----------------------------------------------------------------------------//
size_t OggBuffer::vorbisRead(void *ptr, size_t byteSize, size_t sizeToRead, void *datasource )
{
	size_t				spaceToEOF;			// How much more we can read till we hit the EOF marker
	size_t				actualSizeToRead;	// How much data we are actually going to read from memory
	Ogre::DataStreamPtr	vorbisData;			// Our vorbis data, for the typecast

	// Get the data in the right format
	vorbisData = *(Ogre::DataStreamPtr*)datasource;

	// Calculate how much we need to read.  This can be sizeToRead*byteSize or less depending on how near the EOF marker we are
	spaceToEOF = vorbisData->size() - vorbisData->tell();
	if ((sizeToRead * byteSize) < spaceToEOF)
		actualSizeToRead = (sizeToRead * byteSize);
	else
		actualSizeToRead = spaceToEOF;	
	
	// A simple copy of the data from memory to the datastruct that the vorbis libs will use
	if (actualSizeToRead)
	{
		// Copy the data from the start of the file PLUS how much we have already read in
		vorbisData->read(ptr, actualSizeToRead);
	}

	// Return how much we read (in the same way fread would)
	return actualSizeToRead;
}

//----------------------------------------------------------------------------//
int OggBuffer::vorbisSeek(void *datasource, ogg_int64_t offset, int whence)
{
	size_t				spaceToEOF;
	ogg_int64_t			actualOffset;
	Ogre::DataStreamPtr	vorbisData;

	vorbisData = *(Ogre::DataStreamPtr*)datasource;

	switch (whence)
	{
	case SEEK_SET:
		if (vorbisData->size() >= offset)
			actualOffset = offset;
		else
			actualOffset = vorbisData->size();
		vorbisData->seek((int)actualOffset);
		break;
	case SEEK_CUR:
		spaceToEOF = vorbisData->size() - vorbisData->tell();
		if (offset < spaceToEOF)
			actualOffset = (offset);
		else
			actualOffset = spaceToEOF;	
		vorbisData->seek( static_cast<size_t>(vorbisData->tell() + actualOffset));
		break;
	case SEEK_END:
		vorbisData->seek(vorbisData->size());
		break;
	default:
		SoundSystem::getSingleton().logMessage("*** ERROR *** Unknown seek command in VorbisSeek");
		break;
	};

	return 0;
}

//----------------------------------------------------------------------------//
int OggBuffer::vorbisClose(void *datasource)
{
	return 1;
}
//----------------------------------------------------------------------------//
long OggBuffer::vorbisTell(void *datasource)
{
	Ogre::DataStreamPtr vorbisData = *(Ogre::DataStreamPtr*)datasource;
	return static_cast<long>(vorbisData->tell());
}
//----------------------------------------------------------------------------//
