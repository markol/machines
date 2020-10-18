/*
 * F L A M E T H R  . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogElectroCharger

*/

#ifndef _MACHLOG_ELECHARG_HPP
#define _MACHLOG_ELECHARG_HPP

#include "base/base.hpp"
#include "machlog/wepline.hpp"

class MachPhysLinearWeapon;

class MachLogElectroCharger : public MachLogLinearWeapon
// Canonical form revoked
{
public:
    MachLogElectroCharger( MachLogRace*, MachPhysLinearWeapon*, MachActor* pOwner );
    virtual ~MachLogElectroCharger( void );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogElectroCharger& t );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogElectroCharger );
	PER_FRIEND_READ_WRITE( MachLogElectroCharger );

protected:

	virtual	MachLogLinearProjectile* createLinearProjectile
	(
		const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
	    const W4dEntity& target, const MexPoint3d& targetOffset
	);

private:
    MachLogElectroCharger( const MachLogElectroCharger& );
    MachLogElectroCharger& operator =( const MachLogElectroCharger& );
    bool operator ==( const MachLogElectroCharger& );

};

PER_DECLARE_PERSISTENT( MachLogElectroCharger );

#endif

/* End PULSEWEP.HPP *************************************************/
