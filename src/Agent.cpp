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

/*
#include "Level.h"
#include "World.h"

#include "GameState.h"
#include "StateSystem.h"

#include "CamController.h"

// testing stash/jira linkage // dummy comment
*/
#include "Agent.h"
#include "World.h"

#include "AIKnowledge.h"
#include "BaseController.h"
#include "AIPerceptor.h"
#include "GlobalVars.h"
#include "Sample_TileMesh.h"
#include "Path.h"

#include "OgreVector3.h"
#include "OgreSceneNode.h"
#include "OgreMaterialManager.h"
#include "OgreManualObject.h"
#include "OgreStringConverter.h"
#include "OgreTechnique.h"

#include "GraphicsSystem.h"

#include "characterkinematic\PxController.h"
#include "PxRigidActor.h"
#include "extensions\PxRigidBodyExt.h"
#include "PxRigidDynamic.h"

using namespace Ogre;

bool Agent::isDetachable(PxFilterData& filterData)
{
	return filterData.word3 & PxU32(DETACHABLE_FLAG) ? true : false;
}

void Agent::onContact(const PxContactPair* pairs, PxU32 nbPairs)
{
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		PxU32 n = 2;
		const PxContactPairFlag::Enum delShapeFlags[] = { PxContactPairFlag::eREMOVED_SHAPE_0, PxContactPairFlag::eREMOVED_SHAPE_1 };
		const PxContactPair& cp = pairs[i];
		while (n--)
		{
			if (!(cp.flags & delShapeFlags[n]))
			{
				PxShape* shape = cp.shapes[n];
				PxFilterData fd = shape->getSimulationFilterData();
				if (isDetachable(fd))
				{
					mDetaching.push_back(shape);
				}
			}
		}
	}
}

void Agent::onShapeHit(const PxControllerShapeHit& hit)
{
	PxRigidDynamic* actor = hit.shape->getActor()->is<PxRigidDynamic>();
	if (actor && hit.shape->getFlags() == PxShapeFlag::eSIMULATION_SHAPE)
	{
		// We only allow horizontal pushes. Vertical pushes when we stand on dynamic objects creates
		// useless stress on the solver. It would be possible to enable/disable vertical pushes on
		// particular objects, if the gameplay requires it.
		if (hit.dir.y == 0.0f)
		{
			PxReal coeff = actor->getMass() * hit.length;
			PxRigidBodyExt::addForceAtLocalPos(*actor, hit.dir*coeff, PxVec3(0, 0, 0), PxForceMode::eIMPULSE);
		}
	}
}

Agent::Agent(int id, Race race, Vector3 position, float max_speed, float max_acc, float radius) : Moving(position, max_speed, max_acc, radius)
{
	this->id = id;
	this->race = race;
	attacker = nullptr;
	controller = nullptr;
	hp = 100;
	dead = false;

	node = GSYS->GetSceneMgr()->getRootSceneNode()->createChildSceneNode();
	node->setPosition(Position);
	node->setOrientation(Rotation);

	//lineofsight
	lineofsight = GSYS->GetSceneMgr()->createManualObject();
	lineofsight->setCastShadows(false);
	MaterialPtr lineofsightMaterial = MaterialManager::getSingleton().create("lineofsightMaterial", "General");
	lineofsightMaterial->setReceiveShadows(false);
	//lineofsightMaterial->getTechnique(0)->setLightingEnabled(false);

	//attach to this node
	node->attachObject(lineofsight);

	//coverlocs
	coverlocs = GSYS->GetSceneMgr()->createManualObject();
	coverlocs->setCastShadows(false);

	//attach to root node
	GSYS->GetSceneMgr()->getRootSceneNode()->attachObject(coverlocs);

	//head text
	headText = new MovableText("headTXT" + StringConverter::toString(id), " ", "whatever", 8);
	//Center horizontally and display above the node
	headText->setTextAlignment(MovableText::H_CENTER, MovableText::V_ABOVE);
	headText->setAdditionalHeight(2.0);
	headText->setVisible(true);
	node->attachObject(headText);

	//ais
	knowledge = new AIKnowledge(this);
	eyePos = Vector3::ZERO;
	mWaitTime = 4.0;
	cWaitTime = 0.0;
	viewRange = 60;//standard
	attackRange = 50;//standard

	//statistics
	shotsFired = 0;
	shotsHit = 0;
}

