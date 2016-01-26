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

#ifndef _STREAMINGBUFFER_H
#define _STREAMINGBUFFER_H

#include "SoundBuffer.h"
#include "SoundSource.h"

class StreamingBuffer : public SoundBuffer
{
public:
	StreamingBuffer(const Ogre::String& name, SoundSource* source, int bufferCount = 4);
	bool play(ALuint source) override;
	bool pause(ALuint source) override;
	bool stop(ALuint source) override;
	void update() override;
	void setLoop(bool loop) override
	{ mLoop = loop; }
	virtual void seek() override;
protected:
	virtual bool stream();
	int mBufferCount;
	bool mLoop;
	SoundSource* mSource;
	bool mStopRequested;

};

class OgSream : public StreamingBuffer
{
public:
	OgSream(const Ogre::String& name, SoundSource* source, int bufferCount);
	~OgSream();
	void seek() override;
protected:
	bool stream() override;

    OggVorbis_File  mOgGAMESTATEream;
    vorbis_comment* mVorbisComment;
	vorbis_info* mVorbisInfo;
	ALenum mFormat;
	Ogre::DataStreamPtr mStream;
};

#endif