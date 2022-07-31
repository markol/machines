/*
 * B O L T E R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogBolterWeapon

    Bolter weapons:
		Bolter
		Autocannon
		Heavy
*/

#ifndef _MACHLOG_BOLTER_HPP
#define _MACHLOG_BOLTER_HPP

#include "base/base.hpp"
#include "machlog/weapon.hpp"

class MachLogFireData;

class MachLogBolterWeapon : public MachLogWeapon
// Canonical form revoked
{
public:
    MachLogBolterWeapon( MachLogRace*, MachPhysWeapon*, MachActor* pOwner );
    virtual ~MachLogBolterWeapon();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogBolterWeapon& t );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogBolterWeapon );
	PER_FRIEND_READ_WRITE( MachLogBolterWeapon );

protected:
	virtual void doFire( MachActor*, const MachLogFireData& fireData );
	virtual void doFire( const MexPoint3d& position );

private:
    MachLogBolterWeapon( const MachLogBolterWeapon& );
    MachLogBolterWeapon& operator =( const MachLogBolterWeapon& );
    bool operator ==( const MachLogBolterWeapon& );

};

PER_DECLARE_PERSISTENT( MachLogBolterWeapon );

#endif

/* End Bolter.HPP *************************************************/
