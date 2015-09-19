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


#include "BillboardSystem.h"
#include "BillboardNodeSet.h"
#include "GraphicsSystem.h"

#include "GlobalVars.h"

#include "OgreSceneNode.h"

using namespace Ogre;

BillboardNodeSet::BillboardNodeSet(int sz)
{
	set = NULL;
	counter = 0;
	size = sz;
}

BillboardNodeSet::~BillboardNodeSet()
{
}

void BillboardNodeSet::Update()
{
	//update buffer
	for (size_t i=0;i<buffer.size();i++)
	{
		if (buffer[i]->bill != NULL)
		{
			if (buffer[i]->liveTime <= 0)
			{
				set->removeBillboard(buffer[i]->bill);
				buffer[i]->bill = NULL;
			}
			else
			{
				buffer[i]->liveTime -= GlobalVars::Tick;
			}
		}
	}
}

void BillboardNodeSet::Finalize(Ogre::SceneNode* parent)
{
	//clear buffer
	for (size_t i=0;i<buffer.size();i++)
	{
		if (buffer[i]->bill != NULL)
		{
			set->removeBillboard(buffer[i]->bill);
		}
		delete buffer[i];
		buffer[i] = NULL;
	}
	buffer.clear();

	//detach & remove set
	parent->detachObject(set);
	GSYS->GetSceneMgr()->destroyBillboardSet(set);
	set = NULL;
}