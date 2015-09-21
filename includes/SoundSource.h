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

#ifndef _SOUNDSOURCE_H
#define _SOUNDSOURCE_H

class SoundBuffer;
class SoundTimer;

#include "OgreMovableObject.h"
#include "OgrePrerequisites.h"

#include "al.h"

class SoundSource : public Ogre::MovableObject
{
public:
	SoundSource(const Ogre::String& name);
	~SoundSource();

	bool isPlaying() const;
	bool play();

	bool isPaused() const;
	bool pause();

	bool isStopped() const;
	bool stop();

	bool isInitial() const;
	bool seek(Ogre::Real position);

	Ogre::Real getPitch() const {return mPitch;}
	void setPitch(Ogre::Real pitch);

	Ogre::Real getGain() const {return mGain;}
	void setGain(Ogre::Real gain);

	Ogre::Real getMaxGain() const {return mMaxGain;}
	void setMaxGain(Ogre::Real maxGain);

	Ogre::Real getMinGain() const {return mMinGain;}
	void setMinGain(Ogre::Real minGain);

	void setGainValues(Ogre::Real maxGain, Ogre::Real minGain, Ogre::Real gain);

	Ogre::Real getMaxDistance() const {return mMaxDistance;}
	void setMaxDistance(Ogre::Real maxDistance);

	Ogre::Real getRolloffFactor() const {return mRolloffFactor;}
	void setRolloffFactor(Ogre::Real rolloffFactor);

	Ogre::Real getReferenceDistance() const {return mReferenceDistance;}
	void setReferenceDistance(Ogre::Real refDistance);

	void setDistanceValues(Ogre::Real maxDistance, Ogre::Real rolloffFactor, Ogre::Real refDistance);

	const Ogre::Vector3& getVelocity() const {return mVelocity;}
	void setVelocity(Ogre::Real x, Ogre::Real y, Ogre::Real z);
	void setVelocity(const Ogre::Vector3& vec);

	bool isRelativeToListener() const {return mSourceRelative==AL_TRUE?true:false;}
	void setRelativeToListener(bool relative);

	const Ogre::Vector3& getPosition() const;
	void setPosition(Ogre::Real x, Ogre::Real y, Ogre::Real z);
	void setPosition(const Ogre::Vector3& vec);

	const Ogre::Vector3& getDirection() const;
	void setDirection(Ogre::Real x, Ogre::Real y, Ogre::Real z);
	void setDirection(const Ogre::Vector3& vec);

	Ogre::Real getOuterConeGain() const {return mOuterConeGain;}
	void setOuterConeGain(Ogre::Real outerConeGain);

	Ogre::Real getInnerConeAngle() const {return mInnerConeAngle;}
	void setInnnerConeAngle(Ogre::Real innerConeAngle);

	Ogre::Real getOuterConeAngle() {return mOuterConeAngle;}
	void setOuterConeAngle(Ogre::Real outerConeAngle);

	bool isLooping() const {return mLoop==AL_TRUE?true:false;}
	void setLoop(bool loop);

	int getPriority() const  { return mPriority; }
	void setPriority(int priority) { mPriority = priority; }

	const Ogre::Vector3& getDerivedPosition() const;
	const Ogre::Vector3& getDerivedDirection() const;

	Ogre::Real getEffectiveGain() { return mEffectiveGain; }
	static size_t getActiveCount() { return mActiveCount; }
	bool isActive() { return mActive; }
	void activate(bool activate);

	bool attachBuffer(const Ogre::String& bufferName, bool streaming = false);
	bool _updateSound(Ogre::Real ElapsedTime);
	Ogre::Real getDistanceToListener() const;
	const SoundTimer* getTimer() { return mTimer; }

	/** Overridden from MovableObject */
	const Ogre::String& getMovableType() const;
	const Ogre::AxisAlignedBox& getBoundingBox() const;
	Ogre::Real getBoundingRadius() const {return 0; /* Not Visible */} 
	void _updateRenderQueue(Ogre::RenderQueue* queue);
	void _notifyAttached(Ogre::Node *parent, bool isTagPoint = false);
	void visitRenderables(Ogre::Renderable::Visitor* visitor, bool debugRenderables = false) { }
protected:
	void initSource();
	void deinitSource();
	void _update() const;


	void updateDistanceAttenuation();
	void updateEffectiveGain();

	mutable Ogre::Vector3 mDerivedPosition;
	mutable Ogre::Vector3 mDerivedDirection;
	mutable Ogre::Quaternion mLastParentOrientation;
	mutable Ogre::Vector3 mLastParentPosition;
	mutable bool mLocalTransformDirty;

	Ogre::Real mPitch;
	Ogre::Real mGain;
	Ogre::Real mMaxGain;
	Ogre::Real mMinGain;
	Ogre::Real mMaxDistance;
	Ogre::Real mRolloffFactor;
	Ogre::Real mReferenceDistance;
	Ogre::Real mOuterConeGain;
	Ogre::Real mInnerConeAngle;
	Ogre::Real mOuterConeAngle;
	Ogre::Vector3 mPosition;
	Ogre::Vector3 mVelocity;
	Ogre::Vector3 mDirection;

	Ogre::Real mDistanceAttenuation;
	Ogre::Real mEffectiveGain;

	ALboolean mSourceRelative;
	ALboolean mLoop;
	ALuint mSource;

	bool mActive;
	static size_t mActiveCount;
	int mPriority;

	mutable ALint mState;
	SoundBuffer* mBuffer;
	SoundTimer* mTimer;

};
class SoundSourceFactory : public Ogre::MovableObjectFactory
{
public:
	SoundSourceFactory();
	~SoundSourceFactory();

	static Ogre::String FACTORY_TYPE_NAME;

	const Ogre::String& getType() const;
	void destroyInstance(Ogre::MovableObject* obj);

protected:
	typedef std::map<std::string, SoundBuffer*> BufferMap;
	BufferMap mBufferMap;

	Ogre::MovableObject* createInstanceImpl(const Ogre::String& name, const Ogre::NameValuePairList* params = 0);
};

#endif