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
#include "SoundListener.h"

#include "OgreNode.h"

/************************************************************************/

template<> SoundListener* Ogre::Singleton<SoundListener>::msSingleton = nullptr;
Ogre::String SoundListenerFactory::FACTORY_TYPE_NAME = "SoundListener";
/************************************************************************/
SoundListener::SoundListener() :
	mDerivedPosition(Ogre::Vector3::ZERO),
	mDerivedDirection(Ogre::Vector3::NEGATIVE_UNIT_Z),
	mGain(1.0),
	mPosition(Ogre::Vector3::ZERO),
	mDirection(Ogre::Vector3::NEGATIVE_UNIT_Z),
	mVelocity(Ogre::Vector3::ZERO),
	mUp(Ogre::Vector3::UNIT_Y)
{
	mParentNode = nullptr;
	mLocalTransformDirty = false;
	initListener();
}
/************************************************************************/
SoundListener::SoundListener(const Ogre::String& name) :
	Ogre::MovableObject(name),
	mDerivedPosition(Ogre::Vector3::ZERO),
	mDerivedDirection(Ogre::Vector3::NEGATIVE_UNIT_Z),
	mGain(1.0),
	mPosition(Ogre::Vector3::ZERO),
	mDirection(Ogre::Vector3::NEGATIVE_UNIT_Z),
	mVelocity(Ogre::Vector3::ZERO),
	mUp(Ogre::Vector3::UNIT_Y)
{
	mParentNode = nullptr;
	initListener();
}
/************************************************************************/
SoundListener::~SoundListener()
{
}
/************************************************************************/
SoundListener* SoundListener::getSingletonPtr(void)
{
	return msSingleton;
}
/************************************************************************/
SoundListener& SoundListener::getSingleton(void)
{
	assert(msSingleton);  return (*msSingleton);
}
/************************************************************************/
void SoundListener::setGain(Ogre::Real gain)
{
	mGain = gain;
	alListenerf(AL_GAIN, mGain);
}
/************************************************************************/
void SoundListener::setPosition(Ogre::Real x, Ogre::Real y, Ogre::Real z)
{
	mPosition.x = x;
	mPosition.y = y;
	mPosition.z = z;
	mLocalTransformDirty = true;
}
/************************************************************************/
void SoundListener::setPosition(const Ogre::Vector3& vec)
{
	mPosition = vec;
	mLocalTransformDirty = true;
}
/************************************************************************/
const Ogre::Vector3& SoundListener::getPosition() const
{
	return mPosition;
}
/************************************************************************/
void SoundListener::setDirection(Ogre::Real x, Ogre::Real y, Ogre::Real z)
{
	mDirection.x = x;
	mDirection.y = y;
	mDirection.z = z;
	mLocalTransformDirty = true;
}
/************************************************************************/
void SoundListener::setDirection(const Ogre::Vector3& vec)
{
	mDirection = vec;
	mLocalTransformDirty = true;
}
/************************************************************************/
const Ogre::Vector3& SoundListener::getDirection() const
{
	return mDirection;
}
/************************************************************************/
void SoundListener::setVelocity(Ogre::Real x, Ogre::Real y, Ogre::Real z)
{
	mVelocity.x = x;
	mVelocity.y = y;
	mVelocity.z = z;
	alListener3f(AL_VELOCITY, mVelocity.x, mVelocity.y, mVelocity.z);
}
/************************************************************************/
void SoundListener::setVelocity(const Ogre::Vector3& vec)
{
	mVelocity = vec;
	alListener3f(AL_VELOCITY, mVelocity.x, mVelocity.y, mVelocity.z);
}
/************************************************************************/
const Ogre::Vector3& SoundListener::getVelocity() const
{
	return mVelocity;
}
/************************************************************************/
const Ogre::Vector3& SoundListener::getDerivedPosition(void) const
{
	update();
	return mDerivedPosition;
}
/************************************************************************/
const Ogre::Vector3& SoundListener::getDerivedDirection(void) const
{
	update();
	return mDerivedPosition;
}
/************************************************************************/
void SoundListener::initListener()
{
	mOrientation[0] = mDirection.x; // Forward.x
	mOrientation[1] = mDirection.y; // Forward.y
	mOrientation[2] = mDirection.z; // Forward.z

	mOrientation[3] = mUp.x; // Up.x
	mOrientation[4] = mUp.y; // Up.y
	mOrientation[5] = mUp.z; // Up.z

	alListener3f(AL_POSITION, mPosition.x, mPosition.y, mPosition.z);
	alListenerfv(AL_ORIENTATION, mOrientation);
	alListenerf(AL_GAIN, 1.0f);
	alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);
}
/************************************************************************/
void SoundListener::update() const
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
void SoundListener::updateListener()
{
	update();
	if (mParentNode)
	{
		mPosition = mLastParentPosition;
		mDirection = mLastParentOrientation.zAxis();
		mUp = mLastParentOrientation.yAxis();
	}
	alListener3f(AL_POSITION, mPosition.x, mPosition.y, mPosition.z);
	mOrientation[0] = -mDirection.x; // Forward.x
	mOrientation[1] = -mDirection.y; // Forward.y
	mOrientation[2] = -mDirection.z; // Forward.z

	mOrientation[3] = mUp.x; // Up.x
	mOrientation[4] = mUp.y; // Up.y
	mOrientation[5] = mUp.z; // Up.z
	alListenerfv(AL_ORIENTATION, mOrientation);
}
/************************************************************************/
const Ogre::String& SoundListener::getMovableType() const
{
	return SoundListenerFactory::FACTORY_TYPE_NAME;
}
/************************************************************************/
const Ogre::AxisAlignedBox& SoundListener::getBoundingBox() const
{
	// Null, Sounds are not visible
	static Ogre::AxisAlignedBox box;
	return box;
}
/************************************************************************/
void SoundListener::_updateRenderQueue()
{
	// Do Nothing
}
/************************************************************************/
void SoundListener::_notifyAttached(Ogre::Node* parent)
{
	mParentNode = parent;
}
/************************************************************************/
const Ogre::String& SoundListenerFactory::getType(void) const
{
	return FACTORY_TYPE_NAME;
}
/************************************************************************/
Ogre::MovableObject* SoundListenerFactory::createInstanceImpl(const Ogre::String& name)
{
	SoundListener *listener = SoundListener::getSingletonPtr();
	if (listener)
		return listener;
	return new SoundListener(name);
}
/************************************************************************/
void SoundListenerFactory::destroyInstance(Ogre::MovableObject* obj)
{
	delete obj;
}
/************************************************************************/