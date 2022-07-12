/*
 * T R A I L P R J . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <stdlib.h>

#include "machphys/trailprj.hpp"

#include "phys/rampacce.hpp"
#include "ctl/vector.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/degrees.hpp"
#include "mathex/vec3.hpp"
#include "mathex/line3d.hpp"
#include "mathex/transf3d.hpp"
#include "phys/lsclplan.hpp"
#include "phys/timeangl.hpp"
#include "phys/motplan.hpp"
#include "phys/linemoti.hpp"
#include "world4d/garbcoll.hpp"
#include "world4d/soundman.hpp"
#include "world4d/composit.hpp"
#include "world4d/compplan.hpp"
#include "world4d/link.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/gusplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/light.hpp"
#include "sim/manager.hpp"

#include "system/pathname.hpp"

#include "machphys/fireball.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/random.hpp"
#include "machphys/ofactory.hpp"
#include "machphys/vaptrail.hpp"
#include "machphys/compmgr.hpp"

PER_DEFINE_PERSISTENT( MachPhysTrailedProjectile );

MachPhysTrailedProjectile::MachPhysTrailedProjectile
(
    W4dEntity* pParent, const W4dTransform3d& localTransform, size_t level
)
: MachPhysLinearProjectile( pParent, localTransform ),
 // pImpl_( _NEW( MachPhysTrailedProjectileImpl( level ) ) )
 pVapourTrail_( NULL ),
 pFlame_( NULL ),
 level_( level )
{
    //PRE( level > 0 and level < 10 );

    TEST_INVARIANT;
}

MachPhysTrailedProjectile::MachPhysTrailedProjectile
(
    const MachPhysTrailedProjectile& copyMe, W4dEntity* pParent, const W4dTransform3d& localTransform
)
: MachPhysLinearProjectile( copyMe, pParent, localTransform ),
  //pImpl_( _NEW( MachPhysTrailedProjectileImpl( level ) ) )
 pVapourTrail_( NULL ),
 pFlame_( NULL ),
 level_( copyMe.level_ )
{
	sharedCopyCtor();
}

MachPhysTrailedProjectile::MachPhysTrailedProjectile
(
    const MachPhysTrailedProjectile& copyMe,
	W4dEntity* pParent,
	const W4dTransform3d& localTransform,
	CopyLights copyLights
)
: MachPhysLinearProjectile( copyMe, pParent, localTransform, copyLights ),
  //pImpl_( _NEW( MachPhysTrailedProjectileImpl( level ) ) )
 pVapourTrail_( NULL ),
 pFlame_( NULL ),
 level_( copyMe.level_ )
{
	sharedCopyCtor();
}

void MachPhysTrailedProjectile::sharedCopyCtor()
{
    if(!findLink("flame", &pFlame_))
    	pFlame_ = NULL;

	if( pFlame_ )
		pFlame_->visible( false );	//only be visible when the missile is launched

    TEST_INVARIANT;
}

MachPhysTrailedProjectile::~MachPhysTrailedProjectile()
{
    TEST_INVARIANT;

    _DELETE( pVapourTrail_ );
}


void MachPhysTrailedProjectile::update()
{
	//CB_DEPIMPL(MachPhysVapourTrail*, pVapourTrail_ );
	//CB_DEPIMPL(MexVec3, vapourTrailOffset_ );
	//CB_DEPIMPL(PhysAbsoluteTime, destructionTime_ );

    //  Only update the vapour trail if we're still moving
    if( destructionTime_ > SimManager::instance().currentTime() and pVapourTrail_ != NULL )
    {
        MexPoint3d  position = localTransform().position();

	    //update the vapour trail offset vector as well, since the missile may change its orientation.
	    const MATHEX_SCALAR missileToVapourTrailDistance = 2.5;

	    vapourTrailOffset_ = localTransform().xBasis();
	    vapourTrailOffset_ *= -missileToVapourTrailDistance;

        if( pVapourTrail_ )
        	pVapourTrail_->update( position, vapourTrailOffset_ );
    }
}


void MachPhysTrailedProjectile::startFlame( const PhysAbsoluteTime& startTime, const MachPhysWeaponData& data)
{
	//make it visible
	pFlame_->visible( true );

	PhysRelativeTime interval = 0.1;
	//rotate randomly
	if( data.flameRotate() )
	{
		PhysMotionPlan::Angles radians;
		PhysMotionPlan::Times times;
		radians.push_back(0);

		for(int i=1; i<=20; ++i)
		{
			times.push_back(i*interval);
			radians.push_back( MachPhysRandom::randomDouble(-3.14, 3.14 )	);
		}

		PhysMotionPlan::AnglesPtr anglesPtr = _NEW( PhysMotionPlan::Angles(radians) );
		PhysMotionPlan::TimesPtr timesPtr = _NEW( PhysMotionPlan::Times(times) );


		PhysMotionPlanPtr flameMotionPlanPtr( _NEW( PhysTimedAnglePlan( anglesPtr, timesPtr, MexVec3(1, 0, 0), MexVec3(0, 0, 0) ) ) );
		pFlame_->entityPlanForEdit().absoluteMotion( flameMotionPlanPtr,  startTime, 1000);
	 }

	 //scale randomly
	 if( data.flameScale() )
	 {
		MATHEX_SCALAR scaleFrom = data.flameScaleFrom();
		MATHEX_SCALAR scaleTo = data.flameScaleTo();

		//scale plan
	    PhysLinearScalarPlan::ScalarVec scaleTimes;
	    	scaleTimes.reserve(20);

	    PhysLinearScalarPlan::ScalarVec scales;
	    	scales.reserve(21);
	        scales.push_back(scaleFrom);


		for(int i=1; i<=20; ++i)
		{
			scaleTimes.push_back(i*interval);
			scales.push_back( MachPhysRandom::randomDouble(scaleFrom, scaleTo )	);
		}

	    PhysScalarPlanPtr flamePlanPtr( _NEW( PhysLinearScalarPlan(scaleTimes, scales) ) );
		W4dScalePlanPtr flameScalePlanPtr( _NEW( W4dGeneralUniformScalePlan(flamePlanPtr) ) );
	    pFlame_->entityPlanForEdit().scalePlan( flameScalePlanPtr, startTime, 1000 );
	}
}

void MachPhysTrailedProjectile::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysTrailedProjectile& t )
{

    o << "MachPhysTrailedProjectile " << (void*)&t << " start" << std::endl;
    o << "MachPhysTrailedProjectile " << (void*)&t << " end" << std::endl;

    return o;
}

MachPhysTrailedProjectile::MachPhysTrailedProjectile( PerConstructor con )
: MachPhysLinearProjectile( con ),
  pVapourTrail_( NULL )
{
}

void perWrite( PerOstream& ostr, const MachPhysTrailedProjectile& missile )
{
    const MachPhysLinearProjectile& base = missile;

    ostr << base;
    ostr << missile.pVapourTrail_;
	ostr << missile.pFlame_;
	ostr << missile.vapourTrailOffset_;
	ostr << missile.destructionTime_;
	ostr << missile.level_;

}

void perRead( PerIstream& istr, MachPhysTrailedProjectile& missile )
{
    MachPhysLinearProjectile& base = missile;

    istr >> base;

    istr >> missile.pVapourTrail_;
	istr >> missile.pFlame_;
	istr >> missile.vapourTrailOffset_;
	istr >> missile.destructionTime_;
	istr >> missile.level_;

}

//static
void MachPhysTrailedProjectile::explosion
(
	W4dEntity* pParent,
	const MexTransform3d& explosionXform,
	const PhysAbsoluteTime& destructTime,
	const PhysRelativeTime& duration,
	const RenColour& lightCol,
	const MATHEX_SCALAR& mslStrength,
	MachPhysTrailedProjectile::ExplosionColour explosionColour
)
{
	// Add a brief light for the impact explosion.  The light cannot be
	// attached to the missile because it's not visible during the explosion.
	const MATHEX_SCALAR lightRange = (8 + mslStrength) * 2.5;
	const PhysRelativeTime lightDuration = 0.6;

	W4dUniformLight* pLight = _NEW(W4dUniformLight(pParent, MexVec3(0, 0, 1), lightRange));
	pLight->colour(lightCol );
	pLight->constantAttenuation(0);
	pLight->linearAttenuation(0.177);
	pLight->quadraticAttenuation(0.823);
	pLight->scope(W4dLight::DYNAMIC);
	pLight->localTransform(explosionXform);
	pLight->visible(false);

    W4dVisibilityPlanPtr lightVisibilityPlanPtr(_NEW(W4dVisibilityPlan(false)));
	lightVisibilityPlanPtr->add(true,  0);
	lightVisibilityPlanPtr->add(false, lightDuration);
    pLight->entityPlanForEdit().visibilityPlan( lightVisibilityPlanPtr, destructTime );

    PhysLinearScalarPlan::ScalarVec times;
    	times.reserve(1);
		times.push_back(lightDuration);

    PhysLinearScalarPlan::ScalarVec scales;
    	scales.reserve(2);
        scales.push_back(1.0);
        scales.push_back(0);

	PhysLinearScalarPlan* pPlan = _NEW( PhysLinearScalarPlan(times, scales) );
	PhysScalarPlanPtr intensityPlanPtr = pPlan;
	pLight->intensityPlan(intensityPlanPtr, destructTime );

    //Ensure it gets garbage collected
    W4dGarbageCollector::instance().add(pLight, lightDuration + destructTime);

	explosionWithoutLights(pParent, explosionXform, destructTime, duration, mslStrength, explosionColour);
}

//static
void MachPhysTrailedProjectile::explosionWithoutLights
(
	W4dEntity* pParent,
	const MexTransform3d& explosionXform,
	const PhysAbsoluteTime& destructTime,
	const PhysRelativeTime& duration,
	const MATHEX_SCALAR& mslStrength,
	MachPhysTrailedProjectile::ExplosionColour explosionColour
)
{
	// Add a number of random fireballs.
    MATHEX_SCALAR fireballDepthOffset = -0.5;
    const size_t    nFireballs = 3;
    MATHEX_SCALAR   maxOffset = 1.0;
	MachPhysFireballType fireballType;

	if (explosionColour == GREEN )
		fireballType = FIREBALL_4;
	else
		fireballType = FIREBALL_1;


    for( size_t i = 0; i < nFireballs; ++i )
    {
        const MATHEX_SCALAR fireballSize = mslStrength * (2.0 + MachPhysRandom::randomDouble(0.0, 2.0) );

        MexPoint3d  explosionPosition(
            explosionXform.position().x() + ( ( (double)rand() / RAND_MAX ) - 0.5 ) * maxOffset,
            explosionXform.position().y() + ( ( (double)rand() / RAND_MAX ) - 0.5 ) * maxOffset,
            explosionXform.position().z() + ( ( (double)rand() / RAND_MAX ) - 0.5 ) * maxOffset );

        PhysAbsoluteTime beginTime = destructTime + i * 0.2 * (double)rand() / RAND_MAX;
        MachPhysFireball* pFireball =
            _NEW( MachPhysFireball(
            pParent,
            explosionPosition,
            fireballType,
            fireballSize,
            fireballDepthOffset,
            beginTime,
            duration ) );

        MATHEX_SCALAR rotationAngle = MachPhysRandom::randomDouble(0, 2*(Mathex::PI) );
        pFireball->rotate( MexRadians( rotationAngle) );

        //Ensure it gets garbage collected
        W4dGarbageCollector::instance().add( pFireball, beginTime + duration );
	}
}

PhysRelativeTime MachPhysTrailedProjectile::beLaunched
(
    const PhysAbsoluteTime& startTime,
    const MachPhysWeaponData& data,
    const MexPoint3d&
)
{
	//start the missile animations
	W4dCompositePlanPtr flameSpinPlanPtr;

	if( findCompositePlan( "spin", &flameSpinPlanPtr ) )
	{
        plan( *flameSpinPlanPtr, startTime, 1000000);
	}

	if( pFlame_ )
	{
		startFlame( startTime, data );
	}

	if( data.trailOn() and MachPhysComplexityManager::instance().vapourTrailsEnabled() )
	{
		pVapourTrail_ = _NEW( MachPhysVapourTrail( pParent(), 16, 1.5, level_ ) );
	}

    //Initiate the motion using standard superclass function
    PhysRelativeTime flyTime = move( startTime, data );

    //Set the vapour trail offset vector
    const MATHEX_SCALAR missileToVapourTrailDistance = 2.5;

    vapourTrailOffset_ = localTransform().xBasis();
    vapourTrailOffset_ *= -missileToVapourTrailDistance;

    //Store the scheduled destruction time
    destructionTime_ = startTime + flyTime;
    return flyTime;
}

/* End TRAILPRJ.CPP **************************************************/
