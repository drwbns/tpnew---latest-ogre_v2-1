/*
void PhysicsContactReport::onContactNotify(NxContactPair& pair, PxU32 events)
{
	NxContactStreamIterator j(pair.stream);
	PxVec3 cp(0,0,0);
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
		ZSND->PlaySound("metal", TemplateUtils::toOgre(cp), 25 * fabs(j.getSeparation()));
	}
	if(events & NX_NOTIFY_ON_END_TOUCH_FORCE_THRESHOLD)
	{
	}
	if(events & NX_NOTIFY_ON_TOUCH_FORCE_THRESHOLD)
	{
	}
}
*/