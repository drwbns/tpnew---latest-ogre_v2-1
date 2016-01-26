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

#include "AlliedGetCloseState.h"
#include "GlobalVars.h"

#include "World.h"
#include "AIConsts.h"

#include "OgreVector3.h"
#include "OgreMath.h"

using namespace Ogre;

AlliedGetCloseState::AlliedGetCloseState(int id) : AIState(id)
{
}

AlliedGetCloseState::~AlliedGetCloseState()
{
}

void AlliedGetCloseState::Enter(Agent * agent)
{
	//assumes that 0 is player, allies start with 1 and so on, also max 3 allies
	Vector3 dest = WORLD->getPlayerAgent()->GetPosition();
	int ally_id = agent->getID();
	if (ally_id == 1)
	{
		Quaternion q;
		q.FromAngleAxis(Radian(Math::PI / 2), Vector3::UNIT_Y);
		dest += WORLD->getPlayerAgent()->GetDirection() * q * Vector3::UNIT_Z * AIConsts::PlayerCloseDistance;
	}
	else if (ally_id == 2)
	{
		Quaternion q;
		q.FromAngleAxis(Radian(Math::PI / 2), Vector3::UNIT_Y);
		dest += WORLD->getPlayerAgent()->GetDirection() * q * Vector3::NEGATIVE_UNIT_Z * AIConsts::PlayerCloseDistance;
	}
	else if (ally_id == 3)
	{
		Quaternion q;
		q.FromAngleAxis(Radian(Math::PI / 1), Vector3::UNIT_Y);
		dest += WORLD->getPlayerAgent()->GetDirection() * q * Vector3::UNIT_Z * AIConsts::PlayerCloseDistance;
	}

	agent->orderGoTo(dest);
}

void AlliedGetCloseState::Execute(Agent * agent)
{
	//set direction
	if (agent->GetVelocity().squaredLength() >= 0.1)
	{
		agent->SetDirection(agent->GetVelocity().getRotationTo(Vector3::UNIT_Z).Inverse());
	}

	//checks continiously
	if (GlobalVars::Tick_4 == 0)
	{
		float dist2player = agent->GetPosition().distance(WORLD->getPlayerAgent()->GetPosition());
		if (dist2player > 3)
		{
			this->Enter(agent);
		}
	}
}

void AlliedGetCloseState::Exit(Agent * agent)
{
}

bool AlliedGetCloseState::isReady(Agent * agent)
{
	return true;
}