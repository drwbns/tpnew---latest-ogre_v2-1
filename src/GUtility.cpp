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



/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2010 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
//
/// The MIT License
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////*/

#include <math.h>
#include "Ogre.h"
#include "tinyxml.h"

#include "GUtility.h"
#include "PhysicsSystem.h"
#include "SharedData.h"

using namespace Ogre;

Ogre::String	TemplateUtils::mExePath = "";
unsigned int	TemplateUtils::mVertexBufferSize = 0;
unsigned int	TemplateUtils::mIndexBufferSize = 0;
Ogre::Vector3*	TemplateUtils::mVertexBuffer = 0;
unsigned long*	TemplateUtils::mIndexBuffer = 0;
int				TemplateUtils::mMtlCount = 0;
int				TemplateUtils::mObjCount = 0;
int				TemplateUtils::mNameCount = 1; // used for entities, statemachine objectID 0 is invalid

int				TemplateUtils::mBodyNodeCount = 1;
int				TemplateUtils::mBodyEntityCount = 1;
int				TemplateUtils::mRibbonTrailCount = 1;
int				TemplateUtils::mLeftSwordCount = 1;
int				TemplateUtils::mRightSwordCount = 1;
int				TemplateUtils::mEntityLabelCount = 1;



//-----------------------------------------------------------------------------------------
bool TemplateUtils::getMeshInformation(const Ogre::Mesh* mesh,
                        size_t &vertex_count,
						OgreVector3Array &vertices,
                        size_t &index_count,
                        LongArray &indices,
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
    for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);
		if(!submesh) {
			Ogre::Exception(Exception::ERR_INTERNAL_ERROR,"Could not load submesh at index :" + i,"Mesh::getSubMesh");
		}
        // We only need to add the shared vertices once
        if(submesh->useSharedVertices)
        {
            if( !added_shared )
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

	// NO VALID DATA SHOULD BE PRESENT IN THE FOLLOWING 2 ARRAYS
	// UNTIL AFTER THE NEXT FOR LOOP

	/*
    vertices = new Ogre::Vector3 [vertex_count];
	std::vector<Ogre::Vector3> vertices [vertex_count];
    indices = new unsigned long [index_count];
	std::vector<long> indices [index_count];
	*/

	vertices.resize(vertex_count);
    indices.resize(index_count);

    added_shared = false;
 
    // Run through the submeshes again, adding the data into the arrays
    for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);
		if(!submesh) {
			Ogre::Exception(Exception::ERR_INTERNAL_ERROR,"Could not load submesh at index :" + i,"Mesh::getSubMesh");
		}
        Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;
 
        if ((!submesh->useSharedVertices) || (submesh->useSharedVertices && !added_shared))
        {
            if(submesh->useSharedVertices)
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
            //Ogre::Real* pReal;
            float* pReal;
 
            for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
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
 
        bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);
 
        unsigned int* pLong = static_cast<unsigned int*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);
 
        size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;
 
        if ( use32bitindexes )
        {
            for ( size_t k = 0; k < numTris*3; ++k)
            {
                indices[index_offset++] = pLong[k] + static_cast<unsigned long>(offset);
            }
        }
        else
        {
            for ( size_t k = 0; k < numTris*3; ++k)
            {
                indices[index_offset++] = static_cast<unsigned long>(pShort[k]) +
                                          static_cast<unsigned long>(offset);
            }
        }
 
        ibuf->unlock();
        current_offset = next_offset;
    }
	return true;
}

