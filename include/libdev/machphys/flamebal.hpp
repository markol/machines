/*
 * F L A M E B A L . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysFlameBall
*/

#ifndef _MACHPHYS_FLAMEBAL_HPP
#define _MACHPHYS_FLAMEBAL_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"
#include "machphys/lineproj.hpp"

class MachPhysFlameBall : public MachPhysLinearProjectile
// Canonical form revoked
{
public:
    //ctor
	enum FlameBallType : unsigned int {GRUNT_FLAME_BALL, TURRET_FLAME_BALL};
    MachPhysFlameBall( W4dEntity* pParent, const MexTransform3d& localTransform, FlameBallType type);

    //dtor
    virtual ~MachPhysFlameBall();

	static const MachPhysFlameBall& exemplar( FlameBallType type );

    //Set up the motion and other plans for the flameball projectile.
    //Starts moving at launchTime, and moves from startTransform to endTransform.
    //Starts running through the flameball material plan materialPlanCycleStartDelay
    //after start of motion.
    //Flameball to change scale uniformly from 1 to endScale over the flight.
    PhysRelativeTime move
    (
    	const PhysAbsoluteTime& launchTime,
    	const MexTransform3d& startTransform,
    	const MexTransform3d& endTransform,
    	const PhysRelativeTime& materialPlanCycleStartDelay,
    	MATHEX_SCALAR endScale
    );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysFlameBall& t );

    PER_MEMBER_PERSISTENT( MachPhysFlameBall );

private:
    //Deliberately revoked
    MachPhysFlameBall( const MachPhysFlameBall& );
    MachPhysFlameBall& operator =( const MachPhysFlameBall& );
    bool operator ==( const MachPhysFlameBall& );

	friend class MachPhysOtherPersistence;
    //One-time constructor used to create the exemplar
    MachPhysFlameBall( FlameBallType type );
};

PER_READ_WRITE( MachPhysFlameBall );
PER_DECLARE_PERSISTENT( MachPhysFlameBall );
PER_ENUM_PERSISTENT( MachPhysFlameBall::FlameBallType );

#endif

/* End FLAMEBAL.HPP *************************************************/
