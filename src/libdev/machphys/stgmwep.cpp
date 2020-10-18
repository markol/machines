/*
 * S T G M W E P . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/stgmwep.hpp"


#include "system/pathname.hpp"

#include "mathex/transf3d.hpp"
#include "world4d/composit.hpp"
#include "world4d/link.hpp"
#include "world4d/visplan.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/soundman.hpp"

#include "machphys/private/weapper.hpp"
#include "machphys/stingm.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/snddata.hpp"

PER_DEFINE_PERSISTENT( MachPhysMetalStingWeapon );

//publiv constructor
MachPhysMetalStingWeapon::MachPhysMetalStingWeapon( W4dEntity* pParent, const MexTransform3d& localTransform )
:   MachPhysLinearWeapon( exemplar(), MachPhys::TOP, pParent, localTransform )
{

    TEST_INVARIANT;
}

//one time constructor
MachPhysMetalStingWeapon::MachPhysMetalStingWeapon()
:   MachPhysLinearWeapon( MachPhysWeaponPersistence::instance().pRoot(), MexTransform3d(),
                    SysPathName( compositeFilePath() ), MachPhys::WASP_METAL_STING, MachPhys::TOP )
{

    TEST_INVARIANT;
}

//static
const char* MachPhysMetalStingWeapon::compositeFilePath()
{
	return "models/weapons/wasp/metal/metalgun.cdf";
}


MachPhysMetalStingWeapon::~MachPhysMetalStingWeapon()
{
    TEST_INVARIANT;

}

void MachPhysMetalStingWeapon::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysMetalStingWeapon& t )
{

    o << "MachPhysMetalStingWeapon " << (void*)&t << " start" << std::endl;
    o << "MachPhysMetalStingWeapon " << (void*)&t << " end" << std::endl;

    return o;
}

//static
const MachPhysMetalStingWeapon& MachPhysMetalStingWeapon::exemplar()
{
    return MachPhysWeaponPersistence::instance().metalStingExemplar();
}

//virtual
MachPhysLinearProjectile* MachPhysMetalStingWeapon::createProjectile
(
    const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
    const W4dEntity& target, const MexPoint3d& targetOffset
)
{
	return createMetalSting( burstStartTime, index, pParent, target, targetOffset );
}


//virtual
PhysRelativeTime MachPhysMetalStingWeapon::fire( const PhysAbsoluteTime& startTime, int numberInBurst )
{
	lighting(RenColour(5.1, 1.0, 1.0), startTime, 3.0);

	W4dSoundManager::instance().play(this, SID_WASP_STING_METAL, 0, 1 );

    //do recoil if any
    return recoil( startTime, numberInBurst );
}

MachPhysMetalSting* MachPhysMetalStingWeapon::createMetalSting
(
	const PhysAbsoluteTime& burstStartTime,
	uint index,
	W4dEntity* pParent,
	const W4dEntity& target,
	const MexPoint3d& targetOffset
)
{
    MexTransform3d startTransform;
    MATHEX_SCALAR distance;
    PhysAbsoluteTime launchTime =
        launchData( burstStartTime, index, pParent, target, targetOffset,
                    &startTransform, &distance );

	MachPhysMetalSting* pSting = _NEW( MachPhysMetalSting( pParent, startTransform ) );

    //let it fly
    pSting->beLaunched( launchTime, weaponData(), targetOffset );

	return  pSting;
}

MachPhysMetalStingWeapon::MachPhysMetalStingWeapon( PerConstructor con )
: MachPhysLinearWeapon( con )
{
}

void perWrite( PerOstream& ostr, const MachPhysMetalStingWeapon& weapon )
{
    const MachPhysLinearWeapon& base = weapon;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysMetalStingWeapon& weapon )
{
    MachPhysLinearWeapon& base = weapon;

    istr >> base;
}


/* End STGMWEP.CPP **************************************************/
