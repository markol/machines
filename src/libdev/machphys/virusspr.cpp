/*
 * V I R U S S P R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/virusspr.hpp"
#include "machphys/private/weapper.hpp"

#include "render/texture.hpp"
#include "render/scale.hpp"
#include "render/colour.hpp"

#include "machphys/electro.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/effects.hpp"
#include "machphys/smkcloud.hpp"
#include "machphys/random.hpp"
#include "machphys/machine.hpp"
#include "machphys/constron.hpp"
#include "machphys/halo.hpp"
#include "machphys/halompln.hpp"
#include "machphys/flash.hpp"
#include "machphys/snddata.hpp"
#include "machphys/mphydata.hpp"
//#include "machphys/private/weapper.hpp"

#include "world4d/root.hpp"
#include "world4d/entity.hpp"
#include "world4d/objtrack.hpp"
#include "world4d/garbcoll.hpp"
#include "world4d/soundman.hpp"

#include "phys/timespin.hpp"
#include "phys/motplan.hpp"
#include "phys/linemoti.hpp"

#include "mathex/abox3d.hpp"
#include "mathex/point3d.hpp"

#include "system/pathname.hpp"

#include "sim/manager.hpp"

PER_DEFINE_PERSISTENT( MachPhysVirusSpreader );

//public ctor
MachPhysVirusSpreader::MachPhysVirusSpreader
(
    W4dEntity* pParent, const MexTransform3d& localTransform,
    MachPhys::Mounting mounting
)
:   MachPhysLinearWeapon( exemplar(mounting), mounting, pParent, localTransform )
{

    TEST_INVARIANT;
}

//One-time ctor
MachPhysVirusSpreader::MachPhysVirusSpreader(MachPhys::Mounting mounting)
:   MachPhysLinearWeapon( MachPhysWeaponPersistence::instance().pRoot(), MexTransform3d(),
                    SysPathName( compositeFilePath() ), MachPhys::VIRUS, mounting )
{
    TEST_INVARIANT;
}

MachPhysVirusSpreader::MachPhysVirusSpreader( PerConstructor con )
: MachPhysLinearWeapon( con )
{
}

MachPhysVirusSpreader::~MachPhysVirusSpreader()
{
    TEST_INVARIANT;

}

//static
const MachPhysVirusSpreader& MachPhysVirusSpreader::exemplar( MachPhys::Mounting mounting )
{
    return MachPhysWeaponPersistence::instance().virusSpreaderExemplar( mounting );
}

void MachPhysVirusSpreader::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysVirusSpreader& t )
{

    o << "MachPhysVirusSpreader " << (void*)&t << " start" << std::endl;
    o << "MachPhysVirusSpreader " << (void*)&t << " end" << std::endl;

    return o;
}

//static
const char* MachPhysVirusSpreader::compositeFilePath()
{
	return "models/weapons/virusgun/virusgun.cdf";
}


//virtual
MachPhysLinearProjectile* MachPhysVirusSpreader::createProjectile
(
    const PhysAbsoluteTime& burstStartTime, uint, W4dEntity*,
    const W4dEntity& target, const MexPoint3d& targetOffset
)
{
	PhysRelativeTime duration = weaponData().extras()[0];
	MachPhysLinearProjectile* pProjectile = createVirus(burstStartTime, duration, this, target, targetOffset);

	return pProjectile;
}

//virtual
PhysRelativeTime MachPhysVirusSpreader::fire( const PhysAbsoluteTime& startTime, int )
{
    //get weapon data
    const MachPhysWeaponData& data = weaponData();
    MexPoint3d offSet = data.launchOffsets()[0];
	MATHEX_SCALAR range = data.range();

	MATHEX_SCALAR flashSize = data.extras()[2];

    MachPhysFlashDisc *pFlashDisc = _NEW(MachPhysFlashDisc( this, offSet, flashSize, MachPhysFlashDisc::YELLOW) );
	PhysRelativeTime flashDuration = pFlashDisc->flash(startTime, MachPhysFlashDisc::YELLOW);

	//lighting up the machine and the weapon
	lighting(RenColour(1.5, 1.5, 0), startTime, 5);

    //Garbage collect the  WHITE  puff
    W4dGarbageCollector::instance().add( pFlashDisc, startTime + flashDuration );

    //Play the sound
	W4dSoundManager::instance().play( this, SID_VIRUS, startTime, 1 );
//    W4dSoundManager::instance().play( this, SysPathName( "sounds/virus.wav" ), startTime,
//                                      100.0, 100.0, W4dSoundManager::PLAY_ONCE );

    return flashDuration;
}

MachPhysVirusSpreader::MachPhysVirus* MachPhysVirusSpreader::createVirus(const PhysAbsoluteTime& burstTime,
								   const PhysRelativeTime& duration,
								   W4dEntity* pParent,
								   const W4dEntity& target,
								   const MexPoint3d& targetOffset )
{
    MexTransform3d startTransform;
    MATHEX_SCALAR distance;
	PhysAbsoluteTime launchTime = launchData(burstTime, 0, pParent, target, targetOffset, &startTransform, &distance);

	MachPhysVirusSpreader::MachPhysVirus* pVirus = _NEW(MachPhysElectro(pParent, startTransform, MachPhysElectro::VIRUS) );

 	//@100%, the electro size = 5
	MATHEX_SCALAR topScale = distance/5;

	pVirus->scaleAndSpin(burstTime, duration, topScale, startTransform);

	return pVirus;
 }

//virtual
PhysRelativeTime MachPhysVirusSpreader::victimAnimation
(
    const PhysAbsoluteTime& startTime, const MexLine3d& line, MachPhysMachine* pMachine
) const
{
	return applyVictimAnimation( startTime, line, pMachine );
}

//virtual
PhysRelativeTime MachPhysVirusSpreader::victimAnimation
(
    const PhysAbsoluteTime& startTime, const MexLine3d& line, MachPhysConstruction* pConstruction
) const
{
	return applyVictimAnimation( startTime, line, pConstruction );
}


//static
PhysRelativeTime MachPhysVirusSpreader::applyVictimAnimation
(
    const PhysAbsoluteTime& startTime, const MexLine3d&,
    MachPhysConstruction* pConstruction
)
{
	static const MachPhysWeaponData& data = MachPhysData::instance().weaponData( MachPhys::VIRUS );
	PhysRelativeTime duration = data.extras()[1];
	virusHaloInfection(startTime, duration, pConstruction );
	return duration;
}

//static
PhysRelativeTime MachPhysVirusSpreader::applyVictimAnimation
(
    const PhysAbsoluteTime& startTime, const MexLine3d&,
    MachPhysMachine* pMachine
)
{
	static const MachPhysWeaponData& data = MachPhysData::instance().weaponData( MachPhys::VIRUS );
	PhysRelativeTime duration = data.extras()[1];
	virusHaloInfection(startTime, duration, pMachine );
	return duration;
}

//static
void MachPhysVirusSpreader::virusHaloInfection
(
    const PhysAbsoluteTime& startTime,
    const PhysRelativeTime& duration,
    W4dComposite* pComposit
)
{
	pComposit->updateCompositeBoundingVolume();
	MexAlignedBox3d victimBox3D = pComposit->compositeBoundingVolume();
    MexPoint3d minCorner = victimBox3D.minCorner();
    MexPoint3d maxCorner = victimBox3D.maxCorner();
	MexPoint3d centre = MexPoint3d(0.5*(minCorner.x() + maxCorner.x()),
	                               0.5*(minCorner.y() + maxCorner.y()),
	                               0.5*(minCorner.z() + maxCorner.z()) );

	MexTransform3d localXform = MexTransform3d(centre);

	//lightning effect on the victim
	MachPhysLightningHalo* pHalo1 = _NEW(MachPhysLightningHalo(pComposit, localXform, MachPhysLightningHalo::VIRUS) );
	MachPhysLightningHalo* pHalo2 = _NEW(MachPhysLightningHalo(pComposit, localXform, MachPhysLightningHalo::VIRUS) );

	pHalo1->startLightning(pComposit, startTime, duration, -1, MachPhysLightningHalo::VIRUS);
	pHalo2->startLightning(pComposit, startTime, duration,  1, MachPhysLightningHalo::VIRUS);

//	garbage collect the haloes
    W4dGarbageCollector::instance().add( pHalo1, startTime + duration );
    W4dGarbageCollector::instance().add( pHalo2, startTime + duration );
}

//static
void MachPhysVirusSpreader::virusFromSourceToTarget(W4dEntity *pSourceEntity, const MexPoint3d& sourceOffset,
										W4dEntity& targetEntity, const MexPoint3d& targetOffset)
{
	static const MachPhysWeaponData& data = MachPhysData::instance().weaponData( MachPhys::VIRUS );
	MATHEX_SCALAR range = data.range();
    MATHEX_SCALAR maxAbsYawSine = 0.5;
    MATHEX_SCALAR maxAbsPitchSine = 0.5;

    W4dObjectTracker tracker( *pSourceEntity, sourceOffset,
                              targetEntity, targetOffset,
                              maxAbsYawSine, maxAbsPitchSine,
                              0.1, range, range );

    //Hence get the start launch position
    MexTransform3d startTransform;
	MATHEX_SCALAR distance = tracker.track( &startTransform );

	MachPhysVirusSpreader::MachPhysVirus* pVirus = _NEW(MachPhysElectro(pSourceEntity, startTransform, MachPhysElectro::VIRUS) );

 	//@100%, the electro size = 5
	MATHEX_SCALAR topScale = distance/5;
    const PhysAbsoluteTime now = SimManager::instance().currentTime();
	PhysRelativeTime duration = data.extras()[0];

	pVirus->scaleAndSpin(now + duration, duration, topScale, startTransform);

    W4dGarbageCollector::instance().add( pVirus, now + 2.0*duration );
}

void perWrite( PerOstream& ostr, const MachPhysVirusSpreader& weapon )
{
    const MachPhysLinearWeapon& base = weapon;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysVirusSpreader& weapon )
{
    MachPhysLinearWeapon& base = weapon;

    istr >> base;
}

/* End VIRUSSPR.CPP ****************************************************/
