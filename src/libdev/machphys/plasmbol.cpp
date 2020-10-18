/*
 * P L A S M B O L . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "phys/lsclplan.hpp"
#include "render/render.hpp"
#include "render/texture.hpp"
#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "render/material.hpp"
#include "render/texmgr.hpp"
#include "render/device.hpp"
#include "world4d/root.hpp"
#include "world4d/sprite3d.hpp"
#include "world4d/matseqpl.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/simplsca.hpp"
#include "world4d/alphsimp.hpp"
#include "world4d/garbcoll.hpp"
#include "world4d/soundman.hpp"
#include "world4d/generic.hpp"

#include "sim/manager.hpp"

#include "machphys/plasmbol.hpp"
#include "machphys/effects.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/private/otherper.hpp"
#include "machphys/snddata.hpp"
#include "machphys/plasplat.hpp"
#include "machphys/plasaura.hpp"

PER_DEFINE_PERSISTENT( MachPhysPlasmaBolt );

MachPhysPlasmaBolt::MachPhysPlasmaBolt
(
    W4dEntity* pParent,
	const MexTransform3d& localTransform,
	MachPhys::WeaponType type,
	CreateLights lights
)
:   MachPhysLinearProjectile( exemplar( type ), pParent, localTransform,
							 (lights==CREATE_LIGHTS)? COPY_LIGHTS: DONT_COPY_LIGHTS ),
    boltType_( type )
{
	// The current model is all emissive.  Hence, it should not need
	// lighting.  This could change if the model changes.
	doNotLight(true);

/*
    //Apply the material plan
    PhysAbsoluteTime now = SimManager::instance().currentTime();
    uint nRepetitions = 10000;
    entityPlanForEdit().materialPlan( boltMaterialPlan( type ), now, nRepetitions );

    //Add a TTF for the centre. Set up a material for this
    MATHEX_SCALAR size = MachPhysData::instance().weaponData( type ).extras()[0];
    W4dSprite3d* pTTF =
        _NEW( W4dSprite3d( this, MexTransform3d(), size, size, centreMaterial() ) );
    pTTF->depthOffset( -size );
*/

    TEST_INVARIANT;
}

MachPhysPlasmaBolt::MachPhysPlasmaBolt( MachPhys::WeaponType type )
:   MachPhysLinearProjectile( MachPhysOtherPersistence::instance().pRoot(), MexTransform3d() ),
    boltType_( type )
{
	// The current model is all emissive.  Hence, it should not need
	// lighting.  This could change if the model changes.
	doNotLight(true);

    //Load the STF mesh
    switch( type )
    {
        case MachPhys::PLASMA_RIFLE:
        {
            readCompositeFile( SysPathName( "models/weapons/plaboltr/plaboltr.cdf" ) );
            break;
        }

        case MachPhys::PLASMA_CANNON1:
        {
            readCompositeFile( SysPathName( "models/weapons/plaboltc/plaboltc.cdf" ) );
            break;
        }

        case MachPhys::PLASMA_CANNON2:
        {
            readCompositeFile( SysPathName( "models/weapons/plabolt2/plabolt2.cdf" ) );
            break;
        }

        DEFAULT_ASSERT_BAD_CASE( type );
    }

    TEST_INVARIANT;
}

MachPhysPlasmaBolt::MachPhysPlasmaBolt( PerConstructor con )
: MachPhysLinearProjectile( con )
{
}

MachPhysPlasmaBolt::~MachPhysPlasmaBolt()
{
    TEST_INVARIANT;

}

void MachPhysPlasmaBolt::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysPlasmaBolt& t )
{

    o << "MachPhysPlasmaBolt " << (void*)&t << " start" << std::endl;
    o << "MachPhysPlasmaBolt " << (void*)&t << " end" << std::endl;

    return o;
}

//static
const MachPhysPlasmaBolt& MachPhysPlasmaBolt::exemplar( MachPhys::WeaponType type )
{
	return MachPhysOtherPersistence::instance().plasmaBoltExemplar(type);
/*
    //Use the one time constructor
    MachPhysPlasmaBolt* pResult = NULL;

    switch( type )
    {
        case MachPhys::PLASMA_RIFLE:
        {
            static MachPhysPlasmaBolt* pBolt = _NEW( MachPhysPlasmaBolt( type ) );
            pResult = pBolt;
            break;
        }

        case MachPhys::PLASMA_CANNON1:
        {
            static MachPhysPlasmaBolt* pBolt = _NEW( MachPhysPlasmaBolt( type ) );
            pResult = pBolt;
            break;
        }

        case MachPhys::PLASMA_CANNON2:
        {
            static MachPhysPlasmaBolt* pBolt = _NEW( MachPhysPlasmaBolt( type ) );
            pResult = pBolt;
            break;
        }

        DEFAULT_ASSERT_BAD_CASE( type );
    }

    return *pResult;
*/
}

