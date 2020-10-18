/*
 * T I M E A N G I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/transf3d.hpp"
#include "phys/rampacce.hpp"

#include "phys/internal/timeangi.hpp"

PER_DEFINE_PERSISTENT( PhysTimedAnglePlanImpl );

PhysTimedAnglePlanImpl::PhysTimedAnglePlanImpl( const PhysMotionPlan::AnglesPtr& anglesPtr,
                                        const PhysMotionPlan::TimesPtr& timesPtr,
                                        const MexVec3& axis, const MexVec3& position )
:   position_( position ),
    axis_( axis ),
    timesPtr_( timesPtr ),
    anglesPtr_( anglesPtr ),
    cacheIndex_( 0 )
{
    TEST_INVARIANT;
}

PhysTimedAnglePlanImpl::~PhysTimedAnglePlanImpl()
{
    TEST_INVARIANT;

}

void PhysTimedAnglePlanImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PhysTimedAnglePlanImpl& t )
{

    o << "PhysTimedAnglePlanImpl " << (void*)&t << " start" << std::endl;
    o << "PhysTimedAnglePlanImpl " << (void*)&t << " end" << std::endl;

    return o;
}

PhysTimedAnglePlanImpl::PhysTimedAnglePlanImpl( PerConstructor )
{
}

void perWrite( PerOstream& ostr, const PhysTimedAnglePlanImpl& plan )
{
    ostr << plan.position_;
    ostr << plan.axis_;
    ostr << plan.timesPtr_;
    ostr << plan.anglesPtr_;
    ostr << plan.cacheIndex_;
}

void perRead( PerIstream& istr, PhysTimedAnglePlanImpl& plan )
{
    istr >> plan.position_;
    istr >> plan.axis_;
    istr >> plan.timesPtr_;
    istr >> plan.anglesPtr_;
    istr >> plan.cacheIndex_;
}

/* End TIMEANGI.CPP *************************************************/
