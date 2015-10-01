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

#ifndef PhysicsSystem_H_
#define PhysicsSystem_H_

#include "physxPrereqs.h"

#include "OgreSingleton.h"
#include "OgreMemoryAllocatorConfig.h"

#include "PxPhysicsAPI.h"
#include "extensions/PxDefaultSimulationFilterShader.h"

using namespace physx;

#include <malloc.h>

#define PVD_HOST "127.0.0.1"	//Set this to the IP address of the system running the PhysX Visual Debugger that you want to connect to.

class UserErrorCallback : public PxErrorCallback
{
public:
	virtual void reportError(PxErrorCode::Enum code, const char* message, const char* file,
		int line)
	{
		// error processing implementation
		;
	}
};


#define PHY PhysicsSystem::getSingletonPtr()

class PhysicsSystem : public Ogre::Singleton<PhysicsSystem>, public Ogre::GeneralAllocatedObject
{
public:
	PhysicsSystem();
	~PhysicsSystem();
	static PhysicsSystem& getSingleton();
	static PhysicsSystem* getSingletonPtr();

	void Initialize();
	void Finalize();
	void Update();
	void FlipDebug();

	//utility
	Ogre::Vector3 CastRay1(const Ogre::Vector3 &from, Ogre::Vector3 &dir);
	Ogre::Vector3 CastRay2(Ogre::Vector3 &from, Ogre::Vector3 &to, PxShape** shape);
	Ogre::Vector3 CastRay3(Ogre::Vector3 &from, Ogre::Vector3 &to);
	Ogre::Vector3 CastRay4(const Ogre::Vector3 & from, Ogre::Vector3 & to);
	Ogre::Vector3 CastRay5(const Ogre::Vector3 & from, Ogre::Vector3 & dir);
	bool OverlapTest(Ogre::Vector3 *min, Ogre::Vector3 *max);

	//gets & sets
	std::vector<Ogre::String> * getMaterials() { return &materials; }
	//@TODO: Outdated, marked for removal //NxPhysicsSDK* getSDK()  const { return gPhysicsSDK; } 
	PxScene*      getScene() const { return mScene; }
	PxPhysics * getPhysics() { return mPhysics; }
	PxControllerManager* getCManager() const { 
		return mControllerManager;
	}
	//@TODO: Outdated - new implementation needed //void SetActorCollisionGroup(PxActor* actor, PxCollisionGroup *group);
	//@TODO: Outdated - new implementation needed //PxMaterialIndex addNewMaterial(Ogre::String name);
	PxMaterial* addNewMaterial(Ogre::String name);
	Ogre::String getMaterialName(PxMaterial *mat);

	
private:
	//@TODO: Outdated, marked for removal //PxPhysicsSDK*        gPhysicsSDK;
	//@TODO: Outdated - new implementation needed //PhysicsContactReport * gContactReport;
	std::vector<Ogre::String> materials;

	//debugging
	Ogre::ManualObject*	mVisualDebugger;
	Ogre::SceneNode*	mVisualDebuggerNode;
	bool				mDebuggerView;

	PxFoundation * mFoundation;

	PxDefaultErrorCallback gDefaultErrorCallback;
	PxDefaultAllocator gDefaultAllocatorCallback;
	PxProfileZoneManager * mProfileZoneManager;
	PxDefaultCpuDispatcher *mCpuDispatcher;
	PxFilterFlags gDefaultFilterShader;
	PxPhysics * mPhysics;
	PxVisualDebuggerConnection* gConnection;
	PxScene * mScene;
	PxCooking * mCooking;
	PxControllerManager* mControllerManager;
	PxController * mController;
	PxMaterial * mMaterial;
};

#endif