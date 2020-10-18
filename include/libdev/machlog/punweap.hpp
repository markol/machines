/*
 * P U N W E A P . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogPunchWeapon

*/

#ifndef _MACHLOG_PUNWEAP_HPP
#define _MACHLOG_PUNWEAP_HPP

#include "base/base.hpp"
#include "machlog/weapon.hpp"

// forward declarations
class MachPhysPunchWeapon;
class MachLogFireData;


class MachLogPunchWeapon : public MachLogWeapon
// Canonical form revoked
{
public:
    MachLogPunchWeapon( MachLogRace*, MachPhysPunchWeapon*, MachActor* pOwner );
    virtual ~MachLogPunchWeapon( void );

	//the persistence mechanism has to reset which weapons go where
	//so we have to allow it to remount the weapons.
	void setPhysicalPunchWeapon( MachPhysPunchWeapon* );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogPunchWeapon& t );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogPunchWeapon );
	PER_FRIEND_READ_WRITE( MachLogPunchWeapon );
	
	void doEchoPunch();

protected:
	virtual void doFire( MachActor*, const MachLogFireData& );
	virtual void doFire( const MexPoint3d& position );

private:
    MachLogPunchWeapon( const MachLogPunchWeapon& );
    MachLogPunchWeapon& operator =( const MachLogPunchWeapon& );
    bool operator ==( const MachLogPunchWeapon& );

	MachPhysPunchWeapon* pPhysPunchWeapon_;
};

PER_DECLARE_PERSISTENT( MachLogPunchWeapon );

#endif

/* End PUNWEAP.HPP *************************************************/
