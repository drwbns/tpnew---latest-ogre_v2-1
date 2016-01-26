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

#include "Gign.h"

#include "OgreStringConverter.h"
#include "OgreEntity.h"
#include "OgreSceneNode.h"
#include "OgreManualObject.h"
#include "OgreBone.h"
#include "OgreSkeletonInstance.h"

#include "GraphicsSystem.h"
#include "PhysicsSystem.h"
#include "GlobalVars.h"
#include "ZZZSndSystem.h"
#include "GUtility.h"
#include "ProjectileManager.h"
#include "World.h"

#include "characterkinematic\PxCapsuleController.h"
#include "characterkinematic\PxControllerManager.h"

using namespace Ogre;
using namespace physx;

//ControllerHitReport GignChrHitReport;

Gign::Gign(int id, Race race, Vector3 position) : Agent(id, race, position, 3.0, 40, 0.4)
{
	aimMode = false;
	//important for perception
	eyePos = Vector3(0, 1.75, Radius + 0.1);

	String name = "Gign" + StringConverter::toString(id);
	mEnt = GSYS->GetSceneMgr()->createEntity(name, "gign.mesh");

	name = "GignW" + StringConverter::toString(id);
	wEnt = GSYS->GetSceneMgr()->createEntity(name, "g3.mesh");

	//attach weapon
	mEnt->attachObjectToBone("Bip01 R Hand", static_cast<MovableObject*>(wEnt));

	node->attachObject(mEnt);
	node->setScale(0.03, 0.03, 0.03);

	//show line of sight
	lineofsight->clear();
	lineofsight->begin("lineofsightMaterial", RenderOperation::OT_LINE_LIST);

	float range = viewRange / node->getScale().x;//marked
	//60 degrees left & right, total 120 degrees of sight
	Quaternion q;
	q.FromAngleAxis(Radian(Math::PI / 3), Vector3::UNIT_Y);
	Vector3 direction = GetRotation() * Vector3::UNIT_Z;
	Vector3 A = Vector3::ZERO;
	Vector3 B = A + q * (direction * range);
	Vector3 C = A + q.Inverse() * (direction * range);

	lineofsight->position(A);	lineofsight->colour(1, 0, 0);
	lineofsight->position(A + (eyePos / node->getScale().x));	lineofsight->colour(1, 0, 0);

	lineofsight->position(A);	lineofsight->colour(0, 1, 0);
	lineofsight->position(B);	lineofsight->colour(0, 1, 0);

	lineofsight->position(B);	lineofsight->colour(0, 1, 0);
	lineofsight->position(C);	lineofsight->colour(0, 1, 0);

	lineofsight->position(C);	lineofsight->colour(0, 1, 0);
	lineofsight->position(A);	lineofsight->colour(0, 1, 0);

	lineofsight->end();
	lineofsight->setVisible(true);

	idleAnimState = mEnt->getAnimationState("Idle");
	idleAnimState->setLoop(true);
	idleAnimState->setEnabled(true);
	idleAnimState->setWeight(1.0);

	runAnimState = mEnt->getAnimationState("Walk");
	runAnimState->setLoop(true);
	runAnimState->setEnabled(false);
	runAnimState->setWeight(0.0);

	shootAnimState = mEnt->getAnimationState("Shoot");
	shootAnimState->setLoop(false);
	shootAnimState->setEnabled(true);
	shootAnimState->setWeight(1.0);

	deadAnimState = mEnt->getAnimationState("Die");
	deadAnimState->setLoop(false);
	deadAnimState->setEnabled(false);
	deadAnimState->setWeight(1.0);

	/*
	mControllerManager = PxCreateControllerManager(*mScene);
	PxCapsuleControllerDesc desc;
	desc.material = mMaterial;
	desc.position = PxExtendedVec3(mBodyNode->getPosition().x, mBodyNode->getPosition().y, mBodyNode->getPosition().z);
	desc.height = mStandingSize;
	desc.radius = mControllerRadius;
	desc.slopeLimit = 0.0f;
	desc.contactOffset = 0.1f;
	desc.stepOffset = 0.02f;
	desc.reportCallback = this;
	desc.behaviorCallback = this;
	mController = mControllerManager->createController(desc);
	*/

	//phy controller
	// setup default material...
	mMaterial = PHY->getPhysics()->createMaterial(0.5f, 0.5f, 0.1f);
	if (!mMaterial)
		OgreAssert("createMaterial failed!", 1);

	PxCapsuleControllerDesc desc;
	desc.material = mMaterial;
	desc.slopeLimit = 0.0f;
	desc.contactOffset = 0.1f;
	desc.stepOffset = 0.02f;
	desc.reportCallback = this;
	desc.behaviorCallback = this;
	desc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;
	desc.stepOffset = 0.25;
	desc.callback = this;
	desc.position.x = position.x;
	desc.position.y = position.y + 2.0;
	desc.position.z = position.z;
	desc.height = 1.25;
	desc.radius = Radius;

	//desc.upDirection	= PX_Y;
	//desc.slopeLimit		= cosf(PxMath::degToRad(45.0f));
	//desc.skinWidth		= 0.10;

	phycontrol = static_cast<PxCapsuleController*>(PHY->getCManager()->createController(desc));
	if (phycontrol == nullptr)
		OgreAssert("No capsule controller!", 1);
	//phycontrol->getActor()->getShapes()[0]->setFlag(NX_SF_DISABLE_RAYCASTING, true);
	flying = false;

	// remove controller shape from scene query for standup overlap test
	PxRigidDynamic* actor = phycontrol->getActor();
	if (actor)
	{
		if (actor->getNbShapes())
		{
			PxShape* ctrlShape;
			actor->getShapes(&ctrlShape, 1);
			//ctrlShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
		}
		else
			OgreAssert("character actor has no shape", 1);
	}
	else
		OgreAssert("character could not create actor", 1);
	/*
		PxShape * hitboxShape = actor->createShape(PxBoxGeometry(1, 1, 1), *mMaterial);
		hitboxShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
		hitboxShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);

		PxFilterData fd = hitboxShape->getSimulationFilterData();
		fd.word0 |= PxU32(NO_COLLIDE);
		hitboxShape->setSimulationFilterData(fd);

		fd = hitboxShape->getQueryFilterData();
		fd.word0 |= PxU32(NO_COLLIDE);
		hitboxShape->setQueryFilterData(fd);

		characterFilterData.word0 = 1;

		characterControllerFilters.mFilterData = &characterFilterData;

		hitboxShape->setLocalPose(PxTransform(PxIdentity));
		hitboxes.push_back(hitboxShape);

		//material for hit boxes
		int index = PHY->addNewMaterial("gignbody");

		//create hitboxes
		PxActorDesc actorDesc;
		NxBodyDesc bodyDesc;
		NxBoxShapeDesc boxDesc;
		boxDesc.dimensions.set(0.10,0.15,0.10);
		boxDesc.materialIndex = index;

		actorDesc.shapes.pushBack(&boxDesc);
		actorDesc.globalPose.t = TemplateUtils::toNX(position);
		actorDesc.flags = NX_AF_DISABLE_COLLISION;

		//actor->raiseBodyFlag(NX_BF_DISABLE_GRAVITY);
		//actor->setCMassOffsetLocalPosition(PxVec3(0.05,0.075,0.05));
		hitboxes.push_back(actor);
		mActor->userData = this;

		boxDesc.dimensions.set(0.25,0.80,0.25);
		actorDesc.shapes.clear();
		actorDesc.shapes.pushBack(&boxDesc);
		actorDesc.globalPose.t = TemplateUtils::toNX(position);

		PHY->getScene()->addActor(actorDesc);
		actor->raiseBodyFlag(NX_BF_DISABLE_GRAVITY);
		actor->setCMassOffsetLocalPosition(PxVec3(0.125,0.40,0.125));
		hitboxes.push_back(actor);
		actor->userData = this;
		*/
}

