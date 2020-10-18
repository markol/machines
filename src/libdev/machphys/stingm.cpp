/*
 * S T I N G M . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "mathex/point2d.hpp"

#include "machphys/stingm.hpp"
#include "machphys/private/otherper.hpp"
#include "stdlib/string.hpp"
#include "ctl/vector.hpp"
#include "system/pathname.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/random.hpp"

#include "phys/lsclplan.hpp"
#include "render/colour.hpp"

#include "world4d/composit.hpp"
#include "world4d/link.hpp"
#include "world4d/visplan.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/garbcoll.hpp"
#include "world4d/light.hpp"
#include "world4d/soundman.hpp"
#include "world4d/generic.hpp"

#include "machphys/fireball.hpp"
#include "machphys/random.hpp"
#include "machphys/snddata.hpp"
#include "machphys/vaptrail.hpp"

PER_DEFINE_PERSISTENT( MachPhysMetalSting );

//public constructor
MachPhysMetalSting::MachPhysMetalSting( W4dEntity* pParent, const MexTransform3d& localTransform )
:   MachPhysTrailedProjectile( exemplar(), pParent, localTransform )
{

    TEST_INVARIANT;
}

//one time constructor
MachPhysMetalSting::MachPhysMetalSting()
:   MachPhysTrailedProjectile( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d(), METAL_STING )
{
	readCompositeFile( SysPathName( "models/weapons/wasp/metal/missile.cdf" ) );

    TEST_INVARIANT;
}

MachPhysMetalSting::~MachPhysMetalSting()
{
    TEST_INVARIANT;

}

//static
const MachPhysMetalSting& MachPhysMetalSting::exemplar()
{
	return MachPhysOtherPersistence::instance().metalStingExemplar();
}


void MachPhysMetalSting::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysMetalSting& t )
{

    o << "MachPhysMetalSting " << (void*)&t << " start" << std::endl;
    o << "MachPhysMetalSting " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
PhysRelativeTime MachPhysMetalSting::doBeDestroyedAt( const PhysAbsoluteTime& time, MachPhys::StrikeType )
{
    destructionTime_ = time;
    PhysRelativeTime duration = 2.0;

    //Make sure the vapour trail gets garbage collected
	if( pVapourTrail_ != NULL )
    	pVapourTrail_->finish( time + duration );

    //  Get the position of the missile when it explodes
    MexTransform3d  explosionTransform;
    uint n;

    if( entityPlan().hasMotionPlan() )
        entityPlan().transform( time, &explosionTransform, &n );
    else
        explosionTransform = localTransform();

	//start an explosion
	explosion( pParent(), explosionTransform, time, duration, RenColour(5.1, 1, 1),  3.0 );

	SoundId explosionId = SID_XPLODE1_MISSILE;
	MexBasicRandom tempRandom(MexBasicRandom::constructSeededFromTime());
	int randomNumber =
		mexRandomInt(&tempRandom, 0, 100);
	if(randomNumber < 50)
	{
		explosionId = SID_XPLODE1_MISSILE;
	}
	else
	{
		explosionId = SID_XPLODE2_MISSILE;
	}
	SOUND_STREAM("Playing sting explosion at " << destructionTime_ << std::endl);
    W4dGeneric* pExplosionSite = _NEW(W4dGeneric(pParent(), explosionTransform));
    W4dGarbageCollector::instance().add(pExplosionSite, destructionTime_ + 10);
	W4dSoundManager::instance().play(pExplosionSite, explosionId, destructionTime_, 1);

    return duration;
}


MachPhysMetalSting::MachPhysMetalSting( PerConstructor con )
: MachPhysTrailedProjectile( con )
{
}

void perWrite( PerOstream& ostr, const MachPhysMetalSting& sting )
{
    const MachPhysTrailedProjectile& base = sting;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysMetalSting& sting )
{
    MachPhysTrailedProjectile& base = sting;

    istr >> base;
}

/* End STINGM.CPP ***************************************************/
