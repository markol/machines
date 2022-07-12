/*
 * A C C E T U M B . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    PhysAccelerateTumblePlan

    Implement a motion plan that has an object accelerating through
    space and tumbling in a pseudo random manner.
*/

#ifndef _PHYS_ACCETUMB_HPP
#define _PHYS_ACCETUMB_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "mathex/mathex.hpp"

#include "phys/motplan.hpp"

class PhysAccelerateTumblePlanImpl;
class PhysEulerTransform3d;
class MexVec3;

template <class T> class ctl_vector;
template <class T> class CtlCountedPtr;

class PhysAccelerateTumblePlan : public PhysMotionPlan
{
public:

    typedef ctl_vector< PhysEulerTransform3d > EulerTransforms;
    typedef CtlCountedPtr< EulerTransforms > EulerTransformsPtr;

    PhysAccelerateTumblePlan(
        const EulerTransformsPtr& transformsPtr,
        const TimesPtr& timesPtr,
        const MexVec3& acceleration );
    //  PRE( transformsPtr->size() == durationPtr->size() + 1 );
    //  PRE( inAscendingOrder( *timesPtr ) );

    virtual ~PhysAccelerateTumblePlan();

    //Override defines result as a function of timeOffset.
    //If time is greater than duration, the transform at time duration is returned.
    virtual void transform( const PhysRelativeTime& timeOffset, MexTransform3d* pResult) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PhysAccelerateTumblePlan& t );

    PER_MEMBER_PERSISTENT( PhysAccelerateTumblePlan );
    PER_FRIEND_READ_WRITE( PhysAccelerateTumblePlan );
private:
    // Operation deliberately revoked
    PhysAccelerateTumblePlan( const PhysAccelerateTumblePlan& );

    // Operation deliberately revoked
    PhysAccelerateTumblePlan& operator =( const PhysAccelerateTumblePlan& );

    // Operation deliberately revoked
    bool operator ==( const PhysAccelerateTumblePlan& );

    PhysRelativeTime    currentSegmentDuration() const;

    void setPosition( const PhysRelativeTime& timeOffset, MexTransform3d* pResult ) const;
    void setRotation( const PhysRelativeTime& timeOffset, MexTransform3d* pResult ) const;

    void computeCacheData();

/*
    EulerTransformsPtr   transformsPtr_;
    TimesPtr        segmentTimesPtr_;

    MexVec3         acceleration_;

    //  Cached data - updated as we change segments
    size_t          currentSegmentIndex_;
    MexEulerAngles  deltaOrientation_;
    MexEulerAngles  initialOrientation_;
    MexVec3         initialVelocity_;
*/
	PhysAccelerateTumblePlanImpl* pImpl_;
};

PER_DECLARE_PERSISTENT( PhysAccelerateTumblePlan );

#endif

/* End ACCETUMB.HPP *************************************************/
