/*
 * F L A M E B A L . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "stdlib/string.hpp"
#include "world4d/domain.hpp"
#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"
#include "machlog/flamebal.hpp"
#include "machlog/plandoms.hpp"
//#include "machlog/races.hpp"
//#include "machlog/actor.hpp"
//#include "machlog/spacial.hpp"

PER_DEFINE_PERSISTENT( MachLogFlameBall );

MachLogFlameBall::MachLogFlameBall(
    MachLogRace* pRace,
	MachPhysLinearProjectile* pPhysProjectile,
    MachActor* pOwner,
    const MachPhysWeaponData& weaponData )
:	MachLogLinearProjectile( pRace, pPhysProjectile, pOwner, weaponData )
{
	HAL_STREAM("MLFlameBall::CTOR\n" );
    TEST_INVARIANT;
}

MachLogFlameBall::~MachLogFlameBall()
{
    TEST_INVARIANT;

}

void MachLogFlameBall::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogFlameBall& t )
{

    o << "MachLogFlameBall " << (void*)&t << " start" << std::endl;
    o << "MachLogFlameBall " << (void*)&t << " end" << std::endl;

    return o;
}

void MachLogFlameBall::doBeDestroyed()
{
	checkForDamage( 4.0, MachLogLinearProjectile::LINEAR_DAMAGE, MachPhys::FLAME_THROWER1 );
}

void perWrite( PerOstream& ostr, const MachLogFlameBall& actor )
{
	const MachLogLinearProjectile& base1 = actor;

	ostr << base1;

}

void perRead( PerIstream& istr, MachLogFlameBall& actor )
{
	MachLogLinearProjectile& base1 = actor;

	istr >> base1;

}

MachLogFlameBall::MachLogFlameBall( PerConstructor con )
:	MachLogLinearProjectile( con )
{
}


/* End PULSEBLB.CPP *************************************************/
