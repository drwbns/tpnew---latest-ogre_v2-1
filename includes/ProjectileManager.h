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

#ifndef ProjectileManager_H_
#define ProjectileManager_H_

#include "prereqs.h"

#include "BlueBullet.h"

#define PJM ProjectileManager::getSingletonPtr()

#define SBLUETIME 0.1f
#define EBLUETIME 0.1f
#define IMPACTTIME 10.0f

#include "OgreSingleton.h"

class ProjectileManager : Ogre::Singleton<ProjectileManager>, public Ogre::GeneralAllocatedObject
{
public:
	enum Type
	{
		Blue,
		Red
	};

	ProjectileManager();
	~ProjectileManager();
	static ProjectileManager& getSingleton();
	static ProjectileManager* getSingletonPtr();

	void Initialize();
	void Finalize();
	void Update();
	void Shoot(Type t, Agent* owner, Ogre::Vector3 &pos, Ogre::Vector3 *dir);

private:
	std::vector<BlueBullet*> blues;
	int NextBlue;
};

#endif