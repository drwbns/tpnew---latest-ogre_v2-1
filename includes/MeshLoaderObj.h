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
// warranty.  In no event will the authors be held liable for any damages			//
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


//
// Copyright (c) 2009-2010 Mikko Mononen memon@inside.org
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//


#ifndef MESHLOADER_OBJ
#define MESHLOADER_OBJ
#include <vector>

#include "OIS/OIS.h"

#include "SharedData.h"
#include "PxPhysics.h"
#include "PxMaterial.h"

#include "OGRE\OgreEntity.h"
#include "OGRE\OgreSceneNode.h"
#include "OGRE\OgreSceneManager.h"


using namespace Ogre;

class rcMeshLoaderObj
{
public:
	rcMeshLoaderObj();
	~rcMeshLoaderObj();

	bool load(const char* filepath);
	bool load();

	const std::vector<float>& getVerts() const { return verts; } 
	//const float* getVerts() const { return &verts[0]; } 
	inline const float* getNormals() const { return &m_normals[0]; }
	const std::vector<size_t>& getTris() const{ return tris; }
	inline size_t getVertCount() const { return nverts; }
	inline int getTriCount() const { return ntris; }
	//inline std::vector<PxMaterial*> getMaterials() const { return m_materials; }

	inline Ogre::Entity* getEntity() const { return ent; }
	inline Ogre::SceneNode* getSceneNode() const { return lvlNode; }
	const char* getFileName() { return fileName; }

private:

	Ogre::SceneManager* mSceneMgr;
	Ogre::Entity* ent;
	Ogre::SceneNode* lvlNode;

protected:

	const char* fileName;
	float* m_verts;		// list of verticies
	int* m_tris;		// list of triangles
	std::vector<float> m_normals; // normal buffer array
	int ntris;			// number of total triangles
	size_t nverts;		// number of verticies
	std::vector<float> verts;	// vert buffer - array of floats
	std::vector<size_t> tris;		// triangle buffer array
	int numNodes;
	
	//std::vector<PxMaterial*> m_materials;
	float* newverts;

};

#endif // MESHLOADER_OBJ