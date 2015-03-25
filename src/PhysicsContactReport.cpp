#include "StdAfx.h"
#include "PhysicsContactReport.h"
#include "PhysicsHelper.h"
#include "zzzSndSystem.h"
#include "GUtility.h"

void PhysicsContactReport::onContactNotify(NxContactPair& pair, NxU32 events)
{
	NxContactStreamIterator j(pair.stream);
	NxVec3 cp(0,0,0);
	while(j.goNextPair())
    {
		while(j.goNextPatch())
        {
			while(j.goNextPoint())
            {
				cp = j.getPoint();
            }
        }
    }
	
	if (events & NX_NOTIFY_ON_START_TOUCH_FORCE_THRESHOLD)
	{
		SND->PlaySound("metal", TemplateUtils::toOgre(cp), 25 * fabs(j.getSeparation()));
	}
	if(events & NX_NOTIFY_ON_END_TOUCH_FORCE_THRESHOLD)
	{
	}
	if(events & NX_NOTIFY_ON_TOUCH_FORCE_THRESHOLD)
	{
	}
}