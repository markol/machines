/*
 * J E T R I N G P . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysJetRingPlan

    make a motion plan for the ring in the constructor construction animation:
	as the jet moves with the claw of the constructor, the ring moves accordingly.
	However, its orientation remains to be vertical in the world.

*/

#ifndef _MACHPHYS_JETRINGP_HPP
#define _MACHPHYS_JETRINGP_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "mathex/point3d.hpp"

#include "phys/motplan.hpp"

class W4dEntity;

class MachPhysJetRingPlan : public PhysMotionPlan
// Canonical form revoked
{
public:
    MachPhysJetRingPlan( W4dEntity* pEntity, const MexPoint3d& targetOffsetGlobal, const PhysRelativeTime& duration );
    ~MachPhysJetRingPlan( void );

    virtual void transform( const PhysRelativeTime& timeOffset, MexTransform3d* pResult) const;
    //PRE( pResult != NULL )

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT( MachPhysJetRingPlan );
    PER_FRIEND_READ_WRITE( MachPhysJetRingPlan );

private:
    friend ostream& operator <<( ostream& o, const MachPhysJetRingPlan& t );

    MachPhysJetRingPlan( const MachPhysJetRingPlan& );
    MachPhysJetRingPlan& operator =( const MachPhysJetRingPlan& );

	W4dEntity* pEntity_; //the entity the plan is going to be applied to.
	MexPoint3d localPosition_;
	MexPoint3d targetOffsetGlobal_;
};

PER_DECLARE_PERSISTENT( MachPhysJetRingPlan );

#endif

/* End JETRINGP.HPP **************************************************/
