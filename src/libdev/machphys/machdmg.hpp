/*
 * M A C H D M G . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysMachineDamage

    The damage effects on a W4dEntity when it gets hit by a weapon.
	These include 1) the darkening of its surfaces,
	              2) two smokeclouds,
				  3) two flames, and a
				  4) ground scorch

*/

#ifndef _MACHPHYS_MACHDMG_HPP
#define _MACHPHYS_MACHDMG_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

class MachPhysMachine;
class MachPhysDoublesidedFlame;
class MachPhysSmokeCloud;

class MachPhysMachineDamage
// Canonical form revoked
{
public:
    //Persistence
    PER_MEMBER_PERSISTENT( MachPhysMachineDamage );
	PER_FRIEND_READ_WRITE( MachPhysMachineDamage );

private:

    MachPhysMachineDamage( MachPhysMachine* pTarget_ );
    MachPhysMachineDamage( const MachPhysMachineDamage& );
    ~MachPhysMachineDamage();

	void damageLevel( const double& percent );
	const double& damageLevel() const;
	 
	void update(); //rely on being called at least every 60 seconds
	bool damaged() const;

    void CLASS_INVARIANT;

    MachPhysMachineDamage& operator =( const MachPhysMachineDamage& );

    friend ostream& operator <<( ostream& o, const MachPhysMachineDamage& t );
	friend class MachPhysMachineDamageData;

	MachPhysMachine* pTarget_;
	double damageLevel_;
	MachPhysDoublesidedFlame* pFlame1_;
	MachPhysDoublesidedFlame* pFlame2_;
	MachPhysSmokeCloud* pSmoke1_;
	MachPhysSmokeCloud* pSmoke2_;
};


PER_DECLARE_PERSISTENT( MachPhysMachineDamage );

#endif

/* End MACHDMG.HPP ***************************************************/
