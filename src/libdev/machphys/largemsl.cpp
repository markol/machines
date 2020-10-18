/*
 * M U L A U N C H . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "system/pathname.hpp"
#include "mathex/transf3d.hpp"
#include "world4d/soundman.hpp"
#include "machphys/largemsl.hpp"
#include "machphys/private/weapper.hpp"
#include "machphys/missile.hpp"
#include "render/colour.hpp"
#include "mathex/point3d.hpp"
#include "world4d/entity.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/snddata.hpp"

PER_DEFINE_PERSISTENT( MachPhysLargeMissile );

//Public ctor
MachPhysLargeMissile::MachPhysLargeMissile(W4dEntity* pParent, const MexTransform3d& localTransform, MachPhys::Mounting mounting)
: MachPhysLinearWeapon( exemplar(), mounting, pParent, localTransform )
{
	pMissile_ = _NEW( MachPhysMissile(this, MexPoint3d(0, 0, 1.28), MachPhysMissile::MISSILE_LARGE ) );
	pMissile_->visible(true);
    TEST_INVARIANT;
}

//One-time ctor used to read the mesh (once per multi-launcher type)
MachPhysLargeMissile::MachPhysLargeMissile()
:   MachPhysLinearWeapon( MachPhysWeaponPersistence::instance().pRoot(), MexTransform3d(),
                          SysPathName( compositeFilePath() ),
                          MachPhys::LARGE_MISSILE, MachPhys::LEFT ),
	pMissile_( NULL )
{
    TEST_INVARIANT;
}

MachPhysLargeMissile::MachPhysLargeMissile( PerConstructor con )
: 	MachPhysLinearWeapon( con ),
	pMissile_( NULL )
{
}

MachPhysLargeMissile::~MachPhysLargeMissile()
{
	_DELETE( pMissile_);
    TEST_INVARIANT;
}

void MachPhysLargeMissile::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysLargeMissile& t )
{

    o << "MachPhysLargeMissile " << (void*)&t << " start" << std::endl;
    o << "MachPhysLargeMissile " << (void*)&t << " end" << std::endl;

    return o;
}

//static
const char* MachPhysLargeMissile::compositeFilePath()
{
    return "models/weapons/lmissile/hold.cdf";
}

//static
const MachPhysLargeMissile& MachPhysLargeMissile::exemplar()
{
    return MachPhysWeaponPersistence::instance().largeMissileExemplar();
}

//virtual
MachPhysLinearProjectile* MachPhysLargeMissile::createProjectile
(
    const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
    const W4dEntity& target, const MexPoint3d& targetOffset
)
{
    return createMissile( burstStartTime, index, pParent, target, targetOffset );
}

MachPhysMissile* MachPhysLargeMissile::createMissile
(
    const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
    const W4dEntity& target, const MexPoint3d& targetOffset
)
{
	PRE( pMissile_ != NULL );
	PRE( pParent != NULL );

    //Use utility function to get launch time for the projectile and its
    //start position relative to pParent.
    MexTransform3d startTransform;
    MATHEX_SCALAR distance;
    PhysAbsoluteTime launchTime =
        launchData( burstStartTime, index, pParent, target, targetOffset,
                    &startTransform, &distance );

    //Use the attached missile, attaching it to the correct parent before we do the calculations
	MachPhysMissile* pMissile = pMissile_;
	pMissile_ = NULL;

    pMissile->attachTo( pParent, startTransform );

    //Make it fly
    pMissile->beLaunched( launchTime, weaponData(), targetOffset );

	if(pMissile->weaponType() == MachPhys::LARGE_MISSILE)
	{
	    //Play the firing sound
		W4dSoundManager::instance().play( this, SID_LARGEMLAUNCH, launchTime, 1 );
		SOUND_STREAM("ML large" << std::endl);
	}
	else
	{
	    //Play the firing sound
		W4dSoundManager::instance().play( this, SID_MLAUNCH, launchTime, 1 );
		SOUND_STREAM("ML normal" << std::endl);
	}

    //Attach sound to missile
//	W4dSoundManager::instance().play( pMissile, SID_MFLY, launchTime, 1 );

    //Create another missile
    pMissile_ = _NEW( MachPhysMissile( this, MexPoint3d(0, 0, 1.28), MachPhysMissile::MISSILE_LARGE ) );

	PhysRelativeTime reloadTime = MachPhysData::instance().weaponData(MachPhys::LARGE_MISSILE).reloadTime();
    W4dVisibilityPlanPtr missileVisibilityPlanPtr( _NEW( W4dVisibilityPlan( false ) ) );

	missileVisibilityPlanPtr->add(true,  reloadTime);
    pMissile_->entityPlanForEdit().visibilityPlan( missileVisibilityPlanPtr, burstStartTime );

    return pMissile;
}

//virtual
PhysRelativeTime MachPhysLargeMissile::fire( const PhysAbsoluteTime& startTime, int )
{

	PhysRelativeTime duration = 5;
	lighting(RenColour(0.784, 0.784, 0.784), startTime, 5);

    return duration;
}

void perWrite( PerOstream& ostr, const MachPhysLargeMissile& weapon )
{
    const MachPhysLinearWeapon& base = weapon;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysLargeMissile& weapon )
{
    MachPhysLinearWeapon& base = weapon;

    istr >> base;
}

/* End LARGEMSL.CPP *************************************************/
