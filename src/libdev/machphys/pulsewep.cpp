/*
 * P U L S E W E P . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "render/texture.hpp"
#include "machphys/pulsewep.hpp"
#include "machphys/effects.hpp"
#include "machphys/constron.hpp"
#include "machphys/machine.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/radidisc.hpp"
#include "machphys/pulseblb.hpp"
#include "machphys/artefact.hpp"
#include "machphys/private/weapper.hpp"
#include "machphys/snddata.hpp"
#include "machphys/attack.hpp"

#include "world4d/root.hpp"
#include "world4d/soundman.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/matplan.hpp"
#include "world4d/garbcoll.hpp"
#include "world4d/simplsca.hpp"
#include "world4d/alphsimp.hpp"
#include "world4d/light.hpp"
#include "world4d/compplan.hpp"

#include "render/colour.hpp"
#include "render/material.hpp"
#include "phys/linemoti.hpp"
#include "phys/rampacce.hpp"
#include "phys/asclplan.hpp"
#include "system/pathname.hpp"

#ifndef _INLINE
    #include "machphys/pulsewep.ipp"
#endif

PER_DEFINE_PERSISTENT( MachPhysPulseWeapon );
PER_DEFINE_PERSISTENT( MachPhysPulseRifle );
PER_DEFINE_PERSISTENT( MachPhysPulseCannon );

static void lightingUpVictim( W4dEntity* pVictim, const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration );

///////////////////////////////////////////////////////////////////////

MachPhysPulseRifle::MachPhysPulseRifle
(
    W4dEntity* pParent, const MexTransform3d& localTransform, MachPhys::Mounting mounting
)
:   MachPhysPulseWeapon( exemplar(), mounting, pParent, localTransform )
{
    TEST_INVARIANT;
}

MachPhysPulseRifle::MachPhysPulseRifle()
:   MachPhysPulseWeapon( SysPathName( "models/weapons/pulser/pulsr.cdf" ), MachPhys::PULSE_RIFLE )
{
    TEST_INVARIANT;
}

MachPhysPulseRifle::MachPhysPulseRifle( PerConstructor con )
: MachPhysPulseWeapon( con )
{
}

MachPhysPulseRifle::~MachPhysPulseRifle()
{
    TEST_INVARIANT;
}

//static
const MachPhysPulseRifle& MachPhysPulseRifle::exemplar()
{
    return MachPhysWeaponPersistence::instance().pulseRifleExemplar();
}

void MachPhysPulseRifle::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysPulseRifle& t )
{

    o << "MachPhysPulseRifle " << (void*)&t << " start" << std::endl;
    o << "MachPhysPulseRifle " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachPhysPulseRifle& weapon )
{
    const MachPhysPulseWeapon& base = weapon;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysPulseRifle& weapon )
{
    MachPhysPulseWeapon& base = weapon;

    istr >> base;
}

///////////////////////////////////////////////////////////////////////

MachPhysPulseCannon::MachPhysPulseCannon
(
    W4dEntity* pParent, const MexTransform3d& localTransform, MachPhys::Mounting mounting
)
:   MachPhysPulseWeapon( exemplar(), mounting, pParent, localTransform )
{
    TEST_INVARIANT;
}

MachPhysPulseCannon::MachPhysPulseCannon()
:   MachPhysPulseWeapon( SysPathName( "models/weapons/pulsec/pulsec.cdf" ), MachPhys::PULSE_CANNON )
{
    TEST_INVARIANT;
}

MachPhysPulseCannon::MachPhysPulseCannon( PerConstructor con )
: MachPhysPulseWeapon( con )
{
}

MachPhysPulseCannon::~MachPhysPulseCannon()
{
    TEST_INVARIANT;
}

//static
const MachPhysPulseCannon& MachPhysPulseCannon::exemplar()
{
    return MachPhysWeaponPersistence::instance().pulseCannonExemplar();
}

void MachPhysPulseCannon::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysPulseCannon& t )
{

    o << "MachPhysPulseCannon " << (void*)&t << " start" << std::endl;
    o << "MachPhysPulseCannon " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachPhysPulseCannon& weapon )
{
    const MachPhysPulseWeapon& base = weapon;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysPulseCannon& weapon )
{
    MachPhysPulseWeapon& base = weapon;

    istr >> base;
}

///////////////////////////////////////////////////////////////////////

MachPhysPulseWeapon::MachPhysPulseWeapon
(
    const MachPhysPulseWeapon& copyMe, MachPhys::Mounting mounting,
    W4dEntity* pParent, const MexTransform3d& localTransform

)
:   MachPhysLinearWeapon( copyMe, mounting, pParent, localTransform )
{
    TEST_INVARIANT;
}

MachPhysPulseWeapon::MachPhysPulseWeapon
(
    const SysPathName& compositeFileName, MachPhys::WeaponType type
)
:   MachPhysLinearWeapon(  MachPhysWeaponPersistence::instance().pRoot(), MexTransform3d(), compositeFileName,
                     type, MachPhys::LEFT )
{
}

MachPhysPulseWeapon::MachPhysPulseWeapon( PerConstructor con )
: MachPhysLinearWeapon( con )
{
}

MachPhysPulseWeapon::~MachPhysPulseWeapon()
{
    TEST_INVARIANT;
}

void MachPhysPulseWeapon::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysPulseWeapon& t )
{

    o << "MachPhysPulseWeapon " << (void*)&t << " start" << std::endl;
    o << "MachPhysPulseWeapon " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
PhysRelativeTime MachPhysPulseWeapon::victimAnimation
(
    const PhysAbsoluteTime& startTime, const MexLine3d& fromDirection, MachPhysMachine* pMachine
) const
{
    return applyVictimAnimation( startTime, fromDirection, pMachine );
}

//virtual
PhysRelativeTime MachPhysPulseWeapon::victimAnimation
(
    const PhysAbsoluteTime& startTime, const MexLine3d& fromDirection, MachPhysConstruction* pConstruction
) const
{
    return applyVictimAnimation( startTime, fromDirection, pConstruction );
}

//static
PhysRelativeTime MachPhysPulseWeapon::applyVictimAnimation
(
    const PhysAbsoluteTime& startTime, const MexLine3d&, MachPhysMachine* pMachine
)
{
    //Make the victim flash briefly white
    PhysRelativeTime duration = 0.1;

	lightingUpVictim( pMachine, startTime, duration );

/*
    MachPhysEffects::flashObject
    (
        pMachine, MachPhysEffects::solidWhiteMaterial(), startTime, duration, W4dLOD( 2 )
    );

	W4dUniformLight* pLight = _NEW( W4dUniformLight(pMachine, MexVec3(1, 0, 0), 30));

	pLight->colour( RenColour( 3, 3, 3) );
	pLight->constantAttenuation(0);
	pLight->linearAttenuation(0.7);
	pLight->quadraticAttenuation(0.3);
	pLight->scope(W4dLight::LOCAL);
	pLight->visible(false);
 	pLight->illuminate(pMachine);

    W4dVisibilityPlanPtr lightVisibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	lightVisibilityPlanPtr->add(false, duration);

	pLight->entityPlanForEdit().visibilityPlan(lightVisibilityPlanPtr, startTime );

    W4dGarbageCollector::instance().add( pLight, startTime+duration );
*/
    //Play the appropriate sound
	W4dSoundManager::instance().play( pMachine, SID_PUCHIT, startTime, 1 );
