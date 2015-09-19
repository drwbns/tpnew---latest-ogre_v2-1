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


#include "EnemyStrafeState.h"
#include "World.h"
#include "AIKnowledge.h"
using namespace Ogre;

EnemyStrafeState::EnemyStrafeState(int id) : AIState(id)
{
}

EnemyStrafeState::~EnemyStrafeState()
{
}

void EnemyStrafeState::Enter(Agent* agent)
{
}

void EnemyStrafeState::Execute(Agent* agent)
{
	int i = agent->getKnowledge()->getClosestCover();
	if (i != -1)
	{
		Vector3 dest = agent->getKnowledge()->getCover(i).GetPosition();
		if (agent->GetDest() != dest)
		{
			agent->orderGoTo(dest);
		}
	}
}

void EnemyStrafeState::Exit(Agent* agent)
{
}

bool EnemyStrafeState::isReady(Agent* agent)
{
	return agent->getKnowledge()->totalCover() > 0;
}