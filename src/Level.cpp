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


#include "GUtility.h"
#include "Level.h"
#include "GraphicsSystem.h"
#include "PhysicsSystem.h"
#include "StateSystem.h"
#include "GameState.h"
#include "World.h"
#include "BoxItem.h"
#include "Sample_TileMesh.h"

#include "Detour/DetourNavMesh.h"
#include "OgreDistanceLodStrategy.h"
#include <OgrePixelCountLodStrategy.h>
//#include "NavMesher.h"
#include "DotSceneLoader.hpp"
#include "OgreVector3.h"
#include "OgreQuaternion.h"
#include "OgreLodStrategyManager.h"

#include "foundation\PxSimpleTypes.h"
using namespace Ogre;


Level::Level()
{
	//nv = new NavMesher();
	//nv->Build();
	//mTileMesh = new Sample_TileMesh;
	//this->handleSettings();
	
	
	if(!handleBuild()) {
		return;
	}
	//DotScene Loading code
	
	DotSceneLoader* pDotSceneLoader = new DotSceneLoader();
	pDotSceneLoader->parseDotScene("city.scene", "General", GSYS->GetSceneMgr(), GSYS->GetSceneMgr()->getRootSceneNode());
	delete pDotSceneLoader;
	
	LodStrategyManager::getSingleton().setDefaultStrategy(Ogre::PixelCountLodStrategy::getSingletonPtr());
	// End .scene loading code
	
	const dtNavMesh *nav = m_navMesh;
	int ntiles = 0;
	for (int i = 0; i < nav->getMaxTiles(); i++)
	{
		const dtMeshTile* tile = nav->getTile(i);
		if (!tile->header) continue;
		ntiles++;
	}
	printf("tile count: %d\n", ntiles);

	//handleRender();
	//mTileMesh->handleRender();

	//this->DrawDebug(); // Overwrites handleRender(), old code
	String name = "Level";
	node = GSYS->GetSceneMgr()->getSceneNode(getFilePath());
	
	ent = GSYS->GetSceneMgr()->getEntity(getFilePath());
	//node = GSYS->GetSceneMgr()->getRootSceneNode()->createChildSceneNode();
	//node->attachObject(ent);
	

	//node->setScale(.7,.7,.7); //  for Iogre exporter - C4D - exported with "scale up by a factor of 1"
	//node->setScale(0,0,0); // for Iogre exporter - C4D - exported with "scale down by a factor of 1"

	//physics data

	//TODO : Get a simplified version for large vertex meshes and use that for collision data
	/*
	TemplateUtils::getMeshInformationNx(ent->getMesh().get(),
									  m_nverts,
									  m_tris,
									  m_verts,
									  m_ntris,
									  m_trinorms,
									  m_materials,
									  m_bmin,
									  m_bmax,
									  node->getPosition(),
									  node->getOrientation(),
									  node->getScale());
									  
		*/
	// Quick check for valid mesh data
 	if(INPT->getMesh()->getVerts().size() > 0) {

		// Convert Tris to Nx friendly array (PxU32* )

		std::vector<size_t>::const_iterator myIter = INPT->getMesh()->getTris().begin();

		std::vector<PxU32> fsFaces;
		for (myIter; myIter != INPT->getMesh()->getTris().end(); ++myIter ){
			fsFaces.push_back(*myIter);
		}

		// Convert verts (points) to Nx friendly array (PxVec3*)

		std::vector<float>::const_iterator myVertsIter = INPT->getMesh()->getVerts().begin();

		std::vector<PxVec3> fsVerts;
		PxVec3 tempVec3;
		int vertsIndex = 0;
		for (size_t i = 0; i < INPT->getMesh()->getVerts().size()/3; i++){
			tempVec3.x = INPT->getMesh()->getVerts()[vertsIndex];
			tempVec3.y = INPT->getMesh()->getVerts()[vertsIndex+1];
			tempVec3.z = INPT->getMesh()->getVerts()[vertsIndex+2];
			vertsIndex+=3;
			fsVerts.push_back(tempVec3);
		}
		

		NxTriangleMeshDesc fsDesc;
		fsDesc.numVertices = (PxU32)INPT->getMesh()->getVertCount();
		fsDesc.numTriangles = (PxU32)INPT->getMesh()->getTriCount();
		fsDesc.pointStrideBytes = sizeof(PxVec3);
		fsDesc.triangleStrideBytes = 3*sizeof(PxU32);
		fsDesc.points = &fsVerts[0];  // PxVec3*[] array
		fsDesc.triangles = &fsFaces[0]; // PxU32*[] array
		fsDesc.flags = 0;
		fsDesc.materialIndexStride = sizeof(PxMaterialIndex);
		if (INPT->getMesh()->getMaterials().size() > 0)
			fsDesc.materialIndices = &INPT->getMesh()->getMaterials()[0];
		
		NxTriangleMeshShapeDesc fsShapeDesc;
		NxInitCooking();
		{
			MemoryWriteBuffer buf;
			buf.data = NULL;
			bool status = NxCookTriangleMesh(fsDesc, buf);
			if (status == true )
				fsShapeDesc.meshData = PHY->getSDK()->createTriangleMesh(MemoryReadBuffer(buf.data));
		}

		if (fsShapeDesc.meshData)
		{
			PxActorDesc actorDesc;
			NxBodyDesc bodyDesc;
			actorDesc.shapes.pushBack(&fsShapeDesc);
			actorDesc.body	= NULL;
			actorDesc.globalPose.t = TemplateUtils::toNX(Vector3(0,0,0));
			actorDesc.name = "Hangar";
			if (actorDesc.isValid())
			{
				actor = PHY->getScene()->createActor(actorDesc);
				actor->userData = NULL;
				PHY->SetActorCollisionGroup(actor, GROUP_COLLIDABLE_NON_PUSHABLE);
			}
			else
			{
				throw Ogre::Exception(Ogre::Exception::ERR_INTERNAL_ERROR, "Actor Create Error #6", __FUNCTION__);
			}
		}

		//add box items
		for (int i=0;i<4;i++)
		{
			for (int j=0;j<8;j++)
			{
				//BoxItem* it = new BoxItem(i*8+j, "cube", Vector3(-10+i*5,0.6,-16+j*4), Vector3(0.01,0.01,0.01), 10);
				//items.push_back(it);
			}
		}
	}
}

Level::~Level()
{
	//navmesher
	//delete nv;
	//nv = NULL;

	//node
	node->detachAllObjects();
	node->getParentSceneNode()->removeAndDestroyChild(node->getName());
	node = NULL;
	
	//ent
	GSYS->GetSceneMgr()->destroyEntity(ent);
	ent = NULL;
	
	//phy
	PHY->getScene()->releaseActor(*actor);
	actor = NULL;
	//items
	for (size_t i=0;i<items.size();i++)
	{
		delete items[i];
		items[i] = NULL;
	}
	items.clear();
}

void Level::BuildNavMesh()
{
	//GetNavMesher()->Build();
}

void Level::Update()
{
	for (size_t i=0;i<items.size();i++)
	{
		items[i]->Update();
	}
}