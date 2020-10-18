/*
 * M A C H P H Y S . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _MACHPHYS_MACHPHYS_HPP
#define _MACHPHYS_MACHPHYS_HPP

#include "phys/phys.hpp"
#include "base/persist.hpp"

//Dummy namespace
struct MachPhys
{
    // added type to fix persistence
    enum Race : unsigned char { RED, BLUE, GREEN, YELLOW, N_RACES, NORACE };

    enum MachineType : unsigned char
    {
        AGGRESSOR, GEO_LOCATOR, SPY_LOCATOR, CONSTRUCTOR, ADMINISTRATOR,
        RESOURCE_CARRIER, APC, TECHNICIAN
    };
    enum ConstructionType : unsigned char
    {
        HARDWARE_LAB, /*SOFTWARE_LAB, WEAPONS_LAB,*/ SMELTER, FACTORY,
        MISSILE_EMPLACEMENT, GARRISON, MINE, BEACON, POD
    };

	enum DoorType : unsigned char
	{
		SMALL_DOOR,
		LARGE_DOOR
	};

    enum AggressorSubType : unsigned char { GRUNT, ASSASSIN, KNIGHT, BALLISTA, NINJA };
    enum TechnicianSubType : unsigned char { LAB_TECH, TECH_BOY, BRAIN_BOX };
    enum ConstructorSubType : unsigned char { DOZER, BUILDER, BEHEMOTH };
    enum AdministratorSubType : unsigned char { BOSS, OVERSEER, COMMANDER };

    enum FactorySubType : unsigned char { CIVILIAN, MILITARY };
    enum HardwareLabSubType : unsigned char { LAB_CIVILIAN, LAB_MILITARY };
    enum MissileEmplacementSubType : unsigned char { TURRET, SENTRY, LAUNCHER, ICBM };

    enum LocomotionType : unsigned char
    {
        NO_LOCOMOTION,
        WHEELS,
        TRACKS,
        HOVER,
        SPIDER,
        GLIDER
    };

    enum WeaponType : unsigned char
    {
        //Close combat
        ELECTRIC_CHARGE,
        FLAME_THROWER1, //Turret version
        FLAME_THROWER2, //Grunt version
        BOLTER,     //Short range
        AUTO_CANNON,
        GORILLA_PUNCH,
        PLASMA_RIFLE,
        PULSE_RIFLE,
        //Medium range
        PLASMA_CANNON1,//Knight3 version
        PLASMA_CANNON2,//Knight4/5 version
        PULSE_CANNON,
        HEAVY_BOLTER1, //Grunt3 version
        HEAVY_BOLTER2, //Knight3 version
        HOMING_MISSILE,
        //Long range
        LARGE_MISSILE,
        MULTI_LAUNCHER1,//ballista3
        MULTI_LAUNCHER2,//ballista4
        MULTI_LAUNCHER3,//knight4
        MULTI_LAUNCHER4,//knight5
        MULTI_LAUNCHER5,//sentry3
        MULTI_LAUNCHER6,//sentry4
        MULTI_LAUNCHER7,//ninja5
        //Specials
        SUPERCHARGE_ADVANCED,
        SUPERCHARGE_SUPER,
		BEE_BOMB,
        NUCLEAR_MISSILE,
        VORTEX,
        ION_ORBITAL_CANNON,
        ORB_OF_TREACHERY,
        VIRUS,
		WASP_LIGHT_STING,
		WASP_METAL_STING,
        N_WEAPON_TYPES
    };

    //Variant weapon mounting positions
    enum Mounting : unsigned char
    {
        RIGHT,
        LEFT,
        TOP,
        N_MOUNTINGS
    };

    //The weapon mounting combinations in use
    //L/R/T stand for LEFT RIGHT TOP
    enum WeaponCombo : unsigned char
    {
        L_BOLTER,
        T_ION_ORBITAL_CANNON,
        L_ELECTRIC_CHARGE,
        R_BOLTER,
        T_WASP_LIGHT_STING,
        R_ELECTRIC_CHARGE,
        LR_PULSE_RIFLE_X2,
        L_PULSE_RIFLE_R_SUPERCHARGE_ADVANCED,
        T_WASP_METAL_STING,
        LR_PLASMA_RIFLE_X2,
        T_BEE_BOMB,
        L_MULTI_LAUNCHER5_R_SUPERCHARGE_SUPER,
        L_MULTI_LAUNCHER5_R_TREACHERY,
        LR_PULSE_CANNON_X2,
        L_PULSE_CANNON_R_SUPERCHARGE_SUPER,
        L_PULSE_CANNON_R_VIRUS,
        L_FLAME_THROWER2,
        LR_HEAVY_BOLTER1_X2,
        L_MULTI_LAUNCHER7_T_GORILLA_PUNCH_R_MULTI_LAUNCHER7,
        L_AUTO_CANNON,
        L_PLASMA_RIFLE,
        LR_AUTO_CANNON_X2,
        T_NUCLEAR_MISSILE,
        T_MULTI_LAUNCHER1,
        T_VORTEX,
        LR_MULTI_LAUNCHER2_X2,
        LR_HEAVY_BOLTER2_X2,
        LR_PLASMA_CANNON1_X2,
        LR_MULTI_LAUNCHER3_X2,
        LR_PLASMA_CANNON2_X2,
        LRT_MULTI_LAUNCHER4_X3,
        LRT_PLASMA_CANNON2_X3,
        LR_MULTI_LAUNCHER7_X2,
        T_FLAME_THROWER1,
        LR_MULTI_LAUNCHER5_X2,
        LR_MULTI_LAUNCHER6_X2,
        LR_LARGE_MISSILE_X2,
        N_WEAPON_COMBOS
    };

    //Denotes type of a weapon projectile strike
    enum StrikeType : unsigned char
    {
        ON_OBJECT, //Hits machine, building, ILF, etc
        ON_TERRAIN, //Hits ground
        IN_AIR //hasn't hit anything, but run out of range
    };

	// THese are used for building and artefact demolition
	enum ObjectType : unsigned char { CONSTRUCTION, NOT_CONSTRUCTION };

	typedef struct {
	  ObjectType objectType;
	  ConstructionType constructionType;
	  int subType;
	  int level;
	} DemolitionType;

	typedef int					BuildingMaterialUnits;
	typedef int					ArmourUnits;
	typedef int					HitPointUnits;
	typedef int					ResearchUnits;
	typedef PhysRelativeTime	RepairRateUnits;
	typedef int					SizeUnits;
	typedef int					MineralGrade;

    enum FinalState : unsigned char { AT_REST, IN_MOTION };
	enum AnimationIds : unsigned char  {CONSTRUCTION_WORKING=1, CONSTRUCTION_CONSTRUCTING, MACHINE_LOCOMOTION, CONSTRUCTION_NOT_WORKING };
	enum EntityClientData : unsigned char { ECD_NODATA=0, ECD_DONTAPPLYANIMATION };
};

