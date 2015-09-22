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

#ifndef _SOUNDSYSYEM_H
#define _SOUNDSYSYEM_H

#include <string>
#include <map>
#include <vector>

#include "SoundPrereqs.h"
#include "SoundSource.h"
#include "SoundListener.h"
#include "SoundDevices.h"

#include <OgreIteratorWrappers.h>
#include <OgreSingleton.h>
#include "OgreMemoryAllocatorConfig.h"




#define SOUND_BUFFER_SIZE (4096 * 8)

enum AudioFormat
{
	DEFAULT,
	MONO_CHANNEL,
	STEREO_CHANNEL,
	QUAD_CHANNEL,
	MULTI_CHANNEL_51,
	MULTI_CHANNEL_61,
	MULTI_CHANNEL_71
};

class FormatData : public Ogre::GeneralAllocatedObject
{
public:
	FormatData(ALenum alEnum, ALchar* alChar, const Ogre::String& description)
	{
		formatEnum = alEnum;
		formatName = alChar;
		formatDescription = description;
	}
	ALenum formatEnum;
	ALchar* formatName;
	Ogre::String formatDescription;
};

typedef std::map<Ogre::String, SoundSource*> SourceMap;
typedef std::vector<SoundSource*> SourceVector;
typedef std::map<AudioFormat, FormatData*> FormatMap;
typedef Ogre::MapIterator<FormatMap> FormatMapIterator;

class SoundSystem : public Ogre::GeneralAllocatedObject, public Ogre::Singleton<SoundSystem>
{
public:
	enum DistanceModel
	{
		DMInverseDistance,
		DMInverseDistanceClamped,
		DMLinearDistance, 
		DMLinearDistanceClamped, 
		DMExponentDistance, 
		DMExponentDistanceClamped,
		DMNone
	};
	SoundSystem();
	~SoundSystem() {};

	static SoundSystem& getSingleton(void);
    static SoundSystem* getSingletonPtr(void);

	bool initialize();
	bool update(float elapsedTime = 0);
	void finalize();

	void setDopplerFactor(Ogre::Real dopplerFactor);
	Ogre::Real getDopplerFactor() const {return mDopplerFactor;}

	void setDistanceModel(DistanceModel model);
	DistanceModel getDistanceModel() const;

	void setSpeedOfSound(Ogre::Real speedOfSound);
	Ogre::Real getSpeedOfSound() const {return mSpeedOfSound;}

	FormatMapIterator getSupportedFormatIterator();
	const FormatData* retrieveFormatData(AudioFormat format);

	int eaxSupport() const {return mEAXVersion;}
	bool xRamSupport() const {return mXRAMSupport;}
	ALboolean eaxSetBufferMode(ALsizei numBuffers, ALuint *buffers, ALuint bufferMode);
	ALenum eaxGetBufferMode(ALuint buffer, ALint *reserved = 0);

	SoundBuffer* createBuffer(const Ogre::String& fileName);
	SoundBuffer* createStreamingBuffer(SoundSource* mSource, const Ogre::String& fileName);
	SoundBuffer* getBuffer(const Ogre::String& fileName);
	void destroyAllBuffers();

	SoundSource* createSource(const Ogre::String& name, const Ogre::String& buffer, bool streaming = false);
	SoundSource* getSource(const Ogre::String& name);
	bool hasSource(const Ogre::String& name) const;
	void destroySource(const Ogre::String& name);
	void destroySource(SoundSource* sound);
	void destroyAllSources();
	void pauseAllSounds();
	void resumeAllSounds(); 
	void restoreContext();

	static Ogre::String errorToString(int code);
	static void checkError(const Ogre::String& reference);

	size_t getPlayingSourceCount() { return mPlayingSources.size(); }
	size_t getSourceCount() { return mSourceMap.size(); }

	void logMessage(const Ogre::String& msg);

protected:
	typedef std::map<std::string, SoundBuffer*> BufferMap;
	typedef std::vector< SoundBuffer*> BufferVector;
	typedef std::map<std::string, SoundSource*> SourceMap;
	struct UpdateSource;
	struct SortSources;

	SoundListener* createListener();
	
	SoundSourceFactory* mSourceFactory;
	SoundListenerFactory* mListenerFactory;
	SoundDevices* mDevices;

	bool mEAXSupport;
	int mEAXVersion;
	bool mXRAMSupport;
	ALsizei mXRamSize;
	ALsizei mXRamFree;

	Ogre::Real mDopplerFactor;
	Ogre::Real mSpeedOfSound;

	SourceMap mSourceMap; 
	BufferMap mBufferMap;
	BufferVector mStreamingBuffers;
	FormatMap mSupportedFormats;
	SourceVector mPlayingSources;
	SourceVector mPausedSources;


	int mSelectedDevice;
	ALCcontext *mContext;
};

#endif