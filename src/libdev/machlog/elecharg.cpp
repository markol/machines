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
#include "machlog/elecharg.hpp"
#include "machlog/electro.hpp"

//added for gun targetting.
//#include "machlog/constron.hpp"
//#include "machlog/machine.hpp"
//#include "machlog/mcmotseq.hpp"
//#include "machlog/planet.hpp"
//#include "phys/cspace2.hpp"

PER_DEFINE_PERSISTENT( MachLogElectroCharger );

MachLogElectroCharger::MachLogElectroCharger( MachLogRace* pRace, MachPhysLinearWeapon* pPhysWeapon, MachActor* pOwner )
:	MachLogLinearWeapon( pRace, pPhysWeapon, pOwner )
{

    TEST_INVARIANT;
}

MachLogElectroCharger::~MachLogElectroCharger()
{
    TEST_INVARIANT;

}

void MachLogElectroCharger::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

//virtual
MachLogLinearProjectile* MachLogElectroCharger::createLinearProjectile
(
	const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
    const W4dEntity& target, const MexPoint3d& targetOffset
)	
{
	return _NEW( 
		MachLogElectro( 
			&logRace(), 
			createPhysLinearProjectile( burstStartTime, index, pParent, target, targetOffset ), 
			&owner(), 
			physWeapon().weaponData()
		)
	);
}

ostream& operator <<( ostream& o, const MachLogElectroCharger& t )
{

    o << "MachLogElectroCharger " << (void*)&t << " start" << std::endl;
    o << "MachLogElectroCharger " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogElectroCharger& weapon )
{
	const MachLogLinearWeapon& base1 = weapon;

	ostr << base1;
}

void perRead( PerIstream& istr, MachLogElectroCharger& weapon )
{
	MachLogLinearWeapon& base1 = weapon;

	istr >> base1;
}

MachLogElectroCharger::MachLogElectroCharger( PerConstructor con )
:	MachLogLinearWeapon( con )
{
}

/* End PULSEWEP.CPP *************************************************/
