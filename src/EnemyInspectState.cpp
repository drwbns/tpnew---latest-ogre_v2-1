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

#include "EnemyInspectState.h"
#include "World.h"
#include "AIKnowledge.h"
using namespace Ogre;

EnemyInspectState::EnemyInspectState(int id) : AIState(id)
{
}

EnemyInspectState::~EnemyInspectState()
{
}

void EnemyInspectState::Enter(Agent * agent)
{
	//goto last known enemy location
	int id = agent->getKnowledge()->getClosestEnemy();
	if (id != -1)
	{
		Vector3 dest = agent->getKnowledge()->getEnemy(id).GetPosition();
		agent->orderGoTo(dest);
	}
}

void EnemyInspectState::Execute(Agent * agent)
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

void EnemyInspectState::Exit(Agent * agent)
{
}

bool EnemyInspectState::isReady(Agent * agent)
{
	return true;
}