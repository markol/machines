/*
 * L I N E T R A I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PhysLinearTravelPlanImpl

    A brief description of the class should go in here
*/

#ifndef _PHYS_LINETRAI_HPP
#define _PHYS_LINETRAI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "mathex/mathex.hpp"
#include "mathex/point3d.hpp"
#include "mathex/vec3.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/radians.hpp"
#include "phys/rampacce.hpp"

#include "ctl/countptr.hpp"
#include "ctl/vector.hpp"

#include "phys/motplan.hpp"

class PhysLinearTravelPlanImpl
// Canonical form revoked
{
public:

    PER_MEMBER_PERSISTENT( PhysLinearTravelPlanImpl );
    PER_FRIEND_READ_WRITE( PhysLinearTravelPlanImpl );

private:

    PhysLinearTravelPlanImpl( const PhysMotionPlan::TransformsPtr& transformsPtr );

    PhysLinearTravelPlanImpl(
      const PhysMotionPlan::TransformsPtr& transformsPtr,
      const PhysMotionPlan::RampAccelerationsPtr& rampAccelerationsPtr );

    PhysLinearTravelPlanImpl( void );
    ~PhysLinearTravelPlanImpl( void );

    PhysLinearTravelPlanImpl( const PhysLinearTravelPlanImpl& );
    PhysLinearTravelPlanImpl& operator =( const PhysLinearTravelPlanImpl& );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PhysLinearTravelPlanImpl& t );
	friend class PhysLinearTravelPlan;

    PhysMotionPlan::TransformsPtr transformsPtr_;
    PhysMotionPlan::RampAccelerationsPtr rampAccelerationsPtr_;

    PhysMotionPlan::TimesPtr segmentTimesPtr_;

    ctl_vector< bool > isRotationSegment_;

    MATHEX_SCALAR   startRotationDistance_; //  The distance before a turn point to start rotating to face the new direction
    
    //Cached transform data

    bool cacheValid_; //true iff the cache data exists
    size_t cacheSegment_; //Segment number of the cached data
    MexPoint3d cacheTranslation_; //Translation component of modifying transform
    MexVec3 cacheRotationAxis_; //Unit direction vector for axis of rotation change
    MATHEX_SCALAR cacheAngle_; //Angle of full rotation between bounding positions
    PhysRelativeTime cacheSegmentTime_;//Time spent in cached segment
    PhysRelativeTime cacheAccelerationTime_;//Time spent accelerating in cached segment
    PhysRelativeTime cacheDecelerationTime_;//Time spent decelerating in cached segment
    MATHEX_SCALAR cacheStartSpeed_;
    bool cacheIsRotationSegment_;
    MATHEX_SCALAR   cacheRotationLowerS_;   //The proportion along the move at which we start the turn

};

PER_DECLARE_PERSISTENT( PhysLinearTravelPlanImpl );

#endif

/* End LINETRAI.HPP *************************************************/
