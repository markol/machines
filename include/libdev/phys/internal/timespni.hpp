/*
 * T I M E S P N I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    PhysTimedSpinPlanImpl

    A brief description of the class should go in here
*/

#ifndef _PHYS_TIMESPNI_HPP
#define _PHYS_TIMESPNI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/radians.hpp"
#include "mathex/vec3.hpp"

#include "ctl/countptr.hpp"
#include "ctl/vector.hpp"

#include "phys/motplan.hpp"

class PhysTimedSpinPlanImpl
// Canonical form revoked
{
public:

    PER_MEMBER_PERSISTENT( PhysTimedSpinPlanImpl );
    PER_FRIEND_READ_WRITE( PhysTimedSpinPlanImpl );

private:

    PhysTimedSpinPlanImpl( const MexVec3& axis, const MexPoint3d& position, const MexRadians& startAngle, const MexRadians& startSpeed );
    PhysTimedSpinPlanImpl( const MexVec3& axis, const MexTransform3d& baseTransform, const MexRadians& startAngle, const MexRadians& startSpeed );

    ~PhysTimedSpinPlanImpl();

    PhysTimedSpinPlanImpl( const PhysTimedSpinPlanImpl& );
    PhysTimedSpinPlanImpl& operator =( const PhysTimedSpinPlanImpl& );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const PhysTimedSpinPlanImpl& t );
	friend class PhysTimedSpinPlan;

    //data members
    MexVec3 axis_; // Axis of rotation
    MexTransform3d baseTransform_; //Constant location component of the transform
    MexRadians startAngle_; //Angle at start of motion
    MexRadians startSpeed_; //Initial rate of rotation (rad/sec)
    PhysMotionPlan::Times times_; //cumulative segment end times
    PhysMotionPlan::Angles accelerations_; //The acceleration rate for each segment (rad/sec/sec)
    PhysMotionPlan::Angles endAngles_; //Caches the end angle for each segment
    PhysMotionPlan::Angles endSpeeds_; //Caches the ending rotation rate for each segment
    size_t cacheIndex_; //Cached segment index
};

PER_DECLARE_PERSISTENT( PhysTimedSpinPlanImpl );

#endif

/* End TIMESPNI.HPP *************************************************/
