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
#include "Vip.h"

#include "World.h"
#include "AIKnowledge.h"
#include "GameState.h"
#include "StateSystem.h"
#include "GraphicsSystem.h"
#include "CamController.h"
#include "zzzSndSystem.h"
#include "ProjectileManager.h"
#include "PhysicsSystem.h"

#include "GlobalVars.h"
#include "GUtility.h"

#include "OgreStringConverter.h"
#include "OgreEntity.h"
#include "OgreSceneNode.h"
#include "OgreManualObject.h"
#include "OgreBone.h"
#include "OgreSkeletonInstance.h"
#include "OgreMesh.h"
#include "OgreSubMesh.h"

#include "characterkinematic\PxCapsuleController.h"
#include "characterkinematic\PxControllerManager.h"

using namespace Ogre;
using namespace physx;

//ControllerHitReport VipChrHitReport;

// Get the mesh information for the given mesh.
// Code found in Wiki: www.ogre3d.org/wiki/index.php/RetrieveVertexData
void GetMeshInformation(const Ogre::MeshPtr mesh,
	size_t &vertex_count,
	Ogre::Vector3* &vertices,
	size_t &index_count,
	unsigned long* &indices,
	const Ogre::Vector3 &position,
	const Ogre::Quaternion &orient,
	const Ogre::Vector3 &scale)
{
	bool added_shared = false;
	size_t current_offset = 0;
	size_t shared_offset = 0;
	size_t next_offset = 0;
	size_t index_offset = 0;

	vertex_count = index_count = 0;

	// Calculate how many vertices and indices we're going to need
	for (unsigned short i = 0; i < mesh.getPointer()->getNumSubMeshes(); ++i)
	{
		Ogre::SubMesh* submesh = mesh->getSubMesh(i);

		// We only need to add the shared vertices once
		if (submesh->useSharedVertices)
		{
			if (!added_shared)
			{
				vertex_count += mesh->sharedVertexData->vertexCount;
				added_shared = true;
			}
		}
		else
		{
			vertex_count += submesh->vertexData->vertexCount;
		}

		// Add the indices
		index_count += submesh->indexData->indexCount;
	}


	// Allocate space for the vertices and indices
	vertices = new Ogre::Vector3[vertex_count];
	indices = new unsigned long[index_count];

	added_shared = false;

	// Run through the submeshes again, adding the data into the arrays
	for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
	{
		Ogre::SubMesh* submesh = mesh->getSubMesh(i);

		Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

		if ((!submesh->useSharedVertices) || (submesh->useSharedVertices && !added_shared))
		{
			if (submesh->useSharedVertices)
			{
				added_shared = true;
				shared_offset = current_offset;
			}

			const Ogre::VertexElement* posElem =
				vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

			Ogre::HardwareVertexBufferSharedPtr vbuf =
				vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

			unsigned char* vertex =
				static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

			// There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
			//  as second argument. So make it float, to avoid trouble when Ogre::Real will
			//  be comiled/typedefed as double:
			//      Ogre::Real* pReal;
			float* pReal;

			for (size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
			{
				posElem->baseVertexPointerToElement(vertex, &pReal);

				Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);

				vertices[current_offset + j] = (orient * (pt * scale)) + position;
			}

			vbuf->unlock();
			next_offset += vertex_data->vertexCount;
		}


		Ogre::IndexData* index_data = submesh->indexData;
		size_t numTris = index_data->indexCount / 3;
		Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;
		if (ibuf.isNull()) continue; // need to check if index buffer is valid (which will be not if the mesh doesn't have triangles like a pointcloud)

		bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

		unsigned long*  pLong = static_cast<unsigned long*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
		unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);


		size_t offset = (submesh->useSharedVertices) ? shared_offset : current_offset;
		size_t index_start = index_data->indexStart;
		size_t last_index = numTris * 3 + index_start;

		if (use32bitindexes)
			for (size_t k = index_start; k < last_index; ++k)
			{
				indices[index_offset++] = pLong[k] + static_cast<unsigned long>(offset);
			}

		else
			for (size_t k = index_start; k < last_index; ++k)
			{
				indices[index_offset++] = static_cast<unsigned long>(pShort[k]) +
					static_cast<unsigned long>(offset);
			}

		ibuf->unlock();
		current_offset = next_offset;
	}
}

