//----------------------------------------------------------------------------------//
// OgreRecast Demo - A demonstration of integrating Recast Navigation Meshes		//
//					 with the Ogre3D rendering engine.								//
//																					//
//	This file was either Created by or Modified by :								//
//													Paul A Wilson 					//
//			All contents are Copyright (C) 2010 Paul A Wilson						//
//			Except where otherwise mentioned or where previous						//
//			copyright exists. In the case of pre-existing copyrights				//
//			all rights remain with the original Authors, and this is				//
//			to be considered a derivative work.										//
//																					//
//	Contact Email	:	paulwilson77@dodo.com.au									//
//																					//
// This 'SOFTWARE' is provided 'AS-IS', without any express or implied				//
// warranty.  In no event will the author/s be held liable for any damages			//
// arising from the use of this software.											//
// Permission is granted to anyone to use this software for any purpose,			//
// including commercial applications, and to alter it and redistribute it			//
// freely, subject to the following restrictions:									//
// 1. The origin of this software must not be misrepresented; you must not			//
//    claim that you wrote the original software. If you use this software			//
//    in a product, an acknowledgment in the product documentation would be			//
//    appreciated but is not required.												//
// 2. Altered source versions must be plainly marked as such, and must not be		//
//    misrepresented as being the original software.								//
// 3. This notice may not be removed or altered from any source distribution.		//
//																					//
//----------------------------------------------------------------------------------//

/////////////////////////////////////////////////////////////////////////////////////
// ORIGINALLY PART OF RECAST DEMO APPLICATION - MODIFIED VERSION			       //
/////////////////////////////////////////////////////////////////////////////////////

#include "MeshLoaderObj.h"
#include "SharedData.h"
#include "GUtility.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <OgrePixelCountLodStrategy.h>
#include <OgreDistanceLodStrategy.h>

using namespace Ogre;

typedef std::vector<Ogre::Vector3> OgreVector3Array;
typedef std::vector<OgreVector3Array> OgreVector3Array2D;
typedef std::vector<long>  LongArray;
typedef std::vector<LongArray> LongArray2D;

rcMeshLoaderObj::rcMeshLoaderObj() :
	m_verts(0),	m_tris(0), m_normals(0), ntris(0),
	tris(0), verts(0), nverts(0), numNodes(0),newverts(0)
	{
		mSceneMgr = SharedData::getSingleton().iSceneMgr;
	}

rcMeshLoaderObj::~rcMeshLoaderObj()
{
	delete [] m_verts;
	//delete [] m_normals;
	delete [] m_tris;
	//delete [] tris;
	//delete [] verts;
	
}

