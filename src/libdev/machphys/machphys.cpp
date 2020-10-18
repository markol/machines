/*
 * M A C H P H Y S . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include <fstream>

#include "machphys/machphys.hpp"

ostream& operator <<( ostream& o, MachPhys::MachineType type )
{
	switch( type )
	{
		case MachPhys::AGGRESSOR:
			o << "AGGRESSOR";
			break;
		case MachPhys::GEO_LOCATOR:
			o << "GEO_LOCATOR";
			break;
		case MachPhys::SPY_LOCATOR:
			o << "SPY_LOCATOR";
			break;
		case MachPhys::CONSTRUCTOR:
			o << "CONSTRUCTOR";
			break;
		case MachPhys::ADMINISTRATOR:
			o << "ADMINISTRATOR";
			break;
		case MachPhys::RESOURCE_CARRIER:
			o << "RESOURCE_CARRIER";
			break;
		case MachPhys::APC:
			o << "APC";
			break;
		case MachPhys::TECHNICIAN:
			o << "TECHNICIAN";
			break;
	}
	return o;
}

ostream& operator <<( ostream& o, MachPhys::AggressorSubType type )
{
	switch( type )
	{
		case MachPhys::GRUNT:
			o << "GRUNT";
			break;
		case MachPhys::ASSASSIN:
			o << "ASSASSIN";
			break;
		case MachPhys::KNIGHT:
			o << "KNIGHT";
			break;
		case MachPhys::BALLISTA:
			o << "BALLISTA";
			break;
		case MachPhys::NINJA:
			o << "NINJA";
			break;
	}
	return o;
}

ostream& operator <<( ostream& o, MachPhys::AdministratorSubType type )
{
	switch( type )
	{
		case MachPhys::BOSS:
			o << "BOSS";
			break;
		case MachPhys::OVERSEER:
			o << "OVERSEER";
			break;
		case MachPhys::COMMANDER:
			o << "COMMANDER";
			break;
	}
	return o;
}

ostream& operator <<( ostream& o, MachPhys::TechnicianSubType type )
{
	switch( type )
	{
		case MachPhys::LAB_TECH:
			o << "LAB_TECH";
			break;
		case MachPhys::TECH_BOY:
			o << "TECH_BOY";
			break;
		case MachPhys::BRAIN_BOX:
			o << "BRAIN_BOX";
			break;
	}
	return o;
}

ostream& operator <<( ostream& o, MachPhys::ConstructorSubType type )
{
	switch( type )
	{
		case MachPhys::DOZER:
			o << "DOZER";
			break;
		case MachPhys::BUILDER:
			o << "BUILDER";
			break;
		case MachPhys::BEHEMOTH:
			o << "BEHEMOTH";
			break;
	}
	return o;
}

ostream& operator <<( ostream& o, MachPhys::ConstructionType type )
{
	switch( type )
	{
		case MachPhys::HARDWARE_LAB:
			o << "HARDWARE_LAB";
			break;
		case MachPhys::SMELTER:
			o << "SMELTER";
			break;
		case MachPhys::FACTORY:
			o << "FACTORY";
			break;
		case MachPhys::MISSILE_EMPLACEMENT:
			o << "MISSILE_EMPLACEMENT";
			break;
		case MachPhys::GARRISON:
			o << "GARRISON";
			break;
		case MachPhys::MINE:
			o << "MINE";
			break;
		case MachPhys::BEACON:
			o << "BEACON";
			break;
		case MachPhys::POD:
			o << "POD";
			break;
        default:
            o << "UNKNOWN " << (int)type;
            break;
	}
	return o;
}

ostream& operator <<( ostream& o, MachPhys::FactorySubType type )
{
	switch( type )
	{
		case MachPhys::CIVILIAN:
			o << "CIVILIAN";
			break;
		case MachPhys::MILITARY:
			o << "MILITARY";
			break;
	}
	return o;
}

ostream& operator <<( ostream& o, MachPhys::HardwareLabSubType type )
{
	switch( type )
	{
		case MachPhys::LAB_CIVILIAN:
			o << "LAB_CIVILIAN";
			break;
		case MachPhys::LAB_MILITARY:
			o << "LAB_MILITARY";
			break;
	}
	return o;
}

ostream& operator <<( ostream& o, MachPhys::MissileEmplacementSubType type )
{
	switch( type )
	{
		case MachPhys::TURRET:
			o << "TURRET";
			break;
		case MachPhys::SENTRY:
			o << "SENTRY";
			break;
		case MachPhys::LAUNCHER:
			o << "LAUNCHER";
			break;
		case MachPhys::ICBM:
			o << "ICBM";
			break;
	}
	return o;
}

ostream& operator <<( ostream& o, MachPhys::WeaponType type )
{
	switch( type )
	{
	case MachPhys::ELECTRIC_CHARGE:
		o << "ELECTRIC_CHARGE";
		break;
    case MachPhys::FLAME_THROWER1:
		o << "FLAME_THROWER1";
		break;
    case MachPhys::FLAME_THROWER2:
		o << "FLAME_THROWER2";
		break;
    case MachPhys::BOLTER:
		o << "BOLTER";
		break;
    case MachPhys::AUTO_CANNON:
		o << "AUTO_CANNON";
		break;
    case MachPhys::GORILLA_PUNCH:
		o << "GORILLA_PUNCH";
		break;
    case MachPhys::PLASMA_RIFLE:
		o << "PLASMA_RIFLE";
		break;
    case MachPhys::PULSE_RIFLE:
		o << "PULSE_RIFLE";
		break;
    case MachPhys::PLASMA_CANNON1:
		o << "PLASMA_CANNON1";
		break;
    case MachPhys::PLASMA_CANNON2:
		o << "PLASMA_CANNON2";
		break;
    case MachPhys::PULSE_CANNON:
		o << "PULSE_CANNON";
		break;
    case MachPhys::HEAVY_BOLTER1:
		o << "HEAVY_BOLTER1";
		break;
    case MachPhys::HEAVY_BOLTER2:
		o << "HEAVY_BOLTER2";
		break;
    case MachPhys::HOMING_MISSILE:
		o << "HOMING_MISSILE";
		break;
    case MachPhys::LARGE_MISSILE:
		o << "LARGE_MISSILE";
		break;
    case MachPhys::MULTI_LAUNCHER1:
		o << "MULTI_LAUNCHER1";
		break;
    case MachPhys::MULTI_LAUNCHER2:
		o << "MULTI_LAUNCHER2";
		break;
    case MachPhys::MULTI_LAUNCHER3:
		o << "MULTI_LAUNCHER3";
		break;
    case MachPhys::MULTI_LAUNCHER4:
		o << "MULTI_LAUNCHER4";
		break;
    case MachPhys::MULTI_LAUNCHER5:
		o << "MULTI_LAUNCHER5";
		break;
    case MachPhys::MULTI_LAUNCHER6:
		o << "MULTI_LAUNCHER6";
		break;
    case MachPhys::MULTI_LAUNCHER7:
		o << "MULTI_LAUNCHER7";
		break;
    case MachPhys::SUPERCHARGE_ADVANCED:
		o << "SUPERCHARGE_ADVANCED";
		break;
    case MachPhys::SUPERCHARGE_SUPER:
		o << "SUPERCHARGE_SUPER";
		break;
    case MachPhys::NUCLEAR_MISSILE:
		o << "NUCLEAR_MISSILE";
		break;
    case MachPhys::VORTEX:
		o << "VORTEX";
		break;
    case MachPhys::ION_ORBITAL_CANNON:
		o << "ION_ORBITAL_CANNON";
		break;
    case MachPhys::ORB_OF_TREACHERY:
		o << "ORB_OF_TREACHERY";
		break;
    case MachPhys::VIRUS:
		o << "VIRUS";
		break;
    case MachPhys::BEE_BOMB:
		o << "BEE_BOMB";
		break;
    case MachPhys::WASP_LIGHT_STING:
		o << "WASP_LIGHT_STING";
		break;
    case MachPhys::WASP_METAL_STING:
		o << "WASP_METAL_STING";
		break;
	default:
		o << "UNKNOWN WEAPON TYPE IN MACHPHYS.CPP " << (int)type;
		break;
	}
	return o;
}
ostream& operator <<( ostream& o, MachPhys::Mounting type )
{
	o << "Mountings are not streamed " << (int)type;
	return o;
}
ostream& operator <<( ostream& o, MachPhys::WeaponCombo type )
{
   	switch( type )
	{
		case MachPhys::L_BOLTER:
		o << "L_BOLTER";
		break;
        case MachPhys::T_ION_ORBITAL_CANNON:
        o << "T_ION_ORBITAL_CANNON";
        break;
			case MachPhys::L_ELECTRIC_CHARGE:
		o << "L_ELECTRIC_CHARGE";
		break;
			case MachPhys::R_BOLTER:
		o << "R_BOLTER";
		break;
			case MachPhys::R_ELECTRIC_CHARGE:
		o << "R_ELECTRIC_CHARGE";
		break;
			case MachPhys::LR_PULSE_RIFLE_X2:
		o << "LR_PULSE_RIFLE_X2";
		break;
			case MachPhys::L_PULSE_RIFLE_R_SUPERCHARGE_ADVANCED:
		o << "L_PULSE_RIFLE_R_SUPERCHARGE_ADVANCED";
		break;
			case MachPhys::LR_PLASMA_RIFLE_X2:
		o << "LR_PLASMA_RIFLE_X2";
		break;
			case MachPhys::L_MULTI_LAUNCHER5_R_SUPERCHARGE_SUPER:
		o << "L_MULTI_LAUNCHER5_R_SUPERCHARGE_SUPER";
		break;
			case MachPhys::L_MULTI_LAUNCHER5_R_TREACHERY:
		o << "L_MULTI_LAUNCHER5_R_TREACHERY";
		break;
			case MachPhys::LR_PULSE_CANNON_X2:
		o << "LR_PULSE_CANNON_X2";
		break;
			case MachPhys::L_PULSE_CANNON_R_SUPERCHARGE_SUPER:
		o << "L_PULSE_CANNON_R_SUPERCHARGE_SUPER";
		break;
			case MachPhys::L_PULSE_CANNON_R_VIRUS:
		o << "L_PULSE_CANNON_R_VIRUS";
		break;
			case MachPhys::L_FLAME_THROWER2:
		o << "L_FLAME_THROWER2";
		break;
			case MachPhys::LR_HEAVY_BOLTER1_X2:
		o << "LR_HEAVY_BOLTER1_X2";
		break;
			case MachPhys::L_MULTI_LAUNCHER7_T_GORILLA_PUNCH_R_MULTI_LAUNCHER7:
			o << "L_MULTI_LAUNCHER7_T_GORILLA_PUNCH_R_MULTI_LAUNCHER7";
		break;
			case MachPhys::L_AUTO_CANNON:
		o << "L_AUTO_CANNON";
		break;
			case MachPhys::L_PLASMA_RIFLE:
		o << "L_PLASMA_RIFLE";
		break;
			case MachPhys::LR_AUTO_CANNON_X2:
		o << "LR_AUTO_CANNON_X2";
		break;
			case MachPhys::T_NUCLEAR_MISSILE:
		o << "T_NUCLEAR_MISSILE";
		break;
			case MachPhys::T_MULTI_LAUNCHER1:
		o << "T_MULTI_LAUNCHER1";
		break;
			case MachPhys::T_VORTEX:
		o << "T_VORTEX";
		break;
			case MachPhys::LR_MULTI_LAUNCHER2_X2:
		o << "LR_MULTI_LAUNCHER2_X2";
		break;
			case MachPhys::LR_HEAVY_BOLTER2_X2:
		o << "LR_HEAVY_BOLTER2_X2";
		break;
			case MachPhys::LR_PLASMA_CANNON1_X2:
		o << "LR_PLASMA_CANNON1_X2";
		break;
			case MachPhys::LR_MULTI_LAUNCHER3_X2:
		o << "LR_MULTI_LAUNCHER3_X2";
		break;
			case MachPhys::LR_PLASMA_CANNON2_X2:
		o << "LR_PLASMA_CANNON2_X2";
		break;
			case MachPhys::LRT_MULTI_LAUNCHER4_X3:
		o << "LRT_MULTI_LAUNCHER4_X3";
		break;
			case MachPhys::LRT_PLASMA_CANNON2_X3:
		o << "LRT_PLASMA_CANNON2_X3";
		break;
			case MachPhys::LR_MULTI_LAUNCHER7_X2:
		o << "LR_MULTI_LAUNCHER7_X2";
		break;
			case MachPhys::T_FLAME_THROWER1:
		o << "T_FLAME_THROWER1";
		break;
			case MachPhys::LR_MULTI_LAUNCHER5_X2:
		o << "LR_MULTI_LAUNCHER5_X2";
		break;
			case MachPhys::LR_MULTI_LAUNCHER6_X2:
		o << "LR_MULTI_LAUNCHER6_X2";
		break;
			case MachPhys::LR_LARGE_MISSILE_X2:
		o << "LR_LARGE_MISSILE_X2";
		break;
			case MachPhys::T_BEE_BOMB:
		o << "T_BEE_BOMB";
		break;
			case MachPhys::T_WASP_LIGHT_STING:
		o << "T_WASP_LIGHT_STING";
		break;
			case MachPhys::T_WASP_METAL_STING:
		o << "T_WASP_METAL_STING";
		break;
			case MachPhys::N_WEAPON_COMBOS:
		o << "N_WEAPON_COMBOS";
		break;
		default:
		o << "Unknown weapon combo type " << (int)type << std::endl;
	}
	return o;
}

ostream& operator <<( ostream& o, MachPhysFireballType type )
{
    switch( type )
    {
        case FIREBALL_1:
            o << "FIREBALL_1";
            break;
        case FIREBALL_2:
            o << "FIREBALL_2";
            break;
        case FIREBALL_3:
            o << "FIREBALL_3";
            break;
        case FIREBALL_4:
            o << "FIREBALL_4";
            break;
    }

    return o;
}

ostream& operator <<( ostream& o, MachPhys::Race type )
{
    switch( type )
    {
        case MachPhys::RED:
            o << "RED";
            break;
        case MachPhys::BLUE:
            o << "BLUE";
            break;
        case MachPhys::GREEN:
            o << "GREEN";
            break;
        case MachPhys::YELLOW:
            o << "YELLOW";
            break;
        case MachPhys::NORACE:
            o << "NORACE";
            break;
    }

    return o;
}

ostream& operator <<( ostream& o, MachPhysPuffType type )
{
    switch( type )
    {
        case PUFF_1:
            o << "PUFF_1";
            break;
        case PUFF_2:
            o << "PUFF_2";
            break;
        case PUFF_3:
            o << "PUFF_3";
            break;
        case PUFF_4:
            o << "PUFF_4";
            break;
    }

    return o;
}

ostream& operator <<( ostream& o, MachPhys::AnimationIds animId )
{
	switch( animId )
	{
		case MachPhys::CONSTRUCTION_WORKING:

			o << "CONSTRUCTION_WORKING";
			break;

		case MachPhys::CONSTRUCTION_CONSTRUCTING:

			o << "CONSTRUCTION_CONSTRUCTING";
			break;
	}

	return o;
}

ostream& operator <<( ostream& o, MachPhys::FinalState finalState )
{
    switch( finalState )
    {
        case MachPhys::AT_REST:
            o << "AT_REST";
            break;
        case MachPhys::IN_MOTION:
            o << "IN_MOTION";
            break;
    }

    return o;
}
