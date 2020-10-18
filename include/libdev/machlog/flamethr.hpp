/*
 * F L A M E T H R  . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogFlameThrower

*/

#ifndef _MACHLOG_FLAMETHR_HPP
#define _MACHLOG_FLAMETHR_HPP

#include "base/base.hpp"
#include "machlog/wepline.hpp"

class MachPhysLinearWeapon;

class MachLogFlameThrower : public MachLogLinearWeapon
// Canonical form revoked
{
public:
    MachLogFlameThrower( MachLogRace*, MachPhysLinearWeapon*, MachActor* pOwner );
    virtual ~MachLogFlameThrower( void );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogFlameThrower& t );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogFlameThrower );
	PER_FRIEND_READ_WRITE( MachLogFlameThrower );

protected:

	virtual	MachLogLinearProjectile* createLinearProjectile
	(
		const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
	    const W4dEntity& target, const MexPoint3d& targetOffset
	);

private:
    MachLogFlameThrower( const MachLogFlameThrower& );
    MachLogFlameThrower& operator =( const MachLogFlameThrower& );
    bool operator ==( const MachLogFlameThrower& );

};

PER_DECLARE_PERSISTENT( MachLogFlameThrower );

#endif

/* End PULSEWEP.HPP *************************************************/
