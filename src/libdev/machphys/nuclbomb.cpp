/*
 * N U C L B O M B . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions


#include "machphys/nuclbomb.hpp"

#include "ctl/countptr.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/abox3d.hpp"
#include "phys/rampacce.hpp"
#include "phys/motplan.hpp"
#include "phys/linemoti.hpp"
#include "phys/asclplan.hpp"
#include "phys/lsclplan.hpp"
#include "phys/motplan.hpp"
#include "render/device.hpp"
#include "render/colour.hpp"
#include "render/texmgr.hpp"
#include "render/texture.hpp"
#include "render/matvec.hpp"
#include "render/scale.hpp"
#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "render/material.hpp"
#include "world4d/alphsimp.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/gusplan.hpp"
#include "world4d/matfrmpl.hpp"
#include "world4d/manager.hpp"
#include "world4d/scenemgr.hpp"
#include "world4d/camera.hpp"
#include "world4d/soundman.hpp"
#include "world4d/uvplan.hpp"
#include "world4d/uvtrans.hpp"
#include "world4d/simplsca.hpp"
#include "world4d/light.hpp"
#include "world4d/camshake.hpp"
#include "world4d/garbcoll.hpp"

#include "system/pathname.hpp"

#include "machphys/nukewave.hpp"
#include "machphys/machphys.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/random.hpp"
#include "machphys/mushtop.hpp"
#include "machphys/mushshaf.hpp"
#include "machphys/mushedge.hpp"
#include "machphys/splat.hpp"
#include "machphys/flash.hpp"
#include "machphys/plansurf.hpp"
#include "machphys/snddata.hpp"
#include "machphys/punch.hpp"
#include "machphys/random.hpp"

PER_DEFINE_PERSISTENT( MachPhysNuclearBomb );

//local
static PhysScalarPlanPtr scalarPlanPtr(const PhysRelativeTime& time, const double& alpha);

MachPhysNuclearBomb::MachPhysNuclearBomb(W4dEntity* pParent, const MexTransform3d& localTransform)
:   W4dEntity( pParent, localTransform, W4dEntity::NOT_SOLID )
{
	// The current model is all emissive or black.  Hence, it should not need
	// lighting.  This could change if the model changes.
	doNotLight(true);

	pNukeWave_  = _NEW( MachPhysNukeWave(this, MexTransform3d() ) );
	pMushroomTop_ = _NEW( MachPhysMushroomTop(this, MexPoint3d(0, 0, 6) ) );
	pMushroomShaft_ = _NEW( MachPhysMushroomShaft(this, MexTransform3d() ) );
	pMushroomEdge_ = NULL; //_NEW( MachPhysMushroomEdge(this, MexTransform3d() ) );
	//splat no longer needed
	//pSplat_ = NULL; //_NEW( MachPhysGroundSplat(this, MexTransform3d() ) );
	pFlash_ = _NEW( MachPhysFlashDisc(W4dManager::instance().sceneManager()->currentCamera(),
	                                  MexPoint3d(4, 0, 0),
	                                  10.0,
	                                  MachPhysFlashDisc::WHITE) );
	pLight_ = _NEW( W4dUniformLight(this, MexVec3(0, 0, 1), 1000.0));

    pLight_->localTransform( MexPoint3d(0, 0, 60) );	//highest position of the mashroom
	pLight_->colour(RenColour(6, 2.24, 0.56) );
	pLight_->constantAttenuation(0);
	pLight_->linearAttenuation(0.177);
	pLight_->quadraticAttenuation(0.823);
	pLight_->scope(W4dLight::LOCAL);
	pLight_->visible(false);
	pLight_->turnOn();

	// We want the light to illuminate everything *except* the mushroom.  Turn off
	// lighting for this object.
	doNotLight(true);

    TEST_INVARIANT;
}

MachPhysNuclearBomb::MachPhysNuclearBomb( PerConstructor con )
: W4dEntity( con )
{
}

MachPhysNuclearBomb::~MachPhysNuclearBomb()
{
    TEST_INVARIANT;

    //Stop any playing sound
    W4dSoundManager::instance().stop( this );
}


void MachPhysNuclearBomb::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysNuclearBomb& t )
{

    o << "MachPhysNuclearBomb " << (void*)&t << " start" << std::endl;
    o << "MachPhysNuclearBomb " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysNuclearBomb::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

//static
const PhysRelativeTime& MachPhysNuclearBomb::exPlosionDuration()
{
	static const PhysRelativeTime DURATION = 400/25.0;
	return DURATION;
}

PhysRelativeTime MachPhysNuclearBomb::startExplosion( const PhysAbsoluteTime& startTime, MachPhysPlanetSurface* pSurface)
{
	const PhysRelativeTime frameToSecond = 15.0;

	const MachPhysWeaponData& weapData = MachPhysData::instance().weaponData(MachPhys::NUCLEAR_MISSILE);

    MATHEX_SCALAR fromRadius = weapData.extras()[12];
    MATHEX_SCALAR toRadius = weapData.extras()[13];
    const MATHEX_SCALAR zScale = 1.0;

	const PhysRelativeTime duration = exPlosionDuration(); // = 400/25.0; 4*weapData.extras()[2];
	const PhysRelativeTime waveDuration = weapData.extras()[14];

	//start the NukeWave at the end of the white flach
   	pNukeWave_->startNukeWave(startTime + weapData.extras()[6], waveDuration, fromRadius, toRadius, zScale);

	//visibility plan
    W4dVisibilityPlanPtr mushroomVisibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	mushroomVisibilityPlanPtr->add(false, duration);

	//nuke light
	MATHEX_SCALAR maxIntensity = weapData.extras()[9];
	MATHEX_SCALAR durationPar = weapData.extras()[10];
	MATHEX_SCALAR range = weapData.extras()[11];
	pLight_->maxRange( range );

	PhysRelativeTime lightDuration = duration*durationPar;
    W4dVisibilityPlanPtr lightVisibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	lightVisibilityPlanPtr->add(false, lightDuration);

    pLight_->entityPlanForEdit().visibilityPlan( lightVisibilityPlanPtr, startTime );

	PhysScalarPlanPtr intensityPlanPtr = scalarPlanPtr(lightDuration, maxIntensity);
	pLight_->intensityPlan(intensityPlanPtr, startTime);

	MexPoint3d boxCentre = pLight_->globalTransform().position();
	MATHEX_SCALAR dimension = pLight_->maxRange();
				  dimension *= 2.0;
	MexAlignedBox3d lightCoveredBox( boxCentre,	dimension, dimension, dimension );
	pSurface->addLocalLight( pLight_,  lightCoveredBox );

    RenNonUniformScale a(1, 1, 1);
    RenNonUniformScale b(1, 1, 7);
    W4dScalePlanPtr scalePlanPtr( _NEW( W4dSimpleNonUniformScalePlan( a, b, duration ) ) );

	MATHEX_SCALAR speed = weapData.extras()[8]; //0.0287;

	pMushroomShaft_->propogateScalePlan( scalePlanPtr, startTime );
    pMushroomShaft_->entityPlanForEdit().visibilityPlan( mushroomVisibilityPlanPtr, startTime );

	//pMushroomEdge_->propogateScalePlan( scalePlanPtr, startTime );
    //pMushroomEdge_->entityPlanForEdit().visibilityPlan( mushroomVisibilityPlanPtr, startTime );

	b = RenNonUniformScale(2.67, 2.67, 2.67);
    W4dScalePlanPtr topScalePlanPtr( _NEW( W4dSimpleNonUniformScalePlan( a, b, duration ) ) );

	pMushroomTop_->propogateScalePlan( topScalePlanPtr, startTime );
    pMushroomTop_->entityPlanForEdit().visibilityPlan( mushroomVisibilityPlanPtr, startTime );

	PhysMotionPlanPtr motionPlanPtr =
	        _NEW( PhysLinearMotionPlan( MexPoint3d(0, 0, 6), MexPoint3d(0, 0, 60),  duration ) );

    pMushroomTop_->entityPlanForEdit().absoluteMotion( motionPlanPtr, startTime );

	//white flash alpha plan
	static PhysScalarPlanPtr lineScalarPlanPtr;
	static const PhysRelativeTime flashDuration = weapData.extras()[6];

	static bool first = true;
	if( first )
	{
	    PhysLinearScalarPlan::ScalarVec times;
	    	times.reserve(3);
			times.push_back(weapData.extras()[4]);
			times.push_back(weapData.extras()[5]);
			times.push_back(flashDuration);

	    PhysLinearScalarPlan::ScalarVec scales;
	    	scales.reserve(4);
	        scales.push_back(1.0);
	        scales.push_back(1.0);
	        scales.push_back(weapData.extras()[7]);
	        scales.push_back(0.0);

	    lineScalarPlanPtr = _NEW( PhysLinearScalarPlan(times, scales) );

	}

    W4dSimpleAlphaPlan::makePlan( pFlash_, startTime, lineScalarPlanPtr, 1 );

	//flash visibility
    W4dVisibilityPlanPtr flashVisibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	flashVisibilityPlanPtr->add(false, flashDuration);
	pFlash_->entityPlanForEdit().visibilityPlan( flashVisibilityPlanPtr, startTime );

	//mashroom shaft  alpha plan

	static const PhysRelativeTime shaftDuration = 40.0/25.0;
	static const PhysScalarPlanPtr shaftAlphaPlanPtr = scalarPlanPtr(shaftDuration, 1.0);
	const PhysAbsoluteTime shaftStartTime = startTime+duration-shaftDuration;
    W4dSimpleAlphaPlan::makePlan( pMushroomShaft_, shaftStartTime, shaftAlphaPlanPtr, 1 );

	//mashroom top  alpha plan

	static const PhysRelativeTime topDuration = 34.0/25.0;
	const PhysAbsoluteTime topStartTime = startTime+duration-topDuration;
	static const PhysScalarPlanPtr topAlphaPlanPtr = scalarPlanPtr(topDuration, 1.0);

    W4dSimpleAlphaPlan::makePlan( pMushroomTop_, topStartTime, topAlphaPlanPtr, 1 );

	//crack and fire:
	const uint nCrack = MachPhysRandom::randomInt(4, 6);
	MachPhysPunchBlast* pCrack[6];
	MexTransform3d crackXform[6];
	PhysAbsoluteTime crackStartTime[6];

	const MATHEX_SCALAR angleSeperation = 360.0/nCrack;
	const MATHEX_SCALAR crackLifeTimeStart = duration*0.05;
	const MATHEX_SCALAR crackLifeTimeEnd = crackLifeTimeStart + duration*0.2;

	for( size_t iCrack=0; iCrack<nCrack; ++iCrack)
	{
		crackXform[iCrack] = MexTransform3d();
		MATHEX_SCALAR angleOffset = MachPhysRandom::randomDouble(-10, 10);
		crackXform[iCrack].rotation( MexEulerAngles(MexDegrees(iCrack*angleSeperation + angleOffset ), 0, 0) );
		pCrack[iCrack] = _NEW( MachPhysPunchBlast( this, crackXform[iCrack] ) );
		crackStartTime[iCrack] = startTime + MachPhysRandom::randomDouble(crackLifeTimeStart, crackLifeTimeEnd);
		PhysRelativeTime crackDuration = pCrack[iCrack]->startPunchBlast(crackStartTime[iCrack], *pSurface, MachPhysPunchBlast::NUKE);
		W4dGarbageCollector::instance().add( pCrack[iCrack], crackStartTime[iCrack] + crackDuration );
	}

	//earth quake
	W4dCameraShake& cameraShake = W4dManager::instance().sceneManager()->cameraShakeForEdit();
	cameraShake.startTime( startTime );
	cameraShake.epicCentre( this->globalTransform().position() );
	cameraShake.duration( 2.0*flashDuration );
	cameraShake.scale( W4dCameraShake::FIRST_DEGREE);

    //Play the sound
   W4dSoundManager::instance().play( this, SID_NUKE,
   									 startTime, 1 );

    return std::max(duration, lightDuration); //fireBallDuration + 25.0/frameToSecond;
}


//local
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

void perWrite( PerOstream& ostr, const MachPhysNuclearBomb& bomb )
{
    const W4dEntity& base = bomb;

    ostr << base;
	ostr << bomb.pNukeWave_;
	ostr << bomb.pMushroomTop_;
	ostr << bomb.pMushroomShaft_;
	ostr << bomb.pMushroomEdge_;
	//ostr << bomb.pSplat_;
	ostr << bomb.pFlash_;
	ostr << bomb.pLight_;
}

void perRead( PerIstream& istr, MachPhysNuclearBomb& bomb )
{
    W4dEntity& base = bomb;

    istr >> base;
	istr >> bomb.pNukeWave_;
	istr >> bomb.pMushroomTop_;
	istr >> bomb.pMushroomShaft_;
	istr >> bomb.pMushroomEdge_;
	//istr >> bomb.pSplat_;
	istr >> bomb.pFlash_;
	istr >> bomb.pLight_;
}

//static
MATHEX_SCALAR MachPhysNuclearBomb::shockwaveRadius( const PhysRelativeTime& timeOffset )
{
	static const MachPhysWeaponData& weapData = MachPhysData::instance().weaponData(MachPhys::NUCLEAR_MISSILE);

    static const MATHEX_SCALAR fromRadius = weapData.extras()[12];
    static const MATHEX_SCALAR toRadius = weapData.extras()[13];
	static const PhysRelativeTime waveDuration = weapData.extras()[14];
	static const MATHEX_SCALAR gradient = (toRadius - fromRadius)/waveDuration;

	return 	fromRadius + timeOffset*gradient;
}
/* End NUCLBOMB.CPP *************************************************/

