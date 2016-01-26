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

#include "AIStateMachine.h"

#include "GlobalVars.h"
#include "Agent.h"
#include "AIState.h"

#include "OgreStringConverter.h"

using namespace Ogre;

AIStateMachine::AIStateMachine(Agent* owner, AIState* base, float lockTime) : owner(owner), baseState(base)
{
	cstate = base->GetId();
	pstate = base->GetId();
	locked = false;
	mLockTime = lockTime;
	cLockTime = lockTime;
}

AIStateMachine::~AIStateMachine()
{
	owner = nullptr;

	delete baseState;
	baseState = nullptr;

	cstates.clear();
	pstates.clear();
}

void AIStateMachine::Update()
{
	if (baseState != nullptr)
	{
		if (!locked)
		{
			pstates.clear();
			pstates = cstates;
			cstates.clear();
			baseState->GetAvailables(this, owner, cstates);

			//lock if state changes
			SetLock(cstate != pstate);

			//exit disappeared ones
			for (size_t i = 0; i < pstates.size(); i++)
			{
				bool exist = false;
				for (size_t j = 0; j < cstates.size(); j++)
				{
					if (pstates[i]->GetId() == cstates[j]->GetId())
					{
						exist = true;
						break;
					}
				}
				if (!exist)
				{
					pstates[i]->Exit();
				}
			}

			//enter newly appeared
			for (size_t i = 0; i < cstates.size(); i++)
			{
				bool exist = false;
				for (size_t j = 0; j < pstates.size(); j++)
				{
					if (cstates[i]->GetId() == pstates[j]->GetId())
					{
						exist = true;
						break;
					}
				}
				if (!exist)
				{
					cstates[i]->Enter();
				}
			}
		}
		else
		{
			if (cLockTime > 0)
			{
				cLockTime -= GlobalVars::Tick;
			}
			else
			{
				locked = false;
			}
		}

		//update current
		String states = "/";
		for (size_t i = 0; i < cstates.size(); i++)
		{
			cstates[i]->Execute();
			states += StringConverter::toString(cstates[i]->GetId());
			if (i < cstates.size() - 1)states += "-";
		}

		owner->setHeadText(StringConverter::toString(owner->getHP()) + states);
	}
}