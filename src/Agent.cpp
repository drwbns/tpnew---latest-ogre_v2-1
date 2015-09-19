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


*/
#include "Agent.h"
#include "AIKnowledge.h"
#include "BaseController.h"
#include "AIPerceptor.h"
#include "GlobalVars.h"
#include "Sample_TileMesh.h"

#include "OgreVector3.h"
#include "OgreSceneNode.h"
#include "OgreMaterialManager.h"
#include "OgreManualObject.h"
#include "OgreStringConverter.h"

#include "GraphicsSystem.h"


using namespace Ogre;

Agent::Agent(int id, Race race, Vector3 position, float max_speed, float max_acc, float radius) : Moving(position, max_speed, max_acc, radius)
{
	this->id = id;
	this->race = race;
	attacker = NULL;
	controller = NULL;
	hp = 100;
	dead = false;

	node = GSYS->GetSceneMgr()->getRootSceneNode()->createChildSceneNode();
	node->setPosition(Position);
	node->setOrientation(Rotation);

	//lineofsight
	lineofsight =  GSYS->GetSceneMgr()->createManualObject();
	lineofsight->setCastShadows(false);
	MaterialPtr lineofsightMaterial = MaterialManager::getSingleton().create("lineofsightMaterial","General");
	lineofsightMaterial->setReceiveShadows(false);
	lineofsightMaterial->getTechnique(0)->setLightingEnabled(false);

	//attach to this node
	node->attachObject(lineofsight);

	//coverlocs
	coverlocs =  GSYS->GetSceneMgr()->createManualObject();
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
	*eyePos = Vector3::ZERO;
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
	node->getParentSceneNode()->removeAndDestroyChild(node->getName());
	node = NULL;

	lineofsight->clear();
	GSYS->GetSceneMgr()->destroyManualObject(lineofsight);
	lineofsight = NULL;

	GSYS->GetSceneMgr()->getRootSceneNode()->detachObject(coverlocs->getName());
	coverlocs->clear();
	GSYS->GetSceneMgr()->destroyManualObject(coverlocs);
	coverlocs = NULL;

	delete headText;
	headText = NULL;
	
	delete controller;
	controller = NULL;

	delete knowledge;
	knowledge = NULL;

	attacker = NULL;
}

void Agent::Update()
{
	if (!dead)
	{
		//gather knowledge
		PRC->UpdateKnowledge(this);

		if (controller != NULL)
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
	Ogre::Vector3 dest = Position;
	dest += Vector3(strafe, 0, walk);
	SetDest(dest);

	this->resetBehavior();
	this->seekOn();//seek target
	if(this->getID() != 0) this->SetMaxSpeed(10);

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

void Agent::orderArrive(Ogre::Vector3 pos)
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

void Agent::orderGoTo(Ogre::Vector3 pos)
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
		for (int i=0;i<mPath->GetLength();i++)
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
	if (hp<=0)
	{
		Die();
	}
	if (this == PLAYER)
	{
		headText->setCaption(Ogre::StringConverter::toString(hp));
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
	if (controller != NULL)
	{
		controller->SetAgent(this);
	}
}