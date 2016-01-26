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

#ifndef _SOUNDBUFFER_H
#define _SOUNDBUFFER_H

#include <OgrePrerequisites.h>

class SoundBuffer : public Ogre::GeneralAllocatedObject
{
public:
	SoundBuffer(const Ogre::String& name);
	virtual ~SoundBuffer();
	virtual bool play(ALuint source);
	virtual bool pause(ALuint source);
	virtual bool stop(ALuint source);
	virtual void update();
	virtual void seek();
	virtual void setLoop(bool loop);

	const Ogre::String& getName() const
	{ return mName; }
	Ogre::Real getDuration() const { return mDuration; }
	int getChannels() const { return mChannels; }
	int getFrequency() const { return mFrequency; }
	int getSize() const { return mSize; }
	int getBitDepth() const { return mBits; }


protected:
	void updateInfo();
	SoundBuffer(): mBuffers(nullptr), mDuration(0), mChannels(0), mFrequency(0), mSize(0), mBits(0)
	{ }
	Ogre::String mName;
	ALuint* mBuffers;

	Ogre::Real mDuration; 
	int mChannels; 
	int mFrequency; 
	int mSize; 
	int mBits; 
};

class WavBuffer : public SoundBuffer
{
public:
	WavBuffer(const Ogre::String& name);
};

class OggBuffer : public SoundBuffer
{
public:
	OggBuffer(const Ogre::String& name);

	static size_t vorbisRead(void *ptr, size_t byteSize, size_t sizeToRead, void *datasource );
	static int vorbisSeek(void *datasource, ogg_int64_t offset, int whence);
	static int vorbisClose(void *datasource);
	static long vorbisTell(void *datasource);
};

#endif