/*
 * STGMWEP . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/transf3d.hpp"
#include "mathex/point2d.hpp" //Only needed for template instantiation

#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"
//#include "machphys/random.hpp"

#include "machlog/actor.hpp"
#include "machlog/stgmwep.hpp"
#include "machlog/stingm.hpp"

PER_DEFINE_PERSISTENT( MachLogMetalStingWeapon );

MachLogMetalStingWeapon::MachLogMetalStingWeapon( MachLogRace* pRace, MachPhysLinearWeapon* pPhysWeapon, MachActor* pOwner )
:	MachLogLinearWeapon( pRace, pPhysWeapon, pOwner )
{

    TEST_INVARIANT;
}

MachLogMetalStingWeapon::~MachLogMetalStingWeapon()
{
    TEST_INVARIANT;

}

void MachLogMetalStingWeapon::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

//virtual
MachLogLinearProjectile* MachLogMetalStingWeapon::createLinearProjectile
(
	const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
    const W4dEntity& target, const MexPoint3d& targetOffset
)	
{
	return _NEW( 
		MachLogMetalSting( 
			&logRace(), 
			createPhysLinearProjectile( burstStartTime, index, pParent, target, targetOffset ), 
			&owner(), 
			physWeapon().weaponData()
		)
	);
}

ostream& operator <<( ostream& o, const MachLogMetalStingWeapon& t )
{

    o << "MachLogMetalStingWeapon " << (void*)&t << " start" << std::endl;
    o << "MachLogMetalStingWeapon " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogMetalStingWeapon& weapon )
{
	const MachLogLinearWeapon& base1 = weapon;

	ostr << base1;
}

void perRead( PerIstream& istr, MachLogMetalStingWeapon& weapon )
{
	MachLogLinearWeapon& base1 = weapon;

	istr >> base1;
}

MachLogMetalStingWeapon::MachLogMetalStingWeapon( PerConstructor con )
:	MachLogLinearWeapon( con )
{
}

/* End STGMWEP.CPP *************************************************/