//virtual
PhysRelativeTime MachPhysPlasmaBolt::doBeDestroyedAt
(
    const PhysAbsoluteTime& time, MachPhys::StrikeType strikeType
)
{
    PhysRelativeTime animationDuration = 0.0;

    switch( strikeType )
    {
        case MachPhys::ON_OBJECT:
        case MachPhys::ON_TERRAIN:
        {
            //get the local position of the bolt at the destruct time
            MexTransform3d destructPosition;
            uint n;
            const W4dEntityPlan& myEntityPlan = entityPlan();
            if( myEntityPlan.hasMotionPlan() )
                entityPlan().transform( time, &destructPosition, &n );
            else
                destructPosition = localTransform();

            //Create an impact flash TTF at this position
            animationDuration = createImpactFlash( pParent(), destructPosition, time );
            break;
        }
    }

    return animationDuration;
}

//static
W4dMaterialPlan* MachPhysPlasmaBolt::createBoltMaterialPlan( MachPhys::WeaponType type )
{
    //Get the base material for the type
    const RenMeshInstance& meshInstance = exemplar( type ).mesh();
  	std::auto_ptr<RenMaterialVec> materialVecAPtr = meshInstance.mesh()->materialVec();

    RenMaterial baseMaterial = materialVecAPtr->front();
    uint nMaterialsPerVector = materialVecAPtr->size();

    //Construct the plan
    MachPhysEffects::Textures textures = MachPhysEffects::flameTextures();
    PhysAbsoluteTime duration = textures.size() * 0.05;

    W4dMaterialSequencePlan* pPlan =
        MachPhysEffects::createMaterialSequencePlan( baseMaterial, textures, nMaterialsPerVector,
                                                     duration, W4dLOD( 0 ) );

    return pPlan;
}

const W4dMaterialPlanPtr& MachPhysPlasmaBolt::boltMaterialPlan( MachPhys::WeaponType type ) const
{
    static W4dMaterialPlanPtr resultPtr;

    switch( type )
    {
        case MachPhys::PLASMA_RIFLE:
        {
            static W4dMaterialPlanPtr riflePlanPtr( createBoltMaterialPlan( type ) );
            resultPtr = riflePlanPtr;
            break;
        }

        case MachPhys::PLASMA_CANNON1:
        case MachPhys::PLASMA_CANNON2:
        {
            static W4dMaterialPlanPtr cannonPlanPtr( createBoltMaterialPlan( type ) );
            resultPtr = cannonPlanPtr;
            break;
        }

        DEFAULT_ASSERT_BAD_CASE( type );
    }

    return resultPtr;
}

//static
const RenMaterial& MachPhysPlasmaBolt::centreMaterial()
{
    static RenMaterial theMaterial( RenColour( 0.0, 188.0/255.0, 0.0 ) );
    static bool firstTime = true;
    if( firstTime )
    {
        firstTime = false;
        theMaterial.texture( RenTexManager::instance().createTexture("ptop_bt.bmp") );
    }

    return theMaterial;
}

