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

#include "PhysicsSystem.h"

#include "GlobalVars.h"
#include "GUtility.h"

using namespace Ogre;

template<> PhysicsSystem* Ogre::Singleton<PhysicsSystem>::msSingleton = nullptr;

PhysicsSystem* PhysicsSystem::getSingletonPtr(void)
{
	return msSingleton;
}

PhysicsSystem& PhysicsSystem::getSingleton(void)
{
	assert(msSingleton);  return (*msSingleton);
}

PhysicsSystem::PhysicsSystem() : /*gPhysicsSDK(NULL),*/ mVisualDebugger(nullptr), mVisualDebuggerNode(nullptr), mFoundation(nullptr), mProfileZoneManager(nullptr),
mCpuDispatcher(nullptr), mPhysics(nullptr), gConnection(nullptr), mScene(nullptr), mCooking(nullptr), mControllerManager(nullptr), mController(nullptr), mMaterial(nullptr)
{
	mDebuggerView = false;
}

PhysicsSystem::~PhysicsSystem()
{
	Finalize();
}

void PhysicsSystem::Initialize()
{
	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
	if (mFoundation == nullptr)
		OgreAssert("PxCreateFoundation failed!", 1);

	bool recordMemoryAllocations = true;
	mProfileZoneManager = &PxProfileZoneManager::createProfileZoneManager(mFoundation);
	if (mProfileZoneManager == nullptr)
		OgreAssert("PxProfileZoneManager::createProfileZoneManager failed!", 1);

	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation,
		PxTolerancesScale(), recordMemoryAllocations, mProfileZoneManager);
	if (mPhysics == nullptr)
		OgreAssert("PxCreatePhysics failed!", 1);

	if (mPhysics->getPvdConnectionManager())
	{
		gConnection = PxVisualDebuggerExt::createConnection(mPhysics->getPvdConnectionManager(), PVD_HOST, 5425, 10);
	}

	//sdk

	//@TODO:
	// @TODO // Outdated, marked for removal //gPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);

	; // Start Section Outdated, marked for removal

	//scene
	PxSceneDesc sceneDesc = PxSceneDesc(PxTolerancesScale());
	sceneDesc.gravity = PxVec3(0, -10, 0);

	if (!sceneDesc.cpuDispatcher)
	{
		PxU32 numWorkers = 1;
		mCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
		if (!mCpuDispatcher)
			OgreAssert("PxDefaultCpuDispatcherCreate failed!", 1);
		sceneDesc.cpuDispatcher = mCpuDispatcher;
	}

	if (!sceneDesc.filterShader)
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;

	/*
#ifdef PX_WINDOWS
	if (!sceneDesc.gpuDispatcher && mCudaContextManager)
	{
		sceneDesc.gpuDispatcher = mCudaContextManager->getGpuDispatcher();
	}
#endif
*/

	mScene = mPhysics->createScene(sceneDesc);
	if (!mScene)
		OgreAssert("createScene failed!", 1);

	mControllerManager = PxCreateControllerManager(*mScene);

	mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, PxCookingParams(PxTolerancesScale()));
	/*
	customizeSceneDesc(sceneDesc);
	sceneDesc.simType = NX_SIMULATION_SW;
	sceneDesc.userContactReport = &gContactReport;
	gPhysicsSDK->getFoundationSDK();
	gScene = gPhysicsSDK->createScene(sceneDesc);

	float timestep = 1.0f/100.0f;
	gScene->setTiming(timestep, 8, NX_TIMESTEP_FIXED);

	//props.
	gPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.01);
	gPhysicsSDK->setParameter(NX_VISUALIZATION_SCALE, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_COLLISION_SHAPES, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_ACTOR_AXES, 1);
	gPhysicsSDK->setParameter(NX_VISUALIZE_JOINT_LOCAL_AXES, 10);

	//set default mat
	NxMaterial* defaultMaterial = gScene->getMaterialFromIndex(0);
	defaultMaterial->setRestitution(0.25);
	defaultMaterial->setStaticFriction(0.60);
	defaultMaterial->setDynamicFriction(0.40);
	*/

	/*
	//char controller manager
	ControllerAllocator* alloc = new ControllerAllocator();
	gManager = NxCreateControllerManager(alloc);

	//debugger
	mVisualDebugger = GSYS->GetSceneMgr()->createManualObject("mVisualDebugger");
	mVisualDebugger->setCastShadows(false);
	mVisualDebuggerNode = GSYS->GetSceneMgr()->getRootSceneNode()->createChildSceneNode();
	mVisualDebuggerNode->attachObject(mVisualDebugger);
	MaterialPtr mVisualDebuggerMaterial = MaterialManager::getSingleton().create("mVisualDebuggerMaterial","General");
	mVisualDebuggerMaterial->setReceiveShadows(false);
	mVisualDebuggerMaterial->getTechnique(0)->setLightingEnabled(false);
	mVisualDebugger->clear();
	mVisualDebugger->begin("mVisualDebuggerMaterial", Ogre::v1::RenderOperation::OT_LINE_LIST);
	mVisualDebugger->position(Vector3(0,0,0));
	mVisualDebugger->position(Vector3(0,1.8,0));
	mVisualDebugger->position(Vector3(0,1.8,0));
	mVisualDebugger->position(Vector3(4,1.8,0));
	mVisualDebugger->position(Vector3(0,1.8,0));
	mVisualDebugger->position(Vector3(0,1.8,2));
	mVisualDebugger->end();
	AxisAlignedBox aabInf;
	aabInf.setInfinite();
	mVisualDebugger->setBoundingBox(aabInf);
	*/
	// @TODO // End Section Outdated, marked for removal
	//@TODO:
}

