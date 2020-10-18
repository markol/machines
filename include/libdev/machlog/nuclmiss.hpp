/*
 * N U C L M I S S . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogNuclearMissile - The projectile fired from a MachLogNuclearWeapon. 
	A linear projectile like MLPlasmaBolt etc. Unlike any other MLLinearProjectiles,
	however (at time of creation, certainly), this doesn't instantly confer damage to its
	target if it hits - it creates a MLNuclearBomb object at the point of impact,
	which behaves as an expanding logical ring which damages all that it touches.
	
	The MLNuclearMissiles have fairly low-yield warheads - they certainly wouldn't destroy cities with one strike.

*/

#ifndef _MACHLOG_NUCLMISS_HPP
#define _MACHLOG_NUCLMISS_HPP

#include "base/base.hpp"
#include "machlog/lineproj.hpp"

class MachPhysNuclearMissile;
class MachPhysNuclearBomb;
class MachPhysLinearProjectile;
class MachPhysWeaponData;
class MachActor;

class MachLogNuclearMissile : public MachLogLinearProjectile
// Canonical form revoked
{
public:
    MachLogNuclearMissile(
        MachLogRace* pRace,
		MachPhysLinearProjectile*,
        MachActor* pOwner,
        const MachPhysWeaponData& );

    ~MachLogNuclearMissile( void );
	
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogNuclearMissile& t );	

private:
	void doBeDestroyed();

    MachLogNuclearMissile( const MachLogNuclearMissile& );
    MachLogNuclearMissile& operator =( const MachLogNuclearMissile& );
    bool operator ==( const MachLogNuclearMissile& );
	
	// data members
	
	const MachPhysWeaponData& 			weaponData_;
	MachLogRace*						pLogRace_;

};


#endif

/* End NUCLMISS.HPP *************************************************/
