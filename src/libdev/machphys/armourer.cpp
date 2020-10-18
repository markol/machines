/*
 * A R M O U R E R . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/armourer.hpp"
#include "machphys/weapon.hpp"
#include "machphys/attack.hpp"
#include "machphys/pulsewep.hpp"
#include "machphys/bolter.hpp"
#include "machphys/charger.hpp"
#include "machphys/ionweap.hpp"
#include "machphys/vortweap.hpp"
#include "machphys/treachry.hpp"
#include "machphys/flamethr.hpp"
#include "machphys/elecharg.hpp"
#include "machphys/mulaunch.hpp"
#include "machphys/plasmawe.hpp"
#include "machphys/virusspr.hpp"
#include "machphys/largemsl.hpp"
#include "machphys/nuclweap.hpp"
#include "machphys/punweap.hpp"
#include "machphys/beeweap.hpp"
#include "machphys/stglwep.hpp"
#include "machphys/stgmwep.hpp"

#include "world4d/composit.hpp"

#include "mathex/transf3d.hpp"

//static
MachPhysWeapon* MachPhysArmourer::newWeapon
(
    MachPhys::WeaponType type, MachPhys::Mounting mounting,
    W4dEntity* pParent, const MexTransform3d& localTransform
)
{
    MachPhysWeapon* pWeapon = NULL;

    switch( type )
    {
        case MachPhys::PLASMA_RIFLE:
        case MachPhys::PLASMA_CANNON1:
        case MachPhys::PLASMA_CANNON2:
            pWeapon = _NEW( MachPhysPlasmaWeapon( pParent, localTransform, type, mounting ) );
            break;

        case MachPhys::MULTI_LAUNCHER1:
        case MachPhys::MULTI_LAUNCHER2:
        case MachPhys::MULTI_LAUNCHER3:
        case MachPhys::MULTI_LAUNCHER4:
        case MachPhys::MULTI_LAUNCHER5:
        case MachPhys::MULTI_LAUNCHER6:
        case MachPhys::MULTI_LAUNCHER7:
            pWeapon = _NEW( MachPhysMultiLauncher( pParent, localTransform, type, mounting ) );
            break;

        case MachPhys::ELECTRIC_CHARGE:
            pWeapon = _NEW( MachPhysElectroCharger( pParent, localTransform, mounting ) );
            break;

        case MachPhys::FLAME_THROWER1:
        case MachPhys::FLAME_THROWER2:
            pWeapon = _NEW( MachPhysFlameThrower( pParent, localTransform, type, mounting ) );
            break;

        case MachPhys::BOLTER:
        case MachPhys::AUTO_CANNON:
        case MachPhys::HEAVY_BOLTER1:
        case MachPhys::HEAVY_BOLTER2:
            pWeapon = _NEW( MachPhysBolter( pParent, localTransform, type, mounting ) );
            break;

        case MachPhys::PULSE_RIFLE:
            pWeapon = _NEW( MachPhysPulseRifle( pParent, localTransform, mounting ) );
            break;

        case MachPhys::PULSE_CANNON:
            pWeapon = _NEW( MachPhysPulseCannon( pParent, localTransform, mounting ) );
            break;

        case MachPhys::SUPERCHARGE_ADVANCED:
        case MachPhys::SUPERCHARGE_SUPER:
            pWeapon = _NEW( MachPhysSuperCharger( pParent, localTransform, type, mounting ) );
            break;

        case MachPhys::ION_ORBITAL_CANNON:
            pWeapon = _NEW( MachPhysIonWeapon( pParent, localTransform, type, mounting ) );
            break;

        case MachPhys::VORTEX:
            pWeapon = _NEW( MachPhysVortexWeapon( pParent, localTransform, mounting ) );
            break;

        case MachPhys::ORB_OF_TREACHERY:
            pWeapon = _NEW( MachPhysTreacheryWeapon( pParent, localTransform, mounting ) );
            break;

        case MachPhys::VIRUS:
            pWeapon = _NEW( MachPhysVirusSpreader( pParent, localTransform, mounting ) );
            break;

        case MachPhys::LARGE_MISSILE:
            pWeapon = _NEW( MachPhysLargeMissile( pParent, localTransform, mounting ) );
            break;

        case MachPhys::NUCLEAR_MISSILE:
            pWeapon = _NEW( MachPhysNuclearWeapon( pParent, localTransform, mounting ) );
            break;

        case MachPhys::GORILLA_PUNCH:
            pWeapon = _NEW( MachPhysPunchWeapon( pParent, localTransform ) );
            break;

        case MachPhys::BEE_BOMB:
            pWeapon = _NEW( MachPhysBeeBomber( pParent, localTransform ) );
            break;

        case MachPhys::WASP_LIGHT_STING:
            pWeapon = _NEW( MachPhysLightStingWeapon( pParent, localTransform ) );
            break;			

        case MachPhys::WASP_METAL_STING:
            pWeapon = _NEW( MachPhysMetalStingWeapon( pParent, localTransform ) );
            break;

        DEFAULT_ASSERT_BAD_CASE( type );
    }

    POST( pWeapon != NULL );
    return pWeapon;
}

//static
void MachPhysArmourer::fitWeapons( MachPhysCanAttack* pAttacker, MachPhys::WeaponCombo combo )
{
    MexTransform3d identityTransform;
    W4dEntity* pEntityAttacker = &pAttacker->asComposite();
    MachPhys::WeaponType aTypes[MachPhys::N_MOUNTINGS];
    MachPhys::Mounting aMountings[MachPhys::N_MOUNTINGS];
    int nWeapons = 0;

    switch( combo )
    {
        case MachPhys::L_ELECTRIC_CHARGE:
        {
            aTypes[0] = MachPhys::ELECTRIC_CHARGE;
            aMountings[0] = MachPhys::LEFT;
            nWeapons = 1;
            break;
        }
        case MachPhys::R_ELECTRIC_CHARGE:
        {
            aTypes[0] = MachPhys::ELECTRIC_CHARGE;
            aMountings[0] = MachPhys::RIGHT;
            nWeapons = 1;
            break;
        }
        case MachPhys::T_FLAME_THROWER1:
        {
            aTypes[0] = MachPhys::FLAME_THROWER1;
            aMountings[0] = MachPhys::TOP;
            nWeapons = 1;
            break;
        }
        case MachPhys::L_FLAME_THROWER2:
        {
            aTypes[0] = MachPhys::FLAME_THROWER2;
            aMountings[0] = MachPhys::LEFT;
            nWeapons = 1;
            break;
        }

        case MachPhys::L_BOLTER:
        {
            aTypes[0] = MachPhys::BOLTER;
            aMountings[0] = MachPhys::LEFT;
            nWeapons = 1;
            break;
        }

        case MachPhys::T_ION_ORBITAL_CANNON:
        {
            aTypes[0] = MachPhys::ION_ORBITAL_CANNON;
            aMountings[0] = MachPhys::TOP;
            nWeapons = 1;
            break;
        }

        case MachPhys::T_VORTEX:
        {
            aTypes[0] = MachPhys::VORTEX;
            aMountings[0] = MachPhys::TOP;
            nWeapons = 1;
            break;
        }

        case MachPhys::R_BOLTER:
        {
            aTypes[0] = MachPhys::BOLTER;
            aMountings[0] = MachPhys::RIGHT;
            nWeapons = 1;
            break;
        }

        case MachPhys::LR_HEAVY_BOLTER1_X2:
        {
            aTypes[0] = aTypes[1] = MachPhys::HEAVY_BOLTER1;
            aMountings[0] = MachPhys::RIGHT;
            aMountings[1] = MachPhys::LEFT;
            nWeapons = 2;
            break;
        }

        case MachPhys::LR_HEAVY_BOLTER2_X2:
        {
            aTypes[0] = aTypes[1] = MachPhys::HEAVY_BOLTER2;
            aMountings[0] = MachPhys::RIGHT;
            aMountings[1] = MachPhys::LEFT;
            nWeapons = 2;
            break;
        }

        case MachPhys::L_AUTO_CANNON:
        {
            aTypes[0] = MachPhys::AUTO_CANNON;
            aMountings[0] = MachPhys::LEFT;
            nWeapons = 1;
            break;
        }

        case MachPhys::LR_AUTO_CANNON_X2:
        {
            aTypes[0] = aTypes[1] = MachPhys::AUTO_CANNON;
            aMountings[0] = MachPhys::RIGHT;
            aMountings[1] = MachPhys::LEFT;
            nWeapons = 2;
            break;
        }

        case MachPhys::LR_PULSE_RIFLE_X2:
        {
            aTypes[0] = aTypes[1] = MachPhys::PULSE_RIFLE;
            aMountings[0] = MachPhys::RIGHT;
            aMountings[1] = MachPhys::LEFT;
            nWeapons = 2;
            break;
        }

        case MachPhys::LR_PLASMA_RIFLE_X2:
        {
            //TBD the weapon combo name needs to be changed. Came from typo in wep.doc
            aTypes[0] = aTypes[1] = MachPhys::PLASMA_RIFLE;
            aMountings[0] = MachPhys::RIGHT;
            aMountings[1] = MachPhys::LEFT;
            nWeapons = 2;
            break;
        }

        case MachPhys::L_PULSE_RIFLE_R_SUPERCHARGE_ADVANCED:
        {
            aTypes[0] = MachPhys::SUPERCHARGE_ADVANCED;
            aTypes[1] = MachPhys::PULSE_RIFLE;
            aMountings[0] = MachPhys::RIGHT;
            aMountings[1] = MachPhys::LEFT;
            nWeapons = 2;
            break;
        }

        case MachPhys::LR_PULSE_CANNON_X2:
        {
            aTypes[0] = aTypes[1] = MachPhys::PULSE_CANNON;
            aMountings[0] = MachPhys::RIGHT;
            aMountings[1] = MachPhys::LEFT;
            nWeapons = 2;
            break;
        }

        case MachPhys::L_PULSE_CANNON_R_SUPERCHARGE_SUPER:
        {
            aTypes[0] = MachPhys::SUPERCHARGE_SUPER;
            aTypes[1] = MachPhys::PULSE_CANNON;
            aMountings[0] = MachPhys::RIGHT;
            aMountings[1] = MachPhys::LEFT;
            nWeapons = 2;
            break;
        }

        case MachPhys::L_MULTI_LAUNCHER5_R_SUPERCHARGE_SUPER:
        {
            aTypes[0] = MachPhys::SUPERCHARGE_SUPER;
            aTypes[1] = MachPhys::MULTI_LAUNCHER5;
            aMountings[0] = MachPhys::RIGHT;
            aMountings[1] = MachPhys::LEFT;
            nWeapons = 2;
            break;
        }

        case MachPhys::L_MULTI_LAUNCHER5_R_TREACHERY:
        {
            aTypes[0] = MachPhys::ORB_OF_TREACHERY;
            aTypes[1] = MachPhys::MULTI_LAUNCHER5;
            aMountings[0] = MachPhys::RIGHT;
            aMountings[1] = MachPhys::LEFT;
            nWeapons = 2;
            break;
        }

        case MachPhys::T_MULTI_LAUNCHER1:
        {
            aTypes[0] = MachPhys::MULTI_LAUNCHER1;
            aMountings[0] = MachPhys::TOP;
            nWeapons = 1;
            break;
        }

        case MachPhys::LR_MULTI_LAUNCHER2_X2:
        {
            aTypes[0] = aTypes[1] = MachPhys::MULTI_LAUNCHER2;
            aMountings[0] = MachPhys::RIGHT;
            aMountings[1] = MachPhys::LEFT;
            nWeapons = 2;
            break;
        }

        case MachPhys::LR_MULTI_LAUNCHER3_X2:
        {
            aTypes[0] = aTypes[1] = MachPhys::MULTI_LAUNCHER3;
            aMountings[0] = MachPhys::RIGHT;
            aMountings[1] = MachPhys::LEFT;
            nWeapons = 2;
            break;
        }

        case MachPhys::LRT_MULTI_LAUNCHER4_X3:
        {
            aTypes[0] = aTypes[1] = aTypes[2] = MachPhys::MULTI_LAUNCHER4;
            aMountings[0] = MachPhys::RIGHT;
            aMountings[1] = MachPhys::LEFT;
            aMountings[2] = MachPhys::TOP;
            nWeapons = 3;
            break;
        }

        case MachPhys::LR_MULTI_LAUNCHER5_X2:
        {
            aTypes[0] = aTypes[1] = MachPhys::MULTI_LAUNCHER5;
            aMountings[0] = MachPhys::RIGHT;
            aMountings[1] = MachPhys::LEFT;
            nWeapons = 2;
            break;
        }

        case MachPhys::LR_MULTI_LAUNCHER6_X2:
        {
            aTypes[0] = aTypes[1] = MachPhys::MULTI_LAUNCHER6;
            aMountings[0] = MachPhys::RIGHT;
            aMountings[1] = MachPhys::LEFT;
            nWeapons = 2;
            break;
        }

        case MachPhys::L_PLASMA_RIFLE:
        {
            aTypes[0] = MachPhys::PLASMA_RIFLE;
            aMountings[0] = MachPhys::LEFT;
            nWeapons = 1;
            break;
        }

        case MachPhys::LR_PLASMA_CANNON1_X2:
        {
            aTypes[0] = aTypes[1] = MachPhys::PLASMA_CANNON1;
            aMountings[0] = MachPhys::RIGHT;
            aMountings[1] = MachPhys::LEFT;
            nWeapons = 2;
            break;
        }

        case MachPhys::LR_PLASMA_CANNON2_X2:
        {
            aTypes[0] = aTypes[1] = MachPhys::PLASMA_CANNON2;
            aMountings[0] = MachPhys::RIGHT;
            aMountings[1] = MachPhys::LEFT;
            nWeapons = 2;
            break;
        }

        case MachPhys::LRT_PLASMA_CANNON2_X3:
        {
            aTypes[0] = aTypes[1] = aTypes[2] = MachPhys::PLASMA_CANNON2;
            aMountings[0] = MachPhys::RIGHT;
            aMountings[1] = MachPhys::LEFT;
            aMountings[2] = MachPhys::TOP;
            nWeapons = 3;
            break;
        }

        case MachPhys::L_PULSE_CANNON_R_VIRUS:
        {
            aTypes[0] = MachPhys::VIRUS;
            aTypes[1] = MachPhys::PULSE_CANNON;
            aMountings[0] = MachPhys::RIGHT;
            aMountings[1] = MachPhys::LEFT;
            nWeapons = 2;
            break;
        }

        case MachPhys::LR_MULTI_LAUNCHER7_X2:
        {
            aTypes[0] = aTypes[1] = MachPhys::MULTI_LAUNCHER7;
            aMountings[0] = MachPhys::RIGHT;
            aMountings[1] = MachPhys::LEFT;
            nWeapons = 2;
            break;
        }

        case MachPhys::LR_LARGE_MISSILE_X2:
        {
            aTypes[0] = aTypes[1] = MachPhys::LARGE_MISSILE;
            aMountings[0] = MachPhys::RIGHT;
            aMountings[1] = MachPhys::LEFT;
            nWeapons = 2;
            break;
        }

		case MachPhys::T_NUCLEAR_MISSILE:
        {
            aTypes[0] = MachPhys::NUCLEAR_MISSILE;
            aMountings[0] = MachPhys::TOP;
            nWeapons = 1;
 
            break;
        }

		case MachPhys::L_MULTI_LAUNCHER7_T_GORILLA_PUNCH_R_MULTI_LAUNCHER7:
        {
            aTypes[0] = MachPhys::MULTI_LAUNCHER7;
            aTypes[1] = MachPhys::MULTI_LAUNCHER7;
            aTypes[2] = MachPhys::GORILLA_PUNCH;
            aMountings[0] = MachPhys::RIGHT;
            aMountings[1] = MachPhys::LEFT;
            aMountings[2] = MachPhys::TOP;
            nWeapons = 3;
 
            break;
        }

		case MachPhys::T_BEE_BOMB:
        {
            aTypes[0] = MachPhys::BEE_BOMB;
            aMountings[0] = MachPhys::TOP;
            nWeapons = 1;
 
            break;
        }

		case MachPhys::T_WASP_LIGHT_STING:
        {
            aTypes[0] = MachPhys::WASP_LIGHT_STING;
            aMountings[0] = MachPhys::TOP;
            nWeapons = 1;
 
            break;
        }

		case MachPhys::T_WASP_METAL_STING:
        {
            aTypes[0] = MachPhys::WASP_METAL_STING;
            aMountings[0] = MachPhys::TOP;
            nWeapons = 1;
 
            break;
        }

        DEFAULT_ASSERT_BAD_CASE( combo );
    }

    for( int i = 0; i != nWeapons; ++i )
    {
        pAttacker->mount( newWeapon( aTypes[i], aMountings[i],
                                    pEntityAttacker, identityTransform ), aMountings[i] );
    }

	//update the bounding volume so that the weapons are included
	pAttacker->asComposite().updateCompositeBoundingVolume();
}

//static
void MachPhysArmourer::fitWeapons
(
    MachPhysMachine* pMachine, MachPhysCanAttack* pAttacker, MachPhys::WeaponCombo combo
)
{
    //fit the weapons
    fitWeapons( pAttacker, combo );

    //Set the machine
    for( uint index = 0; index != pAttacker->nMountedWeapons(); ++index )
        pAttacker->weapon( index ).machine( pMachine );
}

//static
void MachPhysArmourer::fitWeapons
(
    MachPhysConstruction* pConstruction, MachPhysCanAttack* pAttacker,
    MachPhys::WeaponCombo combo
)
{
    //fit the weapons
    fitWeapons( pAttacker, combo );

    //Set the construction
    for( uint index = 0; index != pAttacker->nMountedWeapons(); ++index )
        pAttacker->weapon( index ).construction( pConstruction );
}

// forced recompile 7/1/99 CPS

/* End ARMOURER.CPP *************************************************/




                           