//    W4dSoundManager::instance().play( pMachine, SysPathName( "sounds/puchit.wav" ), startTime,
//                                      100.0, 100.0, W4dSoundManager::PLAY_ONCE );

    return duration;
}

//static
PhysRelativeTime MachPhysPulseWeapon::applyVictimAnimation
(
    const PhysAbsoluteTime& startTime, const MexLine3d&, MachPhysConstruction* pConstruction
)
{
    //Make the victim flash briefly white
    PhysRelativeTime duration = 0.1;
	lightingUpVictim( pConstruction, startTime, duration );

/*
    MachPhysEffects::flashObject
    (
        pConstruction, MachPhysEffects::solidWhiteMaterial(), startTime, duration, W4dLOD( 2 )
    );

	//a strong unuiform light generated at he point of impact
	W4dUniformLight* pLight = _NEW( W4dUniformLight(pConstruction, MexVec3(1, 0, 0), 30));

	pLight->colour( RenColour( 3, 3, 3) );
	pLight->constantAttenuation(0);
	pLight->linearAttenuation(0.7);
	pLight->quadraticAttenuation(0.3);
	pLight->scope(W4dLight::LOCAL);
	pLight->visible(false);
 	pLight->illuminate(pConstruction);

    W4dVisibilityPlanPtr lightVisibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	lightVisibilityPlanPtr->add(false, duration);

	pLight->entityPlanForEdit().visibilityPlan(lightVisibilityPlanPtr, startTime );

    W4dGarbageCollector::instance().add( pLight, startTime+duration );
*/

    //Play the appropriate sound
	W4dSoundManager::instance().play( pConstruction, SID_PUCHIT, startTime, 1 );
//    W4dSoundManager::instance().play( pConstruction, SysPathName( "sounds/puchit.wav" ), startTime,
//                                      100.0, 100.0, W4dSoundManager::PLAY_ONCE );

    return duration;
}

