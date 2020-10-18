/*
 * P U L S E W E P . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogTreacheryWeapon

	This is the logical representation of the trechary weapon.
*/

#ifndef _MACHLOG_TREACHERY_HPP
#define _MACHLOG_TREACHERY_HPP

#include "base/base.hpp"
#include "machlog/wepline.hpp"

class MachPhysLinearWeapon;

class MachLogTreacheryWeapon : public MachLogLinearWeapon
// Canonical form revoked
{
public:
    MachLogTreacheryWeapon( MachLogRace*, MachPhysLinearWeapon*, MachActor* pOwner );
    virtual ~MachLogTreacheryWeapon( void );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogTreacheryWeapon& t );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogTreacheryWeapon );
	PER_FRIEND_READ_WRITE( MachLogTreacheryWeapon );

protected:

	virtual	MachLogLinearProjectile* createLinearProjectile
	(
		const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
	    const W4dEntity& target, const MexPoint3d& targetOffset
	);

private:
    MachLogTreacheryWeapon( const MachLogTreacheryWeapon& );
    MachLogTreacheryWeapon& operator =( const MachLogTreacheryWeapon& );
    bool operator ==( const MachLogTreacheryWeapon& );

};

PER_DECLARE_PERSISTENT( MachLogTreacheryWeapon );

#endif

/* End Treachry.HPP *************************************************/
