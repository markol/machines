/*
 * P U L S E W E P . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogBolterWeapon

    Bolter weapons:
		Bolter
		Autocannon
		Heavy
*/

#ifndef _MACHLOG_BOLTERWEP_HPP
#define _MACHLOG_BOLTERWEP_HPP

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

protected:
	virtual void doFire( MachActor*, const MachLogFireData& );

private:
    MachLogBolterWeapon( const MachLogBolterWeapon& );
    MachLogBolterWeapon& operator =( const MachLogBolterWeapon& );
    bool operator ==( const MachLogBolterWeapon& );

};


#endif

/* End BolterWEP.HPP *************************************************/
