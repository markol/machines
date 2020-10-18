/*
 * S T G L W E P . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/stglwep.hpp"

#include "stdlib/string.hpp"
#include "system/pathname.hpp"

#include "ctl/vector.hpp"

#include "mathex/transf3d.hpp"
#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "render/colour.hpp"
#include "render/matvec.hpp"
#include "render/material.hpp"
#include "render/texture.hpp"
#include "render/texmgr.hpp"

#include "world4d/composit.hpp"
#include "world4d/link.hpp"
#include "world4d/visplan.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/coldata.hpp"
#include "world4d/colpulse.hpp"
#include "world4d/matplan.hpp"
#include "world4d/entyiter.hpp"
#include "world4d/soundman.hpp"
#include "world4d/compplan.hpp"

#include "machphys/private/weapper.hpp"
#include "machphys/stingl.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/snddata.hpp"

#include "sim/manager.hpp"

PER_DEFINE_PERSISTENT( MachPhysLightStingWeapon );

//publiv constructor
MachPhysLightStingWeapon::MachPhysLightStingWeapon( W4dEntity* pParent, const MexTransform3d& localTransform )
:   MachPhysLinearWeapon( exemplar(), MachPhys::TOP, pParent, localTransform )
{

	startGlow( SimManager::instance().currentTime() );
    TEST_INVARIANT;
}

//one time constructor
MachPhysLightStingWeapon::MachPhysLightStingWeapon()
:   MachPhysLinearWeapon( MachPhysWeaponPersistence::instance().pRoot(), MexTransform3d(),
                    SysPathName( compositeFilePath() ), MachPhys::WASP_LIGHT_STING, MachPhys::TOP )
{

    TEST_INVARIANT;
}

//static
const char* MachPhysLightStingWeapon::compositeFilePath()
{
	return "models/weapons/wasp/light/lightgun.cdf";
}

MachPhysLightStingWeapon::~MachPhysLightStingWeapon()
{
    TEST_INVARIANT;

}

void MachPhysLightStingWeapon::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysLightStingWeapon& t )
{

    o << "MachPhysLightStingWeapon " << (void*)&t << " start" << std::endl;
    o << "MachPhysLightStingWeapon " << (void*)&t << " end" << std::endl;

    return o;
}

//static
const MachPhysLightStingWeapon& MachPhysLightStingWeapon::exemplar()
{
    return MachPhysWeaponPersistence::instance().lightStingExemplar();
}

//virtual
MachPhysLinearProjectile* MachPhysLightStingWeapon::createProjectile
(
    const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
    const W4dEntity& target, const MexPoint3d& targetOffset
)
{

	return createLightSting( burstStartTime, index, pParent, target, targetOffset );
}


//virtual
PhysRelativeTime MachPhysLightStingWeapon::fire( const PhysAbsoluteTime& startTime, int numberInBurst )
{
	const PhysRelativeTime duration = 3.0;

	lighting(RenColour(10.2, 8.0, 2.0), startTime, duration);

	W4dSoundManager::instance().play(this, SID_WASP_STING_LIGHT, 0, 1 );

	//the weapon stops glowing for while then start to glow again
	stopGlow();
	startGlow(startTime + duration );

    //do recoil if any
    return recoil( startTime, numberInBurst );

}

MachPhysLightSting* MachPhysLightStingWeapon::createLightSting
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

	MachPhysLightSting* pSting = _NEW( MachPhysLightSting( pParent, startTransform ) );

	//let it spin if can
	W4dCompositePlanPtr spinPlanPtr;

	if( pSting->findCompositePlan( "spin", &spinPlanPtr ) )
	{
        pSting->plan( *spinPlanPtr, launchTime, 1000000 );
	}

    //Make it fly
    pSting->move( launchTime, weaponData() );


	return  pSting;
}

MachPhysLightStingWeapon::MachPhysLightStingWeapon( PerConstructor con )
: MachPhysLinearWeapon( con )
{
}


void perWrite( PerOstream& ostr, const MachPhysLightStingWeapon& weapon )
{
    const MachPhysLinearWeapon& base = weapon;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysLightStingWeapon& weapon )
{
    MachPhysLinearWeapon& base = weapon;

    istr >> base;
}

static const ctl_pvector< W4dColourPulseData >& colourPulseDataVec()
{
//		MACH_B FROM 20 20 20 TO 255 150 0 EVERY 0.1 MAX_LOD 0
	static W4dColourPulseData pulseData( "MACH_B.bmp", RenColour(20.0/255.0, 20.0/255.0, 20.0/255.0), RenColour( 1, 150.0/255.0, 0), 0.1 );
	static ctl_pvector< W4dColourPulseData > dataVec;
	static bool first = true;
	if( first )
	{
		dataVec.reserve(1);
		dataVec.push_back( &pulseData );
	}
	return dataVec;
}


static const RenTexture& glowTexture()
{
	static const RenTexture texture = RenTexManager::instance().createTexture("MACH_B.bmp");

	return texture;
}

void  MachPhysLightStingWeapon::startGlow( const PhysAbsoluteTime& startTime )
{
	static const ctl_pvector< W4dColourPulseData >& dataVec = colourPulseDataVec();

	size_t nLinks = links().size();

	for( size_t iLink=0; iLink<nLinks; ++iLink)
	{
		W4dLink* pLink = links()[iLink];
		if ( pLink->hasMesh() )
		{
			W4dColourPulsePlan::makePlan( pLink, dataVec, startTime, GLOWING);
		}
	}

}

void  MachPhysLightStingWeapon::stopGlow()
{
    //Clear the plans with animation id animId immediately.
    //Does this for the composite and all its links.
    clearAnimation( GLOWING );

	size_t nLinks = links().size();

	for( size_t iLink=0; iLink<nLinks; ++iLink)
	{
		W4dLink* pLink = links()[iLink];
    	//pLink->entityPlanForEdit().clearAnimation( GLOWING );

		if ( pLink->hasMesh() )
		{
			W4dColourPulsePlan::changeColour( pLink, glowTexture(), RenColour::black() );
		}
	}
}

/* End STGLWEP.CPP **************************************************/
