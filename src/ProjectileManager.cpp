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

#include "ProjectileManager.h"
#include "World.h"
#include "Projectile.h"
#include "BlueBullet.h"
#include "BillboardSystem.h"
using namespace Ogre;

template<> ProjectileManager* Singleton<ProjectileManager>::msSingleton = nullptr;

ProjectileManager* ProjectileManager::getSingletonPtr(void)
{
	return msSingleton;
}

ProjectileManager& ProjectileManager::getSingleton(void)
{
	assert(msSingleton);  return (*msSingleton);
}

ProjectileManager::ProjectileManager() : NextBlue(0)
{
}

ProjectileManager::~ProjectileManager()
{
	Finalize();
}

void ProjectileManager::Initialize()
{
	NextBlue = 0;
	for (int i = 0; i < 100; i++)
	{
		BlueBullet* b = new BlueBullet();
		blues.push_back(b);
	}
}

void ProjectileManager::Finalize()
{
	NextBlue = 0;
	for (int i = 0; i < static_cast<int>(blues.size()); i++)
	{
		delete blues[i];
		blues[i] = nullptr;
	}
	blues.clear();
}

void ProjectileManager::Update()
{
	for (int i = 0; i < static_cast<int>(blues.size()); i++)
	{
		if (blues[i]->mAlive)
		{
			blues[i]->Update();
		}
	}
}

void ProjectileManager::Shoot(Type t, Agent* owner, Vector3 &pos, Vector3 *dir)
{
	switch (t)
	{
	case Blue:
	{
		blues[NextBlue]->mOwner = owner;
		blues[NextBlue]->mStart = pos;
		blues[NextBlue]->mPosition = pos;
		blues[NextBlue]->mDirection = *dir;
		blues[NextBlue]->mAlive = true;
		blues[NextBlue]->mBill = BBS->ShowBillboard("Bullet", pos, 1.0f);
		blues[NextBlue]->sbill = -1;
		blues[NextBlue]->ebill = -1;
		blues[NextBlue]->ibill = -1;
		NextBlue++;
		NextBlue %= blues.size();
		break;
	}
	case Red:
	{
		break;
	}
	}
}