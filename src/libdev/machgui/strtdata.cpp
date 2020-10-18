/*
 * S T R T D A T A . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/strtdata.hpp"
#include "system/registry.hpp"
#include "machgui/dbscenar.hpp"
#include "machgui/ctxready.hpp"
#include "machgui/startup.hpp"
#include "machgui/messbrok.hpp"
#include "machgui/gui.hpp"
#include "machgui/chatwind.hpp"
#include "machgui/database.hpp"
#include "machgui/dbsystem.hpp"
#include "machgui/dbplanet.hpp"
#include "machgui/dbscenar.hpp"
#include "machgui/strtdata.hpp"
#include "machgui/chatmsgs.hpp"
#include "machlog/network.hpp"
#include "afx/resource.hpp"
#include "machgui/internal/strings.hpp"
#include "machgui/internal/mgsndman.hpp"
#include "gui/restring.hpp"
#include "mathex/random.hpp"

MachGuiStartupData::PlayerInfo::PlayerInfo()
{
	reset();
}

string MachGuiStartupData::PlayerInfo::getDisplayName() const
{
	string retVal;

	switch ( status_ )
	{
		case OPEN:
			retVal = MachGui::openText();
			break;
		case CLOSED:
			retVal = MachGui::closedText();
			break;
		case COMPUTER:
			retVal = MachGui::computerText();
			break;
		case UNAVAILABLE:
			retVal = MachGui::unavailableText();
			break;
		case HUMAN:
			retVal = playerName_;
			break;
		DEFAULT_ASSERT_BAD_CASE( status_ );
	}

	DEBUG_STREAM( DIAG_NEIL, "MachGuiStartupData::PlayerInfo::getDisplayName " << retVal << std::endl );

	return retVal;
}

void MachGuiStartupData::PlayerInfo::setPlayerName( const string& playerName )
{
	ASSERT( playerName.length() <= MAX_PLAYERNAME_LEN, "players name too long" )

	DEBUG_STREAM( DIAG_NEIL, "MachGuiStartupData::PlayerInfo::setPlayerName " << playerName << " " << playerName_ << std::endl );

	// Set status_ to correct value...
	if ( strcasecmp( playerName.c_str(), MachGui::openText().c_str() ) == 0 )
	{
		status_ = OPEN;
		race_ = MachPhys::NORACE;
		hasMachinesCD_ = false;
		ready_ = false;
		// Copy name into playerInfo structure.
		strcpy( playerName_, playerName.c_str() );
	}
	else if ( strcasecmp( playerName.c_str(), MachGui::computerText().c_str() ) == 0 )
	{
		status_ = COMPUTER;
		hasMachinesCD_ = false;
		ready_ = false;
		// Copy name into playerInfo structure.
		strcpy( playerName_, playerName.c_str() );
	}
	else if ( strcasecmp( playerName.c_str(), MachGui::closedText().c_str() ) == 0 )
	{
		status_ = CLOSED;
		race_ = MachPhys::NORACE;
		hasMachinesCD_ = false;
		ready_ = false;
		// Copy name into playerInfo structure.
		strcpy( playerName_, playerName.c_str() );
	}
	else if ( strcasecmp( playerName.c_str(), MachGui::unavailableText().c_str() ) == 0 )
	{
		status_ = UNAVAILABLE;
		race_ = MachPhys::NORACE;
		hasMachinesCD_ = false;
		ready_ = false;
		// Copy name into playerInfo structure.
		strcpy( playerName_, playerName.c_str() );
	}
	else
	{
		status_ = HUMAN;

		// Is new name same as old name? No need to change...
		if ( strcasecmp( playerName_, playerName.c_str() ) != 0 )
		{
			ready_ = false;
			// Copy name into playerInfo structure.
			strcpy( playerName_, playerName.c_str() );
		}
	}
}

void MachGuiStartupData::PlayerInfo::reset()
{
	playerName_[0] = '\0';
	status_ = OPEN;
	race_ = MachPhys::NORACE;
	ready_ = false;
	host_ = false;
	hasMachinesCD_ = false;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////

MachGuiStartupData::GameSettings::GameSettings():
 mapSizeId_(0), terrainTypeId_(0), scenarioId_(0), playerRace_(MachPhys::Race::RED)
{
	// Read game settings from registry (or default to initial values)
	SysRegistry::KeyHandle handle;
	if( SysRegistry::instance().onlyOpenKey( "Game Settings\\Resources", &handle, SysRegistry::CURRENT_USER ) == SysRegistry::SUCCESS )
	{
		resources_ = (MachLog::ResourcesAvailable)SysRegistry::instance().queryIntegerValue( "Game Settings\\Resources", "Value", SysRegistry::CURRENT_USER );
	}
	else
	{
		resources_ = MachLog::RES_HIGH;
	}

	if( SysRegistry::instance().onlyOpenKey( "Game Settings\\Starting Resources", &handle, SysRegistry::CURRENT_USER ) == SysRegistry::SUCCESS )
	{
		startingResources_ = (MachLog::StartingResources)SysRegistry::instance().queryIntegerValue( "Game Settings\\Starting Resources", "Value", SysRegistry::CURRENT_USER );
	}
	else
	{
		startingResources_ = MachLog::STARTING_RESOURCES_DEFAULT;
	}

	if( SysRegistry::instance().onlyOpenKey( "Game Settings\\Starting Positions", &handle, SysRegistry::CURRENT_USER ) == SysRegistry::SUCCESS )
	{
		startingPosition_ = (MachLog::RandomStarts)SysRegistry::instance().queryIntegerValue( "Game Settings\\Starting Positions", "Value", SysRegistry::CURRENT_USER );
	}
	else
	{
		startingPosition_ = MachLog::RANDOM_START_LOCATIONS;
	}

	if( SysRegistry::instance().onlyOpenKey( "Game Settings\\Victory Condition", &handle, SysRegistry::CURRENT_USER ) == SysRegistry::SUCCESS )
	{
		//victoryCondition_ = (MachLog::VictoryCondition)SysRegistry::instance().queryIntegerValue( "Game Settings\\Victory Condition", "Value", SysRegistry::CURRENT_USER );
		victoryCondition_ = _STATIC_CAST(VictoryCondition,
            (MachLog::VictoryCondition)SysRegistry::instance().queryIntegerValue( "Game Settings\\Victory Condition", "Value", SysRegistry::CURRENT_USER ) );
	}
	else
	{
		victoryCondition_ = VC_DEFAULT;
	}

	if( SysRegistry::instance().onlyOpenKey( "Game Settings\\Tech Level", &handle, SysRegistry::CURRENT_USER ) == SysRegistry::SUCCESS )
	{
		techLevel_ = (MachLog::TechnologyLevel)SysRegistry::instance().queryIntegerValue( "Game Settings\\Tech Level", "Value", SysRegistry::CURRENT_USER );
	}
	else
	{
		techLevel_ = MachLog::TECH_LEVEL_DEFAULT;
	}

	if( SysRegistry::instance().onlyOpenKey( "Game Settings\\Fog Of War", &handle, SysRegistry::CURRENT_USER ) == SysRegistry::SUCCESS )
	{
		fogOfWar_ = (bool)SysRegistry::instance().queryIntegerValue( "Game Settings\\Fog Of War", "Value", SysRegistry::CURRENT_USER );
	}
	else
	{
		fogOfWar_ = true;
	}

	if( SysRegistry::instance().onlyOpenKey( "Game Settings\\Broadcast Alliances", &handle, SysRegistry::CURRENT_USER ) == SysRegistry::SUCCESS )
	{
		broadcastAlliances_ = (bool)SysRegistry::instance().queryIntegerValue( "Game Settings\\Broadcast Alliances", "Value", SysRegistry::CURRENT_USER );
	}
	else
	{
		broadcastAlliances_ = false;
	}

	if( SysRegistry::instance().onlyOpenKey( "Game Settings\\Disable First Person", &handle, SysRegistry::CURRENT_USER ) == SysRegistry::SUCCESS )
	{
		disableFirstPerson_ = (bool)SysRegistry::instance().queryIntegerValue( "Game Settings\\Disable First Person", "Value", SysRegistry::CURRENT_USER );
	}
	else
	{
		disableFirstPerson_ = false;
	}

	numPlayers_ = 4;

    MexBasicRandom rng = MexBasicRandom::constructSeededFromTime();
	randomStartsSeed_ = mexRandomInt( &rng, 0, 20000 );
}

MachGuiStartupData::GameSettings::~GameSettings()
{
	// Update registry
	SysRegistry::instance().setIntegerValue( "Game Settings\\Resources", "Value", resources_, SysRegistry::CURRENT_USER );
	SysRegistry::instance().setIntegerValue( "Game Settings\\Starting Resources", "Value", startingResources_, SysRegistry::CURRENT_USER );
	SysRegistry::instance().setIntegerValue( "Game Settings\\Starting Positions", "Value", startingPosition_, SysRegistry::CURRENT_USER );
	SysRegistry::instance().setIntegerValue( "Game Settings\\Victory Condition", "Value", victoryCondition_, SysRegistry::CURRENT_USER );
	SysRegistry::instance().setIntegerValue( "Game Settings\\Tech Level", "Value", techLevel_, SysRegistry::CURRENT_USER );
	SysRegistry::instance().setIntegerValue( "Game Settings\\Fog Of War", "Value", fogOfWar_, SysRegistry::CURRENT_USER );
	SysRegistry::instance().setIntegerValue( "Game Settings\\Broadcast Alliances", "Value", broadcastAlliances_, SysRegistry::CURRENT_USER );
	SysRegistry::instance().setIntegerValue( "Game Settings\\Disable First Person", "Value", disableFirstPerson_, SysRegistry::CURRENT_USER );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

MachGuiStartupData::MachGuiStartupData( MachGuiStartupScreens* pStartupScreens )
:	pScenario_( NULL ),
	host_( false ),
	pCtxImReady_( NULL ),
	pStartupScreens_( pStartupScreens ),
	playerRace_( MachPhys::RED ),
	terminalMultiPlayerGameProblem_( false ),
    includedInGame_( false )
{
	NETWORK_STREAM("MachGuiStartupData::MachGuiStartupData " << (void*)this << std::endl );
	SysRegistry::KeyHandle handle;
	// Get default for menu transitions (on/off)
	if( SysRegistry::instance().onlyOpenKey( "Options\\transitions", &handle, SysRegistry::CURRENT_USER ) == SysRegistry::SUCCESS )
	{
		transitionFlicsOn_ = SysRegistry::instance().queryIntegerValue( "Options\\transitions", "on", SysRegistry::CURRENT_USER );
	}
	else
	{
		transitionFlicsOn_ = true;
	}

	// Get default player race for skirmish games
	if( SysRegistry::instance().onlyOpenKey( "Game Settings\\Player Race", &handle, SysRegistry::CURRENT_USER ) == SysRegistry::SUCCESS )
	{
		playerRace_ = (MachPhys::Race)SysRegistry::instance().queryIntegerValue( "Game Settings\\Player Race", "Value", SysRegistry::CURRENT_USER );
	}

	newGameName_ = SysRegistry::instance().queryStringValue( "Misc", "New Game Name", SysRegistry::CURRENT_USER );
	playerName_ = SysRegistry::instance().queryStringValue( "Misc", "Players Name", SysRegistry::CURRENT_USER );
	lastProtocol_ = SysRegistry::instance().queryStringValue( "Misc", "Chosen Protocol", SysRegistry::CURRENT_USER );

	MexBasicRandom rng = MexBasicRandom::constructSeededFromTime();
	uniqueMachineNumber_ = mexRandomInt( &rng, INT_MIN, INT_MAX );

    TEST_INVARIANT;
}

MachGuiStartupData::~MachGuiStartupData()
{
    TEST_INVARIANT;
	NETWORK_STREAM("MachGuiStartupData::~MachGuiStartupData " << (void*)this << std::endl );

}

void MachGuiStartupData::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiStartupData& t )
{

    o << "MachGuiStartupData " << (void*)&t << " start" << std::endl;
    o << "MachGuiStartupData " << (void*)&t << " end" << std::endl;

    return o;
}

const string& MachGuiStartupData::playerName() const
{
	return playerName_;
}

void MachGuiStartupData::playerName( const string& playerName )
{
	playerName_ = playerName;

	// Store players name in registry so that it persists between different instances of the game
	SysRegistry::instance().setStringValue( "Misc", "Players Name", playerName_, SysRegistry::CURRENT_USER );
}

void MachGuiStartupData::scenario( MachGuiDbScenario* pScenario )
{
	pScenario_ = pScenario;
	//Set the current scenario in the database
    MachGuiDatabase::instance().currentScenario( pScenario );
}

MachGuiDbScenario* MachGuiStartupData::scenario()
{
	return pScenario_;
}

const string& MachGuiStartupData::connectionType() const
{
	if ( MachLogNetwork::instance().desiredProtocol() != "" )
		return MachLogNetwork::instance().desiredProtocol();
	else
		return lastProtocol_;
}

void MachGuiStartupData::connectionType( const string& ct, InitialiseConnection initialise )
{
	MachLogNetwork::instance().terminateAndReset();

	bool success = MachLogNetwork::instance().desiredProtocol( ct, ( MachLogNetwork::InitialiseConnection ) initialise );

	lastProtocol_ = ct;
	// Store chosen protocol in registry so that it persists between different instances of the game
	SysRegistry::instance().setStringValue( "Misc", "Chosen Protocol", lastProtocol_, SysRegistry::CURRENT_USER );

	ASSERT( success, "failed to set protocol" );
}

void MachGuiStartupData::joinGame( const string& str )
{
	joinGame_ = str;
	includedInGame_ = false;
	terminalMultiPlayerGameProblem_ = false;
	MachGuiChatWindow::clearAllText();
}

const string& MachGuiStartupData::joinGame() const
{
	return joinGame_;
}

void MachGuiStartupData::receivedJoinMessage( const string& playerName, int uniqueMachineNumber )
{
	NETWORK_STREAM("MachGuiStartupData::receivedJoinMessage " << playerName << std::endl );
	NETWORK_INDENT( 2 );
	if ( host_ )
	{
		bool nameChanged = false;
		bool uniqueName = false;
		int nameChangeNumber = 2;
		string newPlayerName = playerName;

		// Check to see if name is unique. If not then add a number on the end.
		while ( not uniqueName )
		{
			uniqueName = true;

			// Check against host
			if ( strcasecmp( getHostName().c_str(), newPlayerName.c_str() ) == 0 )
			{
				uniqueName = false;
				nameChanged = true;
				newPlayerName = playerName;
				char buffer[20];
//				newPlayerName += itoa( nameChangeNumber++, buffer, 10 );
				sprintf(buffer, "%d", nameChangeNumber++ );
				newPlayerName += buffer;
			}
			else // Check against other players
			{
				for ( ctl_vector<string>::iterator iter = availablePlayers_.begin(); iter != availablePlayers_.end(); ++iter )
				{
					if ( strcasecmp( (*iter).c_str(), newPlayerName.c_str() ) == 0 )
					{
						uniqueName = false;
						nameChanged = true;
						newPlayerName = playerName;
						char buffer[20];
//						newPlayerName += itoa( nameChangeNumber++, buffer, 10 );
                        sprintf(buffer, "%d", nameChangeNumber++ );
                        newPlayerName += buffer;
						break;
					}
				}
			}
		}

		NETWORK_STREAM("host\n");
		availablePlayers_.push_back( newPlayerName );

		if ( nameChanged )
		{
			NETWORK_STREAM("MachGuiStartupData::receivedJoinMessage name has changed " << newPlayerName << std::endl );
			// Tell player that his name has had to change.
			pStartupScreens_->messageBroker().sendNameChangeMessage( newPlayerName, uniqueMachineNumber );
		}

		int nextSlot = getNextAvailablePlayerSlot();
		NETWORK_STREAM("getNextAvailablePlayerSlot returned " << nextSlot << std::endl );

		if ( nextSlot >= 0 )
		{
	   		players_[nextSlot].status_ = PlayerInfo::HUMAN;
			players_[nextSlot].setPlayerName( newPlayerName );
			players_[nextSlot].race_ = getNextAvailableRace();
			NETWORK_STREAM("sendUpdatePlayersMessage\n" );
			sendUpdatePlayersMessage();
		}

		// Play sound
		MachGuiSoundManager::instance().playSound( "gui/sounds/chatmsg.wav" );

		// Display message telling host person has joined
		GuiResourceString hostMessage( IDS_MENUMSG_CLIENTJOIN, newPlayerName );
		MachGuiChatWindow::addText( hostMessage.asString() );

		// Send out this message so that the new player knows what scenario etc is
		// being used.
		NETWORK_STREAM("sendUpdateGameSettingsMessage\n" );
	    sendUpdateGameSettingsMessage();
	}
	NETWORK_INDENT( -2 );
	NETWORK_STREAM("MachGuiStartupData::receivedJoinMessage DONE " << playerName << std::endl );
}

void MachGuiStartupData::receivedUpdatePlayersMessage()
{
	NETWORK_STREAM("MachGuiStartupData::receivedUpdatePlayersMessage()\n" );
	ASSERT( not host_, "host getting receivedUpdatePlayersMessage" );

	if ( pCtxImReady_ )
	{
		pCtxImReady_->updatePlayerList();
	}
	else
	{
		NETWORK_ERRORS_STREAM("NW03: receivedUpdatePlayersMessage but pCtxImReady was NULL so I'm not doing anything with it\n" );
	}
}

void MachGuiStartupData::receivedUpdateGameSettingsMessage()
{
	ASSERT( not host_, "host getting receivedUpdateGameSettingsMessage" );

	ASSERT_INFO( gameSettings()->mapSizeId_ );
	ASSERT_INFO( gameSettings()->terrainTypeId_ );
	ASSERT_INFO( gameSettings()->scenarioId_ );

	bool found = false;

	// Based on the ids that have just been passed across the network find out which
	// scenario these are linked to.
	for ( MachGuiDatabase::TerrainSize loop = MachGuiDatabase::SMALL; loop <= MachGuiDatabase::LARGE and not found;
                loop = _STATIC_CAST(MachGuiDatabase::TerrainSize, _STATIC_CAST(uint, loop)+1 ) )
	{
		// Find system...
    	MachGuiDbSystem& system = MachGuiDatabase::instance().multiPlayerSystem( loop );

		if ( system.menuStringId() == gameSettings()->mapSizeId_ )
		{
			// Find planet...
			uint numPlanets = system.nPlanets();

			for ( uint loop = 0; loop < numPlanets and not found; ++loop )
			{
		    	MachGuiDbPlanet& planet = system.planet( loop );

				if ( planet.menuStringId() == gameSettings()->terrainTypeId_ )
				{
					// Find scenario...
					uint numScenarios = planet.nScenarios();

					for ( uint loop = 0; loop < numScenarios and not found; ++loop )
					{
				    	MachGuiDbScenario& scenario = planet.scenario( loop );

						if ( scenario.menuStringId() == gameSettings()->scenarioId_ )
						{
							pStartupScreens_->startupData()->scenario( &scenario );
							found = true;
						}
					}
				}
			}
		}
	}

	ASSERT( found, "network passed settings that are not valid" );

	if ( pCtxImReady_ )
		pCtxImReady_->updateGameSettings();
}

void MachGuiStartupData::hostGame( bool host )
{
	host_ = host;

	if ( host_ )
	{
		// Clean down available players list
		availablePlayers_.erase( availablePlayers_.begin(), availablePlayers_.end() );

		int nextSlot = getNextAvailablePlayerSlot();

		ASSERT( nextSlot >= 0, "Trying to host game but the players list is full already!!" )

		players_[nextSlot].status_ = PlayerInfo::HUMAN;
		players_[nextSlot].setPlayerName( playerName_ );
		players_[nextSlot].race_ = getNextAvailableRace();
		players_[nextSlot].host_ = true;

		terminalMultiPlayerGameProblem_ = false;
		MachGuiChatWindow::clearAllText();
		// Add message to say that we are waiting for other people to join in
		GuiResourceString waitingForPlayers( IDS_LOBBY_WAITING_FOR_PLAYERS );
		MachGuiChatWindow::addText( waitingForPlayers.asString() );
	}
}

void MachGuiStartupData::setCtxImReady( MachGuiCtxImReady* pCtxImReady )
{
	pCtxImReady_ = pCtxImReady;
}

void MachGuiStartupData::removeCtxImReady()
{
	NETWORK_STREAM("MachGuiStartupData::removeCtxImReady\n" );
	pCtxImReady_ = NULL;
	NETWORK_STREAM("MachGuiStartupData::removeCtxImReady DONE\n" );
}

ctl_vector< string >& MachGuiStartupData::availablePlayers()
{
	return availablePlayers_;
}

int MachGuiStartupData::getNextAvailablePlayerSlot() const
{
	// Choose open spaces first
	for ( size_t loop = 0; loop < 4; ++loop )
	{
		if ( players_[loop].status_ == PlayerInfo::OPEN )
		{
			return loop;
		}
	}

	// Replace computers with human players if no open spaces.
	for ( size_t loop = 0; loop < 4; ++loop )
	{
		if ( players_[loop].status_ == PlayerInfo::COMPUTER )
		{
			return loop;
		}
	}

	return -1;
}

MachPhys::Race MachGuiStartupData::getNextAvailableRace() const
{
	for ( MachPhys::Race race = MachPhys::RED; race < MachPhys::N_RACES; ++race )
	{
		if ( isRaceAvailable( race ) )	// Found an available race
			return race;
	}

	return MachPhys::NORACE; // No free race colours
}

MachGuiStartupData::PlayerInfo* MachGuiStartupData::players()
{
	return players_;
}

bool MachGuiStartupData::transitionFlicsOn( void ) const
{
	return transitionFlicsOn_;
}

void MachGuiStartupData::transitionFlicsOn( bool flicOn )
{
	transitionFlicsOn_ = flicOn;
}

bool MachGuiStartupData::isRaceAvailable( MachPhys::Race race ) const
{
	if ( race == MachPhys::NORACE )
		return true;

	bool free = true;

	for ( size_t loop = 0; loop < 4 and free; ++loop )
	{
		if ( players_[loop].race_ == race )
		{
		    free = false;
		}
	}

	return free;
}

void MachGuiStartupData::receivedRaceChangeRequest( const string& playerName, size_t playerIndex, MachPhys::Race race )
{
	if ( isHost() )
	{
		if ( isRaceAvailable( race ) )
		{
			if ( playerName == players_[playerIndex].playerName_ )
			{
			    players_[playerIndex].race_ = race;

				sendUpdatePlayersMessage();
			}
		}
	}
}

bool MachGuiStartupData::isHost() const
{
	return host_;
}

void MachGuiStartupData::sendUpdatePlayersMessage()
{
	ASSERT( isHost(), "Only the host can call sendUpdatePlayersMessage" );

	// Update my own list
	if ( pCtxImReady_ )
		pCtxImReady_->updatePlayerList();

	// Tell all players to update their list of players.
	pStartupScreens_->messageBroker().sendUpdatePlayersMessage();
}

void MachGuiStartupData::receivedImReadyMessage( const string& playerName, bool ready )
{
	if( isHost() )
	{
		for ( size_t loop = 0; loop < 4; ++loop )
		{
			if ( strcasecmp( playerName.c_str(), players_[loop].playerName_ ) == 0 )
			{
			    players_[loop].ready_ = ready;
			}
		}
		sendUpdatePlayersMessage();
	}
}

bool MachGuiStartupData::isReady( const string& playerName )
{
	for ( size_t loop = 0; loop < 4; ++loop )
	{
		if ( strcasecmp( playerName.c_str(), players_[loop].playerName_ ) == 0 )
		{
		    return players_[loop].ready_;
		}
	}

	return false;
}

bool MachGuiStartupData::canStartMultiPlayerGame() const
{
	// Only the host can start a multi player game
	if ( not isHost() or terminalMultiPlayerGameProblem_ )
		return false;

	bool canStart = true;
	size_t humans = 0;

	for ( size_t loop = 0; loop < 4 and canStart; ++loop )
	{
		if ( players_[loop].status_ == PlayerInfo::HUMAN )
		{
			++humans;

			// Check for hosts name
			if ( strcasecmp( players_[loop].playerName_, playerName().c_str() ) == 0 )
			{
				// Host never has to press "I'm Ready" button so make an exception at this point
			}
			else
			{
		    	canStart = players_[loop].ready_;
			}
		}
	}

	// Must be at least 2 human players
	if ( humans < 2 )
		canStart = false;

	return canStart;
}

bool MachGuiStartupData::updateIncludedInGame()
{
	NETWORK_STREAM("MachGuiStartupData::updateIncludedInGame() " << std::endl );
	NETWORK_STREAM(" playerName.length " << playerName().length() << std::endl );
	NETWORK_STREAM(" playername " << playerName() << std::endl );
	includedInGame_ = false;
	if( playerName().length() > 0 )
	{
		for ( size_t loop = 0; loop < 4 and not includedInGame_; ++loop )
		{
			if ( players_[loop].status_ == PlayerInfo::HUMAN )
			{
				// Check for players name
				if ( strcasecmp( players_[loop].playerName_, playerName().c_str() ) == 0 )
				{
					includedInGame_ = true;
				}
			}
		}
	}

	return includedInGame_;
}

bool MachGuiStartupData::includedInGame() const
{
	return includedInGame_;
}

void MachGuiStartupData::receivedHostCancelMessage()
{
	ASSERT( not isHost(), "host shouldn't receive this message" );

	terminalMultiPlayerGameProblem_ = true;

	GuiResourceString str( IDS_MENUMSG_HOSTTERM, getHostName() );

	// Play sound
	MachGuiSoundManager::instance().playSound( "gui/sounds/chatmsg.wav" );

	// Display message saying host has canceled
	MachGuiChatWindow::addText( str.asString() );

	if ( pCtxImReady_ )
		pCtxImReady_->updatePlayerList();
}

bool MachGuiStartupData::terminalMultiPlayerGameProblem() const
{
	return terminalMultiPlayerGameProblem_;
}

void MachGuiStartupData::receivedClientCancelMessage( const string& playerName )
{
	if ( isHost() )
	{
		string replacePlayerName = MachGui::openText();

		// Find player not currently included in game and replace slot left empty by client with
		// this new player
		for ( ctl_vector<string>::iterator iter = availablePlayers_.begin(); iter != availablePlayers_.end(); ++iter )
		{
			string iterPlayerName = *iter;
			bool included = false;

			for ( size_t loop = 0; loop < 4; ++loop )
			{
				if ( players_[loop].status_ == PlayerInfo::HUMAN )
				{
					// Check for players name
					if ( strcasecmp( players_[loop].playerName_, iterPlayerName.c_str() ) == 0 )
					{
						included = true;
						break;
					}
				}
			}

			// Found a player not included in the game
			if ( not included )
			{
				replacePlayerName =  iterPlayerName;
				break;
			}
		}

		for ( size_t loop = 0; loop < 4; ++loop )
		{
			if ( players_[loop].status_ == PlayerInfo::HUMAN )
			{
				// Check for players name
				if ( strcasecmp( players_[loop].playerName_, playerName.c_str() ) == 0 )
				{
					players_[loop].setPlayerName( replacePlayerName );
					break;
				}
			}
		}

		// Remove name from available players list
		ctl_vector<string>::iterator iter = find( availablePlayers_.begin(), availablePlayers_.end(), playerName );
		if ( iter != availablePlayers_.end() )
			availablePlayers_.erase( iter );

		// Play sound
		MachGuiSoundManager::instance().playSound( "gui/sounds/chatmsg.wav" );

		// Display message telling host person has left
		GuiResourceString hostMessage( IDS_MENUMSG_CLIENTTERM, playerName );
		MachGuiChatWindow::addText( hostMessage.asString() );

		sendUpdatePlayersMessage();
	}
}

void MachGuiStartupData::receivedStartMessage()
{
	NETWORK_STREAM("MachGuiStartupData::receivedStartMessage\n" );
	if( pStartupScreens_->currentContext() == MachGuiStartupScreens::CTX_MULTI_GAME )
	{
		NETWORK_STREAM(" already in CTX_MULTI_GAME so returning\n" );
		return;
	}

	NETWORK_STREAM(" checking against updateIncludedInGame" << std::endl );
	if ( not updateIncludedInGame() )
	{
		NETWORK_STREAM(" player not in the game so set some values" << std::endl );

		// Play sound
		MachGuiSoundManager::instance().playSound( "gui/sounds/chatmsg.wav" );

		// This player has not been included in the game. Display message and
		// grey out all multiplayer buttons.
		terminalMultiPlayerGameProblem_ = true;
		GuiResourceString message( IDS_MENUMSG_GAMESTARTNOTIN );
		MachGuiChatWindow::addText( message.asString() );

//		NETWORK_STREAM(" pCtxImReady_ " << (void*)pCtxImReady_ << std::endl );
//		if ( pCtxImReady_ )
//		{
//			NETWORK_STREAM(" calling updatePlayerList" << std::endl );
//			pCtxImReady_->updatePlayerList();
//		}

		NETWORK_STREAM("calling terminate and reset" << std::endl );
		MachLogNetwork::instance().terminateAndReset();
		NETWORK_STREAM("calling switch context" << std::endl );
		switch( pStartupScreens_->currentContext() )
		{
			case MachGuiStartupScreens::CTX_IMREADY:
				pStartupScreens_->buttonAction( MachGuiStartupScreens::EXIT );
				break;
			default:
				pStartupScreens_->switchContext( MachGuiStartupScreens::CTX_JOIN );
		}
		// Disconnect from network ( keep protocol!! )
//		string ct = connectionType(); // Store connection type because terminateAndReset sets it to ""
//		connectionType( ct );
	}
	else
	{
		NETWORK_STREAM(" switch context to CTX_MULTI_GAME\n" );
		pStartupScreens_->switchContext( MachGuiStartupScreens::CTX_MULTI_GAME );
	}
}

string MachGuiStartupData::getHostName() const
{
	string retVal;

	for ( size_t loop = 0; loop < 4; ++loop )
	{
		if ( players_[loop].host_ )
		{
			retVal = players_[loop].getDisplayName();
			break;
		}
	}

	return retVal;
}

void MachGuiStartupData::initMachLogNetwork()
{
	HAL_STREAM("MachGuiStartupData::initMachLogNetwork\n" );

	// Assign a race to all players marked as NORACE.
	for ( size_t loop = 0; loop < 4; ++loop )
	{
		if( players_[loop].race_ == MachPhys::NORACE )
		{
			HAL_STREAM("player " << loop << " marked as NORACE calling getNextAvailableRace\n" );
		}
		if ( players_[loop].race_ == MachPhys::NORACE )
		{
		     players_[loop].race_ = getNextAvailableRace();
			HAL_STREAM("player " << loop << " now has race " << players_[loop].race_ << std::endl );
		}
	}

	// Set ready status on all slots not partaking in game
	for ( size_t loop = 0; loop < 4; ++loop )
	{
		if ( players_[loop].status_ != PlayerInfo::HUMAN )
		{
			MachLogNetwork::instance().ready( players_[loop].race_, true );
		}
	}

	// The network which race is local race
	for ( size_t loop = 0; loop < 4; ++loop )
	{
		if ( players_[loop].status_ == PlayerInfo::HUMAN and
			 strcasecmp( playerName().c_str(), players_[loop].playerName_ ) == 0 )
		{
			MachLogNetwork::instance().localRace( players_[loop].race_ );
		}
	}
}

const string& MachGuiStartupData::newGameName() const
{
	return newGameName_;
}

void MachGuiStartupData::newGameName( const string& gameName )
{
	newGameName_ = gameName;

	// Store new game name in registry so that it persists between different instances of the game
	SysRegistry::instance().setStringValue( "Misc", "New Game Name", newGameName_, SysRegistry::CURRENT_USER );
}

void MachGuiStartupData::resetPlayers()
{
	for ( size_t loop = 0; loop < 4; ++loop )
	{
		players_[loop].reset();
	}
}


MachGuiStartupData::GameSettings* MachGuiStartupData::gameSettings()
{
	return &gameSettings_;
}

void MachGuiStartupData::sendUpdateGameSettingsMessage()
{
	PRE( isHost() );

	MachGuiDbScenario& currScenario = *scenario();

	gameSettings_.scenarioId_ = currScenario.menuStringId();
	gameSettings_.terrainTypeId_ = currScenario.planet().menuStringId();
	gameSettings_.mapSizeId_ = currScenario.planet().system().menuStringId();
	//every time we send this rerandomize the value - this will ensure that we get no repition on a restart
	//as we know at least one new updateGameSettings message will be sent. (In fact several will be).

    MexBasicRandom rng = MexBasicRandom::constructSeededFromTime();

	gameSettings_.randomStartsSeed_ = mexRandomInt( &rng, 0, 20000 );

	pStartupScreens_->messageBroker().sendUpdateGameSettingsMessage();
}

///////////////////////////////////////////////////////////////////////////////////////////////

void MachGuiStartupData::resources( MachLog::ResourcesAvailable res )
{
	gameSettings()->resources_ = res;
}

void MachGuiStartupData::resources( const string& str )
{
	GuiResourceString highStr( IDS_MENU_HIGH );
	GuiResourceString mediumStr( IDS_MENU_MEDIUM );
	GuiResourceString lowStr( IDS_MENU_LOW );

	if ( strcasecmp( highStr.asString().c_str(), str.c_str() ) == 0 )
	{
		resources( MachLog::RES_HIGH );
	}
	else if ( strcasecmp( mediumStr.asString().c_str(), str.c_str() ) == 0 )
	{
		resources( MachLog::RES_MEDIUM );
	}
	else if ( strcasecmp( lowStr.asString().c_str(), str.c_str() ) == 0 )
	{
		resources( MachLog::RES_LOW );
	}
	else
	{
		ASSERT_INFO( str );
		ASSERT( false, "MachGuiStartupData::resources passed invalid string" );
	}
}

MachLog::ResourcesAvailable MachGuiStartupData::resources() const
{
	return gameSettings_.resources_;
}

string MachGuiStartupData::resourcesStr() const
{
	GuiResourceString highStr( IDS_MENU_HIGH );
	GuiResourceString mediumStr( IDS_MENU_MEDIUM );
	GuiResourceString lowStr( IDS_MENU_LOW );

	switch ( resources() )
	{
		case MachLog::RES_HIGH:
			return highStr.asString();
		case MachLog::RES_MEDIUM:
			return mediumStr.asString();
		case MachLog::RES_LOW:
			return lowStr.asString();
		DEFAULT_ASSERT_BAD_CASE( resources() );
	}

	return "bad";
}

///////////////////////////////////////////////////////////////////////////////////////////////

void MachGuiStartupData::fogOfWar( bool fog )
{
	gameSettings()->fogOfWar_ = fog;
}

void MachGuiStartupData::fogOfWar( const string& str )
{
	GuiResourceString onStr( IDS_MENU_ON );
	GuiResourceString offStr( IDS_MENU_OFF );

	if ( strcasecmp( onStr.asString().c_str(), str.c_str() ) == 0 )
	{
		fogOfWar( true );
	}
	else if ( strcasecmp( offStr.asString().c_str(), str.c_str() ) == 0 )
	{
		fogOfWar( false );
	}
	else
	{
		ASSERT_INFO( str );
		ASSERT( false, "MachGuiStartupData::fogOfWar passed invalid string" );
	}
}

bool MachGuiStartupData::fogOfWar()	const
{
	return gameSettings_.fogOfWar_;
}

string MachGuiStartupData::fogOfWarStr() const
{
	GuiResourceString onStr( IDS_MENU_ON );
	GuiResourceString offStr( IDS_MENU_OFF );

	if ( fogOfWar()	)
	{
		return onStr.asString();
	}

	return offStr.asString();
}

///////////////////////////////////////////////////////////////////////////////////////////////

void MachGuiStartupData::startingResources( MachLog::StartingResources startRes )
{
	gameSettings()->startingResources_ = startRes;
}

void MachGuiStartupData::startingResources( const string& str )
{
	GuiResourceString defaultStr( IDS_MENU_DEFAULT );
	GuiResourceString level1Str( IDS_MENU_RESOURCESLEVEL1 );
	GuiResourceString level2Str( IDS_MENU_RESOURCESLEVEL2 );
	GuiResourceString level3Str( IDS_MENU_RESOURCESLEVEL3 );
	GuiResourceString level4Str( IDS_MENU_RESOURCESLEVEL4 );
	GuiResourceString level5Str( IDS_MENU_RESOURCESLEVEL5 );

	if ( strcasecmp( defaultStr.asString().c_str(), str.c_str() ) == 0 )
	{
		startingResources( MachLog::STARTING_RESOURCES_DEFAULT );
	}
	else if ( strcasecmp( level1Str.asString().c_str(), str.c_str() ) == 0 )
	{
		startingResources( MachLog::STARTING_RESOURCES_RES_LOW );
	}
	else if ( strcasecmp( level2Str.asString().c_str(), str.c_str() ) == 0 )
	{
		startingResources( MachLog::STARTING_RESOURCES_MEDIUM );
	}
	else if ( strcasecmp( level3Str.asString().c_str(), str.c_str() ) == 0 )
	{
		startingResources( MachLog::STARTING_RESOURCES_HIGH );
	}
	else if ( strcasecmp( level4Str.asString().c_str(), str.c_str() ) == 0 )
	{
		startingResources( MachLog::STARTING_RESOURCES_VERY_HIGH );
	}
	else if ( strcasecmp( level5Str.asString().c_str(), str.c_str() ) == 0 )
	{
		startingResources( MachLog::STARTING_RESOURCES_SUPER_HIGH );
	}
	else
	{
		ASSERT_INFO( str );
		ASSERT( false, "MachGuiStartupData::startingResources passed invalid string" );
	}
}

MachLog::StartingResources MachGuiStartupData::startingResources() const
{
	return gameSettings_.startingResources_;
}

string MachGuiStartupData::startingResourcesStr() const
{
 	switch ( startingResources() )
	{
		case MachLog::STARTING_RESOURCES_DEFAULT:
		{
			GuiResourceString resStr( IDS_MENU_DEFAULT );
			return resStr.asString();
		}
		case MachLog::STARTING_RESOURCES_RES_LOW:
		{
			GuiResourceString resStr( IDS_MENU_RESOURCESLEVEL1 );
			return resStr.asString();
		}
		case MachLog::STARTING_RESOURCES_MEDIUM:
		{
			GuiResourceString resStr( IDS_MENU_RESOURCESLEVEL2 );
			return resStr.asString();
		}
		case MachLog::STARTING_RESOURCES_HIGH:
		{
			GuiResourceString resStr( IDS_MENU_RESOURCESLEVEL3 );
			return resStr.asString();
		}
		case MachLog::STARTING_RESOURCES_VERY_HIGH:
		{
			GuiResourceString resStr( IDS_MENU_RESOURCESLEVEL4 );
			return resStr.asString();
		}
		case MachLog::STARTING_RESOURCES_SUPER_HIGH:
		{
			GuiResourceString resStr( IDS_MENU_RESOURCESLEVEL5 );
			return resStr.asString();
		}
		DEFAULT_ASSERT_BAD_CASE( startingResources() );
	}

	return "bad";

}

///////////////////////////////////////////////////////////////////////////////////////////////

void MachGuiStartupData::startingPosition( MachLog::RandomStarts sp )
{
	gameSettings()->startingPosition_ = sp;
}

void MachGuiStartupData::startingPosition( const string& str )
{
	GuiResourceString fixedStr( IDS_MENU_STARTFIXED );
	GuiResourceString randomStr( IDS_MENU_STARTRANDOM );

	if ( strcasecmp( fixedStr.asString().c_str(), str.c_str() ) == 0 )
	{
		startingPosition( MachLog::FIXED_START_LOCATIONS );
	}
	else if ( strcasecmp( randomStr.asString().c_str(), str.c_str() ) == 0 )
	{
		startingPosition( MachLog::RANDOM_START_LOCATIONS );
	}
 	else
	{
		ASSERT_INFO( str );
		ASSERT( false, "MachGuiStartupData::startingPosition passed invalid string" );
	}
}

MachLog::RandomStarts MachGuiStartupData::startingPosition() const
{
	return gameSettings_.startingPosition_;
}

string MachGuiStartupData::startingPositionStr() const
{
 	switch ( startingPosition() )
	{
		case MachLog::FIXED_START_LOCATIONS:
		{
			GuiResourceString resStr( IDS_MENU_STARTFIXED );
			return resStr.asString();
		}
		case MachLog::RANDOM_START_LOCATIONS:
		{
			GuiResourceString resStr( IDS_MENU_STARTRANDOM );
			return resStr.asString();
		}
		DEFAULT_ASSERT_BAD_CASE( startingPosition() );
	}

	return "bad";
}

///////////////////////////////////////////////////////////////////////////////////////////////

void MachGuiStartupData::victoryCondition( VictoryCondition vc )
{
	gameSettings()->victoryCondition_ = vc;
}

void MachGuiStartupData::victoryCondition( const string& str )
{
	GuiResourceString defaultStr( IDS_MENU_DEFAULT );
	GuiResourceString taStr( IDS_MENU_VICTORYTANNIHILATION );
	GuiResourceString podStr( IDS_MENU_VICTORYDESTROYPOD );
	GuiResourceString timed5Str( IDS_MENU_VICTORYTIMED5 );
	GuiResourceString timed10Str( IDS_MENU_VICTORYTIMED10 );
	GuiResourceString timed15Str( IDS_MENU_VICTORYTIMED15 );
	GuiResourceString timed30Str( IDS_MENU_VICTORYTIMED30 );
	GuiResourceString timed45Str( IDS_MENU_VICTORYTIMED45 );
	GuiResourceString timed60Str( IDS_MENU_VICTORYTIMED60 );
	GuiResourceString timed90Str( IDS_MENU_VICTORYTIMED90 );
	GuiResourceString timed120Str( IDS_MENU_VICTORYTIMED120 );
	GuiResourceString timed180Str( IDS_MENU_VICTORYTIMED180 );

	if ( strcasecmp( defaultStr.asString().c_str(), str.c_str() ) == 0 )
	{
		victoryCondition( VC_DEFAULT );
	}
	else if ( strcasecmp( taStr.asString().c_str(), str.c_str() ) == 0 )
	{
		victoryCondition( VC_TOTALANNIHILATION );
	}
	else if ( strcasecmp( podStr.asString().c_str(), str.c_str() ) == 0 )
	{
		victoryCondition( VC_DESTROYPOD );
	}
	else if ( strcasecmp( timed5Str.asString().c_str(), str.c_str() ) == 0 )
	{
		victoryCondition( VC_TIMED5 );
	}
	else if ( strcasecmp( timed10Str.asString().c_str(), str.c_str() ) == 0 )
	{
		victoryCondition( VC_TIMED10 );
	}
	else if ( strcasecmp( timed15Str.asString().c_str(), str.c_str() ) == 0 )
	{
		victoryCondition( VC_TIMED15 );
	}
	else if ( strcasecmp( timed30Str.asString().c_str(), str.c_str() ) == 0 )
	{
		victoryCondition( VC_TIMED30 );
	}
	else if ( strcasecmp( timed45Str.asString().c_str(), str.c_str() ) == 0 )
	{
		victoryCondition( VC_TIMED45 );
	}
	else if ( strcasecmp( timed60Str.asString().c_str(), str.c_str() ) == 0 )
	{
		victoryCondition( VC_TIMED60 );
	}
	else if ( strcasecmp( timed90Str.asString().c_str(), str.c_str() ) == 0 )
	{
		victoryCondition( VC_TIMED90 );
	}
	else if ( strcasecmp( timed120Str.asString().c_str(), str.c_str() ) == 0 )
	{
		victoryCondition( VC_TIMED120 );
	}
	else if ( strcasecmp( timed180Str.asString().c_str(), str.c_str() ) == 0 )
	{
		victoryCondition( VC_TIMED180 );
	}
	else
	{
		ASSERT_INFO( str );
		ASSERT( false, "MachGuiStartupData::victoryCondition passed invalid string" );
	}
}

MachGuiStartupData::VictoryCondition MachGuiStartupData::victoryCondition() const
{
	return gameSettings_.victoryCondition_;
}

string MachGuiStartupData::victoryConditionStr() const
{
 	switch ( victoryCondition() )
	{
   		case VC_DEFAULT:
		{
			GuiResourceString resStr( IDS_MENU_DEFAULT );
			return resStr.asString();
		}
		case VC_TOTALANNIHILATION:
		{
			GuiResourceString resStr( IDS_MENU_VICTORYTANNIHILATION );
			return resStr.asString();
		}
		case VC_DESTROYPOD:
		{
			GuiResourceString resStr( IDS_MENU_VICTORYDESTROYPOD );
			return resStr.asString();
		}
		case VC_TIMED5:
		{
			GuiResourceString resStr( IDS_MENU_VICTORYTIMED5 );
			return resStr.asString();
		}
		case VC_TIMED10:
		{
			GuiResourceString resStr( IDS_MENU_VICTORYTIMED10 );
			return resStr.asString();
		}
		case VC_TIMED15:
		{
			GuiResourceString resStr( IDS_MENU_VICTORYTIMED15 );
			return resStr.asString();
		}
		case VC_TIMED30:
		{
			GuiResourceString resStr( IDS_MENU_VICTORYTIMED30 );
			return resStr.asString();
		}
		case VC_TIMED45:
		{
			GuiResourceString resStr( IDS_MENU_VICTORYTIMED45 );
			return resStr.asString();
		}
		case VC_TIMED60:
		{
			GuiResourceString resStr( IDS_MENU_VICTORYTIMED60 );
			return resStr.asString();
		}
		case VC_TIMED90:
		{
			GuiResourceString resStr( IDS_MENU_VICTORYTIMED90 );
			return resStr.asString();
		}
		case VC_TIMED120:
		{
			GuiResourceString resStr( IDS_MENU_VICTORYTIMED120 );
			return resStr.asString();
		}
		case VC_TIMED180:
		{
			GuiResourceString resStr( IDS_MENU_VICTORYTIMED180 );
			return resStr.asString();
		}
		DEFAULT_ASSERT_BAD_CASE( victoryCondition() );
	}

	return "bad";
}

///////////////////////////////////////////////////////////////////////////////////////////////

void MachGuiStartupData::numPlayers( int numPlayers )
{
	PRE( numPlayers >= 2 and numPlayers <= 4 );

	gameSettings()->numPlayers_ = numPlayers;
}

void MachGuiStartupData::numPlayers( const string& str )
{
	GuiResourceString two( IDS_MENU_NUMPLAYERS2 );
	GuiResourceString three( IDS_MENU_NUMPLAYERS3 );
	GuiResourceString four( IDS_MENU_NUMPLAYERS4 );

	if ( strcasecmp( two.asString().c_str(), str.c_str() ) == 0 )
	{
		numPlayers( 2 );
	}
	else if ( strcasecmp( three.asString().c_str(), str.c_str() ) == 0 )
	{
		numPlayers( 3 );
	}
	else if ( strcasecmp( four.asString().c_str(), str.c_str() ) == 0 )
	{
		numPlayers( 4 );
	}
 	else
	{
		ASSERT_INFO( str );
		ASSERT( false, "MachGuiStartupData::numPlayers passed invalid string" );
	}
}

int MachGuiStartupData::numPlayers() const
{
	return gameSettings_.numPlayers_;
}

string MachGuiStartupData::numPlayersStr() const
{
 	switch ( numPlayers() )
	{
   		case 2:
		{
			GuiResourceString resStr( IDS_MENU_NUMPLAYERS2 );
			return resStr.asString();
		}
		case 3:
		{
			GuiResourceString resStr( IDS_MENU_NUMPLAYERS3 );
			return resStr.asString();
		}
		case 4:
		{
			GuiResourceString resStr( IDS_MENU_NUMPLAYERS4 );
			return resStr.asString();
		}
		DEFAULT_ASSERT_BAD_CASE( numPlayers() );
	}

	return "bad";
}

///////////////////////////////////////////////////////////////////////////////////////////////

void MachGuiStartupData::techLevel( MachLog::TechnologyLevel tl )
{
	gameSettings()->techLevel_ = tl;
}

void MachGuiStartupData::techLevel( const string& str )
{
	GuiResourceString defaultStr( IDS_MENU_DEFAULT );
	GuiResourceString lowStr( IDS_MENU_TECHLEVELLOW );
	GuiResourceString mediumStr( IDS_MENU_TECHLEVELMED );
	GuiResourceString highStr( IDS_MENU_TECHLEVELHIGH );

	if ( strcasecmp( defaultStr.asString().c_str(), str.c_str() ) == 0 )
	{
		techLevel( MachLog::TECH_LEVEL_DEFAULT );
	}
	else if ( strcasecmp( lowStr.asString().c_str(), str.c_str() ) == 0 )
	{
		techLevel( MachLog::TECH_LEVEL_LOW );
	}
	else if ( strcasecmp( mediumStr.asString().c_str(), str.c_str() ) == 0 )
	{
		techLevel( MachLog::TECH_LEVEL_MEDIUM );
	}
	else if ( strcasecmp( highStr.asString().c_str(), str.c_str() ) == 0 )
	{
		techLevel( MachLog::TECH_LEVEL_HIGH );
	}
 	else
	{
		ASSERT_INFO( str );
		ASSERT( false, "MachGuiStartupData::techLevel passed invalid string" );
	}

}

MachLog::TechnologyLevel MachGuiStartupData::techLevel() const
{
	return gameSettings_.techLevel_;
}

string MachGuiStartupData::techLevelStr() const
{
	switch ( techLevel() )
	{
   		case MachLog::TECH_LEVEL_DEFAULT:
		{
			GuiResourceString resStr( IDS_MENU_DEFAULT );
			return resStr.asString();
		}
		case MachLog::TECH_LEVEL_LOW:
		{
			GuiResourceString resStr( IDS_MENU_TECHLEVELLOW );
			return resStr.asString();
		}
		case MachLog::TECH_LEVEL_MEDIUM:
		{
			GuiResourceString resStr( IDS_MENU_TECHLEVELMED );
			return resStr.asString();
		}
		case MachLog::TECH_LEVEL_HIGH:
		{
			GuiResourceString resStr( IDS_MENU_TECHLEVELHIGH );
			return resStr.asString();
		}
		DEFAULT_ASSERT_BAD_CASE( techLevel() );
	}

	return "bad";
}

///////////////////////////////////////////////////////////////////////////////////////////////

void MachGuiStartupData::playerRace( MachPhys::Race race )
{
	playerRace_ = race;

	// Store value in registry so it is the same the next time Machines is run
	SysRegistry::instance().setIntegerValue( "Game Settings\\Player Race", "Value", playerRace_, SysRegistry::CURRENT_USER );
}

void MachGuiStartupData::playerRace( const string& str )
{
	GuiResourceString red( IDS_MENU_PLAYERCOLOURRED );
	GuiResourceString green( IDS_MENU_PLAYERCOLOURGREEN );
	GuiResourceString blue( IDS_MENU_PLAYERCOLOURBLUE );
	GuiResourceString yellow( IDS_MENU_PLAYERCOLOURYELLOW );

	if ( strcasecmp( red.asString().c_str(), str.c_str() ) == 0 )
	{
		playerRace( MachPhys::RED );
	}
	else if ( strcasecmp( green.asString().c_str(), str.c_str() ) == 0 )
	{
		playerRace( MachPhys::GREEN );
	}
	else if ( strcasecmp( blue.asString().c_str(), str.c_str() ) == 0 )
	{
		playerRace( MachPhys::BLUE );
	}
	else if ( strcasecmp( yellow.asString().c_str(), str.c_str() ) == 0 )
	{
		playerRace( MachPhys::YELLOW );
	}
 	else
	{
		ASSERT_INFO( str );
		ASSERT( false, "MachGuiStartupData::playerRace passed invalid string" );
	}
}

MachPhys::Race MachGuiStartupData::playerRace() const
{
	return playerRace_;
}

string MachGuiStartupData::playerRaceStr() const
{
	switch ( playerRace() )
	{
   		case MachPhys::RED:
		{
			GuiResourceString resStr( IDS_MENU_PLAYERCOLOURRED );
			return resStr.asString();
		}
		case MachPhys::YELLOW:
		{
			GuiResourceString resStr( IDS_MENU_PLAYERCOLOURYELLOW );
			return resStr.asString();
		}
		case MachPhys::GREEN:
		{
			GuiResourceString resStr( IDS_MENU_PLAYERCOLOURGREEN );
			return resStr.asString();
		}
		case MachPhys::BLUE:
		{
			GuiResourceString resStr( IDS_MENU_PLAYERCOLOURBLUE );
			return resStr.asString();
		}
		DEFAULT_ASSERT_BAD_CASE( playerRace() );
	}

	return "bad";
}

int MachGuiStartupData::randomStartSeed() const
{
	return gameSettings_.randomStartsSeed_;
}

void MachGuiStartupData::receivedInGameChatMessage( const string& message, MachPhys::Race intendedRace )
{
	bool displayMessage = false;

	if ( intendedRace == MachPhys::N_RACES )
	{
		displayMessage = true;
	}
	else
	{
		for ( int i = 0; i < 4 and not displayMessage; ++i )
		{
			if ( players_[i].race_ == intendedRace and
				 players_[i].status_ == PlayerInfo::HUMAN and
				 players_[i].getDisplayName() == playerName() )
			{
				displayMessage = true;
			}
		}
	}

	if ( displayMessage )
		MachGuiInGameChatMessages::instance().addMessage( message );
}

bool MachGuiStartupData::doesAtLeastOnePlayerHaveMachinesCD() const
{
	PRE( isHost() );

	bool retVal = false;

	// Check to see if the host has a machines CD
	if ( MachGui::machinesCDIsAvailable( 1 ) or MachGui::machinesCDIsAvailable( 2 ) )
	{
		retVal = true;
	}

	// Check to see if other players have machines CD
	for ( size_t loop = 0; loop < 4 and not retVal; ++loop )
	{
		if ( players_[loop].status_ == PlayerInfo::HUMAN )
		{
	    	retVal = players_[loop].hasMachinesCD_;
		}
	}

	return retVal;
}

void MachGuiStartupData::receivedHasMachinesCDMessage( const string& playerName, bool hasMachinesCD )
{
	if ( isHost() )
	{
		// Update player info to indicate who has a machines CD
		for ( size_t loop = 0; loop < 4; ++loop )
		{
			if ( players_[loop].playerName_ == playerName )
			{
		    	players_[loop].hasMachinesCD_ = hasMachinesCD;
			}
		}

		sendUpdatePlayersMessage();
	}
}

ostream& operator <<( ostream& o, const MachGuiStartupData::PlayerInfo& t )
{
	o << "getDisplayName " << t.getDisplayName() << std::endl;
	o << "Race           " << t.race_ << std::endl;
	o << "status_        " << t.status_ << std::endl;
	o << "ready_         " << t.ready_ << std::endl;
	o << "host_          " << t.host_ << std::endl;
	return o;
}
ostream& operator <<( ostream& o, const MachGuiStartupData::GameSettings& t )
{
	o << "mapSizeId_ " << t.mapSizeId_ << std::endl;
	o << "terrainTypeId_ " << t.terrainTypeId_ << std::endl;
	o << "scenarioId_ " << t.scenarioId_ << std::endl;
	o << "resources_ " << t.resources_ << std::endl;
	o << "fogOfWar_ " << t.fogOfWar_  << std::endl;
	o << "startingResources_ " << t.startingResources_ << std::endl;
	o << "startingPosition_ " << t.startingPosition_ << std::endl;
	o << "victoryCondition_ " << t.victoryCondition_ << std::endl;
	o << "numPlayers_ " << t.numPlayers_ << std::endl;
	o << "playerRace_ " << t.playerRace_ << std::endl;
	o << "techLevel_ " << t.techLevel_ << std::endl;
	o << "randomStartsSeed_ " << t.randomStartsSeed_ << std::endl;
	o << "broadcastAlliances_ " << t.broadcastAlliances_ << std::endl;
	o << "disableFirstPerson_ " << t.disableFirstPerson_ << std::endl;
	return o;
}

ostream& operator <<( ostream& o, const MachGuiStartupData::PlayerInfo::Status& t )
{
	switch( t )
	{
		case MachGuiStartupData::PlayerInfo::COMPUTER: o << "COMPUTER"; break;
		case MachGuiStartupData::PlayerInfo::OPEN: o << "OPEN"; break;
		case MachGuiStartupData::PlayerInfo::CLOSED: o << "CLOSED"; break;
		case MachGuiStartupData::PlayerInfo::HUMAN: o << "HUMAN"; break;
		case MachGuiStartupData::PlayerInfo::UNAVAILABLE: o << "UNAVAILABLE"; break;
		default: o << "Unknown status in MachGuiStartupData::PlayerInfo::Status " << std::endl;
	}
	return o;
}

///////////////////////////////////////////////////////////////////////////////////////////////

void MachGuiStartupData::broadcastAlliances( bool newValue )
{
	gameSettings()->broadcastAlliances_ = newValue;
}

void MachGuiStartupData::broadcastAlliances( const string& str )
{
	GuiResourceString onStr( IDS_MENU_ON );
	GuiResourceString offStr( IDS_MENU_OFF );

	if ( strcasecmp( onStr.asString().c_str(), str.c_str() ) == 0 )
	{
		broadcastAlliances( true );
	}
	else if ( strcasecmp( offStr.asString().c_str(), str.c_str() ) == 0 )
	{
		broadcastAlliances( false );
	}
	else
	{
		ASSERT_INFO( str );
		ASSERT( false, "MachGuiStartupData::broadcastAlliances passed invalid string" );
	}
}

bool MachGuiStartupData::broadcastAlliances()	const
{
	return gameSettings_.broadcastAlliances_;
}

string MachGuiStartupData::broadcastAlliancesStr() const
{
	GuiResourceString onStr( IDS_MENU_ON );
	GuiResourceString offStr( IDS_MENU_OFF );

	if ( broadcastAlliances()	)
	{
		return onStr.asString();
	}

	return offStr.asString();
}

///////////////////////////////////////////////////////////////////////////////////////////////

void MachGuiStartupData::disableFirstPerson( bool newValue )
{
	gameSettings()->disableFirstPerson_ = newValue;
}

void MachGuiStartupData::disableFirstPerson( const string& str )
{
	GuiResourceString onStr( IDS_MENU_ON );
	GuiResourceString offStr( IDS_MENU_OFF );

	if ( strcasecmp( onStr.asString().c_str(), str.c_str() ) == 0 )
	{
		disableFirstPerson( true );
	}
	else if ( strcasecmp( offStr.asString().c_str(), str.c_str() ) == 0 )
	{
		disableFirstPerson( false );
	}
	else
	{
		ASSERT_INFO( str );
		ASSERT( false, "MachGuiStartupData::disableFirstPerson passed invalid string" );
	}
}

bool MachGuiStartupData::disableFirstPerson()	const
{
	return gameSettings_.disableFirstPerson_;
}

string MachGuiStartupData::disableFirstPersonStr() const
{
	GuiResourceString onStr( IDS_MENU_ON );
	GuiResourceString offStr( IDS_MENU_OFF );

	if ( disableFirstPerson()	)
	{
		return onStr.asString();
	}

	return offStr.asString();
}

void MachGuiStartupData::receivedNameChangeMessage( const string& newPlayerName, int uniqueMachineNum )
{
	if ( uniqueMachineNum == uniqueMachineNumber() )
	{
		GuiStrings names;
		names.reserve( 2 );
		names.push_back( newPlayerName );
		names.push_back( playerName() );

		// Change players name
		playerName( newPlayerName );

		pStartupScreens_->displayMsgBox( IDS_MENUMSG_NAMECHANGE, names );
	}
}

int MachGuiStartupData::uniqueMachineNumber() const
{
	return uniqueMachineNumber_;
}

///////////////////////////////////////////////////////////////////////////////////////////////

/* End STRTDATA.CPP *************************************************/
