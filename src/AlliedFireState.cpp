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
#include "AlliedFireState.h"
#include "World.h"
#include "AIKnowledge.h"
#include "AgentInfo.h"
#include "StateSystem.h"
#include "GameState.h"
using namespace Ogre;

AlliedFireState::AlliedFireState(int id) : AIState(id)
{
}

AlliedFireState::~AlliedFireState()
{
}

void AlliedFireState::Enter(Agent* agent)
{
	agent->SetAimMode(true);
	//order to stop
	agent->orderBrake();
}

void AlliedFireState::Execute(Agent* agent)
{
	//find closest Allied
	int id = agent->getKnowledge()->getClosestVisibleEnemy();
	if (id != -1)
	{
		Vector3 closest = agent->getKnowledge()->getEnemy(id).GetPosition();
		float coeff = agent->GetPosition().distance(closest) / agent->getAttackRange();
		Vector3 target = closest;
		target.x += coeff * Math::RangeRandom(-1.0 , 1.0);
		target.y += coeff * Math::RangeRandom(-0.125*agent->getEyePos().y, 1.50*agent->getEyePos().y);
		target.z += coeff * Math::RangeRandom(-1.0 , 0.0);
		agent->Shoot(false, target);

		//set direction
		Vector3 dir = closest - agent->GetPosition();
		dir.y = 0;
		Quaternion q = dir.getRotationTo(Vector3::UNIT_Z).Inverse();
		agent->SetDirection(q);
	}
}

void AlliedFireState::Exit(Agent* agent)
{
	agent->SetAimMode(false);
}

bool AlliedFireState::isReady(Agent* agent)
{
	return true;
}