void PhysicsSystem::Finalize() const
{
	//debugger
	//@TODO:
	// @TODO // Start Section Outdated, marked for removal
	/*
	mVisualDebuggerNode->detachAllObjects();
	mVisualDebuggerNode->getParentSceneNode()->removeAndDestroyChild(mVisualDebuggerNode->getName());
	mVisualDebuggerNode = NULL;
	mVisualDebugger->clear();
	GSYS->GetSceneMgr()->destroyManualObject(mVisualDebugger);
	mVisualDebugger = NULL;

	//phy
	NxReleaseControllerManager(gManager);
	gScene->fetchResults(NX_RIGID_BODY_FINISHED,true);
	gPhysicsSDK->releaseScene(*gScene);
	gPhysicsSDK->release();
	*/
	; // End Section Outdated, marked for removal
	mPhysics->release();
	mFoundation->release();
}

void PhysicsSystem::Update() const
{
	mScene->simulate(GlobalVars::Tick);
	mScene->fetchResults(true);
	// @TODO // Start Section Outdated, marked for removal
	  //@TODO:
	/*
	gScene->simulate(GlobalVars::Tick);
	gScene->flushStream();
	while(!gScene->fetchResults(NX_RIGID_BODY_FINISHED, true)){}

	//debug
	if (mDebuggerView)
	{
		const NxDebugRenderable* data = gScene->getDebugRenderable();
		PxU32 NbLines = NULL;
		if (data)NbLines = data->getNbLines();
		const NxDebugLine* Lines;
		if (data)Lines = data->getLines();

		mVisualDebugger->clear();
		mVisualDebugger->begin("mVisualDebuggerMaterial", Ogre::v1::RenderOperation::OT_LINE_LIST);
		while(NbLines--)
		{
			mVisualDebugger->position(Lines->p0.x,Lines->p0.y,Lines->p0.z);
			mVisualDebugger->colour(1,1,1);
			mVisualDebugger->position(Lines->p1.x,Lines->p1.y,Lines->p1.z);
			mVisualDebugger->colour(1,1,1);
			Lines++;
		}
		mVisualDebugger->end();
	}
	*/
	; // End Section Outdated, marked for removal
	  //@TODO:
}

