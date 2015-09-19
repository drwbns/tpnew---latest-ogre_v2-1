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

#include "OGRE\OgrePrerequisites.h"
#include "OGRE\OgreMemoryAllocatorConfig.h"

class Agent;
class ProjectileManager;

class Projectile : public Ogre::GeneralAllocatedObject
{
	friend class ProjectileManager;

protected:
	Projectile();
	virtual ~Projectile();
	virtual void Update() = 0;
	
	void SetOwner(Agent* a) { Owner = a; }
	void SetPosition(Ogre::Vector3 *v) { Position = v; }
	void SetDirection(Ogre::Vector3 *v) { Direction = v; }
	void SetSpeed(float f) { Speed = f; }
	void SetHitPoint(float f) { HitPoint = f; }

protected:
	Agent* Owner;
	Ogre::Vector3 *Start;
	Ogre::Vector3 *Position;
	Ogre::Vector3 *Direction;
	float Speed;
	float HitPoint;
	bool Alive;
};

#endif