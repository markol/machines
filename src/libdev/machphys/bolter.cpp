/*
 * B O L T E R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/bolter.hpp"
#include "machphys/snddata.hpp"
#include "render/texture.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/effects.hpp"
#include "machphys/smkcloud.hpp"
#include "machphys/random.hpp"
#include "machphys/machine.hpp"
#include "machphys/constron.hpp"
#include "machphys/lineproj.hpp"
#include "machphys/artefact.hpp"
#include "machphys/private/weapper.hpp"
#include "machphys/snddata.hpp"
#include "machphys/attack.hpp"

#include "world4d/root.hpp"
#include "world4d/soundman.hpp"
#include "world4d/generic.hpp"
#include "world4d/garbcoll.hpp"
#include "world4d/sprite3d.hpp"
#include "world4d/link.hpp"
#include "world4d/visplan.hpp"
#include "world4d/entyplan.hpp"
#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "render/colour.hpp"
#include "render/scale.hpp"
#include "render/texmgr.hpp"
#include "phys/linemoti.hpp"
#include "phys/timespin.hpp"
#include "mathex/vec2.hpp"
#include "mathex/line3d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/vec3.hpp"
#include "system/pathname.hpp"
#include "sim/manager.hpp"

PER_DEFINE_PERSISTENT( MachPhysBolter );

MachPhysBolter::MachPhysBolter
(
    W4dEntity* pParent, const MexTransform3d& localTransform,
    MachPhys::WeaponType type, MachPhys::Mounting mounting
)
:   MachPhysWeapon( exemplar( type ), mounting, pParent, localTransform ),
    pTurnLink_( NULL )
{
    //Copy turn link if present
    W4dLink* pOldLink = exemplar( type ).pTurnLink_;
    if( pOldLink )
        pTurnLink_ = links()[ pOldLink->id() ];

    TEST_INVARIANT;
}

//One-time ctor
MachPhysBolter::MachPhysBolter( MachPhys::WeaponType type )
:   MachPhysWeapon( MachPhysWeaponPersistence::instance().pRoot(), MexTransform3d(),
                    SysPathName( compositeFilePath( type ) ), type, MachPhys::LEFT ),
    pTurnLink_( NULL )
{
    //For the auto-cannon find the turning link
    if( type == MachPhys::AUTO_CANNON )
        findLink( "gun", &pTurnLink_ );

    TEST_INVARIANT;
}

MachPhysBolter::MachPhysBolter( PerConstructor con )
: MachPhysWeapon( con )
{
}

MachPhysBolter::~MachPhysBolter()
{
    TEST_INVARIANT;

}

//static
const MachPhysBolter& MachPhysBolter::exemplar( MachPhys::WeaponType type )
{
    return MachPhysWeaponPersistence::instance().bolterExemplar( type );
}

void MachPhysBolter::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysBolter& t )
{

    o << "MachPhysBolter " << (void*)&t << " start" << std::endl;
    o << "MachPhysBolter " << (void*)&t << " end" << std::endl;

    return o;
}

//static
const char* MachPhysBolter::compositeFilePath( MachPhys::WeaponType type )
{
    switch( type )
    {
        case MachPhys::BOLTER:
            return "models/weapons/bolter/bltr.cdf";
        case MachPhys::AUTO_CANNON:
            return "models/weapons/auto/auto.cdf";
        case MachPhys::HEAVY_BOLTER1:
            return "models/weapons/heavyb1/hvyb1.cdf";
        case MachPhys::HEAVY_BOLTER2:
            return "models/weapons/heavyb2/hvyb2.cdf";
    }

    return "";
}

//virtual
PhysRelativeTime MachPhysBolter::fire( const PhysAbsoluteTime& startTime, int numberInBurst )
{
    //get weapon data
    const MachPhysWeaponData& data = weaponData();
    const MexPoint3d& flashOffset = data.launchOffsets()[0];
    uint nRounds = data.nRoundsPerBurst();
    PhysRelativeTime burstDuration = data.burstDuration();
    PhysRelativeTime recoilBackTime = data.recoilBackTime();

    //Set up timing
    PhysAbsoluteTime roundTime = burstDuration / nRounds;
    PhysRelativeTime endTime = startTime + burstDuration;

    //Attach a flash object to the gun
    MexTransform3d flashPosition( flashOffset );
    W4dGeneric* pFlash = _NEW( W4dGeneric( exemplarFlash( type() ), this, flashPosition ) );
    pFlash->visible( false );

	if(type()==MachPhys::BOLTER)
		pFlash->temporaryScale( RenUniformScale(0.8), W4dEntity::NOT_PROPOGATE );

    if( type() == MachPhys::HEAVY_BOLTER1 or type() == MachPhys::HEAVY_BOLTER2)
        pFlash->temporaryScale( RenNonUniformScale(1.5, 1, 1), W4dEntity::NOT_PROPOGATE );

    //Set up a visibility plan for the flash
    W4dVisibilityPlan* pVisibilityPlan = _NEW( W4dVisibilityPlan( true ) );
    pVisibilityPlan->add( false, 0.1 );
    pVisibilityPlan->add( false, roundTime );

    W4dVisibilityPlanPtr visibilityPlanPtr( pVisibilityPlan );
    pFlash->entityPlanForEdit().visibilityPlan( visibilityPlanPtr, startTime, nRounds - 1 );

    //Regular bolters recoil, but the autocannon rotates its turn link
    bool isAutoCannon = type() == MachPhys::AUTO_CANNON;
    if( isAutoCannon )
    {
        if( pTurnLink_ )
        {
            //Create a motion plan for spinning the auto-cannon link
            static PhysMotionPlanPtr spinPlanPtr;
            static bool firstTime = true;
            if( firstTime )
            {
                //It has six barrels, so each round turns 60 degrees
                MexRadians angularSpeed = MexDegrees( 60.0 * nRounds ) / burstDuration;
                size_t likelySegmentCount = 1;

                PhysTimedSpinPlan* pSpinPlan =
                    _NEW( PhysTimedSpinPlan( MexVec3(1.0, 0.0, 0.0),
                                             pTurnLink_->localTransform(),
                                             MexRadians( 0.0 ), angularSpeed,
                                             likelySegmentCount ) );

                pSpinPlan->addSegment( burstDuration, MexRadians( 0.0 ) );

                spinPlanPtr = pSpinPlan;
            }

            pTurnLink_->entityPlanForEdit().absoluteMotion( spinPlanPtr, startTime );
        }
    }
    else
    {
        recoil( startTime, numberInBurst );
    }

    //Add the exhaust smoke cloud
    MachPhysSmokeCloud* pSmokeCloud = _NEW( MachPhysSmokeCloud ( this, flashOffset ) );
    uint nWisps = 3;
    MexPoint3d endPosition( 0.0,0.0,1.1 );
    MexVec3 randomOffset( 0.1,0.1,0.0 );
    PhysRelativeTime wispInterval = 0.08;
    MATHEX_SCALAR wispSize = (isAutoCannon ? 6.0 : 1.8);
    MATHEX_SCALAR zDepthOffset = -1.0;
    PhysRelativeTime smokeDuration =
        pSmokeCloud->startSmoking( nWisps, endPosition, randomOffset, endTime,
                                   wispInterval, wispSize, zDepthOffset,
                                   MachPhysEffects::smokeMaterialPlan( MachPhysEffects::GREY ) );

	//lighting up the machine and the weapon
	MATHEX_SCALAR  amplitude = weaponData().extras()[0];
	lighting(RenColour(amplitude, amplitude, amplitude*0.87), startTime, 3.0, MachPhysWeapon::SHADOW);

    //Garbage collect the flash and exhaust
    W4dGarbageCollector::instance().add( pSmokeCloud, endTime + smokeDuration );
    W4dGarbageCollector::instance().add( pFlash, endTime );

    //Play the firing sounds
    playFiringSounds( startTime );

    return burstDuration;
}

//static
const W4dGeneric& MachPhysBolter::exemplarFlash( MachPhys::WeaponType type )
{
/*
    W4dGeneric* pFlash = NULL;

	switch( type )
	{
        case MachPhys::BOLTER:
        {
            static W4dGeneric* pExemplar = newFlash( type );
            pFlash = pExemplar;
            break;
        }
        case MachPhys::HEAVY_BOLTER1:
        case MachPhys::HEAVY_BOLTER2:
        {
            static W4dGeneric* pExemplar = newFlash( type );
            pFlash = pExemplar;
            break;
        }
        case MachPhys::AUTO_CANNON:
        {
            static W4dGeneric* pExemplar = newFlash( type );
            pFlash = pExemplar;
            break;
		}
	}

    POST( pFlash != NULL );
    return *pFlash;
*/
    return MachPhysWeaponPersistence::instance().bolterFlashExemplar( type );

}