; // Start Section Outdated, marked for removal
/*
void PhysicsSystem::SetActorCollisionGroup(PxActor* actor, PxCollisionGroup group)
{
	PxShape*const* shapes = actor->getShapes();
	PxU32 nShapes = actor->getNbShapes();
	while (nShapes--)
	{
		shapes[nShapes]->setGroup(group);
	}
}
*/
; // End Section Outdated, marked for removal

void PhysicsSystem::FlipDebug()
{
	mDebuggerView = !mDebuggerView;
	//@TODO:
	// @TODO // Start Section Outdated, marked for removal
	/*
	if (!mDebuggerView)
	{
		mVisualDebugger->clear();
		mVisualDebugger->begin("mVisualDebuggerMaterial", Ogre::v1::RenderOperation::OT_LINE_LIST);
		mVisualDebugger->position(Vector3(0,0,0));
		mVisualDebugger->position(Vector3(0,1.8,0));
		mVisualDebugger->position(Vector3(0,1.8,0));
		mVisualDebugger->position(Vector3(4,1.8,0));
		mVisualDebugger->position(Vector3(0,1.8,0));
		mVisualDebugger->position(Vector3(0,1.8,2));
		mVisualDebugger->end();
	}
	*/
	; // End Section Outdated, marked for removal
	  //@TODO:
}

Vector3 PhysicsSystem::CastRay1(const Vector3 & from, Vector3 & dir) const
{
	//@TODO:
	// @TODO
	//ray cast
	PxVec3 org = TemplateUtils::toNX(from);
	PxVec3 ndir = TemplateUtils::toNX(dir.normalisedCopy());
	PxReal maxDistance = 10000;            // [in] Raycast max distance

	const PxU32 bufferSize = 256;        // [in] size of 'hitBuffer'
	PxRaycastHit hitBuffer[bufferSize];  // [out] User provided buffer for results
	PxRaycastBuffer buf; // [out] Blocking hits will be stored here

	// Raycast against all static & dynamic objects (no filtering)
	// The main result from this call is the closest hit, stored in the 'hit.block' structure
	bool hadBlockingHit = mScene->raycast(org, ndir, maxDistance, buf);

	return TemplateUtils::toOgre(buf.block.normal);
}

Vector3 PhysicsSystem::CastRay2(Vector3 & from, Vector3 & to) const
{
	//ray cast
	PxVec3 org = TemplateUtils::toNX(from);
	Vector3 delta = to - from;
	PxVec3 ndir = TemplateUtils::toNX(delta.normalisedCopy());
	//NxRay ray(org, ndir);
	//NxRaycastHit hit;
	//mat = 0;

	PxVec3 origin = org;                 // [in] Ray origin
	PxVec3 unitDir = ndir;                // [in] Normalized ray direction
	PxReal maxDistance = 10000;            // [in] Raycast max distance

	const PxU32 bufferSize = 256;        // [in] size of 'hitBuffer'
	PxRaycastHit hitBuffer[bufferSize];  // [out] User provided buffer for results
	PxRaycastBuffer buf(hitBuffer, bufferSize); // [out] Blocking and touching hits will be stored here

	// Raycast against all static & dynamic objects (no filtering)
	// The main result from this call are all hits along the ray, stored in 'hitBuffer'
	bool hadBlockingHit = mScene->raycast(origin, unitDir, maxDistance, buf);
	if (hadBlockingHit)
		//shape = buf.block.shape;
		for (PxU32 i = 0; i < buf.nbTouches;) {
			//shape = buf.touches[i].shape;
			return TemplateUtils::toOgre(buf.touches[i].position);
		}
	/*
	//Get the closest shape
	PxShape* closestShape = gScene->raycastClosestShape(ray, NX_ALL_SHAPES, hit, -1, delta.length());
	if (closestShape)
	{
		if (shape != NULL)*shape = closestShape;
		mat = hit.materialIndex;
		const PxVec3& worldImpact = hit.worldNormal;
		return TemplateUtils::toOgre(worldImpact);
	}
	*/
	return Vector3::ZERO;
}

