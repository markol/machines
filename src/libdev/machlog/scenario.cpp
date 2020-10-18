/*
 * S C E N A R I O . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

//  TBD: This should be in mcmotseq.hpp, it will cause big recompiles though - Bob
#include "stdlib/string.hpp"
#include <memory>

#include "system/metafile.hpp"
#include "system/metaistr.hpp"

#include "ctl/pvector.hpp"
#include "ctl/list.hpp"
#include "utility/linetok.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/degrees.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/poly2d.hpp"
#include "mathex/circle2d.hpp"

#include "network/netnet.hpp"

#include "phys/motchunk.hpp"
#include "phys/cspace2.hpp"

#include "world4d/generic.hpp"
#include "world4d/domain.hpp"

#include "sim/manager.hpp"
#include "sim/conditim.hpp"

#include "machphys/machine.hpp"
#include "machphys/constron.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"
#include "machlog/scenario.hpp"
#include "machlog/administ.hpp"
#include "machlog/aggressr.hpp"
#include "machlog/beacon.hpp"
#include "machlog/construc.hpp"
#include "machlog/cntrl_ai.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/factory.hpp"
#include "machlog/garrison.hpp"
#include "machlog/hwlab.hpp"
#include "machlog/geolocat.hpp"
#include "machlog/mine.hpp"
#include "machlog/minesite.hpp"
#include "machlog/missilem.hpp"
#include "machlog/oreholo.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/planet.hpp"
#include "machlog/pod.hpp"
#include "machlog/produnit.hpp"
#include "machlog/smelter.hpp"
#include "machlog/rescarr.hpp"
#include "machlog/technici.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/spacial.hpp"
#include "machlog/spy.hpp"
#include "machlog/apc.hpp"
#include "machlog/resitem.hpp"
#include "machlog/stats.hpp"
#include "machlog/network.hpp"
#include "machlog/actmaker.hpp"
#include "machlog/artfacts.hpp"
#include "machlog/actions.hpp"
#include "machlog/squad.hpp"
#include "machlog/creatdat.hpp"
#include "machlog/consitem.hpp"
#include "machlog/constree.hpp"


//static
void MachLogScenario::load
(
	const SysPathName& scenarioFilePath, const MachLogGameCreationData& gameData
)
{
	MachLogRaces& races = MachLogRaces::instance();
	SysPathName fullPath( "data" );
	fullPath.combine( scenarioFilePath );

    ASSERT_INFO( scenarioFilePath );
    ASSERT_INFO( fullPath );

	const SysPathName factoryItemsPath( "data/factory.bld" );

    SysMetaFile metaFile( "mach1.met" );

    std::auto_ptr< std::istream > pIstream;

    if( SysMetaFile::useMetaFile() && metaFile.hasFile(fullPath) )
    {
        //pIstream = _NEW( SysMetaFileIstream( metaFile, fullPath, ios::text ) );
        pIstream = std::auto_ptr< std::istream > (
            _NEW( SysMetaFileIstream( metaFile, fullPath, std::ios::in ) ));
    }
    else
    {
        ASSERT_FILE_EXISTS( fullPath.c_str() );
        //pIstream = _NEW( ifstream( fullPath.c_str(), ios::text | ios::in ) );
        pIstream = std::auto_ptr< std::istream > (
            _NEW( std::ifstream( fullPath.c_str(), std::ios::in ) ));
    }

    UtlLineTokeniser parser( *pIstream, fullPath );

    //Read definitions until finished
	bool doneRace[ MachPhys::N_RACES ] = { false, false, false ,false };
	bool doneMineralSites = false;
	string researchItemsPath[ MachLog::TECH_LEVEL_HIGH + 1 ];
	NETWORK_STREAM("MLScenario::load\nGame creation data\n" << gameData << std::endl );
	//Construct a non-race for the artefacts etc
    races.race( MachPhys::NORACE, _NEW( MachLogRace( MachPhys::NORACE ) ), MachLogRaces::CREATE_SQUADRONS );

	MachPhys::Race loadingRemappedRace[ MachPhys::N_RACES ] = { MachPhys::RED, MachPhys::BLUE, MachPhys::GREEN, MachPhys::YELLOW };

	// counter that is incremented every time a genuine new construction (i.e. not an alternative site entry)
	// is encountered for a race. Starts at -1 which is an illegal number, incremented by the first genuine construction
	// to 0, and thus usable as a test to check for alternative sites - cannot have an alternative site when no
	// genuine sites have yet been encountered in the file.
	int constructionId = -1;

	// pointer to last genuine new construction production unit
	MachLogProductionUnit* pLastProd = NULL;

	// this is used for safety checking - no "alternative site" should be specified for a construction
	// that is going to be actually built rather than stored as a production unit
	bool lastEncounteredWasUnbuiltConstruction = false;

	const MachLogGameCreationData::PlayersCreationData& playersCreationData = gameData.playersCreationData();
	MachLog::RandomStarts randomStarts = gameData.randomStarts();
	MachLog::ResourcesAvailable resources = gameData.resourcesAvailable();

	//if we have defined player creation data then we have to use it to override whatever is specified in the scenario
	//file. The new format files must specify the RACES keyword.
	ASSERT_INFO( playersCreationData.size() );
	//assert for the correct number of entires in the players creation data array.
	//if a player has not been defined then the entry for that race should indicate this by the NOT_DEFINED marker.
	ASSERT( playersCreationData.size() == 0 or playersCreationData.size() == 4,"Defined players data must have 0 or 4 entries\n" );
	bool useCreationData = playersCreationData.size() > 0;
	if( not useCreationData )
		races.gameType( MachLog::CAMPAIGN_SINGLE_PLAYER );
	else if( MachLogNetwork::instance().isNetworkGame() )
		races.gameType( MachLog::MULTIPLAYER );
	else
		races.gameType( MachLog::SKIRMISH_SINGLE_PLAYER );


    //Need to know whether to assert all races have assembly points defined, or just ai races
    bool checkAllRacesHaveAssemblyPoints = useCreationData;
    bool hadAggressorAssemblyPoint = false;
    bool hadAdministratorAssemblyPoint = false;

	//set up default array data.
	MachLogGameCreationData::PlayersCreationData defaultData;
    defaultData.reserve( MachPhys::N_RACES );
	MachLogGameCreationData::PlayersCreationData useData;
    useData.reserve( MachPhys::N_RACES );

	//all defaults to not defined.
	for( MachPhys::Race i = MachPhys::RED; i < MachPhys::N_RACES; ++((int&)i) )
	{
		MachLogGameCreationData::PlayerCreationData data;
		data.colour_ = i;
		data.type_ = MachLog::NOT_DEFINED;
		defaultData.push_back( data );
		useData.push_back( data );
	}

	//get the default races data from the beginning of the file
	//look for races token
	while( not ( parser.tokens()[0] == "RACES_DEFAULT" ) )
		parser.parseNextLine();

	bool racesFinished = false;
	while( not ( parser.tokens()[0] == "ENDRACES_DEFAULT" ) )
	{
		const string& token = parser.tokens()[0];
		if( token == "RED" or
			token == "BLUE" or
			token == "GREEN" or
			token == "YELLOW" )
		{
			MachPhys::Race r = machPhysRace( token );
			const string& typeToken = parser.tokens()[1];
			if( typeToken == "PC_LOCAL" or
				typeToken == "PC" )
				defaultData[r].type_ = MachLog::PC_LOCAL;
			else if( typeToken == "AI_LOCAL" or typeToken == "AI" )
				defaultData[r].type_ = MachLog::AI_LOCAL;
		}
		parser.parseNextLine();

	}

	for( MachPhys::Race i = MachPhys::RED; i < MachPhys::N_RACES; ++((int&)i) )
	{
		if( useCreationData )
		{
			useData[i].type_ = playersCreationData[i].type_;
			useData[i].colour_ = playersCreationData[i].colour_;
		}
		else
		{
			useData[i].type_ = defaultData[i].type_;
		}
		HAL_STREAM("MachLogScenario::Load useData element " << i << " " << useData[i] << std::endl );
	}
	//we now have the actual data specified in the useData array now we must look for races marked as active
	//that have not been defined by the scenario file. The scenario file data is stored in the defaultData array.
	//handle random starts to fixed start locations in different ways.
	{
		//first things first set up an array of booleans as a short hand for which races have been marked as used
		bool gotRace[ MachPhys::N_RACES ] = { false, false, false, false };
		bool usedRace[ MachPhys::N_RACES ] = { false, false, false, false };
		for( MachPhys::Race i = MachPhys::RED; i < MachPhys::N_RACES; ++((int&)i) )
		{
			if( defaultData[i].type_ != MachLog::NOT_DEFINED )
				gotRace[i] = true;
		}
        // TBD: Fix this - Hal
//		if( randomStarts == MachLog::FIXED_START_LOCATIONS )
		{
			//go through each of the races and mark which ones we have matches for already
			for( MachPhys::Race i = MachPhys::RED; i < MachPhys::N_RACES; ++((int&)i) )
				if( useData[i].type_ != MachLog::NOT_DEFINED and
					gotRace[i] )
					usedRace[i] = true;
			//now go through each of the races and see if a useData does not have usedRace marked.
			for( MachPhys::Race i = MachPhys::RED; i < MachPhys::N_RACES; ++((int&)i) )
				if( useData[i].type_ != MachLog::NOT_DEFINED and not usedRace[i] )
				{
					//we need to allocate this useData element to a race which has been defined by the scenario file
					for( MachPhys::Race j = MachPhys::RED; j < MachPhys::N_RACES; ++((int&)j) )
					{
						//match to first unused race.
						if( not usedRace[j] )
						{
							//swap elements around.
							useData[j].colour_ = useData[i].colour_;
							useData[j].type_ = useData[i].type_;
							useData[i].type_ = MachLog::NOT_DEFINED;
							usedRace[j] = true;
                            break;
						}
					}
				}
		}

	}

	HAL_STREAM("MachLogScneario::Load useData after processing\n" );
	for( MachPhys::Race i = MachPhys::RED; i < MachPhys::N_RACES; ++((int&)i) )
	{
		HAL_STREAM("MachLogScenario::Load useData element " << i << " " << useData[i] << std::endl );
	}

	MachLogNetwork& network = MachLogNetwork::instance();

    while( not parser.finished() )
    {
		if( MachLogNetwork::instance().isNetworkGame() )
			NetNetwork::instance().pollMessages();

    	size_t lineSize = parser.tokens().size();
		bool running = true;
		bool stopProcessingRace = false;
		bool instantiateObjects = true;
        W4dEntity* pAnyDomain;
		W4dEntity* pPhysObject;
        MexTransform3d localTransform;
		MachLogRace* pRace;
		MachLogController* pCtl;
		MachLogAIController* pAICtl;
		MachPhys::Race	race;
		bool doingAIRace = false;

		while( 	not parser.finished() and not
				( 	parser.tokens()[0] == "RACE" or
					parser.tokens()[0] == "MINERAL" or
					parser.tokens()[0] == "RESEARCH_ITEMS" or
					parser.tokens()[0] == "ARTEFACTS" or
					parser.tokens()[0] == "CONDITIONS" or
					parser.tokens()[0] == "RESTRICT_CONSTRUCTION") )
		{
			parser.parseNextLine();
			HAL_STREAM("Token 0 : " << parser.tokens()[0] << std::endl );
		}

		pRace = NULL;

		bool parsingMineralSites = false;

        if( parser.tokens()[0] == "ARTEFACTS" )
        {
    		parser.parseNextLine();
			if( network.isNetworkGame() )
			{
				if( network.isNodeLogicalHost() )
    		        races.artefacts().parseArtefactsSection( &parser, MachLogArtefacts::CREATE_ARTEFACT_INSTANCES );
			}
			else
				races.artefacts().parseArtefactsSection( &parser , MachLogArtefacts::CREATE_ARTEFACT_INSTANCES );
            stopProcessingRace = true;
        }
        else if( parser.tokens()[0] == "CONDITIONS" )
        {
			if( gameData.victoryCondition() == MachLog::VICTORY_DEFAULT )
				MachLogActions::parseActionsSection( &parser );
            stopProcessingRace = true;
        }
		else if(parser.tokens()[1] == "RED" or
		   parser.tokens()[1] == "BLUE" or
		   parser.tokens()[1] == "GREEN" or
		   parser.tokens()[1] == "YELLOW" )
        {
            hadAggressorAssemblyPoint = false;
            hadAdministratorAssemblyPoint = false;

			MachPhys::Race parsedRace = machPhysRace( parser.tokens()[1] );
			HAL_STREAM("parsed Race " << parsedRace << " which maps to " );
			race = useData[parsedRace].colour_;
			HAL_STREAM( race << std::endl );
			HAL_STREAM("MachLogScenario::Load processing race " << race << std::endl );
			HAL_STREAM(" useData " << race << " " << useData[parsedRace] << std::endl );
			//TBD:
			// ***************************************************************************************************
			//this bit here needs recoding to cope with all kinds of bollocks to do with paced in vector of colours
			//and giggling around to say if rem\pped colur is MachPhys::_NRACES then don't do any work
			//I shall do all this tomorrow
			//um my thinking has changed - left comment in to remind myself anyway.
			// ***************************************************************************************************

			if( useData[parsedRace].type_ != MachLog::NOT_DEFINED )
			{
	        	ASSERT( not doneRace[race], runtime_error());
	            doneRace[race] = true;
				pRace = _NEW( MachLogRace( race ) );
	        	ASSERT( pRace != NULL, runtime_error());

				races.race( race, pRace, MachLogRaces::CREATE_SQUADRONS );

			    pAnyDomain = MachLogPlanetDomains::pDomainPosition( MexPoint3d(0,0,0), 0, &localTransform );
			    pPhysObject = _NEW( W4dGeneric( pAnyDomain, localTransform ) );
			}
			//create PC controller if No AI is indicated
			if( useData[parsedRace].type_ == MachLog::NOT_DEFINED )
			{
				stopProcessingRace = true;
			}
			else if( useData[parsedRace].type_ == MachLog::PC_LOCAL )
			{
                MachLogPCController* pPCController;
				pCtl = pPCController = _NEW( MachLogPCController( pRace, pPhysObject ) );
				if( not network.isNetworkGame() or
					network.localRace() == race )
				{
    				races.pcController( pPCController );
					if( network.isNetworkGame() )
						network.remoteStatus( race, MachLogNetwork::LOCAL_PROCESS );
				}
				races.defCon( race, MachLog::DEFCON_NORMAL );
				doingAIRace = false;
				pRace->priority( races.stats().pcPriority() );
				instantiateObjects = true;
			}
			else if( useData[parsedRace].type_ == MachLog::PC_REMOTE )
			{

				//register the controller abstract class.
                MachLogPCController* pPCController;
				pCtl = pPCController = _NEW( MachLogPCController( pRace, pPhysObject ) );
    			races.controller( race, pCtl );
				pRace->toBeUpdated( SimProcess::MANAGER_NOT_UPDATE );
				stopProcessingRace = true;
				network.remoteStatus( race, MachLogNetwork::REMOTE_PROCESS );
				races.defCon( race, MachLog::DEFCON_NORMAL );
				doingAIRace = false;
				pRace->priority( races.stats().pcPriority() );
				instantiateObjects = false;
            }
			else if( useData[parsedRace].type_ == MachLog::AI_LOCAL or useData[parsedRace].type_ == MachLog::AI_REMOTE )
            {
				pCtl = _NEW( MachLogAIController( pRace, pPhysObject, parser.tokens()[2] ) );
				pAICtl = (MachLogAIController*)pCtl;
				pAICtl->checkForDynamicAllies( false );
				//if use creation data is true then we came from skirmish/multiplayer
				if( useCreationData )
				{
					//if dynamic allies have not been explicitly revoked...
					if( !getenv( "CB_NO_AI_ALLY" ) )
						pAICtl->checkForDynamicAllies( true );
				}
    			races.controller( race, pCtl );
				races.defCon( race, MachLog::DEFCON_LOW );
				doingAIRace = true;
				pRace->priority( races.stats().aiPriority() );
				if( network.isNetworkGame() )
				{
					if( network.isNodeLogicalHost() )
					{
						network.ready( race, true );
						network.remoteStatus( race, MachLogNetwork::LOCAL_PROCESS );
						instantiateObjects = true;
					}
					else
					{
						pRace->toBeUpdated( SimProcess::MANAGER_NOT_UPDATE );
						instantiateObjects = false;
						network.remoteStatus( race, MachLogNetwork::REMOTE_PROCESS );
					}
				}
            }
			HAL_STREAM(" doing " << race << " race\n" );
        }
		else if(parser.tokens()[1] == "SITES")
        {
        	ASSERT( not doneMineralSites, runtime_error());

            doneMineralSites = true;
			parsingMineralSites = true;
        }
		else if(parser.tokens()[0] == "RESEARCH_ITEMS")
        {
			researchItemsPath[ MachLog::TECH_LEVEL_DEFAULT ] = parser.tokens()[1];
			researchItemsPath[ MachLog::TECH_LEVEL_LOW ] = parser.tokens()[1];
			researchItemsPath[ MachLog::TECH_LEVEL_MEDIUM ] = parser.tokens()[1];
			researchItemsPath[ MachLog::TECH_LEVEL_HIGH ] = parser.tokens()[1];
	    	lineSize = parser.tokens().size();
			if( lineSize > 2 )
				researchItemsPath[ MachLog::TECH_LEVEL_LOW ] = parser.tokens()[2];
			if( lineSize > 3 )
				researchItemsPath[ MachLog::TECH_LEVEL_MEDIUM ] = parser.tokens()[3];
			if( lineSize > 4 )
				researchItemsPath[ MachLog::TECH_LEVEL_HIGH ] = parser.tokens()[4];
        }
		else if(parser.tokens()[0] == "RESTRICT_CONSTRUCTION")
        {
			parseRestrictConstruction( parser );
			stopProcessingRace = true;
        }
        else
        {
        	//Bad race
        	ASSERT(false, "");
        }
		HAL_STREAM("parser.parseNextLine - parser.finished " << parser.finished() << std::endl );
		parser.parseNextLine();
    	lineSize = parser.tokens().size();
		running = !stopProcessingRace;
//        running = true;
		bool parsingMachines = false;
		bool parsingConstructions = false;
		bool displayAsOtherRace = false;
		MachPhys::Race displayAsRace = MachPhys::RED;

		while( running )
        {
	    	lineSize = parser.tokens().size();
			#ifndef NDEBUG
			if( lineSize < 1 or lineSize > 10 )
            {
             	//Bad line
				ASSERT_INFO( lineSize );
				ASSERT(false, "LineSize is unknown size in scenario.\n" );
            }
			#endif

			if( lineSize == 1 )
			{
				if( parser.tokens()[ 0 ] == "MACHINES" )
				{
					parsingMachines = true;
					parsingConstructions = false;
					parsingMineralSites = false;
				}
				else if( parser.tokens()[ 0 ] == "CONSTRUCTIONS" )
				{
					parsingMachines = false;
					parsingConstructions = true;
					parsingMineralSites = false;
				}
				else if( parser.tokens()[ 0 ] == "ENDDISPLAY" )
				{
					displayAsOtherRace = false;
				}
			}
			else if( lineSize == 2)
			{
				if( parser.tokens()[0] == "NBMU" )
				{
					int nBMU = atol(parser.tokens()[1].c_str() );
					HAL_STREAM("MLScenario::NBMU line " << nBMU << " for race " << race << std::endl );
					//Override this if the game data says don't use default starting resources
					if( gameData.startingResources() == MachLog::STARTING_RESOURCES_DEFAULT )
						races.nBuildingMaterialUnits( race, nBMU );
					else
						races.nBuildingMaterialUnits( race, startingResourcesToInt( gameData.startingResources() ) );
				}
				if( parser.tokens()[0] == "SCORE" )
				{
					if( parser.tokens()[1] == "OFF" )
						races.scoreShouldBeDisplayed( race, false );
				}
				else if( parser.tokens()[0] == "NORE" )
				{
					int nOre = atol(parser.tokens()[1].c_str() );
					HAL_STREAM("MLScenario::NORE line " << nOre << " for race " << race << std::endl );
					races.nOre( race ) = nOre;
				}
				else if( parser.tokens()[0] == "ALLY" )
				{
					MachPhys::Race allyRace = machPhysRace( parser.tokens()[1] );
					races.dispositionToRace( race, allyRace, MachLogRaces::ALLY );
				}
				else if( parser.tokens()[0] == "VIRTUAL_BEACON" )
				{
					MachLog::BeaconType beaconType = virtualBeaconType( parser.tokens()[1] );
					races.virtualBeacon( race, beaconType );
				}
			}
			else if( lineSize == 3 )
			{
			        if( parser.tokens()[0] == "DISPLAY" )
			        {
			                displayAsOtherRace = true;
			                displayAsRace = machPhysRace( parser.tokens()[2] );
			        }
			}
			else if( lineSize == 4 )
			{
				if( parser.tokens()[0] == "ASSEMBLY" )
				{
					MexPoint2d pos( atof(parser.tokens()[2].c_str() ), atof(parser.tokens()[3].c_str() ) );

                    //Check a valid position
                    MexCircle2d testCircle( pos, 1.0 );
                    PhysConfigSpace2d::PolygonId badId;
                    ASSERT_INFO( pos );
                    ASSERT( MachLogPlanet::instance().configSpace().contains( testCircle, 0, &badId ), "Assembly point off planet or in obstacle" );

					if( parser.tokens()[1] == "AGGRESSOR" )
                    {
                        hadAggressorAssemblyPoint = true;
						races.addAggressorAssemblyPoint( race, pos );
                    }
					else if(parser.tokens()[1] == "ADMINISTRATOR" )
                    {
                        hadAdministratorAssemblyPoint = true;
						races.addAdministratorAssemblyPoint( race, pos );
                    }
					else
					{
						ASSERT_INFO( parser.tokens()[1] );
						ASSERT( false,"Unknown assembly point type. Not agg or add\n" );
					}
				}
			}
			else if( ( lineSize >= 6 and lineSize <= 10 ) and parsingMachines )
            {
				if( instantiateObjects )
				{
					//Parse machine details
	            	string machineType = parser.tokens()[0];
	            	size_t hwLevel = atoi(parser.tokens()[1].c_str());
	            	size_t swLevel = atoi(parser.tokens()[2].c_str());
	                ASSERT(hwLevel <= 10, runtime_error());
	                ASSERT(swLevel <= 10, runtime_error());
	            	MATHEX_SCALAR xPos = atof(parser.tokens()[3].c_str());
	            	MATHEX_SCALAR yPos = atof(parser.tokens()[4].c_str());
	            	MATHEX_SCALAR zPos = atof(parser.tokens()[5].c_str());
					HAL_STREAM(" machine " << machineType << std::endl );
					MachLog::ObjectType ot = objectType( parser.tokens()[0] );
					MachLogMachine* pMachine = NULL;
					int squadId = -1;		//special case value - if +ve then machine will get added to squadron directly

					if( doingAIRace )
						for( int i = 0; i < parser.tokens().size(); ++i )
							if( parser.tokens()[i] == "SQUAD" )
								squadId = atol( parser.tokens()[i+1].c_str() );

					if( ot == MachLog::GEO_LOCATOR or
						ot == MachLog::SPY_LOCATOR or
						ot == MachLog::RESOURCE_CARRIER or
						ot == MachLog::APC )
					{
						pMachine = MachLogActorMaker::newLogMachine( ot, 0, hwLevel, swLevel, pRace->race(), MexPoint3d( xPos, yPos, zPos ), MachPhys::N_WEAPON_COMBOS );
					}
	                else if( ot == MachLog::TECHNICIAN )
	                {
						pMachine = MachLogActorMaker::newLogMachine( ot, technicianSubType( parser.tokens()[6] ), hwLevel, swLevel, pRace->race(), MexPoint3d( xPos, yPos, zPos ), MachPhys::N_WEAPON_COMBOS );
	                }
	                else if(ot == MachLog::CONSTRUCTOR)
	                {
						pMachine = MachLogActorMaker::newLogMachine( ot, constructorSubType( parser.tokens()[6] ), hwLevel, swLevel, pRace->race(), MexPoint3d( xPos, yPos, zPos ), MachPhys::N_WEAPON_COMBOS );
	                }
	                else if(ot == MachLog::AGGRESSOR)
	                {
						pMachine = MachLogActorMaker::newLogMachine( ot, aggressorSubType( parser.tokens()[6] ), hwLevel, swLevel, pRace->race(), MexPoint3d( xPos, yPos, zPos ), weaponCombo( parser.tokens()[7] ) );
	                }
	                else if(ot == MachLog::ADMINISTRATOR)
	                {
						pMachine = MachLogActorMaker::newLogMachine( ot, administratorSubType( parser.tokens()[6] ), hwLevel, swLevel, pRace->race(), MexPoint3d( xPos, yPos, zPos ), weaponCombo( parser.tokens()[7] ) );
	                }
	                else
	                {
	                	//Bad Machine type
						ASSERT_INFO( machineType );
	                    ASSERT(false, "Unreconginised machine type\n");
	                }
					if( displayAsOtherRace )
					{
						pMachine->displayAsRace( displayAsRace );
					}

					if( squadId != -1 )
					{
						pMachine->squadron( races.squadrons( race )[squadId] );
					}
				}

            }
            else if( ( lineSize >= 5 and lineSize <= 10 ) and parsingConstructions )
            {
				bool isAlternativeSite = false;
				MachLog::ObjectType ot;
				MATHEX_SCALAR xPos;
            	MATHEX_SCALAR yPos;
            	MATHEX_SCALAR zPos;
            	MATHEX_SCALAR orientation;
				bool needRebuild = false;
				bool notBuilt = false;
				size_t buildingLevel;

				//Parse building details
            	string buildingType = parser.tokens()[0];
				if( buildingType == "ALTERNATIVE_SITE" )
				{
					ASSERT( lineSize == 5, "Incorrect number of tokens for ALTERNATIVE_SITE line." );

					xPos = atof(parser.tokens()[1].c_str());
	            	yPos = atof(parser.tokens()[2].c_str());
	            	zPos = atof(parser.tokens()[3].c_str());
	            	orientation = atof(parser.tokens()[4].c_str());
					isAlternativeSite = true;
				}
				else
				{
					ot = objectType( buildingType );
					buildingLevel = atoi(parser.tokens()[1].c_str());
	                ASSERT(buildingLevel <= 10, runtime_error());
	            	xPos = atof(parser.tokens()[2].c_str());
	            	yPos = atof(parser.tokens()[3].c_str());
	            	zPos = atof(parser.tokens()[4].c_str());
	            	orientation = atof(parser.tokens()[5].c_str());
					for( int i = 0; i < parser.tokens().size(); ++i )
					{
						if( parser.tokens()[i] == "REBUILD" )
							needRebuild = true;
						if( parser.tokens()[i] == "NOT_BUILT" )
							notBuilt = true;
					}
				}



				ASSERT_INFO( fabs( orientation ) );
				ASSERT( fabs( orientation ) == 0
					or fabs( orientation ) == 90
					or fabs( orientation ) == 180
					or fabs( orientation ) == 270 ,
					" Incorrect builing orientation detected\n" );

				int constructionSubType = 0;
				if( not isAlternativeSite )
				{
					// work out the subtype on a by-case basis
					switch( ot )
					{
						case MachLog::BEACON:
						case MachLog::GARRISON:
						case MachLog::MINE:
						case MachLog::SMELTER:
						case MachLog::POD:
							constructionSubType = 0;
							break;

						case MachLog::HARDWARE_LAB:
							constructionSubType = hardwareLabSubType( parser.tokens()[6] );
							break;

						case MachLog::FACTORY:
							constructionSubType = factorySubType( parser.tokens()[6] );
							break;

						case MachLog::MISSILE_EMPLACEMENT:
							constructionSubType = missileEmplacementSubType( parser.tokens()[6] );
							break;

						default:
							ASSERT( false, runtime_error() );
					}
				}


				if( isAlternativeSite )
				{

					if( doingAIRace )
					{
						ASSERT( lastEncounteredWasUnbuiltConstruction, "Cannot specify an alternative site for a construction that will actually be built at the beginning of the game." );
						ASSERT( constructionId >=0, "Must have specified a construction plan (NOT_BUILT) in order to specify an alternative site." );
						ASSERT( pLastProd, "Unexpected NULL pointer for pLastProd." );

						// copy details from last construction production unit encountered - this includes its construction ID.
						MachLogProductionUnit* prod = _NEW( MachLogProductionUnit( *pLastProd ) );

						 //Adjust the height for the terrain
					    const MexPoint3d location = MachLogSpacialManipulation::heightAdjustedLocation( MexPoint3d( xPos, yPos, zPos ) );
						//const MexPoint3d& loc2 = location;

						MexDegrees degrees(	orientation );
						MexRadians radians( degrees );
						const MexEulerAngles eulers( radians );
						//const MexEulerAngles& eulers2 = eulers;

						// remember, this production unit has already inherited the rebuild flag, construction id etc.
						// of the site it was copied from. But the new transform must obviously be set independently.

						MexTransform3d globalTransform( eulers, location );
						prod->globalTransform( globalTransform );
						races.AIController( pRace->race() ).addConstructionProductionUnit( prod );

					}

				}
				else if( notBuilt )
				{
					if( doingAIRace )
					{
						// don't actually build a new construction - store it as a production unit plan instead.

						MachLogProductionUnit* prod = _NEW( MachLogProductionUnit( ot, constructionSubType, buildingLevel, 0, 0 ) );

						//Adjust the height for the terrain
					    const MexPoint3d location = MachLogSpacialManipulation::heightAdjustedLocation( MexPoint3d( xPos, yPos, zPos ) );
						//const MexPoint3d& loc2 = location;

						MexDegrees degrees(	orientation );
						MexRadians radians( degrees );
						const MexEulerAngles eulers( radians );
						//const MexEulerAngles& eulers2 = eulers;

						MexTransform3d globalTransform( eulers, location );
						prod->globalTransform( globalTransform );

						++constructionId;

						prod->constructionId( constructionId );

						// ensure that the construction would be rebuilt once again if destroyed after this rebuild if applicable
						if( needRebuild )
							prod->needRebuild( true );

                        //Commented out so the order of constructions can be handled from the spl file.
						//if( ot == MachLog::HARDWARE_LAB or ot == MachLog::FACTORY /*or ot == MachLog::SMELTER */)
						//	prod->priority( 2 );

						races.AIController( pRace->race() ).addConstructionProductionUnit( prod );

						//now store this as the last construction production unit encountered
						pLastProd = prod;
					}
					lastEncounteredWasUnbuiltConstruction = true;
				}
				else
				{
					if( instantiateObjects )
					{
						// actually build a new construction

						MachLogConstruction* pConstruction = NULL;

						pConstruction = MachLogActorMaker::newLogConstruction( ot, constructionSubType, buildingLevel, MexPoint3d( xPos, yPos, zPos ), MexDegrees( orientation ), pRace->race() );
						ASSERT( pConstruction, "Unexpected NULL pointer for pConstruction" );

						// special shenanigens for pods with their ion cannons
						if( ot == MachLog::POD )
						{
							bool callActivateIonCannon = false;
							for( int i = 0; i < parser.tokens().size(); ++i )
								if( parser.tokens()[i] == "ACTIVATE_ION_CANNON" )
									callActivateIonCannon = true;

							if( callActivateIonCannon )
								pConstruction->asPod().activateIonCannon();
						}

						HAL_STREAM(" construction " << buildingType << std::endl );

						pConstruction->makeComplete( MachLogConstruction::FULL_HP_STRENGTH );
						if( displayAsOtherRace )
						{
							pConstruction->displayAsRace( displayAsRace );
						}

						if( doingAIRace and needRebuild )
						{
							pConstruction->needRebuild( true );
						}
					}

					lastEncounteredWasUnbuiltConstruction = false;
				}



            }
            else if( ( lineSize == 6 or lineSize == 7 ) and parsingMineralSites )
            {
				ASSERT( parser.tokens()[ 0 ] == "SITE" ,"Expectine SITE token in scenario file.\n" );
				MachPhys::MineralGrade grade = atol(parser.tokens()[1].c_str() );
				MachPhys::BuildingMaterialUnits amount = atol(parser.tokens()[2].c_str() );
            	MATHEX_SCALAR xPos = atof(parser.tokens()[3].c_str());
            	MATHEX_SCALAR yPos = atof(parser.tokens()[4].c_str());
            	MATHEX_SCALAR zPos = atof(parser.tokens()[5].c_str());
				MachLog::ResourcesAvailable thisResource = MachLog::RES_DEFAULT;
				if( lineSize == 7 )
					thisResource = resourceAvailable( parser.tokens()[6] );
				ASSERT_INFO( grade );
				ASSERT_INFO( amount );
				ASSERT( grade >0 and grade < 5,"bad grade for mineral site in scenario file.\n" );
				ASSERT( amount >0 ,"bad amount for mineral site in scenario file.\n" );
				bool processSite = false;	//( resources == MachLogRaces::RES_DEFAULT );

				if( resources == thisResource or thisResource == MachLog::RES_DEFAULT )
					processSite = true;
				else
					processSite = false;

				if( processSite )
					_NEW( MachLogMineralSite( grade, amount, MexPoint3d( xPos,yPos,zPos ) ) );
            }
			else if (lineSize == 8 )
			{
				ASSERT( parser.tokens()[ 0 ] == "CAMERA_POS" ,"Expecting CAMERA_POS token in scenario file.\n" );
				MATHEX_SCALAR xPos = atof(parser.tokens()[1].c_str());
            	MATHEX_SCALAR yPos = atof(parser.tokens()[2].c_str());
            	MATHEX_SCALAR zPos = atof(parser.tokens()[3].c_str());
				MexDegrees azimuth = atof(parser.tokens()[4].c_str());
				MexDegrees elevation = atof(parser.tokens()[5].c_str());
				MexDegrees roll = atof(parser.tokens()[6].c_str());
				MachLogCamera::Type raceCameraType = cameraType( parser.tokens()[7] );
				races.cameraInfo(race,
													MexTransform3d(	MexEulerAngles( azimuth, elevation, roll ),
																	MexPoint3d( xPos, yPos, zPos ) ),
													raceCameraType );
			}

			if( parser.tokens()[0] != "ENDRESEARCH_ITEMS" )
				parser.parseNextLine();

			if(parser.tokens()[0] == "ENDRACE" or parser.tokens()[0] == "ENDSITES" or parser.tokens()[0] == "ENDRESEARCH_ITEMS" )
            {
                if( parser.tokens()[0] == "ENDRACE" )
                {
                    //Check we have had the assembly points if required
                    if( doingAIRace or checkAllRacesHaveAssemblyPoints )
                    {
                        ASSERT( hadAggressorAssemblyPoint, " No aggressor assembly point defined" );
                        ASSERT( hadAdministratorAssemblyPoint, " No administrator assembly point defined" );
                    }
                }
				if( not parser.finished() )
					parser.parseNextLine();
               	running = false;
            }

			if(parser.finished())
               	running = false;
        }
    }

	if( network.isNetworkGame() )
	{
		network.ready( network.localRace(), true );
	}

	if( gameData.victoryCondition() != MachLog::VICTORY_DEFAULT )
	{
		bool doCreate = true;
		if( gameData.victoryCondition() == MachLog::VICTORY_POD )
		{
			doCreate = false;
			for( MachPhys::Race i = MachPhys::RED; i != MachPhys::N_RACES; ++((int&)i) )
			{
				if( races.raceInGame( i ) and races.pods( i ).size() > 0 )
					doCreate = true;
			}
		}
		if( doCreate )
			MachLogActions::createDynamically( gameData.victoryCondition(), gameData.timerTickAt() );
		else
			MachLogActions::createDynamically( MachLog::VICTORY_ANNIHILATION, 0 );
	}

	if( SimManager::instance().conditionsManager().actions().size() == 0 )
	{
		NETWORK_STREAM("No actions so creating VICTORY_ANNIHILATION\n" );
		MachLogActions::createDynamically( MachLog::VICTORY_ANNIHILATION, 0 );
	}

	for( MachPhys::Race i = MachPhys::RED; i < MachPhys::N_RACES; ++((int&)i) )
		if( not doneRace[i] )
			MachLogNetwork::instance().ready( i, true );

	const SysPathName RSI( researchItemsPath[ gameData.technologyLevel() ] );

    std::auto_ptr< std::istream > pIstream2;

    if( SysMetaFile::useMetaFile() && metaFile.hasFile( RSI ) )
    {
        //pIstream2 = _NEW( SysMetaFileIstream( metaFile, RSI, ios::text ) );
        pIstream2 = std::auto_ptr< std::istream > (
            _NEW( SysMetaFileIstream( metaFile, RSI, std::ios::in ) ));
    }
    else
    {
        ASSERT_FILE_EXISTS( RSI.c_str() );
        //pIstream2 = _NEW( ifstream( RSI.c_str(), ios::text | ios::in ) );
        pIstream2 = std::auto_ptr< std::istream > (
            _NEW( std::ifstream( RSI.c_str(), std::ios::in ) ));
    }

	UtlLineTokeniser riParser( *pIstream2, RSI );
	bool doForRace[MachPhys::N_RACES] = { false, false, false, false };
	MachPhys::Race race = MachPhys::NORACE;
	bool doAI = true;
	HAL_STREAM("MLScenario processing RSI file " << RSI << std::endl );
	while( not riParser.finished() )
	{
		if( riParser.tokens().size() > 0 )
		{
			if( riParser.tokens()[0] == "RACE" )
			{
				HAL_STREAM("Race Token detected " << riParser.tokens()[1] << std::endl );
				if( riParser.tokens()[1] == "RED" )
					race = MachPhys::RED;
				else if( riParser.tokens()[1] == "BLUE" )
					race = MachPhys::BLUE;
				else if( riParser.tokens()[1] == "GREEN" )
					race = MachPhys::GREEN;
				else if( riParser.tokens()[1] == "YELLOW" )
					race = MachPhys::YELLOW;
				else if( riParser.tokens()[1] == "AI" )
				{
					HAL_STREAM("AI Race only\n" );
					doAI = true;
					race = MachPhys::NORACE;
				}
				else if( riParser.tokens()[1] == "PC" )
				{
					HAL_STREAM("PC Race only\n" );
					race = MachPhys::NORACE;
					doAI = false;
				}
				else
				{
					race = MachPhys::N_RACES;
				}

				// reset the construction id counter, last genuine construction production unit pointer
				// and "built construction/stored as plan" type-checking flag for the new race
				constructionId = -1;
				pLastProd = NULL;
				lastEncounteredWasUnbuiltConstruction = false;
			}
			if( riParser.tokens()[0] == "MACHINE" or riParser.tokens()[0] == "CONSTRUCTION" )
			{
				bool researched = riParser.tokens()[1] == "RESEARCHED";
				MachLog::ObjectType type = objectType( riParser.tokens()[2] );
				int hwLevel = atoi( riParser.tokens()[3].c_str() );
				int subType = 0;
				MachPhys::WeaponCombo wc = MachPhys::N_WEAPON_COMBOS;
				MachPhys::ResearchUnits overrideResearchCost = 0;
				MachPhys::BuildingMaterialUnits overrideBuildingCost = 0;
				for( int i = 0; i < riParser.tokens().size(); ++i )
				{
					if( riParser.tokens()[i] == "SUB" )
						subType = objectSubType( type, riParser.tokens()[i+1] );
					else if( riParser.tokens()[i] == "WCOMBO" )
						wc = weaponCombo( riParser.tokens()[i+1] );
					else if( riParser.tokens()[i] == "RESEARCH_COST" )
						overrideResearchCost = atol( riParser.tokens()[i+1].c_str() );
					else if( riParser.tokens()[i] == "BUILDING_COST" )
						overrideBuildingCost = atol( riParser.tokens()[i+1].c_str() );
				}
				HAL_STREAM(" found a machine Token " << type << " sub " << subType << " hw " << hwLevel << " wc " << wc << " researched " << researched << std::endl );
				MachLogResearchItem& ri = races.researchTree().researchItem( type, subType, hwLevel, wc );
				HAL_STREAM(" ri objectTYpe " << ri.objectType() << " ri.sub " << ri.subType() << " ri.hwLevel " << ri.hwLevel() << " ri.wc " << ri.weaponCombo() << std::endl );
				for( MachPhys::Race ridx = MachPhys::RED; ridx != MachPhys::N_RACES; ++((int&)ridx) )
				{
					bool doWork = false;
					doWork = false;
					if( race == ridx )
						doWork = true;
					if( race == MachPhys::N_RACES )
					{
						doWork = true;
					}
					if( race == MachPhys::NORACE )
					{
						doWork = true;
						if( doAI )
						{
							if(	races.raceInGame( ridx ) and
								races.controller( ridx ).type() != MachLogController::AI_CONTROLLER )
							{
								doWork = false;
							}
						}
						else
						{
							if(	races.raceInGame( ridx ) and
								races.controller( ridx ).type() == MachLogController::AI_CONTROLLER )
							{
								doWork = false;
							}
						}
						HAL_STREAM("Race == NORACE, doAI " << doAI << " check against race " << ridx << " doWork is  now " << doWork << std::endl );
					}
					if( doWork )
					{
						ri.available( ridx, true );
						ri.researched( ridx, researched );
						ri.swAvailable( ridx, 1, true );
						ri.swResearched( ridx, 1, researched );
						for( int i = 1; i < 6; ++i )
							if( ri.swTechnologyLevel( i ) < 4 )
								ri.swAvailable( ridx, i, true );
						//ri.swLevel( ridx, 1 );
						if( overrideResearchCost )
							ri.researchCost( overrideResearchCost );
						if( overrideBuildingCost )
							ri.buildingCost( overrideBuildingCost );
						if( not researched )
						{
							ASSERT_INFO( riParser.tokens() );
							//ASSERT( ri.researchCost > 0,"You may not specify an item to be researched if it has zero cost\n" );
							ASSERT( ri.researchCost() > 0,"You may not specify an item to be researched if it has zero cost\n" );
						}
					}
				}
			}
		}
		riParser.parseNextLine();
	}
	races.cascadeUpdateForResearch( MachPhys::RED );
	races.cascadeUpdateForResearch( MachPhys::BLUE );
	races.cascadeUpdateForResearch( MachPhys::GREEN );
	races.cascadeUpdateForResearch( MachPhys::YELLOW );
	HAL_STREAM("MLScenario::load exit\n" );
}