bool RaycastFromPoint(Vector3 &result)
{
	// create the ray to test
	Ogre::Ray ray = GSYS->GetCamera()->getCameraToViewportRay(0.5, 0.5);

	RaySceneQuery * m_pray_scene_query = GSYS->GetSceneMgr()->createRayQuery(ray);

	// check we are initialised
	if (m_pray_scene_query != NULL)
	{
		// create a query object
		m_pray_scene_query->setRay(ray);

		// execute the query, returns a vector of hits
		if (m_pray_scene_query->execute().size() <= 0)
		{
			// raycast did not hit an objects bounding box
			return (false);
		}
	}
	else
	{
		//LOG_ERROR << "Cannot raycast without RaySceneQuery instance" << ENDLOG;
		return (false);
	}

	// at this point we have raycast to a series of different objects bounding boxes.
	// we need to test these different objects to see which is the first polygon hit.
	// there are some minor optimizations (distance based) that mean we wont have to
	// check all of the objects most of the time, but the worst case scenario is that
	// we need to test every triangle of every object.
	Ogre::Real closest_distance = -1.0f;
	Ogre::Vector3 closest_result;
	Ogre::RaySceneQueryResult &query_result = m_pray_scene_query->getLastResults();
	for (size_t qr_idx = 0; qr_idx < query_result.size(); qr_idx++)
	{
		// stop checking if we have found a raycast hit that is closer
		// than all remaining entities
		if ((closest_distance >= 0.0f) &&
			(closest_distance < query_result[qr_idx].distance))
		{
			break;
		}

		// only check this result if its a hit against an entity
		if ((query_result[qr_idx].movable != NULL) &&
			(query_result[qr_idx].movable->getMovableType().compare("Entity") == 0))
		{
			// get the entity to check
			Ogre::Entity *pentity = static_cast<Ogre::Entity*>(query_result[qr_idx].movable);

			// mesh data to retrieve         
			size_t vertex_count;
			size_t index_count;
			Ogre::Vector3 *vertices;
			unsigned long *indices;

			// get the mesh information
			GetMeshInformation(pentity->getMesh(), vertex_count, vertices, index_count, indices,
				pentity->getParentNode()->_getDerivedPosition(),
				pentity->getParentNode()->_getDerivedOrientation(),
				pentity->getParentNode()->_getDerivedScale());

			// test for hitting individual triangles on the mesh
			bool new_closest_found = false;
			for (int i = 0; i < static_cast<int>(index_count); i += 3)
			{
				// check for a hit against this triangle
				std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, vertices[indices[i]],
					vertices[indices[i + 1]], vertices[indices[i + 2]], true, false);

				// if it was a hit check if its the closest
				if (hit.first)
				{
					if ((closest_distance < 0.0f) ||
						(hit.second < closest_distance))
					{
						// this is the closest so far, save it off
						closest_distance = hit.second;
						new_closest_found = true;
					}
				}
			}

			// free the verticies and indicies memory
			delete[] vertices;
			delete[] indices;

			// if we found a new closest raycast for this object, update the
			// closest_result before moving on to the next object.
			if (new_closest_found)
			{
				closest_result = ray.getPoint(closest_distance);
			}
		}
	}

	// return the result
	if (closest_distance >= 0.0f)
	{
		// raycast success
		result = closest_result;
		return (true);
	}
	else
	{
		// raycast failed
		return (false);
	}
}

