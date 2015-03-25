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

#include "StdAfx.h"
#include "AlliedInspectState.h"
#include "World.h"
#include "AIKnowledge.h"
#include "StateSystem.h"
#include "GameState.h"
using namespace Ogre;

AlliedInspectState::AlliedInspectState(int id) : AIState(id)
{
}

AlliedInspectState::~AlliedInspectState()
{
}

void AlliedInspectState::Enter(Agent* agent)
{
	//goto last known enemy location
	int id = agent->getKnowledge()->getClosestEnemy();
	if (id != -1)
	{
		Vector3 dest = agent->getKnowledge()->getEnemy(id).GetPosition();
		agent->orderGoTo(dest);
	}
}

void AlliedInspectState::Execute(Agent* agent)
{
	//set direction
	if (agent->GetVelocity().length() > 0.1)
	{
		agent->SetDirection(agent->GetVelocity().getRotationTo(Vector3::UNIT_Z).Inverse());
	}

	//mark as dead if reached
	if (agent->isOnDest())
	{
		int id = agent->getKnowledge()->getClosestEnemy();
		if (id > -1)
		{
			agent->getKnowledge()->setEnemyFlag(id, AgentInfo::IF_LOST);
		}
	}
}

void AlliedInspectState::Exit(Agent* agent)
{
}

bool AlliedInspectState::isReady(Agent* agent)
{
	return true;
}