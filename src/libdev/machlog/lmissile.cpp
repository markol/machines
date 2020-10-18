/*
 * L M I S S I L E . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "world4d/domain.hpp"
#include "machphys/missile.hpp"
#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"
#include "machlog/lmissile.hpp"
#include "machlog/plandoms.hpp"




MachLogLargeMissile::MachLogLargeMissile(
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

MachLogLargeMissile::~MachLogLargeMissile()
{
    TEST_INVARIANT;

}

void MachLogLargeMissile::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogLargeMissile& t )
{

    o << "MachLogLargeMissile " << (void*)&t << " start" << std::endl;
    o << "MachLogLargeMissile " << (void*)&t << " end" << std::endl;

    return o;
}

void MachLogLargeMissile::doBeDestroyed()
{
	checkForDamage( 20.0, MachLogLinearProjectile::LINEAR_DAMAGE, MachPhys::LARGE_MISSILE );
}

//virtual 
PhysRelativeTime MachLogLargeMissile::update( const PhysRelativeTime& cpu, MATHEX_SCALAR clearance )
{
	pPhysMissile_->update();
	MachLogLinearProjectile::update( cpu, clearance );
	return 0.0;
}

/* End LMISSILE.CPP *************************************************/