//static
void MachPhysBolter::impactSparks
(
    const PhysAbsoluteTime& startTime, W4dEntity* pEntity,
    const MexPoint3d& startPoint, const MexVec3& /*direction*/
)
{
    //get the material plan
    W4dMaterialPlanPtr sparkPlanPtr( MachPhysEffects::sparksMaterialPlan() );

    //Set various values
    const MATHEX_SCALAR sparkSize = 2.5;
    const PhysRelativeTime duration = sparkPlanPtr->duration();
    const PhysAbsoluteTime endTime = startTime + duration;

    //Set the uv coordinates for the sparks
    //MexVec2 aUV[4];
    //aUV[0] = MexVec2(0.03, 0.64);
    //aUV[1] = MexVec2(0.03, 0.34);
    //aUV[2] = MexVec2(0.74, 0.34);
    //aUV[3] = MexVec2(0.74, 0.64);

    //Define a visibility plan
    W4dVisibilityPlan* pVisibilityPlan = _NEW( W4dVisibilityPlan( true ) );
    pVisibilityPlan->add( false, duration );

    W4dVisibilityPlanPtr visibilityPlanPtr( pVisibilityPlan );

    //Set up initial transform
    MexTransform3d startPosition( startPoint );
/*
    //Set up random offsets
    MATHEX_SCALAR minRange = 0.25;
    MATHEX_SCALAR maxRange = 0.75;
    MATHEX_SCALAR xOffset = MachPhysRandom::randomDouble( minRange, maxRange );
    MATHEX_SCALAR yOffset = -MachPhysRandom::randomDouble( minRange, maxRange );
    MATHEX_SCALAR zOffset = MachPhysRandom::randomDouble( minRange, maxRange );
*/
    //Generate 1 TTFs
    for( uint i = 1; i--; )
    {
        //Create the sprite
        W4dSprite3d* pSpark =
            _NEW( W4dSprite3d( pEntity, startPosition, sparkSize, sparkSize, RenMaterial() ) );
        pSpark->depthOffset( -sparkSize );

		W4dSoundManager::instance().play(pEntity, SID_BOLTER_HIT, PhysAbsoluteTime( 0 ), 1);

        //Attach the visibility plan, ensuring invisible until required
        pSpark->visible( false );
        W4dEntityPlan& sparkEntityPlan = pSpark->entityPlanForEdit();
        sparkEntityPlan.visibilityPlan( visibilityPlanPtr, startTime );
        //Attach the material plan
        sparkEntityPlan.materialPlan( sparkPlanPtr, startTime );

        //Garbage collect the spark
        W4dGarbageCollector::instance().add( pSpark, endTime );
    }
}

