/*
 * O B J E X P L O . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/point3d.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/abox3d.hpp"

#include "phys/rampacce.hpp"
#include "phys/linemoti.hpp"
#include "phys/lsclplan.hpp"

#include "render/matvec.hpp"
#include "render/material.hpp"
#include "render/colour.hpp"

#include "world4d/composit.hpp"
#include "world4d/generic.hpp"
#include "world4d/garbcoll.hpp"
#include "world4d/camshake.hpp"
#include "world4d/manager.hpp"
#include "world4d/scenemgr.hpp"
#include "world4d/entity.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/entyiter.hpp"
#include "world4d/simplsca.hpp"
#include "world4d/alphsimp.hpp"
#include "world4d/light.hpp"
#include "world4d/visplan.hpp"
#include "world4d/soundman.hpp"

#include "machphys/objexplo.hpp"
#include "machphys/objdemol.hpp"
#include "machphys/random.hpp"
#include "machphys/smokplum.hpp"
#include "machphys/smokpuff.hpp"
#include "machphys/fireball.hpp"
#include "machphys/damagwav.hpp"
#include "machphys/nukewave.hpp"
#include "machphys/snddata.hpp"

#ifndef _INLINE
    #include "machphys/objexplo.ipp"
#endif

static PhysScalarPlanPtr scalarPlanPtr(const PhysRelativeTime& time, const double& alpha)
{
    PhysLinearScalarPlan::ScalarVec times;
    	times.reserve(1);
		times.push_back(time);

    PhysLinearScalarPlan::ScalarVec scales;
    	scales.reserve(2);
        scales.push_back(alpha);
        scales.push_back(0);

	PhysLinearScalarPlan* pPlan = _NEW( PhysLinearScalarPlan(times, scales) );
    return PhysScalarPlanPtr(pPlan);
}

MachPhysObjectExplosion::MachPhysObjectExplosion( W4dEntity* pObject )
:   pObject_( pObject )
{
    PRE( pObject != NULL );

	// There have been problems with dodgy machlog code providing zero size
	// bounding volumes.
    PRE_DATA(const MexAlignedBox3d& box = (pObject_->isComposite() ?
										   pObject_->asComposite().compositeBoundingVolume():
                                           pObject_->boundingVolume()));
    PRE( box.volume() > MexEpsilon::instance() );

    TEST_INVARIANT;
}

MachPhysObjectExplosion::~MachPhysObjectExplosion()
{
    TEST_INVARIANT;
}

void MachPhysObjectExplosion::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysObjectExplosion& t )
{

    o << "MachPhysObjectExplosion " << (void*)&t << " start" << std::endl;
    o << "MachPhysObjectExplosion " << (void*)&t << " end" << std::endl;

    return o;
}

PhysRelativeTime MachPhysObjectExplosion::explode( const PhysAbsoluteTime& startTime )
{
  // in case we don't know the kind of construction at stake run
  // a generic explosion sequence
	MachPhys::DemolitionType type;
	type.objectType=MachPhys::NOT_CONSTRUCTION;

    return explode( startTime, type );
}


PhysRelativeTime MachPhysObjectExplosion::explode( const PhysAbsoluteTime& startTime, MachPhys::DemolitionType type )
{
	// All the demolition sequences will be scaled relatively to the military fctory dimension which is
	static MATHEX_SCALAR militaryFactoryDimension=39.58;

    const PhysRelativeTime burstTime = 1.0;
    const PhysRelativeTime duration = 10;
	const PhysRelativeTime plumeDuration = 6.0*duration;

	// building scaling and coloring timing
	const PhysRelativeTime shrinkingDuration = 0.10;
	const PhysRelativeTime expansionOffset = shrinkingDuration;
	const PhysRelativeTime expansionDuration = 0.15;
	const PhysRelativeTime visibilityDuration = shrinkingDuration+expansionDuration;
	const PhysRelativeTime glowingOffset = 0.15;
	const PhysRelativeTime glowingDuration = visibilityDuration-glowingOffset;

	// demolition timing
	const PhysRelativeTime demolitionOffset=0.15;
	const PhysRelativeTime demolitionDuration=0.45;

	const PhysRelativeTime mainFireballOffset=expansionOffset;
	const PhysRelativeTime mainFireballDuration=0.3;
	// burstwave timing
	const PhysRelativeTime burstWaveOffset=expansionOffset;
	const PhysRelativeTime burstWaveDuration=demolitionDuration+demolitionOffset-expansionOffset;

    //Allocate an object to own all the explosion animation entities in the same position as the object
    W4dEntity* pOwner = _NEW( W4dGeneric( pObject_->pParent(), pObject_->localTransform(), W4dEntity::NOT_SOLID ) );

	// The current model is all emissive or black.  Hence, it should not need
	// lighting.  This could change if the model changes.
	pOwner->doNotLight(true);

    //Garbage collect this object at end of explosion
    W4dGarbageCollector::instance().add( pOwner, startTime + plumeDuration );

    const MexAlignedBox3d& box = (pObject_->isComposite() ? pObject_->asComposite().compositeBoundingVolume()
                                                               : pObject_->boundingVolume() );

    const MexPoint3d boxCentre = box.centroid();
	const MATHEX_SCALAR boxHeight = box.zLength();
	const MATHEX_SCALAR xOffset = 0.4*box.xLength();
	const MATHEX_SCALAR yOffset = 0.4*box.yLength();
	const MATHEX_SCALAR sizeX = box.xLength();
	const MATHEX_SCALAR sizeY = box.yLength();
	const MATHEX_SCALAR zTop = fabs(box.maxCorner().z());

	MATHEX_SCALAR dimension = std::max(box.xLength(), box.yLength());
				  dimension = std::max(dimension, box.zLength());

	MATHEX_SCALAR demolitionScaling=dimension/militaryFactoryDimension;

	//start a burst wave
	const MATHEX_SCALAR maxShrinkage=0.6;
	const MATHEX_SCALAR maxExpansion=1.8;
	const MATHEX_SCALAR minBurstWaveFromRadius = 7;

	// privde a large max radius which should cover the demolition sequence
	MATHEX_SCALAR burstWaveToRadius   = std::max(sizeX, sizeY) * maxExpansion * 2;
	MATHEX_SCALAR burstWaveFromRadius = std::min(sizeX, sizeY) * maxShrinkage;

	// Dodgy machlog code has been exploding objects with zero bounding volumes.
	// Although the ctor of this object makes a debug check, care needs to be
	// taken here to avoid a divide-by-zero in the event of it getting past the
	// debug checking somehow.
	if (burstWaveFromRadius < MexEpsilon::instance())
	{
		burstWaveToRadius   = minBurstWaveFromRadius * maxExpansion * 2;
		burstWaveFromRadius = minBurstWaveFromRadius * maxShrinkage;
	}
	else if (burstWaveFromRadius < minBurstWaveFromRadius)
	{
		// There's a potenial divide by zero here.
		const MATHEX_SCALAR radiusRatio = burstWaveToRadius/burstWaveFromRadius;
		burstWaveFromRadius = minBurstWaveFromRadius;
		burstWaveToRadius = burstWaveFromRadius*radiusRatio;
	}

	MachPhysBurstWave* pBurstWave = _NEW( MachPhysBurstWave( pOwner, MexTransform3d() ) );
	pBurstWave->startBurstWave( startTime+expansionOffset, burstTime, burstWaveFromRadius, burstWaveToRadius, 1.0);
//	MachPhysNukeWave* pBurstWave = _NEW( MachPhysNukeWave( pObject_, MexTransform3d() ) );
//	pBurstWave->startNukeWave( startTime+expansionOffset, burstTime, burstWaveFromRadius, burstWaveToRadius, 1.0);
//  W4dGarbageCollector::instance().add( pBurstWave, startTime + duration );

	//the main fireball
    MexVec3 vector = box.maxCorner();
    vector -= box.minCorner();
    MATHEX_SCALAR buildingSize = vector.modulus();
    MATHEX_SCALAR mainFireballSize = MachPhysRandom::randomDouble( buildingSize * 4, buildingSize * 6 );
	MATHEX_SCALAR mainFireBallAngle = MachPhysRandom::randomDouble(0, (Mathex::PI) );
    MachPhysFireball* pMainFireball = _NEW( MachPhysFireball(
        pOwner,
        MexPoint3d(boxCentre.x(), boxCentre.y(), 0),
        MachPhysFireball::randomFireball(),
        mainFireballSize,	  //size
        -buildingSize/2.0,									  //depthOffset
        startTime+mainFireballOffset,
        mainFireballDuration ) );
    pMainFireball->rotate( MexRadians( MexRadians( mainFireBallAngle ) ) );


    //W4dGeneric* pExplosionSite = _NEW(W4dGeneric(_STATIC_CAST(W4dEntity*, pObject_->containingDomain()), pObject_->localTransform()));
    W4dGeneric* pExplosionSite = _NEW(W4dGeneric(_REINTERPRET_CAST(W4dEntity*, pObject_->containingDomain()), pObject_->localTransform()));

//	SOUND_STREAM("Object buildingSize " << buildingSize << endl);
	if(buildingSize > 40)
	{
	    W4dSoundManager::instance().play( pExplosionSite, SID_XPLODE_CONSTR_LARGE,
	                        startTime+mainFireballOffset, 1 );
	}
	else
	{
	    W4dSoundManager::instance().play( pExplosionSite, SID_XPLODE_CONSTR_SMALL,
    	                    startTime+mainFireballOffset, 1 );
	}
    W4dGarbageCollector::instance().add(pExplosionSite, startTime+mainFireballOffset + 10);

	// Add a brief light for the duration of the main fireball.
	const MATHEX_SCALAR lightRange = 1.8 * dimension;
	W4dUniformLight* pLight = _NEW(W4dUniformLight(pOwner, MexVec3(0, 0, 1), lightRange));
	pLight->colour(RenColour(3, 2, 0.5));
	pLight->constantAttenuation(0);
	pLight->linearAttenuation(0.2);
	pLight->quadraticAttenuation(0.79);
	pLight->scope(W4dLight::DYNAMIC);

    W4dVisibilityPlanPtr visibilityPlanPtr( _NEW( W4dVisibilityPlan( false ) ) );
	visibilityPlanPtr->add( true, mainFireballOffset );
	visibilityPlanPtr->add( false, mainFireballOffset+mainFireballDuration );
    pLight->entityPlanForEdit().visibilityPlan( visibilityPlanPtr, startTime );

	PhysScalarPlanPtr intensityPlanPtr = scalarPlanPtr( mainFireballDuration, 1 );
	pLight->intensityPlan( intensityPlanPtr, startTime+mainFireballOffset );

    //Create some more fireballs for larger objects
    size_t nFireballs = int(dimension/9.0);

    for( size_t i = 1; i <= nFireballs; ++i )
    {
		MATHEX_SCALAR   fireballSize = MachPhysRandom::randomDouble( std::min(xOffset, yOffset), std::max(xOffset, yOffset) );

        MexPoint3d  fireballPosition( boxCentre.x() + MachPhysRandom::randomDouble( -sizeX/2., sizeX/2. ),
                                      boxCentre.y() + MachPhysRandom::randomDouble( -sizeY/2., sizeY/2. ),
                                      std::max(fireballSize/2.0, MachPhysRandom::randomDouble( 0, zTop ) ) );

        PhysRelativeTime    fireballStartTime = startTime + MachPhysRandom::randomDouble(0.3, 0.6);
        MATHEX_SCALAR depthOffset = -MachPhysRandom::randomDouble(0, std::min(xOffset, yOffset));

        MachPhysFireball* pFireball = _NEW( MachPhysFireball(
            pOwner,
            fireballPosition,
            MachPhysFireball::randomFireball(),
            fireballSize,
            depthOffset,
            fireballStartTime,
            duration ) );

       MATHEX_SCALAR fireballAngle=MachPhysRandom::randomDouble(0, (Mathex::PI) );
	   pFireball->rotate( MexRadians( fireballAngle ) );
    }

	size_t nPuffs = 1 + dimension/15.0;
	MATHEX_SCALAR puffSize = std::min(box.xLength(), box.yLength());
	MexPoint3d plumPosition(boxCentre.x(), boxCentre.y(), 0 );
	PhysRelativeTime plumeOffset = -10.0;

    //  Start the smoke plume at the very end of the fireballs
    MachPhysSmokePlume* pSmokePlume = _NEW( MachPhysSmokePlume(
        pOwner,
        plumPosition,	   //position
        dimension,		   //height
        0,				   //depthOffset
        nPuffs,
        puffSize,	 //avePuuSize
        MachPhysSmokePuff::randomPuff(),
        startTime + plumeOffset,
        plumeDuration ) );


    //Clear all current plans for the object
    if( pObject_->isComposite() )
        pObject_->asComposite().clearAllPlans();
    else
        pObject_->clearPlan();

	// shrink the building from full scale to 60%  over 0.1 second
	scaleObject(startTime, shrinkingDuration, 1.0, maxShrinkage);
	// expand the building from 60%  to 180%
	scaleObject(startTime+expansionOffset, expansionDuration, maxShrinkage, maxExpansion);

	// create a visibility plan which hides the building at duration
    W4dVisibilityPlanPtr objVisibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	objVisibilityPlanPtr->add(false, visibilityDuration);

	// set material plan for white glowing object
//  	static RenMaterial glowingWhite;
//      const int reasonableSize = 100;
//
//      static bool firstTime = true;
//      if( firstTime )
//      {
//      	glowingWhite.diffuse(RenColour::white());
//      	glowingWhite.emissive(RenColour::white());
//          firstTime = false;
//      }
//
//      PhysLinearScalarPlan::ScalarVec linearTimes;
//      	linearTimes.reserve(1);
//  		linearTimes.push_back( glowingDuration );
//
//      PhysLinearScalarPlan::ScalarVec scales;
//      	scales.reserve(2);
//          scales.push_back(0);
//          scales.push_back(0);
//
//      PhysLinearScalarPlan* pAlphaPlan = _NEW( PhysLinearScalarPlan(linearTimes, scales) );
//      PhysScalarPlanPtr alphaPlanPtr( pAlphaPlan );
//
//      W4dSimpleAlphaPlan* pPlan =
//                      _NEW( W4dSimpleAlphaPlan( glowingWhite, reasonableSize, alphaPlanPtr, glowingDuration ) );
//
//      W4dMaterialPlanPtr pMaterialPlanPtr( pPlan );
//
//    pObject_->entityPlanForEdit().materialPlan( pMaterialPlanPtr, startTime+glowingOffset );
//    pObject_->entityPlanForEdit().visibilityPlan( objVisibilityPlanPtr, startTime );
    for( W4dEntityIter it( pObject_ ); not it.isFinished(); ++it )
	{
//        (*it).entityPlanForEdit().materialPlan( pMaterialPlanPtr, startTime+glowingOffset );
        (*it).entityPlanForEdit().visibilityPlan( objVisibilityPlanPtr, startTime );
	}


	// create the demolition sequence hide it until the end of the shrinking-expansion phase
	MachPhysObjDemolish * pDemolish = _NEW ( MachPhysObjDemolish( pOwner, MexTransform3d(), type ) );

	// create a visibility plan which hides the demolistion sequence at startTime
    W4dVisibilityPlanPtr demolishVisibilityPlanPtr( _NEW( W4dVisibilityPlan( false ) ) );
	demolishVisibilityPlanPtr->add( true, demolitionOffset );
	demolishVisibilityPlanPtr->add( false, demolitionOffset+demolitionDuration );

    pDemolish->entityPlanForEdit().visibilityPlan( demolishVisibilityPlanPtr, startTime );
    for( W4dEntityIter it( pDemolish ); not it.isFinished(); ++it )
	{
        (*it).entityPlanForEdit().visibilityPlan( demolishVisibilityPlanPtr, startTime );
	}
    // plan the animation demolition sequence
	pDemolish->startDemolition( startTime+demolitionOffset, demolitionDuration, demolitionScaling, 1.0 );

	// give the demolition sequence to the garbage collector
    W4dGarbageCollector::instance().add( pDemolish, startTime + duration );

	//earth quake
	if( nFireballs > 1 )
	{
		W4dCameraShake& cameraShake = W4dManager::instance().sceneManager()->cameraShakeForEdit();
		cameraShake.startTime( startTime );
		cameraShake.epicCentre( pOwner->globalTransform().position() );
		cameraShake.duration( 2.0*burstTime );

		if( nFireballs == 3 )
			cameraShake.scale( W4dCameraShake::THIRD_DEGREE);

		if( nFireballs == 5 )
			cameraShake.scale( W4dCameraShake::SECOND_DEGREE);

		if( nFireballs > 5 )
			cameraShake.scale( W4dCameraShake::FIRST_DEGREE);
	}

	//get rid of the shadow
	if( pObject_->isComposite()	)
	{
	    for( W4dEntityIter it( pObject_ ); not it.isFinished(); ++it )
		{
		    if ((*it).name() == "SHADOW_PROJ" || (*it).name() == "SHADOW_FIXED")
		    {
	            (*it).visible( false );
			}
		}
	}

    return 2*(burstWaveOffset+burstWaveDuration);
}


void MachPhysObjectExplosion::scaleObject(
 PhysAbsoluteTime startTime,
 PhysRelativeTime duration,
 MATHEX_SCALAR startScale,
 MATHEX_SCALAR endScale
)
{


	W4dSimpleUniformScalePlan *pScalePlan = _NEW( W4dSimpleUniformScalePlan( startScale, endScale, duration ) );
	pObject_->propogateScalePlan( W4dScalePlanPtr( pScalePlan ), startTime );

}

void MachPhysObjectExplosion::sinkObject( PhysAbsoluteTime startTime, const PhysRelativeTime& duration, const MATHEX_SCALAR& height)
{
    //Clear all current plans for the object
    if( pObject_->isComposite() )
        pObject_->asComposite().clearAllPlans();
    else
        pObject_->clearPlan();

    MexTransform3d upTransform( pObject_->localTransform() );

    MexTransform3d downTransform = upTransform;
    MexPoint3d downPosition = upTransform.position();
    downPosition.z( downPosition.z() - height );
    downTransform.position( downPosition );

    //Set up motion plan accordingly
    PhysLinearMotionPlan* pMotionPlan = _NEW( PhysLinearMotionPlan(
        upTransform,  downTransform, duration ) );

    pObject_->entityPlanForEdit().absoluteMotion( PhysMotionPlanPtr( pMotionPlan ), startTime );
}

/* End OBJEXPLO.CPP *************************************************/
