/*
 * P L A S M A W E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "system/pathname.hpp"
#include "mathex/transf3d.hpp"

#include "world4d/root.hpp"
#include "world4d/soundman.hpp"
#include "world4d/composit.hpp"
#include "world4d/compplan.hpp"

#include "machphys/plasmbol.hpp"
#include "machphys/plasmawe.hpp"
#include "machphys/machine.hpp"
#include "machphys/constron.hpp"
#include "machphys/machdata.hpp"
#include "machphys/aggressr.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/private/weapper.hpp"
#include "machphys/snddata.hpp"
#include "render/colour.hpp"
#include "sim/manager.hpp"

PER_DEFINE_PERSISTENT( MachPhysPlasmaWeapon );

//Public ctor
MachPhysPlasmaWeapon::MachPhysPlasmaWeapon
(
     W4dEntity* pParent, const MexTransform3d& localTransform,
     MachPhys::WeaponType type, MachPhys::Mounting mounting
)
:   MachPhysLinearWeapon( exemplar( type ), mounting, pParent, localTransform )
{
    TEST_INVARIANT;
}

//One-time ctor used to read the mesh (once per plasma weapon type)
MachPhysPlasmaWeapon::MachPhysPlasmaWeapon( MachPhys::WeaponType type )
:   MachPhysLinearWeapon( MachPhysWeaponPersistence::instance().pRoot(), MexTransform3d(),
                          SysPathName( compositeFilePath( type ) ),
                          type, MachPhys::LEFT )
{
    TEST_INVARIANT;
}

MachPhysPlasmaWeapon::MachPhysPlasmaWeapon( PerConstructor con )
: MachPhysLinearWeapon( con )
{
}

MachPhysPlasmaWeapon::~MachPhysPlasmaWeapon()
{
    TEST_INVARIANT;

}

//static
const MachPhysPlasmaWeapon& MachPhysPlasmaWeapon::exemplar( MachPhys::WeaponType type )
{
    return MachPhysWeaponPersistence::instance().plasmaWeaponExemplar( type );
}

void MachPhysPlasmaWeapon::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysPlasmaWeapon& t )
{

    o << "MachPhysPlasmaWeapon " << (void*)&t << " start" << std::endl;
    o << "MachPhysPlasmaWeapon " << (void*)&t << " end" << std::endl;

    return o;
}

//static
const char* MachPhysPlasmaWeapon::compositeFilePath( MachPhys::WeaponType type )
{
    switch( type )
    {
        case MachPhys::PLASMA_RIFLE:
            return "models/weapons/plasmr/plasmr.cdf";
        case MachPhys::PLASMA_CANNON1:
            return "models/weapons/plasmc1/plasmc1.cdf";
        case MachPhys::PLASMA_CANNON2:
            return "models/weapons/plasmc2/plasmc2.cdf";

        DEFAULT_ASSERT_BAD_CASE( type );
    }

    return "";
}

//virtual
MachPhysLinearProjectile* MachPhysPlasmaWeapon::createProjectile
(
    const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
    const W4dEntity& target, const MexPoint3d& targetOffset
)
{
    return createPlasmaBolt( burstStartTime, index, pParent, target, targetOffset );
}

MachPhysPlasmaBolt* MachPhysPlasmaWeapon::createPlasmaBolt
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
        launchData( burstStartTime, index, pParent, target, targetOffset,
                    &startTransform, &distance );

	// Deterimine if the owning machine has fired any of it's
	// multi-weapons recently.
	MachPhysCanAttack* pCanAttack = NULL;
	if(hasMachine() && machine().hasCanAttack())
		pCanAttack = machine().canAttack();
    else if( hasConstruction() )
        pCanAttack = construction().canAttack();

    ASSERT( pCanAttack, "Weapon not mounted on a CanAttack" );

    const MachPhysWeaponData& data = weaponData();
    const uint nRounds = data.nRoundsPerBurst();
    const PhysRelativeTime burstDuration = data.burstDuration();

	// Check for a sound within the burst duration.  This should give one
	// sound per burst.
    const PhysAbsoluteTime interval = 0.95 * burstDuration;
	const bool recentSound = pCanAttack && pCanAttack->hasPlayedSoundWithin(type(), interval, launchTime);

	if (!recentSound)
	{
		pCanAttack->cacheSoundPlay(type(), launchTime);
	    playFiringSounds(launchTime);
	}

	// Check for a light within the round duration.  This should give one
	// light per round, rather than burst.  Cf sounds.
    const PhysAbsoluteTime roundTime = burstDuration / nRounds;
    const PhysAbsoluteTime interval2  = 0.95 * roundTime;
	const bool recentLight = pCanAttack && pCanAttack->hasLaunchedLightWithin(type(), interval2, launchTime);
	MachPhysPlasmaBolt::CreateLights createLights = MachPhysPlasmaBolt::DONT_CREATE_LIGHTS;

	if (!recentLight)
	{
		pCanAttack->cacheLightLaunch(type(), launchTime);
		createLights = MachPhysPlasmaBolt::CREATE_LIGHTS;
	}

    //Create the bolt
    MachPhysPlasmaBolt* pBolt = _NEW(MachPhysPlasmaBolt(pParent, startTransform, type(), createLights));

	//let it spin if can
	W4dCompositePlanPtr spinPlanPtr;

	if( pBolt->findCompositePlan( "spin", &spinPlanPtr ) )
	{
        pBolt->plan( *spinPlanPtr, launchTime, 1000000 );
	}

    //Make it fly
    pBolt->move( launchTime, weaponData() );

    return pBolt;
}

//virtual
PhysRelativeTime MachPhysPlasmaWeapon::fire( const PhysAbsoluteTime& startTime, int numberInBurst )
{
	// The lights attached to the projectile may be sufficient to light up the
	// launcher.  Perhaps this isn't needed?  If it is still needed, we should
	// checked the firedRecently method to avoid multiple lights.
	// lighting(col, startTime, 5);
	// MATHEX_SCALAR  amplitude = weaponData().extras()[1];
	// lighting(RenColour(0., amplitude*188.0/255.0, 0.), startTime, 8);

    //do recoil if any
    return recoil( startTime, numberInBurst );
}

//virtual
PhysRelativeTime MachPhysPlasmaWeapon::victimAnimation
(
    const PhysAbsoluteTime& startTime, const MexLine3d& fromDirection, MachPhysMachine* pMachine
) const
{
    return applyVictimAnimation( startTime, fromDirection, pMachine );
}

//virtual
PhysRelativeTime MachPhysPlasmaWeapon::victimAnimation
(
    const PhysAbsoluteTime& startTime, const MexLine3d& fromDirection, MachPhysConstruction* pConstruction
) const
{
    return applyVictimAnimation( startTime, fromDirection, pConstruction );
}

//static
PhysRelativeTime MachPhysPlasmaWeapon::applyVictimAnimation
(
    const PhysAbsoluteTime& startTime, const MexLine3d&, MachPhysMachine* pMachine
)
{
    //Make the machine shake
    return pMachine->shake( startTime );
}

//static
PhysRelativeTime MachPhysPlasmaWeapon::applyVictimAnimation
(
    const PhysAbsoluteTime&, const MexLine3d&, MachPhysConstruction*
)
{
    return 0.0;
}

void MachPhysPlasmaWeapon::playFiringSounds( const PhysAbsoluteTime& startTime )
{
    //get the number of rounds, burst duration, and hence compute interval
    const MachPhysWeaponData& data = weaponData();
    uint nRounds = data.nRoundsPerBurst();
    PhysRelativeTime duration = data.burstDuration();
    PhysRelativeTime interval = duration / nRounds;

    //Get the main wave file name
    char* pWaveFileName = NULL;

	SoundId sndData = SID_PLASMARIFLE;

    switch( type() )
    {
        case MachPhys::PLASMA_RIFLE:
        {
			sndData = SID_PLASMARIFLE;
//            pWaveFileName = "sounds/plasmrif.wav";
            break;
        }
        case MachPhys::PLASMA_CANNON1:
        case MachPhys::PLASMA_CANNON2:
        {
			sndData = SID_PLASMACANNON;
//            pWaveFileName = "sounds/plasmcan.wav";
            break;
        }
    }

    PhysAbsoluteTime soundTime = startTime;
	W4dSoundManager::instance().play( this, sndData, soundTime, 1 );
}

void perWrite( PerOstream& ostr, const MachPhysPlasmaWeapon& weapon )
{
    const MachPhysLinearWeapon& base = weapon;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysPlasmaWeapon& weapon )
{
    MachPhysLinearWeapon& base = weapon;

    istr >> base;
}

//static
PhysRelativeTime MachPhysPlasmaWeapon::applyVictimAnimation
(
    const PhysAbsoluteTime&, const MexLine3d&, MachPhysArtefact*
)
{
    return 0.0;
}

/* End PLASMAWE.CPP *************************************************/
