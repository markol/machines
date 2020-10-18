/*
 * N U C L W E A P . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/transf3d.hpp"

#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"

#include "machlog/actor.hpp"
#include "machlog/nuclweap.hpp"
#include "machlog/nuclmiss.hpp"
//#include "machlog/races.hpp"
#include "machlog/race.hpp"

PER_DEFINE_PERSISTENT( MachLogNuclearWeapon );

MachLogNuclearWeapon::MachLogNuclearWeapon( MachLogRace* pRace, MachPhysLinearWeapon* pPhysWeapon, MachActor* pOwner )
:	MachLogLinearWeapon( pRace, pPhysWeapon, pOwner )
{
	TEST_INVARIANT;
}

MachLogNuclearWeapon::~MachLogNuclearWeapon()
{
    TEST_INVARIANT;

}

void MachLogNuclearWeapon::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

//virtual
MachLogLinearProjectile* MachLogNuclearWeapon::createLinearProjectile
(
	const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
    const W4dEntity& target, const MexPoint3d& targetOffset
)	
{
	return _NEW( 
		MachLogNuclearMissile( 
			&logRace(), 
			createPhysLinearProjectile( burstStartTime, index, pParent, target, targetOffset ), 
			&owner(), 
			physWeapon().weaponData()
		)
	);
}

ostream& operator <<( ostream& o, const MachLogNuclearWeapon& t )
{

    o << "MachLogNuclearWeapon " << (void*)&t << " start" << std::endl;
    o << "MachLogNuclearWeapon " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogNuclearWeapon& weapon )
{
	const MachLogLinearWeapon& base1 = weapon;

	ostr << base1;
}

void perRead( PerIstream& istr, MachLogNuclearWeapon& weapon )
{
	MachLogLinearWeapon& base1 = weapon;

	istr >> base1;
}

MachLogNuclearWeapon::MachLogNuclearWeapon( PerConstructor con )
:	MachLogLinearWeapon( con )
{
}


/* End NUCLWEAP.CPP *************************************************/