MachLogScenario::~MachLogScenario()
{
    TEST_INVARIANT;
}

void MachLogScenario::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

//static
MachLog::ObjectType
MachLogScenario::objectType( const string& type )
{
	//Check for machine types
	if( type == "ADMINISTRATOR" )
		return MachLog::ADMINISTRATOR;
	if( type == "AGGRESSOR" )
		return MachLog::AGGRESSOR;
	if( type == "APC" )
		return MachLog::APC;
	if( type == "CONSTRUCTOR" )
		return MachLog::CONSTRUCTOR;
	if( type == "GEO_LOCATOR" )
		return MachLog::GEO_LOCATOR;
	if( type == "RESOURCE_CARRIER" )
		return MachLog::RESOURCE_CARRIER;
	if( type == "SPY_LOCATOR" )
		return MachLog::SPY_LOCATOR;
	if( type == "TECHNICIAN" )
		return MachLog::TECHNICIAN;

	//check for building types
	if( type == "BEACON" )
		return MachLog::BEACON;
	if( type == "FACTORY" )
		return MachLog::FACTORY;
	if( type == "GARRISON" )
		return MachLog::GARRISON;
	if( type == "HARDWARE_LAB" )
		return MachLog::HARDWARE_LAB;
	if( type == "HWLAB" )
		return MachLog::HARDWARE_LAB;
	if( type == "ORE_HOLOGRAPH" )
		return MachLog::ORE_HOLOGRAPH;
	if( type == "MINE" )
		return MachLog::MINE;
	if( type == "MISSILE_EMPLACEMENT" )
		return MachLog::MISSILE_EMPLACEMENT;
	if( type == "MISSILE" )
		return MachLog::MISSILE_EMPLACEMENT;
	if( type == "POD" )
		return MachLog::POD;
	if( type == "SMELTER" )
		return MachLog::SMELTER;
	if( type == "SOFTWARE_LAB" )
		return MachLog::SOFTWARE_LAB;
	if( type == "SWLAB" )
		return MachLog::SOFTWARE_LAB;
	if( type == "ARTEFACT" )
		return MachLog::ARTEFACT;
	if( type == "ARTIFACT" )
		return MachLog::ARTEFACT;
	if( type == "LAND_MINE" )
		return MachLog::LAND_MINE;

	ASSERT_INFO( type );
	ASSERT( false,"Unknown object string for matching\n" );
	return MachLog::ADMINISTRATOR;
}
//static
int
MachLogScenario::objectSubType( MachLog::ObjectType type,  const string& subType )
{
	switch( type )
	{
		case MachLog::ADMINISTRATOR:
			return administratorSubType( subType );
		case MachLog::AGGRESSOR:
			return aggressorSubType( subType );
		case MachLog::TECHNICIAN:
			return technicianSubType( subType );
		case MachLog::CONSTRUCTOR:
			return constructorSubType( subType );
		case MachLog::FACTORY:
			return factorySubType( subType );
		case MachLog::MISSILE_EMPLACEMENT:
			return missileEmplacementSubType( subType );
		case MachLog::HARDWARE_LAB:
			return hardwareLabSubType( subType );
		case MachLog::POD:
			return 0;
		default:
			ASSERT_INFO( type );
			ASSERT_BAD_CASE;
	}
	return 0;
}
//static
MachPhys::TechnicianSubType
MachLogScenario::technicianSubType( const string& subType )
{
	if( subType == "LAB_TECH" )
		return MachPhys::LAB_TECH;
	if( subType == "TECH_BOY" )
		return MachPhys::TECH_BOY;
	if( subType == "BRAIN_BOX" )
		return MachPhys::BRAIN_BOX;
	ASSERT_INFO( subType );
	ASSERT( false," Unknown subtype\n" );
	return MachPhys::LAB_TECH;
}
//static
MachPhys::ConstructorSubType
MachLogScenario::constructorSubType( const string& subType )
{
	if( subType == "DOZER" )
		return MachPhys::DOZER;
	if( subType == "BUILDER" )
		return MachPhys::BUILDER;
	if( subType == "BEHEMOTH" )
		return MachPhys::BEHEMOTH;
	ASSERT_INFO( subType );
	ASSERT( false," Unknown subtype\n" );
	return MachPhys::DOZER;
}
//static
MachPhys::AggressorSubType
MachLogScenario::aggressorSubType( const string& subType )
{
	if( subType == "GRUNT" )
		return MachPhys::GRUNT;
	if( subType == "ASSASSIN" )
		return MachPhys::ASSASSIN;
	if( subType == "KNIGHT" )
		return MachPhys::KNIGHT;
	if( subType == "BALLISTA" )
		return MachPhys::BALLISTA;
	if( subType == "NINJA" )
		return MachPhys::NINJA;
	ASSERT_INFO( subType );
	ASSERT( false," Unknown subtype\n" );
	return MachPhys::GRUNT;
}
//static
MachPhys::AdministratorSubType
MachLogScenario::administratorSubType( const string& subType )
{
	if( subType == "BOSS" )
		return MachPhys::BOSS;
	if( subType == "OVERSEER" )
		return MachPhys::OVERSEER;
	if( subType == "COMMANDER" )
		return MachPhys::COMMANDER;
	ASSERT_INFO( subType );
	ASSERT( false," Unknown subtype\n" );
	return MachPhys::BOSS;
}
//static
MachPhys::FactorySubType
MachLogScenario::factorySubType( const string& subType )
{
	if( subType == "CIVILIAN" )
		return MachPhys::CIVILIAN;
	if( subType == "MILITARY" )
		return MachPhys::MILITARY;
	ASSERT_INFO( subType );
	ASSERT( false," Unknown subtype\n" );
	return MachPhys::CIVILIAN;
}
//static
MachPhys::HardwareLabSubType
MachLogScenario::hardwareLabSubType( const string& subType )
{
	if( subType == "CIVILIAN" )
		return MachPhys::LAB_CIVILIAN;
	if( subType == "LAB_CIVILIAN" )
		return MachPhys::LAB_CIVILIAN;
	if( subType == "MILITARY" )
		return MachPhys::LAB_MILITARY;
	if( subType == "LAB_MILITARY" )
		return MachPhys::LAB_MILITARY;
	ASSERT_INFO( subType );
	ASSERT( false," Unknown subtype\n" );
	return MachPhys::LAB_CIVILIAN;
}
//static
MachPhys::MissileEmplacementSubType
MachLogScenario::missileEmplacementSubType( const string& subType )
{
	if( subType == "TURRET" )
		return MachPhys::TURRET;
	if( subType == "SENTRY" )
		return MachPhys::SENTRY;
	if( subType == "LAUNCHER" )
		return MachPhys::LAUNCHER;
	if( subType == "ICBM" )
		return MachPhys::ICBM;
	ASSERT_INFO( subType );
	ASSERT( false," Unknown subtype\n" );
	return MachPhys::ICBM;
}

