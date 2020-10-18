
#include "mathex/point2d.hpp"
#include "machgui/actnamid.hpp"
#include "machphys/machphys.hpp"
#include "machphys/artfdata.hpp"
#include "machgui/internal/strings.hpp"
#include "machlog/races.hpp"
#include "machlog/artfacts.hpp"
#include "machlog/actor.hpp"
#include "machlog/machine.hpp"
#include "machlog/constron.hpp"
#include "machlog/canattac.hpp"
#include "gui/restring.hpp"
#include "ctl/vector.hpp"

MachLogActorStringIdRestorer::MachLogActorStringIdRestorer()
{
}

MachLogActorStringIdRestorer::~MachLogActorStringIdRestorer()
{
}

//static
uint MachLogActorStringIdRestorer::stringId( MachActor* pActor )
{
	return stringId( pActor->objectType(), pActor->subType() );
}

//static
uint MachLogActorStringIdRestorer::weaponStringId( MachPhys::WeaponCombo wc )
{
	switch ( wc )
	{
		case MachPhys::L_BOLTER:
			return IDS_L_BOLTER;
		case MachPhys::R_BOLTER:
			return IDS_R_BOLTER;
		case MachPhys::L_ELECTRIC_CHARGE:
			return IDS_L_ELECTRIC_CHARGE;
		case MachPhys::R_ELECTRIC_CHARGE:
			return IDS_R_ELECTRIC_CHARGE;
		case MachPhys::LR_PULSE_RIFLE_X2:
			return IDS_LR_PULSE_RIFLE_X2;
		case MachPhys::LR_PULSE_CANNON_X2:
			return IDS_LR_PULSE_CANNON_X2;
		case MachPhys::L_PULSE_RIFLE_R_SUPERCHARGE_ADVANCED:
			return IDS_L_PULSE_RIFLE_R_SUPERCHARGE_ADVANCED;
	    case MachPhys::L_PULSE_CANNON_R_SUPERCHARGE_SUPER:
			return IDS_L_PULSE_CANNON_R_SUPERCHARGE_SUPER;
		case MachPhys::L_MULTI_LAUNCHER5_R_SUPERCHARGE_SUPER:
			return IDS_L_HOMING_R_SUPERCHARGE_SUPER;
		case MachPhys::LR_PLASMA_RIFLE_X2:
			return IDS_L_PLASMA_RIFLE_R_PULSE_RIFLE;
		case MachPhys::L_PLASMA_RIFLE:
			return IDS_L_PLASMA_RIFLE;
		case MachPhys::LR_PLASMA_CANNON1_X2:
			return IDS_LR_PLASMA_CANNON1_X2;
		case MachPhys::LR_PLASMA_CANNON2_X2:
			return IDS_LR_PLASMA_CANNON2_X2;
		case MachPhys::LRT_PLASMA_CANNON2_X3:
			return IDS_LRT_PLASMA_CANNON2_X3;
		case MachPhys::L_MULTI_LAUNCHER5_R_TREACHERY:
			return IDS_L_HOMING_R_TREACHERY;
		case MachPhys::L_PULSE_CANNON_R_VIRUS:
			return IDS_L_PULSE_CANNON_R_VIRUS;
		case MachPhys::T_VORTEX:
			return IDS_T_VORTEX;
		case MachPhys::L_FLAME_THROWER2:
			return IDS_L_FLAME_THROWER2;
		case MachPhys::T_FLAME_THROWER1:
			return IDS_T_FLAME_THROWER1;
		case MachPhys::LR_MULTI_LAUNCHER7_X2:
			return IDS_LR_MULTI_LAUNCHER7_X2;
		case MachPhys::LR_MULTI_LAUNCHER5_X2:
			return IDS_LR_MULTI_LAUNCHER5_X2;
		case MachPhys::LR_MULTI_LAUNCHER6_X2:
			return IDS_LR_MULTI_LAUNCHER6_X2;
		case MachPhys::LR_MULTI_LAUNCHER2_X2:
			return IDS_LR_MULTI_LAUNCHER2_X2;
		case MachPhys::LR_MULTI_LAUNCHER3_X2:
			return IDS_LR_MULTI_LAUNCHER3_X2;
		case MachPhys::LRT_MULTI_LAUNCHER4_X3:
			return IDS_LRT_MULTI_LAUNCHER4_X3;
		case MachPhys::T_MULTI_LAUNCHER1:
			return IDS_T_MULTI_LAUNCHER1;
		case MachPhys::LR_HEAVY_BOLTER2_X2:
			return IDS_LR_HEAVY_BOLTER2_X2;
		case MachPhys::LR_HEAVY_BOLTER1_X2:
			return IDS_LR_HEAVY_BOLTER1_X2;
		case MachPhys::L_AUTO_CANNON:
			return IDS_L_AUTO_CANNON;
		case MachPhys::LR_AUTO_CANNON_X2:
			return IDS_LR_AUTO_CANNON_X2;
		case MachPhys::LR_LARGE_MISSILE_X2:
			return IDS_LR_LARGE_MISSILE_X2;
	    case MachPhys::T_ION_ORBITAL_CANNON:
			return IDS_T_ION_ORBITAL_CANNON;
		case MachPhys::L_MULTI_LAUNCHER7_T_GORILLA_PUNCH_R_MULTI_LAUNCHER7:
			return IDS_L_MULTI_LAUNCHER7_T_GORILLA_PUNCH_R_MULTI_LAUNCHER7;
		case MachPhys::T_NUCLEAR_MISSILE:
			return IDS_T_NUCLEAR_MISSILE;
		case MachPhys::T_WASP_LIGHT_STING:
			return IDS_T_WASP_LIGHT_STING;
		case MachPhys::T_WASP_METAL_STING:
			return IDS_T_WASP_METAL_STING;
		case MachPhys::T_BEE_BOMB:
			return IDS_T_BEE_BOMB;
		case MachPhys::N_WEAPON_COMBOS:
			return IDS_N_WEAPON_COMBOS;
		default:
			break;
	}
	ASSERT( false, "Failed to find weapon combo string id" );
	return 0;
}

