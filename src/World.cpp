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


#include "World.h"

World::World()
{
	player = -1;
}

World::~World()
{

	//agents
	for (int i=0;i<(int)agents.size();i++)
	{
		delete agents[i];
		agents[i] = NULL;
	}
	agents.clear();

	//obstacles
	for (int i=0;i<(int)obstacles.size();i++)
	{
		delete obstacles[i];
		obstacles[i] = NULL;
	}
	obstacles.clear();

	//walls
	for (int i=0;i<(int)walls.size();i++)
	{
		delete walls[i];
		walls[i] = NULL;
	}
	walls.clear();

	//paths
	for (int i=0;i<(int)paths.size();i++)
	{
		delete paths[i];
		paths[i] = NULL;
	}
	paths.clear();
}

void World::Update()
{
	std::vector<Agent*>::iterator it = agents.begin();
	while (it != agents.end())
	{
		Agent* a = (*it);
		a->Update();
		if (a->isDead() && a->getCwaitTime() < 0)
		{
			delete a;
			a = NULL;
			it = agents.erase(it);
		}
		else
		{
			it++;
		}
	}

	
}

void World::addAgent(Agent::Type type, Agent::Race race, Ogre::Vector3 position, BaseController* ct)
{
	if (type == Agent::AT_VIP)
	{
		Vip* a = new Vip(agents.size(), race, position);
		a->ChangeController(ct);
		agents.push_back(a);
	}
	else if (type == Agent::AT_GIGN)
	{
		Gign* a = new Gign(agents.size(), race, position);
		a->ChangeController(ct);
		agents.push_back(a);
	}
}

void World::addPath(std::vector<Ogre::Vector3> list)
{
	Path* p = new Path();
	paths.push_back(p);
	//populate path
	for (size_t i=0;i<list.size();i++)
	{
		p->AddNode(list[i]);
	}
}

int World::addObstacle(Ogre::Vector3 position, float radius)
{
	Obstacle* o = new Obstacle(position, radius);
	obstacles.push_back(o);
	return obstacles.size()-1;
}

void World::updateObstacle(int i, Ogre::Vector3 position)
{
	getObstacle(i)->SetPos(position);
	getObstacle(i)->resetDebug();
}

void World::addWall(Ogre::Vector3 from, Ogre::Vector3 to, bool leftnormal)
{
	Wall* w = new Wall(from, to, leftnormal);
	walls.push_back(w);
}