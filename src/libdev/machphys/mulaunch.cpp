/*
 * M U L A U N C H . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

//string.hpp included to avoid compilation errors with the
//inclusion of machine.hpp
#include "stdlib/string.hpp"
#include "system/pathname.hpp"
#include "mathex/transf3d.hpp"
#include "world4d/root.hpp"
#include "world4d/soundman.hpp"
#include "machphys/mulaunch.hpp"
#include "machphys/missile.hpp"
#include "machphys/machine.hpp"
#include "machphys/attack.hpp"
#include "machphys/private/weapper.hpp"
#include "machphys/snddata.hpp"
#include "machphys/constron.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/machdata.hpp"

#include "render/colour.hpp"
#include "sim/manager.hpp"

PER_DEFINE_PERSISTENT( MachPhysMultiLauncher );

//Public ctor
MachPhysMultiLauncher::MachPhysMultiLauncher
(
     W4dEntity* pParent, const MexTransform3d& localTransform,
     MachPhys::WeaponType type, MachPhys::Mounting mounting
)
:   MachPhysLinearWeapon( exemplar( type ), mounting, pParent, localTransform )
{
    TEST_INVARIANT;
}

//One-time ctor used to read the mesh (once per multi-launcher type)
MachPhysMultiLauncher::MachPhysMultiLauncher( MachPhys::WeaponType type )
:   MachPhysLinearWeapon( MachPhysWeaponPersistence::instance().pRoot(), MexTransform3d(),
                          SysPathName( compositeFilePath( type ) ),
                          type, MachPhys::LEFT )
{
    TEST_INVARIANT;
}

MachPhysMultiLauncher::MachPhysMultiLauncher( PerConstructor con )
: MachPhysLinearWeapon( con )
{
}

MachPhysMultiLauncher::~MachPhysMultiLauncher()
{
    TEST_INVARIANT;
}

void MachPhysMultiLauncher::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysMultiLauncher& t )
{

    o << "MachPhysMultiLauncher " << (void*)&t << " start" << std::endl;
    o << "MachPhysMultiLauncher " << (void*)&t << " end" << std::endl;

    return o;
}

//static
const char* MachPhysMultiLauncher::compositeFilePath( MachPhys::WeaponType type )
{
    switch( type )
    {
        case MachPhys::MULTI_LAUNCHER1:
            return "models/weapons/mult1/mult1.cdf";
        case MachPhys::MULTI_LAUNCHER2:
            return "models/weapons/mult2/mult2.cdf";
        case MachPhys::MULTI_LAUNCHER3:
            return "models/weapons/mult3/mult3.cdf";
        case MachPhys::MULTI_LAUNCHER4:
            return "models/weapons/mult4/mult4.cdf";
        case MachPhys::MULTI_LAUNCHER5:
            return "models/weapons/mult5/mult5.cdf";
        case MachPhys::MULTI_LAUNCHER6:
            return "models/weapons/mult6/mult6.cdf";
        case MachPhys::MULTI_LAUNCHER7:
            return "models/weapons/mult7/mult7.cdf";
    }

    return "";
}

//static
const MachPhysMultiLauncher& MachPhysMultiLauncher::exemplar( MachPhys::WeaponType type )
{
    return MachPhysWeaponPersistence::instance().multiLauncherExemplar( type );
}

//virtual
MachPhysLinearProjectile* MachPhysMultiLauncher::createProjectile
(
    const PhysAbsoluteTime& burstStartTime, uint index, W4dEntity* pParent,
    const W4dEntity& target, const MexPoint3d& targetOffset
)
{
    return createMissile( burstStartTime, index, pParent, target, targetOffset );
}

MachPhysMissile* MachPhysMultiLauncher::createMissile
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

	// Deterimine if the owning machine or emplacement has fired any of it's
	// multi-weapons recently.
	MachPhysCanAttack* pCanAttack = NULL;

	if(hasMachine() && machine().hasCanAttack())
		pCanAttack = machine().canAttack();
	else if (hasConstruction() && construction().hasCanAttack())
		pCanAttack = construction().canAttack();

    const MachPhysWeaponData& data = weaponData();
    const uint nRounds = data.nRoundsPerBurst();
    const PhysRelativeTime burstDuration = data.burstDuration();

	// Check for a light within the round duration.  This should give one
	// light per round, rather than burst.  Cf sounds.
    const PhysAbsoluteTime roundTime = burstDuration / nRounds;
    const PhysAbsoluteTime interval2  = 0.95 * roundTime;
	const bool recentLight = pCanAttack && pCanAttack->hasLaunchedLightWithin(type(), interval2, launchTime);
	MachPhysMissile::CreateLights createLights = MachPhysMissile::DONT_CREATE_LIGHTS;

	if (!recentLight)
	{
		pCanAttack->cacheLightLaunch(type(), launchTime);
		createLights = MachPhysMissile::CREATE_LIGHTS;
	}

    //Create the corresponding missile
	MachPhysMissile* pMissileResult = NULL;

	SoundId launchSound = SID_MLAUNCH;

	bool attachedToMachine = hasMachine();

    switch( type() )
    {
        case MachPhys::MULTI_LAUNCHER1:
		{
			//ballista3
    		MachPhysMissile* pMissile = _NEW( MachPhysMissile( pParent, startTransform, MachPhysMissile::MISSILE6, createLights ) );
			pMissileResult = pMissile;
			launchSound = SID_MLAUNCH1;
			SOUND_STREAM("Playing SID_MLAUNCH1" << std::endl);
			break;
		}
        case MachPhys::MULTI_LAUNCHER2:
		{
			//ballista4
    		MachPhysMissile* pMissile = _NEW( MachPhysMissile( pParent, startTransform, MachPhysMissile::MISSILE5, createLights ) );
			pMissileResult = pMissile;
			launchSound = SID_MLAUNCH2;
			SOUND_STREAM("Playing SID_MLAUNCH2" << std::endl);
			break;
		}
        case MachPhys::MULTI_LAUNCHER3:
		{
			//knight4
    		MachPhysMissile* pMissile = _NEW( MachPhysMissile( pParent, startTransform, MachPhysMissile::MISSILE1, createLights ) );
			pMissileResult = pMissile;
			launchSound = SID_MLAUNCH3;
			SOUND_STREAM("Playing SID_MLAUNCH3" << std::endl);
			break;
		}
        case MachPhys::MULTI_LAUNCHER4:
		{
			//knight5
    		MachPhysMissile* pMissile = _NEW( MachPhysMissile( pParent, startTransform, MachPhysMissile::MISSILE4, createLights ) );
			pMissileResult = pMissile;
			launchSound = SID_MLAUNCH4;
			SOUND_STREAM("Playing SID_MLAUNCH4" << std::endl);
			break;
		}
        case MachPhys::MULTI_LAUNCHER5:
		{
			//sentry3
    		MachPhysMissile* pMissile = _NEW( MachPhysMissile( pParent, startTransform, MachPhysMissile::MISSILE2, createLights ) );
			pMissileResult = pMissile;
			//If the machine is an administrator then assign it its own
			//launch sound
		    if(attachedToMachine && machine().machineData().subType() == MachPhys::COMMANDER)
			{
				launchSound = SID_MLAUNCH8;
				SOUND_STREAM("Playing SID_MLAUNCH8" << std::endl);
			}
			else
			{
				launchSound = SID_MLAUNCH5;
				SOUND_STREAM("Playing SID_MLAUNCH5" << std::endl);
			}
			break;
		}
        case MachPhys::MULTI_LAUNCHER6:
		{
			//sentry4
    		MachPhysMissile* pMissile = _NEW( MachPhysMissile( pParent, startTransform, MachPhysMissile::MISSILE3, createLights ) );
			pMissileResult = pMissile;
			launchSound = SID_MLAUNCH6;
			SOUND_STREAM("Playing SID_MLAUNCH6" << std::endl);
			break;
		}
        case MachPhys::MULTI_LAUNCHER7:
		{
			//ninja5
    		MachPhysMissile* pMissile = _NEW( MachPhysMissile( pParent, startTransform, MachPhysMissile::MISSILE7, createLights ) );
			pMissileResult = pMissile;
			launchSound = SID_MLAUNCH7;
			SOUND_STREAM("Playing SID_MLAUNCH7" << std::endl);
			break;
		}
        DEFAULT_ASSERT_BAD_CASE( type() );
    }

	if( attachedToMachine )
	{
		//If this missile is being fired from first person
		//make sure it knows this fact
		pMissileResult->setLauncherControl(machine().getMachineControlType());
	}

    //Make it fly
    pMissileResult->beLaunched( launchTime, weaponData(), targetOffset );

	// Check for a sound within the burst duration.  This should give one
	// sound per burst.
    const PhysAbsoluteTime interval  = 0.95 * burstDuration;
	const bool recentSound = pCanAttack && pCanAttack->hasPlayedSoundWithin(type(), interval, launchTime);

	if (!recentSound)
	{
		pCanAttack->cacheSoundPlay(type(), launchTime - 0.5);

	    // Play the firing sound and attach a sound to the missile.
		W4dSoundManager::instance().play(this, launchSound, launchTime - 0.5, 1);
	}

    return pMissileResult;
}

//virtual
PhysRelativeTime MachPhysMultiLauncher::fire( const PhysAbsoluteTime& startTime, int numberInBurst )
{
	//lighting up the machine and the weapon
	RenColour col;
	switch(type())
	{
		case MachPhys::MULTI_LAUNCHER1:
			col = RenColour(1.0, 122.0/255.0, 0.0);
		case MachPhys::MULTI_LAUNCHER2:
			col = RenColour(1.0, 1.0, 200.0/255.0);
			break;
		case MachPhys::MULTI_LAUNCHER3:
			col = RenColour(1.0, 200.0/255.0, 200.0/255.0);
			break;
		case MachPhys::MULTI_LAUNCHER4:
			col = RenColour(200./255.0, 1.0, 1.0);
			break;
		case MachPhys::MULTI_LAUNCHER5:
			col = RenColour(200./255.0, 1.0, 200./255.0);
			break;
		case MachPhys::MULTI_LAUNCHER6:
			col = RenColour(200./255.0, 200./255.0, 1.0);
			break;
		case MachPhys::MULTI_LAUNCHER7:
		//case MachPhys::LARGE_MISSILE:
			col = RenColour(0.784, 0.784, 0.784);
			break;
	}

	// The lights attached to the missiles may be sufficient to light up the
	// launcher.  Perhaps this isn't needed?  If it is still needed, we should
	// checked the firedRecently method to avoid multiple lights.
	// lighting(col, startTime, 5);
    recoil( startTime, numberInBurst );

    return 0.0;
}

void perWrite( PerOstream& ostr, const MachPhysMultiLauncher& weapon )
{
    const MachPhysLinearWeapon& base = weapon;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysMultiLauncher& weapon )
{
    MachPhysLinearWeapon& base = weapon;

    istr >> base;
}

/* End MULAUNCH.CPP *************************************************/
