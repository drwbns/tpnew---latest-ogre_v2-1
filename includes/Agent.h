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

#ifndef Agent_H_
#define Agent_H_

#include "prereqs.h"
#include "OGRE\OgrePrerequisites.h"
#include "physxPrereqs.h"

#include "characterkinematic\PxController.h"
#include "characterkinematic\PxControllerBehavior.h"
#include "PxSimulationEventCallback.h"

#include "MovableText.h"
#include "Moving.h"

using namespace physx;

class Agent: public PxUserControllerHitReport, public PxControllerBehaviorCallback, public Moving
{
	friend class World;
public:
	enum Type
	{
		AT_VIP,
		AT_GIGN
	};

	enum Race
	{
		RT_ALLY,
		RT_ENEMY
	};

	enum
	{
		CCD_FLAG = 1 << 29,
		SNOWBALL_FLAG = 1 << 30,
		DETACHABLE_FLAG = 1 << 31
	};

protected:
	Agent(int id, Race race, Ogre::Vector3 position, float max_speed, float max_acc, float radius);
	virtual ~Agent();
	virtual void Update();

public:
	bool isDetachable(PxFilterData & filterData);
	// Implements PxSimulationEventCallback
	virtual	void							onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);
	virtual	void							onTrigger(PxTriggerPair*, PxU32) {}
	virtual void							onConstraintBreak(PxConstraintInfo*, PxU32) {}
	virtual void							onWake(PxActor**, PxU32) {}
	virtual void							onSleep(PxActor**, PxU32) {}

	///////////////////////////////////////////////////////////////////////////////

	// Implements PxUserControllerHitReport
	virtual void							onShapeHit(const PxControllerShapeHit& hit);
	virtual void							onControllerHit(const PxControllersHit& hit) {}
	virtual void							onObstacleHit(const PxControllerObstacleHit& hit) {}

	// Implements PxControllerBehaviorCallback
	virtual PxControllerBehaviorFlags		getBehaviorFlags(const PxShape&, const PxActor&) { return PxControllerBehaviorFlags(0); }
	virtual PxControllerBehaviorFlags		getBehaviorFlags(const PxController&) { return PxControllerBehaviorFlags(0); }
	virtual PxControllerBehaviorFlags		getBehaviorFlags(const PxObstacle&) { return PxControllerBehaviorFlags(0); }

	virtual void orderMove(float walk, float strafe);
	virtual void orderBrake();
	virtual void orderArrive(Ogre::Vector3 pos);
	virtual void orderGoTo(Ogre::Vector3 pos);
	virtual void orderPathFollow();
	virtual void orderNextPathNode();
	virtual void Die();

	//get / set
	virtual Ogre::Vector3 GetFirePosition() = 0;

	int getID()const { return id; }
	int getRace() { return race; }
	void ChangeController(BaseController* ct);
	BaseController* getController() { return controller; }
	void add2Hp(int i);
	int getHP() { return hp; }
	bool isDead() { return dead; }
	void ShowHeadText(bool b) { headText->setVisible(b); }
	AIKnowledge* getKnowledge() { return knowledge; }
	Ogre::Vector3 getEyePos() { return eyePos; }
	Agent* getAttacker() { return attacker; }
	void setAttacker(Agent* a) { attacker = a; }
	void setHeadText(Ogre::String s) { headText->setCaption(s); }

	float getMwaitTime() { return mWaitTime; }
	float getCwaitTime() { return cWaitTime; }
	void setWaitTime(float f) { cWaitTime = f; }
	void add2WaitTime(float f) { cWaitTime +=f; }
	float getViewRange() { return viewRange; }
	float getAttackRange() { return attackRange; }
	void setShotsFired(int i) { shotsFired = i; }
	void incShotsFired() { shotsFired++; }
	void setShotsHit(int i) { shotsHit = i; }
	void incShotsHit() { shotsHit++; }
	int getShotsFired() { return shotsFired; }
	int getShotsHit() { return shotsHit; }

	//pure
	virtual physx::PxActor* getHitBox(int i) = 0;
	virtual void SetAimMode(bool b) = 0;
	virtual void Shoot(bool first, Ogre::Vector3 trg_pos = Ogre::Vector3::ZERO) = 0;

protected:
	std::vector<PxShape*>			mDetaching;

	BaseController* controller;

	//visual
	Ogre::SceneNode* node;
	Ogre::ManualObject* lineofsight;
	Ogre::ManualObject* coverlocs;
	MovableText* headText;

	//stats & info
	AIKnowledge* knowledge;
	Type t;
	Race race;
	int id;
	int hp;
	bool dead;

	//ai helper
	Agent* attacker;
	Ogre::Vector3 eyePos;
	float mWaitTime;
	float cWaitTime;
	float viewRange;
	float attackRange;

	//statistics
	int shotsFired;
	int shotsHit;

	//Physx
	PxActor * mActor;
	PxMaterial * mMaterial;
};

#endif