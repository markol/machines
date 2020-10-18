/*
 * A C T B M P N M . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/actbmpnm.hpp"

#include "machphys/machphys.hpp"
#include <algorithm>

//static
string MachActorBitmaps::name( MachLog::ObjectType objectType, int subType, int hwLevel, MachPhys::WeaponCombo wc, MachPhys::Race race )
{
	return name( objectType, subType, hwLevel, wc, race, false );
}

//static
string MachActorBitmaps::name( MachLog::ObjectType objectType, int subType, int hwLevel, MachPhys::WeaponCombo wc, MachPhys::Race race, bool inFirstPerson )
{
	HAL_STREAM("MachActorBitmaps::name " << objectType << " " << subType << " " << hwLevel << " " << inFirstPerson << std::endl );

	string retVal;

	ASSERT_INFO( objectType );

	// Add main sub-directory based on whether actor is machine or construction
	switch( objectType )
    {
        case MachLog::ADMINISTRATOR:
        case MachLog::AGGRESSOR:
        case MachLog::CONSTRUCTOR:
        case MachLog::GEO_LOCATOR:
        case MachLog::SPY_LOCATOR:
        case MachLog::TECHNICIAN:
        case MachLog::RESOURCE_CARRIER:
        case MachLog::APC:
			if ( inFirstPerson )
				retVal = "gui/fstpersn/machines/";
			else
				retVal = "gui/machines/";
			break;
        case MachLog::BEACON:
        case MachLog::FACTORY:
        case MachLog::GARRISON:
        case MachLog::HARDWARE_LAB:
        case MachLog::POD:
        case MachLog::MINE:
        case MachLog::MISSILE_EMPLACEMENT:
        case MachLog::SOFTWARE_LAB:
        case MachLog::SMELTER:
			if ( inFirstPerson )
				retVal = "gui/fstpersn/construc/";
			else
				retVal = "gui/construc/";
			break;
	    DEFAULT_ASSERT_BAD_CASE( objectType );
    }

	if ( not inFirstPerson )
	{
		// Add race identifier
		switch ( race )
		{
			case MachPhys::RED:
				retVal += "red/";
				break;
			case MachPhys::GREEN:
				retVal += "green/";
				break;
			case MachPhys::BLUE:
				retVal += "blue/";
				break;
			case MachPhys::YELLOW:
				retVal += "yellow/";
				break;
		};
	}

	// Add actor type
    switch( objectType )
    {
        case MachLog::ADMINISTRATOR:
            retVal += administratorName( (MachPhys::AdministratorSubType)subType );
			break;
        case MachLog::AGGRESSOR:
            retVal += aggressorName( (MachPhys::AggressorSubType)subType );
			break;
        case MachLog::CONSTRUCTOR:
            retVal += constructorName( (MachPhys::ConstructorSubType)subType );
			break;
        case MachLog::GEO_LOCATOR:
            retVal += geoLocatorName();
			break;
        case MachLog::SPY_LOCATOR:
            retVal += spyLocatorName();
			break;
        case MachLog::TECHNICIAN:
            retVal += technicianName( (MachPhys::TechnicianSubType)subType );
			break;
        case MachLog::RESOURCE_CARRIER:
            retVal += resourceCarrierName();
			break;
        case MachLog::APC:
            retVal += apcName();
			break;
        case MachLog::BEACON:
            retVal += beaconName();
			break;
        case MachLog::FACTORY:
            retVal += factoryName( (MachPhys::FactorySubType)subType );
			break;
        case MachLog::GARRISON:
            retVal += garrisonName();
			break;
        case MachLog::HARDWARE_LAB:
            retVal += hwLabName( (MachPhys::HardwareLabSubType)subType );
			break;
        case MachLog::POD:
            retVal += podName();
			break;
        case MachLog::MINE:
            retVal += mineName();
			break;
        case MachLog::MISSILE_EMPLACEMENT:
            retVal += missileEmplacementName( (MachPhys::MissileEmplacementSubType)subType );
			break;
        case MachLog::SOFTWARE_LAB:
			{
				ASSERT_INFO( objectType );
				ASSERT_BAD_CASE;
            	retVal += swLabName();
			}
			break;
        case MachLog::SMELTER:
            retVal += smelterName();
			break;
	    DEFAULT_ASSERT_BAD_CASE( objectType );
    }

	appendHwLevel( retVal, hwLevel );
	appendWeaponCombo( retVal, wc );

    std::transform(retVal.begin(), retVal.end(), retVal.begin(), ::tolower);
	ASSERT_FILE_EXISTS( retVal.c_str() );

    return retVal;
}

//static
const char* MachActorBitmaps::administratorName( MachPhys::AdministratorSubType subType )
{
    switch( subType )
    {
        case MachPhys::BOSS:
        	return "adb";
        case MachPhys::OVERSEER:
            return "ado";
        case MachPhys::COMMANDER:
			return "adc";
		DEFAULT_ASSERT_BAD_CASE( subType );
    }

    return "bad";
}

//static
const char* MachActorBitmaps::aggressorName( MachPhys::AggressorSubType subType )
{
    switch( subType )
    {
        case MachPhys::GRUNT:
            return "agg";
        case MachPhys::ASSASSIN:
            return "aga";
        case MachPhys::BALLISTA:
            return "agb";
        case MachPhys::KNIGHT:
            return "agk";
        case MachPhys::NINJA:
			return "agn";
        DEFAULT_ASSERT_BAD_CASE( subType );
    }

    return "bad";
}

//static
const char* MachActorBitmaps::constructorName( MachPhys::ConstructorSubType subType )
{
    switch( subType )
    {
        case MachPhys::DOZER:
			return "cod";
        case MachPhys::BUILDER:
			return "cob";
		case MachPhys::BEHEMOTH:
			return "como";
        DEFAULT_ASSERT_BAD_CASE( subType );
    }

    return "bad";
}

//static
const char* MachActorBitmaps::geoLocatorName()
{
   	return "log";
}

//static
const char* MachActorBitmaps::spyLocatorName()
{
    return "los";
}

//static
const char* MachActorBitmaps::technicianName( MachPhys::TechnicianSubType subType )
{
    switch( subType )
    {
        case MachPhys::LAB_TECH:
            return "tel";
        case MachPhys::TECH_BOY:
            return "tet";
        case MachPhys::BRAIN_BOX:
            return "teb";
        DEFAULT_ASSERT_BAD_CASE( subType );
    }

    return "bad";
}

//static
const char* MachActorBitmaps::resourceCarrierName()
{
	return "trr";
}

//static
const char* MachActorBitmaps::apcName()
{
    return "trp";
}

//static
const char* MachActorBitmaps::beaconName()
{
	return "bk";
}

//static
const char* MachActorBitmaps::factoryName( MachPhys::FactorySubType subType )
{
    switch( subType )
    {
        case MachPhys::CIVILIAN:
            return "fac";
        case MachPhys::MILITARY:
            return "fam";
        DEFAULT_ASSERT_BAD_CASE( subType );
    }

    return "bad";
}

//static
const char* MachActorBitmaps::garrisonName()
{
	return "ga";
}

//static
const char* MachActorBitmaps::hwLabName( MachPhys::HardwareLabSubType subType )
{
	HAL_STREAM("MachActorBitmaps::hwLabName " << subType << std::endl );
    switch( subType )
    {
        case MachPhys::LAB_CIVILIAN:
			return "cl";
        case MachPhys::LAB_MILITARY:
		    return "ml";
        DEFAULT_ASSERT_BAD_CASE( subType );
    }

    return "bad";
}

//static
const char* MachActorBitmaps::podName()
{
	return "pd";
}

//static
const char* MachActorBitmaps::mineName()
{
	return "mn";
}

//static
const char* MachActorBitmaps::missileEmplacementName( MachPhys::MissileEmplacementSubType subType )
{
	ASSERT_INFO( subType );

    switch( subType )
    {
        case MachPhys::TURRET:
            return "met";
        case MachPhys::SENTRY:
            return "mes";
		case MachPhys::LAUNCHER:
            return "mel";
		case MachPhys::ICBM:
            return "mei";
        DEFAULT_ASSERT_BAD_CASE( subType );
    }

    return "bad";
}

//static
const char* MachActorBitmaps::swLabName()
{
	return "sw";
}

//static
const char* MachActorBitmaps::smelterName()
{
	return "sm";
}

//static
void MachActorBitmaps::appendHwLevel( string& str, int hwLevel )
{
	char buffer[20];
//	itoa( hwLevel, buffer, 10 );
    sprintf(buffer, "%d", hwLevel);
	str += buffer;
}

//static
void MachActorBitmaps::appendWeaponCombo( string& s,  MachPhys::WeaponCombo wc )
{
	ASSERT_INFO( wc );

	switch ( wc )
	{
		case MachPhys::L_BOLTER:
		case MachPhys::R_BOLTER:
			s += "BOLT";
			break;
		case MachPhys::L_ELECTRIC_CHARGE:
		case MachPhys::R_ELECTRIC_CHARGE:
			s += "ELEC";
			break;
		case MachPhys::LR_PULSE_RIFLE_X2:
		case MachPhys::LR_PULSE_CANNON_X2:
			s += "PULS";
			break;
		case MachPhys::L_PULSE_RIFLE_R_SUPERCHARGE_ADVANCED:
	    case MachPhys::L_PULSE_CANNON_R_SUPERCHARGE_SUPER:
		case MachPhys::L_MULTI_LAUNCHER5_R_SUPERCHARGE_SUPER:
	    	s += "SCHG";
			break;
		case MachPhys::LR_PLASMA_RIFLE_X2:
		case MachPhys::L_PLASMA_RIFLE:
		case MachPhys::LR_PLASMA_CANNON1_X2:
		case MachPhys::LR_PLASMA_CANNON2_X2:
		case MachPhys::LRT_PLASMA_CANNON2_X3:
			s += "PLSM";
			break;
		case MachPhys::L_MULTI_LAUNCHER5_R_TREACHERY:
			s += "TRCH";
			break;
		case MachPhys::L_PULSE_CANNON_R_VIRUS:
			s += "VIRU";
			break;
		case MachPhys::T_VORTEX:
			s += "VORT";
			break;
		case MachPhys::L_FLAME_THROWER2:
		case MachPhys::T_FLAME_THROWER1:
			s += "FLM1";
			break;
		case MachPhys::LR_MULTI_LAUNCHER7_X2:
		case MachPhys::LR_MULTI_LAUNCHER5_X2:
		case MachPhys::LR_MULTI_LAUNCHER6_X2:
		case MachPhys::LR_MULTI_LAUNCHER2_X2:
		case MachPhys::LR_MULTI_LAUNCHER3_X2:
		case MachPhys::LRT_MULTI_LAUNCHER4_X3:
		case MachPhys::T_MULTI_LAUNCHER1:
			s += "MULT";
			break;
		case MachPhys::LR_HEAVY_BOLTER2_X2:
			s += "HVY2";
			break;
		case MachPhys::LR_HEAVY_BOLTER1_X2:
			s += "HVY1";
			break;
		case MachPhys::L_AUTO_CANNON:
		case MachPhys::LR_AUTO_CANNON_X2:
			s += "AUTO";
			break;
		case MachPhys::LR_LARGE_MISSILE_X2:
			s += "LMIS";
			break;
	    case MachPhys::T_NUCLEAR_MISSILE:
			s += "NUKE";
			break;
		case MachPhys::L_MULTI_LAUNCHER7_T_GORILLA_PUNCH_R_MULTI_LAUNCHER7:
			s += "PUNC";
			break;
		case MachPhys::T_WASP_LIGHT_STING:
			s += "LSTG";
			break;
		case MachPhys::T_WASP_METAL_STING:
			s += "MSTG";
			break;
		case MachPhys::T_BEE_BOMB:
			s += "BBMB";
			break;
		case MachPhys::T_ION_ORBITAL_CANNON:
			s += "ION";
			break;
		case MachPhys::N_WEAPON_COMBOS:
		default:
			break;
	}

	s += ".bmp";
}

/* End ACTBMPNM.CPP *************************************************/