//static
uint MachLogActorStringIdRestorer::stringId( MachLog::ObjectType objType, size_t subType )
{
	switch ( objType )
	{
		// Machines
		case MachLog::ADMINISTRATOR:
			switch( (MachPhys::AdministratorSubType)subType )
		    {
		        case MachPhys::BOSS:
		        	return IDS_ADMINISTRATOR_BOSS;
		        case MachPhys::OVERSEER:
					return IDS_ADMINISTRATOR_OVERSEER;
		        case MachPhys::COMMANDER:
					return IDS_ADMINISTRATOR_COMMANDER;
				DEFAULT_ASSERT_BAD_CASE( subType );
		    }
		case MachLog::AGGRESSOR:
		    switch( (MachPhys::AggressorSubType)subType )
		    {
		        case MachPhys::GRUNT:
					return IDS_AGGRESSOR_GRUNT;
		        case MachPhys::ASSASSIN:
					return IDS_AGGRESSOR_ASSASSIN;
		        case MachPhys::BALLISTA:
					return IDS_AGGRESSOR_BALLISTA;
		        case MachPhys::KNIGHT:
					return IDS_AGGRESSOR_KNIGHT;
		        case MachPhys::NINJA:
					return IDS_AGGRESSOR_NINJA;
		        DEFAULT_ASSERT_BAD_CASE( subType );
		    }
		case MachLog::CONSTRUCTOR:
		    switch( (MachPhys::ConstructorSubType)subType )
		    {
		        case MachPhys::DOZER:
					return IDS_CONSTRUCTOR_DOZER;
		        case MachPhys::BUILDER:
					return IDS_CONSTRUCTOR_BUILDER;
				case MachPhys::BEHEMOTH:
					return IDS_CONSTRUCTOR_BEHEMOTH;
				DEFAULT_ASSERT_BAD_CASE( subType );
		    }
		case MachLog::GEO_LOCATOR:
			return IDS_GEO_LOCATOR;
		case MachLog::SPY_LOCATOR:
			return IDS_SPY_LOCATOR;
		case MachLog::TECHNICIAN:
    		switch( (MachPhys::TechnicianSubType)subType )
		    {
		        case MachPhys::LAB_TECH:
					return IDS_TECHNICIAN_LAB_TECH;
		        case MachPhys::TECH_BOY:
					return IDS_TECHNICIAN_TECH_BOY;
		        case MachPhys::BRAIN_BOX:
					return IDS_TECHNICIAN_BRAIN_BOX;
		        DEFAULT_ASSERT_BAD_CASE( subType );
		    }
		case MachLog::RESOURCE_CARRIER:
			return IDS_RESOURCE_CARRIER;
		case MachLog::APC:
			return IDS_APC;

		// Constructions
        case MachLog::POD:
			return IDS_POD;
	    case MachLog::MISSILE_EMPLACEMENT:
		    switch( (MachPhys::MissileEmplacementSubType)subType )
		    {
		        case MachPhys::TURRET:
					return IDS_MISSILE_EMPLACEMENT_TURRET;
		        case MachPhys::SENTRY:
					return IDS_MISSILE_EMPLACEMENT_SENTRY;
		        case MachPhys::LAUNCHER:
					return IDS_MISSILE_EMPLACEMENT_LAUNCHER;
		        case MachPhys::ICBM:
					return IDS_MISSILE_EMPLACEMENT_ICBM;
		        DEFAULT_ASSERT_BAD_CASE( subType );
		    }
    	case MachLog::FACTORY:
		    switch( (MachPhys::FactorySubType)subType )
		    {
		        case MachPhys::CIVILIAN:
					return IDS_FACTORY_CIVILIAN;
		        case MachPhys::MILITARY:
					return IDS_FACTORY_MILITARY;
		        DEFAULT_ASSERT_BAD_CASE( subType );
		    }
        case MachLog::HARDWARE_LAB:
		    switch( (MachPhys::HardwareLabSubType)subType )
		    {
		        case MachPhys::LAB_CIVILIAN:
					return IDS_HARDWARE_LAB_CIVILIAN;
		        case MachPhys::LAB_MILITARY:
					return IDS_HARDWARE_LAB_MILITARY;
		        DEFAULT_ASSERT_BAD_CASE( subType );
		    }
	    case MachLog::SMELTER:
			return IDS_SMELTER;
	    case MachLog::MINE:
			return IDS_MINE;
    	case MachLog::GARRISON:
			return IDS_GARRISON;
		case MachLog::BEACON:
			return IDS_BEACON;
		case MachLog::DEBRIS:
			return IDS_DEBRIS;
		case MachLog::LAND_MINE:
			return IDS_LAND_MINE;
		case MachLog::ORE_HOLOGRAPH:
			return IDS_ORE_HOLOGRAPH;
		case MachLog::ARTEFACT:
			return MachLogRaces::instance().artefacts().artefactData( subType ).resourceStringId();
    }
	ASSERT_INFO( objType );
	ASSERT_INFO( subType );
	ASSERT( false, "Failed to get string ID" );
	return 0;
}