// PARTS OF THE FOLLOWING METHOD WERE TAKEN FROM AN OGRE3D FORUM POST ABOUT RECAST
bool rcMeshLoaderObj::load(const char* filepath)
{
	SharedData::getSingleton().mNavNodeList.resize(0);
	fileName = filepath;
	// check to make sure we have the same amount of filenames and entity names
	//if(entNames.size() != fileNames.size())
	//	return false;

	//m_entNames = entNames;

	// number of entities we need to load
	//numEnt = fileNames.size();
	float offsetX = 0.0f;
	float offsetZ = 0.0f;

	//  ******** Entity Loading code **********

	ent = SharedData::getSingleton().iSceneMgr->createEntity(fileName,fileName);
	ent->setQueryFlags(GEOMETRY_QUERY_MASK);
	lvlNode = SharedData::getSingleton().iSceneMgr->getRootSceneNode()->createChildSceneNode(fileName);
	lvlNode->attachObject(ent);
	lvlNode->setVisible(false); // To display the mesh or not (set to false for .scene loading )

	//lvlNode->setScale(.1,.1,.1);
	//lvlNode->setScale(.3,.3,.3);
	//lvlNode->setScale(.5,.5,.5);
	//lvlNode->setScale(.7,.7,.7);
	//lvlNode->setScale(2,2,2);
	lvlNode->setPosition(offsetX, 0.0f, offsetZ);
	SharedData::getSingleton().mNavNodeList.push_back(lvlNode);
	offsetX += 270.0f;
	offsetZ += 270.0f;

	//initialize mesh counts and data arrays
	const int numNodes = SharedData::getSingleton().mNavNodeList.size();


	std::vector<size_t> meshVertexCount(numNodes);
	std::vector<size_t> meshIndexCount(numNodes);
	OgreVector3Array2D meshVertices(numNodes);
	LongArray2D meshIndices(numNodes);
	/*
	std::vector<size_t> meshVertexCount (numNodes);  // Vertex count
	std::vector<Ogre::Vector3**> meshVertices(numNodes); // Vertices
	
	std::vector<size_t> meshIndexCount (numNodes); // Index count
	std::vector<long > meshIndices(numNodes);  // Indices
	*/ 
	
	/*
	size_t *meshVertexCount = new size_t[numNodes];
	size_t *meshIndexCount = new size_t[numNodes];
	
	Ogre::Vector3 **meshVertices = new Ogre::Vector3*[numNodes];
	unsigned long **meshIndices = new unsigned long*[numNodes];

	*/

	Ogre::Mesh* mesh = reinterpret_cast<Ogre::Mesh *>(Ogre::Root::getSingleton().getMeshManager()->getByName(fileName).get() );	

	for (int i = 0 ; i < numNodes ; i++)
	{
		if (!TemplateUtils::getMeshInformation(mesh, meshVertexCount[i], meshVertices[i], meshIndexCount[i], meshIndices[i])) {
			return false;
			
		}
		//total number of verts
		nverts += meshVertexCount[i];
		//total number of indices
		ntris += meshIndexCount[i];
	}

	// Allocate space for verts and tris arrays based on mesh values
	verts.resize(nverts*3);
	tris.resize(ntris);

	//verts = new float[nverts*3];// verts holds x,y,&z for each verts in the array
 	//tris = new int[ntris];// tris in recast is really indices like ogre

	//convert index count into tri count
	ntris = ntris/3; //although the tris array are indices the ntris is actual number of triangles, eg. indices/3;

	//set the reference node
	SceneNode *referenceNode;
		referenceNode = SharedData::getSingleton().iSceneMgr->getRootSceneNode();

	//copy all meshes verticies into single buffer and transform to world space relative to parentNode
	int vertsIndex = 0;
	int prevVerticiesCount = 0;
	int prevIndexCountTotal = 0;
	for (int i = 0 ; i < numNodes ; i++)
	{
		//find the transform between the reference node and this node
		Ogre::Matrix4 transform = referenceNode->_getFullTransform().inverse() * SharedData::getSingleton().mNavNodeList[i]->_getFullTransform();
		Ogre::Vector3 vertexPos;
		for (uint j = 0 ; j < meshVertexCount[i] ; j++)
		{
			vertexPos = transform*meshVertices[i][j];
			verts[vertsIndex] = vertexPos.x;
			verts[vertsIndex+1] = vertexPos.y;
			verts[vertsIndex+2] = vertexPos.z;
			vertsIndex+=3;
		}

		for (uint j = 0 ; j < meshIndexCount[i] ; j++)
		{
			tris[prevIndexCountTotal+j] = meshIndices[i][j]+prevVerticiesCount;
		}
		prevIndexCountTotal += meshIndexCount[i];
		prevVerticiesCount = meshVertexCount[i];
	}
	//delete tempory arrays 
	//TODO These probably could member varibles, this would increase performance slightly

	/* // Removed due to using std::vector instead
	for (int i = 0 ; i < numNodes ; i++)
	{
		delete [] meshVertices[i];
		delete [] meshIndices[i];
	}
	delete [] meshVertices;
	//delete [] meshVertexCount;
	delete [] meshIndices;
	//delete [] meshIndexCount;
	*/

	// calculate normals data for Recast - im not 100% sure where this is required
	// but it is used, Ogre handles its own Normal data for rendering, this is not related
	// to Ogre at all ( its also not correct lol )
	// TODO : fix this

	// Allocate space for normals data
	m_normals.resize(ntris*3);

 	for (int i = 0; i < ntris*3; i += 3)
 	{
 		const float* v0 = &verts[tris[i]*3];
 		const float* v1 = &verts[tris[i+1]*3];
 		const float* v2 = &verts[tris[i+2]*3];
 		float e0[3], e1[3];
 		for (int j = 0; j < 3; ++j)
 		{
 			e0[j] = (v1[j] - v0[j]);
 			e1[j] = (v2[j] - v0[j]);
 		}
 		float* n = &m_normals[i];
  		n[0] = ((e0[1]*e1[2]) - (e0[2]*e1[1]));
  		n[1] = ((e0[2]*e1[0]) - (e0[0]*e1[2]));
  		n[2] = ((e0[0]*e1[1]) - (e0[1]*e1[0]));
 		
 		float d = sqrtf(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
 		if (d > 0)
 		{
 			d = 1.0f/d;
 			n[0] *= d;
 			n[1] *= d;
 			n[2] *= d;
 		}	
 	}


	return true;
}