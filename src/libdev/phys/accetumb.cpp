/*
 * A C C E T U M B . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "mathex/transf3d.hpp"
#include "mathex/radians.hpp"
#include "phys/rampacce.hpp"

#include "phys/accetumb.hpp"
#include "phys/internal/accetumi.hpp"

#include "mathex/point3d.hpp"
#include "mathex/eulerang.hpp"

PER_DEFINE_PERSISTENT( PhysAccelerateTumblePlan );

PhysAccelerateTumblePlan::PhysAccelerateTumblePlan(
        const EulerTransformsPtr& transformsPtr,
        const TimesPtr& segmentTimesPtr,
        const MexVec3& acceleration )
: PhysMotionPlan( segmentTimesPtr->back() ),
  pImpl_( _NEW( PhysAccelerateTumblePlanImpl( transformsPtr, segmentTimesPtr, acceleration ) ) )
{
    PRE( transformsPtr->size() == segmentTimesPtr->size() + 1 );
    PRE( inAscendingOrder( *segmentTimesPtr ) );

    computeCacheData();

    TEST_INVARIANT;
}

PhysAccelerateTumblePlan::PhysAccelerateTumblePlan( PerConstructor con )
: PhysMotionPlan( con )
{
}

PhysAccelerateTumblePlan::~PhysAccelerateTumblePlan()
{
    TEST_INVARIANT;
	_DELETE( pImpl_ );
}

//  virtual
void PhysAccelerateTumblePlan::transform( const PhysRelativeTime& timeOffset, MexTransform3d* pResult ) const
{
    CB_DEPIMPL(size_t, currentSegmentIndex_);
    CB_DEPIMPL(TimesPtr, segmentTimesPtr_);
    CB_DEPIMPL(EulerTransformsPtr, transformsPtr_);

    if( timeOffset >= duration() )
    {
        //Just return the final transform
        transformsPtr_->back().transform( pResult );
    }
    else
    {
        //We'll need to modify some cached data
        PhysAccelerateTumblePlan* nonConstThis = _CONST_CAST( PhysAccelerateTumblePlan*, this );
        PhysMotionPlan::Times& segmentTimes = *segmentTimesPtr_;

        bool    recalculateSegmentIndex = false;

        if( segmentTimes[ currentSegmentIndex_ ] < timeOffset )
        {
            recalculateSegmentIndex = true;
        }
        else if( currentSegmentIndex_ != 0  and  timeOffset < segmentTimes[currentSegmentIndex_ - 1] )
        {
            recalculateSegmentIndex = true;
            nonConstThis->pImpl_->currentSegmentIndex_ = 0;
        }

        if( recalculateSegmentIndex )
        {
            ASSERT_INFO( currentSegmentIndex_ );
            ASSERT_INFO( segmentTimes[ currentSegmentIndex_ ] );
            ASSERT_INFO( timeOffset );

//            ASSERT( segmentTimes[ currentSegmentIndex_ ] < timeOffset, "" );

            while( segmentTimes[ currentSegmentIndex_ ] < timeOffset )
                ++nonConstThis->pImpl_->currentSegmentIndex_;

            nonConstThis->computeCacheData();
        }

        ASSERT_INFO( currentSegmentIndex_ );
        ASSERT_INFO( segmentTimes[ currentSegmentIndex_ ] );
        ASSERT_INFO( timeOffset );
        ASSERT_INFO( duration() );

        ASSERT( segmentTimes[ currentSegmentIndex_ ] >= timeOffset, "" );
        ASSERT( implies( currentSegmentIndex_ != 0, segmentTimes[ currentSegmentIndex_ -1 ] <= timeOffset ), "" );

        //Compute the interpolated transform
        PhysRelativeTime    timeFromStartOfSegment( timeOffset );
        if( currentSegmentIndex_ != 0 )
            timeFromStartOfSegment -= segmentTimes[ currentSegmentIndex_ -1 ];

        setPosition( timeFromStartOfSegment, pResult );
        setRotation( timeFromStartOfSegment, pResult );
    }
}

void PhysAccelerateTumblePlan::setPosition( const PhysRelativeTime& timeFromStartOfSegment, MexTransform3d* pResult ) const
{
    CB_DEPIMPL(size_t, currentSegmentIndex_);
    CB_DEPIMPL(MexVec3, acceleration_);
    CB_DEPIMPL(MexVec3, initialVelocity_);
    CB_DEPIMPL(MexEulerAngles, initialOrientation_);
    CB_DEPIMPL(EulerTransformsPtr, transformsPtr_);

    MATHEX_SCALAR x =
      (*transformsPtr_)[ currentSegmentIndex_ ].position()[ 0 ] +
      initialVelocity_.x() * timeFromStartOfSegment +
      acceleration_.x() * timeFromStartOfSegment * timeFromStartOfSegment / 2.0;

    MATHEX_SCALAR y =
      (*transformsPtr_)[ currentSegmentIndex_ ].position()[ 1 ] +
      initialVelocity_.y() * timeFromStartOfSegment +
      acceleration_.y() * timeFromStartOfSegment * timeFromStartOfSegment / 2.0;

    MATHEX_SCALAR z =
      (*transformsPtr_)[ currentSegmentIndex_ ].position()[ 2 ] +
      initialVelocity_.z() * timeFromStartOfSegment +
      acceleration_.z() * timeFromStartOfSegment * timeFromStartOfSegment / 2.0;

    pResult->position( MexPoint3d( x, y, z ) );
}

void PhysAccelerateTumblePlan::setRotation( const PhysRelativeTime& timeFromStartOfSegment, MexTransform3d* pResult ) const
{
    CB_DEPIMPL(MexEulerAngles, deltaOrientation_);
    CB_DEPIMPL(MexEulerAngles, initialOrientation_);
    CB_DEPIMPL(TimesPtr, segmentTimesPtr_);

    MATHEX_SCALAR   timeFraction = timeFromStartOfSegment / currentSegmentDuration();

    ASSERT_INFO( timeFraction );
    ASSERT( timeFraction >= 0.0 and timeFraction <= 1.0, "" );

    MexRadians azimuth = initialOrientation_.azimuth() + timeFraction * deltaOrientation_.azimuth();
    MexRadians elevation = initialOrientation_.elevation() + timeFraction * deltaOrientation_.elevation();
    MexRadians roll = initialOrientation_.roll() + timeFraction * deltaOrientation_.roll();

    pResult->rotation( MexEulerAngles( azimuth, elevation, roll ) );
}

PhysRelativeTime    PhysAccelerateTumblePlan::currentSegmentDuration( void ) const
{
    CB_DEPIMPL(size_t, currentSegmentIndex_);
    CB_DEPIMPL(TimesPtr, segmentTimesPtr_);

    PhysRelativeTime    result;

    if( currentSegmentIndex_ == 0 )
        result = (*segmentTimesPtr_)[ currentSegmentIndex_ ];
    else
        result = (*segmentTimesPtr_)[ currentSegmentIndex_ ] - (*segmentTimesPtr_)[ currentSegmentIndex_ - 1 ];

    return result;
}

void PhysAccelerateTumblePlan::computeCacheData()
{
    CB_DEPIMPL(TimesPtr, segmentTimesPtr_);
    CB_DEPIMPL(MexVec3, acceleration_);
    CB_DEPIMPL(size_t, currentSegmentIndex_);
    CB_DEPIMPL(MexEulerAngles, initialOrientation_);
    CB_DEPIMPL(MexEulerAngles, deltaOrientation_);
    CB_DEPIMPL(EulerTransformsPtr, transformsPtr_);
    CB_DEPIMPL(MexVec3, initialVelocity_);

    PRE( currentSegmentIndex_ < (*segmentTimesPtr_).size() );

    PhysRelativeTime    duration = currentSegmentDuration();

    const PhysEulerTransform3d&    initialTransform = (*transformsPtr_)[ currentSegmentIndex_ ];
    const PhysEulerTransform3d&    finalTransform = (*transformsPtr_)[ currentSegmentIndex_ + 1];

    //  Calculate the initial velocity needed to make the motion finish
    //  in the correct place at the correct time.

    MATHEX_SCALAR distanceX = finalTransform.position()[ 0 ] - initialTransform.position()[ 0 ];
    initialVelocity_.x( ( distanceX - acceleration_.x() * duration * duration / 2.0 ) / duration );

    MATHEX_SCALAR distanceY = finalTransform.position()[ 1 ] - initialTransform.position()[ 1 ];
    initialVelocity_.y( ( distanceY - acceleration_.y() * duration * duration / 2.0 ) / duration );

    MATHEX_SCALAR distanceZ = finalTransform.position()[ 2 ] - initialTransform.position()[ 2 ];
    initialVelocity_.z( ( distanceZ - acceleration_.z() * duration * duration / 2.0 ) / duration );

    initialOrientation_ = initialTransform.rotation();

    MexEulerAngles  finalOrientation = finalTransform.rotation();

    deltaOrientation_.azimuth( finalOrientation.azimuth() - initialOrientation_.azimuth() );
    deltaOrientation_.elevation( finalOrientation.elevation() - initialOrientation_.elevation() );
    deltaOrientation_.roll( finalOrientation.roll() - initialOrientation_.roll() );
}

void PhysAccelerateTumblePlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const PhysAccelerateTumblePlan& t )
{
    o << "PhysAccelerateTumblePlan " << (void*)&t << " start" << std::endl;

    o << *(t.pImpl_);

    o << "PhysAccelerateTumblePlan " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const PhysAccelerateTumblePlan& plan )
{
    const PhysMotionPlan& base = plan;

    ostr << base;

    ostr << plan.pImpl_;
}

void perRead( PerIstream& istr, PhysAccelerateTumblePlan& plan )
{
    PhysMotionPlan& base = plan;

    istr >> base;

    istr >> plan.pImpl_;
}


/* End ACCETUMB.CPP *************************************************/
