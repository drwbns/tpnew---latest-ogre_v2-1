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
#include "BlueBullet.h"

#include "Projectile.h"
#include "GUtility.h"
#include "ProjectileManager.h"
#include "World.h"
#include "PhysicsSystem.h"
#include "BillboardSystem.h"
#include "GlobalVars.h"

#include "PxShape.h"

using namespace Ogre;
using namespace physx;

BlueBullet::BlueBullet() : Projectile()
{
	mBill = -1;
	sbill = -1;
	ebill = -1;
	ibill = -1; // impact mBill
	mSpeed = 100;
	mHitPoint = -5.5;
}

BlueBullet::~BlueBullet()
{
}

void BlueBullet::Update()
{
	//check if it will hit sth.;
	Vector3 toAdd = mDirection * mSpeed * GlobalVars::Tick;
	PxShape* shape = nullptr;

	//@TODO:
	// @TODO // Start Section Outdated, marked for removal

	PxMaterial * mat; // Possibly used for material names? Need to check if material names can be done in PhysX3 or if a new enum or struct needs to be made
	//Vector3 normal = PHY->CastRay2(mPosition, mPosition+toAdd, shape);

	const PxU32 bufferSize = 256;        // [in] size of 'hitBuffer'
	PxRaycastHit hitBuffer[bufferSize];  // [out] User provided buffer for results
	PxRaycastBuffer buf(hitBuffer, bufferSize); // [out] Blocking and touching hits will be stored here

	Vector3 hitPosition = Vector3::ZERO;

	PxVec3 org = TemplateUtils::toNX(mPosition);
	Vector3 delta = mPosition + toAdd - mPosition;
	PxVec3 ndir = TemplateUtils::toNX(delta.normalisedCopy());

	PxVec3 origin = org;                 // [in] Ray origin
	PxVec3 unitDir = ndir;                // [in] Normalized ray direction
	PxReal maxDistance = 10000;            // [in] Raycast max distance

	// Raycast against all static & dynamic objects (no filtering)
	// The main result from this call are all hits along the ray, stored in 'hitBuffer'
	bool hadBlockingHit = PHY->getScene()->raycast(origin, unitDir, maxDistance, buf);
	if (hadBlockingHit)
		; // buf shows hasBlock is false here

	//show source
	if (sbill == -1)
	{
		sbill = BBS->ShowBillboard("BulletSource", mPosition, IMPACTTIME);
		sbill = 0;
	}

	//show impact
	/*
	if (ibill == -1)
	{
		ibill = BBS->ShowBillboard("Flare", mPosition + toAdd, IMPACTTIME);
		ibill = 0;
	}
	*/

	//move it
	mPosition += toAdd;
	BBS->UpdateBillboard("BulletSource", mBill, mPosition);
	if (mPosition.distance(mStart) > maxDistance)
	{
		mAlive = false;
		BBS->HideBillboard("BulletSource", mBill);
	}

	if (ebill == -1)
	{
		//show impact
		//ebill = BBS->ShowBillboard("RFlare", mPosition + toAdd, IMPACTTIME);
		//ibill = BBS->ShowBillboard("TFlare", mPosition + toAdd, IMPACTTIME);
		/*
		String mat_name = PHY->getMaterialName(mat);
		if (mat_name.length() > 0)
		{
			if (mat_name == "cube")
			{
				PTM->ShowParticle("hit2", mPosition, normal);
			}
			else if (mat_name == "taban" || mat_name == "tavan" || mat_name == "duvar")
			{
				PTM->ShowParticle("spark", mPosition, normal);
			}
			else if (mat_name == "engel")
			{
				PTM->ShowParticle("spark2", mPosition, normal);
			}
			else if (mat_name == "vipbody")
			{
				PTM->ShowParticle("blood", mPosition, normal);
			}
		}
		*/
		//apply impact
		Vector3 normal;
		if (normal != Vector3::ZERO && buf.hasBlock)
		{
			if (buf.touches[0].actor->userData != nullptr)
			{
				Agent* a = static_cast<Agent*>(shape->getActor()->userData);
				//warn attacked one
				a->setAttacker(mOwner);
				//inc. hit score
				mOwner->incShotsHit();
				//double dmg. if head shot
				if (shape->getActor()->getGlobalPose().p == a->getHitBox(0)->getLocalPose().p)
				{
					a->add2Hp(2 * mHitPoint);
				}
				else
				{
					a->add2Hp(mHitPoint);
				}
			}
		}
	}

	// @TODO // End Section Outdated, marked for removal
}