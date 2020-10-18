/*
 * V M M A N 2. C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/vmman.hpp"
#include "machlog/internal/vminfo.hpp"
#include "machlog/internal/vmail.hpp"
#include "machlog/internal/vmmani.hpp"
#include "machlog/vmdata.hpp"

void MachLogVoiceMailManager::assignMappingGroup1()
{
	CB_MachLogVoiceMailManager_DEPIMPL();

	// ************************************ REGISTERED VOICEMAIL IDs ********************************************

	// ========================================== A G G R E S S O R S ==========================================

	//------------------------------------ G R U N T ---------------------------------------

	registeredIds_.insert( "VID_GRUNT_SELF_DESTRUCT", VID_GRUNT_SELF_DESTRUCT );
	registeredIds_.insert( "VID_GRUNT_BUILT0", VID_GRUNT_BUILT0 );
	registeredIds_.insert( "VID_GRUNT_BUILT1", VID_GRUNT_BUILT1 );
	registeredIds_.insert( "VID_GRUNT_DESTROYED", VID_GRUNT_DESTROYED );
	registeredIds_.insert( "VID_GRUNT_NEAR_DEATH0", VID_GRUNT_NEAR_DEATH0 );
	registeredIds_.insert( "VID_GRUNT_NEAR_DEATH1", VID_GRUNT_NEAR_DEATH1 );
	registeredIds_.insert( "VID_GRUNT_NEAR_DEATH2", VID_GRUNT_NEAR_DEATH2 );
	registeredIds_.insert( "VID_GRUNT_NEAR_DEATH3", VID_GRUNT_NEAR_DEATH3 );
	registeredIds_.insert( "VID_GRUNT_CHANGED_RACE0", VID_GRUNT_CHANGED_RACE0 );
	registeredIds_.insert( "VID_GRUNT_CHANGED_RACE1", VID_GRUNT_CHANGED_RACE1 );
	registeredIds_.insert( "VID_GRUNT_VIRUS_INFECTED", VID_GRUNT_VIRUS_INFECTED );
	registeredIds_.insert( "VID_GRUNT_DAMAGED", VID_GRUNT_DAMAGED );
	registeredIds_.insert( "VID_GRUNT_TASKED0", VID_GRUNT_TASKED0 );
	registeredIds_.insert( "VID_GRUNT_TASKED1", VID_GRUNT_TASKED1 );
	registeredIds_.insert( "VID_GRUNT_TASKED2", VID_GRUNT_TASKED2 );
	registeredIds_.insert( "VID_GRUNT_TASKED3", VID_GRUNT_TASKED3 );
	registeredIds_.insert( "VID_GRUNT_MOVING0", VID_GRUNT_MOVING0 );
	registeredIds_.insert( "VID_GRUNT_MOVING1", VID_GRUNT_MOVING1 );
	registeredIds_.insert( "VID_GRUNT_MOVING2", VID_GRUNT_MOVING2 );
	registeredIds_.insert( "VID_GRUNT_MOVING3", VID_GRUNT_MOVING3 );
	registeredIds_.insert( "VID_GRUNT_SELECTED0", VID_GRUNT_SELECTED0 );
	registeredIds_.insert( "VID_GRUNT_SELECTED1", VID_GRUNT_SELECTED1 );
	registeredIds_.insert( "VID_GRUNT_SELECTED2", VID_GRUNT_SELECTED2 );
	registeredIds_.insert( "VID_GRUNT_TARGET_ENEMY0", VID_GRUNT_TARGET_ENEMY0 );
	registeredIds_.insert( "VID_GRUNT_TARGET_ENEMY1", VID_GRUNT_TARGET_ENEMY1 );
	registeredIds_.insert( "VID_GRUNT_TARGET_ENEMY2", VID_GRUNT_TARGET_ENEMY2 );
	registeredIds_.insert( "VID_GRUNT_TARGET_ENEMY3", VID_GRUNT_TARGET_ENEMY3 );
	registeredIds_.insert( "VID_GRUNT_TARGET_ENEMY4", VID_GRUNT_TARGET_ENEMY4 );
	registeredIds_.insert( "VID_GRUNT_RECYCLE", VID_GRUNT_RECYCLE );

	//----------------------------------- A S S A S S I N  ---------------------------------------

	registeredIds_.insert( "VID_ASSASSIN_SELF_DESTRUCT", VID_ASSASSIN_SELF_DESTRUCT );
	registeredIds_.insert( "VID_ASSASSIN_BUILT", VID_ASSASSIN_BUILT );
	registeredIds_.insert( "VID_ASSASSIN_DESTROYED", VID_ASSASSIN_DESTROYED );
	registeredIds_.insert( "VID_ASSASSIN_NEAR_DEATH0", VID_ASSASSIN_NEAR_DEATH0 );
	registeredIds_.insert( "VID_ASSASSIN_NEAR_DEATH1", VID_ASSASSIN_NEAR_DEATH1 );
	registeredIds_.insert( "VID_ASSASSIN_CHANGED_RACE0", VID_ASSASSIN_CHANGED_RACE0 );
	registeredIds_.insert( "VID_ASSASSIN_CHANGED_RACE1", VID_ASSASSIN_CHANGED_RACE1 );
	registeredIds_.insert( "VID_ASSASSIN_VIRUS_INFECTED", VID_ASSASSIN_VIRUS_INFECTED );
	registeredIds_.insert( "VID_ASSASSIN_DAMAGED", VID_ASSASSIN_DAMAGED );
	registeredIds_.insert( "VID_ASSASSIN_TASKED0", VID_ASSASSIN_TASKED0 );
	registeredIds_.insert( "VID_ASSASSIN_TASKED1", VID_ASSASSIN_TASKED1 );
	registeredIds_.insert( "VID_ASSASSIN_MOVING0", VID_ASSASSIN_MOVING0 );
	registeredIds_.insert( "VID_ASSASSIN_MOVING1", VID_ASSASSIN_MOVING1 );
	registeredIds_.insert( "VID_ASSASSIN_SELECTED0", VID_ASSASSIN_SELECTED0 );
	registeredIds_.insert( "VID_ASSASSIN_SELECTED1", VID_ASSASSIN_SELECTED1 );
	registeredIds_.insert( "VID_ASSASSIN_SELECTED2", VID_ASSASSIN_SELECTED2 );
	registeredIds_.insert( "VID_ASSASSIN_TARGET_ENEMY0", VID_ASSASSIN_TARGET_ENEMY0 );
	registeredIds_.insert( "VID_ASSASSIN_TARGET_ENEMY1", VID_ASSASSIN_TARGET_ENEMY1 );
	registeredIds_.insert( "VID_ASSASSIN_TARGET_ENEMY2", VID_ASSASSIN_TARGET_ENEMY2 );
	registeredIds_.insert( "VID_ASSASSIN_RECYCLE", VID_ASSASSIN_RECYCLE );

	//----------------------------------- K N I G H T ---------------------------------------

	registeredIds_.insert( "VID_KNIGHT_TARGET_ENEMY0", VID_KNIGHT_TARGET_ENEMY0 );
	registeredIds_.insert( "VID_KNIGHT_TARGET_ENEMY1", VID_KNIGHT_TARGET_ENEMY1 );
	registeredIds_.insert( "VID_KNIGHT_BUILT", VID_KNIGHT_BUILT );
	registeredIds_.insert( "VID_KNIGHT_NEAR_DEATH", VID_KNIGHT_NEAR_DEATH );
	registeredIds_.insert( "VID_KNIGHT_DAMAGED0", VID_KNIGHT_DAMAGED0 );
	registeredIds_.insert( "VID_KNIGHT_DAMAGED1", VID_KNIGHT_DAMAGED1 );
	registeredIds_.insert( "VID_KNIGHT_MOVING0", VID_KNIGHT_MOVING0 );
	registeredIds_.insert( "VID_KNIGHT_MOVING1", VID_KNIGHT_MOVING1 );
	registeredIds_.insert( "VID_KNIGHT_MOVING2", VID_KNIGHT_MOVING2 );
	registeredIds_.insert( "VID_KNIGHT_MOVING3", VID_KNIGHT_MOVING3 );
	registeredIds_.insert( "VID_KNIGHT_SELF_DESTRUCT", VID_KNIGHT_SELF_DESTRUCT );
	registeredIds_.insert( "VID_KNIGHT_SELECTED0", VID_KNIGHT_SELECTED0 );
	registeredIds_.insert( "VID_KNIGHT_SELECTED1", VID_KNIGHT_SELECTED1 );
	registeredIds_.insert( "VID_KNIGHT_TASKED0", VID_KNIGHT_TASKED0 );
	registeredIds_.insert( "VID_KNIGHT_TASKED1", VID_KNIGHT_TASKED1 );
	registeredIds_.insert( "VID_KNIGHT_CHANGED_RACE", VID_KNIGHT_CHANGED_RACE );
	registeredIds_.insert( "VID_KNIGHT_RECYCLE", VID_KNIGHT_RECYCLE );

	//----------------------------------- B A L L I S T A  ---------------------------------------

	registeredIds_.insert( "VID_BALLISTA_SELF_DESTRUCT", VID_BALLISTA_SELF_DESTRUCT );
	registeredIds_.insert( "VID_BALLISTA_CRUSADER_BUILT", VID_BALLISTA_CRUSADER_BUILT );
	registeredIds_.insert( "VID_BALLISTA_ERADICATOR_BUILT", VID_BALLISTA_ERADICATOR_BUILT );
	registeredIds_.insert( "VID_BALLISTA_GOLIATH_BUILT", VID_BALLISTA_GOLIATH_BUILT );
	registeredIds_.insert( "VID_BALLISTA_DESTROYED", VID_BALLISTA_DESTROYED );
	registeredIds_.insert( "VID_BALLISTA_NEAR_DEATH", VID_BALLISTA_NEAR_DEATH );
	registeredIds_.insert( "VID_BALLISTA_CHANGED_RACE0", VID_BALLISTA_CHANGED_RACE0 );
	registeredIds_.insert( "VID_BALLISTA_CHANGED_RACE1", VID_BALLISTA_CHANGED_RACE1 );
	registeredIds_.insert( "VID_BALLISTA_VIRUS_INFECTED", VID_BALLISTA_VIRUS_INFECTED );
	registeredIds_.insert( "VID_BALLISTA_DAMAGED", VID_BALLISTA_DAMAGED );
	registeredIds_.insert( "VID_BALLISTA_TASKED0", VID_BALLISTA_TASKED0 );
	registeredIds_.insert( "VID_BALLISTA_TASKED1", VID_BALLISTA_TASKED1 );
	registeredIds_.insert( "VID_BALLISTA_TASKED2", VID_BALLISTA_TASKED2 );
	registeredIds_.insert( "VID_BALLISTA_TASKED3", VID_BALLISTA_TASKED3 );
	registeredIds_.insert( "VID_BALLISTA_MOVING0", VID_BALLISTA_MOVING0 );
	registeredIds_.insert( "VID_BALLISTA_MOVING1", VID_BALLISTA_MOVING1 );
	registeredIds_.insert( "VID_BALLISTA_MOVING2", VID_BALLISTA_MOVING2 );
	registeredIds_.insert( "VID_BALLISTA_SELECTED0", VID_BALLISTA_SELECTED0 );
	registeredIds_.insert( "VID_BALLISTA_SELECTED1", VID_BALLISTA_SELECTED1 );
	registeredIds_.insert( "VID_BALLISTA_TARGET_ENEMY0", VID_BALLISTA_TARGET_ENEMY0 );
	registeredIds_.insert( "VID_BALLISTA_TARGET_ENEMY1", VID_BALLISTA_TARGET_ENEMY1 );
	registeredIds_.insert( "VID_BALLISTA_TARGET_ENEMY2", VID_BALLISTA_TARGET_ENEMY2 );
	registeredIds_.insert( "VID_BALLISTA_TARGET_ENEMY3", VID_BALLISTA_TARGET_ENEMY3 );
	registeredIds_.insert( "VID_BALLISTA_RECYCLE", VID_BALLISTA_RECYCLE );

	// ballista-specific
	registeredIds_.insert( "VID_BALLISTA_FIRE_VORTEX", VID_BALLISTA_FIRE_VORTEX );



	//----------------------------------- N I N J A ---------------------------------------

	registeredIds_.insert( "VID_NINJA_SELF_DESTRUCT", VID_NINJA_SELF_DESTRUCT );
	registeredIds_.insert( "VID_NINJA_BUILT", VID_NINJA_BUILT );
	registeredIds_.insert( "VID_NINJA_DESTROYED", VID_NINJA_DESTROYED );
	registeredIds_.insert( "VID_NINJA_NEAR_DEATH0", VID_NINJA_NEAR_DEATH0 );
	registeredIds_.insert( "VID_NINJA_CHANGED_RACE", VID_NINJA_CHANGED_RACE );
	registeredIds_.insert( "VID_NINJA_DAMAGED", VID_NINJA_DAMAGED );
	registeredIds_.insert( "VID_NINJA_NEAR_DEATH1", VID_NINJA_NEAR_DEATH1 );
	registeredIds_.insert( "VID_NINJA_TASKED0", VID_NINJA_TASKED0 );
	registeredIds_.insert( "VID_NINJA_TASKED1", VID_NINJA_TASKED1 );
	registeredIds_.insert( "VID_NINJA_MOVING0", VID_NINJA_MOVING0 );
	registeredIds_.insert( "VID_NINJA_MOVING1", VID_NINJA_MOVING1 );
	registeredIds_.insert( "VID_NINJA_MOVING2", VID_NINJA_MOVING2 );
	registeredIds_.insert( "VID_NINJA_SELECTED0", VID_NINJA_SELECTED0 );
	registeredIds_.insert( "VID_NINJA_SELECTED1", VID_NINJA_SELECTED1 );
	registeredIds_.insert( "VID_NINJA_SELECTED2", VID_NINJA_SELECTED2 );
	registeredIds_.insert( "VID_NINJA_TARGET_ENEMY0", VID_NINJA_TARGET_ENEMY0 );
	registeredIds_.insert( "VID_NINJA_TARGET_ENEMY1", VID_NINJA_TARGET_ENEMY1 );
	registeredIds_.insert( "VID_NINJA_TARGET_ENEMY2", VID_NINJA_TARGET_ENEMY2 );
	registeredIds_.insert( "VID_NINJA_TARGET_ENEMY3", VID_NINJA_TARGET_ENEMY3 );
	registeredIds_.insert( "VID_NINJA_RECYCLE", VID_NINJA_RECYCLE );

	// ninja-specific

	registeredIds_.insert( "VID_NINJA_SMASH0", VID_NINJA_SMASH0 );
	registeredIds_.insert( "VID_NINJA_SMASH1", VID_NINJA_SMASH1 );

	// ====================================== A D M I N I S T R A T O R S ======================================

	//--------------------------------------- B O S S ---------------------------------------

	registeredIds_.insert( "VID_BOSS_TARGET_ENEMY0", VID_BOSS_TARGET_ENEMY0 );
	registeredIds_.insert( "VID_BOSS_TARGET_ENEMY1", VID_BOSS_TARGET_ENEMY1 );
	registeredIds_.insert( "VID_BOSS_NEAR_DEATH0", VID_BOSS_NEAR_DEATH0 );
	registeredIds_.insert( "VID_BOSS_NEAR_DEATH1", VID_BOSS_NEAR_DEATH1 );
	registeredIds_.insert( "VID_BOSS_BEE_BUILT", VID_BOSS_BEE_BUILT );
	registeredIds_.insert( "VID_BOSS_DAMAGED0", VID_BOSS_DAMAGED0 );
	registeredIds_.insert( "VID_BOSS_DAMAGED1", VID_BOSS_DAMAGED1 );
	registeredIds_.insert( "VID_BOSS_MOVING0", VID_BOSS_MOVING0 );
	registeredIds_.insert( "VID_BOSS_MOVING1", VID_BOSS_MOVING1 );
	registeredIds_.insert( "VID_BOSS_MOVING2", VID_BOSS_MOVING2 );
	registeredIds_.insert( "VID_BOSS_MOVING3", VID_BOSS_MOVING3 );
	registeredIds_.insert( "VID_BOSS_SELF_DESTRUCT", VID_BOSS_SELF_DESTRUCT );
	registeredIds_.insert( "VID_BOSS_SELECTED0", VID_BOSS_SELECTED0 );
	registeredIds_.insert( "VID_BOSS_SELECTED1", VID_BOSS_SELECTED1 );
	registeredIds_.insert( "VID_BOSS_TASKED0", VID_BOSS_TASKED0 );
	registeredIds_.insert( "VID_BOSS_TASKED1", VID_BOSS_TASKED1 );
	registeredIds_.insert( "VID_BOSS_TASKED2", VID_BOSS_TASKED2 );
	registeredIds_.insert( "VID_BOSS_TASKED3", VID_BOSS_TASKED3 );
	registeredIds_.insert( "VID_BOSS_CHANGED_RACE", VID_BOSS_CHANGED_RACE );
	registeredIds_.insert( "VID_BOSS_WASP_BUILT", VID_BOSS_WASP_BUILT );

	//----------------------------------- O V E R S E E R -------------------------------------

	registeredIds_.insert( "VID_OVERSEER_SELF_DESTRUCT", VID_OVERSEER_SELF_DESTRUCT );
	registeredIds_.insert( "VID_OVERSEER_BUILT", VID_OVERSEER_BUILT );
	registeredIds_.insert( "VID_OVERSEER_NEAR_DEATH", VID_OVERSEER_NEAR_DEATH );
	registeredIds_.insert( "VID_OVERSEER_CHANGED_RACE0", VID_OVERSEER_CHANGED_RACE0 );
	registeredIds_.insert( "VID_OVERSEER_CHANGED_RACE1", VID_OVERSEER_CHANGED_RACE1 );
	registeredIds_.insert( "VID_OVERSEER_DAMAGED", VID_OVERSEER_DAMAGED );
	registeredIds_.insert( "VID_OVERSEER_TASKED", VID_OVERSEER_TASKED );
	registeredIds_.insert( "VID_OVERSEER_MOVING", VID_OVERSEER_MOVING );
	registeredIds_.insert( "VID_OVERSEER_SELECTED0", VID_OVERSEER_SELECTED0 );
	registeredIds_.insert( "VID_OVERSEER_SELECTED1", VID_OVERSEER_SELECTED1 );
	registeredIds_.insert( "VID_OVERSEER_TARGET_ENEMY0", VID_OVERSEER_TARGET_ENEMY0 );
	registeredIds_.insert( "VID_OVERSEER_TARGET_ENEMY1", VID_OVERSEER_TARGET_ENEMY1 );

	registeredIds_.insert( "VID_OVERSEER_HEAL_TARGET", VID_OVERSEER_HEAL_TARGET );
	registeredIds_.insert( "VID_OVERSEER_HEALING_COMPLETE", VID_OVERSEER_HEALING_COMPLETE );

	//----------------------------------- C O M M A N D E R -------------------------------------

	registeredIds_.insert( "VID_COMMANDER_TARGET_ENEMY0", VID_COMMANDER_TARGET_ENEMY0 );
	registeredIds_.insert( "VID_COMMANDER_TARGET_ENEMY1", VID_COMMANDER_TARGET_ENEMY1 );
	registeredIds_.insert( "VID_COMMANDER_BUILT", VID_COMMANDER_BUILT );
	registeredIds_.insert( "VID_COMMANDER_HEALING_COMPLETE", VID_COMMANDER_HEALING_COMPLETE );
	registeredIds_.insert( "VID_COMMANDER_NEAR_DEATH", VID_COMMANDER_NEAR_DEATH );
	registeredIds_.insert( "VID_COMMANDER_HEAL_TARGET", VID_COMMANDER_HEAL_TARGET );
	registeredIds_.insert( "VID_COMMANDER_DAMAGED", VID_COMMANDER_DAMAGED );
	registeredIds_.insert( "VID_COMMANDER_MOVING0", VID_COMMANDER_MOVING0 );
	registeredIds_.insert( "VID_COMMANDER_MOVING1", VID_COMMANDER_MOVING1 );
	registeredIds_.insert( "VID_COMMANDER_SELF_DESTRUCT", VID_COMMANDER_SELF_DESTRUCT );
	registeredIds_.insert( "VID_COMMANDER_SELECTED0", VID_COMMANDER_SELECTED0 );
	registeredIds_.insert( "VID_COMMANDER_SELECTED1", VID_COMMANDER_SELECTED1 );
	registeredIds_.insert( "VID_COMMANDER_TASKED0", VID_COMMANDER_TASKED0 );
	registeredIds_.insert( "VID_COMMANDER_TASKED1", VID_COMMANDER_TASKED1 );
	registeredIds_.insert( "VID_COMMANDER_CHANGED_RACE", VID_COMMANDER_CHANGED_RACE );
	registeredIds_.insert( "VID_COMMANDER_TREACHERY_TARGET", VID_COMMANDER_TREACHERY_TARGET );

	// ====================================== T E C H N I C I A N ======================================

	//----------------------------------- T E C H N I C I A N -------------------------------------

	registeredIds_.insert( "VID_TECHNICIAN_SELF_DESTRUCT", VID_TECHNICIAN_SELF_DESTRUCT );
	registeredIds_.insert( "VID_TECHNICIAN_BUILT", VID_TECHNICIAN_BUILT );
	registeredIds_.insert( "VID_TECHNICIAN_DESTROYED", VID_TECHNICIAN_DESTROYED );
	registeredIds_.insert( "VID_TECHNICIAN_NEAR_DEATH", VID_TECHNICIAN_NEAR_DEATH );
	registeredIds_.insert( "VID_TECHNICIAN_CHANGED_RACE0", VID_TECHNICIAN_CHANGED_RACE0 );
	registeredIds_.insert( "VID_TECHNICIAN_CHANGED_RACE1", VID_TECHNICIAN_CHANGED_RACE1 );
	registeredIds_.insert( "VID_TECHNICIAN_VIRUS_INFECTED", VID_TECHNICIAN_VIRUS_INFECTED );
	registeredIds_.insert( "VID_TECHNICIAN_DAMAGED", VID_TECHNICIAN_DAMAGED );
	registeredIds_.insert( "VID_TECHNICIAN_TASKED0", VID_TECHNICIAN_TASKED0 );
	registeredIds_.insert( "VID_TECHNICIAN_TASKED1", VID_TECHNICIAN_TASKED1 );
	registeredIds_.insert( "VID_TECHNICIAN_MOVING0", VID_TECHNICIAN_MOVING0 );
	registeredIds_.insert( "VID_TECHNICIAN_MOVING1", VID_TECHNICIAN_MOVING1 );
	registeredIds_.insert( "VID_TECHNICIAN_MOVING2", VID_TECHNICIAN_MOVING2 );
	registeredIds_.insert( "VID_TECHNICIAN_SELECTED0", VID_TECHNICIAN_SELECTED0 );
	registeredIds_.insert( "VID_TECHNICIAN_SELECTED1", VID_TECHNICIAN_SELECTED1 );
	registeredIds_.insert( "VID_TECHNICIAN_RECYCLE", VID_TECHNICIAN_RECYCLE );

	registeredIds_.insert( "VID_TECHNICIAN_RESEARCH_COMPLETE", VID_TECHNICIAN_RESEARCH_COMPLETE );
	registeredIds_.insert( "VID_TECHNICIAN_RESEARCHING_NEXT", VID_TECHNICIAN_RESEARCHING_NEXT );
	registeredIds_.insert( "VID_TECHNICIAN_RESEARCHING", VID_TECHNICIAN_RESEARCHING );
	registeredIds_.insert( "VID_TECHNICIAN_RESEARCHING_TOUGH_ONE", VID_TECHNICIAN_RESEARCHING_TOUGH_ONE );

	//------------------------------------ E N G I N E E R --------------------------------------

	registeredIds_.insert( "VID_TECHNICIAN_ENGINEER_BUILT", VID_TECHNICIAN_ENGINEER_BUILT );

	//----------------------------------- B R A I N - B O X -------------------------------------

	registeredIds_.insert( "VID_TECHNICIAN_BRAINBOX_BUILT", VID_TECHNICIAN_BRAINBOX_BUILT );
}


void MachLogVoiceMailManager::assignMappingGroup2()
{
	CB_MachLogVoiceMailManager_DEPIMPL();

	// ==================================== G E O - L O C A T O R =======================================

	registeredIds_.insert( "VID_GEO_SELF_DESTRUCT", VID_GEO_SELF_DESTRUCT );
	registeredIds_.insert( "VID_GEO_BUILT", VID_GEO_BUILT );
	registeredIds_.insert( "VID_GEO_DESTROYED", VID_GEO_DESTROYED );
	registeredIds_.insert( "VID_GEO_NEAR_DEATH", VID_GEO_NEAR_DEATH );
	registeredIds_.insert( "VID_GEO_CHANGED_RACE0", VID_GEO_CHANGED_RACE0 );
	registeredIds_.insert( "VID_GEO_CHANGED_RACE1", VID_GEO_CHANGED_RACE1 );
	registeredIds_.insert( "VID_GEO_VIRUS_INFECTED", VID_GEO_VIRUS_INFECTED );
	registeredIds_.insert( "VID_GEO_DAMAGED", VID_GEO_DAMAGED );
	registeredIds_.insert( "VID_GEO_TASKED0", VID_GEO_TASKED0 );
	registeredIds_.insert( "VID_GEO_TASKED1", VID_GEO_TASKED1 );
	registeredIds_.insert( "VID_GEO_MOVING0", VID_GEO_MOVING0 );
	registeredIds_.insert( "VID_GEO_MOVING1", VID_GEO_MOVING1 );
	registeredIds_.insert( "VID_GEO_SELECTED0", VID_GEO_SELECTED0 );
	registeredIds_.insert( "VID_GEO_SELECTED1", VID_GEO_SELECTED1 );
	registeredIds_.insert( "VID_GEO_RECYCLE", VID_GEO_RECYCLE );

	registeredIds_.insert( "VID_GEO_LOCATING", VID_GEO_LOCATING );
	registeredIds_.insert( "VID_GEO_MINERAL_LOCATED", VID_GEO_MINERAL_LOCATED );
	registeredIds_.insert( "VID_GEO_SEARCH_COMPLETE", VID_GEO_SEARCH_COMPLETE );

	// ==================================== S P Y - L O C A T O R =======================================

	registeredIds_.insert( "VID_SPY_SELF_DESTRUCT", VID_SPY_SELF_DESTRUCT );
	registeredIds_.insert( "VID_SPY_BUILT", VID_SPY_BUILT );
	registeredIds_.insert( "VID_SPY_DESTROYED", VID_SPY_DESTROYED );
	registeredIds_.insert( "VID_SPY_NEAR_DEATH", VID_SPY_NEAR_DEATH );
	registeredIds_.insert( "VID_SPY_CHANGED_RACE", VID_SPY_CHANGED_RACE );
	registeredIds_.insert( "VID_SPY_VIRUS_INFECTED", VID_SPY_VIRUS_INFECTED );
	registeredIds_.insert( "VID_SPY_DAMAGED", VID_SPY_DAMAGED );
	registeredIds_.insert( "VID_SPY_TASKED0", VID_SPY_TASKED0 );
	registeredIds_.insert( "VID_SPY_MOVING0", VID_SPY_MOVING0 );
	registeredIds_.insert( "VID_SPY_MOVING1", VID_SPY_MOVING1 );
	registeredIds_.insert( "VID_SPY_SELECTED0", VID_SPY_SELECTED0 );
	registeredIds_.insert( "VID_SPY_RECYCLE", VID_SPY_RECYCLE );

	registeredIds_.insert( "VID_SPY_NO_MORE_MINES", VID_SPY_NO_MORE_MINES );
	registeredIds_.insert( "VID_SPY_RESEARCH_DOWNLOADED", VID_SPY_RESEARCH_DOWNLOADED );
	registeredIds_.insert( "VID_SPY_NEW_DOWNLOAD", VID_SPY_NEW_DOWNLOAD );
	registeredIds_.insert( "VID_SPY_TASKED_LAY_MINE", VID_SPY_TASKED_LAY_MINE );
	registeredIds_.insert( "VID_SPY_TASKED_INFLITRATE", VID_SPY_TASKED_INFLITRATE );

	// ================================ C O N S T R U C T O R S =======================================

	//----------------------------------- D O Z E R -------------------------------------

	registeredIds_.insert( "VID_DOZER_SELF_DESTRUCT", VID_DOZER_SELF_DESTRUCT );
	registeredIds_.insert( "VID_DOZER_BUILT", VID_DOZER_BUILT );
	registeredIds_.insert( "VID_DOZER_DESTROYED", VID_DOZER_DESTROYED );
	registeredIds_.insert( "VID_DOZER_NEAR_DEATH", VID_DOZER_NEAR_DEATH );
	registeredIds_.insert( "VID_DOZER_CHANGED_RACE0", VID_DOZER_CHANGED_RACE0 );
	registeredIds_.insert( "VID_DOZER_CHANGED_RACE1", VID_DOZER_CHANGED_RACE1 );
	registeredIds_.insert( "VID_DOZER_VIRUS_INFECTED", VID_DOZER_VIRUS_INFECTED );
	registeredIds_.insert( "VID_DOZER_DAMAGED", VID_DOZER_DAMAGED );
	registeredIds_.insert( "VID_DOZER_TASKED0", VID_DOZER_TASKED0 );
	registeredIds_.insert( "VID_DOZER_TASKED1", VID_DOZER_TASKED1 );
	registeredIds_.insert( "VID_DOZER_TASKED2", VID_DOZER_TASKED2 );
	registeredIds_.insert( "VID_DOZER_MOVING0", VID_DOZER_MOVING0 );
	registeredIds_.insert( "VID_DOZER_MOVING1", VID_DOZER_MOVING1 );
	registeredIds_.insert( "VID_DOZER_MOVING2", VID_DOZER_MOVING2 );
	registeredIds_.insert( "VID_DOZER_SELECTED0", VID_DOZER_SELECTED0 );
	registeredIds_.insert( "VID_DOZER_SELECTED1", VID_DOZER_SELECTED1 );
	registeredIds_.insert( "VID_DOZER_RECYCLE", VID_DOZER_RECYCLE );

	registeredIds_.insert( "VID_DOZER_BUILDING_COMPLETE", VID_DOZER_BUILDING_COMPLETE );
	registeredIds_.insert( "VID_DOZER_AWAITING_NEW_JOB", VID_DOZER_AWAITING_NEW_JOB );
	registeredIds_.insert( "VID_DOZER_MOVING_TO_NEXT", VID_DOZER_MOVING_TO_NEXT );
	registeredIds_.insert( "VID_DOZER_MOVE_TO_SITE", VID_DOZER_MOVE_TO_SITE );
	registeredIds_.insert( "VID_DOZER_BUILDING_CAPTURED", VID_DOZER_BUILDING_CAPTURED );
	registeredIds_.insert( "VID_DOZER_BUILDING_DECONSTRUCTED", VID_DOZER_BUILDING_DECONSTRUCTED );
	registeredIds_.insert( "VID_DOZER_BUILDING_REPAIRED", VID_DOZER_BUILDING_REPAIRED );

	//--------------------------------- B U I L D E R -------------------------------------

	registeredIds_.insert( "VID_BUILDER_BUILT", VID_BUILDER_BUILT );


	//--------------------------------- B E H E M O T H -------------------------------------

	registeredIds_.insert( "VID_BEHEMOTH_BUILT", VID_BEHEMOTH_BUILT );

	//------------------------------------ T I T A N ----------------------------------------

	registeredIds_.insert( "VID_TITAN_BUILT", VID_TITAN_BUILT );

  	// ================================== T R A N S P O R T E R =======================================

	registeredIds_.insert( "VID_RESOURCE_CARRIER_ALTERNATIVE_DESTINATION", VID_RESOURCE_CARRIER_ALTERNATIVE_DESTINATION );
	registeredIds_.insert( "VID_RESOURCE_CARRIER_NEAR_DEATH", VID_RESOURCE_CARRIER_NEAR_DEATH );
	registeredIds_.insert( "VID_RESOURCE_CARRIER_DAMAGED", VID_RESOURCE_CARRIER_DAMAGED );
	registeredIds_.insert( "VID_RESOURCE_CARRIER_ROUTE_FAILURE0", VID_RESOURCE_CARRIER_ROUTE_FAILURE0 );
	registeredIds_.insert( "VID_RESOURCE_CARRIER_ROUTE_FAILURE1", VID_RESOURCE_CARRIER_ROUTE_FAILURE1 );
	registeredIds_.insert( "VID_RESOURCE_CARRIER_MOVING0", VID_RESOURCE_CARRIER_MOVING0 );
	registeredIds_.insert( "VID_RESOURCE_CARRIER_MOVING1", VID_RESOURCE_CARRIER_MOVING1 );
	registeredIds_.insert( "VID_RESOURCE_CARRIER_MOVING2", VID_RESOURCE_CARRIER_MOVING2 );
	registeredIds_.insert( "VID_RESOURCE_CARRIER_SCAVENGER_BUILT", VID_RESOURCE_CARRIER_SCAVENGER_BUILT );
	registeredIds_.insert( "VID_RESOURCE_CARRIER_SCAVENGING", VID_RESOURCE_CARRIER_SCAVENGING );
	registeredIds_.insert( "VID_RESOURCE_CARRIER_SELF_DESTRUCT", VID_RESOURCE_CARRIER_SELF_DESTRUCT );
	registeredIds_.insert( "VID_RESOURCE_CARRIER_SELECTED0", VID_RESOURCE_CARRIER_SELECTED0 );
	registeredIds_.insert( "VID_RESOURCE_CARRIER_SELECTED1", VID_RESOURCE_CARRIER_SELECTED1 );
	registeredIds_.insert( "VID_RESOURCE_CARRIER_TASKED0", VID_RESOURCE_CARRIER_TASKED0 );
	registeredIds_.insert( "VID_RESOURCE_CARRIER_TASKED1", VID_RESOURCE_CARRIER_TASKED1 );
	registeredIds_.insert( "VID_RESOURCE_CARRIER_BUILT", VID_RESOURCE_CARRIER_BUILT );
	registeredIds_.insert( "VID_RESOURCE_CARRIER_TRANSPORTING", VID_RESOURCE_CARRIER_TRANSPORTING );
	registeredIds_.insert( "VID_RESOURCE_CARRIER_CHANGED_RACE0", VID_RESOURCE_CARRIER_CHANGED_RACE0 );
	registeredIds_.insert( "VID_RESOURCE_CARRIER_CHANGED_RACE1", VID_RESOURCE_CARRIER_CHANGED_RACE1 );

	// ======================================== A P C ===========================================

	registeredIds_.insert( "VID_APC_SELF_DESTRUCT", VID_APC_SELF_DESTRUCT );
	registeredIds_.insert( "VID_APC_BUILT", VID_APC_BUILT );
	registeredIds_.insert( "VID_APC_DESTROYED", VID_APC_DESTROYED );
	registeredIds_.insert( "VID_APC_NEAR_DEATH", VID_APC_NEAR_DEATH );
	registeredIds_.insert( "VID_APC_CHANGED_RACE0", VID_APC_CHANGED_RACE0 );
	registeredIds_.insert( "VID_APC_CHANGED_RACE1", VID_APC_CHANGED_RACE1 );
	registeredIds_.insert( "VID_APC_VIRUS_INFECTED", VID_APC_VIRUS_INFECTED );
	registeredIds_.insert( "VID_APC_DAMAGED", VID_APC_DAMAGED );
	registeredIds_.insert( "VID_APC_TASKED0", VID_APC_TASKED0 );
	registeredIds_.insert( "VID_APC_TASKED1", VID_APC_TASKED1 );
	registeredIds_.insert( "VID_APC_TASKED2", VID_APC_TASKED2 );
	registeredIds_.insert( "VID_APC_MOVING0", VID_APC_MOVING0 );
	registeredIds_.insert( "VID_APC_MOVING1", VID_APC_MOVING1 );
	registeredIds_.insert( "VID_APC_MOVING2", VID_APC_MOVING2 );
	registeredIds_.insert( "VID_APC_SELECTED0", VID_APC_SELECTED0 );
	registeredIds_.insert( "VID_APC_SELECTED1", VID_APC_SELECTED1 );
	registeredIds_.insert( "VID_APC_RECYCLE", VID_APC_RECYCLE );

	registeredIds_.insert( "VID_APC_DEPLOYMENT_COMPLETE", VID_APC_DEPLOYMENT_COMPLETE );
	registeredIds_.insert( "VID_APC_UNIT_BOARDED", VID_APC_UNIT_BOARDED );
	registeredIds_.insert( "VID_APC_MOVING_TO_DEPLOY_POINT", VID_APC_MOVING_TO_DEPLOY_POINT );

	// ======================================== P O D ===========================================

	registeredIds_.insert( "VID_POD_NIGHTFALL_APPROACHING", VID_POD_NIGHTFALL_APPROACHING );
	registeredIds_.insert( "VID_POD_DAYBREAK_APPROACHING", VID_POD_DAYBREAK_APPROACHING );
	registeredIds_.insert( "VID_POD_BEACON_DESTROYED", VID_POD_BEACON_DESTROYED );
	registeredIds_.insert( "VID_POD_UPLINK_DESTROYED", VID_POD_UPLINK_DESTROYED );
	registeredIds_.insert( "VID_POD_POD_ATTACKED", VID_POD_POD_ATTACKED );
	registeredIds_.insert( "VID_POD_POD_CRITICAL_DAMAGE", VID_POD_POD_CRITICAL_DAMAGE );
	registeredIds_.insert( "VID_POD_ION_CANNON_ACQUIRED", VID_POD_ION_CANNON_ACQUIRED );
	registeredIds_.insert( "VID_POD_ION_CANNON_CHARGED", VID_POD_ION_CANNON_CHARGED );
	registeredIds_.insert( "VID_POD_BUILDING_DESTROYED", VID_POD_BUILDING_DESTROYED );
	registeredIds_.insert( "VID_POD_BUILDING_CRITICAL", VID_POD_BUILDING_CRITICAL );
	registeredIds_.insert( "VID_POD_CONSTRUCTION_ATTACKED", VID_POD_CONSTRUCTION_ATTACKED );
	registeredIds_.insert( "VID_POD_MINERALS_EXHAUSTED", VID_POD_MINERALS_EXHAUSTED );
	registeredIds_.insert( "VID_POD_MINE_INOPERABLE", VID_POD_MINE_INOPERABLE );
	registeredIds_.insert( "VID_POD_NO_BMUS_LEFT", VID_POD_NO_BMUS_LEFT );
	registeredIds_.insert( "VID_POD_BMUS_LOW", VID_POD_BMUS_LOW );
	registeredIds_.insert( "VID_POD_NEED_FURTHER_SMELTERS", VID_POD_NEED_FURTHER_SMELTERS );
	registeredIds_.insert( "VID_POD_NEED_AT_LEAST_ONE_SMELTER", VID_POD_NEED_AT_LEAST_ONE_SMELTER );
	registeredIds_.insert( "VID_POD_ENEMY_NUKE_LAUNCH", VID_POD_ENEMY_NUKE_LAUNCH );
	registeredIds_.insert( "VID_POD_ENEMY_ION_CANNON", VID_POD_ENEMY_ION_CANNON );
	registeredIds_.insert( "VID_POD_NUKE_READY", VID_POD_NUKE_READY );
	registeredIds_.insert( "VID_POD_NO_TECHNICIANS", VID_POD_NO_TECHNICIANS );
	registeredIds_.insert( "VID_POD_POD_DESTROYED", VID_POD_POD_DESTROYED );
	registeredIds_.insert( "VID_POD_MINE_SWITCHED_SOURCE", VID_POD_MINE_SWITCHED_SOURCE );
	registeredIds_.insert( "VID_POD_ENEMY_NUKE_ONLINE", VID_POD_ENEMY_NUKE_ONLINE );
	registeredIds_.insert( "VID_POD_ENEMY_ION_CANNON_ONLINE", VID_POD_ENEMY_ION_CANNON_ONLINE );
	registeredIds_.insert( "VID_POD_REINFORCEMENTS_ONLINE", VID_POD_REINFORCEMENTS_ONLINE );
	registeredIds_.insert( "VID_POD_TURRET_ATTACKED", VID_POD_TURRET_ATTACKED );
	registeredIds_.insert( "VID_POD_INSUFFICIENT_CASH_FOR_NUKE", VID_POD_INSUFFICIENT_CASH_FOR_NUKE );
	registeredIds_.insert( "VID_POD_NEW_CONSTRUCTIONS", VID_POD_NEW_CONSTRUCTIONS );
	registeredIds_.insert( "VID_POD_INSUFFICIENT_BMUS", VID_POD_INSUFFICIENT_BMUS );

	// ========================== INTERFERENCE SAMPLES ============================

	registeredIds_.insert( "VID_INTERFERENCE_0", VID_INTERFERENCE_0 );
	registeredIds_.insert( "VID_INTERFERENCE_1", VID_INTERFERENCE_1 );
	registeredIds_.insert( "VID_INTERFERENCE_2", VID_INTERFERENCE_2 );
	registeredIds_.insert( "VID_INTERFERENCE_3", VID_INTERFERENCE_3 );
	registeredIds_.insert( "VID_INTERFERENCE_4", VID_INTERFERENCE_4 );
	registeredIds_.insert( "VID_INTERFERENCE_5", VID_INTERFERENCE_5 );

	// ========================== S P E C I A L   C A M P A I G N   M A I L S ============================

	registeredIds_.insert( "VID_1O1A", VID_1O1A );
	registeredIds_.insert( "VID_1O1B", VID_1O1B );
	registeredIds_.insert( "VID_1O1C", VID_1O1C );
	registeredIds_.insert( "VID_1O1D", VID_1O1D );
	registeredIds_.insert( "VID_1O1E", VID_1O1E );
	registeredIds_.insert( "VID_1O1F", VID_1O1F );
	registeredIds_.insert( "VID_1O1G", VID_1O1G );

	registeredIds_.insert( "VID_RESFAC_01", VID_RESFAC_01 );
	registeredIds_.insert( "VID_RESFAC_02", VID_RESFAC_02 );
	registeredIds_.insert( "VID_RESFAC_03", VID_RESFAC_03 );
	registeredIds_.insert( "VID_RESFAC_04", VID_RESFAC_04 );
	registeredIds_.insert( "VID_RESFAC_05", VID_RESFAC_05 );

	registeredIds_.insert( "VID_GUNS_01", VID_GUNS_01 );
	registeredIds_.insert( "VID_GUNS_02", VID_GUNS_02 );
	registeredIds_.insert( "VID_GUNS_03", VID_GUNS_03 );
	registeredIds_.insert( "VID_GUNS_04", VID_GUNS_04 );
	registeredIds_.insert( "VID_GUNS_05", VID_GUNS_05 );
	registeredIds_.insert( "VID_GUNS_06", VID_GUNS_06 );

	registeredIds_.insert( "VID_5O1_01", VID_5O1_01 );
	registeredIds_.insert( "VID_5O1_02", VID_5O1_02 );

	registeredIds_.insert( "VID_PCS_01", VID_PCS_01 );
	registeredIds_.insert( "VID_PCS_02", VID_PCS_02 );
	registeredIds_.insert( "VID_PCS_03", VID_PCS_03 );
	registeredIds_.insert( "VID_PCS_04", VID_PCS_04 );
	registeredIds_.insert( "VID_PCS_05", VID_PCS_05 );
	registeredIds_.insert( "VID_PCS_06", VID_PCS_06 );
	registeredIds_.insert( "VID_PCS_07", VID_PCS_07 );
	registeredIds_.insert( "VID_PCS_08", VID_PCS_08 );
	registeredIds_.insert( "VID_PCS_09", VID_PCS_09 );
	registeredIds_.insert( "VID_PCS_10", VID_PCS_10 );
	registeredIds_.insert( "VID_PCS_11", VID_PCS_11 );
	registeredIds_.insert( "VID_PCS_12", VID_PCS_12 );
	registeredIds_.insert( "VID_PCS_13", VID_PCS_13 );
	registeredIds_.insert( "VID_PCS_14", VID_PCS_14 );

}

/* End VMMAN2.CPP ****************************************************/