Vector3 PhysicsSystem::CastRay6(Vector3 & from, Vector3 & to, PxRaycastBuffer &buf) const
{
	//ray cast
	PxVec3 org = TemplateUtils::toNX(from);
	Vector3 delta = to - from;
	PxVec3 ndir = TemplateUtils::toNX(delta.normalisedCopy());
	//NxRay ray(org, ndir);
	//NxRaycastHit hit;
	//mat = 0;

	PxVec3 origin = org;                 // [in] Ray origin
	PxVec3 unitDir = ndir;                // [in] Normalized ray direction
	PxReal maxDistance = 10000;            // [in] Raycast max distance

												// Raycast against all static & dynamic objects (no filtering)
												// The main result from this call are all hits along the ray, stored in 'hitBuffer'
	bool hadBlockingHit = mScene->raycast(origin, unitDir, maxDistance, buf);
	if (hadBlockingHit)
		return TemplateUtils::toOgre(buf.touches[0].position);

	/*
	//Get the closest shape
	PxShape* closestShape = gScene->raycastClosestShape(ray, NX_ALL_SHAPES, hit, -1, delta.length());
	if (closestShape)
	{
	if (shape != NULL)*shape = closestShape;
	mat = hit.materialIndex;
	const PxVec3& worldImpact = hit.worldNormal;
	return TemplateUtils::toOgre(worldImpact);
	}
	*/
	return Vector3::ZERO;
}

Vector3 PhysicsSystem::CastRay3()
{
	//@TODO:
	// @TODO
	return Vector3();
}

Vector3 PhysicsSystem::CastRay4(const Vector3 & from, Vector3 & to) const
{
	//@TODO:
	// @TODO
	//ray cast
	PxVec3 org = TemplateUtils::toNX(from);
	Vector3 delta = to - from;
	PxVec3 ndir = TemplateUtils::toNX(delta.normalisedCopy());
	PxReal maxDistance = 10000;            // [in] Raycast max distance

	const PxU32 bufferSize = 256;        // [in] size of 'hitBuffer'
	PxRaycastHit hitBuffer[bufferSize];  // [out] User provided buffer for results
	PxRaycastBuffer buf; // [out] Blocking hits will be stored here

	// Raycast against all static & dynamic objects (no filtering)
	// The main result from this call is the closest hit, stored in the 'hit.block' structure
	bool hadBlockingHit = mScene->raycast(org, ndir, maxDistance, buf);

	//if(hadBlockingHit)
		//return TemplateUtils::toOgre(buf.block.position);
	//else
	return to;
}

Vector3 PhysicsSystem::CastRay5(const Vector3 & from, Vector3 & to) const
{
	//@TODO:
	// @TODO
	//ray cast
	PxVec3 org = TemplateUtils::toNX(from);
	Vector3 delta = to - from;
	PxVec3 ndir = TemplateUtils::toNX(delta.normalisedCopy());
	PxReal maxDistance = 10000;            // [in] Raycast max distance

	const PxU32 bufferSize = 256;        // [in] size of 'hitBuffer'
	PxRaycastHit hitBuffer[bufferSize];  // [out] User provided buffer for results
	PxRaycastBuffer buf; // [out] Blocking hits will be stored here

	// Raycast against all static & dynamic objects (no filtering)
	// The main result from this call is the closest hit, stored in the 'hit.block' structure
	bool hadBlockingHit = mScene->raycast(org, ndir, maxDistance, buf);

	if (hadBlockingHit)
		return TemplateUtils::toOgre(buf.block.position);
	//else
	return Vector3::ZERO;
}

bool PhysicsSystem::OverlapTest()
{
	return false;
}

PxMaterial* PhysicsSystem::addNewMaterial()
{
	return nullptr;
}

