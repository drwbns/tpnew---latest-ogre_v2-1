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

#ifndef AIStateMachine_h_
#define AIStateMachine_h_

#include "AIState.h"

#include "EnemyIdleState.h"
#include "EnemyPatrolState.h"
#include "EnemyWaitState.h"
#include "EnemyNextPatrolState.h"
#include "EnemyPatrolIdlingState.h"
#include "EnemyWaitIdlingState.h"
#include "EnemyLookAroundState.h"
#include "EnemyAttackState.h"
#include "EnemyDefenseState.h"
#include "EnemyDefensiveState.h"
#include "EnemyFallbackState.h"
#include "EnemyOffensiveState.h"
#include "EnemySeekState.h"
#include "EnemyExploreState.h"
#include "EnemyInspectState.h"
#include "EnemyFireState.h"
#include "EnemyStrafeState.h"

#include "AlliedAttackState.h"
#include "AlliedFireState.h"
#include "AlliedIdleState.h"
#include "AlliedLookAroundState.h"
#include "AlliedGetCloseState.h"
#include "AlliedOffensiveState.h"
#include "AlliedOutOfWayState.h"
#include "AlliedKeepCloseState.h"
#include "AlliedStrafeState.h"
#include "AlliedWaitIdlingState.h"
#include "AlliedWaitState.h"
#include "AlliedInspectState.h"
#include "AlliedExploreState.h"
#include "AlliedSeekState.h"


class AIStateMachine : public Ogre::GeneralAllocatedObject
{
public:
	AIStateMachine(Agent* owner, AIState* base, float lockTime);
	~AIStateMachine();
	void Update();

	//gets & sets
	void SetOwner(Agent* a) { owner = a; }
	void SetLock(bool b) { locked = b;cLockTime = mLockTime; }
	void SetState(int id) { pstate = cstate;cstate = id; }

private:
	Agent* owner;
	AIState* baseState;
	int cstate;
	int pstate;
	std::vector<AIState*> cstates;
	std::vector<AIState*> pstates;

	//lock decisions
	bool locked;
	float cLockTime;
	float mLockTime;
};

#endif