//virtual
PhysRelativeTime MachPhysPulseWeapon::fire( const PhysAbsoluteTime& startTime, int numberInBurst )
{
	// The lights attached to the projectile may be sufficient to light up the
	// launcher.  Perhaps this isn't needed?  If it is still needed, we should
	// checked the firedRecently method to avoid multiple lights.
	//lighting up the machine and the weapon
	MATHEX_SCALAR amplitude = weaponData().extras()[0];
	lighting(RenColour(amplitude*42.0/255.0, amplitude*97.0/255.0, amplitude*209.0/255.0), startTime, 3);

    //Launch the radial discs
    createFiringDiscs( startTime );

    //Run the recoil animation
    PhysRelativeTime duration = recoil( startTime, numberInBurst );

    return duration;
}

void MachPhysPulseWeapon::createFiringDiscs( const PhysAbsoluteTime& startTime )
{
    //Get the necessary data from the weapon data object
    const MachPhysWeaponData& data = weaponData();
    uint nRoundsPerBurst = data.nRoundsPerBurst();
    PhysRelativeTime burstDuration = data.burstDuration();
    const MexPoint3d& launchOffset = data.launchOffsets()[0];

    //Derive the local transform for the disc
    MexTransform3d discOffset( launchOffset );

    //Set up timing
    PhysAbsoluteTime discStartTime = startTime;
    PhysRelativeTime interval =
        (nRoundsPerBurst == 1 ? 0.0 : burstDuration / (nRoundsPerBurst - 1));

    while( nRoundsPerBurst-- )
    {
        //Create the disc
        createPulseDisc( this, discOffset, discStartTime );

        discStartTime += interval;
    }
}

//static
PhysRelativeTime MachPhysPulseWeapon::createPulseDisc
(
    W4dEntity* pParent, const MexTransform3d& localPosition, const PhysAbsoluteTime& startTime
)
{
    //Counted pointers etc for standard plans
    static W4dVisibilityPlanPtr visibilityPlanPtr;
    static W4dScalePlanPtr scalePlanPtr;
    static RenMaterial discMaterial;
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

        //Set the disc material
        discMaterial.diffuse( RenColour::black() );
        discMaterial.emissive( RenColour( 95.0/255.0, 174.0/255.0, 255.0/255.0 ) );
        discMaterial.texture( MachPhysRadialDisc::circleTexture() );

        //Create the alpha scalar plan
        PhysRampAcceleration ramp( 0.0, duration, 1.0, -0.49, -0.1, -0.1,
                                   PhysRampAcceleration::BY_DISTANCE );
        PhysAcceleratedScalarPlan::RampAccelerations accs;
        accs.reserve( 1 );
        accs.push_back( ramp );
        PhysAcceleratedScalarPlan* pAlphaValuePlan =
            _NEW( PhysAcceleratedScalarPlan(accs, 0.5 ) );
        alphaValuePlanPtr = pAlphaValuePlan;
    }

    //Construct a radial disc at the specified location
    MachPhysRadialDisc* pDisc = _NEW( MachPhysRadialDisc( pParent, localPosition ) );

    //Make invisible till needed
    pDisc->visible( false );

    //Set its visibility plan
    W4dEntityPlan& entityPlan = pDisc->entityPlanForEdit();
    entityPlan.visibilityPlan( visibilityPlanPtr, startTime );

    //Set the scaling plan
    entityPlan.scalePlan( scalePlanPtr, startTime );

    //Set up the alpha plan. We need a fresh copy for each disc because it
    //uses a cached material.
    W4dSimpleAlphaPlan* pAlphaPlan =
        _NEW( W4dSimpleAlphaPlan( discMaterial, 1, alphaValuePlanPtr, 2 ) );
    W4dMaterialPlanPtr alphaPlanPtr( pAlphaPlan );
    entityPlan.materialPlan( alphaPlanPtr, startTime );

    //Add to the garbage collection list
    W4dGarbageCollector::instance().add( pDisc, startTime + duration );

    return duration;
}

//virtual
MachPhysLinearProjectile* MachPhysPulseWeapon::createProjectile
(
    const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
    const W4dEntity& target, const MexPoint3d& targetOffset
)
{
    return createPulseBlob( burstStartTime, index, pParent, target, targetOffset );
}

