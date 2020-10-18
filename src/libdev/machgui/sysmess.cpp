/*
 * S Y S M E S S . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#if defined PRODUCTION && defined DEMO
#undef PRODUCTION
#endif

#include "ctl/list.hpp"

#include "mathex/point2d.hpp"

#include "sim/process.hpp"
#include "sim/disevent.hpp"
#include "sim/manager.hpp"

#include "gui/restring.hpp"

#include "machlog/network.hpp"
#include "machlog/races.hpp"
#include "machlog/race.hpp"
#include "machlog/cntrl.hpp"
#include "machlog/actor.hpp"

#include "machgui/internal/strings.hpp"
#include "machgui/sysmess.hpp"
#include "machgui/startup.hpp"
#include "machgui/strtdata.hpp"
#include "machgui/chatmsgs.hpp"

unsigned char pPromptTextKey5[] = { 180, 136, 148, 74, 77, 198, 4, 194, 208, 181, 12, 106, 220, 202, 95, 245, 222, 0 };
unsigned char pPromptText6[] = { 173, 46, 120, 216, 105, 232, 200, 189, 72, 13, 147, 196, 148, 3, 5, 145, 206, 192, 84, 168, 97, 38, 233, 83, 193, 218, 102, 201, 77, 223, 217, 110, 78, 42, 142, 73, 132, 86, 0 };
unsigned char pPromptTextKey6[] = { 195, 91, 27, 180, 12, 137, 186, 157, 63, 104, 242, 180, 251, 109, 118, 177, 173, 161, 58, 136, 5, 71, 132, 50, 166, 191, 70, 176, 34, 170, 171, 78, 38, 79, 239, 37, 240, 62, 0 };
unsigned char pPromptText9[] = {  161, 50, 124, 148, 107, 252, 212, 238, 31, 9, 128, 209, 219, 11, 3, 223, 0 };
unsigned char pPromptTextKey9[] = {  195, 91, 27, 180, 12, 137, 186, 157, 63, 104, 242, 180, 251, 109, 118, 177, 0 };

class MachGuiSessionLostGameTerminatorEvent : public SimDiscreteEvent
{
public:
	MachGuiSessionLostGameTerminatorEvent(  MachGuiStartupScreens* pStartupScreens , const PhysAbsoluteTime& time )
	:   SimDiscreteEvent( time ),
		pStartupScreens_( pStartupScreens )
	{};
    virtual void execute()
    {
		NETWORK_STREAM("MachGuiSessionLostGameTerminatorEvent::execute\n" );

		//if we got into this screen via a lobby session then we need to terminate correctly at this point.
		NETWORK_STREAM("MachGuiSessionLostGameTerminatorEvent::execute terminate and reset\n" );
		MachLogNetwork::instance().terminateAndReset();
		NETWORK_STREAM("MachGuiSessionLostGameTerminatorEvent::execute contextFinish\n" );
// Changed
        pStartupScreens_->deleteAllChildren();
//    	pStartupScreens_->contextFinishFromLobby();
//		NETWORK_STREAM("MachGuiSessionLostGameTerminatorEvent::execute DONE\n" );
    };
private:
	MachGuiStartupScreens* pStartupScreens_;
};

MachGuiSystemMessageHandler::MachGuiSystemMessageHandler( MachGuiStartupScreens* pStartupScreens )
:	pStartupScreens_( pStartupScreens )
{

    TEST_INVARIANT;
}

MachGuiSystemMessageHandler::~MachGuiSystemMessageHandler()
{
    TEST_INVARIANT;

}

void MachGuiSystemMessageHandler::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiSystemMessageHandler& t )
{

    o << "MachGuiSystemMessageHandler " << (void*)&t << " start" << std::endl;
    o << "MachGuiSystemMessageHandler " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool	MachGuiSystemMessageHandler::handleHostMessage()
{
	NETWORK_STREAM("MachGuiSystemMessageHandler::handleHostMessage\n" );
	if( respondAsInGame() )
	{
		NETWORK_STREAM("MachGuiStartupScreens::CTX_MULTI_GAME\n" );
        MachLogNetwork::instance().isNodeLogicalHost( true );
		MachLogRaces& races = MachLogRaces::instance();
		for( MachPhys::Race i = MachPhys::RED; i < MachPhys::N_RACES; ++((int&)i) )
		{
			if( races.raceInGame( i ) and races.controller( i ).type() == MachLogController::AI_CONTROLLER )
			{
				MexPoint2d pos;
				if( races.aggressorAssemblyPoints( i ).size() == 0 or
				    races.aggressorAssemblyPoints( i ).size() == 0 )
				{
					if( races.raceObjects( i ).size() > 0 )
						pos = races.raceObjects( i ).front()->position();
					else
						pos = MexPoint2d( 400, 400 );

				}
				races.addAggressorAssemblyPoint( i, pos );
				races.addAdministratorAssemblyPoint( i, pos );
				races.race( i ).toBeUpdated( SimProcess::MANAGER_UPDATE );
				MachLogNetwork::instance().remoteStatus( i, MachLogNetwork::LOCAL_PROCESS );
			}
		}
        GuiResourceString errorString( IDS_MULTI_ERROR_HOST_CONNECTION_LOST, pStartupScreens_->startupData()->playerName() );
		MachGuiInGameChatMessages::instance().addMessage( errorString.asString() );
		return true;
//		return MachLogSystemMessageHandler::handleHostMessage();
	}
	NETWORK_STREAM(" calling startupscreens handle host\n" );
	return pStartupScreens_->handleHostMessage();
}

//virtual
bool	MachGuiSystemMessageHandler::handleDestroyPlayerMessage( const string& name )
{
	NETWORK_STREAM("MachGuiSystemMessageHandler::handleDestroyPlayerMessage " << name << std::endl );
	NETWORK_INDENT( 2 );
	if( respondAsInGame() )
	{
		NETWORK_STREAM("MachGuiStartupScreens::CTX_MULTI_GAME\n" );
		MachPhys::Race r = MachPhys::N_RACES;
		bool foundPlayer = false;
		for( int i = 0; i < 4; ++i )
		{
			const string& playerName = pStartupScreens_->startupData()->players()[i].getDisplayName();
			NETWORK_STREAM("Checking against " << playerName << std::endl );
			if( playerName == name and pStartupScreens_->startupData()->players()[i].status_ == MachGuiStartupData::PlayerInfo::HUMAN )
			{
				foundPlayer = true;
				NETWORK_STREAM("found a match with.\n" );

				if( pStartupScreens_->startupData()->players()[i].host_ )
				{
                    pStartupScreens_->startupData()->players()[i].host_ = false;
//					//determine which player is next in list of races to become the host.
//					int lowest = 4;
//					for( int j = 3; j >= 0; --j )
//						if( pStartupScreens_->startupData()->players()[i].status_ == MachGuiStartupData::PlayerInfo::HUMAN and i != j )
//							lowest = j;
//					if( lowest == (int)pStartupScreens_->startupData()->gameSettings()->playerRace_ )
//						MachLogNetwork::instance().isNodeLogicalHost( true );
//					GuiResourceString errorString( IDS_MULTI_ERROR_HOST_CONNECTION_LOST, pStartupScreens_->startupData()->players()[i].getDisplayName() );

				}

				r = pStartupScreens_->startupData()->players()[i].race_;
				pStartupScreens_->startupData()->players()[i].status_ = MachGuiStartupData::PlayerInfo::CONNECTION_LOST;
				GuiResourceString errorString( IDS_MULTI_ERROR_PLAYER_CONNECTION_LOST, name );
				MachGuiInGameChatMessages::instance().addMessage( errorString.asString() );
				logicalHandleDestroyPlayerMessage( r );
			}
		}
		return true;
	}

	NETWORK_STREAM(" calling startupscreens handle destroy\n" );
	return pStartupScreens_->handleDestroyPlayerMessage( name );
}

//virtual
bool	MachGuiSystemMessageHandler::handleSessionLostMessage()
{
    NETWORK_STREAM("MachGuiSystemMessageHandler::handleSessionLostMessage\n" );
	if( respondAsInGame() )
	{
		GuiResourceString errorString( IDS_MULTI_ERROR_SESSION_LOST_INGAME );
		MachGuiInGameChatMessages::instance().addMessage( errorString.asString() );
		MachGuiSessionLostGameTerminatorEvent* pEvent = _NEW( MachGuiSessionLostGameTerminatorEvent( pStartupScreens_, SimManager::instance().currentTime() + 5 ) );
		SimManager::instance().add( pEvent );
		return true;
	}
	return pStartupScreens_->handleSessionLostMessage();
}

bool MachGuiSystemMessageHandler::respondAsInGame() const
{
	bool result = false;
	MachGuiStartupScreens::Context context = pStartupScreens_->currentContext();
	switch( context )
	{
		case MachGuiStartupScreens::CTX_MULTI_GAME:
		case MachGuiStartupScreens::CTX_INGAMEOP:
		case MachGuiStartupScreens::CTX_IGOPTIONS:
		case MachGuiStartupScreens::CTX_HOTKEYS:
		case MachGuiStartupScreens::CTX_MPINGAMEOPTIONS:
			result = true;
			break;
		default:
			result = false;
	}
	return result;
}

/* End SYSMESS.CPP **************************************************/
