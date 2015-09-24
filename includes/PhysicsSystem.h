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

using namespace physx;

#include <malloc.h>
class PxDefaultAllocator : public PxAllocatorCallback
{
	void* allocate(size_t size, const char*, const char*, int)
	{
		return _aligned_malloc(size, 16);
	}

	void deallocate(void* ptr)
	{
		_aligned_free(ptr);
	}
};

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
	Ogre::Vector3 CastRay2(Ogre::Vector3 &from, Ogre::Vector3 &to, PxShape** shape, PxMaterial &mat);
	Ogre::Vector3 CastRay3(Ogre::Vector3 &from, Ogre::Vector3 &to);
	bool OverlapTest(Ogre::Vector3 *min, Ogre::Vector3 *max);

	//gets & sets
	std::vector<Ogre::String> * getMaterials() { return &materials; }
	; // Outdated, marked for removal //NxPhysicsSDK* getSDK()  const { return gPhysicsSDK; } 
	PxScene*      getScene() const { return gScene; }
	PxControllerManager* getCManager() const { return gManager; }
	; // Outdated - new implementation needed //void SetActorCollisionGroup(PxActor* actor, PxCollisionGroup *group);
	; // Outdated - new implementation needed //PxMaterialIndex addNewMaterial(Ogre::String name);
	Ogre::String getMaterialName(PxMaterial *mat);

	
private:
	; // Outdated, marked for removal //PxPhysicsSDK*        gPhysicsSDK;
    PxScene*	         gScene;
	PxControllerManager* gManager;
	; // Outdated - new implementation needed //PhysicsContactReport * gContactReport;
	std::vector<Ogre::String> materials;

	//debugging
	Ogre::ManualObject*	mVisualDebugger;
	Ogre::SceneNode*	mVisualDebuggerNode;
	bool				mDebuggerView;

	PxFoundation * mFoundation;

	static PxDefaultErrorCallback gDefaultErrorCallback;
	static PxDefaultAllocator gDefaultAllocatorCallback;
	static PxProfileZoneManager * mProfileZoneManager;
	PxPhysics * mPhysics;
};

#endif