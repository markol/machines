/*
 * L M I S S L C H . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogLargeMissileLauncher - a device capable of launching MLLargeMissiles at targets.

*/

#ifndef _MACHLOG_LMISSLCH_HPP
#define _MACHLOG_LMISSLCH_HPP

#include "base/base.hpp"
#include "machlog/wepline.hpp"

class MachPhysLinearWeapon;
class MachPhysLargeMissile;

class MachLogLargeMissileLauncher : public MachLogLinearWeapon
// Canonical form revoked
{
public:
    MachLogLargeMissileLauncher( MachLogRace*, MachPhysLargeMissile*, MachActor* pOwner );
    virtual ~MachLogLargeMissileLauncher();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogLargeMissileLauncher& t );
	
	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogLargeMissileLauncher );
	PER_FRIEND_READ_WRITE( MachLogLargeMissileLauncher );

protected:

	virtual	MachLogLinearProjectile* createLinearProjectile
	(
		const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
	    const W4dEntity& target, const MexPoint3d& targetOffset
	);

private:
	//ops deliberately revoked
    MachLogLargeMissileLauncher( const MachLogLargeMissileLauncher& );
    MachLogLargeMissileLauncher& operator =( const MachLogLargeMissileLauncher& );
    bool operator ==( const MachLogLargeMissileLauncher& );

	//data members
	MachPhysLargeMissile*	pPhysLargeMissile_;
};

PER_DECLARE_PERSISTENT( MachLogLargeMissileLauncher );

#endif

/* End LMISSLCH.HPP *************************************************/
