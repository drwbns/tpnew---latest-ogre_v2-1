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


#include "Projectile.h"
#include "BlueBullet.h"
#include "ProjectileManager.h"
#include "World.h"
#include "PhysicsSystem.h"
#include "BillboardSystem.h"
#include "ParticleManager.h"

#include "GlobalVars.h"

#include "PxShape.h"

using namespace Ogre;
using namespace physx;

BlueBullet::BlueBullet() : Projectile()
{
	bill = -1;
	sbill = -1;
	ebill = -1;
	Speed = 1800;
	HitPoint = -5.5;
}

BlueBullet::~BlueBullet()
{
}

void BlueBullet::Update()
{
	//check if it will hit sth.;
	Vector3 toAdd = Direction * Speed * GlobalVars::Tick;
	PxShape* shape = NULL;
	/*
	PxMaterial mat;
	Vector3 normal = PHY->CastRay2(Position, &(*Position+toAdd), &shape, mat);
	if (normal == Vector3::ZERO)
	{
		//show source
		if (sbill == -1)
		{
			
			if(PTM) {
				PTM->ShowParticle("spark2", *Position, normal);
			}
			else {
				sbill = BBS->ShowBillboard("BulletSource", *Position, SBLUETIME);
				sbill = 0;
			}
		}

		//move it
		*Position += toAdd;
		BBS->UpdateBillboard("Bullet", bill, *Position);
		PTM->ShowParticle("spark2", *Position, *Direction);
		if (Position->distance(*Start) > 100.0)
		{
			Alive = false;
			BBS->HideBillboard("Bullet", bill);
		}
	}
	else
	{
		//hide bullet
		if (bill != -1)
		{
			BBS->HideBillboard("Bullet", bill);
		}
		Alive = false;

		if (ebill == -1)
		{
			//show impact
			ebill = BBS->ShowBillboard("BulletSource", *Position, EBLUETIME);
			String mat_name = PHY->getMaterialName(&mat);
			if (mat_name.length() > 0)
			{
				if (mat_name == "cube")
				{
					PTM->ShowParticle("hit2", *Position, normal);
				}
				else if (mat_name == "taban" || mat_name == "tavan" || mat_name == "duvar")
				{
					PTM->ShowParticle("spark", *Position, normal);
				}
				else if (mat_name == "engel")
				{
					PTM->ShowParticle("spark2", *Position, normal);
				}
				else if (mat_name == "vipbody")
				{
					PTM->ShowParticle("blood", *Position, normal);
				}
			}

			//apply impact
			if (shape != NULL)
			{
				if (shape->getActor().isDynamic())
				{
					shape->getActor().addForceAtPos(TemplateUtils::toPX(*Direction * Speed * 0.25), TemplateUtils::toPX(Position), PX_IMPULSE);
				}
				else if (shape->getActor().userData != NULL)
				{
					Agent* a = (Agent*)shape->getActor().userData;
					//warn attacked one
					a->setAttacker(Owner);
					//inc. hit score
					Owner->incShotsHit();
					//double dmg. if head shot
					if (shape->getActor().getGlobalPosition() == a->getHitBox(0)->getGlobalPosition())
					{
						a->add2Hp(2 * HitPoint);
					}
					else
					{
						a->add2Hp(HitPoint);
					}
				}
			}
		}
	}
	*/
}