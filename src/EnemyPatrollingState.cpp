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

#include "EnemyPatrollingState.h"
#include "Agent.h"
#include "GlobalVars.h"

using namespace Ogre;

EnemyPatrollingState::EnemyPatrollingState(int id) : AIState(id)
{
}

EnemyPatrollingState::~EnemyPatrollingState()
{
}

void EnemyPatrollingState::Enter(Agent * agent)
{
	agent->setWaitTime(0);
	//turn around randomly
	Quaternion mq = agent->GetRotation();
	float some = Math::fDeg2Rad * Math::RangeRandom(-90, 90);
	Quaternion aq; aq.FromAngleAxis(Radian(some), Vector3::UNIT_Y);
	agent->SetDirection(mq * aq);
}

void EnemyPatrollingState::Execute(Agent * agent)
{
	agent->add2WaitTime(GlobalVars::Tick);
}

void EnemyPatrollingState::Exit(Agent * agent)
{
	agent->setWaitTime(agent->getMwaitTime());
}

bool EnemyPatrollingState::isReady(Agent * agent)
{
	return agent->getCwaitTime() < agent->getMwaitTime();
}