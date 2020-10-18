/*
 * P U L S E W E P . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/transf3d.hpp"
#include "mathex/point2d.hpp" //Only needed for template instantiation

#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/mulaunch.hpp"

#include "machlog/actor.hpp"
#include "machlog/mulaunch.hpp"
#include "machlog/missile.hpp"

PER_DEFINE_PERSISTENT( MachLogMultiLauncher );

MachLogMultiLauncher::MachLogMultiLauncher( MachLogRace* pRace, MachPhysMultiLauncher* pPhysWeapon, MachActor* pOwner )
:	MachLogLinearWeapon( pRace, pPhysWeapon, pOwner ),
	pPhysMultiLauncher_( pPhysWeapon )
{

    TEST_INVARIANT;
}

MachLogMultiLauncher::~MachLogMultiLauncher()
{
    TEST_INVARIANT;

}

void MachLogMultiLauncher::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void MachLogMultiLauncher::setPhysicalMultiLauncher( MachPhysMultiLauncher* pPhysWeapon )
{
	pPhysMultiLauncher_ = pPhysWeapon;
	setPhysicalLinearWeapon( pPhysWeapon );
}

//virtual
MachLogLinearProjectile* MachLogMultiLauncher::createLinearProjectile
(
	const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
    const W4dEntity& target, const MexPoint3d& targetOffset
)
{
	HAL_STREAM("MLMultiLauncher::createLinearProjectile\n" );
	return _NEW(
		MachLogMissile(
			&logRace(),
			//pPhysMultiLauncher_->createMissile( burstStartTime, index, pParent, target, targetOffset ),
			pPhysMultiLauncher_->createMissile( burstStartTime, index, pParent, _CONST_CAST(W4dEntity&, target), targetOffset ),
//			createPhysLinearProjectile( burstStartTime, index, pParent, target, targetOffset ),
			&owner(),
			physWeapon().weaponData()
		)
	);
	HAL_STREAM("MLMultiLauncher::createLinearProjectile exit\n" );
}

ostream& operator <<( ostream& o, const MachLogMultiLauncher& t )
{

    o << "MachLogMultiLauncher " << (void*)&t << " start" << std::endl;
    o << "MachLogMultiLauncher " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogMultiLauncher& weapon )
{
	const MachLogLinearWeapon& base1 = weapon;

	ostr << base1;
}

void perRead( PerIstream& istr, MachLogMultiLauncher& weapon )
{
	MachLogLinearWeapon& base1 = weapon;

	istr >> base1;
}

MachLogMultiLauncher::MachLogMultiLauncher( PerConstructor con )
:	MachLogLinearWeapon( con )
{
}



/* End PULSEWEP.CPP *************************************************/