Ogre::String PhysicsSystem::getMaterialName()
{
	return {};
}

//@TODO:
; // Start Section Outdated, marked for removal
/*

Ogre::Vector3 PhysicsSystem::CastRay1(Ogre::Vector3 from, Ogre::Vector3 dir)
{
	//ray cast
	PxVec3 org = TemplateUtils::toNX(from);
	PxVec3 ndir = TemplateUtils::toNX(dir);
	NxRay ray(org, ndir);
	NxRaycastHit hit;
	//Get the closest shape
	PxShape* closestShape = gScene->raycastClosestShape(ray, NX_STATIC_SHAPES, hit);
	if (closestShape)
	{
		const PxVec3& worldImpact = hit.worldImpact;
		return TemplateUtils::toOgre(worldImpact);
	}
	return Vector3::ZERO;
}

Ogre::Vector3 PhysicsSystem::CastRay2(Ogre::Vector3 from, Ogre::Vector3 to, PxShape** shape, PxMaterialIndex &mat)
{
	//ray cast
	PxVec3 org = TemplateUtils::toNX(from);
	Vector3 delta = to - from;
	PxVec3 ndir = TemplateUtils::toNX(delta.normalisedCopy());
	NxRay ray(org, ndir);
	NxRaycastHit hit;
	mat = 0;
	//Get the closest shape
	PxShape* closestShape = gScene->raycastClosestShape(ray, NX_ALL_SHAPES, hit, -1, delta.length());
	if (closestShape)
	{
		if (shape != NULL)*shape = closestShape;
		mat = hit.materialIndex;
		const PxVec3& worldImpact = hit.worldNormal;
		return TemplateUtils::toOgre(worldImpact);
	}
	if (shape != NULL)*shape = NULL;
	return Vector3::ZERO;
}

Ogre::Vector3 PhysicsSystem::CastRay3(Ogre::Vector3 from, Ogre::Vector3 to)
{
	//ray cast
	PxVec3 org = TemplateUtils::toNX(from);
	Vector3 delta = to - from;
	PxVec3 ndir = TemplateUtils::toNX(delta.normalisedCopy());
	NxRay ray(org, ndir);
	NxRaycastHit hit;
	//Get the closest shape
	PxShape* closestShape = gScene->raycastClosestShape(ray, NX_STATIC_SHAPES, hit, -1, delta.length());
	if (closestShape)
	{
		const PxVec3& worldImpact = hit.worldImpact;
		return TemplateUtils::toOgre(worldImpact);
	}
	return Vector3::ZERO;
}

bool PhysicsSystem::OverlapTest(Ogre::Vector3 min, Ogre::Vector3 max)
{
	PxBounds3 worldBounds;
	worldBounds.set(TemplateUtils::toNX(min), TemplateUtils::toNX(max));
	return gScene->checkOverlapAABB(worldBounds, NX_STATIC_SHAPES);
}

PxMaterialIndex PhysicsSystem::addNewMaterial(Ogre::String name)
{
	//check existance
	for (size_t i = 0;i < materials.size();i++)
	{
		if (name == materials[i])
		{
			return (PxMaterialIndex)i;
		}
	}
	//else create new
	// resize the array to fit the new found material
	NxMaterialDesc mdesc;
	mdesc.restitution = 0.25;
	mdesc.staticFriction = 0.60;
	mdesc.dynamicFriction = 0.40;
	NxMaterial* mat = gScene->createMaterial(mdesc);
	PxMaterialIndex index = mat->getMaterialIndex();
	try {
		materials.push_back(name);
	}
	catch(const std::bad_alloc & e )
	{
		return (PxMaterialIndex);
	}
	return (PxMaterialIndex)materials.size();
}

Ogre::String PhysicsSystem::getMaterialName(PxMaterialIndex index)
{
	if(index == materials.size()) return materials[(int)index - 1];
	return materials[(int)index];
}
*/

//@TODO:
// End Section Outdated, marked for removal 