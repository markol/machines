/*
 * L I N E T R A V . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */
 /*
    CB_DEPIMPL(PhysMotionPlan::TransformsPtr, transformsPtr_);
    CB_DEPIMPL(PhysMotionPlan::RampAccelerationsPtr, rampAccelerationsPtr_);
    CB_DEPIMPL(PhysMotionPlan::TimesPtr, segmentTimesPtr_);
    CB_DEPIMPL(ctl_vector< bool >, isRotationSegment_);
    CB_DEPIMPL(MATHEX_SCALAR,   startRotationDistance_);
    CB_DEPIMPL(bool, cacheValid_);
    CB_DEPIMPL(size_t, cacheSegment_);
    CB_DEPIMPL(MexPoint3d, cacheTranslation_);
    CB_DEPIMPL(MexVec3, cacheRotationAxis_);
    CB_DEPIMPL(MATHEX_SCALAR, cacheAngle_);
    CB_DEPIMPL(PhysRelativeTime, cacheSegmentTime_);
    CB_DEPIMPL(PhysRelativeTime, cacheAccelerationTime_);
    CB_DEPIMPL(PhysRelativeTime, cacheDecelerationTime_);
    CB_DEPIMPL(MATHEX_SCALAR, cacheStartSpeed_);
    CB_DEPIMPL(bool, cacheIsRotationSegment_);
    CB_DEPIMPL(MATHEX_SCALAR,   cacheRotationLowerS_);
*/
//  Definitions of non-inline non-template methods and global functions

#include "base/base.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/vec3.hpp"
#include "mathex/quatern.hpp"
#include "mathex/radians.hpp"
#include "phys/rampacce.hpp"

#include "phys/linetrav.hpp"
#include "phys/internal/linetrai.hpp"

PER_DEFINE_PERSISTENT( PhysLinearTravelPlan );

//////////////////////////////////////////////////////////////////////////////////////////

PhysLinearTravelPlan::PhysLinearTravelPlan( const PhysMotionPlan::TransformsPtr& transformsPtr,
                          MATHEX_SCALAR startTranslationSpeed,
                          MATHEX_SCALAR translationSpeed,
                          MATHEX_SCALAR translationAcceleration,
                          MATHEX_SCALAR translationDeceleration,
                          MATHEX_SCALAR startRotationSpeed,
                          MATHEX_SCALAR rotationSpeed,
                          MATHEX_SCALAR rotationAcceleration,
                          MATHEX_SCALAR rotationDeceleration,
                          bool comeToRest )