Gign::~Gign()
{
	//graphics
	idleAnimState = nullptr;
	runAnimState = nullptr;
	shootAnimState = nullptr;
	deadAnimState = nullptr;
	GSYS->GetSceneMgr()->destroyEntity(mEnt);
	mEnt = nullptr;
	GSYS->GetSceneMgr()->destroyEntity(wEnt);
	wEnt = nullptr;

	//phy
	//PHY->getCManager()->releaseController(*phycontrol);
	phycontrol = nullptr;
	for (size_t i = 0; i < hitboxes.size(); i++)
	{
		//PHY->getScene()->releaseActor(*hitboxes[i]);
		hitboxes[i] = nullptr;
	}
	hitboxes.clear();
}

void Gign::Update()
{
	Agent::Update();

	if (dead)
	{
		if (!deadAnimState->hasEnded())
		{
			deadAnimState->addTime(GlobalVars::Tick);
		}
		return;
	}

	if (aimMode)
	{
		Quaternion q;
		q.FromAngleAxis(Radian(Math::PI * -0.1), Vector3::UNIT_Y);
		node->setOrientation(q * Rotation);
	}
	else
	{
		node->setOrientation(Rotation);
	}

	//move phy controller
	PxVec3 disp = TemplateUtils::toNX(Velocity * GlobalVars::Tick);
	if (flying)disp.y -= 10.0 * GlobalVars::Tick;
	PxU32 collisionFlag;

	phycontrol->move(disp, 0.01f, GlobalVars::Tick, characterControllerFilters);
	//phycontrol->move(disp, 0.01f, GlobalVars::Tick, PxControllerFilters()); // Causes collisions between hitboxes and player itself when moving

	//flying = !(collisionFlag & NXCC_COLLISION_DOWN);

	//alter pos.
	Position.x = phycontrol->getActor()->getGlobalPose().p.x;
	Position.y = phycontrol->getActor()->getGlobalPose().p.y - 1.0;
	Position.z = phycontrol->getActor()->getGlobalPose().p.z;
	/**/
	node->setPosition(Position);

	//update hitbox transform.
	//head
	Vector3 hpos = GetHeadPosition();
	//Quaternion hori = GetHeadRotation();
	//hitboxes[0]->setGlobalPosition(TemplateUtils::toNX(hpos));
	//hitboxes[0]->setGlobalOrientationQuat(TemplateUtils::toNX(hori));
	//body
	Vector3 bpos = GetBodyPosition();
	//Quaternion bori = GetBodyRotation();
	//hitboxes[1]->setGlobalPosition(TemplateUtils::toNX(bpos));
	//hitboxes[1]->setGlobalOrientationQuat(TemplateUtils::toNX(bori));

	//set anim acc. to speed
	float speed = Velocity.length();

	float idle_weight = 1.0 - (speed / MaxSpeed);
	float run_weight = (speed / MaxSpeed);

	idleAnimState->setEnabled(idle_weight > 0);
	idleAnimState->setWeight(idle_weight);
	idleAnimState->addTime(GlobalVars::Tick);

	runAnimState->setEnabled(run_weight > 0);
	runAnimState->setWeight(run_weight);
	float time2add = GlobalVars::Tick * Velocity.length() * 0.4;
	runAnimState->addTime(time2add);

	shootAnimState->setEnabled(aimMode);
	shootAnimState->setWeight(1.0);
	shootAnimState->addTime(GlobalVars::Tick);

	//step sounds
	static int scount = 0;
	if (scount == 0 && runAnimState->getTimePosition() >= 0.5 * runAnimState->getLength())
	{
		scount++;
#ifndef DISABLE_SOUND
		ZSND->PlaySound("step");
#endif
	}
	else if (scount == 1 && runAnimState->getTimePosition() >= runAnimState->getLength() - 1.5 * time2add)
	{
		scount++;
#ifndef DISABLE_SOUND
		ZSND->PlaySound("step");
#endif
	}
	else if (scount == 2)
	{
		scount = 0;
	}
}

