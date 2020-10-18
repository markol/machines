/*
 * D A M A G E . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysEntityDamage

    The damage effects on a W4dEntity when it gets hit by a weapon.
	These include 1) the darkening of its surfaces,
	              2) two smokeclouds,
				  3) two flames, and a
				  4) ground scorch

*/

#ifndef _MACHPHYS_DAMAGE_HPP
#define _MACHPHYS_DAMAGE_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machphys/machphys.hpp"

class MachPhysEntityDamageImpl;
class W4dEntity;
class MachPhysConstruction;

class MachPhysEntityDamage
// Canonical form revoked
{
public:
    MachPhysEntityDamage( W4dEntity* pTarget );
    MachPhysEntityDamage( MachPhysConstruction* pTarget );
    ~MachPhysEntityDamage( void );

	void damageLevel( const double& percent );
	const double& damageLevel() const;
	 
	void update(); //rely on being called at least every 60 seconds
	bool damaged() const;

	//void damageReferencePoint(const MexPoint3d& refPoint);

	void smoke1Type( const MachPhysPuffType& type1 );
	const MachPhysPuffType& smoke1Type(void) const;

	void smoke2Type( const MachPhysPuffType& type2 );
	const MachPhysPuffType& smoke2Type(void) const;
												   
    void CLASS_INVARIANT;

    //Persistence
    PER_MEMBER_PERSISTENT( MachPhysEntityDamage );
	PER_FRIEND_READ_WRITE( MachPhysEntityDamage );

private:
    friend ostream& operator <<( ostream& o, const MachPhysEntityDamage& t );

    MachPhysEntityDamage( const MachPhysEntityDamage& );
    MachPhysEntityDamage& operator =( const MachPhysEntityDamage& );
	void darkening();
	//MexPoint3d referencePointOnEntity() const;

	MachPhysEntityDamageImpl* pImpl_;

};


PER_DECLARE_PERSISTENT( MachPhysEntityDamage );

#endif

/* End DAMAGE.HPP ***************************************************/
