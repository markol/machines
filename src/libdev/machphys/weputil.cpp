/*
 * W E P U T I L . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/weputil.hpp"
#include "machphys/pulsewep.hpp"
#include "machphys/bolter.hpp"
#include "machphys/elecharg.hpp"
#include "machphys/plasmawe.hpp"
#include "machphys/flamethr.hpp"

//static
PhysRelativeTime MachPhysWeaponUtility::victimAnimation
(
    MachPhys::WeaponType type, const PhysAbsoluteTime& startTime,
    const MexLine3d& fromDirection, MachPhysMachine* pMachine
)
{
    PhysRelativeTime duration = 0.0;

    switch( type )
    {
        case MachPhys::ELECTRIC_CHARGE:
        {
            duration = MachPhysElectroCharger::applyVictimAnimation( startTime, fromDirection, pMachine );
            break;
        }

        case MachPhys::PLASMA_RIFLE:
        case MachPhys::PLASMA_CANNON1:
        case MachPhys::PLASMA_CANNON2:
        {
            duration = MachPhysPlasmaWeapon::applyVictimAnimation( startTime, fromDirection, pMachine );
            break;
        }

        case MachPhys::PULSE_RIFLE:
        case MachPhys::PULSE_CANNON:
        {
            duration = MachPhysPulseWeapon::applyVictimAnimation( startTime, fromDirection, pMachine );
            break;
        }

		case MachPhys::VORTEX:
        case MachPhys::BOLTER:
        case MachPhys::AUTO_CANNON:
        case MachPhys::HEAVY_BOLTER1:
        case MachPhys::HEAVY_BOLTER2:
        {
            duration = MachPhysBolter::applyVictimAnimation( startTime, fromDirection, pMachine );
            break;
        }

        case MachPhys::FLAME_THROWER1:
        case MachPhys::FLAME_THROWER2:
        {
            duration = MachPhysFlameThrower::applyVictimAnimation( startTime, fromDirection, pMachine );
            break;
        }
    }

    return duration;
}

//static
PhysRelativeTime MachPhysWeaponUtility::victimAnimation
(
    MachPhys::WeaponType type, const PhysAbsoluteTime& startTime,
    const MexLine3d& fromDirection, MachPhysConstruction* pConstruction
)
{
    PhysRelativeTime duration = 0.0;

    switch( type )
    {
        case MachPhys::ELECTRIC_CHARGE:
        {
            duration = MachPhysElectroCharger::applyVictimAnimation( startTime, fromDirection, pConstruction );
            break;
        }

        case MachPhys::PLASMA_RIFLE:
        case MachPhys::PLASMA_CANNON1:
        case MachPhys::PLASMA_CANNON2:
        {
            duration = MachPhysPlasmaWeapon::applyVictimAnimation( startTime, fromDirection, pConstruction );
            break;
        }

        case MachPhys::PULSE_RIFLE:
        case MachPhys::PULSE_CANNON:
        {
            duration = MachPhysPulseWeapon::applyVictimAnimation( startTime, fromDirection,
                                                                  pConstruction );
            break;
        }

        case MachPhys::BOLTER:
        case MachPhys::AUTO_CANNON:
        case MachPhys::HEAVY_BOLTER1:
        case MachPhys::HEAVY_BOLTER2:
        {
            duration = MachPhysBolter::applyVictimAnimation( startTime, fromDirection,
                                                             pConstruction );
            break;
        }

        case MachPhys::FLAME_THROWER1:
        case MachPhys::FLAME_THROWER2:
        {
            duration = MachPhysFlameThrower::applyVictimAnimation( startTime, fromDirection,
                                                                   pConstruction );
            break;
        }
    }

    return duration;
}

//static
PhysRelativeTime MachPhysWeaponUtility::victimAnimation
(
    MachPhys::WeaponType type, const PhysAbsoluteTime& startTime,
    const MexLine3d& fromDirection, MachPhysArtefact* pArtefact
)
{
    PhysRelativeTime duration = 0.0;

    switch( type )
    {
        case MachPhys::ELECTRIC_CHARGE:
        {
            duration = MachPhysElectroCharger::applyVictimAnimation( startTime, fromDirection, pArtefact );
            break;
        }

        case MachPhys::PLASMA_RIFLE:
        case MachPhys::PLASMA_CANNON1:
        case MachPhys::PLASMA_CANNON2:
        {
            duration = MachPhysPlasmaWeapon::applyVictimAnimation( startTime, fromDirection, pArtefact );
            break;
        }

        case MachPhys::PULSE_RIFLE:
        case MachPhys::PULSE_CANNON:
        {
            duration = MachPhysPulseWeapon::applyVictimAnimation( startTime, fromDirection,
                                                                  pArtefact );
            break;
        }

        case MachPhys::BOLTER:
        case MachPhys::AUTO_CANNON:
        case MachPhys::HEAVY_BOLTER1:
        case MachPhys::HEAVY_BOLTER2:
        {
            duration = MachPhysBolter::applyVictimAnimation( startTime, fromDirection,
                                                             pArtefact );
            break;
        }

        case MachPhys::FLAME_THROWER1:
        case MachPhys::FLAME_THROWER2:
        {
            duration = MachPhysFlameThrower::applyVictimAnimation( startTime, fromDirection,
                                                                   pArtefact );
            break;
        }
    }

    return duration;
}
/* End WEPUTIL.CPP **************************************************/
