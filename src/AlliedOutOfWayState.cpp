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


#include "AlliedOutOfWayState.h"
#include "Agent.h"
#include "World.h"
#include "AIConsts.h"

using namespace Ogre;

AlliedOutOfWayState::AlliedOutOfWayState(int id) : AIState(id)
{
}

AlliedOutOfWayState::~AlliedOutOfWayState()
{
}

void AlliedOutOfWayState::Enter(Agent* agent)
{
	Vector3 dest = agent->GetPosition();
	Vector3 delta = dest - WORLD->getPlayerAgent()->GetPosition();
	delta.y = 0;
	delta.normalise();

	Quaternion q;
	q.FromAngleAxis(Radian(Math::PI / 2), Vector3::UNIT_Y);
	float dist = WORLD->getPlayerAgent()->GetRadius() * AIConsts::PlayerOutOfWayDistance;
	
	float angle = delta.getRotationTo(WORLD->getPlayerAgent()->GetDirection() * Vector3::UNIT_Z).getYaw().valueDegrees();
	if (angle < 0)
	{
		dest += WORLD->getPlayerAgent()->GetDirection() * q * Vector3::UNIT_Z * dist;
	}
	else
	{
		dest += WORLD->getPlayerAgent()->GetDirection() * q * Vector3::NEGATIVE_UNIT_Z * dist;
	}

	agent->orderGoTo(dest);
}

void AlliedOutOfWayState::Execute(Agent* agent)
{
}

void AlliedOutOfWayState::Exit(Agent* agent)
{
}

bool AlliedOutOfWayState::isReady(Agent* agent)
{
	//stopped and touching player
	float dist2player = agent->GetPosition().distance(WORLD->getPlayerAgent()->GetPosition()) - AIConsts::PlayerTouchDistance;
	return agent->isOnDest() && dist2player < agent->GetRadius() + WORLD->getPlayerAgent()->GetRadius();
}