//Added missing operator
 inline MachPhys::Race& operator++(MachPhys::Race& race)
 {
     return race = _STATIC_CAST(MachPhys::Race, (_STATIC_CAST(unsigned char, race)+1));
 }

ostream& operator <<( ostream& o, MachPhys::Race race );
ostream& operator <<( ostream& o, MachPhys::MachineType type );
ostream& operator <<( ostream& o, MachPhys::AggressorSubType type );
ostream& operator <<( ostream& o, MachPhys::AdministratorSubType type );
ostream& operator <<( ostream& o, MachPhys::TechnicianSubType type );
ostream& operator <<( ostream& o, MachPhys::ConstructorSubType type );
ostream& operator <<( ostream& o, MachPhys::ConstructionType type );
ostream& operator <<( ostream& o, MachPhys::FactorySubType type );
ostream& operator <<( ostream& o, MachPhys::HardwareLabSubType type );
ostream& operator <<( ostream& o, MachPhys::MissileEmplacementSubType type );
ostream& operator <<( ostream& o, MachPhys::WeaponType type );
ostream& operator <<( ostream& o, MachPhys::Mounting type );
ostream& operator <<( ostream& o, MachPhys::WeaponCombo type );
ostream& operator <<( ostream& o, MachPhys::AnimationIds animId );
ostream& operator <<( ostream& o, MachPhys::FinalState finalState );

enum MachPhysPuffType : unsigned char { PUFF_1, PUFF_2, PUFF_3, PUFF_4, PUFF_5, PUFF_6,
						POD_GREEN,
						POD_PURPLE,
						C_FACTORY_GREEN,
						C_FACTORY_WHITE,
						M_FACTORY_GREEN,
						M_FACTORY_BROWN,
						F_FACTORY_GREEN,
						F_FACTORY_RED,
						C_LAB_BLUE,
						C_LAB_WHITE,
						M_LAB_BLUE,
						M_LAB_BROWN,
						SMELTER_PURPLE,
						SMELTER_RED,
						MINE_YELLOW,
						MINE_BLACK,
						GARRISON_GREEN,
						GARRISON_YELLOW,
						EMPLACEMENT_RED,
						EMPLACEMENT_YELLOW,
						BEACON_RED,
						BEACON_AQUA,
						 };

enum MachPhysFireballType : unsigned char { FIREBALL_1, FIREBALL_2, FIREBALL_3, FIREBALL_4, N_FIREBALLS };

ostream& operator <<( ostream& o, MachPhysPuffType type );
ostream& operator <<( ostream& o, MachPhysFireballType type );

PER_ENUM_PERSISTENT( MachPhys::Race );
PER_ENUM_PERSISTENT( MachPhys::MachineType );
PER_ENUM_PERSISTENT( MachPhys::ConstructionType );
PER_ENUM_PERSISTENT( MachPhys::DoorType );
PER_ENUM_PERSISTENT( MachPhys::AggressorSubType );
PER_ENUM_PERSISTENT( MachPhys::TechnicianSubType );
PER_ENUM_PERSISTENT( MachPhys::ConstructorSubType );
PER_ENUM_PERSISTENT( MachPhys::AdministratorSubType );
PER_ENUM_PERSISTENT( MachPhys::FactorySubType );
PER_ENUM_PERSISTENT( MachPhys::HardwareLabSubType );
PER_ENUM_PERSISTENT( MachPhys::MissileEmplacementSubType );
PER_ENUM_PERSISTENT( MachPhys::LocomotionType );
PER_ENUM_PERSISTENT( MachPhys::WeaponType );
PER_ENUM_PERSISTENT( MachPhys::Mounting );
PER_ENUM_PERSISTENT( MachPhys::WeaponCombo );
PER_ENUM_PERSISTENT( MachPhys::StrikeType );
PER_ENUM_PERSISTENT( MachPhysPuffType );
PER_ENUM_PERSISTENT( MachPhysFireballType );
PER_ENUM_PERSISTENT( MachPhys::AnimationIds );

#endif

/* End MACHPHYS.HPP ***************************************************/
