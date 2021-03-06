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

#ifndef BillboardSystem_H_
#define BillboardSystem_H_

#include "prereqs.h"
#include "OgrePrerequisites.h"

#include "OgreSingleton.h"
#include "OgreMemoryAllocatorConfig.h"

#define BBS BillboardSystem::getSingletonPtr()

class BillboardSystem : Ogre::Singleton<BillboardSystem>, public Ogre::GeneralAllocatedObject
{
public:
	BillboardSystem();
	~BillboardSystem();
	static BillboardSystem& getSingleton();
	static BillboardSystem* getSingletonPtr();

	void Initialize();
	void Finalize();
	void Update();
	int ShowBillboard(Ogre::String name, Ogre::Vector3 pos, float time);
	void UpdateBillboard(Ogre::String name, int id, Ogre::Vector3 pos);
	void HideBillboard(Ogre::String name, int id);

private:
	std::map<Ogre::String, BillboardNodeSet*> sets;
	Ogre::SceneNode* bnode;
};

#endif