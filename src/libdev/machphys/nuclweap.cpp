/*
 * N U N L W E A P . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/nuclweap.hpp"

#include "stdlib/string.hpp"
#include "mathex/abox3d.hpp"
#include "mathex/abox2d.hpp"
#include "mathex/line3d.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/degrees.hpp"
#include "phys/rampacce.hpp"
#include "phys/linemoti.hpp"
#include "phys/lsclplan.hpp"
#include "render/texture.hpp"
#include "render/material.hpp"
#include "render/texmgr.hpp"
#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "render/scale.hpp"
#include "render/colour.hpp"
#include "world4d/entity.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/root.hpp"
#include "world4d/soundman.hpp"
#include "world4d/generic.hpp"
#include "world4d/garbcoll.hpp"
#include "world4d/composit.hpp"
#include "world4d/simintpl.hpp"
#include "world4d/visplan.hpp"

#include "machphys/wepdata.hpp"
#include "machphys/constron.hpp"
#include "machphys/missile.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/private/weapper.hpp"
#include "machphys/snddata.hpp"

#include "system/pathname.hpp"

PER_DEFINE_PERSISTENT( MachPhysNuclearWeapon );

MachPhysNuclearWeapon::MachPhysNuclearWeapon( W4dEntity* pParent, const MexTransform3d& localTransform, MachPhys::Mounting mounting )
: MachPhysLinearWeapon( exemplar(), mounting, pParent, localTransform )
{
    TEST_INVARIANT;
}

//One-time ctor
MachPhysNuclearWeapon::MachPhysNuclearWeapon()
:MachPhysLinearWeapon( MachPhysWeaponPersistence::instance().pRoot(), MexTransform3d(),
                          SysPathName( compositeFilePath() ),
                          MachPhys::NUCLEAR_MISSILE, MachPhys::LEFT )
{
    TEST_INVARIANT;
}

MachPhysNuclearWeapon::~MachPhysNuclearWeapon()
{
    TEST_INVARIANT;

}

//static
const MachPhysNuclearWeapon& MachPhysNuclearWeapon::exemplar()
{
    return MachPhysWeaponPersistence::instance().nuclearExemplar();
}

void MachPhysNuclearWeapon::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysNuclearWeapon& t )
{

    o << "MachPhysNuclearWeapon " << (void*)&t << " start" << std::endl;
    o << "MachPhysNuclearWeapon " << (void*)&t << " end" << std::endl;

    return o;
}

//static
const char* MachPhysNuclearWeapon::compositeFilePath()
{
	return "models/weapons/nmissile/point.cdf";
}

//static
PhysRelativeTime MachPhysNuclearWeapon::destroy(W4dComposite* pComposit, const PhysAbsoluteTime& startTime)
{
	PhysRelativeTime duration = 10;

	//the victim becomes invisible once hit
    W4dVisibilityPlanPtr visibilityPlanPtr( _NEW( W4dVisibilityPlan( false ) ) );
	visibilityPlanPtr->add(false, duration);

    pComposit->entityPlanForEdit().visibilityPlan( visibilityPlanPtr, startTime );

	return duration;

}

//virtual
MachPhysLinearProjectile* MachPhysNuclearWeapon::createProjectile
(
    const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
    const W4dEntity& target, const MexPoint3d& targetOffset
)
{
    return createMissile( burstStartTime, index, pParent, target, targetOffset );
}

MachPhysMissile* MachPhysNuclearWeapon::createMissile
(
    const PhysAbsoluteTime& startTime, uint, W4dEntity* pParent,
    const W4dEntity& target, const MexPoint3d& targetOffset
)
{
    //Create a nuke missile
	MexTransform3d xform = localTransform();
	xform.translate(MexPoint3d(0, 0, 0.128));

	//xform = globalTransform() * xForm
	xform.preTransform( globalTransform() );  //global

	MexTransform3d missileXform = pParent->globalTransform();

    //this = inV(this) * t
 	missileXform.transformInverse( xform );

    MachPhysMissile* pMissile = _NEW( MachPhysMissile( pParent, missileXform, MachPhysMissile::NUCLEAR_MISSILE ) );

    //Make it fly
	MexPoint3d targetOffsetGlobal = targetOffset;
	target.globalTransform().transform(&targetOffsetGlobal);

	pMissile->beLaunched(startTime, weaponData(), targetOffsetGlobal);

	W4dSoundManager::instance().play( pMissile, SID_NUKEMLAUNCH,
	                                  startTime, 1 );

	W4dSoundManager::instance().play( pMissile, SID_NUKEMFLY,
	                                  startTime, 0 );

    return pMissile;
}

//virtual
PhysRelativeTime MachPhysNuclearWeapon::fire( const PhysAbsoluteTime& startTime, int )
{

	PhysRelativeTime duration = 5;
	lighting(RenColour(0.784, 0.784, 0.784), startTime, 5);

    return duration;
}

MachPhysNuclearWeapon::MachPhysNuclearWeapon( PerConstructor con )
: MachPhysLinearWeapon( con )
{
}


void perWrite( PerOstream& ostr, const MachPhysNuclearWeapon& weapon )
{
    const MachPhysWeapon& base = weapon;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysNuclearWeapon& weapon )
{
    MachPhysWeapon& base = weapon;

    istr >> base;
}
/* End NUCLWEAP.CPP ****************************************************/