//static
const RenMaterial& MachPhysBolter::sparkMaterial()
{
    //Create a material using the blam texture

    static RenMaterial mat;
    static bool firstTime = true;
    if( firstTime )
    {
        firstTime = false;

    	RenTexture texture = RenTexManager::instance().createTexture("blam_bt.bmp");
	    mat.texture( texture );
        mat.diffuse(RenColour::black());
        mat.emissive(RenColour::white());

    }

    return mat;
}

//virtual
PhysRelativeTime MachPhysBolter::victimAnimation
(
    const PhysAbsoluteTime& startTime, const MexLine3d& fromDirection, MachPhysMachine* pMachine
) const
{
    return applyVictimAnimation( startTime, fromDirection, pMachine );
}

//virtual
PhysRelativeTime MachPhysBolter::victimAnimation
(
    const PhysAbsoluteTime& startTime, const MexLine3d& fromDirection, MachPhysConstruction* pConstruction
) const
{
    return applyVictimAnimation( startTime, fromDirection, pConstruction );
}

//static
PhysRelativeTime MachPhysBolter::applyVictimAnimation
(
    const PhysAbsoluteTime& startTime, const MexLine3d& fromDirection,
    MachPhysMachine* pMachine
)
{
    PRE( fromDirection.length() != 0.0 );

    //Make the machine shake
    PhysRelativeTime shakeTime = pMachine->shake( startTime );

    //Apply sparks at the intersection between the direction line, and the impact point.
    MexVec3 unitDirection;
    MexPoint3d impactPoint;

    if( MachPhysLinearProjectile::impactData( *pMachine, fromDirection, &impactPoint, &unitDirection ) )
    {
        //Apply sparks to the impact point
        impactSparks( startTime, pMachine, impactPoint, unitDirection );
    }

    return shakeTime;
}

//static
PhysRelativeTime MachPhysBolter::applyVictimAnimation
(
    const PhysAbsoluteTime& startTime, const MexLine3d& fromDirection,
    MachPhysConstruction* pConstruction
)
{
    PRE( fromDirection.length() != 0.0 );

    //Apply sparks at the intersection between the direction line, and the impact point.
    MexVec3 unitDirection;
    MexPoint3d impactPoint;

    if( MachPhysLinearProjectile::impactData( *pConstruction, fromDirection, &impactPoint, &unitDirection ) )
    {
        //Apply sparks to the impact point
        impactSparks( startTime, pConstruction, impactPoint, unitDirection );
    }

    return 0.0;
}

