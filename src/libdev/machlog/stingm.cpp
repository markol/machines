/*
 * STINGM . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "world4d/domain.hpp"
#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"
#include "machlog/stingm.hpp"
#include "machlog/plandoms.hpp"

PER_DEFINE_PERSISTENT( MachLogMetalSting );

MachLogMetalSting::MachLogMetalSting(
    MachLogRace* pRace,
	MachPhysLinearProjectile* pPhysProjectile,
    MachActor* pOwner,
    const MachPhysWeaponData& weaponData )
:	MachLogLinearProjectile( pRace, pPhysProjectile, pOwner, weaponData )
{
	HAL_STREAM("MLMetalSting::CTOR\n" );
    TEST_INVARIANT;
}

MachLogMetalSting::~MachLogMetalSting()
{
    TEST_INVARIANT;

}

void MachLogMetalSting::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogMetalSting& t )
{

    o << "MachLogMetalSting " << (void*)&t << " start" << std::endl;
    o << "MachLogMetalSting " << (void*)&t << " end" << std::endl;

    return o;
}

void MachLogMetalSting::doBeDestroyed()
{
	checkForDamage( 3.0, MachLogLinearProjectile::CONSTANT_DAMAGE, MachPhys::WASP_METAL_STING );
}


void perWrite( PerOstream& ostr, const MachLogMetalSting& actor )
{
	const MachLogLinearProjectile& base1 = actor;

	ostr << base1;

}

void perRead( PerIstream& istr, MachLogMetalSting& actor )
{
	MachLogLinearProjectile& base1 = actor;

	istr >> base1;

}

MachLogMetalSting::MachLogMetalSting( PerConstructor con )
:	MachLogLinearProjectile( con )
{
}

/* End STINGM.CPP *************************************************/
