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

#include "stdafx.h"
#include "Wall.h"
#include "GraphicsSystem.h"
using namespace Ogre;

Wall::Wall(Ogre::Vector3 from, Ogre::Vector3 to, bool leftnormal)
{
	this->From = from;
	this->To = to;

	//find normal
	this->Normal = (to - from);
	Quaternion q;
	if (leftnormal)
	{
		q.FromAngleAxis(Radian(Math::fDeg2Rad * 90), Vector3::UNIT_Y);
	}
	else
	{
		q.FromAngleAxis(Radian(Math::fDeg2Rad * -90), Vector3::UNIT_Y);
	}
	this->Normal = q * this->Normal;
	this->Normal.y = 0;
	this->Normal.normalise();

	//mark
	mark = GSYS->GetSceneMgr()->createManualObject();
	mark->setCastShadows(false);
	MaterialPtr markMaterial = MaterialManager::getSingleton().create("markMaterial","General");
	markMaterial->setReceiveShadows(false);
	markMaterial->getTechnique(0)->setLightingEnabled(false);
	mark->clear();

	mark->begin("markMaterial", Ogre::RenderOperation::OT_LINE_LIST);

	mark->position(from);	mark->colour(0,0,1);
	mark->position(to);		mark->colour(0,0,1);

	mark->position(from.midPoint(to));			mark->colour(1,1,1);
	mark->position(from.midPoint(to)+Normal);	mark->colour(1,1,1);

	mark->end();

	GSYS->GetSceneMgr()->getRootSceneNode()->attachObject(mark);
}

Wall::~Wall()
{
	GSYS->GetSceneMgr()->getRootSceneNode()->detachObject(mark);
	GSYS->GetSceneMgr()->destroyManualObject(mark);
	mark = NULL;
}