//static
MachPhys::WeaponCombo
MachLogScenario::weaponCombo( const string& weaponCombo )
{
	if( weaponCombo == "L_BOLTER" )
        return MachPhys::L_BOLTER;
	if( weaponCombo == "T_ION_ORBITAL_CANNON" )
        return MachPhys::T_ION_ORBITAL_CANNON;
	if( weaponCombo == "L_ELECTRIC_CHARGE" )
        return MachPhys::L_ELECTRIC_CHARGE;
	if( weaponCombo == "R_BOLTER" )
        return MachPhys::R_BOLTER;
	if( weaponCombo == "R_ELECTRIC_CHARGE" )
        return MachPhys::R_ELECTRIC_CHARGE;
	if( weaponCombo == "LR_PULSE_RIFLE_X2" )
        return MachPhys::LR_PULSE_RIFLE_X2;
	if( weaponCombo == "L_PULSE_RIFLE_R_SUPERCHARGE_ADVANCED" )
        return MachPhys::L_PULSE_RIFLE_R_SUPERCHARGE_ADVANCED;
	if( weaponCombo == "LR_PLASMA_RIFLE_X2" )
        return MachPhys::LR_PLASMA_RIFLE_X2;
	if( weaponCombo == "L_MULTI_LAUNCHER5_R_SUPERCHARGE_SUPER" )
        return MachPhys::L_MULTI_LAUNCHER5_R_SUPERCHARGE_SUPER;
	if( weaponCombo == "L_MULTI_LAUNCHER5_R_TREACHERY" )
        return MachPhys::L_MULTI_LAUNCHER5_R_TREACHERY;
	if( weaponCombo == "LR_PULSE_CANNON_X2" )
        return MachPhys::LR_PULSE_CANNON_X2;
	if( weaponCombo == "L_PULSE_CANNON_R_SUPERCHARGE_SUPER" )
        return MachPhys::L_PULSE_CANNON_R_SUPERCHARGE_SUPER;
	if( weaponCombo == "L_PULSE_CANNON_R_VIRUS" )
        return MachPhys::L_PULSE_CANNON_R_VIRUS;
	if( weaponCombo == "L_FLAME_THROWER2" )
        return MachPhys::L_FLAME_THROWER2;
	if( weaponCombo == "LR_HEAVY_BOLTER1_X2" )
        return MachPhys::LR_HEAVY_BOLTER1_X2;
	if( weaponCombo == "L_MULTI_LAUNCHER7_T_GORILLA_PUNCH_R_MULTI_LAUNCHER7" )
        return MachPhys::L_MULTI_LAUNCHER7_T_GORILLA_PUNCH_R_MULTI_LAUNCHER7;
//	if( weaponCombo == "LR_GRENADE_LAUNCHER_X2" )
//        return MachPhys::LR_GRENADE_LAUNCHER_X2;
	if( weaponCombo == "L_AUTO_CANNON" )
        return MachPhys::L_AUTO_CANNON;
	if( weaponCombo == "L_PLASMA_RIFLE" )
        return MachPhys::L_PLASMA_RIFLE;
	if( weaponCombo == "LR_AUTO_CANNON_X2" )
        return MachPhys::LR_AUTO_CANNON_X2;
	if( weaponCombo == "T_NUCLEAR_MISSILE" )
        return MachPhys::T_NUCLEAR_MISSILE;
	if( weaponCombo == "T_MULTI_LAUNCHER1" )
        return MachPhys::T_MULTI_LAUNCHER1;
	if( weaponCombo == "T_VORTEX" )
        return MachPhys::T_VORTEX;
	if( weaponCombo == "LR_MULTI_LAUNCHER2_X2" )
        return MachPhys::LR_MULTI_LAUNCHER2_X2;
	if( weaponCombo == "LR_HEAVY_BOLTER2_X2" )
        return MachPhys::LR_HEAVY_BOLTER2_X2;
	if( weaponCombo == "LR_PLASMA_CANNON1_X2" )
        return MachPhys::LR_PLASMA_CANNON1_X2;
	if( weaponCombo == "LR_MULTI_LAUNCHER3_X2" )
        return MachPhys::LR_MULTI_LAUNCHER3_X2;
	if( weaponCombo == "LR_PLASMA_CANNON2_X2" )
        return MachPhys::LR_PLASMA_CANNON2_X2;
	if( weaponCombo == "LRT_MULTI_LAUNCHER4_X3" )
        return MachPhys::LRT_MULTI_LAUNCHER4_X3;
	if( weaponCombo == "LRT_PLASMA_CANNON2_X3" )
        return MachPhys::LRT_PLASMA_CANNON2_X3;
	if( weaponCombo == "T_FLAME_THROWER1" )
        return MachPhys::T_FLAME_THROWER1;
	if( weaponCombo == "LR_MULTI_LAUNCHER5_X2" )
        return MachPhys::LR_MULTI_LAUNCHER5_X2;
	if( weaponCombo == "LR_MULTI_LAUNCHER6_X2" )
        return MachPhys::LR_MULTI_LAUNCHER6_X2;
	if( weaponCombo == "LR_LARGE_MISSILE_X2" )
        return MachPhys::LR_LARGE_MISSILE_X2;
	if( weaponCombo == "LR_MULTI_LAUNCHER7_X2" )
        return MachPhys::LR_MULTI_LAUNCHER7_X2;
	if( weaponCombo == "T_WASP_LIGHT_STING" )
        return MachPhys::T_WASP_LIGHT_STING;
	if( weaponCombo == "T_WASP_METAL_STING" )
        return MachPhys::T_WASP_METAL_STING;
	if( weaponCombo == "T_BEE_BOMB" )
        return MachPhys::T_BEE_BOMB;
    ASSERT_INFO( weaponCombo );
	ASSERT( false," Unknown Weapon Combo\n" );
	return MachPhys::L_BOLTER;
}

