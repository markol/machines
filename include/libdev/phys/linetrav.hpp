/*
 * L I N E T R A V . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    PhysLinearTravelPlan

    Implement a transform mapping between a collection of transform positions.
    Travel rates are supplied for translational motion and rotational motion.
    Acceleration rates must also be supplied.
    Journey times and acceleration times are computed and exported.
*/

#ifndef _LINETRAV_HPP
#define _LINETRAV_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/motplan.hpp"

class PhysLinearTravelPlanImpl;

//orthodox canonical (revoked)
class PhysLinearTravelPlan : public PhysMotionPlan
{
public:
    //Define the collection of transforms, and the terminal translational and
    //rotational speeds to use. If a journey segment between 2 transforms requires
    //only rotation with no translation, the orientation change proceeds at the rotation
    //speed specified. Otherwise the orientation change is linearly interpolated between
    //positions.
    //Initial translation and rotational speeds respectively given by
    //startTranslationSpeed and startRotationSpeed.
    //If comeToRest is true, must come to rest at end of journey, and between each
    //point. If not coming to rest, it is assumed each segment of the journey
    //is long enough to attain the terminal speed.
    PhysLinearTravelPlan( const PhysMotionPlan::TransformsPtr& transformsPtr,
                          MATHEX_SCALAR startTranslationSpeed,
                          MATHEX_SCALAR translationSpeed,
                          MATHEX_SCALAR translationAcceleration,
                          MATHEX_SCALAR translationDeceleration,
                          MATHEX_SCALAR startRotationSpeed,
                          MATHEX_SCALAR rotationSpeed,
                          MATHEX_SCALAR rotationAcceleration,
                          MATHEX_SCALAR rotationDeceleration,
                          bool comeToRest );
    //PRE( transformsPtr.isDefined() );
    //PRE( transformsPtr->size() >= 2 );
    //PRE( startTranslationSpeed > 0.0 );
    //PRE( translationSpeed > 0.0 );
    //PRE( translationAcceleration > 0.0 );
    //PRE( translationDeceleration > 0.0 );
    //PRE( startRotationSpeed > 0.0 );
    //PRE( rotationSpeed > 0.0 );
    //PRE( rotationAcceleration > 0.0 );
    //PRE( rotationDeceleration > 0.0 );


    PhysLinearTravelPlan(
      const PhysMotionPlan::TransformsPtr& transformsPtr,
      const PhysMotionPlan::RampAccelerationsPtr& rampAccelerationsPtr );
    //  PRE( transformsPtr.isDefined() );
    //  PRE( transformsPtr->size() >= 2 );
    //  PRE( rampAccelerationsPtr.isDefined() );
    //  PRE( transformsPtr->size() == rampAccelerationsPtr->size() + 1 );

    //dtor
    ~PhysLinearTravelPlan( void );

    //Export the cumulative travel times for each segment of the journey.
    //The number of times is 1 less than the number of transforms supplied.
    const PhysMotionPlan::TimesPtr& segmentTimes( void ) const;

    size_t  nSegments() const;

    //Export the ramp accelerations for each segment of the journey
    //The number of accelerations is 1 less than the number of transforms supplied.
    const PhysMotionPlan::RampAccelerationsPtr& rampAccelerations( void ) const;

    //Override defines result as a function of timeOffset.
    //If time is greater than duration, the transform at time duration is returned.
    virtual void transform( const PhysRelativeTime& timeOffset, MexTransform3d* pResult) const;

    // Set the distance before a turn point to start rotating to face
    // the new direction by the time we reach the turn point
    void startRotationDistance( MATHEX_SCALAR distance );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PhysLinearTravelPlan& t );

    PER_MEMBER_PERSISTENT( PhysLinearTravelPlan );
    PER_FRIEND_READ_WRITE( PhysLinearTravelPlan );

private:
    // Operations deliberately revoked
    PhysLinearTravelPlan( const PhysLinearTravelPlan& );
    PhysLinearTravelPlan& operator =( const PhysLinearTravelPlan& );
    bool operator ==( const PhysLinearTravelPlan& );

    //Load the cached data for segment cacheSegment_;
    void computeCacheData( void );

    //Compute the interpolated transform at time timeOffset from start of plan.
    //Returns transform in pResult.
    void computeInterpolatedTransform( const PhysRelativeTime& timeOffset,
                                       MexTransform3d* pResult ) const;
    //PRE( timeOffset lies in cached segment );

    PhysLinearTravelPlanImpl* pImpl_;
};

PER_DECLARE_PERSISTENT( PhysLinearTravelPlan );

#endif

/* End LINETRAV.HPP *************************************************/
