/*
 * E L E C T R O . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "world4d/domain.hpp"
#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"
#include "machlog/electro.hpp"
#include "machlog/plandoms.hpp"
//#include "machlog/races.hpp"
//#include "machlog/actor.hpp"
//#include "machlog/spacial.hpp"

PER_DEFINE_PERSISTENT( MachLogElectro );

MachLogElectro::MachLogElectro(
    MachLogRace* pRace,
	MachPhysLinearProjectile* pPhysProjectile,
    MachActor* pOwner,
    const MachPhysWeaponData& weaponData )
:	MachLogLinearProjectile( pRace, pPhysProjectile, pOwner, weaponData )
{
	HAL_STREAM("MLElectro::CTOR\n" );
    TEST_INVARIANT;
}

MachLogElectro::~MachLogElectro()
{
    TEST_INVARIANT;

}

void MachLogElectro::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogElectro& t )
{

    o << "MachLogElectro " << (void*)&t << " start" << std::endl;
    o << "MachLogElectro " << (void*)&t << " end" << std::endl;

    return o;
}

void MachLogElectro::doBeDestroyed()
{
	checkForDamage( 3.0, MachLogLinearProjectile::CONSTANT_DAMAGE, MachPhys::ELECTRIC_CHARGE );
}

void perWrite( PerOstream& ostr, const MachLogElectro& actor )
{
	const MachLogLinearProjectile& base1 = actor;

	ostr << base1;

}

void perRead( PerIstream& istr, MachLogElectro& actor )
{
	MachLogLinearProjectile& base1 = actor;

	istr >> base1;

}

MachLogElectro::MachLogElectro( PerConstructor con )
:	MachLogLinearProjectile( con )
{
}



/* End PULSEBLB.CPP *************************************************/
