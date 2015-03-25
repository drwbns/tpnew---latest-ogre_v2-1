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

#ifndef Level_H_
#define Level_H_

#include "Sample_TileMesh.h"
#include "NavMeshTesterTool.h"

//class NavMesher;
class BoxItem;


class Level : public Sample_TileMesh
{
public:
	Level();
	~Level();

	void BuildNavMesh();
	void Update();

	NxActor* GetActor() { return actor; }
	//NavMesher* GetNavMesher() { return nv; }

	static Ogre::Vector3 toOgre(const NxVec3 &vec3)
	{
		return Ogre::Vector3(vec3.x, vec3.y, vec3.z);
	}
	static Ogre::Quaternion toOgre(const NxQuat &q)
	{
		return Ogre::Quaternion(q.w, q.x, q.y, q.z);
	}
	static Ogre::AxisAlignedBox toOgre(const NxBounds3 &b)
	{
		return Ogre::AxisAlignedBox(toOgre(b.min), toOgre(b.max));
	}

	static NxVec3 toNx(const Ogre::Vector3 &vec3)
	{
		return NxVec3(vec3.x, vec3.y, vec3.z);
	}

	static unsigned short * toNx(std::vector<unsigned short*> myVector)
	{
		unsigned short * myArray = new unsigned short[myVector.size()];
		for (size_t i = 0; i < myVector.size(); i++){
			myArray[i] = *myVector[i];
		}
		return myArray;
	}

	static NxVec3 * toNx(std::vector<NxVec3*> myVector)
	{
		NxVec3 * myArray = new NxVec3[myVector.size()];
		for (size_t i = 0; i < myVector.size(); i++){
			myArray[i] = *myVector[i];
		}
		return myArray;
	}

	NxVec3* toNx()
	{
		NxVec3* newverts = new NxVec3[INPT->getMesh()->getVertCount()/3];
		for (size_t i=0;i < INPT->getMesh()->getVertCount()/3;i++)
		{
			newverts[i].x = INPT->getMesh()->getVerts()[i*3+0];
			newverts[i].y = INPT->getMesh()->getVerts()[i*3+1];
			newverts[i].z = INPT->getMesh()->getVerts()[i*3+2];
		}
		return newverts;
	}

	static NxQuat toNx(const Ogre::Quaternion &q)
	{
		NxQuat nxq; nxq.setWXYZ(q.w, q.x, q.y, q.z);
		return nxq;
	}
	static NxBounds3 toNx(const Ogre::AxisAlignedBox &b)
	{
		NxBounds3 nxb;
		nxb.min = toNx(b.getMinimum());
		nxb.max = toNx(b.getMaximum());
		return nxb;
	}
	static NxMat34 toNx(const Ogre::Vector3 &pos, const Ogre::Quaternion &rot)
	{
		return NxMat34(NxMat33(toNx(rot)), toNx(pos));
	}

protected:
	Ogre::SceneNode* node;
	Ogre::Entity*    ent;
	NxActor*         actor;
	std::vector<BoxItem*> items;
	//NavMesher* nv;
	

public:
	//geometric information
	float*           m_bmin;
	float*           m_bmax;

};

#endif