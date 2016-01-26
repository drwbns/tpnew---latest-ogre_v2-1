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

#include "AIKnowledge.h"
using namespace Ogre;

AIKnowledge::AIKnowledge(Agent* owner) : owner(owner)
{
}

AIKnowledge::~AIKnowledge()
{
	owner = nullptr;
	allies.clear();
	enemies.clear();
	coverPositions.clear();
}

void AIKnowledge::Update()
{
	//remove dead flagged
	std::vector<AgentInfo>::iterator it = allies.begin();
	while (it != allies.end())
	{
		if ((*it).GetAgent()->isDead() || (*it).GetFlag() == AgentInfo::IF_LOST)
		{
			it = allies.erase(it);
		}
		else ++it;
	}

	it = enemies.begin();
	while (it != enemies.end())
	{
		if ((*it).GetAgent()->isDead() || (*it).GetFlag() == AgentInfo::IF_LOST)
		{
			it = enemies.erase(it);
		}
		else ++it;
	}
}

int AIKnowledge::allyExists(int id)
{
	for (size_t i = 0; i < allies.size(); i++)
	{
		if (id == allies[i].GetID())
		{
			return i;
		}
	}
	return -1;
}

int AIKnowledge::enemyExists(int id)
{
	for (size_t i = 0; i < enemies.size(); i++)
	{
		if (id == enemies[i].GetID())
		{
			return i;
		}
	}
	return -1;
}

int AIKnowledge::totalVisibleAlly()
{
	int ret = 0;
	for (size_t i = 0; i < allies.size(); i++)
	{
		if (allies[i].IsVisible())
		{
			ret++;
		}
	}
	return ret;
}

int AIKnowledge::totalVisibleEnemy()
{
	int ret = 0;
	for (size_t i = 0; i < enemies.size(); i++)
	{
		if (enemies[i].IsVisible())
		{
			ret++;
		}
	}
	return ret;
}

int AIKnowledge::totalVisibleCover() const
{
	return coverPositions.size();
}

int AIKnowledge::getClosestEnemy()
{
	float max = 999999;
	int id = -1;
	for (size_t i = 0; i < enemies.size(); i++)
	{
		float d = owner->GetPosition().squaredDistance(enemies[i].GetPosition());
		if (d < max)
		{
			max = d;
			id = i;
		}
	}
	return id;
}

int AIKnowledge::getClosestVisibleEnemy()
{
	float max = 999999;
	int id = -1;
	for (size_t i = 0; i < enemies.size(); i++)
	{
		if (enemies[i].IsVisible())
		{
			float d = owner->GetPosition().squaredDistance(enemies[i].GetPosition());
			if (d < max)
			{
				max = d;
				id = i;
			}
		}
	}
	return id;
}

int AIKnowledge::getClosestCover()
{
	float max = 999999;
	int id = -1;
	for (size_t i = 0; i < coverPositions.size(); i++)
	{
		float d = owner->GetPosition().squaredDistance(coverPositions[i].GetPosition());
		if (d < max)
		{
			max = d;
			id = i;
		}
	}
	return id;
}