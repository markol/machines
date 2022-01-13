/*
 * D A T A B A S E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "base/persist.hpp"
#include "base/error.hpp"
#include "stdlib/string.hpp"
#include <algorithm>
#include "utility/linetok.hpp"

#include "machlog/scenario.hpp"
#include "machgui/database.hpp"
#include "machgui/internal/databasi.hpp"
#include "machgui/internal/strings.hpp"
#include "machgui/dbsystem.hpp"
#include "machgui/dbplanet.hpp"
#include "machgui/dbscenar.hpp"
#include "machgui/dbplayer.hpp"
#include "machgui/dbplyscn.hpp"
#include "machgui/dbsavgam.hpp"
#include "machgui/startup.hpp"

MachGuiDatabase::MachGuiDatabase()
:   pData_( NULL )
{
    pData_ = _NEW( MachGuiIDatabase );

	// Initialise the database
	readDatabase();

	// Clean out any save games place holders that have had the actual save game deleted
	// from the hard disk
	bool savedGameRemoved = true;

	while ( savedGameRemoved )
	{
		savedGameRemoved = false;
		uint numSavedGames = nSavedGames();

		for ( uint loop = 0; loop < numSavedGames and not savedGameRemoved; ++loop )
		{
			MachGuiDbSavedGame& savedGameObj = savedGame( loop );

			// Check for external file name on hard disk
			SysPathName externFileName( savedGameObj.externalFileName() );
			if ( not externFileName.existsAsFile() )
			{
				removeSavedGame( &savedGameObj );
				savedGameRemoved = true;
			}
		}
    }

    TEST_INVARIANT;
}

//static
MachGuiDatabase& MachGuiDatabase::instance()
{
	static MachGuiDatabase singleton;
	return singleton;
}

MachGuiDatabase::~MachGuiDatabase()
{
    TEST_INVARIANT;

    //Delete all the systems
    for( size_t i = pData_->campaignSystems_.size(); i--; )
    {
        _DELETE( pData_->campaignSystems_[i] );
    }

    for( size_t i = pData_->skirmishSystems_.size(); i--; )
    {
        _DELETE( pData_->skirmishSystems_[i] );
    }

    for( size_t i = pData_->multiPlayerSystems_.size(); i--; )
    {
        _DELETE( pData_->multiPlayerSystems_[i] );
    }

    //And all the players
    for( size_t i = pData_->players_.size(); i--; )
    {
        _DELETE( pData_->players_[i] );
    }

    _DELETE( pData_ );
}

void MachGuiDatabase::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiDatabase& t )
{

    o << "MachGuiDatabase " << (void*)&t << " start" << std::endl;
    o << "MachGuiDatabase " << (void*)&t << " end" << std::endl;

    return o;
}

uint MachGuiDatabase::nCampaignSystems() const
{
    return pData_->campaignSystems_.size();
}

MachGuiDbSystem& MachGuiDatabase::campaignSystem( uint index )
{
    PRE( index < nCampaignSystems() );
    return *(pData_->campaignSystems_[ index ]);
}

MachGuiDbSystem& MachGuiDatabase::skirmishSystem( TerrainSize size )
{
    return *(pData_->skirmishSystems_[ size ]);
}

MachGuiDbSystem& MachGuiDatabase::multiPlayerSystem( TerrainSize size )
{
    return *(pData_->multiPlayerSystems_[ size ]);
}

void MachGuiDatabase::readDatabase()
{
    //Set up the persistence for database hierarchies
    registerDerivedClasses();

    //If there is a player updated version, load it
    if( playerDatabasePath().existsAsFile() )
    {
        //Load the version with current player info
    	std::ifstream ifstr( playerDatabasePath().c_str(), std::ios::binary );
    	PerIstream istr( ifstr );

    	istr >> *(pData_);
    }
    else
    {
        //Check for a virgin persistent database
        if( virginDatabasePersistentPath().existsAsFile() )
        {
            //Load the version with current player info
        	std::ifstream ifstr( virginDatabasePersistentPath().c_str(), std::ios::binary );
        	PerIstream istr( ifstr );

        	istr >> *(pData_);
        }
        else
        {
            //Read the database from source file
            parseCampaignFile();

            //Save it as a virgin persistent file
            std::ofstream ofstr( virginDatabasePersistentPath().c_str(), std::ios::binary );
            PerOstream ostr( ofstr );

            ostr << *(pData_);
        }
    }
    //Read the user scenarios database
    if ( doCustomScenariosExist() )
        parseUserCampaignFile();
}

void MachGuiDatabase::writeDatabase() const
{
    std::ofstream ofstr( playerDatabasePath().c_str(), std::ios::binary );
    PerOstream ostr( ofstr );

    ostr << *pData_;
}

void MachGuiDatabase::parseCampaignFile()
{
    //Construct the basic data object. Create empty small, medium and large systems
    //for skirmishes and multi-player games.
    pData_->skirmishSystems_.push_back( _NEW( MachGuiDbSystem( "SMALL", IDS_SMALL ) ) );
    pData_->skirmishSystems_.push_back( _NEW( MachGuiDbSystem( "MEDIUM", IDS_MEDIUM ) ) );
    pData_->skirmishSystems_.push_back( _NEW( MachGuiDbSystem( "LARGE", IDS_LARGE ) ) );
    pData_->multiPlayerSystems_.push_back( _NEW( MachGuiDbSystem( "SMALL", IDS_SMALL ) ) );
    pData_->multiPlayerSystems_.push_back( _NEW( MachGuiDbSystem( "MEDIUM", IDS_MEDIUM ) ) );
    pData_->multiPlayerSystems_.push_back( _NEW( MachGuiDbSystem( "LARGE", IDS_LARGE ) ) );

    //Open the data file constructing a parser
    UtlLineTokeniser parser( virginDatabaseSourcePath() );

    //Loop through the higher level constructs
    while( not parser.finished() )
    {
        ASSERT( parser.tokens().size() == 1, "Incorrect number of tokens" );
        if( parser.tokens()[0] == "CAMPAIGNS" )
        {
            parseCategory( parser, CAMPAIGNS );
        }
        else if( parser.tokens()[0] == "SKIRMISHES" )
        {
            parseCategory( parser, SKIRMISHES );
        }
        else if( parser.tokens()[0] == "MULTI_PLAYER" )
        {
            parseCategory( parser, MULTI_PLAYER );
        }
        else
        {
            ASSERT( false, "CAMPAIGNS, SKIRMISHES or MULTI_PLAYER expected" );
        }

        parser.parseNextLine();
    }
}

void MachGuiDatabase::parseCategory( UtlLineTokeniser& parser, Context context, bool custom )
{
    parser.parseNextLine();

    //Construct a temporary element map for setting up dependencies if doing the campaign section
    if( context == CAMPAIGNS )
        pData_->pElementMap_ = _NEW( MachGuiIDatabase::ElementMap );

    //Loop finding systems
    while( parser.tokens()[0] != "END" )
    {
        if( parser.tokens()[0] == "SYSTEM" )
        {
            ASSERT( context == CAMPAIGNS, "SYSTEM or END expected" );
            parseCampaignSystem( parser );
        }
        else if( parser.tokens()[0] == "SMALL" )
        {
            ASSERT( context != CAMPAIGNS, "SMALL not expected" );
            MachGuiDbSystem* pSystem =
                (context == SKIRMISHES ? pData_->skirmishSystems_[0] : pData_->multiPlayerSystems_[0]);

            pSystem->isCustom( custom ); //Prevent user levels from stroage in db
            parseNonCampaignSystem( parser, pSystem, context );
        }
        else if( parser.tokens()[0] == "MEDIUM" )
        {
            ASSERT( context != CAMPAIGNS, "MEDIUM not expected" );
            MachGuiDbSystem* pSystem =
                (context == SKIRMISHES ? pData_->skirmishSystems_[1] : pData_->multiPlayerSystems_[1]);

            pSystem->isCustom( custom );
            parseNonCampaignSystem( parser, pSystem, context );
        }
        else if( parser.tokens()[0] == "LARGE" )
        {
            ASSERT( context != CAMPAIGNS, "LARGE not expected" );
            MachGuiDbSystem* pSystem =
                (context == SKIRMISHES ? pData_->skirmishSystems_[2] : pData_->multiPlayerSystems_[2]);

            pSystem->isCustom( custom );
            parseNonCampaignSystem( parser, pSystem, context );
        }
        else if( parser.tokens()[0] == "CUSTOM" )
        {
            ASSERT( context != CAMPAIGNS, "CUSTOM not expected" );
            MachGuiDbSystem* pSystem =
                (context == SKIRMISHES ? pData_->skirmishSystems_[3] : pData_->multiPlayerSystems_[3]);

            pSystem->isCustom( true );
            parseNonCampaignSystem( parser, pSystem, context );
        }
        else
        {
            ALWAYS_ASSERT_FAIL( "Invalid keyword in context" );
        }

        parser.parseNextLine();
    }

    //Delete any temporary element map
    _DELETE( pData_->pElementMap_ );
    pData_->pElementMap_ = NULL;
}

void MachGuiDatabase::parseCampaignSystem( UtlLineTokeniser& parser )
{
    ASSERT( parser.tokens()[0] == "SYSTEM", "" );

    //Get the system data
    size_t index = 2;
    size_t nTokens = parser.tokens().size();
    uint menuStringId = 0;
    string textFile;
    string systemPicture;
    bool hadAfter = false;

    while( not hadAfter  and  index + 1 < nTokens )
    {
        const string& token = parser.tokens()[ index++ ];
        if( token == "IDS" )
            menuStringId = atol( parser.tokens()[ index++ ].c_str() );
        else if( token == "TEXT" )
            textFile = parser.tokens()[ index++ ];
        else if( token == "FLIC" )
            systemPicture = parser.tokens()[ index++ ];
        else if( token == "AFTER" )
            hadAfter = true;
        else
        {
            ASSERT_INFO( token );
            ASSERT_FAIL( "Unrecognised keyword" );
        }
    }

    if( not hadAfter )
    {
        ASSERT( index == nTokens, "Incorrect number of keywords" );
    }
    ASSERT( menuStringId != 0, "No IDS clause" );
    ASSERT( textFile.length() != 0, "No TEXT clause" );
    ASSERT( systemPicture.length() != 0, "No FLIC clause" );

    //Construct and add a new campaign system
    MachGuiDbSystem* pSystem = _NEW( MachGuiDbSystem( parser.tokens()[1], menuStringId ) );
    pSystem->campaignPicture( systemPicture );
    pSystem->textDataFileName( textFile );

    pData_->campaignSystems_.push_back( pSystem );

    //Deal with antecedents
    if( hadAfter )
    {
        while( index < nTokens )
        {
            //Look this one up in the map and add to the list of antecedents
            const string& antecedentName = parser.tokens()[ index++ ];
            pSystem->addDependency( (*pData_->pElementMap_)[ antecedentName ] );
        }
    }

    //Add this system to the element map
    pData_->pElementMap_->insert( parser.tokens()[1], pSystem );

    parser.parseNextLine();

    //Parse all the planets
    while( parser.tokens()[0] != "ENDSYSTEM" )
    {
        ASSERT( parser.tokens()[0]  == "PLANET", "PLANET or ENDSYSTEM expected" );
        parsePlanet( parser, pSystem, CAMPAIGNS );

        parser.parseNextLine();
    }
}

void MachGuiDatabase::parseNonCampaignSystem( UtlLineTokeniser& parser, MachGuiDbSystem* pSystem, Context context )
{
    parser.parseNextLine();

    //Keep looking for terrain constructs or the terminating END
    while( parser.tokens()[0] != "END" )
    {
        if( parser.tokens()[0] == "TERRAIN" )
            parsePlanet( parser, pSystem, context );
        else
        {
            ASSERT_FAIL( "TERRAIN or END expected" );
        }

        parser.parseNextLine();
    }
}

void MachGuiDatabase::parsePlanet( UtlLineTokeniser& parser, MachGuiDbSystem* pSystem, Context context )
{
    //Get the planet data
    size_t index = 2;
    size_t nTokens = parser.tokens().size();
    const string& planetName = parser.tokens()[1];
    uint menuStringId = 0;
    string textFile;
    string planetPicture;
    string planetNameMenu;
    bool hadAfter = false;
    bool campaignContext = context == CAMPAIGNS;

    while( not hadAfter  and  index + 1 < nTokens )
    {
        const string& token = parser.tokens()[ index++ ];
        if( token == "IDS" )
            menuStringId = atol( parser.tokens()[ index++ ].c_str() );
        else if( token == "NAME" )
            planetNameMenu = parser.tokens()[ index++ ];
        else if( token == "TEXT" )
            textFile = parser.tokens()[ index++ ];
        else if( campaignContext and  token == "FLIC" )
            planetPicture = parser.tokens()[ index++ ];
        else if( campaignContext and  token == "AFTER" )
                hadAfter = true;
        else
        {
            ASSERT_INFO( token );
            ASSERT_FAIL( "Unrecognised keyword" );
        }
    }

    if( not hadAfter )
    {
        ASSERT( index == nTokens, "Incorrect number of keywords" );
    }
    ALWAYS_ASSERT( menuStringId != 0 || planetNameMenu.length() != 0, "No IDS or NAME clause" );
    ALWAYS_ASSERT( textFile.length() != 0, "No TEXT clause" );

    if( context == CAMPAIGNS )
    {
        ASSERT( planetPicture.length() != 0, "No FLIC clause" );
    }

    //Construct and add a new planet
    MachGuiDbPlanet* pPlanet = _NEW( MachGuiDbPlanet( planetName, menuStringId ) );
    pPlanet->campaignPicture( planetPicture );
    pPlanet->textDataFileName( textFile );
    pPlanet->menuString( planetNameMenu );
    pPlanet->isCustom( pSystem->isCustom() );

    pSystem->addPlanet( pPlanet );

    //Deal with antecedents
    if( hadAfter )
    {
        while( index < nTokens )
        {
            //Look this one up in the map and add to the list of antecedents
            const string& antecedentName = parser.tokens()[ index++ ];
            pPlanet->addDependency( (*pData_->pElementMap_)[ antecedentName ] );
        }
    }

    //Add this planet to the element map
    if( context == CAMPAIGNS )
        pData_->pElementMap_->insert( parser.tokens()[1], pSystem );

    parser.parseNextLine();

    //Parse all the scenarios
    const char* endKeyword = (context == CAMPAIGNS ? "ENDPLANET" : "ENDTERRAIN");
    while( parser.tokens()[0] != endKeyword )
    {
        ASSERT( parser.tokens()[0]  == "SCENARIO", "Unrecognised keyword" );
        parseScenario( parser, pPlanet, context );

        parser.parseNextLine();
    }
}

void MachGuiDatabase::parseScenario( UtlLineTokeniser& parser, MachGuiDbPlanet* pPlanet, Context context )
{
    ASSERT( parser.tokens()[0] == "SCENARIO", "" );

    //Get the scenario basic data
    size_t index = 2;
    size_t nTokens = parser.tokens().size();
    const string& scenarioName = parser.tokens()[1];
    uint menuStringId = 0;
    string textFile;
    string planetFile;
    string planetName;
    bool hadAfter = false;
    bool campaignContext = context == CAMPAIGNS;
	uint maxPlayers = 4;
    uint musicTrack = MachGuiStartupScreens::DEFAULT_INGAME_MUSIC;

    while( not hadAfter  and  index + 1 < nTokens )
    {
        const string& token = parser.tokens()[ index++ ];
        if( token == "IDS" )
            menuStringId = atol( parser.tokens()[ index++ ].c_str() );
        else if( token == "NAME" )
            planetName = parser.tokens()[ index++ ];
        else if( token == "TEXT" )
            textFile = parser.tokens()[ index++ ];
        else if( token == "PLANET" )
            planetFile = parser.tokens()[ index++ ];
        else if( token == "MUSIC" )
            musicTrack = atol( parser.tokens()[ index++ ].c_str() ) - 2;
        else if( campaignContext  and  token == "AFTER" )
            hadAfter = true;
		else if( token == "MAXPLAYERS" )
		{
			ASSERT( not campaignContext, "Max players cannot  be specified for campaign"	);
			maxPlayers = atol( parser.tokens()[ index++ ].c_str() );
         }
        else
        {
            ASSERT_INFO( token );
            ASSERT_FAIL( "Unrecognised keyword" );
        }
    }

    if( not hadAfter )
    {
        ASSERT( index == nTokens, "Incorrect number of keywords" );
    }
    ALWAYS_ASSERT( menuStringId != 0 || planetName.size() != 0, "No IDS or NAME clause" );
    ALWAYS_ASSERT( textFile.length() != 0, "No TEXT clause" );
    ALWAYS_ASSERT( planetFile.length() != 0, "No PLANET clause" );

    //Construct and add a new scenario
    MachGuiDbScenario* pScenario = _NEW( MachGuiDbScenario( scenarioName, planetFile, menuStringId, maxPlayers ) );
    pScenario->textDataFileName( textFile );
    pScenario->musicTrack( musicTrack );
    pScenario->menuString( planetName );

    pPlanet->addScenario( pScenario );

    //Deal with antecedents
    if( hadAfter )
    {
        while( index < nTokens )
        {
            //Look this one up in the map and add to the list of antecedents
            const string& antecedentName = parser.tokens()[ index++ ];
            pScenario->addDependency( (*pData_->pElementMap_)[ antecedentName ] );
        }
    }

    //Extra work in campaign mode
    if( campaignContext )
    {
        //Add this scenario to the element map
        pData_->pElementMap_->insert( parser.tokens()[1], pScenario );

        //Search for the campaign scenario bits
        while( true )
        {
            //Check for end
            parser.parseNextLine();
            if( parser.tokens()[0] == "ENDSCENARIO" )
                break;

            //Must be a useful keyword
            if( parser.tokens()[0] == "CAMPAIGN" )
            {
                ASSERT( parser.tokens().size() == 2, "Wrong number of tokens" );
                pScenario->campaignPicture( parser.tokens()[1] );
            }
            else if( parser.tokens()[0] == "BRIEFING" )
            {
                ASSERT( parser.tokens().size() == 2, "Wrong number of tokens" );
                pScenario->briefingPicture( parser.tokens()[1] );
            }
            else if( parser.tokens()[0] == "DEBRIEFING" )
            {
                ASSERT( parser.tokens().size() == 2, "Wrong number of tokens" );
                pScenario->debriefingPicture( parser.tokens()[1] );
            }
			else if( parser.tokens()[0] == "DEBRIEFINGLOSE" )
            {
                ASSERT( parser.tokens().size() == 2, "Wrong number of tokens" );
                pScenario->debriefingLosePicture( parser.tokens()[1] );
            }
            else if( parser.tokens()[0] == "ENTRY" )
            {
                ASSERT( parser.tokens().size() == 2, "Wrong number of tokens" );
                pScenario->entryFlic( parser.tokens()[1] );
            }
            else if( parser.tokens()[0] == "WIN" )
            {
                ASSERT( parser.tokens().size() == 2, "Wrong number of tokens" );
                pScenario->winFlic( parser.tokens()[1] );
            }
            else if( parser.tokens()[0] == "LOSE" )
            {
                ASSERT( parser.tokens().size() == 2, "Wrong number of tokens" );
                pScenario->loseFlic( parser.tokens()[1] );
            }
            else if( parser.tokens()[0] == "SAVE_MACHINES" )
            {
                ASSERT( parser.tokens().size() > 1, "Too few tokens" );
                uint n = parser.tokens().size();
                for( size_t i = 1; i != n; ++i )
                {
                    MachPhys::Race race = MachLogScenario::machPhysRace( parser.tokens()[i] );
                    pScenario->raceToHaveSurvivingMachinesSaved( race );
                }
            }
            else
            {
                ASSERT_INFO( parser.tokens()[0] );
                ASSERT_FAIL( "Unrecognised keyword" );
            }
        }
    }
}

void MachGuiDatabase::parseUserCampaignFile()
{
    pData_->skirmishSystems_.push_back( _NEW( MachGuiDbSystem( "CUSTOM", IDS_CUSTOM ) ) );
    
    //Open the data file constructing a parser
    UtlLineTokeniser parser( customDatabaseSourcePath() );

    //Loop through the higher level constructs
    while( not parser.finished() )
    {
        ASSERT( parser.tokens().size() == 1, "Incorrect number of tokens" );
        if( parser.tokens()[0] == "SKIRMISHES" )
        {
            parseCategory( parser, SKIRMISHES, true );
        }
        else if( parser.tokens()[0] == "MULTI_PLAYER" )
        {
            parseCategory( parser, MULTI_PLAYER, true );
        }
        else
        {
            ALWAYS_ASSERT( false, "SKIRMISHES or MULTI_PLAYER expected" );
        }

        parser.parseNextLine();
    }
}

//static
const SysPathName& MachGuiDatabase::customDatabaseSourcePath()
{
    static SysPathName path( "data/customscen.dat" );
    return path;
}

//static
const SysPathName& MachGuiDatabase::virginDatabaseSourcePath()
{
    static SysPathName path( "data/sceninfo.dat" );
    return path;
}

//static
const SysPathName& MachGuiDatabase::virginDatabasePersistentPath()
{
    static SysPathName path( "pdata/sceninfo.bin" );
    return path;
}

//static
const SysPathName& MachGuiDatabase::playerDatabasePath()
{
    static SysPathName path( "savegame/playinfo.bin" );
    return path;
}

void MachGuiDatabase::sortPlayersByName()
{
    MachGuiIDatabase::Players& players = pData_->players_;
    //sort( players.begin(), players.end(), MachGuiIDatabase::PlayerNameComparator() );
}

void MachGuiDatabase::sortPlayersByTime()
{
    MachGuiIDatabase::Players& players = pData_->players_;
    //sort( players, MachGuiIDatabase::PlayerTimeComparator() );
}

MachGuiDbPlayer& MachGuiDatabase::addPlayer( const string& name )
{
    //Create a new player and add to collection
    MachGuiDbPlayer* pDbPlayer = _NEW( MachGuiDbPlayer( pData_->nextPlayerId_++, name ) );
    pData_->players_.push_back( pDbPlayer );

    return *pDbPlayer;
}

uint MachGuiDatabase::nPlayers() const
{
    return pData_->players_.size();
}

MachGuiDbPlayer& MachGuiDatabase::player( uint index ) const
{
    PRE( index < nPlayers() );
    return *(pData_->players_[ index ]);
}

MachGuiDbPlayer& MachGuiDatabase::currentPlayer() const
{
    return *pData_->pDbCurrentPlayer_;
}

void MachGuiDatabase::currentPlayer( MachGuiDbPlayer* pDbPlayer )
{
    PRE( pDbPlayer != NULL );

    //Store the player
    pData_->pDbCurrentPlayer_ = pDbPlayer;

    //Mark it as the most recently used
    pDbPlayer->useSequenceId( pData_->nextUseSequenceId_++ );

    //Set up the campaign complete flags for the player
    setCampaignCompleteFlags( *pDbPlayer );
}

bool MachGuiDatabase::hasCurrentPlayer() const
{
	return pData_->pDbCurrentPlayer_ != NULL;
}

void MachGuiDatabase::clearCurrentPlayer()
{
	pData_->pDbCurrentPlayer_ = NULL;
}

void MachGuiDatabase::clearAllCampaignCompleteFlags()
{
    //Do each system
    MachGuiIDatabase::Systems& systems = pData_->campaignSystems_;
    for( MachGuiIDatabase::Systems::iterator it = systems.begin(); it != systems.end(); ++it )
    {
        MachGuiDbSystem* pSystem = (*it);
        pSystem->isComplete( false );

        //Do its planets
        uint nPlanets = pSystem->nPlanets();
        for( uint i = nPlanets; i--; )
        {
            MachGuiDbPlanet& planet = pSystem->planet( i );
            planet.isComplete( false );


            //Do all its scenarios
            uint nScenarios = planet.nScenarios();
            for( uint j = nScenarios; j--; )
                planet.scenario( j ).isComplete( false );
        }
    }
}

void MachGuiDatabase::setPlayerScenarioFlags( const MachGuiDbPlayer& player )
{
    //Iterate through the players scenarios and mark every one s/he has won complete
    uint n = player.nPlayerScenarios();
    for( uint i = n; i--; )
    {
        MachGuiDbPlayerScenario& playerScenario = player.playerScenario( i );
        if( playerScenario.hasWon() )
        {
            MachGuiDbScenario& scenario = playerScenario.scenario();
            playerScenario.scenario().isComplete( true );
        }
    }
}

void MachGuiDatabase::setCampaignCompleteFlags( const MachGuiDbPlayer& player )
{
    //Clear all the flags
    clearAllCampaignCompleteFlags();

    //Set the scenario complete flags for the player
    setPlayerScenarioFlags( player );

    //Now set the complete flags correctly for each system
    MachGuiIDatabase::Systems& systems = pData_->campaignSystems_;
    for( MachGuiIDatabase::Systems::iterator it = systems.begin(); it != systems.end(); ++it )
    {
        MachGuiDbSystem* pSystem = (*it);
        bool systemComplete = true;

        //Do its planets
        uint nPlanets = pSystem->nPlanets();
        for( uint i = nPlanets; i--; )
        {
            MachGuiDbPlanet& planet = pSystem->planet( i );
            bool planetComplete = true;

            //Check all its scenarios
            uint nScenarios = planet.nScenarios();
            for( uint j = nScenarios; planetComplete and j--; )
            {
                planetComplete = planet.scenario( j ).isComplete();
            }

            //If any planet is not complete, the system isn't either
            if( not planetComplete )
                systemComplete = false;
        }

        pSystem->isComplete( systemComplete );
    }
}

void MachGuiDatabase::clearAllTextData()
{
    //Do each system
    MachGuiIDatabase::Systems& systems = pData_->campaignSystems_;
    for( MachGuiIDatabase::Systems::iterator it = systems.begin(); it != systems.end(); ++it )
    {
        MachGuiDbSystem* pSystem = (*it);
        pSystem->clearTextData();

        //Do its planets
        uint nPlanets = pSystem->nPlanets();
        for( uint i = nPlanets; i--; )
        {
            MachGuiDbPlanet& planet = pSystem->planet( i );
            planet.clearTextData();

            //Do all its scenarios
            uint nScenarios = planet.nScenarios();
            for( uint j = nScenarios; j--; )
                planet.scenario( j ).clearTextData();
        }
    }
}

//static
bool MachGuiDatabase::doCustomScenariosExist() {
    return customDatabaseSourcePath().existsAsFile();
}

uint MachGuiDatabase::nSavedGames() const
{
    return pData_->savedGames_.size();
}

MachGuiDbSavedGame& MachGuiDatabase::savedGame( uint index ) const
{
    PRE( index < nSavedGames() );

    //return in reverse order, so they appear in correct order in the list
    return *(pData_->savedGames_[ pData_->savedGames_.size() - 1 - index ]);
}

void MachGuiDatabase::addSavedGame( MachGuiDbSavedGame* pDbSavedGame )
{
    pData_->savedGames_.push_back( pDbSavedGame );
}

void MachGuiDatabase::removeSavedGame( MachGuiDbSavedGame* pDbSavedGame )
{
    //Find the position of the entry in the collection, and erase it
    MachGuiIDatabase::SavedGames& savedGames = pData_->savedGames_;
    MachGuiIDatabase::SavedGames::iterator it = find( savedGames.begin(), savedGames.end(), pDbSavedGame );
    if( it != savedGames.end() )
        savedGames.erase( it );
}

//static
void MachGuiDatabase::registerDerivedClasses()
{
    //Only do it once
    static bool firstTime = true;
    if( firstTime )
    {
        firstTime = false;

        PER_REGISTER_DERIVED_CLASS( MachGuiDbElement );
        PER_REGISTER_DERIVED_CLASS( MachGuiDbScenario );
        PER_REGISTER_DERIVED_CLASS( MachGuiDbPlanet );
        PER_REGISTER_DERIVED_CLASS( MachGuiDbSystem );
    }
}

void MachGuiDatabase::removePlayer( MachGuiDbPlayer* pDbPlayer )
{
    //Find the player in the collection
    MachGuiIDatabase::Players::iterator it = find( pData_->players_.begin(), pData_->players_.end(), pDbPlayer );
    ASSERT( it != pData_->players_.end(), "Attempt to remove non-existent player" );

    //Erase the entry
    pData_->players_.erase( it );

    //Search all the saved games and nullify any pointers to this player
    for( MachGuiIDatabase::SavedGames::iterator it = pData_->savedGames_.begin(); it != pData_->savedGames_.end(); ++it )
    {
        MachGuiDbSavedGame* pDbSavedGame = (*it);

        if( pDbSavedGame->hasPlayer()  and  &(pDbSavedGame->player()) == pDbPlayer )
            pDbSavedGame->player( NULL );
    }

    //Delete the player
    _DELETE( pDbPlayer );
}

bool MachGuiDatabase::campaignScenario( const string& name, MachGuiDbScenario** pDbScenario )
{
    bool foundIt = false;

    //Check all the campaigns
    for( size_t i = 0; not foundIt  and  i != nCampaignSystems(); ++i )
    {
        MachGuiDbSystem& system = campaignSystem( i );
        for( size_t j = 0; not foundIt  and  j != system.nPlanets(); ++j )
        {
            MachGuiDbPlanet& planet = system.planet( j );
            for( size_t k = 0; not foundIt  and  k != planet.nScenarios(); ++k )
            {
                MachGuiDbScenario& scenario = planet.scenario( k );
                if( scenario.name() == name )
                {
                    foundIt = true;
                    *pDbScenario = &scenario;
                }
            }
        }
    }

    return foundIt;
}

MachGuiDatabaseHandler& MachGuiDatabase::handler()
{
    return *pData_->pDbHandler_;
}

void MachGuiDatabase::currentScenario( MachGuiDbScenario* pDbScenario )
{
    pData_->pCurrentDbScenario_ = pDbScenario;
}

MachGuiDbScenario& MachGuiDatabase::currentScenario() const
{
    PRE( hasCurrentScenario() );
    return *pData_->pCurrentDbScenario_;
}

bool MachGuiDatabase::hasCurrentScenario() const
{
    return pData_->pCurrentDbScenario_ != NULL;
}

/* End DATABASE.CPP *************************************************/
