/*
 * M O T P L A N . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "phys/motplan.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/radians.hpp"
#include "phys/rampacce.hpp"
#include "ctl/vector.hpp"

#ifndef _INLINE
    #include "phys/motplan.ipp"
#endif

PER_DEFINE_PERSISTENT_ABSTRACT( PhysMotionPlan );

//////////////////////////////////////////////////////////////////////////////////////////

PhysMotionPlan::~PhysMotionPlan()
{
    TEST_INVARIANT;

}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysMotionPlan::CLASS_INVARIANT
{
    //Cannot have negative duration
    INVARIANT(duration_ >= 0.0);
}
//////////////////////////////////////////////////////////////////////////////////////////

bool PhysMotionPlan::inAscendingOrder( const PhysMotionPlan::Times& times ) const
{
    bool inOrder = true;

    size_t n = times.size();
    PRE( n > 0 )

    for( size_t i = 1; i < n; ++i )
    {
        if( times[i] < times[i-1] )
        {
            ASSERT_INFO( times[ i ] );
            ASSERT_INFO( times[ i - 1 ] );
            ASSERT_INFO( times[ i ] - times[ i - 1 ] );

            inOrder = false;
            break;
        }
    }
    
    return inOrder;
}

PhysMotionPlan::PhysMotionPlan( PerConstructor )
{
}

void perWrite( PerOstream& ostr, const PhysMotionPlan& plan )
{
    ostr << plan.duration_;
}

void perRead( PerIstream& istr, PhysMotionPlan& plan )
{
    istr >> plan.duration_;
}

//////////////////////////////////////////////////////////////////////////////////////////
/* End MOTPLAN.CPP **************************************************/