MachPhysPulseBlob* MachPhysPulseWeapon::createPulseBlob
(
    const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
    const W4dEntity& target, const MexPoint3d& targetOffset
)
{
    //Use utility function to get launch time for the projectile and its
    //start position relative to pParent.
    MexTransform3d startTransform;
	MATHEX_SCALAR distance;
    PhysAbsoluteTime launchTime =
        launchData( burstStartTime, index, pParent, target, targetOffset, &startTransform, &distance );

	// Deterimine if the owning machine has fired any of it's
	// multi-weapons recently.
	MachPhysCanAttack* pCanAttack = NULL;
	if(hasMachine() && machine().hasCanAttack())
		pCanAttack = machine().canAttack();
    else if( hasConstruction() )
        pCanAttack = construction().canAttack();

    ASSERT( pCanAttack, "Weapon not mounted on a CanAttack" );

	if (!playedSoundRecently(launchTime))
	{
		pCanAttack->cacheSoundPlay(type(), launchTime);
		SoundId sndData = (type() == MachPhys::PULSE_RIFLE)? SID_PULSERIFLE: SID_PULSECANNON;
		W4dSoundManager::instance().play( this, sndData, launchTime, 1 );
	}

	MachPhysPulseBlob::CreateLights createLights = MachPhysPulseBlob::DONT_CREATE_LIGHTS;
	if (!firedLightRecently(launchTime))
	{
		pCanAttack->cacheLightLaunch(type(), launchTime);
		createLights = MachPhysPulseBlob::CREATE_LIGHTS;
	}

    //Create the blob
    MachPhysPulseBlob* pBlob = _NEW(MachPhysPulseBlob(pParent, startTransform, type(), createLights));

	//let it spin if can
	W4dCompositePlanPtr spinPlanPtr;

	if( pBlob->findCompositePlan( "spin", &spinPlanPtr ) )
	{
        pBlob->plan( *spinPlanPtr, launchTime, 1000000 );
	}

    //Make it fly
    pBlob->move( launchTime, weaponData() );

    return pBlob;
}

void perWrite( PerOstream& ostr, const MachPhysPulseWeapon& weapon )
{
    const MachPhysLinearWeapon& base = weapon;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysPulseWeapon& weapon )
{
    MachPhysLinearWeapon& base = weapon;

    istr >> base;
}

//static
PhysRelativeTime MachPhysPulseWeapon::applyVictimAnimation
(
    const PhysAbsoluteTime& startTime, const MexLine3d&, MachPhysArtefact* pArtefact
)
{
    //Make the victim flash briefly white
    W4dEntity* pVictim = &pArtefact->physEntity();

    PhysRelativeTime duration = 0.1;
	lightingUpVictim( pVictim, startTime, duration);
/*
    MachPhysEffects::flashObject
    (
        pVictim, MachPhysEffects::solidWhiteMaterial(), startTime, duration, W4dLOD( 2 )
    );

	W4dUniformLight* pLight = _NEW( W4dUniformLight(pVictim, MexVec3(1, 0, 0), 30) );

	pLight->colour( RenColour( 3, 3, 3) );
	pLight->constantAttenuation(0);
	pLight->linearAttenuation(0.7);
	pLight->quadraticAttenuation(0.3);
	pLight->scope(W4dLight::LOCAL);
	pLight->visible(false);
 	pLight->illuminate(pVictim);

    W4dVisibilityPlanPtr lightVisibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	lightVisibilityPlanPtr->add(false, duration);

	pLight->entityPlanForEdit().visibilityPlan(lightVisibilityPlanPtr, startTime );

    W4dGarbageCollector::instance().add( pLight, startTime+duration );
*/
    //Play the appropriate sound
	W4dSoundManager::instance().play( pVictim, SID_PUCHIT, startTime, 1 );
//    W4dSoundManager::instance().play( pVictim, SysPathName( "sounds/puchit.wav" ), startTime,
//                                      100.0, 100.0, W4dSoundManager::PLAY_ONCE );

    return duration;
}

//local function
static void lightingUpVictim( W4dEntity* pVictim, const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration )
{
	W4dUniformLight* pLight = _NEW( W4dUniformLight(pVictim, MexVec3(1, 0, 0), 30) );

	pLight->colour( RenColour( 3, 3, 3) );
	pLight->constantAttenuation(0);
	pLight->linearAttenuation(0.7);
	pLight->quadraticAttenuation(0.3);
	pLight->scope(W4dLight::LOCAL);
	pLight->visible(false);
 	pLight->illuminate(pVictim);

    W4dVisibilityPlanPtr lightVisibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	lightVisibilityPlanPtr->add(false, duration);

	pLight->entityPlanForEdit().visibilityPlan(lightVisibilityPlanPtr, startTime );

    W4dGarbageCollector::instance().add( pLight, startTime+duration );
}
///////////////////////////////////////////////////////////////////////
/* End PULSEWEP.CPP *************************************************/