Vip::Vip(int id, Race race, Vector3 position) : Agent(id, race, position, 3, 40, 0.4)
{
	aimMode = false;
	//important for perception
	eyePos = Vector3(0, 1.75, Radius+0.1);

	String name = "Vip" + StringConverter::toString(id);
	mEnt = GSYS->GetSceneMgr()->createEntity(name,"vip.mesh");

	name = "VipW" + StringConverter::toString(id);
	wEnt = GSYS->GetSceneMgr()->createEntity(name,"ak47.mesh");

	//attach weapon
	mEnt->attachObjectToBone("Bip01 R Hand",(MovableObject*)wEnt);

	node->attachObject(mEnt);
	node->setScale(0.03,0.03,0.03);

	//show line of sight
	lineofsight->clear();
	lineofsight->begin("lineofsightMaterial", Ogre::RenderOperation::OT_LINE_LIST);

	float range = viewRange / node->getScale().x;//marked
	//60 degrees left & right, total 120 degrees of sight
	Quaternion q;
	q.FromAngleAxis(Radian(Math::PI/3), Vector3::UNIT_Y);
	Vector3 direction = GetRotation() * Vector3::UNIT_Z;
	Vector3 A = Vector3::ZERO;
	Vector3 B = A + q * (direction * range);
	Vector3 C = A + q.Inverse() * (direction * range);

	lineofsight->position(A);	lineofsight->colour(1,0,0);
	lineofsight->position(A + (eyePos/node->getScale().x));	lineofsight->colour(1,0,0);

	lineofsight->position(A);	lineofsight->colour(0,1,0);
	lineofsight->position(B);	lineofsight->colour(0,1,0);

	lineofsight->position(B);	lineofsight->colour(0,1,0);
	lineofsight->position(C);	lineofsight->colour(0,1,0);

	lineofsight->position(C);	lineofsight->colour(0,1,0);
	lineofsight->position(A);	lineofsight->colour(0,1,0);

	lineofsight->end();
	lineofsight->setVisible(true);

	
	idleAnimState = mEnt->getAnimationState("Idle");
	idleAnimState->setLoop(true);
	idleAnimState->setEnabled(true);
	idleAnimState->setWeight(0.9);

	aimAnimState = mEnt->getAnimationState("Aim");
	aimAnimState->setLoop(true);
	aimAnimState->setEnabled(true);
	aimAnimState->setWeight(1.0);

	runAnimState = mEnt->getAnimationState("Walk");
	runAnimState->setLoop(true);
	runAnimState->setEnabled(true);
	runAnimState->setWeight(0.9);

	shootAnimState = mEnt->getAnimationState("Shoot");
	shootAnimState->setLoop(false);
	shootAnimState->setEnabled(true);
	shootAnimState->setWeight(0.9);

	deadAnimState = mEnt->getAnimationState("Die");
	deadAnimState->setLoop(false);
	deadAnimState->setEnabled(false);
	deadAnimState->setWeight(1.0);
	
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
	if (phycontrol == NULL)
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
			ctrlShape->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, false);
		}
		else
			OgreAssert("character actor has no shape", 1);
	}
	else
		OgreAssert("character could not create actor", 1);
	//phy controller
	/*
	PxCapsuleControllerDesc desc;
	desc.position.x		= position.x;
	desc.position.y		= position.y + 2.0;
	desc.position.z		= position.z;
	desc.height         = 1.25;
	desc.radius	        = Radius;
	desc.upDirection	= NX_Y;
	desc.slopeLimit		= cosf(PxMath::degToRad(45.0f));
	desc.skinWidth		= 0.10;
	desc.stepOffset		= 0.25;
	desc.callback		= &VipChrHitReport;
	phycontrol = (PxCapsuleController*)PHY->getCManager()->createController(PHY->getScene(), desc);
	phycontrol->getActor()->getShapes()[0]->setFlag(NX_SF_DISABLE_RAYCASTING, true);
	flying = false;

	//material for hit boxes
	int index = PHY->addNewMaterial("vipbody");

	PxScene * myScene = PHY->getScene();
	*/
	//create hitboxes
	//PxRigidDynamic * hitbox = PHY->getPhysics()->createRigidDynamic(PxTransform(TemplateUtils::toNX(GetPosition()))); // Possibly wrong

	if (true); // uncomment below for hitbox creation
	

	
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
	

	//actor->userData = this;
	

	//PxActorDesc actorDesc;
	//PxBodyDesc bodyDesc;
	

	//boxDesc.dimensions.set(0.10,0.15,0.10);
	//boxDesc.materialIndex = index;
	//actorDesc.shapes.pushBack(&boxDesc);
	//actorDesc.globalPose.t = TemplateUtils::toNX(position);
	//actorDesc.flags = NX_AF_DISABLE_COLLISION;
	//PxActor* actor = PHY->getScene()->createActor(actorDesc);
	//actor->raiseBodyFlag(NX_BF_DISABLE_GRAVITY);
	//actor->setCMassOffsetLocalPosition(PxVec3(0.05,0.075,0.05));


	//boxDesc.dimensions.set(0.25,0.80,0.25);
	//actorDesc.shapes.clear();
	//actorDesc.shapes.pushBack(&boxDesc);
	//actorDesc.globalPose.t = TemplateUtils::toNX(position);
	//actor = PHY->getScene()->createActor(actorDesc);
	//actor->raiseBodyFlag(NX_BF_DISABLE_GRAVITY);
	//actor->setCMassOffsetLocalPosition(PxVec3(0.125,0.40,0.125));
	//hitboxes.push_back(actor);
	//actor->userData = this;

	
	
}