: PhysMotionPlan( 0 ),
  pImpl_( _NEW( PhysLinearTravelPlanImpl( transformsPtr ) )	)
{
    PRE_INFO( *transformsPtr );
    PRE_INFO( startTranslationSpeed );
    PRE_INFO( translationSpeed );
    PRE_INFO( translationAcceleration );
    PRE_INFO( translationDeceleration );
    PRE_INFO( startRotationSpeed );
    PRE_INFO( rotationSpeed );
    PRE_INFO( rotationAcceleration );
    PRE_INFO( rotationDeceleration );

    //Check arguments
    PRE( transformsPtr.isDefined() );
    PRE( transformsPtr->size() >= 2 );
    PRE( startTranslationSpeed >= 0.0 );
    PRE( translationSpeed > 0.0 );
    PRE( translationAcceleration > 0.0 );
    PRE( translationDeceleration > 0.0 );
    PRE( startRotationSpeed >= 0.0 );
    PRE( rotationSpeed > 0.0 );
    PRE( rotationAcceleration > 0.0 );
    PRE( rotationDeceleration > 0.0 );
    PRE( transformsPtr->size() > 1 );

    LINEAR_TRAVEL_STREAM( "Enter PhysLinearTravelPlan::PhysLinearTravelPlan " << (void*)this << std::endl );
    LINEAR_TRAVEL_INDENT( 2 );
    LINEAR_TRAVEL_WHERE;

    LINEAR_TRAVEL_INSPECT( *transformsPtr );
    LINEAR_TRAVEL_INSPECT( startTranslationSpeed );
    LINEAR_TRAVEL_INSPECT( translationSpeed );
    LINEAR_TRAVEL_INSPECT( translationAcceleration );
    LINEAR_TRAVEL_INSPECT( translationDeceleration );
    LINEAR_TRAVEL_INSPECT( startRotationSpeed );
    LINEAR_TRAVEL_INSPECT( rotationSpeed );
    LINEAR_TRAVEL_INSPECT( rotationAcceleration );
    LINEAR_TRAVEL_INSPECT( rotationDeceleration );

    CB_DEPIMPL(TransformsPtr, transformsPtr_);
    CB_DEPIMPL(RampAccelerationsPtr, rampAccelerationsPtr_);
    CB_DEPIMPL(TimesPtr, segmentTimesPtr_);
    CB_DEPIMPL(ctl_vector< bool >, isRotationSegment_);
    CB_DEPIMPL(MATHEX_SCALAR,   startRotationDistance_);

    //Set the sizes of the time collections
    const PhysMotionPlan::Transforms& transforms = *transformsPtr;

    size_t nSegments = transforms.size() - 1;
    isRotationSegment_.reserve( nSegments );
    rampAccelerationsPtr_->reserve( nSegments );
    segmentTimesPtr_->reserve( nSegments );

    //Compute the times for each segment. Get the location component of the first transform.
    PhysRelativeTime totalDuration = 0;
    MexPoint3d startPos( transforms[0].position() );
    MATHEX_SCALAR startSpeed = startTranslationSpeed;
    MATHEX_SCALAR endSpeed;

    for( size_t i = 0; i < nSegments; ++i )
    {
        //Get the position of the next transform
        MexPoint3d endPos( transforms[i + 1].position() );

        //Compute the separation distance
        MATHEX_SCALAR distance = endPos.euclidianDistance( startPos );
        PhysRelativeTime segmentTime;
        bool isRotationSegment = distance < MexEpsilon::instance();

        if( not isRotationSegment )
        {
            //We are translating.

            endSpeed = (comeToRest ? 0 : translationSpeed);
            PhysRampAcceleration rampAcceleration(
                distance, startSpeed, translationSpeed, endSpeed,
                translationAcceleration,
                translationDeceleration,
                PhysRampAcceleration::BY_SPEED );

            segmentTime = rampAcceleration.totalTime();
            rampAccelerationsPtr_->push_back( rampAcceleration );
        }
        else
        {
//             //We are just rotating. Compute the orientation change as a quaternion
//             MexTransform3d inverse, differenceTransform;
//             transforms[i].invert( &inverse );
//             inverse.transform( transforms[i+1], &differenceTransform );
//             MexQuaternion params;
//             differenceTransform.rotation( &params );
//
//             //Hence compute the angle we rotate through
//             MexVec3 axis;
//             MATHEX_SCALAR angle = params.rotation( &axis );

            const MATHEX_SCALAR angle = ( transforms[ i ].xBasis().signedAngleBetween( transforms[ i + 1 ].xBasis() ) ).asScalar();
            const MATHEX_SCALAR desiredSpeed = angle < 0 ? -rotationSpeed : rotationSpeed;

            PhysRampAcceleration rampAcceleration(
                angle, 0.0, desiredSpeed, 0.0,
                rotationAcceleration,
                rotationDeceleration,
                PhysRampAcceleration::BY_SPEED );

            segmentTime = rampAcceleration.totalTime();
            rampAccelerationsPtr_->push_back( rampAcceleration );

            startSpeed = endSpeed = 0;
        }

        //Push the times etc onto the vectors
        totalDuration += segmentTime;

        segmentTimesPtr_->push_back( totalDuration );
        startSpeed = endSpeed;
        isRotationSegment_.push_back( isRotationSegment );

        startPos = endPos;
    }

    //Update the base class with the actual duration
    duration( totalDuration );

    TEST_INVARIANT;

    LINEAR_TRAVEL_INDENT( -2 );
    LINEAR_TRAVEL_STREAM( "Exit PhysLinearTravelPlan::PhysLinearTravelPlan " << (void*)this << std::endl );
}

