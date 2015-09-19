#pragma once

#include "OgreVector3.h"
#include "foundation\PxVec3.h
#include "foundation\PxQuat.h"

class MathConvert {
	public:
	static Ogre::Vector3 toOgre(const PxVec3 &vec3)
	{
		return Ogre::Vector3(vec3.x, vec3.y, vec3.z);
	}
	static Ogre::Quaternion toOgre(const PxQuat &q)
	{
		return Ogre::Quaternion(q.w, q.x, q.y, q.z);
	}
	static Ogre::AxisAlignedBox toOgre(const PxBounds3 &b)
	{
		return Ogre::AxisAlignedBox(toOgre(b.min), toOgre(b.max));
	}

	static PxVec3 toPx(const Ogre::Vector3 &vec3)
	{
		return PxVec3(vec3.x, vec3.y, vec3.z);
	}

	static unsigned short * toPx(std::vector<unsigned short*> myVector)
	{
		unsigned short * myArray = new unsigned short[myVector.size()];
		for (size_t i = 0; i < myVector.size(); i++) {
			myArray[i] = *myVector[i];
		}
		return myArray;
	}

	static PxVec3 * toPx(std::vector<PxVec3*> myVector)
	{
		PxVec3 * myArray = new PxVec3[myVector.size()];
		for (size_t i = 0; i < myVector.size(); i++) {
			myArray[i] = *myVector[i];
		}
		return myArray;
	}

	PxVec3* toPx()
	{
		PxVec3* newverts = new PxVec3[INPT->getMesh()->getVertCount() / 3];
		for (size_t i = 0; i < INPT->getMesh()->getVertCount() / 3; i++)
		{
			newverts[i].x = INPT->getMesh()->getVerts()[i * 3 + 0];
			newverts[i].y = INPT->getMesh()->getVerts()[i * 3 + 1];
			newverts[i].z = INPT->getMesh()->getVerts()[i * 3 + 2];
		}
		return newverts;
	}

	static PxQuat toPx(const Ogre::Quaternion &q)
	{
		PxQuat nxq; nxq.setWXYZ(q.w, q.x, q.y, q.z);
		return nxq;
	}
	static PxBounds3 toPx(const Ogre::AxisAlignedBox &b)
	{
		PxBounds3 nxb;
		nxb.min = toPx(b.getMinimum());
		nxb.max = toPx(b.getMaximum());
		return nxb;
	}
	static PxMat34 toPx(const Ogre::Vector3 &pos, const Ogre::Quaternion &rot)
	{
		return PxMat34(PxMat33(toPx(rot)), toPx(pos));
	}
};