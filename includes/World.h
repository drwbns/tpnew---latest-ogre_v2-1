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

#ifndef World_H_
#define World_H_

#include "AIConsts.h"
#include "Obstacle.h"
#include "Wall.h"
#include "Path.h"
#include "Moving.h"
#include "Agent.h"
#include "Vip.h"
#include "Gign.h"
//#include "NavMesher.h"
#include "PtfNode.h"
#include "AIUtility.h"

#include "OgreSingleton.h"

#define WORLD World::getSingletonPtr()

class World : public Ogre::Singleton<World>
{
public:
	World();
	~World();
	void Update();

	Agent * addAgent(Agent::Type type, Agent::Race race, Ogre::Vector3 *position, BaseController* ct = NULL);
	Agent* getAgent(int i) { return agents[i]; }
	int getAgentTotal() { return agents.size(); }
	void setAgentController(int i, BaseController* ct) { agents[i]->ChangeController(ct); }

	void addPath(std::vector<Ogre::Vector3*> list);
	Path* getPath(int i) { return paths[i]; }
	int getPathTotal() { return paths.size(); }

	int addObstacle(Ogre::Vector3 *position, float radius);
	void updateObstacle(int i, Ogre::Vector3 * position);
	Obstacle* getObstacle(int i) { return obstacles[i]; }
	int getObstacleTotal() { return obstacles.size(); }

	void addWall(Ogre::Vector3 *from, Ogre::Vector3 * to, bool leftnormal);
	Wall* getWall(int i) { return walls[i]; }
	int getWallTotal() { return walls.size(); }

	void setPlayer(int id) { player = id; }
	int getPlayer() { return player; }
	Moving* getPlayerAgent() { return agents[player]; }


private:
	int player;
	std::vector<Agent*> agents;
	std::vector<Path*> paths;
	std::vector<Obstacle*> obstacles;
	std::vector<Wall*> walls;
};

#endif