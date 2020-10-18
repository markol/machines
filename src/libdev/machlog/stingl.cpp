/*
 * STINGL . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "world4d/domain.hpp"
#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"
#include "machlog/stingl.hpp"
#include "machlog/plandoms.hpp"

PER_DEFINE_PERSISTENT( MachLogLightSting );

MachLogLightSting::MachLogLightSting(
    MachLogRace* pRace,
	MachPhysLinearProjectile* pPhysProjectile,
    MachActor* pOwner,
    const MachPhysWeaponData& weaponData )
:	MachLogLinearProjectile( pRace, pPhysProjectile, pOwner, weaponData )
{
	HAL_STREAM("MLLightSting::CTOR\n" );
    TEST_INVARIANT;
}

MachLogLightSting::~MachLogLightSting()
{
    TEST_INVARIANT;

}

void MachLogLightSting::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogLightSting& t )
{

    o << "MachLogLightSting " << (void*)&t << " start" << std::endl;
    o << "MachLogLightSting " << (void*)&t << " end" << std::endl;

    return o;
}

void MachLogLightSting::doBeDestroyed()
{
	checkForDamage( 3.0, MachLogLinearProjectile::CONSTANT_DAMAGE, MachPhys::WASP_LIGHT_STING );
}


void perWrite( PerOstream& ostr, const MachLogLightSting& actor )
{
	const MachLogLinearProjectile& base1 = actor;

	ostr << base1;

}

void perRead( PerIstream& istr, MachLogLightSting& actor )
{
	MachLogLinearProjectile& base1 = actor;

	istr >> base1;

}

MachLogLightSting::MachLogLightSting( PerConstructor con )
:	MachLogLinearProjectile( con )
{
}

/* End STINGL.CPP *************************************************/