//static
MachPhys::Race MachLogScenario::machPhysRace( const string& race )
{
	if( race == "RED" )
		return MachPhys::RED;
	if( race == "BLUE" )
		return MachPhys::BLUE;
	if( race == "GREEN" )
		return MachPhys::GREEN;
	if( race == "YELLOW" )
		return MachPhys::YELLOW;
	if( race == "N_RACES" )
		return MachPhys::N_RACES;
    ASSERT_INFO( race );
	ASSERT( false," Unknown Race token Combo\n" );
	return MachPhys::RED;
}

//static
MachLog::ObjectType  MachLogScenario::physConstructionToLogObject ( MachPhys::ConstructionType CT)
{
	switch( CT )
	{
		case MachPhys::HARDWARE_LAB:
			return MachLog::HARDWARE_LAB;
		case MachPhys::SMELTER:
			return MachLog::SMELTER;
		case MachPhys::FACTORY:
			return MachLog::FACTORY;
		case MachPhys::MISSILE_EMPLACEMENT:
			return MachLog::MISSILE_EMPLACEMENT;
		case MachPhys::GARRISON:
			return MachLog::GARRISON;
		case MachPhys::MINE:
			return MachLog::MINE;
		case MachPhys::BEACON:
			return MachLog::BEACON;
		case MachPhys::POD:
			return MachLog::POD;
		default:
			ASSERT_INFO( CT );
			ASSERT_BAD_CASE;
	}
	return MachLog::POD;	// this is never executed
}

