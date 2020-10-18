/*
 * V O R T W E A P . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/transf3d.hpp"

#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/mphydata.hpp"

#include "machlog/actor.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/ionweap.hpp"
#include "machlog/ionbeam.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"

PER_DEFINE_PERSISTENT( MachLogIonWeapon );

MachLogIonWeapon::MachLogIonWeapon( MachLogRace* pRace, MachPhysWeapon* pPhysWeapon, MachActor* pOwner )
:	MachLogWeapon( pRace, pPhysWeapon, pOwner )
{
	HAL_STREAM("MLIonWeapon::CTOR owner.id " << pOwner->id() << std::endl );	

    TEST_INVARIANT;
}

MachLogIonWeapon::~MachLogIonWeapon()
{
	HAL_STREAM("MLIonWeapon::DTOR race " << std::endl );
    TEST_INVARIANT;

}

void MachLogIonWeapon::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

//virtual
void MachLogIonWeapon::doFire( MachActor* pTarget, const MachLogFireData& )
{
	MachLogIonWeapon::doFire( pTarget->position() );
}

//virtual
void MachLogIonWeapon::doFire( const MexPoint3d& position )
{
	//create pulse blobs (the logical entites here).
	PhysRelativeTime timeOffset = 0;
	int launchOffset = 0;
	_NEW( MachLogIonBeam( &logRace() , position, physWeapon().weaponData(), &owner() ) );	
}

ostream& operator <<( ostream& o, const MachLogIonWeapon& t )
{

    o << "MachLogIonWeapon " << (void*)&t << " start" << std::endl;
    o << "MachLogIonWeapon " << (void*)&t << " end" << std::endl;

    return o;
}



/////////////////////////////////////////////////// persistence /////////////////////////////////////////////////////

void perWrite( PerOstream& ostr, const MachLogIonWeapon& weapon )
{
	const MachLogWeapon& base1 = weapon;

	ostr << base1;
}

void perRead( PerIstream& istr, MachLogIonWeapon& weapon )
{
	MachLogWeapon& base1 = weapon;

	istr >> base1;
}

MachLogIonWeapon::MachLogIonWeapon( PerConstructor con )
:	MachLogWeapon( con )
{
}


/* End PULSEWEP.CPP *************************************************/
