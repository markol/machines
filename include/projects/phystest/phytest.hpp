/*
 * P H Y T E S T . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include <fstream>
#include "base/base.hpp"

/* This class when instantiated runs the Phys lib test */
class PhysTest
{
public:
	PhysTest();
	~PhysTest();

    bool testPhysLinearMotionPlan( ostream& out) const;
    bool testPhysLinearTravelPlan( ostream& out) const;
    bool testPhysTimedSpinPlan( ostream& out) const;
    bool testPhysAccelerationTumblePlan( ostream& out) const;
    bool testAlignedBSPTree2d( ostream& out ) const;
    bool testPhysMotionChunk( ostream& out ) const;
    bool testPhysRampAccelerationByDistanceTime( ostream& ) const;
    bool testPhysRampAccelerationBySpeed( ostream& ) const;
};