//static
MachLogCamera::Type
MachLogScenario::cameraType( const string& cameraType )
{
	if( cameraType == "ZENITH" )
		return MachLogCamera::ZENITH;
	if( cameraType == "GROUND" )
		return MachLogCamera::GROUND;
	if( cameraType == "FREE_MOVING" )
		return MachLogCamera::FREE_MOVING;
	if( cameraType == "THIRD_PERSON" )
		return MachLogCamera::THIRD_PERSON;
	ASSERT_INFO( cameraType );
	ASSERT( false," Unknown cameraType\n" );
	return MachLogCamera::GROUND;
}

//static
MachLog::BeaconType	MachLogScenario::virtualBeaconType( const string& beaconType )
{
	if( beaconType == "NO_BEACON" )
		return MachLog::NO_BEACON;
	if( beaconType == "LEVEL_1_BEACON" )
		return MachLog::LEVEL_1_BEACON;
	if( beaconType == "LEVEL_3_BEACON" )
		return MachLog::LEVEL_3_BEACON;
	ASSERT_INFO( beaconType );
	ASSERT( false," Unknown beaconType\n" );
	return MachLog::NO_BEACON;

}

//static
MachLog::ResourcesAvailable MachLogScenario::resourceAvailable( const string& resourceType )
{
	if( resourceType == "RES_DEFAULT" )
		return MachLog::RES_DEFAULT;
	if( resourceType == "DEFAULT" )
		return MachLog::RES_DEFAULT;
	if( resourceType == "RES_LOW" )
		return MachLog::RES_LOW;
	if( resourceType == "LOW" )
		return MachLog::RES_LOW;
	if( resourceType == "RES_MEDIUM" )
		return MachLog::RES_MEDIUM;
	if( resourceType == "MEDIUM" )
		return MachLog::RES_MEDIUM;
	if( resourceType == "RES_HIGH" )
		return MachLog::RES_HIGH;
	if( resourceType == "HIGH" )
		return MachLog::RES_HIGH;
	ASSERT_INFO( resourceType );
	ASSERT( false,"Unknown resource type marker\n" );
	return MachLog::RES_DEFAULT;

}

