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


#include "GameState.h"

#include "StateSystem.h"
#include "GraphicsSystem.h"
#include "GuiSystem.h"
#include "CamController.h"
#include "FreeCamController.h"
#include "RtsCamController.h"
#include "TpCamController.h"
#include "PhysicsSystem.h"
#include "BillboardSystem.h"
#include "ParticleManager.h"
#include "Level.h"
#include "World.h"
#include "InputController.h"
#include "AlliedAIController.h"
#include "EnemyAIController.h"
#include "GlobalVars.h"
#include "PtfNode.h"

#include "OgreTimer.h"

using namespace Ogre;

GameState::GameState()
{
}

GameState::~GameState()
{
}

void GameState::Initialize()
{
	UISYS->InitDebuggers();
	UISYS->SetCursor(false);
	UISYS->SetCrossHair(false);

	//add cam conrollers
	ccam = 2;//current cam = tps

	CamController* cam1 = new FreeCamController();
	cam1->Initialize(GSYS->GetCamera());
	cams.push_back(cam1);

	CamController* cam2 = new RtsCamController();
	cam2->Initialize(GSYS->GetCamera());
	cams.push_back(cam2);

	CamController* cam3 = new TpCamController();
	cam3->Initialize(GSYS->GetCamera());
	TPCAM->SetFarCamOffset(   Vector3(0.0,  2.0, -4.0));
	TPCAM->SetFarDestOffset(  Vector3(0.0,  1.5,  1.0));
	TPCAM->SetCloseCamOffset( Vector3(-0.8, 1.5, -1.75));
	TPCAM->SetCloseDestOffset(Vector3(-0.8, 1.75,  1.0));
	TPCAM->SetCloseSpeed(8);
	cams.push_back(cam3);

	//order is important here on
	PHY->Initialize();

	mWorld = new World();
	mWorld->setPlayer(0);

	//mWorld->addAgent(Agent::AT_VIP, Agent::RT_ALLY, Vector3(-2,0.1,-30), new InputController());
	//mWorld->addAgent(Agent::AT_GIGN, Agent::RT_ENEMY, Vector3(2,0.1,30), NULL);
	//mWorld->addAgent(Agent::AT_GIGN, Agent::RT_ENEMY, Vector3(-5,0.1,30), NULL);
	Agent * player = mWorld->addAgent(Agent::AT_VIP, Agent::RT_ALLY, Vector3(0,0.1,-28), new InputController());

	for (int i=-1;i<=1;i++)
	{
		if (i != 0)
		{
			mWorld->addAgent(Agent::AT_VIP, Agent::RT_ALLY, Vector3(i*-4, 0.1, -30), new AlliedAIController());
		}
	}
	
	for (int i=0;i<=1;i++)
	{
		mWorld->addAgent(Agent::AT_GIGN, Agent::RT_ENEMY, Vector3(i*4,0.1,30), new EnemyAIController());
		int ci = mWorld->getAgentTotal() - 1;
		Vector3 * cpos = &mWorld->getAgent(ci)->GetPosition();
		PtfNode node1(cpos);
		cpos->z -= 30;
		PtfNode node2(cpos);
		cposs->x -= 10;
		PtfNode node3(cpos);
		cposs->z += 30;
		PtfNode node4(cpos);
		mWorld->getAgent(ci)->GetPatrolPath()->AddNode(node1);
		mWorld->getAgent(ci)->GetPatrolPath()->AddNode(node2);
		mWorld->getAgent(ci)->GetPatrolPath()->AddNode(node3);
		mWorld->getAgent(ci)->GetPatrolPath()->AddNode(node4);
	}
	
	mLevel = new Level;

	BBS->Initialize();
	PTM->Initialize();
	
}

void GameState::Finalize()
{
	PTM->Finalize();

	UISYS->DeinitDebuggers();

	//del cam controllers
	for (int i=0;i<(int)cams.size();i++)
	{
		cams[i]->Finalize();
		delete cams[i];
		cams[i] = NULL;
	}
	cams.clear();

	delete mWorld;
	mWorld = NULL;

	delete mLevel;
	mLevel = NULL;

	PHY->Finalize();
	BBS->Finalize();
}

void GameState::Update()
{
	if (GlobalVars::Tick > 0)
	{
		Ogre::Timer tmr;
		unsigned long tm = tmr.getMicroseconds();
		mWorld->Update();
		tm = tmr.getMicroseconds() - tm;
		if (GlobalVars::Tick_4 == 0)
		{
			//GlobalVars::test1 = tm;
		}

		tm = tmr.getMicroseconds();
		PHY->Update();
		tm = tmr.getMicroseconds() - tm;
		if (GlobalVars::Tick_4 == 0)
		{
			//GlobalVars::test2 = tm;
		}

		tm = tmr.getMicroseconds();
		BBS->Update();
		//PTM->Update();
		mLevel->Update();
		tm = tmr.getMicroseconds() - tm;
		if (GlobalVars::Tick_4 == 0)
		{
			//GlobalVars::test3 = tm;
		}
	}
}