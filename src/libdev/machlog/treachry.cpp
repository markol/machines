/*
 * T R E A C H R Y . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/transf3d.hpp"
#include "mathex/point2d.hpp" //Only needed for template instantiation

#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"
//#include "machphys/random.hpp"

#include "machlog/actor.hpp"
#include "machlog/treachry.hpp"
#include "machlog/trechorb.hpp"

PER_DEFINE_PERSISTENT( MachLogTreacheryWeapon );

MachLogTreacheryWeapon::MachLogTreacheryWeapon( MachLogRace* pRace, MachPhysLinearWeapon* pPhysWeapon, MachActor* pOwner )
:	MachLogLinearWeapon( pRace, pPhysWeapon, pOwner )
{

    TEST_INVARIANT;
}

MachLogTreacheryWeapon::~MachLogTreacheryWeapon()
{
    TEST_INVARIANT;

}

void MachLogTreacheryWeapon::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

//virtual
MachLogLinearProjectile* MachLogTreacheryWeapon::createLinearProjectile
(
	const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
    const W4dEntity& target, const MexPoint3d& targetOffset
)
{
	HAL_STREAM("MLTreacheryWeapon::createLinearProjectile\n" );
	return _NEW(
		MachLogTreacheryOrb(
			&logRace(),
			createPhysLinearProjectile( burstStartTime, index, pParent, target, targetOffset ),
			&owner(),
			physWeapon().weaponData()
		)
	);
}

ostream& operator <<( ostream& o, const MachLogTreacheryWeapon& t )
{

    o << "MachLogTreacheryWeapon " << (void*)&t << " start" << std::endl;
    o << "MachLogTreacheryWeapon " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogTreacheryWeapon& weapon )
{
	const MachLogLinearWeapon& base1 = weapon;

	ostr << base1;
}

void perRead( PerIstream& istr, MachLogTreacheryWeapon& weapon )
{
	MachLogLinearWeapon& base1 = weapon;

	istr >> base1;
}

MachLogTreacheryWeapon::MachLogTreacheryWeapon( PerConstructor con )
:	MachLogLinearWeapon( con )
{
}


/* End TreacheryWEP.CPP *************************************************/