Vip::~Vip()
{
	//graphics
	idleAnimState = NULL;
	runAnimState = NULL;
	shootAnimState = NULL;
	deadAnimState = NULL;
	GSYS->GetSceneMgr()->destroyEntity(mEnt);
	mEnt = NULL;
	GSYS->GetSceneMgr()->destroyEntity(wEnt);
	wEnt = NULL;

	//phy
	/*
	PHY->getCManager()->releaseController(*phycontrol);
	phycontrol = NULL;
	for (size_t i=0;i<hitboxes.size();i++)
	{
		PHY->getScene()->releaseActor(*hitboxes[i]);
		hitboxes[i] = NULL;
	}
	*/
	hitboxes.clear();
}

void Vip::Update()
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
	if (flying)
		disp.y -= 10.0 * GlobalVars::Tick;
	//PxU32 collisionFlag;

	phycontrol->move(disp, 0.01f, GlobalVars::Tick, characterControllerFilters);
	//phycontrol->move(disp, 0.01f, GlobalVars::Tick, PxControllerFilters()); // Causes collisions between hitboxes and player itself when moving

	//flying = !(collisionFlag & NXCC_COLLISION_DOWN);

	//alter pos.
	Position.x = phycontrol->getActor()->getGlobalPose().p.x;
	Position.y = phycontrol->getActor()->getGlobalPose().p.y - 1.0;
	Position.z = phycontrol->getActor()->getGlobalPose().p.z;
	/**/
	node->setPosition(Position);

	//move phy controller
	/*
	PxVec3 disp = TemplateUtils::toNX(Velocity * GlobalVars::Tick);
	if (flying)disp.y -= 10.0 * GlobalVars::Tick;
	PxU32 collisionFlag;
	phycontrol->move(disp,COLLIDABLE_MASK,0.001,collisionFlag);
	flying = !(collisionFlag & NXCC_COLLISION_DOWN);

	//alter pos.
	mPosition.x = phycontrol->getActor()->getGlobalPosition().x;
	mPosition.y = phycontrol->getActor()->getGlobalPosition().y - 1.0;
	mPosition.z = phycontrol->getActor()->getGlobalPosition().z;

	node->setPosition(mPosition);

	//update hitbox transform.
	//head
	Vector3 hpos = GetHeadPosition();hpos.y += 0.05;
	//Quaternion hori = GetHeadRotation();
	hitboxes[0]->setGlobalPosition(TemplateUtils::toNX(hpos));
	//hitboxes[0]->setGlobalOrientationQuat(TemplateUtils::toNX(hori));
	//body
	Vector3 bpos = GetBodyPosition();bpos.y -= 0.20;
	//Quaternion bori = GetBodyRotation();
	hitboxes[1]->setGlobalPosition(TemplateUtils::toNX(bpos));
	//hitboxes[1]->setGlobalOrientationQuat(TemplateUtils::toNX(bori));
	*/
	//set anim acc. to speed
	float speed = Velocity.length();

	float idle_weight = 1.0 - (speed / MaxSpeed);
	float run_weight = (speed / MaxSpeed);

	float time2add = GlobalVars::Tick * Velocity.length() * 0.4;
	
	idleAnimState->setEnabled(idle_weight > 0);
	idleAnimState->setWeight(idle_weight);

	runAnimState->setEnabled(run_weight > 0);
	runAnimState->setWeight(run_weight);	

	shootAnimState->setEnabled(aimMode);
	shootAnimState->setWeight(1.0);
	
	idleAnimState->addTime(GlobalVars::Tick);

	aimAnimState->addTime(GlobalVars::Tick); // Time position isnt moving, check shootAnimState for how to play animation
	
	runAnimState->addTime(time2add); // Time position isnt moving, check shootAnimState for how to play animation


	shootAnimState->addTime(GlobalVars::Tick*5);
	
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

	//show cover locs
	/*
	coverlocs->clear();
	coverlocs->begin("lineofsightMaterial", Ogre::RenderOperation::OT_LINE_LIST);

	for (int i=0;i<knowledge->totalCover();i++)
	{
		coverlocs->position(knowledge->getCover(i).GetPosition());
		coverlocs->colour(0,0,1);
		coverlocs->position(knowledge->getCover(i).GetPosition() + Vector3(0,0.5,0));
		coverlocs->colour(0,0,1);
	}

	coverlocs->end();
	*/
}

