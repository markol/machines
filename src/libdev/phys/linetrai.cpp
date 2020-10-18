/*
 * L I N E T R A I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "phys/internal/linetrai.hpp"

PER_DEFINE_PERSISTENT( PhysLinearTravelPlanImpl );

PhysLinearTravelPlanImpl::PhysLinearTravelPlanImpl( const PhysMotionPlan::TransformsPtr& transformsPtr )
: transformsPtr_( transformsPtr ),
  rampAccelerationsPtr_( _NEW( PhysMotionPlan::RampAccelerations() ) ),
  cacheValid_( false ),
  segmentTimesPtr_( _NEW( PhysMotionPlan::Times ) ),
  startRotationDistance_( 2.0 )
{

    TEST_INVARIANT;
}

PhysLinearTravelPlanImpl::PhysLinearTravelPlanImpl(
    const PhysMotionPlan::TransformsPtr& transformsPtr,
    const PhysMotionPlan::RampAccelerationsPtr& rampAccelerationsPtr )
: transformsPtr_( transformsPtr ),
  rampAccelerationsPtr_( rampAccelerationsPtr ),
  cacheValid_( false ),
  segmentTimesPtr_( _NEW( PhysMotionPlan::Times ) ),
  startRotationDistance_( 2.0 )
{
    TEST_INVARIANT;
}

PhysLinearTravelPlanImpl::~PhysLinearTravelPlanImpl()
{
    TEST_INVARIANT;

}

void PhysLinearTravelPlanImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PhysLinearTravelPlanImpl& t )
{

    o << "PhysLinearTravelPlanImpl " << (void*)&t << " start" << std::endl;
    o << (*t.transformsPtr_) << std::endl;
    o << (*t.rampAccelerationsPtr_) << std::endl;
    o << "PhysLinearTravelPlanImpl " << (void*)&t << " end" << std::endl;

    return o;
}

PhysLinearTravelPlanImpl::PhysLinearTravelPlanImpl( PerConstructor )
{
}

void perWrite( PerOstream& ostr, const PhysLinearTravelPlanImpl& plan )
{
    ostr << plan.transformsPtr_;
    ostr << plan.rampAccelerationsPtr_;
    ostr << plan.segmentTimesPtr_;

    ostr << plan.isRotationSegment_;
    ostr << plan.startRotationDistance_;
    ostr << plan.cacheValid_;
    ostr << plan.cacheSegment_;
    ostr << plan.cacheTranslation_;
    ostr << plan.cacheRotationAxis_;
    ostr << plan.cacheAngle_;
    ostr << plan.cacheSegmentTime_;
    ostr << plan.cacheAccelerationTime_;
    ostr << plan.cacheDecelerationTime_;
    ostr << plan.cacheStartSpeed_;
    ostr << plan.cacheIsRotationSegment_;
    ostr << plan.cacheRotationLowerS_;
}

void perRead( PerIstream& istr, PhysLinearTravelPlanImpl& plan )
{
    istr >> plan.transformsPtr_;
    istr >> plan.rampAccelerationsPtr_;
    istr >> plan.segmentTimesPtr_;

    istr >> plan.isRotationSegment_;
    istr >> plan.startRotationDistance_;
    istr >> plan.cacheValid_;
    istr >> plan.cacheSegment_;
    istr >> plan.cacheTranslation_;
    istr >> plan.cacheRotationAxis_;
    istr >> plan.cacheAngle_;
    istr >> plan.cacheSegmentTime_;
    istr >> plan.cacheAccelerationTime_;
    istr >> plan.cacheDecelerationTime_;
    istr >> plan.cacheStartSpeed_;
    istr >> plan.cacheIsRotationSegment_;
    istr >> plan.cacheRotationLowerS_;
}

/* End LINETRAI.CPP *************************************************/
