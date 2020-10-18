/*
 * F L A M E T H R . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/transf3d.hpp"
#include "mathex/point2d.hpp" //Only needed for template instantiation

#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"
//#include "machphys/random.hpp"

#include "machlog/actor.hpp"
#include "machlog/flamethr.hpp"
#include "machlog/flamebal.hpp"

PER_DEFINE_PERSISTENT( MachLogFlameThrower );

MachLogFlameThrower::MachLogFlameThrower( MachLogRace* pRace, MachPhysLinearWeapon* pPhysWeapon, MachActor* pOwner )
:	MachLogLinearWeapon( pRace, pPhysWeapon, pOwner )
{

    TEST_INVARIANT;
}

MachLogFlameThrower::~MachLogFlameThrower()
{
    TEST_INVARIANT;

}

void MachLogFlameThrower::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

//virtual
MachLogLinearProjectile* MachLogFlameThrower::createLinearProjectile
(
	const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
    const W4dEntity& target, const MexPoint3d& targetOffset
)	
{
	return _NEW( 
		MachLogFlameBall( 
			&logRace(), 
			createPhysLinearProjectile( burstStartTime, index, pParent, target, targetOffset ), 
			&owner(), 
			physWeapon().weaponData()
		)
	);
}

ostream& operator <<( ostream& o, const MachLogFlameThrower& t )
{

    o << "MachLogFlameThrower " << (void*)&t << " start" << std::endl;
    o << "MachLogFlameThrower " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogFlameThrower& weapon )
{
	const MachLogLinearWeapon& base1 = weapon;

	ostr << base1;
}

void perRead( PerIstream& istr, MachLogFlameThrower& weapon )
{
	MachLogLinearWeapon& base1 = weapon;

	istr >> base1;
}

MachLogFlameThrower::MachLogFlameThrower( PerConstructor con )
:	MachLogLinearWeapon( con )
{
}


/* End PULSEWEP.CPP *************************************************/
