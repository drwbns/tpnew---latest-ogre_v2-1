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

#include "Path.h"

#include "GraphicsSystem.h"

#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include "OgreTechnique.h"
#include "OgreMaterialManager.h"
#include "OgreManualObject.h"

using namespace Ogre;

Path::Path() : numPolys(0), numPaths(0), numStraightPaths(0)
{
	//mark
	mark = GSYS->GetSceneMgr()->createManualObject();
	mark->setCastShadows(false);
	MaterialPtr markMaterial = MaterialManager::getSingleton().create("markMaterial", "General");
	markMaterial->setReceiveShadows(false);
	markMaterial->getTechnique(0)->setLightingEnabled(false);
	mark->clear();
	GSYS->GetSceneMgr()->getRootSceneNode()->attachObject(mark);
}

Path::~Path()
{
	GSYS->GetSceneMgr()->getRootSceneNode()->detachObject(mark);
	GSYS->GetSceneMgr()->destroyManualObject(mark);
	mark = nullptr;

	mNodes.clear();
}

void Path::AddNode(PtfNode node, bool shift)
{
	if (!shift)
	{
		mNodes.push_back(node);
	}
	else
	{
		mNodes.insert(mNodes.begin(), node);
	}
}

void Path::Clear()
{
	mNodes.clear();
	mark->clear();
}

void Path::ShowDebug(bool b)
{
	mark->clear();

	if (b)
	{
		mark->begin("markMaterial", RenderOperation::OT_LINE_LIST);
		for (int i = 0; i < GetLength() - 1; i++)
		{
			//node to next node
			mark->position(*mNodes[i].getPos());
			mark->colour(1, 1, 1);
			mark->position(*mNodes[i + 1].getPos());
			mark->colour(1, 1, 1);

			//node to upper
			mark->position(*mNodes[i].getPos());
			mark->colour(1, 1, 0);
			mark->position(mNodes[i].getPos()->x, mNodes[i].getPos()->y + 0.75, mNodes[i].getPos()->z);
			mark->colour(1, 1, 0);
		}
		mark->end();
	}
}