//////////////////////////////////////////////////////////////////////////////////////////

PhysLinearTravelPlan::PhysLinearTravelPlan(
    const PhysMotionPlan::TransformsPtr& transformsPtr,
    const PhysMotionPlan::RampAccelerationsPtr& rampAccelerationsPtr )
: PhysMotionPlan( 0 ),
  pImpl_( _NEW( PhysLinearTravelPlanImpl( transformsPtr, rampAccelerationsPtr ) )	)
{
    CB_DEPIMPL(TransformsPtr, transformsPtr_);
    CB_DEPIMPL(RampAccelerationsPtr, rampAccelerationsPtr_);
    CB_DEPIMPL(TimesPtr, segmentTimesPtr_);
    CB_DEPIMPL(ctl_vector< bool >, isRotationSegment_);
    CB_DEPIMPL(MATHEX_SCALAR,   startRotationDistance_);

    PRE( transformsPtr.isDefined() );
    PRE( transformsPtr->size() >= 2 );
    PRE( rampAccelerationsPtr.isDefined() );
    PRE( transformsPtr->size() == rampAccelerationsPtr->size() + 1 );

    LINEAR_TRAVEL_STREAM( "Enter PhysLinearTravelPlan::PhysLinearTravelPlan " << (void*)this << std::endl );
    LINEAR_TRAVEL_INDENT( 2 );
    LINEAR_TRAVEL_WHERE;

    LINEAR_TRAVEL_INSPECT( *transformsPtr );
    LINEAR_TRAVEL_INSPECT( *rampAccelerationsPtr );

    const PhysMotionPlan::Transforms& transforms = *transformsPtr;

    size_t nSegments = transforms.size() - 1;
    isRotationSegment_.reserve( nSegments );
    rampAccelerationsPtr_->reserve( nSegments );
    segmentTimesPtr_->reserve( nSegments );

    PhysRelativeTime totalDuration = 0;
    MexPoint3d startPos( transforms[0].position() );

    for( size_t i = 0; i < nSegments; ++i )
    {
        //Get the position of the next transform
        MexPoint3d endPos( transforms[i + 1].position() );

        //Compute the separation distance
        MATHEX_SCALAR distance = endPos.euclidianDistance( startPos );
        PhysRelativeTime segmentTime = (*rampAccelerationsPtr)[ i ].totalTime();
        bool isRotationSegment = distance < MexEpsilon::instance();

        //Push the times etc onto the vectors
        totalDuration += segmentTime;

        segmentTimesPtr_->push_back( totalDuration );
        isRotationSegment_.push_back( isRotationSegment );

        startPos = endPos;
    }

    //Update the base class with the actual duration
    duration( totalDuration );

    LINEAR_TRAVEL_INDENT( -2 );
    LINEAR_TRAVEL_STREAM( "Exit PhysLinearTravelPlan::PhysLinearTravelPlan " << (void*)this << std::endl );
}

