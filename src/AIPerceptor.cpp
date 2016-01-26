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

#include "AIPerceptor.h"
#include "AIKnowledge.h"
#include "PhysicsSystem.h"
#include "AIUtility.h"
#include "World.h"
#include "Sample_TileMesh.h"
/*
#include "Level.h"

#include "AgentInfo.h"
#include "PositionalInfo.h"

#include "GameState.h"
#include "StateSystem.h"

*/

using namespace Ogre;

template<> AIPerceptor* Singleton<AIPerceptor>::msSingleton = nullptr;

AIPerceptor* AIPerceptor::getSingletonPtr(void)
{
	return msSingleton;
}

AIPerceptor& AIPerceptor::getSingleton(void)
{
	assert(msSingleton);  return (*msSingleton);
}

AIPerceptor::AIPerceptor()
{
}

AIPerceptor::~AIPerceptor()
{
}

bool AIPerceptor::CanSee(Agent* agent1, Agent* agent2) const
{
	//check range
	float minrange = 5;
	float maxrange = agent1->getViewRange();//marked
	float dist = agent1->GetPosition().squaredDistance(agent2->GetPosition());
	//directly see if closer than minrange and moving at a certain speed
	//todo : take collision into account !
	if (dist < 1)
	{
		return true;
	}
	if (dist < minrange*minrange && agent2->GetVelocity().length() > 1.0)
	{
		return true;
	}
	if (dist < maxrange*maxrange)
	{
		//prepare line of sight triangle
		//60 degrees left & right, total 120 degrees of sight
		Quaternion q;
		q.FromAngleAxis(Radian(Math::PI / 3), Vector3::UNIT_Y);
		Vector3 direction = agent1->GetRotation() * Vector3::UNIT_Z;
		Vector3 A = agent1->GetPosition();
		Vector3 B = agent1->GetPosition() + q * (direction * maxrange);
		Vector3 C = agent1->GetPosition() + q.Inverse() * (direction * maxrange);
		Vector3 P = agent2->GetPosition();
		//check delta y
		float dy = Math::Abs(A.y - P.y);
		//check ray cast
		Vector3 add1 = agent1->GetRotation() * agent1->getEyePos();
		Vector3 add2(-add1.x, add1.y, -add1.z);
		//Vector3 add3 = agent1->GetFirePosition();
		Vector3 result = PHY->CastRay3();
		//
		return dy < 1.0 && AIUtility::PointInTriangle2D(P, A, B, C) && result == Vector3::ZERO;
	}

	return false;
}

bool AIPerceptor::CanSee(Agent* agent1, Vector3 position) const
{
	//check range
	float range = agent1->getViewRange();
	float dist = agent1->GetPosition().squaredDistance(position);
	if (dist < range*range)
	{
		//prepare line of sight triangle
		//60 degrees left & right, total 120 degrees of sight
		Quaternion q; q.FromAngleAxis(Radian(Math::PI / 3), Vector3::UNIT_Y);
		Vector3 direction = agent1->GetRotation() * Vector3::UNIT_Z;
		Vector3 A = agent1->GetPosition();
		Vector3 B = agent1->GetPosition() + q * (direction * range);
		Vector3 C = agent1->GetPosition() + q.Inverse() * (direction * range);
		Vector3 P = position;
		//check delta y
		float dy = Math::Abs(A.y - P.y);
		//
		return dy < 1.0 && AIUtility::PointInTriangle2D(P, A, B, C);
	}

	return false;
}

