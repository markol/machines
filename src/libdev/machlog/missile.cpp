/*
 * P U L S E B L B . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "stdlib/string.hpp"
#include "world4d/domain.hpp"
#include "machphys/missile.hpp"
#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"
#include "machlog/missile.hpp"
#include "machlog/plandoms.hpp"

PER_DEFINE_PERSISTENT( MachLogMissile );

MachLogMissile::MachLogMissile(
    MachLogRace* pRace,
	MachPhysMissile* pPhysProjectile,
    MachActor* pOwner,
    const MachPhysWeaponData& weaponData )
:	MachLogLinearProjectile( pRace, pPhysProjectile, pOwner, weaponData ),
	pPhysMissile_( pPhysProjectile )
{
	HAL_STREAM("MLMissile::CTOR\n" );
    TEST_INVARIANT;
}

MachLogMissile::~MachLogMissile()
{
    TEST_INVARIANT;

}

void MachLogMissile::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogMissile& t )
{

    o << "MachLogMissile " << (void*)&t << " start" << std::endl;
    o << "MachLogMissile " << (void*)&t << " end" << std::endl;

    return o;
}

void MachLogMissile::doBeDestroyed()
{
	checkForDamage( 10.0, MachLogLinearProjectile::LINEAR_DAMAGE, MachPhys::MULTI_LAUNCHER1 );
}

//virtual 
PhysRelativeTime MachLogMissile::update( const PhysRelativeTime& cpu, MATHEX_SCALAR clearance )
{
	pPhysMissile_->update();
	MachLogLinearProjectile::update( cpu, clearance );
	return 0.0;
}

void perWrite( PerOstream& ostr, const MachLogMissile& actor )
{
	const MachLogLinearProjectile& base1 = actor;

	ostr << base1;

}

void perRead( PerIstream& istr, MachLogMissile& actor )
{
	MachLogLinearProjectile& base1 = actor;

	istr >> base1;

}

MachLogMissile::MachLogMissile( PerConstructor con )
:	MachLogLinearProjectile( con )
{
}

/* End PULSEBLB.CPP *************************************************/