PhysLinearTravelPlan::~PhysLinearTravelPlan()
{
    TEST_INVARIANT;

    _DELETE( pImpl_ );
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysLinearTravelPlan::transform( const PhysRelativeTime& timeOffset, MexTransform3d* pResult) const
{
    CB_DEPIMPL(TransformsPtr, transformsPtr_);
    CB_DEPIMPL(RampAccelerationsPtr, rampAccelerationsPtr_);
    CB_DEPIMPL(TimesPtr, segmentTimesPtr_);
    CB_DEPIMPL(bool, cacheValid_);
    CB_DEPIMPL(size_t, cacheSegment_);

    PRE( pResult != NULL );
    PRE( timeOffset >= 0.0 );

    LINEAR_TRAVEL_STREAM( "Enter PhysLinearTravelPlan::transform " << (void*)this << std::endl );
    LINEAR_TRAVEL_INDENT( 2 );
    LINEAR_TRAVEL_INSPECT( timeOffset );

    //Check for past duration
    if( timeOffset >= duration() )
    {
        //Just return the final transform
        *pResult = transformsPtr_->back();
    }
    else
    {
        //We'll need to modify some cached data
        PhysLinearTravelPlan* nonConstThis = (PhysLinearTravelPlan*)this;
        PhysMotionPlan::Times& segmentTimes = *segmentTimesPtr_;

        //Check for the desired time being in the most recent cache interval
        if( cacheValid_ )
        {
            if( timeOffset > segmentTimes[cacheSegment_] )
            {
                nonConstThis->pImpl_->cacheValid_ = false;
                ++nonConstThis->pImpl_->cacheSegment_;
            }
            else if( cacheSegment_ != 0  and  timeOffset < segmentTimes[cacheSegment_ - 1] )
            {
                nonConstThis->pImpl_->cacheValid_ = false;
                nonConstThis->pImpl_->cacheSegment_ = 0;
            }
        }
        else
            nonConstThis->pImpl_->cacheSegment_ = 0;

        //If required, find the new correct interval
        if( not cacheValid_ )
        {
            size_t nSegments = segmentTimes.size();
            while( segmentTimes[cacheSegment_] < timeOffset )
            {
                ++nonConstThis->pImpl_->cacheSegment_;
                ASSERT( cacheSegment_ < nSegments, "" );
            }

            //Load the cached data for this interval
            nonConstThis->computeCacheData();
        }

        //Compute the interpolated transform
        computeInterpolatedTransform( timeOffset, pResult );
    }

    LINEAR_TRAVEL_INSPECT( *pResult );
    LINEAR_TRAVEL_INDENT( -2 );
    LINEAR_TRAVEL_STREAM( "Exit PhysLinearTravelPlan::transform " << (void*)this << std::endl );
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysLinearTravelPlan::computeCacheData()
{
    CB_DEPIMPL(TransformsPtr, transformsPtr_);
    CB_DEPIMPL(RampAccelerationsPtr, rampAccelerationsPtr_);
    CB_DEPIMPL(TimesPtr, segmentTimesPtr_);
    CB_DEPIMPL(ctl_vector< bool >, isRotationSegment_);
    CB_DEPIMPL(MATHEX_SCALAR,   startRotationDistance_);
    CB_DEPIMPL(bool, cacheValid_);
    CB_DEPIMPL(size_t, cacheSegment_);
    CB_DEPIMPL(MexPoint3d, cacheTranslation_);
    CB_DEPIMPL(MexVec3, cacheRotationAxis_);
    CB_DEPIMPL(MATHEX_SCALAR, cacheAngle_);
    CB_DEPIMPL(PhysRelativeTime, cacheSegmentTime_);
    CB_DEPIMPL(PhysRelativeTime, cacheAccelerationTime_);
    CB_DEPIMPL(PhysRelativeTime, cacheDecelerationTime_);
    CB_DEPIMPL(MATHEX_SCALAR, cacheStartSpeed_);
    CB_DEPIMPL(bool, cacheIsRotationSegment_);
    CB_DEPIMPL(MATHEX_SCALAR,   cacheRotationLowerS_);

    PRE( cacheSegment_ < (*segmentTimesPtr_).size() );

    LINEAR_TRAVEL_STREAM( "Enter PhysLinearTravelPlan::computeCacheData " << (void*)this << std::endl );
    LINEAR_TRAVEL_INDENT( 2 );

    //Get the difference transform
    PhysMotionPlan::Times& segmentTimes = *segmentTimesPtr_;
    PhysMotionPlan::Transforms& transforms = *transformsPtr_;
    MexTransform3d differenceTransform;
    transforms[cacheSegment_].invert( &differenceTransform );
    differenceTransform.transform( transforms[cacheSegment_ + 1] );

    LINEAR_TRAVEL_INSPECT( cacheSegment_ );
    LINEAR_TRAVEL_INSPECT( transforms[cacheSegment_] );
    LINEAR_TRAVEL_INSPECT( transforms[cacheSegment_ + 1] );

    //Store the translation component of the difference transform
    differenceTransform.position( &cacheTranslation_ );

    LINEAR_TRAVEL_INSPECT( cacheTranslation_ );

    //Get the orientation component as an euler parameter set, and convert this
    //to rotation axis and angle.
    MexQuaternion params;
    differenceTransform.rotation( &params );
    cacheAngle_ = params.rotation( &cacheRotationAxis_ );

//    cacheAngle_ = ( transforms[ cacheSegment_ ].xBasis().signedAngleBetween(
//      transforms[ cacheSegment_ + 1 ].xBasis() ) ).asScalar();

    LINEAR_TRAVEL_INSPECT( cacheAngle_ );

    //  The angle produced by signedAngleBetween assumes that the axis of
    //  rotation points up rather than down so correct cacheRotationAxis_
    //  if necessary.

//    if( cacheRotationAxis_.z() < 0.0 )
//        cacheRotationAxis_ *= -1.0;

    //Store the segment time data
    cacheSegmentTime_ = ( cacheSegment_ == 0 ?
                          segmentTimes[0] :
                          segmentTimes[cacheSegment_] - segmentTimes[cacheSegment_ - 1] );
    cacheAccelerationTime_ = (*rampAccelerationsPtr_)[cacheSegment_].firstAccelerationTime();
    cacheDecelerationTime_ = (*rampAccelerationsPtr_)[cacheSegment_].secondAccelerationTime();
    cacheStartSpeed_ = (*rampAccelerationsPtr_)[cacheSegment_].startSpeed();
    cacheIsRotationSegment_ = isRotationSegment_[cacheSegment_];

    //  Calculate the point at which we're going to start any necessary rotation

    const MATHEX_SCALAR translationDistance = MexVec3( cacheTranslation_ ).modulus();

    if( translationDistance < startRotationDistance_ )
        cacheRotationLowerS_ = 0.0;
    else
        cacheRotationLowerS_ = ( translationDistance - startRotationDistance_ ) / translationDistance;

    cacheValid_ = true;

    LINEAR_TRAVEL_INDENT( -2 );
    LINEAR_TRAVEL_STREAM( "Exit PhysLinearTravelPlan::computeCacheData " << (void*)this << std::endl );

}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysLinearTravelPlan::computeInterpolatedTransform( const PhysRelativeTime& timeOffset,
                                                         MexTransform3d* pResult ) const
{
    CB_DEPIMPL(TimesPtr, segmentTimesPtr_);
    CB_DEPIMPL(bool, cacheValid_);
    CB_DEPIMPL(size_t, cacheSegment_);
    CB_DEPIMPL(PhysRelativeTime, cacheSegmentTime_);
    CB_DEPIMPL(bool, cacheIsRotationSegment_);
    CB_DEPIMPL(PhysRelativeTime, cacheAccelerationTime_);
    CB_DEPIMPL(RampAccelerationsPtr, rampAccelerationsPtr_);
    CB_DEPIMPL(PhysRelativeTime, cacheDecelerationTime_);
    CB_DEPIMPL(MATHEX_SCALAR, cacheStartSpeed_);
    CB_DEPIMPL(MATHEX_SCALAR,   cacheRotationLowerS_);
    CB_DEPIMPL(MexPoint3d, cacheTranslation_);
    CB_DEPIMPL(TransformsPtr, transformsPtr_);

    //CB_DEPIMPL(ctl_vector< bool >, isRotationSegment_);
    //CB_DEPIMPL(MATHEX_SCALAR,   startRotationDistance_);
    CB_DEPIMPL(MexVec3, cacheRotationAxis_);
    CB_DEPIMPL(MATHEX_SCALAR, cacheAngle_);

    PRE( cacheValid_ );

    LINEAR_TRAVEL_STREAM( "Enter PhysLinearTravelPlan::computeInterpolatedTransform " << (void*)this << std::endl );
    LINEAR_TRAVEL_INDENT( 2 );
    LINEAR_TRAVEL_INSPECT( timeOffset );

    //Compute the proportion of the distance between the current target position
    //and the previous position.
    PhysMotionPlan::Times& segmentTimes = *segmentTimesPtr_;
    PhysRelativeTime segmentTimeOffset =
        (cacheSegment_ == 0 ? timeOffset : timeOffset - segmentTimes[cacheSegment_ - 1]);

    LINEAR_TRAVEL_INSPECT( cacheIsRotationSegment_ );

    //Method used depends on whether rotating/translating
    MATHEX_SCALAR s;
    if( cacheIsRotationSegment_ )
    {
        s = PhysRampAcceleration::proportionOfDistance( cacheSegmentTime_,
                                                        cacheAccelerationTime_,
                                                        segmentTimeOffset );
    }
    else
    {
        MATHEX_SCALAR translationAcceleration = (*rampAccelerationsPtr_)[cacheSegment_].firstAcceleration();
        MATHEX_SCALAR translationDeceleration = (*rampAccelerationsPtr_)[cacheSegment_].secondAcceleration();

        s = PhysRampAcceleration::proportionOfDistance( cacheStartSpeed_,
                                                        translationAcceleration,
                                                        translationDeceleration,
                                                        cacheSegmentTime_,
                                                        cacheAccelerationTime_,
                                                        cacheDecelerationTime_,
                                                        segmentTimeOffset );
    }

    LINEAR_TRAVEL_INSPECT( s );

    //hence construct the difference transform a proportion s between the 2 positions
    MexQuaternion params;

    MATHEX_SCALAR rotationS;

    //  See if we have reached the point on this segment to start rotating
    if( s < cacheRotationLowerS_ )
        rotationS = 0.0;
    else
        rotationS = mexInterpolate( s, cacheRotationLowerS_, 1.0, 0.0, 1.0 );

    LINEAR_TRAVEL_INSPECT( cacheRotationLowerS_ );
    LINEAR_TRAVEL_INSPECT( rotationS );

    params.set( cacheRotationAxis_, cacheAngle_ * rotationS );

    MexPoint3d pos(
        cacheTranslation_.x() * s,
        cacheTranslation_.y() * s,
        cacheTranslation_.z() * s );

    MexTransform3d interpolatedTransform;
    interpolatedTransform.position( pos );
    interpolatedTransform.rotation( params );

    //Combine this transform with the begin transform
    (*transformsPtr_)[cacheSegment_].transform( interpolatedTransform, pResult );

    LINEAR_TRAVEL_INSPECT( *pResult );
    LINEAR_TRAVEL_INDENT( -2 );
    LINEAR_TRAVEL_STREAM( "Exit PhysLinearTravelPlan::computeInterpolatedTransform " << (void*)this << std::endl );
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysLinearTravelPlan::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}
//////////////////////////////////////////////////////////////////////////////////////////

void PhysLinearTravelPlan::startRotationDistance( MATHEX_SCALAR distance )
{
    CB_DEPIMPL(MATHEX_SCALAR,   startRotationDistance_);
    startRotationDistance_ = distance;
}

size_t PhysLinearTravelPlan::nSegments() const
{
	return segmentTimes()->size();
}

const PhysMotionPlan::TimesPtr& PhysLinearTravelPlan::segmentTimes() const
{
	return pImpl_->segmentTimesPtr_;
}

const PhysMotionPlan::RampAccelerationsPtr& PhysLinearTravelPlan::rampAccelerations() const
{
	return pImpl_->rampAccelerationsPtr_;
}


ostream& operator <<( ostream& o, const PhysLinearTravelPlan& t )
{

    o << "PhysLinearTravelPlan " << (void*)&t << " start" << std::endl;
    o << (*t.pImpl_) << std::endl;
    o << "PhysLinearTravelPlan " << (void*)&t << " end" << std::endl;

    return o;
}

PhysLinearTravelPlan::PhysLinearTravelPlan( PerConstructor con )
: PhysMotionPlan( con )
{
}

void perWrite( PerOstream& ostr, const PhysLinearTravelPlan& plan )
{
    const PhysMotionPlan& base = plan;

    ostr << base;

    ostr << plan.pImpl_;
}

void perRead( PerIstream& istr, PhysLinearTravelPlan& plan )
{
    PhysMotionPlan& base = plan;

    istr >> base;

    istr >> plan.pImpl_;
}

//////////////////////////////////////////////////////////////////////////////////////////
/* End LINETRAV.CPP *************************************************/
