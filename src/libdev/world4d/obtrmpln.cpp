/*
 * O B T R M P L N . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/obtrmpln.hpp"

W4dObjectTrackerMotionPlan::W4dObjectTrackerMotionPlan
(
    const W4dObjectTrackerPtr& objectTrackerPtr, const PhysRelativeTime& duration
)
:   PhysMotionPlan( duration ),
    objectTrackerPtr_( objectTrackerPtr )
{
    TEST_INVARIANT;
}

W4dObjectTrackerMotionPlan::~W4dObjectTrackerMotionPlan()
{
    TEST_INVARIANT;
}

void W4dObjectTrackerMotionPlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const W4dObjectTrackerMotionPlan& t )
{

    o << "W4dObjectTrackerMotionPlan " << (void*)&t << " start" << std::endl;
    o << "W4dObjectTrackerMotionPlan " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void W4dObjectTrackerMotionPlan::transform
(
    const PhysRelativeTime&, MexTransform3d* pResult
) const
{
    objectTrackerPtr_->track( pResult );
}
/* End OBTRMPLN.CPP *************************************************/
