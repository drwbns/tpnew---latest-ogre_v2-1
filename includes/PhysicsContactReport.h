#ifndef PhysicsContactReport_h_
#define PhysicsContactReport_h_

#include "NxUserContactReport.h"

class PhysicsContactReport : public NxUserContactReport
{
public:
    virtual void onContactNotify(NxContactPair& pair, NxU32 events);
};

#endif
