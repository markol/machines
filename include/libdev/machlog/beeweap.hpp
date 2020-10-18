/*
 * B E E W E A P . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogBeeBomber

*/

#ifndef _MACHLOG_BEEWEAP_HPP
#define _MACHLOG_BEEWEAP_HPP

#include "base/base.hpp"
#include "machlog/wepline.hpp"

class MachPhysLinearWeapon;

class MachLogBeeBomber : public MachLogLinearWeapon
// Canonical form revoked
{
public:
    MachLogBeeBomber( MachLogRace*, MachPhysLinearWeapon*, MachActor* pOwner );
    virtual ~MachLogBeeBomber( void );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogBeeBomber& t );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogBeeBomber );
	PER_FRIEND_READ_WRITE( MachLogBeeBomber );

protected:

	virtual	MachLogLinearProjectile* createLinearProjectile
	(
		const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
	    const W4dEntity& target, const MexPoint3d& targetOffset
	);

private:
    MachLogBeeBomber( const MachLogBeeBomber& );
    MachLogBeeBomber& operator =( const MachLogBeeBomber& );
    bool operator ==( const MachLogBeeBomber& );

};

PER_DECLARE_PERSISTENT( MachLogBeeBomber );

#endif

/* End BEEWEAP.HPP *************************************************/
