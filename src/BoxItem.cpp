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


#include "BoxItem.h"
#include "GraphicsSystem.h"
#include "PhysicsSystem.h"
#include "World.h"

#include "OgreEntity.h"
#include "OgreSceneNode.h"
#include "OgreMesh.h"

#include "PxActor.h"


using namespace physx;
using namespace Ogre;

BoxItem::BoxItem(int id, String mat, Vector3 pos, Vector3 scl, float mass)
{
	//model
	std::string name = "box" + StringConverter::toString(id);
	ent = GSYS->GetSceneMgr()->createEntity(name, SceneManager::PT_CUBE);
	ent->setMaterialName(mat);
	node = GSYS->GetSceneMgr()->getRootSceneNode()->createChildSceneNode();
	node->attachObject(ent);
	node->setPosition(pos);
	node->setScale(scl);

	//phy
	/*
	PxActorDesc actorDesc;
	NxBodyDesc bodyDesc;
	NxBoxShapeDesc boxDesc;
	*/
	Ogre::AxisAlignedBox b1 = ent->getMesh()->getBounds();
	Ogre::Real szx1 = abs(b1.getMaximum().x-b1.getMinimum().x);
	Ogre::Real szy1 = abs(b1.getMaximum().y-b1.getMinimum().y);
	Ogre::Real szz1 = abs(b1.getMaximum().z-b1.getMinimum().z);
	/*
	boxDesc.dimensions.set((Ogre::Real)(scl.x*szx1/2),
			               (Ogre::Real)(scl.y*szy1/2),
			               (Ogre::Real)(scl.z*szz1/2));

	//phy material
	int index = PHY->addNewMaterial(mat);
	boxDesc.materialIndex = index;

	actorDesc.shapes.pushBack(&boxDesc);
	if (mass > 0)actorDesc.body = &bodyDesc;
	actorDesc.density = 1;
	actorDesc.globalPose.t = TemplateUtils::toNX(pos);
	actor = PHY->getScene()->createActor(actorDesc);
	actor->updateMassFromShapes(0, mass);
	actor->userData = NULL;
	PHY->SetActorCollisionGroup(actor, GROUP_COLLIDABLE_PUSHABLE);

	//report contact
	actor->setContactReportFlags(NX_NOTIFY_ON_START_TOUCH_FORCE_THRESHOLD | NX_NOTIFY_ON_TOUCH_FORCE_THRESHOLD | NX_NOTIFY_ON_END_TOUCH_FORCE_THRESHOLD);    
	actor->setContactReportThreshold(10);
	*/
	//add obstacle
	obstacle = WORLD->addObstacle(Vector3(pos.x,pos.y, pos.z), scl.x*szx1/2*Math::Sqrt(2));
}

BoxItem::~BoxItem()
{
	//node
	node->detachAllObjects();
	node->getParentSceneNode()->removeAndDestroyChild(node->getName());
	node = NULL;
	//ent
	GSYS->GetSceneMgr()->destroyEntity(ent);
	ent = NULL;
	//phy
//	PHY->getScene()->releaseActor(*actor);
	actor = NULL;
}

void BoxItem::Update()
{
	/*
	if (actor->isDynamic())
	{
		PxVec3 pos = actor->getGlobalPosition();
		PxQuat rot = actor->getGlobalOrientationQuat();
		node->setPosition(pos.x,pos.y,pos.z);
		node->setOrientation(rot.w,rot.x,rot.y,rot.z);
		if (!actor->isSleeping())
		{
			WORLD->updateObstacle(obstacle, Vector3(pos.x,pos.y, pos.z));
		}
	}
	*/
}