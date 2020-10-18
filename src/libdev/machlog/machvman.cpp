/*
 * M A C H V M A N . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

//#include "machlog/internal/VMinfo.hpp"
//#include "machlog/internal/VMail.hpp"

#include "machlog/machvman.hpp"

#include "stdlib/string.hpp"

#include "machlog/actor.hpp"
#include "machlog/aggressr.hpp"
#include "machlog/construc.hpp"
#include "machlog/machine.hpp"
#include "machlog/races.hpp"
#include "machlog/rescarr.hpp"
#include "machlog/technici.hpp"
#include "machlog/vmman.hpp"

#include "machphys/random.hpp"

// static
MachLogMachineVoiceMailManager& MachLogMachineVoiceMailManager::instance()
{
    static MachLogMachineVoiceMailManager instance_;
    return instance_;
}

MachLogMachineVoiceMailManager::MachLogMachineVoiceMailManager()
{
    TEST_INVARIANT;
}

MachLogMachineVoiceMailManager::~MachLogMachineVoiceMailManager()
{
    TEST_INVARIANT;
}

void MachLogMachineVoiceMailManager::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void MachLogMachineVoiceMailManager::postNewMail(MachLog::ObjectType ot, int subType, MachineVoiceMailEventID id, MachPhys::Race race )
{
	VoiceMailID globalId = getGlobalFromMachineEvent( ot, subType, id );
	MachLogVoiceMailManager& manref = MachLogVoiceMailManager::instance();
	MachLogVoiceMailManager::instance().postNewMail( globalId, race );
}

void MachLogMachineVoiceMailManager::postNewMail(MachLog::ObjectType ot, int subType, MachineVoiceMailEventID id, UtlId actorId, MachPhys::Race race )
{
	PRE( MachLogRaces::instance().actorExists( actorId ) );
	PRE( MachLogRaces::instance().actor( actorId ).objectIsMachine() );

	if( not MachLogRaces::instance().actor( actorId ).isIn1stPersonView() )
	{
		VoiceMailID globalId = getGlobalFromMachineEvent( ot, subType, id, actorId );
		MachLogVoiceMailManager::instance().postNewMail( globalId, actorId, race );
	}
}

void MachLogMachineVoiceMailManager::postNewMail(MachLog::ObjectType ot, int subType, MachineVoiceMailEventID id, MexPoint3d position, MachPhys::Race race )
{
	VoiceMailID globalId = getGlobalFromMachineEvent( ot, subType, id );
	MachLogVoiceMailManager::instance().postNewMail( globalId, position, race );
}

ostream& operator <<( ostream& o, const MachLogMachineVoiceMailManager& t )
{

    o << "MachLogMachineVoiceMailManager " << (void*)&t << " start" << std::endl;
    o << "MachLogMachineVoiceMailManager " << (void*)&t << " end" << std::endl;

    return o;
}

ostream& operator <<( ostream& o, MachLogMachineVoiceMailManager::MachineVoiceMailEventID id )
{

    switch( id )
    {
        case MachLogMachineVoiceMailManager:: MEV_SELF_DESTRUCT:
            o << "MEV_SELF_DESTRUCT";
            break;
        case MachLogMachineVoiceMailManager:: MEV_BUILT:
            o << "MEV_BUILT";
            break;
        case MachLogMachineVoiceMailManager:: MEV_DESTROYED:
            o << "MEV_DESTROYED";
            break;
        case MachLogMachineVoiceMailManager:: MEV_NEAR_DEATH:
            o << "MEV_NEAR_DEATH";
			break;
		case MachLogMachineVoiceMailManager:: MEV_CHANGED_RACE:
            o << "MEV_CHANGED_RACE";
            break;
        case MachLogMachineVoiceMailManager:: MEV_VIRUS_INFECTED:
            o << "MEV_VIRUS_INFECTED";
            break;
        case MachLogMachineVoiceMailManager:: MEV_DAMAGED:
            o << "MEV_DAMAGED";
            break;
        case MachLogMachineVoiceMailManager:: MEV_TASKED:
            o << "MEV_TASKED";
			break;
		case MachLogMachineVoiceMailManager:: MEV_MOVING:
            o << "MEV_MOVING";
            break;
        case MachLogMachineVoiceMailManager:: MEV_SELECTED:
            o << "MEV_SELECTED";
            break;
        case MachLogMachineVoiceMailManager:: MEV_TARGET_ENEMY:
            o << "MEV_TARGET_ENEMY";
            break;
        case MachLogMachineVoiceMailManager:: MEV_RECYCLE:
            o << "MEV_RECYCLE";
            break;
		case MachLogMachineVoiceMailManager:: MEV_HEAL_TARGET:
            o << "MEV_HEAL_TARGET";
            break;
        case MachLogMachineVoiceMailManager:: MEV_HEALING_COMPLETE:
            o << "MEV_HEALING_COMPLETE";
            break;
		case MachLogMachineVoiceMailManager:: MEV_LAUNCH_VIRUS:
            o << "MEV_LAUNCH_VIRUS";
            break;
        case MachLogMachineVoiceMailManager:: MEV_VIRUS_LAUNCHED:
            o << "MEV_VIRUS_LAUNCHED";
            break;
        case MachLogMachineVoiceMailManager:: MEV_TREACHERY_TARGET:
            o << "MEV_TREACHERY_TARGET";
            break;
        case MachLogMachineVoiceMailManager:: MEV_BUILDING_COMPLETE:
            o << "MEV_BUILDING_COMPLETE";
			break;
		case MachLogMachineVoiceMailManager:: MEV_AWAITING_NEW_JOB:
            o << "MEV_AWAITING_NEW_JOB";
            break;
        case MachLogMachineVoiceMailManager:: MEV_MOVING_TO_NEXT:
            o << "MEV_MOVING_TO_NEXT";
            break;
        case MachLogMachineVoiceMailManager:: MEV_MOVE_TO_SITE:
            o << "MEV_MOVE_TO_SITE";
            break;
        case MachLogMachineVoiceMailManager:: MEV_BUILDING_CAPTURED:
            o << "MEV_BUILDING_CAPTURED";
            break;
		case MachLogMachineVoiceMailManager:: MEV_BUILDING_DECONSTRUCTED:
            o << "MEV_BUILDING_DECONSTRUCTED";
            break;
        case MachLogMachineVoiceMailManager:: MEV_BUILDING_REPAIRED:
            o << "MEV_BUILDING_REPAIRED";
            break;


  		DEFAULT_ASSERT_BAD_CASE( id );
    }

    return o;
}

VoiceMailID MachLogMachineVoiceMailManager::getGlobalFromMachineEvent( MachLog::ObjectType ot, int subType, MachineVoiceMailEventID id, UtlId actorId )
{

	// this method has multiple return points to avoid writing 192 break; statements.

	ASSERT_INFO( ot );

	ASSERT( ot == MachLog::AGGRESSOR
			or ot == MachLog::ADMINISTRATOR
			or ot == MachLog::CONSTRUCTOR
			or ot == MachLog::GEO_LOCATOR
			or ot == MachLog::SPY_LOCATOR
			or ot == MachLog::TECHNICIAN
			or ot == MachLog::RESOURCE_CARRIER
			or ot == MachLog::APC,
			"No, that's rubbish. Not a recognised machine type." );

	switch( ot )
	{
		// ========================================== A G G R E S S O R S ==========================================
		case MachLog::AGGRESSOR:
			switch( subType )
			{
				//--------------------------------------- G R U N T ---------------------------------------
				case MachPhys::GRUNT:
					switch( id )
					{
						case MEV_SELF_DESTRUCT:
							return VID_GRUNT_SELF_DESTRUCT;
						case MEV_BUILT:
							return VID_GRUNT_BUILT0;
						case MEV_DESTROYED:
							return VID_GRUNT_DESTROYED;
						case MEV_NEAR_DEATH:
							switch( MachPhysRandom::randomInt( 0, 4 ) )
							{
								case 0:
									return VID_GRUNT_NEAR_DEATH0;
								case 1:
									return VID_GRUNT_NEAR_DEATH1;
								case 2:
									return VID_GRUNT_NEAR_DEATH2;
								case 3:
									return VID_GRUNT_NEAR_DEATH3;
							}
						case MEV_CHANGED_RACE:
							switch( MachPhysRandom::randomInt( 0, 2 ) )
							{
								case 0:
									return VID_GRUNT_CHANGED_RACE0;
								case 1:
									return VID_GRUNT_CHANGED_RACE1;
							}
						case MEV_VIRUS_INFECTED:
							return VID_GRUNT_VIRUS_INFECTED;
						case MEV_DAMAGED:
							return VID_GRUNT_DAMAGED;
						case MEV_TASKED:
							switch( MachPhysRandom::randomInt( 0, 4 ) )
							{
								case 0:
									return VID_GRUNT_TASKED0;
								case 1:
									return VID_GRUNT_TASKED1;
								case 2:
									return VID_GRUNT_TASKED2;
								case 3:
									return VID_GRUNT_TASKED3;
							}
						case MEV_MOVING:
							switch( MachPhysRandom::randomInt( 0, 4 ) )
							{
								case 0:
									return VID_GRUNT_MOVING0;
								case 1:
									return VID_GRUNT_MOVING1;
								case 2:
									return VID_GRUNT_MOVING2;
								case 3:
									return VID_GRUNT_MOVING3;
							}
						case MEV_SELECTED:
							switch( MachPhysRandom::randomInt( 0, 3 ) )
							{
								case 0:
									return VID_GRUNT_SELECTED0;
								case 1:
									return VID_GRUNT_SELECTED1;
								case 2:
									return VID_GRUNT_SELECTED2;
							}
						case MEV_TARGET_ENEMY:
							switch( MachPhysRandom::randomInt( 0, 5 ) )
							{
								case 0:
									return VID_GRUNT_TARGET_ENEMY0;
								case 1:
									return VID_GRUNT_TARGET_ENEMY1;
								case 2:
									return VID_GRUNT_TARGET_ENEMY2;
								case 3:
									return VID_GRUNT_TARGET_ENEMY3;
								case 4:
									return VID_GRUNT_TARGET_ENEMY4;
							}
						case MEV_RECYCLE:
							return VID_GRUNT_RECYCLE;

						DEFAULT_ASSERT_BAD_CASE( id );
					}
				//----------------------------------- A S S A S S I N -------------------------------------
				case MachPhys::ASSASSIN:
					switch( id )
					{
						case MEV_SELF_DESTRUCT:
							return VID_ASSASSIN_SELF_DESTRUCT;
						case MEV_BUILT:
							return VID_ASSASSIN_BUILT;
						case MEV_DESTROYED:
							return VID_ASSASSIN_DESTROYED;
						case MEV_NEAR_DEATH:
							switch( MachPhysRandom::randomInt( 0, 2 ) )
							{
								case 0:
									return VID_ASSASSIN_NEAR_DEATH0;
								case 1:
									return VID_ASSASSIN_NEAR_DEATH1;
							}
						case MEV_CHANGED_RACE:
							switch( MachPhysRandom::randomInt( 0, 2 ) )
							{
								case 0:
									return VID_ASSASSIN_CHANGED_RACE0;
								case 1:
									return VID_ASSASSIN_CHANGED_RACE1;
							}
						case MEV_VIRUS_INFECTED:
							return VID_ASSASSIN_VIRUS_INFECTED;
						case MEV_DAMAGED:
							return VID_ASSASSIN_DAMAGED;
						case MEV_TASKED:
							switch( MachPhysRandom::randomInt( 0, 2 ) )
							{
								case 0:
									return VID_ASSASSIN_TASKED0;
								case 1:
									return VID_ASSASSIN_TASKED1;
							}
						case MEV_MOVING:
							switch( MachPhysRandom::randomInt( 0, 2 ) )
							{
								case 0:
									return VID_ASSASSIN_MOVING0;
								case 1:
									return VID_ASSASSIN_MOVING1;
							}
						case MEV_SELECTED:
							switch( MachPhysRandom::randomInt( 0, 3 ) )
							{
								case 0:
									return VID_ASSASSIN_SELECTED0;
								case 1:
									return VID_ASSASSIN_SELECTED1;
								case 2:
									return VID_ASSASSIN_SELECTED2;
							}
						case MEV_TARGET_ENEMY:
							switch( MachPhysRandom::randomInt( 0, 3 ) )
							{
								case 0:
									return VID_ASSASSIN_TARGET_ENEMY0;
								case 1:
									return VID_ASSASSIN_TARGET_ENEMY1;
								case 2:
									return VID_ASSASSIN_TARGET_ENEMY2;
							}
						case MEV_RECYCLE:
							return VID_ASSASSIN_RECYCLE;

						DEFAULT_ASSERT_BAD_CASE( id );
					}
				//----------------------------------- K N I G H T ---------------------------------------
				case MachPhys::KNIGHT:
					switch( id )
					{
						case MEV_SELF_DESTRUCT:
							return VID_KNIGHT_SELF_DESTRUCT;
						case MEV_BUILT:
							return VID_KNIGHT_BUILT;
						case MEV_NEAR_DEATH:
							return VID_KNIGHT_NEAR_DEATH;
						case MEV_CHANGED_RACE:
							return VID_KNIGHT_CHANGED_RACE;
						case MEV_DAMAGED:
							switch( MachPhysRandom::randomInt( 0, 2 ) )
							{
								case 0:
									return VID_KNIGHT_DAMAGED0;
								case 1:
									return VID_KNIGHT_DAMAGED1;
							}
						case MEV_TASKED:
						case MEV_RECYCLE:
							switch( MachPhysRandom::randomInt( 0, 2 ) )
							{
								case 0:
									return VID_KNIGHT_TASKED0;
								case 1:
									return VID_KNIGHT_TASKED1;
							}
						case MEV_MOVING:
							switch( MachPhysRandom::randomInt( 0, 4 ) )
							{
								case 0:
									return VID_KNIGHT_MOVING0;
								case 1:
									return VID_KNIGHT_MOVING1;
								case 2:
									return VID_KNIGHT_MOVING2;
								case 3:
									return VID_KNIGHT_MOVING3;
							}
						case MEV_SELECTED:
							switch( MachPhysRandom::randomInt( 0, 2 ) )
							{
								case 0:
									return VID_KNIGHT_SELECTED0;
								case 1:
									return VID_KNIGHT_SELECTED1;
							}
						case MEV_TARGET_ENEMY:
							switch( MachPhysRandom::randomInt( 0, 2 ) )
							{
								case 0:
									return VID_KNIGHT_TARGET_ENEMY0;
								case 1:
									return VID_KNIGHT_TARGET_ENEMY1;
							}

						DEFAULT_ASSERT_BAD_CASE( id );
					}
				//----------------------------------- B A L L I S T A ---------------------------------------
				case MachPhys::BALLISTA:
					switch( id )
					{
						case MEV_SELF_DESTRUCT:
							return VID_BALLISTA_SELF_DESTRUCT;
						case MEV_BUILT:
						{
							ASSERT( actorId != 0, "Wasn't passed an actor id!" );
							ASSERT( MachLogRaces::instance().actorExists( actorId ), "That actor doesn't exist!" );

							const MachActor& actor = MachLogRaces::instance().actor( actorId );
							ASSERT( actor.objectType() == MachLog::AGGRESSOR, "Id of 'aggressor' was for a non-aggressor actor!" );

							const MachLogAggressor& ballista = actor.asAggressor();
							if( ballista.hwLevel() == 3 )
							{
								// either a crusader or an eradicator
								if( ballista.weaponCombo() == MachPhys::T_MULTI_LAUNCHER1 )
									return VID_BALLISTA_CRUSADER_BUILT;
								else
									return VID_BALLISTA_ERADICATOR_BUILT;
							}
							else
							{
								// must be a goliath
								return VID_BALLISTA_GOLIATH_BUILT;
							}
						}
						case MEV_DESTROYED:
							return VID_BALLISTA_DESTROYED;
						case MEV_NEAR_DEATH:
							return VID_BALLISTA_NEAR_DEATH;
						case MEV_CHANGED_RACE:
							switch( MachPhysRandom::randomInt( 0, 2 ) )
							{
								case 0:
									return VID_BALLISTA_CHANGED_RACE0;
								case 1:
									return VID_BALLISTA_CHANGED_RACE1;
							}
						case MEV_VIRUS_INFECTED:
							return VID_BALLISTA_VIRUS_INFECTED;
						case MEV_DAMAGED:
							return VID_BALLISTA_DAMAGED;
						case MEV_TASKED:
							switch( MachPhysRandom::randomInt( 0, 4 ) )
							{
								case 0:
									return VID_BALLISTA_TASKED0;
								case 1:
									return VID_BALLISTA_TASKED1;
								case 2:
									return VID_BALLISTA_TASKED2;
								case 3:
									return VID_BALLISTA_TASKED3;
							}
						case MEV_MOVING:
							switch( MachPhysRandom::randomInt( 0, 3 ) )
							{
								case 0:
									return VID_BALLISTA_MOVING0;
								case 1:
									return VID_BALLISTA_MOVING1;
								case 2:
									return VID_BALLISTA_MOVING2;
							}
						case MEV_SELECTED:
							switch( MachPhysRandom::randomInt( 0, 2 ) )
							{
								case 0:
									return VID_BALLISTA_SELECTED0;
								case 1:
									return VID_BALLISTA_SELECTED1;
							}
						case MEV_TARGET_ENEMY:
							switch( MachPhysRandom::randomInt( 0, 4 ) )
							{
								case 0:
									return VID_BALLISTA_TARGET_ENEMY0;
								case 1:
									return VID_BALLISTA_TARGET_ENEMY1;
								case 2:
									return VID_BALLISTA_TARGET_ENEMY2;
								case 3:
									return VID_BALLISTA_TARGET_ENEMY3;
							}
						case MEV_RECYCLE:
							return VID_BALLISTA_RECYCLE;

						DEFAULT_ASSERT_BAD_CASE( id );
					}

				//----------------------------------- N I N J A ---------------------------------------
				case MachPhys::NINJA:
					switch( id )
					{
						case MEV_SELF_DESTRUCT:
							return VID_NINJA_SELF_DESTRUCT;
						case MEV_BUILT:
							return VID_NINJA_BUILT;
						case MEV_DESTROYED:
							return VID_NINJA_DESTROYED;
						case MEV_NEAR_DEATH:
							switch( MachPhysRandom::randomInt( 0, 2 ) )
							{
								case 0:
									return VID_NINJA_NEAR_DEATH0;
								case 1:
									return VID_NINJA_NEAR_DEATH1;
							}
						case MEV_CHANGED_RACE:
							return VID_NINJA_CHANGED_RACE;
						case MEV_VIRUS_INFECTED:
							break;
						case MEV_DAMAGED:
							return VID_NINJA_DAMAGED;
						case MEV_TASKED:
							switch( MachPhysRandom::randomInt( 0, 2 ) )
							{
								case 0:
									return VID_NINJA_TASKED0;
								case 1:
									return VID_NINJA_TASKED1;
							}
						case MEV_MOVING:
							switch( MachPhysRandom::randomInt( 0, 3 ) )
							{
								case 0:
									return VID_NINJA_MOVING0;
								case 1:
									return VID_NINJA_MOVING1;
								case 2:
									return VID_NINJA_MOVING2;
							}
						case MEV_SELECTED:
							switch( MachPhysRandom::randomInt( 0, 3 ) )
							{
								case 0:
									return VID_NINJA_SELECTED0;
								case 1:
									return VID_NINJA_SELECTED1;
								case 2:
									return VID_NINJA_SELECTED2;
							}
						case MEV_TARGET_ENEMY:
							switch( MachPhysRandom::randomInt( 0, 4 ) )
							{
								case 0:
									return VID_NINJA_TARGET_ENEMY0;
								case 1:
									return VID_NINJA_TARGET_ENEMY1;
								case 2:
									return VID_NINJA_TARGET_ENEMY2;
								case 3:
									return VID_NINJA_TARGET_ENEMY3;
							}
						case MEV_RECYCLE:
							return VID_NINJA_RECYCLE;

						DEFAULT_ASSERT_BAD_CASE( id );
					}
				DEFAULT_ASSERT_BAD_CASE( subType );
			}
		// ====================================== A D M I N I S T R A T O R S ======================================
		case MachLog::ADMINISTRATOR:
			switch( subType )
			{
				//--------------------------------------- B O S S ---------------------------------------
				case MachPhys::BOSS:
					switch( id )
					{
						case MEV_SELF_DESTRUCT:
							return VID_BOSS_SELF_DESTRUCT;
						case MEV_BUILT:
						{
							ASSERT( actorId != 0, "Wasn't passed an actor id!" );
							ASSERT( MachLogRaces::instance().actorExists( actorId ), "That actor doesn't exist!" );

							const MachActor& actor = MachLogRaces::instance().actor( actorId );
							ASSERT( actor.objectIsMachine(), "Id of 'boss' was for a non-machine actor!" );

							if( actor.asMachine().hwLevel() == 2 )
								return VID_BOSS_BEE_BUILT;
							else
								return VID_BOSS_WASP_BUILT;
						}
						case MEV_NEAR_DEATH:
							switch( MachPhysRandom::randomInt( 0, 2 ) )
							{
								case 0:
									return VID_BOSS_NEAR_DEATH0;
								case 1:
									return VID_BOSS_NEAR_DEATH1;
							}
						case MEV_CHANGED_RACE:
							return VID_BOSS_CHANGED_RACE;
						case MEV_DAMAGED:
							switch( MachPhysRandom::randomInt( 0, 2 ) )
							{
								case 0:
									return VID_BOSS_DAMAGED0;
								case 1:
									return VID_BOSS_DAMAGED1;
							}
						case MEV_RECYCLE:
						case MEV_TASKED:
							switch( MachPhysRandom::randomInt( 0, 4 ) )
							{
								case 0:
									return VID_BOSS_TASKED0;
								case 1:
									return VID_BOSS_TASKED1;
								case 2:
									return VID_BOSS_TASKED2;
								case 3:
									return VID_BOSS_TASKED3;
							}
						case MEV_MOVING:
							switch( MachPhysRandom::randomInt( 0, 4 ) )
							{
								case 0:
									return VID_BOSS_MOVING0;
								case 1:
									return VID_BOSS_MOVING1;
								case 2:
									return VID_BOSS_MOVING2;
								case 3:
									return VID_BOSS_MOVING3;
							}
						case MEV_SELECTED:
							switch( MachPhysRandom::randomInt( 0, 2 ) )
							{
								case 0:
									return VID_BOSS_SELECTED0;
								case 1:
									return VID_BOSS_SELECTED1;
							}
						case MEV_TARGET_ENEMY:
							switch( MachPhysRandom::randomInt( 0, 2 ) )
							{
								case 0:
									return VID_BOSS_TARGET_ENEMY0;
								case 1:
									return VID_BOSS_TARGET_ENEMY1;
							}

						DEFAULT_ASSERT_BAD_CASE( id );
					}
				//----------------------------------- O V E R S E E R -------------------------------------
				case MachPhys::OVERSEER:
					switch( id )
					{
						case MEV_SELF_DESTRUCT:
							return VID_OVERSEER_SELF_DESTRUCT;
						case MEV_BUILT:
							return VID_OVERSEER_BUILT;
						case MEV_NEAR_DEATH:
							return VID_OVERSEER_NEAR_DEATH;
						case MEV_CHANGED_RACE:
							switch( MachPhysRandom::randomInt( 0, 2 ) )
							{
								case 0:
									return VID_OVERSEER_CHANGED_RACE0;
								case 1:
									return VID_OVERSEER_CHANGED_RACE1;
							}
						case MEV_DAMAGED:
							return VID_OVERSEER_DAMAGED;
						case MEV_TASKED:
							return VID_OVERSEER_TASKED;
						case MEV_MOVING:
							return VID_OVERSEER_MOVING;
						case MEV_SELECTED:
							switch( MachPhysRandom::randomInt( 0, 2 ) )
							{
								case 0:
									return VID_OVERSEER_SELECTED0;
								case 1:
									return VID_OVERSEER_SELECTED1;
							}
						case MEV_TARGET_ENEMY:
							switch( MachPhysRandom::randomInt( 0, 2 ) )
							{
								case 0:
									return VID_OVERSEER_TARGET_ENEMY0;
								case 1:
									return VID_OVERSEER_TARGET_ENEMY1;
							}
						case MEV_RECYCLE:
							return VID_OVERSEER_MOVING;
						case MEV_HEAL_TARGET:
							return VID_OVERSEER_HEAL_TARGET;
						case MEV_HEALING_COMPLETE:
							return VID_OVERSEER_HEALING_COMPLETE;

						DEFAULT_ASSERT_BAD_CASE( id );
					}
				//----------------------------------- C O M M A N D E R -------------------------------------
				case MachPhys::COMMANDER:
					switch( id )
					{
						case MEV_SELF_DESTRUCT:
							return VID_COMMANDER_SELF_DESTRUCT;
						case MEV_BUILT:
							return VID_COMMANDER_BUILT;
						case MEV_NEAR_DEATH:
							return VID_COMMANDER_NEAR_DEATH;
						case MEV_CHANGED_RACE:
							return VID_COMMANDER_CHANGED_RACE;
						case MEV_DAMAGED:
							return VID_COMMANDER_DAMAGED;
						case MEV_RECYCLE:
						case MEV_TASKED:
							switch( MachPhysRandom::randomInt( 0, 2 ) )
							{
								case 0:
									return VID_COMMANDER_TASKED0;
								case 1:
									return VID_COMMANDER_TASKED1;
							}
						case MEV_MOVING:
							switch( MachPhysRandom::randomInt( 0, 2 ) )
							{
								case 0:
									return VID_COMMANDER_MOVING0;
								case 1:
									return VID_COMMANDER_MOVING1;
							}
						case MEV_SELECTED:
							switch( MachPhysRandom::randomInt( 0, 2 ) )
							{
								case 0:
									return VID_COMMANDER_SELECTED0;
								case 1:
									return VID_COMMANDER_SELECTED1;
							}
						case MEV_TARGET_ENEMY:
							switch( MachPhysRandom::randomInt( 0, 2 ) )
							{
								case 0:
									return VID_COMMANDER_TARGET_ENEMY0;
								case 1:
									return VID_COMMANDER_TARGET_ENEMY1;
							}

						case MEV_HEAL_TARGET:
							return VID_COMMANDER_HEAL_TARGET;
						case MEV_HEALING_COMPLETE:
							return VID_COMMANDER_HEALING_COMPLETE;
						case MEV_TREACHERY_TARGET:
							return VID_COMMANDER_TREACHERY_TARGET;

						DEFAULT_ASSERT_BAD_CASE( id );
					}
				DEFAULT_ASSERT_BAD_CASE( subType );
			}
		// ====================================== T E C H N I C I A N ======================================
		case MachLog::TECHNICIAN:
			switch( id )
			{
				case MEV_SELF_DESTRUCT:
					return VID_TECHNICIAN_SELF_DESTRUCT;
				case MEV_BUILT:
					{
						ASSERT( actorId != 0, "Wasn't passed an actor id!" );
						ASSERT( MachLogRaces::instance().actorExists( actorId ), "That actor doesn't exist!" );

						const MachActor& actor = MachLogRaces::instance().actor( actorId );
						ASSERT( actor.objectType() == MachLog::TECHNICIAN, "Id of 'technician' was for a non-technician actor!" );

						switch( actor.asTechnician().subType() )
						{
							case MachPhys::LAB_TECH:
								return VID_TECHNICIAN_BUILT;
							case MachPhys::TECH_BOY:
								return VID_TECHNICIAN_ENGINEER_BUILT;
							case MachPhys::BRAIN_BOX:
								return VID_TECHNICIAN_BRAINBOX_BUILT;

							DEFAULT_ASSERT_BAD_CASE( actor.asTechnician().subType() );
						}
						}
 				case MEV_DESTROYED:
					return VID_TECHNICIAN_DESTROYED;
				case MEV_NEAR_DEATH:
					return VID_TECHNICIAN_NEAR_DEATH;
				case MEV_CHANGED_RACE:
					switch( MachPhysRandom::randomInt( 0, 2 ) )
					{
						case 0:
							return VID_TECHNICIAN_CHANGED_RACE0;
						case 1:
							return VID_TECHNICIAN_CHANGED_RACE1;
					}
				case MEV_VIRUS_INFECTED:
					return VID_TECHNICIAN_VIRUS_INFECTED;
				case MEV_DAMAGED:
					switch( MachPhysRandom::randomInt( 0, 2 ) )
					{
						case 0:
							return VID_TECHNICIAN_DAMAGED;
						case 1:
							return VID_TECHNICIAN_DESTROYED;
					}
				case MEV_TASKED:
					switch( MachPhysRandom::randomInt( 0, 2 ) )
					{
						case 0:
							return VID_TECHNICIAN_TASKED0;
						case 1:
							return VID_TECHNICIAN_TASKED1;
					}
				case MEV_MOVING:
					switch( MachPhysRandom::randomInt( 0, 3 ) )
					{
						case 0:
							return VID_TECHNICIAN_MOVING0;
						case 1:
							return VID_TECHNICIAN_MOVING1;
						case 2:
							return VID_TECHNICIAN_MOVING2;

					}
				case MEV_SELECTED:
					switch( MachPhysRandom::randomInt( 0, 2 ) )
					{
						case 0:
							return VID_TECHNICIAN_SELECTED0;
						case 1:
							return VID_TECHNICIAN_SELECTED1;

					}
				case MEV_RECYCLE:
					return VID_TECHNICIAN_RECYCLE;

				DEFAULT_ASSERT_BAD_CASE( id );
			}
		// ==================================== G E O - L O C A T O R =======================================
		case MachLog::GEO_LOCATOR:
			switch( id )
			{
				case MEV_SELF_DESTRUCT:
					return VID_GEO_SELF_DESTRUCT;
				case MEV_BUILT:
					return VID_GEO_BUILT;
				case MEV_DESTROYED:
					return VID_GEO_DESTROYED;
				case MEV_NEAR_DEATH:
					return VID_GEO_NEAR_DEATH;
				case MEV_CHANGED_RACE:
					switch( MachPhysRandom::randomInt( 0, 2 ) )
					{
						case 0:
							return VID_GEO_CHANGED_RACE0;
						case 1:
							return VID_GEO_CHANGED_RACE1;
					}
				case MEV_VIRUS_INFECTED:
					return VID_GEO_VIRUS_INFECTED;
				case MEV_DAMAGED:
					return VID_GEO_DAMAGED;
				case MEV_TASKED:
					switch( MachPhysRandom::randomInt( 0, 2 ) )
					{
						case 0:
							return VID_GEO_TASKED0;
						case 1:
							return VID_GEO_TASKED1;
					}
				case MEV_MOVING:
					switch( MachPhysRandom::randomInt( 0, 2 ) )
					{
						case 0:
							return VID_GEO_MOVING0;
						case 1:
							return VID_GEO_MOVING1;
					}
				case MEV_SELECTED:
					switch( MachPhysRandom::randomInt( 0, 2 ) )
					{
						case 0:
							return VID_GEO_SELECTED0;
						case 1:
							return VID_GEO_SELECTED1;
					}
				case MEV_RECYCLE:
					return VID_GEO_RECYCLE;

				DEFAULT_ASSERT_BAD_CASE( id );
			}
		// ==================================== S P Y - L O C A T O R =======================================
		case MachLog::SPY_LOCATOR:
			switch( id )
			{
				case MEV_SELF_DESTRUCT:
					return VID_SPY_SELF_DESTRUCT;
				case MEV_BUILT:
					return VID_SPY_BUILT;
				case MEV_DESTROYED:
					return VID_SPY_DESTROYED;
				case MEV_NEAR_DEATH:
					return VID_SPY_NEAR_DEATH;
				case MEV_CHANGED_RACE:
					return VID_SPY_CHANGED_RACE;
				case MEV_VIRUS_INFECTED:
					return VID_SPY_VIRUS_INFECTED;
				case MEV_DAMAGED:
					return VID_SPY_DAMAGED;
				case MEV_TASKED:
					return VID_SPY_TASKED0;
				case MEV_MOVING:
					switch( MachPhysRandom::randomInt( 0, 2 ) )
					{
						case 0:
							return VID_SPY_MOVING0;
						case 1:
							return VID_SPY_MOVING1;
					}
				case MEV_SELECTED:
					return VID_SPY_SELECTED0;
				case MEV_RECYCLE:
					return VID_SPY_RECYCLE;

				DEFAULT_ASSERT_BAD_CASE( id );
			}

		// =================================== C O N S T R U C T O R S ======================================

		// note that all 3 constructors use the same .wavs for all but the "I've been built" voicemails.
		case MachLog::CONSTRUCTOR:
			switch( id )
			{
				case MEV_SELF_DESTRUCT:
					return VID_DOZER_SELF_DESTRUCT;
				case MEV_BUILT:
					switch( subType )
					{
						case MachPhys::DOZER:
							return VID_DOZER_BUILT;
						case MachPhys::BUILDER:
							return VID_BUILDER_BUILT;
						case MachPhys::BEHEMOTH:
						{
							ASSERT( actorId != 0, "Wasn't passed an actor id!" );
							ASSERT( MachLogRaces::instance().actorExists( actorId ), "That actor doesn't exist!" );

							const MachActor& actor = MachLogRaces::instance().actor( actorId );
							ASSERT( actor.objectType() == MachLog::CONSTRUCTOR, "Id of 'constructor' was for a non-constructor actor!" );

							if( actor.asConstructor().hwLevel() == 4 )
								return VID_BEHEMOTH_BUILT;
							else
								return VID_TITAN_BUILT;
						}

						DEFAULT_ASSERT_BAD_CASE( subType );
					}
				case MEV_DESTROYED:
					return VID_DOZER_DESTROYED;
				case MEV_NEAR_DEATH:
					return VID_DOZER_NEAR_DEATH;
				case MEV_CHANGED_RACE:
					switch( MachPhysRandom::randomInt( 0, 2 ) )
					{
						case 0:
							return VID_DOZER_CHANGED_RACE0;
						case 1:
							return VID_DOZER_CHANGED_RACE1;
					}
				case MEV_VIRUS_INFECTED:
					return VID_DOZER_VIRUS_INFECTED;
				case MEV_DAMAGED:
					return VID_DOZER_DAMAGED;
				case MEV_TASKED:
					switch( MachPhysRandom::randomInt( 0, 3 ) )
					{
						case 0:
							return VID_DOZER_TASKED0;
						case 1:
							return VID_DOZER_TASKED1;
						case 2:
							return VID_DOZER_TASKED2;
					}
				case MEV_MOVING:
					switch( MachPhysRandom::randomInt( 0, 3 ) )
					{
						case 0:
							return VID_DOZER_MOVING0;
						case 1:
							return VID_DOZER_MOVING1;
						case 2:
							return VID_DOZER_MOVING2;
					}
				case MEV_SELECTED:
					switch( MachPhysRandom::randomInt( 0, 2 ) )
					{
						case 0:
							return VID_DOZER_SELECTED0;
						case 1:
							return VID_DOZER_SELECTED1;
					}
				case MEV_RECYCLE:
					return VID_DOZER_RECYCLE;
				case MEV_BUILDING_COMPLETE:
					return VID_DOZER_BUILDING_COMPLETE;
				case MEV_AWAITING_NEW_JOB:
					return VID_DOZER_AWAITING_NEW_JOB;
				case MEV_MOVING_TO_NEXT:
					return VID_DOZER_MOVING_TO_NEXT;
				case MEV_MOVE_TO_SITE:
					return VID_DOZER_MOVE_TO_SITE;
				case MEV_BUILDING_CAPTURED:
					return VID_DOZER_BUILDING_CAPTURED;
				case MEV_BUILDING_DECONSTRUCTED:
					return VID_DOZER_BUILDING_DECONSTRUCTED;
				case MEV_BUILDING_REPAIRED:
					return  VID_DOZER_BUILDING_REPAIRED;

				DEFAULT_ASSERT_BAD_CASE( id );
			}
		// ================================ R E S O U R C E    C A R R I E R =======================================
		case MachLog::RESOURCE_CARRIER:
			switch( id )
			{
				case MEV_SELF_DESTRUCT:
					return VID_RESOURCE_CARRIER_SELF_DESTRUCT;
				case MEV_BUILT:
				{
					ASSERT( actorId != 0, "Wasn't passed an actor id!" );
					ASSERT( MachLogRaces::instance().actorExists( actorId ), "That actor doesn't exist!" );

					const MachActor& actor = MachLogRaces::instance().actor( actorId );
					ASSERT( actor.objectType() == MachLog::RESOURCE_CARRIER, "Id of 'resource carrier' was for a non-resource carrier actor!" );

					if( actor.asResourceCarrier().isScavenger() )
						return VID_RESOURCE_CARRIER_SCAVENGER_BUILT;
					else
						return VID_RESOURCE_CARRIER_BUILT;
				}
				case MEV_NEAR_DEATH:
					return VID_RESOURCE_CARRIER_NEAR_DEATH;
				case MEV_CHANGED_RACE:
					switch( MachPhysRandom::randomInt( 0, 2 ) )
					{
						case 0:
							return VID_RESOURCE_CARRIER_CHANGED_RACE0;
						case 1:
							return VID_RESOURCE_CARRIER_CHANGED_RACE1;
					}
				case MEV_DAMAGED:
					return VID_RESOURCE_CARRIER_DAMAGED;
				case MEV_RECYCLE:
				case MEV_TASKED:
					switch( MachPhysRandom::randomInt( 0, 2 ) )
					{
						case 0:
							return VID_RESOURCE_CARRIER_TASKED0;
						case 1:
							return VID_RESOURCE_CARRIER_TASKED1;
					}
				case MEV_MOVING:
					switch( MachPhysRandom::randomInt( 0, 3 ) )
					{
						case 0:
							return VID_RESOURCE_CARRIER_MOVING0;
						case 1:
							return VID_RESOURCE_CARRIER_MOVING1;
						case 2:
							return VID_RESOURCE_CARRIER_MOVING2;
					}
				case MEV_SELECTED:
					switch( MachPhysRandom::randomInt( 0, 2 ) )
					{
						case 0:
							return VID_RESOURCE_CARRIER_SELECTED0;
						case 1:
							return VID_RESOURCE_CARRIER_SELECTED1;
					}

				DEFAULT_ASSERT_BAD_CASE( id );
			}
		   		// ============================================= A P C =============================================
		case MachLog::APC:
			switch( id )
			{
				case MEV_SELF_DESTRUCT:
					return VID_APC_SELF_DESTRUCT;
				case MEV_BUILT:
					return VID_APC_BUILT;
				case MEV_DESTROYED:
					return VID_APC_DESTROYED;
				case MEV_NEAR_DEATH:
					return VID_APC_NEAR_DEATH;
				case MEV_CHANGED_RACE:
					switch( MachPhysRandom::randomInt( 0, 2 ) )
					{
						case 0:
							return VID_APC_CHANGED_RACE0;
						case 1:
							return VID_APC_CHANGED_RACE1;
					}
				case MEV_VIRUS_INFECTED:
					return VID_APC_VIRUS_INFECTED;
				case MEV_DAMAGED:
					return VID_APC_DAMAGED;
				case MEV_TASKED:
					switch( MachPhysRandom::randomInt( 0, 3 ) )
					{
						case 0:
							return VID_APC_TASKED0;
						case 1:
							return VID_APC_TASKED1;
						case 2:
							return VID_APC_TASKED2;
					}
				case MEV_MOVING:
					switch( MachPhysRandom::randomInt( 0, 3 ) )
					{
						case 0:
							return VID_APC_MOVING0;
						case 1:
							return VID_APC_MOVING1;
						case 2:
							return VID_APC_MOVING2;
					}
				case MEV_SELECTED:
					switch( MachPhysRandom::randomInt( 0, 2 ) )
					{
						case 0:
							return VID_APC_SELECTED0;
						case 1:
							return VID_APC_SELECTED1;
					}
				case MEV_RECYCLE:
					return VID_APC_RECYCLE;

				DEFAULT_ASSERT_BAD_CASE( id );
			}

		default:
			return VID_BALLISTA_RECYCLE;
	}
}

/* End MACHVMAN.CPP ****************************************************/