Agent::~Agent()
{
	node->detachAllObjects();
	node->getParentSceneNode()->removeAndDestroyChild(node);
	node = nullptr;

	lineofsight->clear();
	GSYS->GetSceneMgr()->destroyManualObject(lineofsight);
	lineofsight = nullptr;

	GSYS->GetSceneMgr()->getRootSceneNode()->detachObject(coverlocs);
	coverlocs->clear();
	GSYS->GetSceneMgr()->destroyManualObject(coverlocs);
	coverlocs = nullptr;

	delete headText;
	headText = nullptr;

	delete controller;
	controller = nullptr;

	delete knowledge;
	knowledge = nullptr;

	attacker = nullptr;
}

void Agent::Update()
{
	if (!dead)
	{
		//gather knowledge
		PRC->UpdateKnowledge(this);

		if (controller != nullptr)
		{
			controller->Update();
		}

		Moving::Update();
	}
	else if (cWaitTime > 0)
	{
		cWaitTime -= GlobalVars::Tick;
	}
}

void Agent::orderMove(float walk, float strafe)
{
	Vector3 dest = Position;
	dest += Vector3(strafe, 0, walk);
	SetDest(dest);

	this->resetBehavior();
	this->seekOn();//seek target
	if (this->getID() != 0) this->SetMaxSpeed(10);

	//avoid obstacles,walls & seperate
	//this->avoid1On();
	//this->avoid2On();
	this->avoid3On();
	//this->seperateOn();
}

void Agent::orderBrake()
{
	this->resetBehavior();
	this->brakeOn();//brake

	//avoid obstacles,walls & seperate
	//this->avoid1On();
	//this->avoid2On();
	this->avoid3On();
	//this->seperateOn();
}

void Agent::orderArrive(Vector3 pos)
{
	SetDest(pos);
	this->SetMaxSpeed(6);
	this->resetBehavior();
	this->arriveOn();//arrive at destination

	//avoid obstacles,walls & seperate
	//this->avoid1On();
	//this->avoid2On();
	this->avoid3On();
	//this->seperateOn();
}

void Agent::orderGoTo(Vector3 pos)
{
	this->SetMaxSpeed(10);
	SetDest(pos);
	SMPL->GetPath(Position, pos, mPath);
	orderPathFollow();
}

void Agent::orderPathFollow()
{
	if (mPath->GetLength() > 0)
	{
		this->SetMaxSpeed(10);
		//find closest node
		float min = 999999;
		for (int i = 0; i < mPath->GetLength(); i++)
		{
			Vector3 delta = Position - *mPath->GetNode(i).getPos();
			if (delta.length() < min)
			{
				min = delta.length();
				PathPosition = i;
			}
		}

		//go
		this->resetBehavior();
		this->pathOn();

		//avoid obstacles,walls & seperate
		//this->avoid1On();
		//this->avoid2On();
		this->avoid3On();
		//this->seperateOn();
	}
}

void Agent::orderNextPathNode()
{
	if (pPath->GetLength() > 0)
	{
		PatrolPathPosition++;
		PatrolPathPosition %= pPath->GetLength();
		Vector3 * target = pPath->GetNode(PatrolPathPosition).getPos();
		orderGoTo(*target);
	}
}

void Agent::add2Hp(int i)
{
	hp += i;
	if (hp <= 0)
	{
		Die();
	}
	if (this == WORLD->getPlayerAgent())
	{
		headText->setCaption(StringConverter::toString(hp));
	}
}

void Agent::Die()
{
	hp = 0;
	dead = true;
	headText->setVisible(false);
	mWaitTime = 10.0;
	cWaitTime = mWaitTime;
}

void Agent::ChangeController(BaseController* ct)
{
	delete controller;
	controller = ct;
	if (controller != nullptr)
	{
		controller->SetAgent(this);
	}
}