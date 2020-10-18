/*
 * P U L S E B L B . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "world4d/domain.hpp"
#include "machphys/pulseblb.hpp"
#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"
#include "machlog/pulseblb.hpp"
#include "machlog/plandoms.hpp"

PER_DEFINE_PERSISTENT( MachLogPulseBlob );

MachLogPulseBlob::MachLogPulseBlob(
    MachLogRace* pRace,
	MachPhysLinearProjectile* pPhysProjectile,
    MachActor* pOwner,
    const MachPhysWeaponData& weaponData )
:	MachLogLinearProjectile( pRace, pPhysProjectile, pOwner, weaponData )
{
	HAL_STREAM("MLPulseBlob::CTOR\n" );
    TEST_INVARIANT;
}

MachLogPulseBlob::~MachLogPulseBlob()
{
    TEST_INVARIANT;

}

void MachLogPulseBlob::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogPulseBlob& t )
{

    o << "MachLogPulseBlob " << (void*)&t << " start" << std::endl;
    o << "MachLogPulseBlob " << (void*)&t << " end" << std::endl;

    return o;
}

void MachLogPulseBlob::doBeDestroyed()
{
	checkForDamage( 3.0, MachLogLinearProjectile::CONSTANT_DAMAGE, MachPhys::PULSE_RIFLE );
}

void perWrite( PerOstream& ostr, const MachLogPulseBlob& actor )
{
	const MachLogLinearProjectile& base1 = actor;

	ostr << base1;

}

void perRead( PerIstream& istr, MachLogPulseBlob& actor )
{
	MachLogLinearProjectile& base1 = actor;

	istr >> base1;

}

MachLogPulseBlob::MachLogPulseBlob( PerConstructor con )
:	MachLogLinearProjectile( con )
{
}

/* End PULSEBLB.CPP *************************************************/
