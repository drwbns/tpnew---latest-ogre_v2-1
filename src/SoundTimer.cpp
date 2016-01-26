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


#include "SoundTimer.h"
#include <OGRE/OgrePrerequisites.h>

//----------------------------------------------------------------------------//
SoundTimer::SoundTimer()
	: mState(AL_INITIAL), mSecOffset(0), mPitch(1.0)
{
}
//----------------------------------------------------------------------------//
void SoundTimer::play()
{
	switch (mState)
	{
	case AL_INITIAL:
	case AL_PLAYING:
	case AL_STOPPED:
		mSecOffset = 0;
		mTimer.reset();
		break;

	case AL_PAUSED:
		mTimer.reset();
		break;
	}

	mState = AL_PLAYING;
}
//----------------------------------------------------------------------------//
void SoundTimer::pause()
{
	switch (mState)
	{
	case AL_PLAYING:
		mSecOffset += mPitch * static_cast<float>(mTimer.getMilliseconds()) / 1000.0f;
		break;

	case AL_STOPPED:
	case AL_PAUSED:
	case AL_INITIAL:
		//NOOP;
		break;
	}

	mState = AL_PAUSED;
}
//----------------------------------------------------------------------------//
void SoundTimer::stop()
{
	switch (mState)
	{
	case AL_PLAYING:
	case AL_PAUSED:
		mSecOffset = 0;
		break;


	case AL_STOPPED:
	case AL_INITIAL:
		//NOOP;
		break;
	}
	mState = AL_STOPPED;
}
//----------------------------------------------------------------------------//
Ogre::Real SoundTimer::getPosition() const
{
	switch(mState)
	{
	case AL_PLAYING:
		return mSecOffset + mPitch * static_cast<Ogre::Real>(mTimer.getMilliseconds()) /  1000.0;
	case AL_PAUSED:
		return mSecOffset;
	case AL_INITIAL:
	case AL_STOPPED:
		return 0;
	default:
		return 0;
	}
}
//----------------------------------------------------------------------------//
void SoundTimer::seek(Ogre::Real position)
{
	mSecOffset = position;
	mTimer.reset();
}
//----------------------------------------------------------------------------//
void SoundTimer::setPitch(Ogre::Real pitch)
{
	if(mState == AL_PLAYING)
	{
		pause();
		mPitch = pitch;
		play();
	}
	else
		mPitch = pitch;
}
//----------------------------------------------------------------------------//