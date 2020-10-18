/*
 * B E E B O M B . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/beebomb.hpp"
#include "machphys/private/otherper.hpp"
#include "stdlib/string.hpp"
#include "system/pathname.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/line2d.hpp"
#include "mathex/line3d.hpp"

#include "phys/rampacce.hpp"
#include "ctl/vector.hpp"
#include "phys/motplan.hpp"
#include "phys/linemoti.hpp"
#include "phys/timeangl.hpp"
#include "phys/lsclplan.hpp"

#include "render/colour.hpp"

#include "world4d/composit.hpp"
#include "world4d/link.hpp"
#include "world4d/domain.hpp"
#include "world4d/visplan.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/gusplan.hpp"
#include "world4d/garbcoll.hpp"

#include "machphys/beewave.hpp"
#include "machphys/beebombe.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/random.hpp"
#include "machphys/plansurf.hpp"
#include "machphys/vaptrail.hpp"

#include "sim/manager.hpp"

PER_DEFINE_PERSISTENT( MachPhysBeeBomb );

//public constructor
MachPhysBeeBomb::MachPhysBeeBomb( W4dEntity* pParent, const MexTransform3d& localTransform )
:MachPhysTrailedProjectile( exemplar(), pParent, localTransform )
{
	//pFlame_ = links()[exemplar().pFlame_->id()];

    if(!findLink("flame", &pFlame_))
    	pFlame_ = NULL;

	ASSERT( pFlame_ != NULL, " flame not exist" );
	pFlame_->visible( false );	//only be visible when the missile is launched

    TEST_INVARIANT;
}


//one time constructor
MachPhysBeeBomb::MachPhysBeeBomb()
:MachPhysTrailedProjectile(MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), BEE_BOMB)
{
	readCompositeFile( SysPathName( "models/weapons/bee/bomb.cdf" ) );

    if(!findLink("flame", &pFlame_))
		pFlame_ = NULL;

	ASSERT( pFlame_ != NULL, " flame not found" );

    TEST_INVARIANT;
}

MachPhysBeeBomb::~MachPhysBeeBomb()
{
    TEST_INVARIANT;
}

void MachPhysBeeBomb::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

//static
const MachPhysBeeBomb& MachPhysBeeBomb::exemplar()
{
	return MachPhysOtherPersistence::instance().beeBombExemplar();
}


ostream& operator <<( ostream& o, const MachPhysBeeBomb& t )
{

    o << "MachPhysBeeBomb " << (void*)&t << " start" << std::endl;
    o << "MachPhysBeeBomb " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
PhysRelativeTime MachPhysBeeBomb::doBeDestroyedAt
(
    const PhysAbsoluteTime& time, MachPhys::StrikeType
)
{
    destructionTime_ = time;
    PhysRelativeTime animationDuration = 6.0;  //can not be zero nor even s smaller value, otherwise crash. don't understand. Yueai

    //Make sure the vapour trail gets garbage collected
	if( pVapourTrail_ != NULL )
    	pVapourTrail_->finish( time + animationDuration );

	RICHARD_STREAM("Creating bomb blast" << std::endl);

    //Create the explosion
    //createBombExplosion( time );

    return animationDuration;
}

MachPhysBeeBomb::MachPhysBeeBomb( PerConstructor con )
: MachPhysTrailedProjectile( con )
{
}

void perWrite( PerOstream& ostr, const MachPhysBeeBomb& beeBomb )
{
    const MachPhysTrailedProjectile& base = beeBomb;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysBeeBomb& beeBomb )
{
    MachPhysTrailedProjectile& base = beeBomb;

    istr >> base;
}

void MachPhysBeeBomb::createBombExplosion( const PhysAbsoluteTime& time )
{
    //Kick off a bomb explosion animation at the appropriate time, provided teh client has provided us
    //with a planet surface.
    if( hasPlanetSurface() )
    {
        //Get the coordinates of the bomb when it explodes
        MexPoint3d explodePosition = flightPosition( time );

        //Project down onto the planet surface - get terrain height
        MachPhysPlanetSurface& surface = planetSurface();
        MATHEX_SCALAR z = surface.terrainHeight( explodePosition.x(), explodePosition.y() );
        explodePosition.z( z );

        //Create the bomb explosion
        W4dDomain* pParent = surface.domainAt( explodePosition );
		pParent->globalTransform().transformInverse( &explodePosition);

        MachPhysBeeBombExplosion* pExplosion =
            _NEW( MachPhysBeeBombExplosion( pParent, MexTransform3d( explodePosition ) ) );


        PhysRelativeTime explosionDuration = pExplosion->startExplosion( time, surface );

        //Have it garbage collected
        W4dGarbageCollector::instance().add( pExplosion, time + explosionDuration );
    }
}
/* End BEEBOMB.CPP **************************************************/
