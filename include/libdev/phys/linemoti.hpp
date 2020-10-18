/*
 * L I N E M O T I . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    PhysLinearMotionPlan

    Implements a mapping from time to transform, with optional multiple transforms,
    each with a specified time offset from start of motion.
*/

#ifndef _LINEMOTI_HPP
#define _LINEMOTI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/motplan.hpp"
//#include "phys/jonvec.hpp"

class PhysLinearMotionPlanImpl;

//orthodox canonical revoked
class PhysLinearMotionPlan : public PhysMotionPlan
{
public:
    //Initial motion plan is to move from startPosition at time 0 to endPosition
    //at time endTime.
    PhysLinearMotionPlan( const MexTransform3d& startPosition,
                          const MexTransform3d& endPosition,
                          const PhysRelativeTime& endTime);

    ~PhysLinearMotionPlan( void );

    //Extends the motion path to end at endPosition at time EndTime
    void add( const MexTransform3d& endPosition, const PhysRelativeTime& endTime );
    //PRE( endTime >= duration )

    //Compute the defined transform at time timeOffset, and return in pResult.
    //If timeOffset is > duration, return final position.
    virtual void transform( const PhysRelativeTime& timeOffset, MexTransform3d* pResult) const;
    //PRE( pResult != NULL )

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PhysLinearMotionPlan& t );

    PER_MEMBER_PERSISTENT( PhysLinearMotionPlan );
    PER_FRIEND_READ_WRITE( PhysLinearMotionPlan );

private:
    // Operation deliberately revoked
    PhysLinearMotionPlan( const PhysLinearMotionPlan& );

    // Operation deliberately revoked
    PhysLinearMotionPlan& operator =( const PhysLinearMotionPlan& );

    // Operation deliberately revoked
    bool operator ==( const PhysLinearMotionPlan& );

    // Set the cache data for the current targetIndex_
    void computeCacheData();

    //Compute the interpolated transform at time timeOffset and return it in pResult.
    void computeInterpolatedTransform( const PhysRelativeTime& timeOffset,
                                       MexTransform3d* pResult ) const;
    //PRE( interpolationCached_ )


	PhysLinearMotionPlanImpl* pImpl_;
};

PER_DECLARE_PERSISTENT( PhysLinearMotionPlan );

#endif

/* End LINEMOTI.HPP *************************************************/
