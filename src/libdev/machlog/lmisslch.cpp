/*
 * L M I S S L C H . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/transf3d.hpp"

#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/largemsl.hpp"
//#include "machphys/random.hpp"

#include "machlog/actor.hpp"
#include "machlog/lmisslch.hpp"
#include "machlog/lmissile.hpp"

//added for gun targetting.
//#include "machlog/constron.hpp"
//#include "machlog/machine.hpp"
//#include "machlog/mcmotseq.hpp"
//#include "machlog/planet.hpp"
//#include "phys/cspace2.hpp"

PER_DEFINE_PERSISTENT( MachLogLargeMissileLauncher );

MachLogLargeMissileLauncher::MachLogLargeMissileLauncher( MachLogRace* pRace, MachPhysLargeMissile* pPhysWeapon, MachActor* pOwner )
:	MachLogLinearWeapon( pRace, pPhysWeapon, pOwner ),
	pPhysLargeMissile_( pPhysWeapon )
{

    TEST_INVARIANT;
}

MachLogLargeMissileLauncher::~MachLogLargeMissileLauncher()
{
    TEST_INVARIANT;

}

void MachLogLargeMissileLauncher::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

//virtual
MachLogLinearProjectile* MachLogLargeMissileLauncher::createLinearProjectile
(
	const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
    const W4dEntity& target, const MexPoint3d& targetOffset
)
{
	HAL_STREAM("MLMultiLauncher::createLinearProjectile\n" );
	return _NEW(
		MachLogLargeMissile(
			&logRace(),
			//pPhysLargeMissile_->createMissile( burstStartTime, index, pParent, target, targetOffset ),
			pPhysLargeMissile_->createMissile( burstStartTime, index, pParent, _CONST_CAST(W4dEntity&, target), targetOffset ),
//			createPhysLinearProjectile( burstStartTime, index, pParent, target, targetOffset ),
			&owner(),
			physWeapon().weaponData()
		)
	);
}

ostream& operator <<( ostream& o, const MachLogLargeMissileLauncher& t )
{

    o << "MachLogLargeMissileLauncher " << (void*)&t << " start" << std::endl;
    o << "MachLogLargeMissileLauncher " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogLargeMissileLauncher& weapon )
{
	const MachLogLinearWeapon& base1 = weapon;

	ostr << base1;
}

void perRead( PerIstream& istr, MachLogLargeMissileLauncher& weapon )
{
	MachLogLinearWeapon& base1 = weapon;

	istr >> base1;
}

MachLogLargeMissileLauncher::MachLogLargeMissileLauncher( PerConstructor con )
:	MachLogLinearWeapon( con )
{
}

/* End LMISSLCH.CPP *************************************************/
