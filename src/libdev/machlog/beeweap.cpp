/*
 * B E E W E A P . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/transf3d.hpp"
#include "mathex/point2d.hpp" //Only needed for template instantiation

#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"

#include "machlog/actor.hpp"
#include "machlog/beeweap.hpp"
#include "machlog/beebomb.hpp"

PER_DEFINE_PERSISTENT( MachLogBeeBomber );

MachLogBeeBomber::MachLogBeeBomber( MachLogRace* pRace, MachPhysLinearWeapon* pPhysWeapon, MachActor* pOwner )
:	MachLogLinearWeapon( pRace, pPhysWeapon, pOwner )
{

    TEST_INVARIANT;
}

MachLogBeeBomber::~MachLogBeeBomber()
{
    TEST_INVARIANT;

}

void MachLogBeeBomber::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

//virtual
MachLogLinearProjectile* MachLogBeeBomber::createLinearProjectile
(
	const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
    const W4dEntity& target, const MexPoint3d& targetOffset
)	
{
	return _NEW( 
		MachLogBeeBomb( 
			&logRace(), 
			createPhysLinearProjectile( burstStartTime, index, pParent, target, targetOffset ), 
			&owner(), 
			physWeapon().weaponData()
		)
	);
}

ostream& operator <<( ostream& o, const MachLogBeeBomber& t )
{

    o << "MachLogBeeBomber " << (void*)&t << " start" << std::endl;
    o << "MachLogBeeBomber " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogBeeBomber& weapon )
{
	const MachLogLinearWeapon& base1 = weapon;

	ostr << base1;
}

void perRead( PerIstream& istr, MachLogBeeBomber& weapon )
{
	MachLogLinearWeapon& base1 = weapon;

	istr >> base1;
}

MachLogBeeBomber::MachLogBeeBomber( PerConstructor con )
:	MachLogLinearWeapon( con )
{
}

/* End BEEWEAP.CPP *************************************************/
