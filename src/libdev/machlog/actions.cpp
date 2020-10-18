/*
 * A C T I O N S . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "ctl/map.hpp"
#include "utility/linetok.hpp"
#include "sim/conditio.hpp"
#include "sim/manager.hpp"
#include "machlog/actions.hpp"
//conditions
#include "machlog/condtime.hpp"
#include "machlog/condunit.hpp"
//#include "machlog/condhuni.hpp"
#include "machlog/condbmus.hpp"
#include "machlog/condrese.hpp"
#include "machlog/condreac.hpp"
#include "machlog/conddead.hpp"
#include "machlog/condvort.hpp"
#include "machlog/condtrea.hpp"
#include "machlog/condfire.hpp"
#include "machlog/condpod.hpp"
#include "machlog/condanya.hpp"
#include "machlog/condoded.hpp"
#include "machlog/condflag.hpp"
#include "machlog/condnot.hpp"
//actions
#include "machlog/actreinf.hpp"
#include "machlog/actvmail.hpp"
#include "machlog/actracec.hpp"
#include "machlog/actwin.hpp"
#include "machlog/actlost.hpp"
#include "machlog/actworl.hpp"
#include "machlog/actanim.hpp"
#include "machlog/actsquad.hpp"
#include "machlog/actenabl.hpp"
#include "machlog/actexplo.hpp"
#include "machlog/actflag.hpp"
#include "machlog/acttasks.hpp"

#include "machlog/vmman.hpp"
#include "machlog/races.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/network.hpp"
#include "machlog/stats.hpp"

MachLogActions::MachLogActions()
{

    TEST_INVARIANT;
}

MachLogActions::~MachLogActions()
{
    TEST_INVARIANT;

}

void MachLogActions::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogActions& t )
{

    o << "MachLogActions " << (void*)&t << " start" << std::endl;
    o << "MachLogActions " << (void*)&t << " end" << std::endl;

    return o;
}

//static
void MachLogActions::parseActionsSection( UtlLineTokeniser* pParser )
{
	HAL_STREAM("MachLogActions::parseActionsSection\n" );
	bool finished = false;
	bool parsedActions = false;
	bool parsedConditions = false;
	bool parsingConditions = true;
	bool parsingActions = false;

    typedef ctl_map< string , SimCondition*, std::less< string >	> ConditionNameMap;
	ConditionNameMap conditionMap;

	while( not finished )
	{
		HAL_STREAM("MLActions:: token 0 is " << pParser->tokens()[0] << std::endl );
		if( pParser->tokens()[0] == "ENDACTIONS" )
		{
			ASSERT( not parsedActions,"Already set actions parsed trying to do it again\n" );
			parsedActions = true;
			ASSERT( parsedConditions and parsedActions, "You cannot end the actions section without having parsed conditons and actions.\n" );
			finished = true;
			parsingActions = false;
		}
		else if( pParser->tokens()[0] == "CONDITIONS" )
		{
		}
		else if( pParser->tokens()[0] == "ENDCONDITIONS" )
		{
			ASSERT( parsingConditions,"Setting parsedCondtions but not in parsingConditons state\n" );
			ASSERT( not parsedConditions,"Already set conditions parsed trying to do it again\n" );
			parsingConditions = false;
			parsedConditions = true;
		}
		else if( pParser->tokens()[0] == "ACTIONS" )
		{
			ASSERT( not parsingActions,"Setting parsingActions but already set\n" );
			parsingActions = true;
		}
		else if( parsingConditions )
		{
			HAL_STREAM(" parsing conditions is true looking for match on token\n" );
			if( pParser->tokens()[0] == "TIMER" )
			{
				HAL_STREAM(" found a TIMER processing\n" );
				conditionMap.insert( pParser->tokens()[1], MachLogTimerCondition::newFromParser( pParser ) );
			}
			else if( pParser->tokens()[0] == "RESEARCH_COMPLETE" )
			{
				HAL_STREAM(" found a RESEARCH_COMPLETE processing\n" );
				conditionMap.insert( pParser->tokens()[1], MachLogResearchCompleteCondition::newFromParser( pParser ) );
			}
			else if( pParser->tokens()[0] == "LOW_UNIT_COUNT" )
			{
				HAL_STREAM(" found a LOW_UNIT_COUNT processing\n" );
				conditionMap.insert( pParser->tokens()[1], MachLogReachedLocationCondition::newFromParser( pParser, MachLogReachedLocationCondition::LESS_THAN ) );
//				conditionMap.insert( pParser->tokens()[1], MachLogLowUnitCountCondition::newFromParser( pParser ) );
			}
			else if( pParser->tokens()[0] == "HIGH_UNIT_COUNT" )
			{
				HAL_STREAM(" found a HIGH_UNIT_COUNT processing\n" );
				conditionMap.insert( pParser->tokens()[1], MachLogReachedLocationCondition::newFromParser( pParser, MachLogReachedLocationCondition::GREATER_THAN_EQUAL ) );
//				conditionMap.insert( pParser->tokens()[1], MachLogHighUnitCountCondition::newFromParser( pParser ) );
			}
			else if( pParser->tokens()[0] == "BMU_COUNT" )
			{
				HAL_STREAM(" found a BMU_COUNT processing\n" );
				conditionMap.insert( pParser->tokens()[1], MachLogBMUCountCondition::newFromParser( pParser ) );
			}
			else if( pParser->tokens()[0] == "REACHED_LOCATION" )
			{
				HAL_STREAM(" found a REACHED_LOCATION processing\n" );
				conditionMap.insert( pParser->tokens()[1], MachLogReachedLocationCondition::newFromParser( pParser ) );
			}
			else if( pParser->tokens()[0] == "VORTEX_FIRED" )
			{
				HAL_STREAM(" found a VORTEX_FIRED processing\n" );
				conditionMap.insert( pParser->tokens()[1], MachLogVortexHasBeenFiredCondition::newFromParser( pParser ) );
			}
			else if( pParser->tokens()[0] == "TECHNICIANS_TREACHERIED" )
			{
				HAL_STREAM(" found a TECHNICIANS_TREACHERIED processing\n" );
				conditionMap.insert( pParser->tokens()[1], MachLogTechniciansTreacheriedCondition::newFromParser( pParser ) );
			}
			else if( pParser->tokens()[0] == "ALL_UNITS_DEAD" )
			{
				HAL_STREAM(" found a ALL_UNITS_DEAD processing\n" );
				conditionMap.insert( pParser->tokens()[1], MachLogAllUnitsDeadCondition::newFromParser( pParser ) );
			}
			else if( pParser->tokens()[0] == "ALL_OTHER_RACES_UNITS_DEAD" )
			{
				HAL_STREAM(" found a ALL_OTHER_RACES_UNITS_DEAD processing\n" );
				conditionMap.insert( pParser->tokens()[1], MachLogAllOtherRacesUnitsDeadCondition::newFromParser( pParser ) );
			}
			else if( pParser->tokens()[0] == "FIRED_AT" )
			{
				HAL_STREAM(" found a FIRED_AT processing\n" );
				conditionMap.insert( pParser->tokens()[1], MachLogFiredAtCondition::newFromParser( pParser ) );
			}
			else if( pParser->tokens()[0] == "POD_CAPTURED" )
			{
				HAL_STREAM(" found a POD_CAPTURED processing\n" );
				conditionMap.insert( pParser->tokens()[1], MachLogPodCapturedCondition::newFromParser( pParser ) );
			}
			else if( pParser->tokens()[0] == "SCENARIO_FLAG" )
			{
				HAL_STREAM(" found an SCENARIO_FLAG line processing\n" );
				conditionMap.insert( pParser->tokens()[1], MachLogScenarioFlagCondition::newFromParser( pParser ) );
			}
			else if( pParser->tokens()[0] == "ANY" )
			{
				HAL_STREAM(" found an ANY line processing\n" );
				conditionMap.insert( pParser->tokens()[1], MachLogAnyAllCondition::newFromParser( pParser, &conditionMap, SimCondition::ANY ) );
			}
			else if( pParser->tokens()[0] == "ALL" )
			{
				HAL_STREAM(" found an ALL line processing\n" );
				conditionMap.insert( pParser->tokens()[1], MachLogAnyAllCondition::newFromParser( pParser, &conditionMap, SimCondition::ALL ) );
			}
			else if( pParser->tokens()[0] == "NOT" )
			{
				HAL_STREAM(" found an NOT line processing\n" );
				conditionMap.insert( pParser->tokens()[1], MachLogNotCondition::newFromParser( pParser, &conditionMap ) );
			}
		}
		else if( parsingActions )
		{
			HAL_STREAM(" parsing actions os true looking for match on a token\n" );
			size_t tokensSize = pParser->tokens().size();
			ASSERT( tokensSize >= 3,"There must be at least three tokens on an actions line <type> ON <keyname>\n" );
			SimCondition* pCondition = NULL;
			string conditionKeyName = pParser->tokens()[2];
			ASSERT_INFO( "ConditionKeyName " << conditionKeyName << std::endl );
			pCondition = conditionMap[ conditionKeyName ];
			bool enabled = true;
			SimAction* pAction = NULL;
			//check for disabled flag - this can actually appear any where on the line (irrelevant of action type -
			//so have to watch out for contentious keywords).
			for( int i = 0; i < tokensSize; ++i )
				if( pParser->tokens()[i] == "DISABLED" )
					enabled = false;

			if( pParser->tokens()[0] == "REINFORCEMENTS" )
			{
				HAL_STREAM(" found a REINFORCEMENTS processing\n" );
				pAction = MachLogReinforcementsAction::newFromParser( pCondition, enabled, pParser );
			}
			else if( pParser->tokens()[0] == "VOICE_MAIL" )
			{
				HAL_STREAM(" found a VOICE_MAIL processing\n" );
				if( MachLogVoiceMailManager::voiceMailsActivated() )
					pAction = MachLogVoiceMailAction::newFromParser( pCondition, enabled, pParser );
			}
			else if( pParser->tokens()[0] == "CHANGE_RACE" )
			{
				HAL_STREAM(" found a CHANGE_RACE processing\n" );
				pAction = MachLogChangeRaceAction::newFromParser( pCondition, enabled, pParser );
			}
			else if( pParser->tokens()[0] == "WON" )
			{
				HAL_STREAM(" found a WIN processing\n" );
				pAction = MachLogWinAction::newFromParser( pCondition, enabled, pParser );
			}
			else if( pParser->tokens()[0] == "LOST" )
			{
				HAL_STREAM(" found a LOST processing\n" );
				pAction = MachLogLostAction::newFromParser( pCondition, enabled, pParser );
			}
			else if( pParser->tokens()[0] == "WON_OR_LOST" )
			{
				HAL_STREAM(" found a WON_OR_LOST processing\n" );
				pAction = MachLogWinOrLoseAction::newFromParser( pCondition, enabled, pParser );
			}
			else if( pParser->tokens()[0] == "PLAY_ANIMATION" )
			{
				HAL_STREAM(" found a PLAY_ANIMATION processing\n" );
				pAction = MachLogPlayAnimationAction::newFromParser( pCondition, enabled, pParser );
			}
			else if( pParser->tokens()[0] == "ACTIVATE_SQUADRONS" )
			{
				HAL_STREAM(" found a ACTIVATE_SQUADRONS processing\n" );
				pAction = MachLogActivateSquadronsAction::newFromParser( pCondition, enabled, pParser );
			}
			else if( pParser->tokens()[0] == "ENABLE_ACTION" )
			{
				HAL_STREAM(" found a ENABLE_ACTION processing\n" );
				pAction = MachLogEnableActionAction::newFromParser( pCondition, enabled, pParser );
			}
			else if( pParser->tokens()[0] == "EXPLOSION" )
			{
				HAL_STREAM(" found a EXPLOSION processing\n" );
				pAction = MachLogExplosionAction::newFromParser( pCondition, enabled, pParser );
			}
			else if( pParser->tokens()[0] == "SET_SCENARIO_FLAG" )
			{
				HAL_STREAM(" found a SET_SCENARIO_FLAG processing\n" );
				pAction = MachLogSetFlagAction::newFromParser( pCondition, enabled, pParser );
			}
			else if( pParser->tokens()[0] == "TASK_STATE" )
			{
				HAL_STREAM(" found a TASK_STATE processing\n" );
				pAction = MachLogTaskStateAction::newFromParser( pCondition, enabled, pParser );
			}

			ASSERT( pAction,"the action has not been created - soemthing has gone horribly wrong.\n" );
			HAL_STREAM( "Adding action\n" );

			if( pAction )
			{
				SimManager::instance().addAction( pAction );
				pAction->enabled( enabled );
			}


		}
		if( not finished )
			pParser->parseNextLine();
	}

    //Delete any unreferenced conditions
    // Calling destructor of parent condition deletes its linked children
    // In next iterations over map, attempt to read & delete deleted condition may occur hence collect them into list first
    std::vector<SimCondition*> unusedConditions;
    for( ConditionNameMap::iterator it = conditionMap.begin(); it != conditionMap.end(); ++it )
    {
        SimCondition* pCondition = (*it).second;
        if( pCondition->nLinkedActions() == 0 )
            unusedConditions.push_back(pCondition);
    }
    for( std::vector<SimCondition*>::iterator it = unusedConditions.begin(); it != unusedConditions.end(); ++it )
    {
        SimCondition* pCondition = (*it);
        _DELETE(pCondition);
    }
}

void MachLogActions::registerDerivedClasses()
{
	PER_REGISTER_DERIVED_CLASS( MachLogTimerCondition );
	PER_REGISTER_DERIVED_CLASS( MachLogResearchCompleteCondition );
	PER_REGISTER_DERIVED_CLASS( MachLogLowUnitCountCondition );
///	PER_REGISTER_DERIVED_CLASS( MachLogHighUnitCountCondition );
	PER_REGISTER_DERIVED_CLASS( MachLogBMUCountCondition );
	PER_REGISTER_DERIVED_CLASS( MachLogReachedLocationCondition );
	PER_REGISTER_DERIVED_CLASS( MachLogVortexHasBeenFiredCondition );
	PER_REGISTER_DERIVED_CLASS( MachLogTechniciansTreacheriedCondition );
	PER_REGISTER_DERIVED_CLASS( MachLogAllUnitsDeadCondition );
	PER_REGISTER_DERIVED_CLASS( MachLogAllOtherRacesUnitsDeadCondition );
	PER_REGISTER_DERIVED_CLASS( MachLogFiredAtCondition );
	PER_REGISTER_DERIVED_CLASS( MachLogPodCapturedCondition );
	PER_REGISTER_DERIVED_CLASS( MachLogAnyAllCondition );
	PER_REGISTER_DERIVED_CLASS( MachLogScenarioFlagCondition );
	PER_REGISTER_DERIVED_CLASS( MachLogNotCondition );
	PER_REGISTER_DERIVED_CLASS( MachLogReinforcementsAction );
	PER_REGISTER_DERIVED_CLASS( MachLogVoiceMailAction );
	PER_REGISTER_DERIVED_CLASS( MachLogChangeRaceAction );
	PER_REGISTER_DERIVED_CLASS( MachLogWinAction );
	PER_REGISTER_DERIVED_CLASS( MachLogLostAction );
	PER_REGISTER_DERIVED_CLASS( MachLogWinOrLoseAction );
	PER_REGISTER_DERIVED_CLASS( MachLogPlayAnimationAction );
	PER_REGISTER_DERIVED_CLASS( MachLogActivateSquadronsAction );
	PER_REGISTER_DERIVED_CLASS( MachLogEnableActionAction );
	PER_REGISTER_DERIVED_CLASS( MachLogExplosionAction );
	PER_REGISTER_DERIVED_CLASS( MachLogSetFlagAction );
	PER_REGISTER_DERIVED_CLASS( MachLogTaskStateAction );
	PER_REGISTER_DERIVED_CLASS( MachLogTaskStateAction );
}

//static
void MachLogActions::createDynamically( MachLog::VictoryCondition vc, const PhysRelativeTime& timerFireDelay )
{
	NETWORK_STREAM("MachLogActions::createDynamically vc " << vc << " timerFireDelay " << timerFireDelay << std::endl );
	PRE( vc != MachLog::VICTORY_DEFAULT );
	MachLogRaces& races = MachLogRaces::instance();
	bool createEnableActions = MachLogNetwork::instance().isNetworkGame();
	NETWORK_STREAM("hasPCRace() " << races.hasPCRace() << std::endl );

	//create an all other races dead condition and attach it to a win action for the pcControlled race
	//create disabled in network games as we have to allow for the connection time before we start checking for win/lost
	MachLogAllOtherRacesUnitsDeadCondition* pAllOthersDead = _NEW( MachLogAllOtherRacesUnitsDeadCondition( "dynamic_all_other_races_dead", races.pcController().race() ) );
	MachLogWinAction* pWin = MachLogWinAction::newDynamic( pAllOthersDead, not createEnableActions, races.pcController().race() );
	SimManager::instance().addAction( pWin );
	pWin->enabled( not createEnableActions );
	//create an all dead condition and attach it to a lost action for the pcControlled race
	//create disabled in network games as we have to allow for the connection time before we start checking for win/lost
	MachLogAllUnitsDeadCondition* pAllDead = _NEW( MachLogAllUnitsDeadCondition( "dynamic_all_dead", races.pcController().race() ) );
	MachLogLostAction* pLost = MachLogLostAction::newDynamic( pAllDead, not createEnableActions, races.pcController().race() );
	SimManager::instance().addAction( pLost );
	pLost->enabled( not createEnableActions );

	if( createEnableActions )
	{
	    MachLogTimerCondition* pTimerForLost = _NEW( MachLogTimerCondition( "dynamic_timer_for_lost", 10 ) );
	    MachLogTimerCondition* pTimerForWin = _NEW( MachLogTimerCondition( "dynamic_timer_for_win", 10 ) );
		MachLogEnableActionAction *pEnableLost = MachLogEnableActionAction::newDynamic( pTimerForLost, true, "dynamic_all_dead" );
		MachLogEnableActionAction *pEnableWin = MachLogEnableActionAction::newDynamic( pTimerForWin, true, "dynamic_all_other_races_dead" );
		SimManager::instance().addAction( pEnableLost );
		SimManager::instance().addAction( pEnableWin );
	}


	switch( vc )
	{
		case MachLog::VICTORY_ANNIHILATION:
			break;
		case MachLog::VICTORY_POD:
		{
			//create a LOW_UNIT_COUNT on the POD and a LOST action to go with it.
		    MachLogLowUnitCountCondition* pLUC = _NEW( MachLogLowUnitCountCondition( "dynamic_low_unit_count", races.pcController().race(), MachLog::POD, 1, 0 ) );
			MachLogLostAction* pPodLost = MachLogLostAction::newDynamic( pLUC, not createEnableActions, races.pcController().race() );
			SimManager::instance().addAction( pPodLost );
			pPodLost->enabled( not createEnableActions );
			if( createEnableActions )
			{
			    MachLogTimerCondition* pTimerForPodLost = _NEW( MachLogTimerCondition( "dynamic_timer_for_pod_lost", 10 ) );
				MachLogEnableActionAction *pEnablePodLost = MachLogEnableActionAction::newDynamic( pTimerForPodLost, true, "dynamic_low_unit_count" );
				SimManager::instance().addAction( pEnablePodLost );
			}
			for( MachPhys::Race i = MachPhys::RED; i < MachPhys::N_RACES; ++((int&)i) )
			{
				bool doWork;
				doWork = false;
				if( MachLogRaces::instance().raceInGame( i ) )
				{
					if( MachLogRaces::instance().controller( i ).type() == MachLogController::AI_CONTROLLER )
					{
						if( MachLogNetwork::instance().isNetworkGame() )
						{
							if( MachLogNetwork::instance().remoteStatus( i ) == MachLogNetwork::LOCAL_PROCESS )
							{
								doWork = true;
							}
						}
						else
						{
							doWork = true;
						}

					}
				}
				if( doWork )
				{
					//create a LOW_UNIT_COUNT on the POD and a LOST action to go with it.
					string raceExtensions[] = { "_RED", "_BLUE", "_GREEN", "_YELLOW" };
					string nameLow = "dynamic_low_unit_count";
					nameLow += raceExtensions[ i ];
				    MachLogLowUnitCountCondition* pLUCr = _NEW( MachLogLowUnitCountCondition( nameLow, i, MachLog::POD, 1, 0 ) );
					MachLogLostAction* pPodLostr = MachLogLostAction::newDynamic( pLUCr, not createEnableActions, i );
					SimManager::instance().addAction( pPodLostr );
					pPodLostr->enabled( not createEnableActions );
					if( createEnableActions )
					{
						string nameTimer = "dynamic_timer_for_pod_lost";
						nameTimer += raceExtensions[ i ];
					    MachLogTimerCondition* pTimerForPodLostr = _NEW( MachLogTimerCondition( nameTimer, 10 ) );
						MachLogEnableActionAction *pEnablePodLostr = MachLogEnableActionAction::newDynamic( pTimerForPodLostr, true, nameLow );
						SimManager::instance().addAction( pEnablePodLostr );
					}
				}
			}
		}
		break;
		case MachLog::VICTORY_TIMER:
		{
			//create a WinOrLose action which will get fired by a timer with the delay set (indirectly) from the gameData
		    MachLogTimerCondition* pTimer = _NEW( MachLogTimerCondition( "dynamic_timer", timerFireDelay ) );
			MachLogWinOrLoseAction* pWinOrLose = MachLogWinOrLoseAction::newDynamic( pTimer, not createEnableActions, races.pcController().race() );
			SimManager::instance().addAction( pWinOrLose );
			pWinOrLose->enabled( not createEnableActions );
			if( createEnableActions )
			{
			    MachLogTimerCondition* pTimerForTimerWinOrLose = _NEW( MachLogTimerCondition( "dynamic_timer_for_timer_win_or_lose", 10 ) );
				MachLogEnableActionAction *pEnableTimer = MachLogEnableActionAction::newDynamic( pTimerForTimerWinOrLose, true, "dynamic_timer" );
				SimManager::instance().addAction( pEnableTimer );
			}
		}
		break;
		default:
			ASSERT_INFO( vc );
			ASSERT_BAD_CASE;
	}
	NETWORK_STREAM("MachLogActions::createDynamically DONE vc " << vc << " timerFireDelay " << timerFireDelay << std::endl );
}
/* End ACTIONS.CPP **************************************************/
