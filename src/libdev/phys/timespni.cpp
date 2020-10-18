/*
 * T I M E S P N I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "phys/rampacce.hpp"

#include "phys/internal/timespni.hpp"

PER_DEFINE_PERSISTENT( PhysTimedSpinPlanImpl );


PhysTimedSpinPlanImpl::PhysTimedSpinPlanImpl( const MexVec3& axis, const MexPoint3d& position,
                                      const MexRadians& startAngle, const MexRadians& startSpeed )
:   axis_( axis ),
    baseTransform_( MexTransform3d(position) ),
    startAngle_( startAngle ),
    startSpeed_( startSpeed ),
    cacheIndex_( 0 )
{
    TEST_INVARIANT;
}

PhysTimedSpinPlanImpl::PhysTimedSpinPlanImpl( const MexVec3& axis, const MexTransform3d& baseTransform,
                                      const MexRadians& startAngle, const MexRadians& startSpeed )
:   axis_( axis ),
    baseTransform_( baseTransform ),
    startAngle_( startAngle ),
    startSpeed_( startSpeed ),
    cacheIndex_( 0 )
{
    TEST_INVARIANT;
}

PhysTimedSpinPlanImpl::~PhysTimedSpinPlanImpl()
{
    TEST_INVARIANT;

}

void PhysTimedSpinPlanImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PhysTimedSpinPlanImpl& t )
{

    o << "PhysTimedSpinPlanImpl " << (void*)&t << " start" << std::endl;
    o << "PhysTimedSpinPlanImpl " << (void*)&t << " end" << std::endl;

    return o;
}

PhysTimedSpinPlanImpl::PhysTimedSpinPlanImpl( PerConstructor )
{
}

void perWrite( PerOstream& ostr, const PhysTimedSpinPlanImpl& plan )
{
    ostr << plan.axis_;
    ostr << plan.baseTransform_;
    ostr << plan.startAngle_;
    ostr << plan.startSpeed_;
    ostr << plan.times_;
    ostr << plan.accelerations_;
    ostr << plan.endAngles_;
    ostr << plan.endSpeeds_;
    ostr << plan.cacheIndex_;
}

void perRead( PerIstream& istr, PhysTimedSpinPlanImpl& plan )
{
    istr >> plan.axis_;
    istr >> plan.baseTransform_;
    istr >> plan.startAngle_;
    istr >> plan.startSpeed_;
    istr >> plan.times_;
    istr >> plan.accelerations_;
    istr >> plan.endAngles_;
    istr >> plan.endSpeeds_;
    istr >> plan.cacheIndex_;
}

/* End TIMESPNI.CPP *************************************************/