void AIPerceptor::UpdateKnowledge(Agent* agent) const
{
	//get a list of visibles
	AIKnowledge* ledge = agent->getKnowledge();
	std::vector<int> visible_ally;
	std::vector<int> visible_enemy;
	for (int i = 0; i < WORLD->getAgentTotal(); i++)
	{
		if (agent->getID() != WORLD->getAgent(i)->getID() && !WORLD->getAgent(i)->isDead())
		{
			//check visibility
			if (CanSee(agent, WORLD->getAgent(i)))
			{
				//ally
				if (agent->getRace() == WORLD->getAgent(i)->getRace())
				{
					visible_ally.push_back(i);
				}
				//enemy
				else
				{
					visible_enemy.push_back(i);
				}
			}
		}
	}

	//set all outdated
	for (int i = 0; i < ledge->totalAlly(); i++)
	{
		if (!ledge->allies[i].GetAgent()->isDead())
		{
			ledge->allies[i].flag = AgentInfo::IF_OUTDATED;
		}
		ledge->allies[i].visible = false;
	}

	for (int i = 0; i < ledge->totalEnemy(); i++)
	{
		if (!ledge->enemies[i].GetAgent()->isDead())
		{
			ledge->enemies[i].flag = AgentInfo::IF_OUTDATED;
		}
		ledge->enemies[i].visible = false;
	}

	//update to remove dead ones
	ledge->Update();

	//check existance, add if not, update if so
	for (size_t i = 0; i < visible_ally.size(); i++)
	{
		Agent* a = WORLD->getAgent(visible_ally[i]);
		int pos = ledge->allyExists(a->getID());
		if (pos == -1)
		{
			//add
			AgentInfo inf(a);
			ledge->allies.push_back(inf);
		}
		else
		{
			//update
			ledge->allies[pos].position = a->GetPosition();
			ledge->allies[pos].hp = a->getHP();
			ledge->allies[pos].visible = true;
			ledge->allies[pos].flag = AgentInfo::IF_UP2DATE;
		}
	}

	//add attacker if exists and not ally
	if (agent->getAttacker() != nullptr && agent->getRace() != agent->getAttacker()->getRace())
	{
		//check existance, add if not, update if so
		bool e = true;
		for (size_t i = 0; i < ledge->enemies.size(); i++)
		{
			if (ledge->enemies[i].GetAgent() == agent->getAttacker())
			{
				//update
				ledge->enemies[i].position = agent->getAttacker()->GetPosition();
				ledge->enemies[i].hp = agent->getAttacker()->getHP();
				ledge->enemies[i].visible = true;
				ledge->enemies[i].flag = AgentInfo::IF_UP2DATE;
				e = false;
				break;
			}
		}
		//add
		if (e)
		{
			AgentInfo inf(agent->getAttacker());
			ledge->enemies.push_back(inf);
		}
		agent->setAttacker(nullptr);
	}

	//check existance, add if not, update if so
	for (size_t i = 0; i < visible_enemy.size(); i++)
	{
		Agent* a = WORLD->getAgent(visible_enemy[i]);
		int pos = ledge->enemyExists(a->getID());
		if (pos == -1)
		{
			//add
			AgentInfo inf(a);
			ledge->enemies.push_back(inf);
		}
		else
		{
			//update
			ledge->enemies[pos].position = a->GetPosition();
			ledge->enemies[pos].hp = a->getHP();
			ledge->enemies[pos].visible = true;
			ledge->enemies[pos].flag = AgentInfo::IF_UP2DATE;
		}
	}

	//update cover points
	FindCovers(agent);
}

void AIPerceptor::FindCovers(Agent* agent)
{
	//clear all
	agent->getKnowledge()->coverPositions.clear();

	if (agent->getKnowledge()->totalVisibleEnemy() == 0)
	{
		return;
	}

	//add sensor grid
	int halfw = 4;
	std::vector<Vector3> tposs;
	for (int i = -halfw; i <= halfw; i++)
	{
		for (int j = -halfw; j <= halfw; j++)
		{
			if (i != 0 || j != 0)
			{
				Vector3 npos = agent->GetPosition();
				npos.x += i;
				npos.z += j;
				tposs.push_back(npos);
			}
		}
	}

	//remove inaccessible ones
	std::vector<Vector3>::iterator it = tposs.begin();
	while (it != tposs.end())
	{
		bool ok = SMPL->CanBeWalkedTo(*it, Vector3(0.05, agent->getEyePos().y / 2, 0.05));
		if (!ok)
		{
			it = tposs.erase(it);
		}
		else
		{
			++it;
		}
	}

	//leave only cover points for all visible enemy
	for (int i = 0; i < agent->getKnowledge()->totalEnemy(); i++)
	{
		if (agent->getKnowledge()->getEnemy(i).GetFlag() == AgentInfo::IF_UP2DATE)
		{
			Vector3 coverFrom = agent->getKnowledge()->getEnemy(i).GetPosition();
			it = tposs.begin();
			while (it != tposs.end())
			{
				Vector3 ret = PHY->CastRay3();
				if (ret == Vector3::ZERO)
				{
					it = tposs.erase(it);
				}
				else
				{
					++it;
				}
			}
		}
	}

	//add all
	for (size_t i = 0; i < tposs.size(); i++)
	{
		PositionalInfo info(tposs[i]);
		agent->getKnowledge()->coverPositions.push_back(info);
	}
}