void MachPhysBolter::playFiringSounds( const PhysAbsoluteTime& startTime )
{
    //get the number of rounds, burst duration, and hence compute interval
    const MachPhysWeaponData& data = weaponData();
    uint nRounds = data.nRoundsPerBurst();
    PhysRelativeTime duration = data.burstDuration();
    PhysRelativeTime interval = duration / nRounds;
	SoundId sndData = SID_BOLTER;
    //Get the main wave file name
    char* pWaveFileName = NULL;

    switch( type() )
    {
        case MachPhys::BOLTER:
        {
			sndData = SID_BOLTER;
//            pWaveFileName = "sounds/bolter.wav";
            break;
        }
        case MachPhys::HEAVY_BOLTER1:
        case MachPhys::HEAVY_BOLTER2:
        {
			sndData = SID_HEAVYBOLTER;
//            pWaveFileName = "sounds/hevybolt.wav";
            break;
        }
        case MachPhys::AUTO_CANNON:
        {
			sndData = SID_AUTOCANNON;
//            pWaveFileName = "sounds/autocan.wav";

		    W4dSoundManager::instance().play( this, SID_AUTOBK,
                                      PhysAbsoluteTime( 0 ), 1 );
            //Also play the background sound
//            W4dSoundManager::instance().play( this, SysPathName( "sounds/autobk.wav" ), startTime,
//                                              100.0, 100.0, W4dSoundManager::PLAY_ONCE );
            break;
        }
    }

    //Play the firing sound once per burst
//    SysPathName wavePath( pWaveFileName );
    PhysAbsoluteTime soundTime = startTime;

    //Get the CanAttack on which the weapon is mounted
	MachPhysCanAttack* pCanAttack = NULL;
	if(hasMachine() && machine().hasCanAttack())
		pCanAttack = machine().canAttack();
    else if( hasConstruction() )
        pCanAttack = construction().canAttack();

    ASSERT( pCanAttack, "Weapon not mounted on a CanAttack" );

	//Only play the sounds if they haven't played recently
	if(!pCanAttack->hasPlayedSoundWithin(type(), 0.95*duration, startTime))
	{
	    W4dSoundManager::instance().play( this, sndData,
                                 PhysAbsoluteTime( 0 ), 1 );
		pCanAttack->cacheSoundPlay(type(), startTime);
	}
//    for( uint i = 0; i != nRounds; ++i )
//    {
//	    W4dSoundManager::instance().playForDuration( this, sndData,
//                                      PhysAbsoluteTime( 0 ),
//                                      5.2 );
////        W4dSoundManager::instance().play( this, wavePath, soundTime,
////                                          100.0, 100.0, W4dSoundManager::PLAY_ONCE );
//
//        soundTime += interval;
//    }
}

void perWrite( PerOstream& ostr, const MachPhysBolter& bolter )
{
    const MachPhysWeapon& base = bolter;

    ostr << base;
    ostr << bolter.pTurnLink_;
}

void perRead( PerIstream& istr, MachPhysBolter& bolter )
{
    MachPhysWeapon& base = bolter;

    istr >> base;
    istr >> bolter.pTurnLink_;
}

//static
PhysRelativeTime MachPhysBolter::applyVictimAnimation
(
    const PhysAbsoluteTime& startTime, const MexLine3d& fromDirection,
    MachPhysArtefact* pArtefact
)
{
    PRE( fromDirection.length() != 0.0 );

    //Apply sparks at the intersection between the direction line, and the impact point.
    MexVec3 unitDirection;
    MexPoint3d impactPoint;
    W4dEntity& physArtefact = pArtefact->physEntity();

    if( MachPhysLinearProjectile::impactData( physArtefact, fromDirection, &impactPoint, &unitDirection ) )
    {
        //Apply sparks to the impact point
        impactSparks( startTime, &physArtefact, impactPoint, unitDirection );
    }

    return 0.0;
}

//static
W4dGeneric* MachPhysBolter::newFlash( MachPhys::WeaponType type )
{
    W4dGeneric* pFlash =
        _NEW( W4dGeneric( MachPhysWeaponPersistence::instance().pRoot(), MexTransform3d(), W4dEntity::NOT_SOLID ) );

	// The current model is all emissive.  Hence, it should not need
	// lighting.  This could change if the model changes.
	pFlash->doNotLight(true);

	switch( type )
	{
        case MachPhys::BOLTER:
        {
        	pFlash->loadLODFile( SysPathName( "models/weapons/flash/bolter.lod" ) );
            break;
        }
        case MachPhys::HEAVY_BOLTER1:
        case MachPhys::HEAVY_BOLTER2:
        {
        	pFlash->loadLODFile( SysPathName( "models/weapons/flash/heavy.lod" ) );
            break;
        }
        case MachPhys::AUTO_CANNON:
        {
        	pFlash->loadLODFile( SysPathName( "models/weapons/flash/auto.lod" ) );
            break;
		}
	}

    return pFlash;
}

/* End BOLTER.CPP ****************************************************/
