/*
 * P U L S E W E P . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/transf3d.hpp"
#include "mathex/point2d.hpp" //Only needed for template instantiation

#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"
//#include "machphys/random.hpp"

#include "machlog/actor.hpp"
#include "machlog/plasmawe.hpp"
#include "machlog/plasmbol.hpp"

PER_DEFINE_PERSISTENT( MachLogPlasmaWeapon );

MachLogPlasmaWeapon::MachLogPlasmaWeapon( MachLogRace* pRace, MachPhysLinearWeapon* pPhysWeapon, MachActor* pOwner )
:	MachLogLinearWeapon( pRace, pPhysWeapon, pOwner )
{

    TEST_INVARIANT;
}

MachLogPlasmaWeapon::~MachLogPlasmaWeapon()
{
    TEST_INVARIANT;

}

void MachLogPlasmaWeapon::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

//virtual
MachLogLinearProjectile* MachLogPlasmaWeapon::createLinearProjectile
(
	const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
    const W4dEntity& target, const MexPoint3d& targetOffset
)	
{
	return _NEW( 
		MachLogPlasmaBolt( 
			&logRace(), 
			createPhysLinearProjectile( burstStartTime, index, pParent, target, targetOffset ), 
			&owner(), 
			physWeapon().weaponData()
		)
	);
}

ostream& operator <<( ostream& o, const MachLogPlasmaWeapon& t )
{

    o << "MachLogPlasmaWeapon " << (void*)&t << " start" << std::endl;
    o << "MachLogPlasmaWeapon " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogPlasmaWeapon& weapon )
{
	const MachLogLinearWeapon& base1 = weapon;

	ostr << base1;
}

void perRead( PerIstream& istr, MachLogPlasmaWeapon& weapon )
{
	MachLogLinearWeapon& base1 = weapon;

	istr >> base1;
}

MachLogPlasmaWeapon::MachLogPlasmaWeapon( PerConstructor con )
:	MachLogLinearWeapon( con )
{
}

/* End PULSEWEP.CPP *************************************************/
