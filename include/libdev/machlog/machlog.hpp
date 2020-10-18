/*
 * M A C H L O G . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _MACH_MACHLOG_HPP
#define _MACH_MACHLOG_HPP

#include "base/persist.hpp"
#include "machphys/machphys.hpp"

// forward refs
class MachActor;
typedef MachActor * MachLogCommsId;

//Dummy namespace
struct MachLog
{
	enum ObjectType : unsigned char
	{
		ADMINISTRATOR,
		AGGRESSOR,
		CONSTRUCTOR,
		GEO_LOCATOR,
		SPY_LOCATOR,
		TECHNICIAN,
		RESOURCE_CARRIER,
		APC,
		BEACON,
		FACTORY,
		GARRISON,
		HARDWARE_LAB,
		ORE_HOLOGRAPH,
		POD,
		MINE,
		MISSILE_EMPLACEMENT,
		SOFTWARE_LAB,
		SMELTER,
		DEBRIS,
		SQUADRON,
		LAND_MINE,
        ARTEFACT,
		N_OBJECT_TYPES
	};
	enum Message : unsigned char
	{
		TELEPORTED_OUT_OF_WORLD,
		ENTERED_WORLD,
        MACHINE_BUILT, //Sent by factory on completion of building a machine
        HW_RESEARCH_COMPLETE, //Sent by hardware lab when research item completed
        SW_RESEARCH_COMPLETE, //Sent by hardware lab when research item completed
		HEALTH_STATUS_CHANGED, //Sent by Actors/machines when armour or hp changes.
		RACE_CHANGED,	//Sent by actors when they change race (captured/orb of treachery)
		SQUADRON_CHANGED,	//squadrons will send this on add/remove methods
		NOT_GOING_TO_APC,        //Sent by machines when they were previously going to board an APC but have now changed plan
		PERCENTAGE_COMPLETE_CHANGED,	//Sent by constructions when they are being built - particularly for benefit of status bars
		WEAPON_AVAILABILITY_CHANGED,             //Used by actors with weapons that become available dynamically and/or have large recharge times
    	MINE_CHANGED_OPERATIONAL_STATUS,			// Sent by mines when they have either exhausted a mineral site when previously active or found
												// a new mineral source when dormant
		CHANGED_CAMOUFLAGE_STATUS,
		CHANGED_MINERALS_CARRIED,				// When transporters pick up or put down
		CHANGED_N_MACHINES_INSIDE_CONSTRUCTION
	};
    //Selection state for actors
    enum SelectionState : unsigned char
    {
    	NOT_SELECTED,
    	SELECTED, //Permanently selected
    	HIGHLIGHTED //Denotes item under cursor
    };

	//defcon for machines..."Bravado level"
	enum DefCon : unsigned char
	{
		DEFCON_HIGH,
		DEFCON_NORMAL,
		DEFCON_LOW
	};

	enum TargetSystemType : unsigned char
                          { TARGET_NORMAL,
							TARGET_RESEARCH,
							TARGET_RESOURCE,
   							TARGET_OBJECT,
    						TARGET_AGGRESSIVES,
							FAVOUR_STATIC_TARGETS,
							TARGET_AIR_UNITS,
							DONT_TARGET_AIR_UNITS };

	enum SpecialParameters { DONTCARE = 999 };

	enum SelectableType :   unsigned char { NOT_SELECTABLE, FULLY_SELECTABLE };

	enum BeaconType :       unsigned char { NO_BEACON, LEVEL_1_BEACON, LEVEL_3_BEACON };

    //  Types of obstacles that some machines can travel over
    //  Make sure that OBSTACLE_ALL is the OR of all other obstacles.
    //  These are bitwise definitions
    enum ObstacleFlag
    {
        OBSTACLE_NORMAL = 0,
        OBSTACLE_WATER = 1,
        OBSTACLE_LOW = 2,
        OBSTACLE_ALL = 3
    };
	enum RandomStarts :         unsigned char { RANDOM_START_LOCATIONS, FIXED_START_LOCATIONS };
	enum PlayerType :           unsigned char { PC_LOCAL, PC_REMOTE, AI_LOCAL, AI_REMOTE, NOT_DEFINED };
	enum ResourcesAvailable :   unsigned char { RES_DEFAULT, RES_LOW, RES_MEDIUM, RES_HIGH };
	enum StartingResources :    unsigned char { STARTING_RESOURCES_DEFAULT, STARTING_RESOURCES_RES_LOW,
                                                STARTING_RESOURCES_MEDIUM, STARTING_RESOURCES_HIGH,
                                                STARTING_RESOURCES_VERY_HIGH, STARTING_RESOURCES_SUPER_HIGH };
	enum VictoryCondition :     unsigned char { VICTORY_DEFAULT, VICTORY_ANNIHILATION, VICTORY_POD, VICTORY_TIMER };
	enum TechnologyLevel :      unsigned char { TECH_LEVEL_DEFAULT, TECH_LEVEL_LOW, TECH_LEVEL_MEDIUM, TECH_LEVEL_HIGH };
	enum ForceRemove :          unsigned char { FORCE_REMOVE, DO_NOT_FORCE_REMOVE };
	enum GameType :             unsigned char { CAMPAIGN_SINGLE_PLAYER, SKIRMISH_SINGLE_PLAYER, MULTIPLAYER };
};

ostream& operator<<( ostream&, const MachLog::ObjectType& );
ostream& operator<<( ostream& , const MachLog::TargetSystemType& );
ostream& operator<<( ostream& , const MachLog::DefCon& );
ostream& operator<<( ostream& , const MachLog::SelectableType& );
ostream& operator<<( ostream& , const MachLog::BeaconType& );
ostream& operator<<( ostream&, const MachLog::RandomStarts& );
ostream& operator<<( ostream&, const MachLog::PlayerType& );
ostream& operator<<( ostream&, const MachLog::ResourcesAvailable& );
ostream& operator<<( ostream&, const MachLog::StartingResources& );
ostream& operator<<( ostream&, const MachLog::TechnologyLevel& );
ostream& operator<<( ostream&, const MachLog::VictoryCondition& );
ostream& operator<<( ostream&, const MachLog::ForceRemove& );
ostream& operator<<( ostream&, const MachLog::GameType& );

PER_ENUM_PERSISTENT( MachLog::ObjectType );
PER_ENUM_PERSISTENT( MachLog::Message );
PER_ENUM_PERSISTENT( MachLog::SelectionState );
PER_ENUM_PERSISTENT( MachLog::DefCon );
PER_ENUM_PERSISTENT( MachLog::TargetSystemType );
PER_ENUM_PERSISTENT( MachLog::SpecialParameters );
PER_ENUM_PERSISTENT( MachLog::SelectableType );
PER_ENUM_PERSISTENT( MachLog::BeaconType );
PER_ENUM_PERSISTENT( MachLog::RandomStarts );
PER_ENUM_PERSISTENT( MachLog::PlayerType );
PER_ENUM_PERSISTENT( MachLog::ResourcesAvailable );
PER_ENUM_PERSISTENT( MachLog::StartingResources );
PER_ENUM_PERSISTENT( MachLog::TechnologyLevel );
PER_ENUM_PERSISTENT( MachLog::VictoryCondition );
PER_ENUM_PERSISTENT( MachLog::GameType );
/* //////////////////////////////////////////////////////////////// */

#endif	/* #ifndef _MACH_MACHLOG_HPP	*/

/* End MACHLOG.HPP ***************************************************/
