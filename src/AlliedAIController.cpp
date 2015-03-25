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

#include "stdafx.h"
#include "AlliedAIController.h"
#include "GameState.h"
#include "StateSystem.h"
#include "AIStateMachine.h"
using namespace Ogre;

AlliedAIController::AlliedAIController() : BaseController()
{
	sm = NULL;
}

AlliedAIController::~AlliedAIController()
{
	delete sm;
	sm = NULL;
}

void AlliedAIController::SetAgent(Agent* a)
{
	BaseController::SetAgent(a);
	if (sm == NULL)
	{
		//prepare states
		int id = 0;
		AIState* base = new AIState(id++);//0

			AlliedIdleState* idle_state = new AlliedIdleState(id++);//1
				
				AlliedKeepCloseState* kclose_state1 = new AlliedKeepCloseState(id++);//2
					AlliedGetCloseState*   gclose_state1  = new AlliedGetCloseState(id++);//3

				AlliedWaitState* wait_state = new AlliedWaitState(id++);//4
					AlliedLookAroundState* looka_state  = new AlliedLookAroundState(id++);//5
					AlliedWaitIdlingState* widling_state = new AlliedWaitIdlingState(id++);//6
					AlliedOutOfWayState* outofway_state = new AlliedOutOfWayState(id++);//7

			AlliedOffensiveState* ofensive_state = new AlliedOffensiveState(id++);//8

				AlliedAttackState* atack_state = new AlliedAttackState(id++);//9

					AlliedFireState*   fire_state   = new AlliedFireState(id++);//10
					AlliedStrafeState* strafe_state = new AlliedStrafeState(id++);//11

				AlliedSeekState* seek_state = new AlliedSeekState(id++);//12

					AlliedExploreState* explore_state = new AlliedExploreState(id++);//13
					AlliedInspectState* inspect_state = new AlliedInspectState(id++);//14

				//AlliedKeepCloseState* kclose_state2 = new AlliedKeepCloseState(id++);//15
					//AlliedGetCloseState*   gclose_state2  = new AlliedGetCloseState(id++);//16

		//setup hierarchy
		base->AddChild(idle_state);
			idle_state->AddChild(kclose_state1);
				kclose_state1->AddChild(gclose_state1);

			idle_state->AddChild(wait_state);
				wait_state->AddChild(looka_state);
				wait_state->AddChild(widling_state);
				wait_state->AddChild(outofway_state);

		base->AddChild(ofensive_state);
			ofensive_state->AddChild(atack_state);
				atack_state->AddChild(fire_state);
				atack_state->AddChild(strafe_state);

			ofensive_state->AddChild(seek_state);
				seek_state->AddChild(explore_state);
				seek_state->AddChild(inspect_state);

			//ofensive_state->AddChild(kclose_state2);
				//kclose_state2->AddChild(gclose_state2);

		//create sm
		sm = new AIStateMachine(agent, base, 1.0);
	}
	else
	{
		sm->SetOwner(agent);
	}
}

void AlliedAIController::Update()
{
	if (sm != NULL && !agent->isDead())
	{
		sm->Update();
	}
}