void TemplateUtils::getMeshInformationNx (Mesh* const mesh,
							  unsigned int &vertex_count,
								           unsigned int* &indices,
										   float* & vertices,
							   unsigned int &index_count,
							   std::vector<Ogre::Vector3*> &normals,
										 std::vector<PxMaterialIndex*> &mmaterials,
										     float* &min,
										     float* &max,
									    Vector3 position,
									   Quaternion orient,
										   Vector3 scale)
{
	bool added_shared = false;
	size_t current_offset = 0;
	size_t shared_offset = 0;
	size_t next_offset = 0;
	size_t index_offset = 0;
	vertex_count = index_count = 0;

	min = new float[3];
	min[0] = mesh->getBounds().getMinimum().x * scale.x;
	min[1] = mesh->getBounds().getMinimum().y * scale.y;
	min[2] = mesh->getBounds().getMinimum().z * scale.z;

	max = new float[3];
	max[0] = mesh->getBounds().getMaximum().x * scale.x;
	max[1] = mesh->getBounds().getMaximum().y * scale.y;
	max[2] = mesh->getBounds().getMaximum().z * scale.z;

	// Calculate how many vertices and indices we're going to need
	for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
	{
		Ogre::SubMesh* submesh = mesh->getSubMesh( i );

		// We only need to add the shared vertices once
		if(submesh->useSharedVertices)
		{
			if( !added_shared )
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
	vertex_count *= 3;
	vertices = new float[vertex_count];
	indices  = new unsigned int[index_count];
	//normals  = new float[index_count/3*3];
	added_shared = false;

	// Run through the submeshes again, adding the data into the arrays
	int vct = 0;
	int nct = 0;
	int mct = 0;
	for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
	{
		Ogre::SubMesh* submesh = mesh->getSubMesh(i);
		Ogre::String mat_name = submesh->getMaterialName();
		printf(">>>>>> sub %i mat : %s\n", i, mat_name.c_str());
		Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;
		//vertices
		if((!submesh->useSharedVertices)||(submesh->useSharedVertices && !added_shared))
		{
			if(submesh->useSharedVertices)
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

			float* pfloat;
			
			for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
			{
				posElem->baseVertexPointerToElement(vertex, &pfloat);
				Ogre::Vector3 pt(pfloat[0], pfloat[1], pfloat[2]);
				pt = (orient * (pt * scale)) + position;
				vertices[vct++] = pt.x;
				vertices[vct++] = pt.y;
				vertices[vct++] = pt.z;
			}
			
			vbuf->unlock();

			// normals
			
			const Ogre::VertexElement* nrmElem =
				vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_NORMAL);
			// Check if we have normals data
			if(nrmElem){
				Ogre::HardwareVertexBufferSharedPtr vbuf2 =
				vertex_data->vertexBufferBinding->getBuffer(nrmElem->getSource());

				unsigned char* vertex2 =
				static_cast<unsigned char*>(vbuf2->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

				float* pfloat2;

				for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex2 += vbuf2->getVertexSize())
				{
					nrmElem->baseVertexPointerToElement(vertex, &pfloat2);
					Ogre::Vector3 pt(pfloat[0], pfloat[1], pfloat[2]);
					normals.push_back(&pt);
				}

				vbuf2->unlock();
				//
			}
			
			next_offset += vertex_data->vertexCount;
		}

		//indices
		Ogre::IndexData* index_data = submesh->indexData;
		size_t numTris = index_data->indexCount / 3;
		Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;
		bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);
		unsigned int*  pLong = static_cast<unsigned int*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
		unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);
		size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;
		if ( use32bitindexes )
		{
			for ( size_t k = 0; k < numTris*3; ++k)
			{
				indices[index_offset++] = pLong[k] + static_cast<unsigned int>(offset);
			}
		}
		else
		{
			for ( size_t k = 0; k < numTris*3; ++k)
			{
				unsigned int t = static_cast<unsigned int>(pShort[k]) + static_cast<unsigned int>(offset);
				indices[index_offset++] = static_cast<unsigned int>(pShort[k]) + static_cast<unsigned int>(offset);
			}
		}
		ibuf->unlock();
		current_offset = next_offset;

		//add material
		PxMaterialIndex mat = PHY->addNewMaterial(mat_name);
		//int tri_count = numTris;
		mmaterials.push_back(&mat);
		
	}

	printf(">>>>>>> verts : %i / %i\n", vct, vertex_count);
	printf(">>>>>>> indexes : %i / %i\n", index_offset, index_count);
	printf(">>>>>>> normals : %i / %i\n", nct, index_count/3*3);
	printf(">>>>>>> mats : %i / %i\n", mct, index_count/3);
}

//-----------------------------------------------------------------------------------------
bool TemplateUtils::WorldIntersect(Ogre::RaySceneQuery* mRaySceneQuery, Ogre::Ray &ray, Ogre::Vector3 &hitposition)
{
    mRaySceneQuery->setRay(ray);
    mRaySceneQuery->setQueryTypeMask(Ogre::SceneManager::ENTITY_TYPE_MASK);
    //mRaySceneQuery->setsetWorldFragmentType(Ogre::SceneQuery::WFT_SINGLE_INTERSECTION);
    Ogre::RaySceneQueryResult& qryResult = mRaySceneQuery->execute();
    Ogre::RaySceneQueryResult::iterator i = qryResult.begin();
    if (i != qryResult.end() && i->worldFragment)
    {
        hitposition = i->worldFragment->singleIntersection;
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------------------
void TemplateUtils::vzero(float *v)
{
    v[0] = 0.0;
    v[1] = 0.0;
    v[2] = 0.0;
}

//-----------------------------------------------------------------------------------------
void TemplateUtils::vset(float *v, float x, float y, float z)
{
    v[0] = x;
    v[1] = y;
    v[2] = z;
}

//-----------------------------------------------------------------------------------------
void TemplateUtils::vsub(const float *src1, const float *src2, float *dst)
{
    dst[0] = src1[0] - src2[0];
    dst[1] = src1[1] - src2[1];
    dst[2] = src1[2] - src2[2];
}

//-----------------------------------------------------------------------------------------
void TemplateUtils::vcopy(float *v1, const float *v2)
{
    int i;
    for (i = 0 ; i < 3 ; ++i)
        v1[i] = v2[i];
}

//-----------------------------------------------------------------------------------------
void TemplateUtils::vcross(const float *v1, const float *v2, float *cross)
{
    float temp[3];

    temp[0] = (v1[1] * v2[2]) - (v1[2] * v2[1]);
    temp[1] = (v1[2] * v2[0]) - (v1[0] * v2[2]);
    temp[2] = (v1[0] * v2[1]) - (v1[1] * v2[0]);
    vcopy(temp, cross);
}

//-----------------------------------------------------------------------------------------
float TemplateUtils::vlength(const float *v)
{
    return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

//-----------------------------------------------------------------------------------------
void TemplateUtils::vscale(float *v, float div)
{
    v[0] *= div;
    v[1] *= div;
    v[2] *= div;
}

//-----------------------------------------------------------------------------------------
void TemplateUtils::vnormal(float *v)
{
    vscale(v,1.0f/vlength(v));
}

//-----------------------------------------------------------------------------------------
float TemplateUtils::vdot(const float *v1, const float *v2)
{
    return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
}

//-----------------------------------------------------------------------------------------
void TemplateUtils::vadd(const float *src1, const float *src2, float *dst)
{
    dst[0] = src1[0] + src2[0];
    dst[1] = src1[1] + src2[1];
    dst[2] = src1[2] + src2[2];
}