void Vip::orderMove(float walk, float strafe)
{
	Agent::orderMove(walk, strafe);
}

void Vip::orderBrake()
{
	Agent::orderBrake();
}

void Vip::orderArrive(Ogre::Vector3 pos)
{
	Agent::orderArrive(pos);
}

void Vip::orderPathFollow()
{
	Agent::orderPathFollow();
}

Vector3 Vip::GetFirePosition()
{ 
	Bone* bone = wEnt->getSkeleton()->getBone("firing");
	return wEnt->_getParentNodeFullTransform() * bone->_getDerivedPosition();
}

Vector3 Vip::GetFireDirection(Vector3 &trg_pos)
{
	if (this == WORLD->getPlayerAgent())
	{
		trg_pos = PHY->CastRay1(GSYS->GetCamera()->getPosition(), GSYS->GetCamera()->getDirection());
	}
	return (trg_pos - GetFirePosition()).normalisedCopy(); 
}

Ogre::Vector3 Vip::GetHeadPosition()
{
	Bone* bone = mEnt->getSkeleton()->getBone("Bone01");
	return mEnt->_getParentNodeFullTransform() * bone->_getDerivedPosition();
}

Ogre::Quaternion Vip::GetHeadRotation()
{
	Bone* bone = mEnt->getSkeleton()->getBone("Bone01");
	Quaternion q = mEnt->_getParentNodeFullTransform().extractQuaternion();
	return q * bone->_getDerivedOrientation();
}

Ogre::Vector3 Vip::GetBodyPosition()
{
	Bone* bone = mEnt->getSkeleton()->getBone("Bip01");
	return mEnt->_getParentNodeFullTransform() * bone->_getDerivedPosition();
}

Ogre::Quaternion Vip::GetBodyRotation()
{
	Bone* bone = mEnt->getSkeleton()->getBone("Bip01");
	Quaternion q = mEnt->_getParentNodeFullTransform().extractQuaternion();
	return q * bone->_getDerivedOrientation();
}

void Vip::Shoot(bool first, Vector3 &trg_pos)
{
	if (!dead && aimMode)
	{
		if (shootAnimState->hasEnded())
		{
			/*
			Ray myRay = GSYS->GetCamera()->getCameraToViewportRay(0.5, 0.5);
			RaySceneQuery * rayQuery = GSYS->GetSceneMgr()->createRayQuery(myRay);
			rayQuery->execute();
			rayQuery->setSortByDistance(true);
			Vector3 vec = rayQuery->getLastResults()[0].worldFragment->singleIntersection;
			Vector3 dir = vec - GetFirePosition();
			*/
			Vector3 result;
			bool bResult = RaycastFromPoint(result);
			if (bResult) {
				Vector3 dir = result - GetFirePosition();
				PJM->Shoot(ProjectileManager::Blue, this, GetFirePosition(), dir.normalisedCopy());
				shootAnimState->setTimePosition(0);
#ifndef DISABLE_SOUND
				ZSND->PlaySound("fire");
#endif
				incShotsFired();
			}
		}
	}
}

void Vip::Die()
{
	Agent::Die();

	//disable hitboxes

	/*
	hitboxes[0]->getShapes()[0]->setFlag(NX_SF_DISABLE_RAYCASTING, true);
	hitboxes[1]->getShapes()[0]->setFlag(NX_SF_DISABLE_RAYCASTING, true);
	//disable controller
	phycontrol->setInteraction(NXIF_INTERACTION_EXCLUDE);
	phycontrol->setCollision(false);
	*/

	headText->setVisible(false);
	deadAnimState->setEnabled(true);
	idleAnimState->setEnabled(false);
	runAnimState->setEnabled(false);
	shootAnimState->setEnabled(false);
}