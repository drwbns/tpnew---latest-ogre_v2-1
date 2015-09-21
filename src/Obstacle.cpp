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


#include "Obstacle.h"

#include "GraphicsSystem.h"

#include "OgreManualObject.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include "OgreMaterialManager.h"
#include "OgreTechnique.h"

using namespace Ogre;

Obstacle::Obstacle(Ogre::Vector3 position, float radius)
{
	this->Position = position;
	this->Radius = radius;

	//mark
	mark = GSYS->GetSceneMgr()->createManualObject();
	mark->setCastShadows(false);
	MaterialPtr markMaterial = MaterialManager::getSingleton().create("markMaterial","General");
	markMaterial->setReceiveShadows(false);
	markMaterial->getTechnique(0)->setLightingEnabled(false);
	GSYS->GetSceneMgr()->getRootSceneNode()->attachObject(mark);
	resetDebug();
}

Obstacle::~Obstacle()
{
	GSYS->GetSceneMgr()->getRootSceneNode()->detachObject(mark);
	GSYS->GetSceneMgr()->destroyManualObject(mark);
	mark = NULL;
}

void Obstacle::resetDebug()
{
	mark->clear();
	mark->begin("markMaterial", Ogre::RenderOperation::OT_LINE_LIST);

	int step = 16;
	float degree = 0;
	for (int i=0;i<step;i++)
	{
		degree = i * (360.0 / step);
		float x = Math::Sin(degree * Math::fDeg2Rad);
		float y = Math::Cos(degree * Math::fDeg2Rad);
		mark->position(Position.x + x * Radius, 0.5, Position.z + y * Radius);
		mark->colour(0,0,1);

		degree = (i+1) * (360.0 / step);
		x = Math::Sin(degree * Math::fDeg2Rad);
		y = Math::Cos(degree * Math::fDeg2Rad);
		mark->position(Position.x + x * Radius, 0.5, Position.z + y * Radius);	
		mark->colour(0,0,1);
	}

	mark->end();
}