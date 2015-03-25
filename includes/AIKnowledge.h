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

#ifndef AIKnowledge_H_
#define AIKnowledge_H_

#include "Moving.h"
#include "Agent.h"
#include "AgentInfo.h"
#include "PositionalInfo.h"

class AIKnowledge
{
	friend class AIPerceptor;

public:
	AIKnowledge(Agent* owner);
	~AIKnowledge();
	void Update();

	Agent* getOwner() { return owner; }
	AgentInfo& getAlly(int i) { return allies[i]; }
	AgentInfo& getEnemy(int i) { return enemies[i]; }
	int allyExists(int id);
	int enemyExists(int id);
	PositionalInfo getCover(int i) { return coverPositions[i]; }
	int totalAlly() { return (int)allies.size(); }
	int totalEnemy() { return (int)enemies.size(); }
	int totalCover() { return (int)coverPositions.size(); }
	int totalVisibleAlly();
	int totalVisibleEnemy();
	int totalVisibleCover();
	int getClosestEnemy();
	int getClosestVisibleEnemy();
	int getClosestCover();
	void setEnemyFlag(int i, AgentInfo::Flag f) { enemies[i].SetFlag(f); }

private:
	Agent* owner;
	std::vector<AgentInfo> allies;
	std::vector<AgentInfo> enemies;
	std::vector<PositionalInfo> coverPositions;
};

#endif