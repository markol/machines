/*
 * N U C L W E A P . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogNuclearWeapon - a device capable of launching MLNuclearMissiles at targets.

		Rifle and cannon
*/

#ifndef _MACHLOG_NUCLWEAP_HPP
#define _MACHLOG_NUCLWEAP_HPP

#include "base/base.hpp"
#include "machlog/wepline.hpp"

class MachPhysLinearWeapon;

class MachLogNuclearWeapon : public MachLogLinearWeapon
// Canonical form revoked
{
public:
    MachLogNuclearWeapon( MachLogRace*, MachPhysLinearWeapon*, MachActor* pOwner );
    virtual ~MachLogNuclearWeapon();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogNuclearWeapon& t );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogNuclearWeapon );
	PER_FRIEND_READ_WRITE( MachLogNuclearWeapon );

protected:

	virtual	MachLogLinearProjectile* createLinearProjectile
	(
		const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
	    const W4dEntity& target, const MexPoint3d& targetOffset
	);

private:
    MachLogNuclearWeapon( const MachLogNuclearWeapon& );
    MachLogNuclearWeapon& operator =( const MachLogNuclearWeapon& );
    bool operator ==( const MachLogNuclearWeapon& );

};

PER_DECLARE_PERSISTENT( MachLogNuclearWeapon );

#endif

/* End NUCLWEAP.HPP *************************************************/