/*
PhysRelativeTime MachPhysPlasmaBolt::createImpactFlash
(
    W4dEntity* pParent, const MexTransform3d& localPosition, const PhysAbsoluteTime& startTime
)
{
    //Counted pointers etc for standard plans
    static W4dVisibilityPlanPtr visibilityPlanPtr;
    static W4dScalePlanPtr scalePlanPtr;
    static RenMaterial flashMaterial;
    static PhysScalarPlanPtr alphaValuePlanPtr;
    PhysRelativeTime duration = 0.33;

    //First time set up standard plans
    static bool firstTime = true;
    if( firstTime )
    {
        firstTime = false;

        //Create the visibility plan
        W4dVisibilityPlan* pVisibilityPlan = _NEW( W4dVisibilityPlan( true ) );
        pVisibilityPlan->add( false, duration );
        visibilityPlanPtr = pVisibilityPlan;

        //Create the scale plan
        W4dSimpleUniformScalePlan* pScalePlan =
            _NEW( W4dSimpleUniformScalePlan( 1.0, 5.0, duration ) );
        scalePlanPtr = pScalePlan;

        //Set the flash material
        flashMaterial.diffuse( RenColour( 0.0, 188.0/255.0, 0.0 ) );
        flashMaterial.texture( RenTexManager::instance().createTexture("shkwv_bt.bmp") );

        //Create the alpha scalar plan
        ctl_vector< PhysRelativeTime > times;
        times.reserve( 1 );
        times.push_back( duration );

        ctl_vector< MATHEX_SCALAR > alphaValues;
        alphaValues.reserve( 2 );
        alphaValues.push_back( 1.0 );
        alphaValues.push_back( 0.0 );

        PhysLinearScalarPlan* pAlphaValuePlan =
            _NEW( PhysLinearScalarPlan( times, alphaValues ) );
        alphaValuePlanPtr = pAlphaValuePlan;
    }

    //Construct a TTF at the specified location
    MATHEX_SCALAR size = 1.0;
    W4dSprite3d* pTTF = _NEW( W4dSprite3d( pParent, localPosition, size, size, flashMaterial ) );

    //Make invisible till needed
    pTTF->visible( false );

    //Set its visibility plan
    W4dEntityPlan& entityPlan = pTTF->entityPlanForEdit();
    entityPlan.visibilityPlan( visibilityPlanPtr, startTime );

    //Set the scaling plan
    entityPlan.scalePlan( scalePlanPtr, startTime );

    //Set up the alpha plan. We need a fresh copy for each TTF because it
    //uses a cached material.
    W4dSimpleAlphaPlan* pAlphaPlan =
        _NEW( W4dSimpleAlphaPlan( flashMaterial, 1, alphaValuePlanPtr, 2 ) );
    W4dMaterialPlanPtr alphaPlanPtr( pAlphaPlan );
    entityPlan.materialPlan( alphaPlanPtr, startTime );

    //Add to the garbage collection list. Let it survive, so the sound
    //associated with it isn't stopped too early
    W4dGarbageCollector::instance().add( pTTF, startTime + 4.0 );

    //Attach an impact sound to the impact thingy
//    char* pWaveFile = (boltType_ == MachPhys::PLASMA_RIFLE ? "sounds/plrhit.wav" :
//                                                             "sounds/plchit.wav");
//    W4dSoundManager::instance().play( pTTF, SysPathName( pWaveFile ), startTime,
//                                      100.0, 100.0, W4dSoundManager::PLAY_ONCE );

    SoundId sndData = (boltType_ == MachPhys::PLASMA_RIFLE ? SID_PLRHIT :
                                                             	  SID_PLCHIT);
	W4dSoundManager::instance().play( pTTF, sndData,
	                                  startTime, 1 );

    return duration;
}
*/

void perWrite( PerOstream& ostr, const MachPhysPlasmaBolt& bolt )
{
    const MachPhysLinearProjectile& base = bolt;

    ostr << base;
    ostr << bolt.boltType_;
}

void perRead( PerIstream& istr, MachPhysPlasmaBolt& bolt )
{
    MachPhysLinearProjectile& base = bolt;

    istr >> base;
    istr >> bolt.boltType_;
}

PhysRelativeTime MachPhysPlasmaBolt::createImpactFlash
(
    W4dEntity* pParent, const MexTransform3d& localPosition, const PhysAbsoluteTime& startTime
)
{
	PhysRelativeTime duration = 0.33;

	MachPhysPlasmaAura* pAura = _NEW( MachPhysPlasmaAura( pParent, localPosition) );
	pAura->startPlasmaAura( startTime, duration );

	MachPhysPlasmaSplat* pSplat = _NEW( MachPhysPlasmaSplat( pParent, localPosition) );
	pSplat->startPlasmaSplat( startTime, duration );

	SoundId sndData = (boltType_ == MachPhys::PLASMA_RIFLE ? SID_PLRHIT :
	                                                         	  SID_PLCHIT);
	//Cache the time that the last impact flash was played,
	//so that we don't get too many sounds playing at
	//the same time.
	static PhysRelativeTime lastTimePlayed = 0;
	const PhysRelativeTime acceptableTime = 1.0;

    W4dGeneric* pSplatSite = NULL;
	//Only play sound if it is an acceptable time away from the
	//previous play
	if(startTime > lastTimePlayed + acceptableTime)
	{
	    pSplatSite = _NEW(W4dGeneric(pParent, localPosition, W4dEntity::NOT_SOLID));
		W4dSoundManager::instance().play( pSplatSite, sndData,
	                                  startTime, 1 );
		lastTimePlayed = startTime;
	}

	 //Add to the garbage collection list. Let it survive, so the sound
	//associated with it isn't stopped too early
	W4dGarbageCollector::instance().add( pSplat, startTime + duration);
	W4dGarbageCollector::instance().add( pAura, startTime + duration );
	if(pSplatSite)
		W4dGarbageCollector::instance().add( pSplatSite, startTime + duration + 5 );

	return duration;
}

/* End PLASMBOL.CPP *************************************************/
