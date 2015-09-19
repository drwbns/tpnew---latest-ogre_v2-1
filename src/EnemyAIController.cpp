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


#include "EnemyAIController.h"
#include "GameState.h"
#include "StateSystem.h"
#include "AIStateMachine.h"
using namespace Ogre;

EnemyAIController::EnemyAIController() : BaseController()
{
	sm = NULL;
}

EnemyAIController::~EnemyAIController()
{
	delete sm;
	sm = NULL;
}

void EnemyAIController::SetAgent(Agent* a)
{
	BaseController::SetAgent(a);
	if (sm == NULL)
	{
		//prepare states
		int id = 0;
		AIState* base = new AIState(id++);//0

			EnemyIdleState* idle_state = new EnemyIdleState(id++);//1

				EnemyPatrolState* patrol_state = new EnemyPatrolState(id++);//2

					EnemyNextPatrolState*   nextp_state  = new EnemyNextPatrolState(id++);//3
					EnemyPatrollingState* pidling_state = new EnemyPatrollingState(id++);//4

				EnemyWaitState* wait_state = new EnemyWaitState(id++);//5

					EnemyLookAroundState* looka_state  = new EnemyLookAroundState(id++);//6
					EnemyWaitIdlingState* widling_state = new EnemyWaitIdlingState(id++);//7

			EnemyOffensiveState* ofensive_state = new EnemyOffensiveState(id++);//8

				EnemyAttackState* atack_state = new EnemyAttackState(id++);//9

					EnemyFireState*   fire_state   = new EnemyFireState(id++);//10
					EnemyStrafeState* strafe_state = new EnemyStrafeState(id++);//11

				EnemySeekState* seek_state = new EnemySeekState(id++);//12

					EnemyExploreState* explore_state = new EnemyExploreState(id++);//13
					EnemyInspectState* inspect_state = new EnemyInspectState(id++);//14

			EnemyDefensiveState* defensive_state = new EnemyDefensiveState(id++);//15

				EnemyFallbackState* fback_state   = new EnemyFallbackState(id++);//16
				EnemyDefenseState*  defense_state = new EnemyDefenseState(id++);//17

		//setup hierarchy
		base->AddChild(idle_state);
			idle_state->AddChild(patrol_state);
				patrol_state->AddChild(nextp_state);
				patrol_state->AddChild(pidling_state);
			idle_state->AddChild(wait_state);
				wait_state->AddChild(looka_state);
				wait_state->AddChild(widling_state);

		base->AddChild(ofensive_state);
			ofensive_state->AddChild(atack_state);
				atack_state->AddChild(fire_state);
				atack_state->AddChild(strafe_state);
			ofensive_state->AddChild(seek_state);
				seek_state->AddChild(explore_state);
				seek_state->AddChild(inspect_state);

		base->AddChild(defensive_state);
			defensive_state->AddChild(fback_state);
			defensive_state->AddChild(defense_state);

		//create sm
		sm = new AIStateMachine(agent, base, 1.0);
	}
	else
	{
		sm->SetOwner(agent);
	}
}

void EnemyAIController::Update()
{
	if (sm != NULL && !agent->isDead())
	{
		sm->Update();
	}
}