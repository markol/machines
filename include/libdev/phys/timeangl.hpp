/*
 * T I M E A N G L . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    PhysTimedAnglePlan

    IS-A PhysMotionPlan which defines rotations about a single axis between
    a predefined list of angles. The time at which it is to arrive at each
    angle is specified.
*/

#ifndef _TIMEANGL_HPP
#define _TIMEANGL_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/motplan.hpp"

//forward refs
class MexVec3;
class MexTransform3d;
class PhysTimedAnglePlanImpl;

//orthodox canonical
class PhysTimedAnglePlan : public PhysMotionPlan
{
public:
    //anglesPtr references a collection of the required angles. First angle is at time 0.
    //timesPtr references a collection of the times at which we reach the angles.
    //axis is the axis about which we rotate.
    //position is the constant location component of the resulting transform.
    PhysTimedAnglePlan( const PhysMotionPlan::AnglesPtr& anglesPtr,
                        const PhysMotionPlan::TimesPtr& timesPtr,
                        const MexVec3& axis, const MexVec3& position );
    //PRE( (*anglesPtr).size() > 1 )
    //PRE( (*timesPtr).size() == (*anglesPtr).size() - 1 )
    //PRE( times in timesPtr in ascending order )

    //dtor
    ~PhysTimedAnglePlan();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PhysTimedAnglePlan& t );

    //Override defines pResult as a function of timeOffset.
    //If time is greater than duration, the transform at time duration is returned.
    virtual void transform( const PhysRelativeTime& timeOffset, MexTransform3d* pResult ) const;

	// Return the angle as a function of timeOffset.
	MexRadians angle(const PhysRelativeTime& timeOffset) const;

    PER_MEMBER_PERSISTENT( PhysTimedAnglePlan );
    PER_FRIEND_READ_WRITE( PhysTimedAnglePlan );

private:
    // Operation deliberately revoked
    PhysTimedAnglePlan( const PhysTimedAnglePlan& );
    PhysTimedAnglePlan& operator =( const PhysTimedAnglePlan& );
    bool operator ==( const PhysTimedAnglePlan& );

    PhysTimedAnglePlanImpl* pImpl_;
};

PER_DECLARE_PERSISTENT( PhysTimedAnglePlan );

#endif

/* End TIMEANGL.HPP *************************************************/