//static
int	MachLogScenario::startingResourcesToInt( MachLog::StartingResources res )
{
	switch( res )
	{
		case MachLog::STARTING_RESOURCES_DEFAULT:
			return -1;
		case MachLog::STARTING_RESOURCES_RES_LOW:
			return 0;
		case MachLog::STARTING_RESOURCES_MEDIUM:
			return 100;
		case MachLog::STARTING_RESOURCES_HIGH:
			return 500;
		case MachLog::STARTING_RESOURCES_VERY_HIGH:
			return 1000;
		case MachLog::STARTING_RESOURCES_SUPER_HIGH:
			return 10000;
		default:
			ASSERT_INFO( res );
			ASSERT_BAD_CASE;
	}
	return 0;
}

//static
void MachLogScenario::parseRestrictConstruction( const UtlLineTokeniser& parser )
{
	PRE( parser.tokens()[1] == "RACE" );
	MachLogRaces& races = MachLogRaces::instance();

	MachPhys::Race race;
	string raceString = parser.tokens()[2];
	bool doAI = true;
	if( raceString == "RED" )
		race = MachPhys::RED;
	else if( raceString == "BLUE" )
		race = MachPhys::BLUE;
	else if( raceString == "GREEN" )
		race = MachPhys::GREEN;
	else if( raceString == "YELLOW" )
		race = MachPhys::YELLOW;
	else if( raceString == "AI" )
	{
		doAI = true;
		race = MachPhys::NORACE;
	}
	else if( raceString == "PC" )
	{
		race = MachPhys::NORACE;
		doAI = false;
	}
	else
	{
		race = MachPhys::N_RACES;
	}

	size_t lineSize = parser.tokens().size();
	MachLog::ObjectType type;
	int subType = 0;
	size_t hwLevel = 0;
	MachPhys::WeaponCombo weaponCombo = MachPhys::N_WEAPON_COMBOS;

	ASSERT_INFO( lineSize );
	ASSERT( ( lineSize == 7 or lineSize==6 or lineSize==5 ),"Wrong number of tokens in RESTRICT_CONSTRUCTION line.\n" );

   	type = MachLogScenario::objectType( parser.tokens()[ 3 ] );
	if ( lineSize>=6 )
	{
		subType = MachLogScenario::objectSubType( type, parser.tokens()[ 4 ] );
   		//hwLevel = _CONST_CAST(size_t, atoi( parser.tokens()[ 5 ].c_str() ) );
   		hwLevel = atoi( parser.tokens()[ 5 ].c_str() ) ;
	}
	else
	{
		subType = 0;
		//hwLevel = _CONST_CAST(size_t, atoi( parser.tokens()[ 4 ].c_str() ) );
		hwLevel = atoi( parser.tokens()[ 4 ].c_str() ) ;
	}

	MachLogConstructionItem& consItem = races.constructionTree().constructionItem( (MachPhys::ConstructionType)type, subType, hwLevel, weaponCombo );
	for( MachPhys::Race i = MachPhys::RED; i != MachPhys::N_RACES; ++((int&)i) )
	{
		bool doWork = false;
		doWork = false;
		if( race == i )
			doWork = true;
		if( race == MachPhys::N_RACES )
		{
			doWork = true;
		}
		if( race == MachPhys::NORACE )
		{
			doWork = true;
			if( doAI )
			{
				if(	races.raceInGame( i ) and
					races.controller( i ).type() != MachLogController::AI_CONTROLLER )
				{
					doWork = false;
				}
			}
			else
			{
				if(	races.raceInGame( i ) and
					races.controller( i ).type() == MachLogController::AI_CONTROLLER )
				{
					doWork = false;
				}
			}
		}
		if( doWork )
			consItem.activationLocked( i, true );
	}

}

/* End SCENARIO.CPP *************************************************/
