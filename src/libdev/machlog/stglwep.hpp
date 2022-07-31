/*
 * S T G L W E P . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogLightStingWeapon

*/

#ifndef _MACHLOG_STGLWEP_HPP
#define _MACHLOG_STGLWEP_HPP

#include "base/base.hpp"
#include "machlog/wepline.hpp"

class MachPhysLinearWeapon;

class MachLogLightStingWeapon : public MachLogLinearWeapon
// Canonical form revoked
{
public:
    MachLogLightStingWeapon( MachLogRace*, MachPhysLinearWeapon*, MachActor* pOwner );
    virtual ~MachLogLightStingWeapon();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogLightStingWeapon& t );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogLightStingWeapon );
	PER_FRIEND_READ_WRITE( MachLogLightStingWeapon );

protected:

	virtual	MachLogLinearProjectile* createLinearProjectile
	(
		const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
	    const W4dEntity& target, const MexPoint3d& targetOffset
	);

private:
    MachLogLightStingWeapon( const MachLogLightStingWeapon& );
    MachLogLightStingWeapon& operator =( const MachLogLightStingWeapon& );
    bool operator ==( const MachLogLightStingWeapon& );

};

PER_DECLARE_PERSISTENT( MachLogLightStingWeapon );

#endif

/* End STGLWEP.HPP *************************************************/
