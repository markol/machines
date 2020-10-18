/*
 * A C C E T U M I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/transf3d.hpp"
#include "mathex/radians.hpp"
#include "phys/rampacce.hpp"

#include "phys/internal/accetumi.hpp"

PER_DEFINE_PERSISTENT( PhysAccelerateTumblePlanImpl );

PhysAccelerateTumblePlanImpl::PhysAccelerateTumblePlanImpl
(
	const PhysAccelerateTumblePlan::EulerTransformsPtr& transformsPtr,
    const PhysMotionPlan::TimesPtr& segmentTimesPtr,
  	const MexVec3& acceleration
)
: transformsPtr_( transformsPtr ),
  segmentTimesPtr_( segmentTimesPtr ),
  acceleration_( acceleration ),
  currentSegmentIndex_( 0 )
{

    TEST_INVARIANT;
}

PhysAccelerateTumblePlanImpl::~PhysAccelerateTumblePlanImpl()
{
    TEST_INVARIANT;

}

void PhysAccelerateTumblePlanImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PhysAccelerateTumblePlanImpl& t )
{

    o << "PhysAccelerateTumblePlanImpl " << (void*)&t << " start" << std::endl;

    o << "Time = 0" << std::endl;

    PhysMotionPlan::Times::const_iterator j = t.segmentTimesPtr_->begin();

    for( PhysAccelerateTumblePlan::EulerTransforms::const_iterator i = t.transformsPtr_->begin();
      i != t.transformsPtr_->end();
      ++i )
    {
        o << *i;

        if( j != t.segmentTimesPtr_->end() )
        {
            o << "Time = " << *j << std::endl;
            ++j;
        }
    }

    o << "Acceleration " << t.acceleration_ << std::endl;

    o << "PhysAccelerateTumblePlanImpl " << (void*)&t << " end" << std::endl;

    return o;
}

PhysAccelerateTumblePlanImpl::PhysAccelerateTumblePlanImpl( PerConstructor )
{
}

void perWrite( PerOstream& ostr, const PhysAccelerateTumblePlanImpl& plan )
{
    ostr << plan.transformsPtr_;
    ostr << plan.segmentTimesPtr_;
    ostr << plan.acceleration_;
    ostr << plan.currentSegmentIndex_;
    ostr << plan.deltaOrientation_;
    ostr << plan.initialOrientation_;
    ostr << plan.initialVelocity_;
}

void perRead( PerIstream& istr, PhysAccelerateTumblePlanImpl& plan )
{
    istr >> plan.transformsPtr_;
    istr >> plan.segmentTimesPtr_;
    istr >> plan.acceleration_;
    istr >> plan.currentSegmentIndex_;
    istr >> plan.deltaOrientation_;
    istr >> plan.initialOrientation_;
    istr >> plan.initialVelocity_;
}

/* End ACCETUMI.CPP *************************************************/
