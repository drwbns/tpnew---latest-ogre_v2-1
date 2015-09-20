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


#include "AlliedExploreState.h"
#include "Path.h"
#include "NavMesher.h"
#include "Agent.h"


#include "OgreVector3.h"


using namespace Ogre;

AlliedExploreState::AlliedExploreState(int id) : AIState(id)
{
}

AlliedExploreState::~AlliedExploreState()
{
}

void AlliedExploreState::Enter(Agent* agent)
{
}

void AlliedExploreState::Execute(Agent* agent)
{
	if (!agent->isActive(Moving::path))
	{
		//select a random target node
		int rnd = rand() % NAV->GetTotalPos();
		Vector3 target = NAV->GetPos(rnd);
		//find path
		Path* path = agent->GetPath();
		NAV->GetPath(agent->GetPosition(), target, path);
		//show path
		path->ShowDebug(true);
		//order
		agent->orderPathFollow();
	}

	//set direction
	if (agent->GetVelocity().length() > 0.1)
	{
		agent->SetDirection(agent->GetVelocity().getRotationTo(Vector3::UNIT_Z).Inverse());
	}
}

void AlliedExploreState::Exit(Agent* agent)
{
}

bool AlliedExploreState::isReady(Agent* agent)
{
	return false;
}