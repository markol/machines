/*
 * E L E C H A R G . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/elecharg.hpp"
#include "machphys/snddata.hpp"
#include "render/texture.hpp"
#include "machphys/electro.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/private/weapper.hpp"
#include "machphys/effects.hpp"
#include "machphys/smkcloud.hpp"
#include "machphys/random.hpp"
#include "machphys/machine.hpp"
#include "machphys/constron.hpp"
#include "machphys/halo.hpp"
#include "machphys/halompln.hpp"
#include "machphys/flash.hpp"
#include "machphys/artefact.hpp"
#include "machphys/private/weapper.hpp"
#include "machphys/snddata.hpp"
#include "machphys/mphydata.hpp"

#include "world4d/root.hpp"
#include "world4d/entity.hpp"
#include "world4d/link.hpp"
#include "world4d/objtrack.hpp"
#include "world4d/garbcoll.hpp"
#include "world4d/gusplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/soundman.hpp"

#include "render/scale.hpp"
#include "render/colour.hpp"

#include "phys/timespin.hpp"
#include "phys/motplan.hpp"
#include "phys/linemoti.hpp"

#include "mathex/abox3d.hpp"
#include "mathex/point3d.hpp"

#include "system/pathname.hpp"

PER_DEFINE_PERSISTENT( MachPhysElectroCharger );

//public ctor
MachPhysElectroCharger::MachPhysElectroCharger
(
    W4dEntity* pParent, const MexTransform3d& localTransform,
    MachPhys::Mounting mounting
)
:   MachPhysLinearWeapon( exemplar(mounting), mounting, pParent, localTransform )
{

    TEST_INVARIANT;
}

//One-time ctor
MachPhysElectroCharger::MachPhysElectroCharger(MachPhys::Mounting mounting)
:   MachPhysLinearWeapon( MachPhysWeaponPersistence::instance().pRoot(), MexTransform3d(),
                    SysPathName( compositeFilePath() ), MachPhys::ELECTRIC_CHARGE, mounting )
{
    TEST_INVARIANT;
}

MachPhysElectroCharger::MachPhysElectroCharger( PerConstructor con )
: MachPhysLinearWeapon( con )
{
}

MachPhysElectroCharger::~MachPhysElectroCharger()
{
    TEST_INVARIANT;

}

//static
const MachPhysElectroCharger& MachPhysElectroCharger::exemplar( MachPhys::Mounting mounting )
{
    return MachPhysWeaponPersistence::instance().electroChargerExemplar( mounting );
}

void MachPhysElectroCharger::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysElectroCharger& t )
{

    o << "MachPhysElectroCharger " << (void*)&t << " start" << std::endl;
    o << "MachPhysElectroCharger " << (void*)&t << " end" << std::endl;

    return o;
}

//static
const char* MachPhysElectroCharger::compositeFilePath()
{
	return "models/weapons/elecch/elecch.cdf";
}


//virtual
MachPhysLinearProjectile* MachPhysElectroCharger::createProjectile
(
    const PhysAbsoluteTime& burstStartTime, uint, W4dEntity* pParent,
    const W4dEntity& target, const MexPoint3d& targetOffset
)
{
	PhysRelativeTime duration = weaponData().extras()[0];
	MachPhysElectro* pElectro = createElectro(burstStartTime, duration, pParent, target, targetOffset);
	MachPhysLinearProjectile* pProjectile = pElectro;

	return pProjectile;
}

//virtual
PhysRelativeTime MachPhysElectroCharger::fire( const PhysAbsoluteTime& startTime, int )
{
    //get weapon data
    const MachPhysWeaponData& data = weaponData();
    MexPoint3d smokeOffset = data.launchOffsets()[0];
	MATHEX_SCALAR range = data.range();
    PhysRelativeTime burstDuration = data.burstDuration();

    //Add the exhaust WHITE smoke cloud
    MachPhysSmokeCloud* pSmokeCloud = _NEW( MachPhysSmokeCloud ( this, smokeOffset ) );
    PhysAbsoluteTime endTime = startTime + burstDuration;
    uint nWisps = 3;
    MexPoint3d endPosition( 0.0,0.0,2.0 );
    MexVec3 randomOffset( 0.1,0.1,0.0 );
    PhysRelativeTime wispInterval = 0.08;
    MATHEX_SCALAR wispSize = 1.5;
    MATHEX_SCALAR zDepthOffset = -1.0;
    PhysRelativeTime smokeDuration =
        pSmokeCloud->startSmoking( nWisps, endPosition, randomOffset, endTime,
                                   wispInterval, wispSize, zDepthOffset,
                                   MachPhysEffects::smokeMaterialPlan( MachPhysEffects::WHITE ) );

	MATHEX_SCALAR flashSize = data.extras()[2];
    MachPhysFlashDisc *pFlashDisc = _NEW(MachPhysFlashDisc( this, smokeOffset, flashSize, MachPhysFlashDisc::WHITE) );
	PhysRelativeTime flashDuration = pFlashDisc->flash(startTime, MachPhysFlashDisc::WHITE);

	//lighting up the machine and the weapon
	lighting(RenColour( 82.0/255.0, 220.0/255.0, 1. ), startTime, 4);

    //Garbage collect the  WHITE  puff and the flash
    W4dGarbageCollector::instance().add( pSmokeCloud, endTime + smokeDuration );
    W4dGarbageCollector::instance().add( pFlashDisc, startTime + flashDuration );

    //Play sound
    W4dSoundManager::instance().play( this, SID_ELECTRICCHARGE,
                                      startTime, 1 );
//    W4dSoundManager::instance().play( this, SysPathName( "sounds/electchg.wav" ), startTime,
//                                      100.0, 100.0, W4dSoundManager::PLAY_ONCE );

    return burstDuration;
}

MachPhysElectro* MachPhysElectroCharger::createElectro(const PhysAbsoluteTime& burstTime,
								   const PhysRelativeTime& duration,
								   W4dEntity* pParent,
								   const W4dEntity& target,
								   const MexPoint3d& targetOffset )
{
    MexTransform3d startTransform;
    MATHEX_SCALAR distance;
	PhysAbsoluteTime launchTime = launchData(burstTime, 0, pParent, target, targetOffset, &startTransform, &distance);

	MachPhysElectro* pElectro = _NEW(MachPhysElectro(pParent, startTransform, MachPhysElectro::ELECTRIC) );

 	//@100%, the electro size = 5
	MATHEX_SCALAR topScale = distance/5;

	pElectro->scaleAndSpin(burstTime, duration,	topScale, startTransform);

	//The purple smoke
    MexPoint3d smokeOffset = MexPoint3d(0.0, 0.0, 0.0);
    PhysRelativeTime burstDuration = weaponData().burstDuration();

    uint nWisps = 1;
    MexPoint3d endPosition( 0.0, 0.0, 0.0 );
    MexVec3   randomOffset( 0.0, 0.0, 0.0 );
    PhysRelativeTime wispInterval = 0.0;
    MATHEX_SCALAR wispSize = 5.5;
    MATHEX_SCALAR zDepthOffset = -1.0;

	MATHEX_SCALAR seg = distance/3.0;

    MachPhysSmokeCloud* pPurpleCloud1 = _NEW( MachPhysSmokeCloud ( pElectro, smokeOffset ) );

	smokeOffset = MexPoint3d(smokeOffset.x() + seg, smokeOffset.y(), smokeOffset.z());
    MachPhysSmokeCloud* pPurpleCloud2 = _NEW( MachPhysSmokeCloud ( pElectro, smokeOffset ) );

	smokeOffset = MexPoint3d(smokeOffset.x() + seg, smokeOffset.y(), smokeOffset.z());
    MachPhysSmokeCloud* pPurpleCloud3 = _NEW( MachPhysSmokeCloud ( pElectro, smokeOffset ) );

	smokeOffset = MexPoint3d(smokeOffset.x() + seg, smokeOffset.y(), smokeOffset.z());
    MachPhysSmokeCloud* pPurpleCloud4 = _NEW( MachPhysSmokeCloud ( pElectro, smokeOffset ) );

    PhysRelativeTime purpleDuration =
        pPurpleCloud1->startSmoking( nWisps, endPosition, randomOffset, burstTime,
                                   wispInterval, wispSize, zDepthOffset,
                                   MachPhysEffects::smokeMaterialPlan( MachPhysEffects::PURPLE ) );
	purpleDuration =
        pPurpleCloud2->startSmoking( nWisps, endPosition, randomOffset, burstTime,
                                   wispInterval, wispSize, zDepthOffset,
                                   MachPhysEffects::smokeMaterialPlan( MachPhysEffects::PURPLE ) );
	purpleDuration =
        pPurpleCloud3->startSmoking( nWisps, endPosition, randomOffset, burstTime,
                                   wispInterval, wispSize, zDepthOffset,
                                   MachPhysEffects::smokeMaterialPlan( MachPhysEffects::PURPLE ) );
	purpleDuration =
        pPurpleCloud4->startSmoking( nWisps, endPosition, randomOffset, burstTime,
                                   wispInterval, wispSize, zDepthOffset,
                                   MachPhysEffects::smokeMaterialPlan( MachPhysEffects::PURPLE ) );

    W4dGarbageCollector::instance().add( pPurpleCloud1, burstTime + purpleDuration );
    W4dGarbageCollector::instance().add( pPurpleCloud2, burstTime + purpleDuration );
    W4dGarbageCollector::instance().add( pPurpleCloud3, burstTime + purpleDuration );
    W4dGarbageCollector::instance().add( pPurpleCloud4, burstTime + purpleDuration );

	return pElectro;
 }

//virtual
PhysRelativeTime MachPhysElectroCharger::victimAnimation
(
    const PhysAbsoluteTime& startTime, const MexLine3d& line, MachPhysMachine* pMachine
) const
{
	return applyVictimAnimation( startTime, line, pMachine );
}

//virtual
PhysRelativeTime MachPhysElectroCharger::victimAnimation
(
    const PhysAbsoluteTime& startTime, const MexLine3d& line, MachPhysConstruction* pConstruction
) const
{
	return applyVictimAnimation( startTime, line, pConstruction );
}


//static
PhysRelativeTime MachPhysElectroCharger::applyVictimAnimation
(
    const PhysAbsoluteTime& startTime,
    const MexLine3d&,
    MachPhysConstruction* pConstruction
)
{
    PhysRelativeTime haloLightningTime = 0.0;

    //Check the girth is less than 12m before applying the electric halo effect
    const MexAlignedBox3d& boundary = pConstruction->compositeBoundingVolume();
    if( (boundary.xLength() + boundary.yLength()) < 12.0 )
    {
	    static const MachPhysWeaponData& data = MachPhysData::instance().weaponData( MachPhys::ELECTRIC_CHARGE );
    	PhysRelativeTime duration = data.extras()[1];

    	//halo lightning on the victim
    	haloLightningTime = startLightning(startTime, duration, pConstruction );
    }

	return haloLightningTime;
}

//static
PhysRelativeTime MachPhysElectroCharger::applyVictimAnimation
(
    const PhysAbsoluteTime& startTime,
    const MexLine3d&,
    MachPhysMachine* pMachine
)
{
	static const MachPhysWeaponData& data = MachPhysData::instance().weaponData( MachPhys::ELECTRIC_CHARGE );
	PhysRelativeTime duration = data.extras()[1];

	//halo lightning on the victim
	PhysRelativeTime haloLightningTime = startLightning(startTime, duration, pMachine );

	//If the machine has a faceplate, make some black smoke come from it
    PhysRelativeTime smokeDuration = 0.0;
	if( pMachine->hasFacePlate() )
	{
		W4dLink* pFacePlate = &pMachine->facePlate();
		const MexAlignedBox3d& victimBox3D = pFacePlate->boundingVolume();

        const MexPoint3d& minCorner = victimBox3D.minCorner();
        const MexPoint3d& maxCorner = victimBox3D.maxCorner();

        MATHEX_SCALAR wispSize = std::max((maxCorner.x() - minCorner.x()), (maxCorner.y() - minCorner.y()) );
					  wispSize *= 4.0;

    	MexPoint3d smokeOffset( 0.5*(minCorner.x() + maxCorner.x()),
    	                        0.5*(minCorner.y() + maxCorner.y()),
    	                        maxCorner.z() );

        //Attach the smoke cloud to the face plate. SInce this is a link, we need to establish a holding
        //relationship to get it drawn.
        MachPhysSmokeCloud* pSmokeCloud = _NEW( MachPhysSmokeCloud ( pFacePlate, smokeOffset ) );
        pMachine->hold( pSmokeCloud, pFacePlate, pFacePlate->localTransform() );

        uint nWisps = 3;
        MexPoint3d endPosition( 0.0, 0.0, 0.0 );
        MexVec3 randomOffset( 0.05,0.05,0.0 );
        PhysRelativeTime wispInterval = 0.1;
        MATHEX_SCALAR zDepthOffset = 0;
        smokeDuration =
            pSmokeCloud->startSmoking( nWisps, endPosition, randomOffset, startTime + haloLightningTime,
                                       wispInterval, wispSize, zDepthOffset,
                                       MachPhysEffects::smokeMaterialPlan( MachPhysEffects::BLACK ) );

        //Garbage collect the  smoke cloud
        W4dGarbageCollector::instance().add( pSmokeCloud, startTime + smokeDuration );
    }

	//Do a shake animation
	PhysRelativeTime headShakeTime = pMachine->shake( startTime );

	return std::max(haloLightningTime+smokeDuration, headShakeTime);
}

//static
PhysRelativeTime MachPhysElectroCharger::startLightning
(
    const PhysAbsoluteTime& startTime,
    const PhysRelativeTime& duration,
    W4dEntity* pVictim
)
{
	const MexAlignedBox3d& victimBox3D =
	    (pVictim->isComposite() ? pVictim->asComposite().compositeBoundingVolume()
                                : pVictim->boundingVolume());
    const MexPoint3d& minCorner = victimBox3D.minCorner();
    const MexPoint3d& maxCorner = victimBox3D.maxCorner();
	MexPoint3d centre(0.5*(minCorner.x() + maxCorner.x()),
	                  0.5*(minCorner.y() + maxCorner.y()),
	                  0.5*(minCorner.z() + maxCorner.z()) );

	MexTransform3d localXform = MexTransform3d(centre);

	//lightning effect on the victim
	MachPhysLightningHalo* pHalo1 = _NEW(MachPhysLightningHalo(pVictim, localXform, MachPhysLightningHalo::ELECTRIC) );
	MachPhysLightningHalo* pHalo2 = _NEW(MachPhysLightningHalo(pVictim, localXform, MachPhysLightningHalo::ELECTRIC) );

	pHalo1->startLightning(pVictim, startTime, duration, -1, MachPhysLightningHalo::ELECTRIC);
	pHalo2->startLightning(pVictim, startTime, duration,  1, MachPhysLightningHalo::ELECTRIC);

    //	garbage collect the haloes
    W4dGarbageCollector::instance().add( pHalo1, startTime + duration );
    W4dGarbageCollector::instance().add( pHalo2, startTime + duration );

	return duration;
}

void perWrite( PerOstream& ostr, const MachPhysElectroCharger& weapon )
{
    const MachPhysLinearWeapon& base = weapon;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysElectroCharger& weapon )
{
    MachPhysLinearWeapon& base = weapon;

    istr >> base;
}

//static
PhysRelativeTime MachPhysElectroCharger::applyVictimAnimation
(
    const PhysAbsoluteTime& startTime,
    const MexLine3d&,
    MachPhysArtefact* pArtefact
)
{
    PhysRelativeTime haloLightningTime = 0.0;

    //Check the girth is less than 12m before applying the electric halo effect
    W4dEntity* pPhysArtefact = &(pArtefact->physEntity());
    const MexAlignedBox3d& boundary =
        (pPhysArtefact->isComposite() ? pPhysArtefact->asComposite().compositeBoundingVolume()
                                      : pPhysArtefact->boundingVolume());

    if( (boundary.xLength() + boundary.yLength()) < 12.0 )
    {
	    static const MachPhysWeaponData& data = MachPhysData::instance().weaponData( MachPhys::ELECTRIC_CHARGE );
    	PhysRelativeTime duration = data.extras()[1];

    	//halo lightning on the victim
    	haloLightningTime = startLightning(startTime, duration, pPhysArtefact );
    }

	return haloLightningTime;
}

/* End FLAMETHR.CPP ****************************************************/
