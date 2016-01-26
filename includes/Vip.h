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

#ifndef Vip_H_
#define Vip_H_

#include "OGRE\OgrePrerequisites.h"
#include "physxPrereqs.h"
#include "prereqs.h"

#include "Agent.h"


class Vip : public Agent
{
	friend class World;

private:
	Vip(int id, Race race, Ogre::Vector3 position);
	~Vip();
	void Update() override;

public:
	void orderMove(float walk, float strafe) override;
	void orderBrake() override;
	void orderArrive(Ogre::Vector3 pos) override;
	void orderPathFollow() override;
	void Shoot() override;
	void Die() override;

	//get / set
	void SetAimMode(bool b)  override { aimMode = b; }
	Ogre::Vector3 *GetFirePosition() override;
	Ogre::Vector3 GetFireDirection(Ogre::Vector3 &trg_pos);
	Ogre::Vector3 GetHeadPosition() const;
	Ogre::Quaternion GetHeadRotation() const;
	Ogre::Vector3 GetBodyPosition() const;
	Ogre::Quaternion GetBodyRotation() const;
	PxShape* getHitBox(int i)  override { return hitboxes[i]; }

protected:


	//phy
	std::vector<PxShape*> hitboxes;
	PxCapsuleController* phycontrol;
	bool flying;
};

#endif