//static
bool MachLogActorStringIdRestorer::isExceptionToRule( MachLog::ObjectType objType, size_t subType, MachPhys::WeaponCombo wc, int level, uint* pStringId )
{
	switch ( objType )
	{
		// Machines
		case MachLog::ADMINISTRATOR:
			switch( (MachPhys::AdministratorSubType)subType )
		    {
		        case MachPhys::OVERSEER:
					if ( level == 2 and wc == MachPhys::L_PULSE_RIFLE_R_SUPERCHARGE_ADVANCED )
					{
						*pStringId = IDS_EXCEPT_OVERSEER2HEAL;
						return true;
					}
					else if ( level == 3 and wc == MachPhys::L_PULSE_RIFLE_R_SUPERCHARGE_ADVANCED )
					{
						*pStringId = IDS_EXCEPT_OVERSEER3HEAL;
						return true;
					}
					break;
		        case MachPhys::COMMANDER:
					if ( level == 4 and wc == MachPhys::L_MULTI_LAUNCHER5_R_TREACHERY )
					{
						*pStringId = IDS_EXCEPT_COMMANDER4TREACH;
						return true;
					}
					else if ( level == 4 and wc == MachPhys::L_MULTI_LAUNCHER5_R_SUPERCHARGE_SUPER )
					{
						*pStringId = IDS_EXCEPT_COMMANDER4HEAL;
						return true;
					}
					else if ( level == 5 and wc == MachPhys::L_PULSE_CANNON_R_SUPERCHARGE_SUPER )
					{
						*pStringId = IDS_EXCEPT_COMMANDER5HEAL;
						return true;
					}
					else if ( level == 5 and wc == MachPhys::L_PULSE_CANNON_R_VIRUS )
					{
						*pStringId = IDS_EXCEPT_COMMANDER5VIRUS;
						return true;
					}
					break;
		        case MachPhys::BOSS:
					break;
		        DEFAULT_ASSERT_BAD_CASE( subType );
		    }
		case MachLog::AGGRESSOR:
		    switch( (MachPhys::AggressorSubType)subType )
		    {
		        case MachPhys::GRUNT:
					if ( level == 2 and wc == MachPhys::L_FLAME_THROWER2 )
					{
						*pStringId = IDS_EXCEPT_GRUNT2FLAME;
						return true;
					}
					break;
		        case MachPhys::BALLISTA:
					if ( level == 3 and wc == MachPhys::T_VORTEX )
					{
						*pStringId = IDS_EXCEPT_BALLISTA3VORTEX;
						return true;
					}
					break;
		        case MachPhys::KNIGHT:
		        case MachPhys::NINJA:
		        case MachPhys::ASSASSIN:
					break;
		        DEFAULT_ASSERT_BAD_CASE( subType );
		    }
		case MachLog::CONSTRUCTOR:
		case MachLog::GEO_LOCATOR:
		case MachLog::SPY_LOCATOR:
		case MachLog::TECHNICIAN:
		case MachLog::RESOURCE_CARRIER:
		case MachLog::APC:
			break;
		// Constructions
	    case MachLog::MISSILE_EMPLACEMENT:
		    switch( (MachPhys::MissileEmplacementSubType)subType )
		    {
		        case MachPhys::TURRET:
					if ( level == 1 )
					{
						*pStringId = IDS_EXCEPT_MISSILE_EMPLACEMENT1FLAME;
						return true;
					}
					break;
		        case MachPhys::SENTRY:
					if ( level == 3 )
					{
						*pStringId = IDS_EXCEPT_MISSILE_EMPLACEMENT3SENTRY;
						return true;
					}
					else if ( level == 4 )
					{
						*pStringId = IDS_EXCEPT_MISSILE_EMPLACEMENT4SENTRY;
						return true;
					}
					break;
		        case MachPhys::LAUNCHER:
					*pStringId = IDS_EXCEPT_MISSILE_EMPLACEMENT4LAUNCHER;
				    return true;
		        case MachPhys::ICBM:
					*pStringId = IDS_MISSILE_EMPLACEMENT_ICBM5;
					return true;
		        DEFAULT_ASSERT_BAD_CASE( subType );
		    }
			break;
        case MachLog::POD:
    	case MachLog::FACTORY:
        case MachLog::HARDWARE_LAB:
	    case MachLog::SMELTER:
	    case MachLog::MINE:
    	case MachLog::GARRISON:
		case MachLog::BEACON:
		case MachLog::DEBRIS:
		case MachLog::LAND_MINE:
		case MachLog::ORE_HOLOGRAPH:
		case MachLog::ARTEFACT:
			break;
    }

	return false;
}

