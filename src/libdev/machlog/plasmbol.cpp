/*
 * P U L S E B L B . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "world4d/domain.hpp"
#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"
#include "machlog/plasmbol.hpp"
#include "machlog/plandoms.hpp"

PER_DEFINE_PERSISTENT( MachLogPlasmaBolt );

MachLogPlasmaBolt::MachLogPlasmaBolt(
    MachLogRace* pRace,
	MachPhysLinearProjectile* pPhysProjectile,
    MachActor* pOwner,
    const MachPhysWeaponData& weaponData )
:	MachLogLinearProjectile( pRace, pPhysProjectile, pOwner, weaponData )
{
	HAL_STREAM("MLPlasmaBolt::CTOR\n" );
    TEST_INVARIANT;
}

MachLogPlasmaBolt::~MachLogPlasmaBolt()
{
    TEST_INVARIANT;

}

void MachLogPlasmaBolt::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogPlasmaBolt& t )
{

    o << "MachLogPlasmaBolt " << (void*)&t << " start" << std::endl;
    o << "MachLogPlasmaBolt " << (void*)&t << " end" << std::endl;

    return o;
}

void MachLogPlasmaBolt::doBeDestroyed()
{
	checkForDamage( 3.0, MachLogLinearProjectile::CONSTANT_DAMAGE, MachPhys::PLASMA_RIFLE );
}


void perWrite( PerOstream& ostr, const MachLogPlasmaBolt& actor )
{
	const MachLogLinearProjectile& base1 = actor;

	ostr << base1;

}

void perRead( PerIstream& istr, MachLogPlasmaBolt& actor )
{
	MachLogLinearProjectile& base1 = actor;

	istr >> base1;

}

MachLogPlasmaBolt::MachLogPlasmaBolt( PerConstructor con )
:	MachLogLinearProjectile( con )
{
}

/* End PULSEBLB.CPP *************************************************/
