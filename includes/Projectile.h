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

#ifndef Projectile_H_
#define Projectile_H_

#include "prereqs.h"

#include "OGRE\OgreMemoryAllocatorConfig.h"
#include "OgreVector3.h"

class Projectile : public Ogre::GeneralAllocatedObject
{
	friend class ProjectileManager;

protected:
	Projectile();
	virtual ~Projectile();
	virtual void Update() = 0;
	
	void SetOwner(Agent* a) { mOwner = a; }
	void SetPosition(Ogre::Vector3 &v) { mPosition = v; }
	void SetDirection(Ogre::Vector3 &v) { mDirection = v; }
	void SetSpeed(float f) { mSpeed = f; }
	void SetHitPoint(float f) { mHitPoint = f; }

	Agent* mOwner;
	Ogre::Vector3 mStart;
	Ogre::Vector3 mPosition;
	Ogre::Vector3 mDirection;
	float mSpeed;
	float mHitPoint;
	bool mAlive;
};

#endif