//static
bool MachLogActorStringIdRestorer::isExceptionToRule( MachActor* pActor, uint* pStringId )
{
	// Extract hw level from actor
	int level = 0;
	if ( pActor->objectIsMachine() )
	{
		MachLogMachine& machine = pActor->asMachine();
 		level = machine.hwLevel();
	}
	else if ( pActor->objectIsConstruction() )
	{
		MachLogConstruction& construction = pActor->asConstruction();
 		level = construction.level();
	}

	// Extract weapon combo from actor
	if ( pActor->objectIsCanAttack() )
	{
		return isExceptionToRule( pActor->objectType(), pActor->subType(), pActor->asCanAttack().weaponCombo(), level, pStringId );
	}

	return isExceptionToRule( pActor->objectType(), pActor->subType(), MachPhys::N_WEAPON_COMBOS, level, pStringId );
}

//static
string MachLogActorStringIdRestorer::getActorPromptText( MachActor* pActor, uint actorId, uint actorWithWeaponId )
{
	// Extract hw level from actor
	int level = 0;
	if ( pActor->objectIsMachine() )
	{
		MachLogMachine& machine = pActor->asMachine();
 		level = machine.hwLevel();
	}
	else if ( pActor->objectIsConstruction() )
	{
		MachLogConstruction& construction = pActor->asConstruction();
 		level = construction.level();
	}

	// Extract weapon combo from actor
	if ( pActor->objectIsCanAttack() )
	{
		return getActorPromptText( pActor->objectType(), pActor->subType(), pActor->asCanAttack().weaponCombo(), level, actorId, actorWithWeaponId );
	}

	return getActorPromptText( pActor->objectType(), pActor->subType(), MachPhys::N_WEAPON_COMBOS, level, actorId, actorWithWeaponId );
}

//static
string MachLogActorStringIdRestorer::getActorPromptText(	MachLog::ObjectType objType,
															size_t subType,
															MachPhys::WeaponCombo wc,
															int level,
															uint actorStrId,
															uint actorWithWeaponStrId )
{
	GuiResourceString::Id stringId;
	string machName;
	string weaponName;

	if ( MachLogActorStringIdRestorer::isExceptionToRule( objType, subType, wc, level, &stringId ) )
	{}
	else
	{
		stringId = MachLogActorStringIdRestorer::stringId( objType, subType );

	   	stringId += level;

	    //Load the resource string
		GuiResourceString resWeaponName( MachLogActorStringIdRestorer::weaponStringId( wc ) );
		weaponName = resWeaponName.asString();
	}

	GuiResourceString resMachName( stringId );
	machName = resMachName.asString();
	GuiString retVal;

	if ( weaponName.length() == 0 )
	{
		GuiResourceString text( actorStrId, machName );
		retVal = text.asString();
	}
	else
	{
		GuiStrings strings;
        strings.reserve( 2 );
		strings.push_back( machName );
		strings.push_back( weaponName );
		GuiResourceString text( actorWithWeaponStrId, strings );
		retVal = text.asString();
	}

	return retVal;
}



