#ifndef PhysicsHelper_H_
#define PhysicsHelper_H_

#include "PxCharacter.h"
#include "PxController.h"
#include "PxCapsuleController.h"
#include "NxUserAllocator.h"
#include "PxShape.h"
#include "PxActor.h"

#define isnan( x ) (( x ) !=  ( x ))

enum CollGroup
{
	GROUP_NON_COLLIDABLE,
	GROUP_COLLIDABLE_NON_PUSHABLE,
	GROUP_COLLIDABLE_PUSHABLE,
};

#define COLLIDABLE_MASK	(1<<GROUP_COLLIDABLE_NON_PUSHABLE) | (1<<GROUP_COLLIDABLE_PUSHABLE)

class ControllerAllocator : public NxUserAllocator 
{
public:
	virtual void*   mallocDEBUG(size_t size, const char* fileName, int line)    { return ::malloc(size); }
	virtual void*   malloc(size_t size)                                         { return ::malloc(size); }
	virtual void*   realloc(void* memory, size_t size)                          { return ::realloc(memory, size); }
	virtual void    free(void* memory)                                          { ::free(memory); }
};

class ControllerHitReport : public NxUserControllerHitReport
{
	public:
	virtual PxControllerAction onShapeHit(const PxControllerShapeHit& hit)
	{
		if (1 && hit.shape)
		{
			PxCollisionGroup group = hit.shape->getGroup();
			if (group == GROUP_COLLIDABLE_PUSHABLE)
			{
				PxActor& actor = hit.shape->getActor();
				if(actor.isDynamic())
				{
					if(hit.dir.y==0.0f)
					{
						NxF32 coeff = hit.length * 64.0f;
						actor.addForceAtLocalPos(hit.dir*coeff, PxVec3(0,0,0), NX_IMPULSE);
					}
				}
			}
			else if(group==GROUP_COLLIDABLE_NON_PUSHABLE)
			{
			}
		}
		return NX_ACTION_NONE;
	}

	virtual PxControllerAction onControllerHit(const PxControllersHit& hit)
	{
		return NX_ACTION_NONE;
	}

};

#endif