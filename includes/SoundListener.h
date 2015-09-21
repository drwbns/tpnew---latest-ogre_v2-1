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

#ifndef _SOUNDLISTENER_H
#define _SOUNDLISTENER_H

#include "OgrePrerequisites.h"

#include "OgreMovableObject.h"
#include "OgreSingleton.h"

#include "al.h"

class SoundListener : public Ogre::MovableObject, protected Ogre::Singleton<SoundListener>
{
protected:
	SoundListener();
	SoundListener(const Ogre::String& name);

public:
	virtual ~SoundListener();
	static SoundListener& getSingleton();
	static SoundListener* getSingletonPtr();

	void setGain(Ogre::Real gain);
	Ogre::Real getGain() const {return mGain;}

	void setPosition(Ogre::Real x, Ogre::Real y, Ogre::Real z);
	void setPosition(const Ogre::Vector3& vec);
	const Ogre::Vector3& getPosition() const;

	void setDirection(Ogre::Real x, Ogre::Real y, Ogre::Real z);
	void setDirection(const Ogre::Vector3& vec);
	const Ogre::Vector3& getDirection() const;

	void setVelocity(Ogre::Real x, Ogre::Real y, Ogre::Real z);
	void setVelocity(const Ogre::Vector3& vec);
	const Ogre::Vector3& getVelocity() const;

	const Ogre::Vector3& getDerivedPosition() const;
	const Ogre::Vector3& getDerivedDirection() const;

	const Ogre::String& getMovableType() const;
	const Ogre::AxisAlignedBox& getBoundingBox() const;

	Ogre::Real getBoundingRadius() const {return 0; /* Not Visible */} 
	void _updateRenderQueue(Ogre::RenderQueue* queue);
	void _notifyAttached(Ogre::Node* parent, bool isTagPoint = false);
	void visitRenderables(Ogre::Renderable::Visitor* visitor, bool debugRenderables = false) { }

	void updateListener();

protected:
	virtual void update() const;
	void initListener();

	mutable Ogre::Vector3 mDerivedPosition;
	mutable Ogre::Vector3 mDerivedDirection;
	mutable Ogre::Quaternion mLastParentOrientation;
	mutable Ogre::Vector3 mLastParentPosition;

	Ogre::Real mGain;
	Ogre::Vector3 mPosition;
	Ogre::Vector3 mDirection;
	Ogre::Vector3 mVelocity;
	Ogre::Vector3 mUp;
	ALfloat mOrientation[6];

	mutable bool mLocalTransformDirty;

	friend class SoundListenerFactory;
};

class SoundListenerFactory : public Ogre::MovableObjectFactory
{
public:
	SoundListenerFactory() {}
	~SoundListenerFactory() {}

	static Ogre::String FACTORY_TYPE_NAME;

	const Ogre::String& getType() const;
	void destroyInstance(Ogre::MovableObject* obj);

protected:
	Ogre::MovableObject* createInstanceImpl(const Ogre::String& name, 
		const Ogre::NameValuePairList* params = 0);
};

#endif