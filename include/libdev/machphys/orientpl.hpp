/*
 * O R I E N T P L . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysOrientationAndPositionPlan

    make a motion plan for an element in the constructor construction animation,
    such as the light shadow, so that it moves with the claw of the constructor accordingly.
    At the same time, the shadow is parelle to the floor and stays just on the floor.
*/

#ifndef _MACHPHYS_ORIENTPL_HPP
#define _MACHPHYS_ORIENTPL_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "phys/motplan.hpp"
#include "mathex/point3d.hpp"

class W4dEntity;

class MachPhysOrientationAndPositionPlan : public PhysMotionPlan
// Canonical form revoked
{
public:
    MachPhysOrientationAndPositionPlan( W4dEntity* pEntity,  const PhysRelativeTime& duration );
    ~MachPhysOrientationAndPositionPlan();

    virtual void transform( const PhysRelativeTime& timeOffset, MexTransform3d* pResult) const;
    //PRE( pResult != NULL )

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( MachPhysOrientationAndPositionPlan );
    PER_FRIEND_READ_WRITE( MachPhysOrientationAndPositionPlan );

private:
    friend ostream& operator <<( ostream& o, const MachPhysOrientationAndPositionPlan& t );

    MachPhysOrientationAndPositionPlan( const MachPhysOrientationAndPositionPlan& );
    MachPhysOrientationAndPositionPlan& operator =( const MachPhysOrientationAndPositionPlan& );

	W4dEntity* pEntity_; //the entity the plan is going to be applied to.
	MexPoint3d localPosition_;
	MATHEX_SCALAR zGlobal_;
};

PER_DECLARE_PERSISTENT( MachPhysOrientationAndPositionPlan );

#endif

/* End ORIENTPL.HPP **************************************************/
