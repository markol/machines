/*
 * M A C H L O G . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include <fstream>
#include "machlog/machlog.hpp"

ostream& operator<<(ostream& o, const MachLog::ObjectType& t)
{
	switch( t )
	{
		case MachLog::ADMINISTRATOR:
			o << "ADMINISTRATOR";
			break;
		case MachLog::AGGRESSOR:
			o << "AGGRESSOR";
			break;
		case MachLog::CONSTRUCTOR:
			o << "CONSTRUCTOR";
			break;
		case MachLog::GEO_LOCATOR:
			o << "GEO_LOCATOR";
			break;
		case MachLog::SPY_LOCATOR:
			o << "SPY_LOCATOR";
			break;
		case MachLog::TECHNICIAN:
			o << "TECHNICIAN";
			break;
		case MachLog::RESOURCE_CARRIER:
			o << "RESOURCE_CARRIER";
			break;
		case MachLog::APC:
			o << "APC";
			break;
		case MachLog::BEACON:
			o << "BEACON";
			break;
		case MachLog::FACTORY:
			o << "FACTORY";
			break;
		case MachLog::GARRISON:
			o << "GARRISON";
			break;
		case MachLog::HARDWARE_LAB:
			o << "HARDWARE_LAB";
			break;
		case MachLog::POD:
			o << "POD";
			break;
		case MachLog::MINE:
			o << "MINE";
			break;
		case MachLog::MISSILE_EMPLACEMENT:
			o << "MISSILE_EMPLACEMENT";
			break;
		case MachLog::SOFTWARE_LAB:
			o << "SOFTWARE_LAB";
			break;
		case MachLog::SMELTER:
			o << "SMELTER";
			break;
		case MachLog::ORE_HOLOGRAPH:
			o << "ORE_HOLOGRAPH";
			break;
		case MachLog::DEBRIS:
			o << "DEBRIS";
			break;
		case MachLog::SQUADRON:
			o << "SQUADRON";
			break;
		case MachLog::LAND_MINE:
			o << "LAND_MINE";
			break;
		case MachLog::ARTEFACT:
			o << "ARTEFACT";
			break;
		default:
			ASSERT_INFO( (int)t )
			ASSERT_BAD_CASE;
	}
	return o;
}

ostream& operator<<(ostream& o, const MachLog::DefCon& t)
{
	switch( t )
	{
		case MachLog::DEFCON_HIGH:
			o << "DEFCON_HIGH";
			break;
		case MachLog::DEFCON_NORMAL:
			o << "DEFCON_NORMAL";
			break;
		case MachLog::DEFCON_LOW:
			o << "DEFCON_LOW";
			break;
		default:
			ASSERT_INFO( (int)t )
			ASSERT_BAD_CASE;
	};
	return o;
}

ostream& operator<<(ostream& o, const MachLog::TargetSystemType& t )
{ 
	switch( t )
	{
	case MachLog::TARGET_NORMAL:
		o << "TARGET_NORMAL";
		break;
	case MachLog::TARGET_RESEARCH:
		o << "TARGET_RESEARCH";
		break;
	case MachLog::TARGET_RESOURCE:
		o << "TARGET_RESOURCE";
		break;
	case MachLog::TARGET_OBJECT:
		o << "TARGET_OBJECT";
		break;
	default:
		ASSERT_INFO( (int)t )
		ASSERT_BAD_CASE;
	}
	return o;
}

ostream& operator<<(ostream& o, const MachLog::BeaconType& t )
{ 
	switch( t )
	{
	case MachLog::NO_BEACON:
		o << "NO_BEACON";
		break;
	case MachLog::LEVEL_1_BEACON:
		o << "LEVEL_1_BEACON";
		break;
	case MachLog::LEVEL_3_BEACON:
		o << "LEVEL_3_BEACON";
		break;
	default:
		o << "Unknown type in BeaconType " << (int)t << std::endl;
	}
	return o;
}

ostream& operator<<( ostream& o, const MachLog::RandomStarts& randomStarts )
{
	switch( randomStarts )
	{
		case MachLog::RANDOM_START_LOCATIONS: o << "RANDOM_START_LOCATIONS"; break;
		case MachLog::FIXED_START_LOCATIONS: o << "FIXED_START_LOCATIONS"; break;
	}
	return o;
}

ostream& operator<<( ostream& o, const MachLog::PlayerType& playerType )
{
	switch( playerType )
	{
		case MachLog::PC_LOCAL: o << "PC_LOCAL"; break;
		case MachLog::PC_REMOTE: o << "PC_REMOTE"; break;
		case MachLog::AI_LOCAL: o << "AI_LOCAL"; break;
		case MachLog::AI_REMOTE: o << "AI_REMOTE"; break;
		case MachLog::NOT_DEFINED: o << "NOT_DEFINED"; break;
	}
	return o;
}

ostream& operator<<( ostream& o, const MachLog::ResourcesAvailable& resources )
{
	switch( resources )
	{
		case MachLog::RES_DEFAULT: o << "RES_DEFAULT"; break;
		case MachLog::RES_LOW: o << "RES_LOW"; break;
		case MachLog::RES_MEDIUM: o << "RES_MEDIUM"; break;
		case MachLog::RES_HIGH: o << "RES_HIGH"; break;
	}
	return o;
}

ostream& operator<<( ostream& o, const MachLog::StartingResources& resources )
{
	switch( resources )
	{
		case MachLog::STARTING_RESOURCES_DEFAULT: o << "STARTING_RESOURCES_DEFAULT"; break;
		case MachLog::STARTING_RESOURCES_RES_LOW: o << "STARTING_RESOURCES_RES_LOW"; break;
		case MachLog::STARTING_RESOURCES_MEDIUM: o << "STARTING_RESOURCES_MEDIUM"; break;
		case MachLog::STARTING_RESOURCES_HIGH: o << "STARTING_RESOURCES_HIGH"; break;
		case MachLog::STARTING_RESOURCES_VERY_HIGH: o << "STARTING_RESOURCES_VERY_HIGH"; break;
		case MachLog::STARTING_RESOURCES_SUPER_HIGH: o << "STARTING_RESOURCES_SUPER_HIGH"; break;
	}
	return o;
}

ostream& operator<<( ostream& o, const MachLog::TechnologyLevel& techLevel )
{
	switch( techLevel )
	{
		case MachLog::TECH_LEVEL_DEFAULT: o << "TECH_LEVEL_DEFAULT"; break;
		case MachLog::TECH_LEVEL_LOW: o << "TECH_LEVEL_LOW"; break;
		case MachLog::TECH_LEVEL_MEDIUM: o << "TECH_LEVEL_MEDIUM"; break;
		case MachLog::TECH_LEVEL_HIGH: o << "TECH_LEVEL_HIGH"; break;
	}
	return o;
}

ostream& operator<<( ostream& o, const MachLog::VictoryCondition& victory )
{
	switch( victory )
	{
		case MachLog::VICTORY_DEFAULT: o << "VICTORY_DEFAULT"; break;
		case MachLog::VICTORY_ANNIHILATION: o << "VICTORY_ANNIHILATION"; break;
		case MachLog::VICTORY_POD: o << "VICTORY_POD"; break;
		case MachLog::VICTORY_TIMER: o << "VICTORY_TIMER"; break;
	}
	return o;
}

ostream& operator<<( ostream& o, const MachLog::GameType& t)
{
	switch( t )
	{
		case MachLog::CAMPAIGN_SINGLE_PLAYER: o << "CAMPAIGN_SINGLE_PLAYER"; break;
		case MachLog::SKIRMISH_SINGLE_PLAYER: o << "SKIRMISH_SINGLE_PLAYER"; break;
		case MachLog::MULTIPLAYER: o << "MULTIPLAYER"; break;
	}
	return o;
}
/* End MACHLOG.CPP ***************************************************/
