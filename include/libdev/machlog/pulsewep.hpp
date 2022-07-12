/*
 * P U L S E W E P . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogPulseWeapon

    Pulse weapons:
		Rifle and cannon
*/

#ifndef _MACHLOG_PULSEWEP_HPP
#define _MACHLOG_PULSEWEP_HPP

#include "base/base.hpp"
#include "machlog/wepline.hpp"

class MachPhysLinearWeapon;

class MachLogPulseWeapon : public MachLogLinearWeapon
// Canonical form revoked
{
public:
    MachLogPulseWeapon( MachLogRace*, MachPhysLinearWeapon*, MachActor* pOwner );
    virtual ~MachLogPulseWeapon();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogPulseWeapon& t );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogPulseWeapon );
	PER_FRIEND_READ_WRITE( MachLogPulseWeapon );

protected:

	virtual	MachLogLinearProjectile* createLinearProjectile
	(
		const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
	    const W4dEntity& target, const MexPoint3d& targetOffset
	);

private:
    MachLogPulseWeapon( const MachLogPulseWeapon& );
    MachLogPulseWeapon& operator =( const MachLogPulseWeapon& );
    bool operator ==( const MachLogPulseWeapon& );

};

PER_DECLARE_PERSISTENT( MachLogPulseWeapon );

#endif

/* End PULSEWEP.HPP *************************************************/
