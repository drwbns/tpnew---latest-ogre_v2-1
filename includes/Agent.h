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

//#include "prereqs.h"
#include "OGRE\OgrePrerequisites.h"
#include "physxPrereqs.h"

#include "characterkinematic\PxController.h"
#include "characterkinematic\PxControllerBehavior.h"
#include "PxSimulationEventCallback.h"

#include "MovableText.h"
#include "Moving.h"
#include "AIKnowledge.h"

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
		NO_COLLIDE = 1 << 28,
		CCD_FLAG = 1 << 29,
		SNOWBALL_FLAG = 1 << 30,
		DETACHABLE_FLAG = 1 << 31
	};

protected:
	Agent(int id, Race race, Ogre::Vector3 position, float max_speed, float max_acc, float radius);
	virtual ~Agent();
	virtual void Update() override;

public:
	static bool isDetachable(PxFilterData & filterData);
	// Implements PxSimulationEventCallback
	virtual	void							onContact(const PxContactPair* pairs, PxU32 nbPairs);
	virtual	void							onTrigger(PxTriggerPair*, PxU32) {}
	virtual void							onConstraintBreak(PxConstraintInfo*, PxU32) {}
	virtual void							onWake(PxActor**, PxU32) {}
	virtual void							onSleep(PxActor**, PxU32) {}

	///////////////////////////////////////////////////////////////////////////////

	// Implements PxUserControllerHitReport
	virtual void							onShapeHit(const PxControllerShapeHit& hit) override;
	virtual void							onControllerHit(const PxControllersHit& hit) override {}
	virtual void							onObstacleHit(const PxControllerObstacleHit& hit) override {}

	// Implements PxControllerBehaviorCallback
	virtual PxControllerBehaviorFlags		getBehaviorFlags(const PxShape&, const PxActor&) override { return PxControllerBehaviorFlags(0); }
	virtual PxControllerBehaviorFlags		getBehaviorFlags(const PxController&) override { return PxControllerBehaviorFlags(0); }
	virtual PxControllerBehaviorFlags		getBehaviorFlags(const PxObstacle&) override
	{ return PxControllerBehaviorFlags(0); }

	virtual void orderMove(float walk, float strafe);
	virtual void orderBrake();
	virtual void orderArrive(Ogre::Vector3 pos);
	virtual void orderGoTo(Ogre::Vector3 pos);
	virtual void orderPathFollow();
	virtual void orderNextPathNode();
	virtual void Die();

	//get / set
	virtual Ogre::Vector3 *GetFirePosition() = 0;

	int getID() const { return id; }
	int getRace() const { return race; }
	void ChangeController(BaseController* ct);
	BaseController* getController() const { return controller; }
	void add2Hp(int i);
	int getHP() const { return hp; }
	bool isDead() const { return dead; }
	void ShowHeadText(bool b) const { headText->setVisible(b); }
	AIKnowledge* getKnowledge() const { return knowledge; }
	Ogre::Vector3 getEyePos() const { return eyePos; }
	Agent* getAttacker() const { return attacker; }
	void setAttacker(Agent* a) { attacker = a; }
	void setHeadText(Ogre::String s) const { headText->setCaption(s); }

	float getMwaitTime() const { return mWaitTime; }
	float getCwaitTime() const { return cWaitTime; }
	void setWaitTime(float f) { cWaitTime = f; }
	void add2WaitTime(float f) { cWaitTime +=f; }
	float getViewRange() const { return viewRange; }
	float getAttackRange() const { return attackRange; }
	void setShotsFired(int i) { shotsFired = i; }
	void incShotsFired() { shotsFired++; }
	void setShotsHit(int i) { shotsHit = i; }
	void incShotsHit() { shotsHit++; }
	int getShotsFired() const { return shotsFired; }
	int getShotsHit() const { return shotsHit; }

	Ogre::v1::AnimationState* getRunState() const { return runAnimState; }
	Ogre::v1::AnimationState* getIdleAnimState() const
	{ return idleAnimState; }



	//pure
	virtual PxShape* getHitBox(int i) = 0;
	virtual void SetAimMode(bool b) = 0;
	virtual void Shoot() = 0;

protected:
	std::vector<PxShape*>			mDetaching;

	BaseController* controller;

	//visual
	Ogre::SceneNode* node;
	Ogre::ManualObject* lineofsight;
	Ogre::ManualObject* coverlocs;
	MovableText* headText;

	Ogre::v1::Entity* mEnt;
	Ogre::v1::Entity* wEnt;
	Ogre::v1::AnimationState* aimAnimState;
	Ogre::v1::AnimationState* idleAnimState;
	Ogre::v1::AnimationState* runAnimState;
	Ogre::v1::AnimationState* shootAnimState;
	Ogre::v1::AnimationState* deadAnimState;
	bool aimMode;

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

	PxFilterData characterFilterData;
	PxControllerFilters characterControllerFilters;
};

#endif