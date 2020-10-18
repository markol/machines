/*
 * P U L S E W E P . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/transf3d.hpp"
#include "mathex/point2d.hpp"

#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"
//#include "machphys/random.hpp"

#include "machlog/actor.hpp"
#include "machlog/pulsewep.hpp"
#include "machlog/pulseblb.hpp"

PER_DEFINE_PERSISTENT( MachLogPulseWeapon );

MachLogPulseWeapon::MachLogPulseWeapon( MachLogRace* pRace, MachPhysLinearWeapon* pPhysWeapon, MachActor* pOwner )
:	MachLogLinearWeapon( pRace, pPhysWeapon, pOwner )
{

    TEST_INVARIANT;
}

MachLogPulseWeapon::~MachLogPulseWeapon()
{
    TEST_INVARIANT;

}

void MachLogPulseWeapon::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

//virtual
MachLogLinearProjectile* MachLogPulseWeapon::createLinearProjectile
(
	const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
    const W4dEntity& target, const MexPoint3d& targetOffset
)	
{
	//HAL_STREAM("MLPulseWeapon::createLinearProjectile\n" );
	return _NEW( 
		MachLogPulseBlob( 
			&logRace(), 
			createPhysLinearProjectile( burstStartTime, index, pParent, target, targetOffset ), 
			&owner(), 
			physWeapon().weaponData()
		)
	);
	//HAL_STREAM("MLPulseWeapon::createLinearProjectile exit\n" );
}

ostream& operator <<( ostream& o, const MachLogPulseWeapon& t )
{

    o << "MachLogPulseWeapon " << (void*)&t << " start" << std::endl;
    o << "MachLogPulseWeapon " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogPulseWeapon& weapon )
{
	const MachLogLinearWeapon& base1 = weapon;

	ostr << base1;
}

void perRead( PerIstream& istr, MachLogPulseWeapon& weapon )
{
	MachLogLinearWeapon& base1 = weapon;

	istr >> base1;
}

MachLogPulseWeapon::MachLogPulseWeapon( PerConstructor con )
:	MachLogLinearWeapon( con )
{
}


/* End PULSEWEP.CPP *************************************************/
