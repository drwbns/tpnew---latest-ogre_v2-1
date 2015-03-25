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
#include "AIState.h"
#include "AIStateMachine.h"

AIState::AIState(int id) : id(id), parent(NULL)
{
}

AIState::~AIState()
{
	parent = NULL;
	for (size_t i=0;i<children.size();i++)
	{
		delete children[i];
		children[i] = NULL;
	}
	children.clear();
}

void AIState::Enter(Agent* agent)
{
}

void AIState::Execute(Agent* agent)
{
}

void AIState::Exit(Agent* agent)
{
}

void AIState::GetAvailables(AIStateMachine* sm, Agent* agent, std::vector<AIState*> &list)
{
	if (this->IsLeaf() && this->isReady(agent))
	{
		list.push_back(this);
	}
	else
	{
		for (int i=0;i<(int)children.size();i++)
		{
			if (children[i]->isReady(agent))
			{
				if (children[i]->IsLeaf())
				{
					sm->SetState(children[i]->GetId());
				}
				children[i]->GetAvailables(sm, agent, list);
			}
		}
	}
}