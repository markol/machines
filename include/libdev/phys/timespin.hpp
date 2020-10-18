/*
 * T I M E S P I N . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    PhysTimedSpinPlan

    Defines a transform mapping as a function of time, which represents
    rotation about a single axis. The rate of rotation is governed by a terminal
    velocity and an acceleration rate. The plan is divided into timed segments, and
    the rotation rate is to be zero at the start and end of each segment.
    Each segment also has a flag associated with it indicating whether motion during the
    segment is at rest, forwards, or backwards.
*/

#ifndef _TIMESPIN_HPP
#define _TIMESPIN_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "mathex/mathex.hpp"
#include "phys/phys.hpp"

#include "phys/motplan.hpp"

class MexPoint3d;
class MexVec3;
class MexRadians;
class MexTransform3d;
class PhysTimedSpinPlanImpl;

//orthodox canonical( revoked )
class PhysTimedSpinPlan : public PhysMotionPlan
{
public:
    //ctor. Orientation is defined by rotation about axis beginning at startAngle.
    //The location component of the transform is constant at position.
    //The initial angle to use is startAngle, and the initial rate of rotation is
    //startSpeed (actually radians/sec). Initially has a duration of zero,
    //so the startAngle would always be implied.
    //likelySegmentCount indicates the likely number of segments to be added, and is used
    //to initialise the size of internal vectors etc.
    PhysTimedSpinPlan( const MexVec3& axis, const MexPoint3d& position,
                       const MexRadians& startAngle, const MexRadians& startSpeed,
                       size_t likelySegmentCount );

    PhysTimedSpinPlan( const MexVec3& axis, const MexTransform3d& baseTransform,
                       const MexRadians& startAngle, const MexRadians& startSpeed,
                       size_t likelySegmentCount );

    //dtor.
    ~PhysTimedSpinPlan( void );

    //Add another segment to the plan of duration segmentDuration, during which time
    //the spin rate changes with specified acceleration, which may be negative to
    //denote deceleration, or zero to imply constant velocity period.
    //If the resulting implied speed at the end of the segment is close to zero,
    //it will be reset to zero.
    void addSegment( const PhysRelativeTime& segmentDuration, const MexRadians& acceleration );

    //Override defines result as a function of timeOffset.
    //If time is greater than duration, the transform at time duration is returned.
    virtual void transform( const PhysRelativeTime& timeOffset, MexTransform3d* pResult) const;
    //PRE( timeOffset >= 0 )

    //The angle at time timeOffset.
    MexRadians angle( const PhysRelativeTime& timeOffset ) const;
    //PRE( timeOffset >= 0 )

    //The rotation rate at time timeOffset.
    MexRadians speed( const PhysRelativeTime& timeOffset ) const;
    //PRE( timeOffset >= 0 )

    //Inherited from PhysMotionPlan
    //const PhysRelativeTime& duration( void ) const;
    //bool isDone( const PhysRelativeTime& timeOffset ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PhysTimedSpinPlan& t );

    PER_MEMBER_PERSISTENT( PhysTimedSpinPlan );
    PER_FRIEND_READ_WRITE( PhysTimedSpinPlan );

private:
    // Operations deliberately revoked
    PhysTimedSpinPlan( const PhysTimedSpinPlan& );
    PhysTimedSpinPlan& operator =( const PhysTimedSpinPlan& );
    bool operator ==( const PhysTimedSpinPlan& );

    //Set cacheIndex_ to the segment containing time timeOffset
    void setCacheSegment( const PhysRelativeTime& timeOffset );

	PhysTimedSpinPlanImpl* pImpl_;
};

PER_DECLARE_PERSISTENT( PhysTimedSpinPlan );

#endif

/* End TIMESPIN.HPP *************************************************/
