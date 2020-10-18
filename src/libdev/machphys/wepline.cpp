/*
 * W E P L I N E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/wepline.hpp"
#include "machphys/wepdata.hpp"

#include "world4d/manager.hpp"
#include "world4d/objtrack.hpp"
#include "mathex/transf3d.hpp"

PER_DEFINE_PERSISTENT( MachPhysLinearWeapon );

//One-time
MachPhysLinearWeapon::MachPhysLinearWeapon
(
    W4dEntity* pParent, const MexTransform3d& localTransform,
    const SysPathName& compositeFileName,
    MachPhys::WeaponType type, MachPhys::Mounting mounting
)
:   MachPhysWeapon( pParent, localTransform, compositeFileName, type, mounting )
{
    TEST_INVARIANT;
}

//Copy ctor.
MachPhysLinearWeapon::MachPhysLinearWeapon
(
    const MachPhysLinearWeapon& copyMe, MachPhys::Mounting mounting,
    W4dEntity* pParent, const W4dTransform3d& localTransform
)
:   MachPhysWeapon( copyMe, mounting, pParent, localTransform )
{
    TEST_INVARIANT;
}

MachPhysLinearWeapon::MachPhysLinearWeapon( PerConstructor con )
: MachPhysWeapon( con )
{
    TEST_INVARIANT;
}

MachPhysLinearWeapon::~MachPhysLinearWeapon()
{
    TEST_INVARIANT;

}

void MachPhysLinearWeapon::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysLinearWeapon& t )
{

    o << "MachPhysLinearWeapon " << (void*)&t << " start" << std::endl;
    o << "MachPhysLinearWeapon " << (void*)&t << " end" << std::endl;

    return o;
}

PhysAbsoluteTime MachPhysLinearWeapon::launchData
(
    const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
    const W4dEntity& target, const MexPoint3d& targetOffset,
    MexTransform3d* pStartTransform,
	MATHEX_SCALAR* distance
) const
{
    PRE( index < weaponData().nRoundsPerBurst() );

    //get the relevant weapon data
    const MachPhysWeaponData& data = weaponData();
    uint nRounds = data.nRoundsPerBurst();
    PhysRelativeTime burstDuration = data.burstDuration();
    MATHEX_SCALAR range = data.range();

    //Hence compute the actual launch time. Set world4d artificial time for this time.
    PhysAbsoluteTime launchTime = burstStartTime + index * (burstDuration / nRounds);
    W4dManager& manager = W4dManager::instance();
    manager.artificialTime( launchTime );

    //Get the launch offset to use, and increment it
    uint launchIndex = nextLaunchOffset();
    _CONST_CAST( MachPhysLinearWeapon*, this )->nextLaunchOffset( launchIndex + 1);

    //Set up an object tracker to do the computations for getting the start transform for
    //the projectile
    MATHEX_SCALAR maxAbsYawSine = sin( data.horizontalDeviationAngle() );
    MATHEX_SCALAR maxAbsPitchSine = sin( data.verticalDeviationAngle() );

    W4dObjectTracker tracker( *this, data.launchOffsets()[ launchIndex ],
                              target, targetOffset,
                              maxAbsYawSine, maxAbsPitchSine,
                              0.1, range, range );

    //Hence get the start launch position
    MexTransform3d startTransform;
    *distance = tracker.track( &startTransform );

    //This transform is relative to the weapon, and we need it relative to pParent
    MexTransform3d t1 = pParent->globalTransform();
    t1.invert();

    MexTransform3d t2;
    t1.transform( globalTransform(), &t2 );

    t2.transform( startTransform, pStartTransform );

    //restore the actual w4d time
    manager.clearArtificialTime();

    return launchTime;
}

void perWrite( PerOstream& ostr, const MachPhysLinearWeapon& weapon )
{
    const MachPhysWeapon& base = weapon;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysLinearWeapon& weapon )
{
    MachPhysWeapon& base = weapon;

    istr >> base;
}

// forced recompile 5/1/99 CPS
/* End WEPLINE.CPP **************************************************/
