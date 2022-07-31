/*
 * W E P L I N E. H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogLinearWeapon

*/

#ifndef _MACHLOG_WEPLINE_HPP
#define _MACHLOG_WEPLINE_HPP

#include "base/base.hpp"
#include "machlog/weapon.hpp"
#include "machlog/messbrok.hpp"

class MachPhysLinearWeapon;
class MachLogLinearProjectile;
class MachPhysLinearProjectile;
class MachLogFireData;

class MachLogLinearWeapon : public MachLogWeapon
// Canonical form revoked
{
public:
	enum CreateHolder { DO_NOT_CREATE_HOLDER, CREATE_HOLDER };

    MachLogLinearWeapon( MachLogRace*, MachPhysLinearWeapon*, MachActor* pOwner );
    virtual ~MachLogLinearWeapon();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogLinearWeapon& t );

	void doEchoFire( MachActor* pTarget,
					 MexPoint3d targetOffset,
                     const PhysAbsoluteTime& launchTime,
					 int nRounds, 
					 const MachLogMessageBroker::ProjectileDestroyData& extraData, 
					 CreateHolder );
					 
	void doEchoFireForTerrain( MexPoint3d position,
                                const PhysAbsoluteTime& launchTime,
								int nRounds,
    							const MachLogMessageBroker::ProjectileDestroyData& extraData, 
								CreateHolder );
    
	//the persistence mechanism has to reset which weapons go where
	//so we have to allow it to remount the weapons.
	void setPhysicalLinearWeapon( MachPhysLinearWeapon* );

	PER_MEMBER_PERSISTENT_ABSTRACT( MachLogLinearWeapon );
	PER_FRIEND_READ_WRITE( MachLogLinearWeapon );

protected:
	virtual void doFire( MachActor*, const MachLogFireData& );
	virtual void doFire( const MexPoint3d& location );
	MachPhysLinearWeapon&	physLinearWeapon();

	MachPhysLinearProjectile* createPhysLinearProjectile
    (
        const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
        const W4dEntity& target, const MexPoint3d& targetOffset
    );

	virtual MachLogLinearProjectile* createLinearProjectile
    (
        const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
        const W4dEntity& target, const MexPoint3d& targetOffset
    ) = 0;

private:
	void dtlw( const char* text, bool start );
    MachLogLinearWeapon( const MachLogLinearWeapon& );
    MachLogLinearWeapon& operator =( const MachLogLinearWeapon& );
    bool operator ==( const MachLogLinearWeapon& );

	MachPhysLinearWeapon*	pPhysLinearWeapon_;
};

PER_DECLARE_PERSISTENT( MachLogWeapon );

#endif

/* End LinearWEP.HPP *************************************************/