void Gign::orderMove(float walk, float strafe)
{
	Agent::orderMove(walk, strafe);
}

void Gign::orderBrake()
{
	Agent::orderBrake();
}

void Gign::orderArrive(Vector3 pos)
{
	Agent::orderArrive(pos);
}

void Gign::orderPathFollow()
{
	Agent::orderPathFollow();
}

Vector3 *Gign::GetFirePosition()
{
	Bone* bone = wEnt->getSkeleton()->getBone("firing");
	Vector3 vec = wEnt->_getParentNodeFullTransform() * bone->_getDerivedPosition();
	return &vec;
}

Vector3 Gign::GetFireDirection(Vector3 trg_pos)
{
	if (this == WORLD->getPlayerAgent())
	{
		trg_pos = PHY->CastRay1(GSYS->GetCamera()->getPosition(), GSYS->GetCamera()->getDirection());
	}

	return (trg_pos - GetFirePosition()).normalisedCopy();
}

Vector3 Gign::GetHeadPosition() const
{
	Bone* bone = mEnt->getSkeleton()->getBone("Bone01");
	return mEnt->_getParentNodeFullTransform() * bone->_getDerivedPosition();
}

Quaternion Gign::GetHeadRotation() const
{
	Bone* bone = mEnt->getSkeleton()->getBone("Bone01");
	Quaternion q = mEnt->_getParentNodeFullTransform().extractQuaternion();
	return q * bone->_getDerivedOrientation();
}

Vector3 Gign::GetBodyPosition() const
{
	Bone* bone = mEnt->getSkeleton()->getBone("Bip01");
	return mEnt->_getParentNodeFullTransform() * bone->_getDerivedPosition();
}

Quaternion Gign::GetBodyRotation() const
{
	Bone* bone = mEnt->getSkeleton()->getBone("Bip01");
	Quaternion q = mEnt->_getParentNodeFullTransform().extractQuaternion();
	return q * bone->_getDerivedOrientation();
}

void Gign::Shoot()
{
	if (!dead && aimMode)
	{
		if (shootAnimState->hasEnded())
		{
			PJM->Shoot(ProjectileManager::Blue, this, GetFirePosition(), GetFireDirection(trg_pos));
			shootAnimState->setTimePosition(0);
#ifndef DISABLE_SOUND
			ZSND->PlaySound("fire");
#endif
			incShotsFired();
		}
	}
}

void Gign::Die()
{
	Agent::Die();

	//disable hitboxes
	//hitboxes[0]->getShapes()[0]->setFlag(NX_SF_DISABLE_RAYCASTING, true);
	//hitboxes[1]->getShapes()[0]->setFlag(NX_SF_DISABLE_RAYCASTING, true);
	//disable controller
	/*
	phycontrol->setInteraction(NXIF_INTERACTION_EXCLUDE);
	phycontrol->setCollision(false);
	*/

	headText->setVisible(false);
	deadAnimState->setEnabled(true);
	idleAnimState->setEnabled(false);
	runAnimState->setEnabled(false);
	shootAnimState->setEnabled(false);
}