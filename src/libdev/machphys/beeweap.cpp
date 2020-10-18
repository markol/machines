/*
 * B E E W E A P . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/beeweap.hpp"

#include "system/pathname.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/degrees.hpp"
#include "world4d/composit.hpp"
#include "world4d/link.hpp"
#include "world4d/visplan.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/entity.hpp"
#include "world4d/soundman.hpp"

#include "machphys/private/weapper.hpp"
#include "machphys/beebomb.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/snddata.hpp"

PER_DEFINE_PERSISTENT( MachPhysBeeBomber );

//public constructor
MachPhysBeeBomber::MachPhysBeeBomber(W4dEntity* pParent, const MexTransform3d& localXform)
:   MachPhysLinearWeapon( exemplar(), MachPhys::TOP, pParent, localXform ),
pBeeBomb_( _NEW( MachPhysBeeBomb( this, MexTransform3d() ) )	)
{
	pBeeBomb_->visible( true );

	//tilt the weapon downwards
	//MexTransform3d newXform = localXform;
	//newXform.rotate( MexEulerAngles(MexDegrees(weaponData().extras()[0]),
	//                                MexDegrees(weaponData().extras()[1]),
	//                                MexDegrees(weaponData().extras()[2]) ) );
	//newXform.position( MexPoint3d(-10, 0, 0 ) );
	//localTransform( newXform );

    TEST_INVARIANT;
}

//one time constructor
MachPhysBeeBomber::MachPhysBeeBomber()
:   MachPhysLinearWeapon( MachPhysWeaponPersistence::instance().pRoot(), MexTransform3d(),
                    SysPathName( compositeFilePath() ), MachPhys::BEE_BOMB, MachPhys::TOP )
{

    TEST_INVARIANT;
}

//static
const char* MachPhysBeeBomber::compositeFilePath()
{
	return "models/weapons/bee/hold.cdf";
}


MachPhysBeeBomber::MachPhysBeeBomber( PerConstructor con )
: MachPhysLinearWeapon( con )
{
}

MachPhysBeeBomber::~MachPhysBeeBomber()
{
	_DELETE( pBeeBomb_ );

    TEST_INVARIANT;

}

void MachPhysBeeBomber::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

//static
const MachPhysBeeBomber& MachPhysBeeBomber::exemplar()
{
    return MachPhysWeaponPersistence::instance().beeExemplar();
}

//virtual
MachPhysLinearProjectile* MachPhysBeeBomber::createProjectile
(
    const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
    const W4dEntity& target, const MexPoint3d& targetOffset
)
{
	return createBeeBomb( burstStartTime, index, pParent, target, targetOffset );
}


//virtual
PhysRelativeTime MachPhysBeeBomber::fire( const PhysAbsoluteTime& startTime, int )
{
	lighting(RenColour(1.0, 5.1, 1.0), startTime, 1.5);

    //do recoil if any
    //return recoil( startTime );
	return 1.5;
}

MachPhysBeeBomb* MachPhysBeeBomber::createBeeBomb
(
	const PhysAbsoluteTime& burstStartTime,
	uint index,
	W4dEntity* pParent,
	const W4dEntity& target,
	const MexPoint3d& targetOffset
)
{
	PRE( pBeeBomb_ != NULL );

    MexTransform3d startTransform;
    MATHEX_SCALAR distance;
    PhysAbsoluteTime launchTime =
        launchData( burstStartTime, index, pParent, target, targetOffset,
                    &startTransform, &distance );

	MachPhysBeeBomb* pBeeBomb = pBeeBomb_;
	pBeeBomb_= NULL;

	pBeeBomb->attachTo(pParent, startTransform);

    //Make it fly
	MexPoint3d targetOffsetGlobal = targetOffset;
	target.globalTransform().transform(&targetOffsetGlobal);

	pBeeBomb->beLaunched(launchTime, weaponData(), targetOffsetGlobal);

	W4dSoundManager::instance().play(pBeeBomb, SID_BEE_BOMB, burstStartTime, 1);

	//hold another one
	pBeeBomb_ = _NEW( MachPhysBeeBomb( this, MexTransform3d() ) );

	PhysRelativeTime reloadTime = weaponData().reloadTime();

    W4dVisibilityPlanPtr beeBombVisibilityPlanPtr( _NEW( W4dVisibilityPlan( false ) ) );

	beeBombVisibilityPlanPtr->add(true,  reloadTime);
    pBeeBomb_->entityPlanForEdit().visibilityPlan( beeBombVisibilityPlanPtr, launchTime );

	return  pBeeBomb;
}



ostream& operator <<( ostream& o, const MachPhysBeeBomber& t )
{

    o << "MachPhysBeeBomber " << (void*)&t << " start" << std::endl;
    o << "MachPhysBeeBomber " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachPhysBeeBomber& weapon )
{
    const MachPhysLinearWeapon& base = weapon;

    ostr << base;
	ostr << weapon.pBeeBomb_;
}

void perRead( PerIstream& istr, MachPhysBeeBomber& weapon )
{
    MachPhysLinearWeapon& base = weapon;

    istr >> base;
	istr >> weapon.pBeeBomb_;
}

/* End BEEWEAP.CPP **************************************************/
