/*
 * M E S S B R O K . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <stdlib.h>
#include "gui/restring.hpp"
#include "network/message.hpp"
#include "machgui/messbrok.hpp"
#include "machgui/dispmess.hpp"
#include "machgui/chatwind.hpp"
#include "machgui/strtdata.hpp"
#include "machgui/chatmsgs.hpp"
#include "machgui/internal/strings.hpp"
#include "machgui/internal/mgsndman.hpp"
#include "network/node.hpp"
#include "system/pathname.hpp"

#pragma pack(push,1)
struct MachGuiChatMessage
{
	MachLogMessageHeader header_;
	char 				 chat_[MAX_CHATMESSAGE_LEN + 1];
};

struct MachGuiJoinMessage
{
	MachLogMessageHeader header_;
	char 				 playerName_[MAX_PLAYERNAME_LEN + 1];
	int					 uniqueMachineNumber_;
};

struct MachGuiUpdatePlayersMessage
{
	MachLogMessageHeader header_;
	MachGuiStartupData::PlayerInfo playerInfo_[4];
};

struct MachGuiUpdateGameSettingsMessage
{
	MachLogMessageHeader header_;
	MachGuiStartupData::GameSettings gameSettings_;
};

struct MachGuiRaceChangeRequestMessage
{
	MachLogMessageHeader header_;
	char 				 playerName_[MAX_PLAYERNAME_LEN + 1];
	MachPhys::Race 		 race_;
	int				     index_;
};

struct MachGuiImReadyMessage
{
	MachLogMessageHeader header_;
	char 				 playerName_[MAX_PLAYERNAME_LEN + 1];
	bool				 ready_;
};

struct MachGuiStartMessage
{
	MachLogMessageHeader header_;
};

struct MachGuiHostCancelMessage
{
	MachLogMessageHeader header_;
};

struct MachGuiClientCancelMessage
{
	MachLogMessageHeader header_;
	char 				 playerName_[MAX_PLAYERNAME_LEN + 1];
};

struct MachGuiInGameChatMessage
{
	MachLogMessageHeader header_;
	char 				 chatMessage_[MAX_INGAMECHATMESSAGE_LEN + 1];
	MachPhys::Race 		 race_; // Who the message is intended for
};

struct MachGuiHasMachinesCDMessage
{
	MachLogMessageHeader header_;
	char 				 playerName_[MAX_PLAYERNAME_LEN + 1];
	bool				 hasMachinesCD_;
};

struct MachGuiIveLostMessage
{
	MachLogMessageHeader header_;
	char 				 playerName_[MAX_PLAYERNAME_LEN + 1];
};

struct MachGuiNameChangeMessage
{
	MachLogMessageHeader header_;
	char 				 playerName_[MAX_PLAYERNAME_LEN + 1];
	int					 uniqueMachineNumber_;
};
#pragma pack(pop)

MachGuiMessageBroker::MachGuiMessageBroker( MachGuiStartupData* pStartupData )
:	pStartupData_( pStartupData )
{

    TEST_INVARIANT;
}

//virtual
MachGuiMessageBroker::~MachGuiMessageBroker()
{
    TEST_INVARIANT;

}

void MachGuiMessageBroker::processChatMessage( MachGuiChatMessage* pMessage )
{
	// Play sound
	MachGuiSoundManager::instance().playSound( "gui/sounds/chatmsg.wav" );

	MachGuiChatWindow::addText( pMessage->chat_ );
}

void MachGuiMessageBroker::processJoinMessage( MachGuiJoinMessage* pMessage )
{
	PRE( pStartupData_ );

	pStartupData_->receivedJoinMessage( pMessage->playerName_, pMessage->uniqueMachineNumber_ );
}


void MachGuiMessageBroker::processUpdatePlayersMessage( MachGuiUpdatePlayersMessage* pMessage )
{
	NETWORK_STREAM("MachGuiMessageBroker::processUpdatePlayersMessage\n" );
	NETWORK_INDENT( 2 );
	PRE( pStartupData_ );

	// Copy new information into players info structure
	memcpy( pStartupData_->players(), pMessage->playerInfo_, sizeof(MachGuiStartupData::PlayerInfo) * 4 );

	// Refresh any gui components...
	pStartupData_->receivedUpdatePlayersMessage();
	NETWORK_INDENT( -2 );
	NETWORK_STREAM("MachGuiMessageBroker::processUpdatePlayersMessage DONE\n" );
}

void MachGuiMessageBroker::processUpdateGameSettingsMessage( MachGuiUpdateGameSettingsMessage* pMessage )
{
	PRE( pStartupData_ );

	// Copy new information into game settings structure
	memcpy( pStartupData_->gameSettings(), &pMessage->gameSettings_, sizeof(MachGuiStartupData::GameSettings) );

	// Refresh any gui components...
	pStartupData_->receivedUpdateGameSettingsMessage();
}

void MachGuiMessageBroker::processRaceChangeRequest( MachGuiRaceChangeRequestMessage* pMessage )
{
	pStartupData_->receivedRaceChangeRequest( pMessage->playerName_, pMessage->index_, pMessage->race_ );
}

void MachGuiMessageBroker::processImReadyMessage( MachGuiImReadyMessage* pMessage )
{
	PRE( pStartupData_ );

	pStartupData_->receivedImReadyMessage( pMessage->playerName_, pMessage->ready_ );
}

void MachGuiMessageBroker::processStartMessage( MachGuiStartMessage* /*pMessage*/ )
{
	pStartupData_->receivedStartMessage();
}

void MachGuiMessageBroker::processHostCancelMessage( MachGuiHostCancelMessage* /*pMessage*/ )
{
	pStartupData_->receivedHostCancelMessage();
}

void MachGuiMessageBroker::processClientCancelMessage( MachGuiClientCancelMessage* pMessage )
{
	pStartupData_->receivedClientCancelMessage( pMessage->playerName_ );
}

void MachGuiMessageBroker::processInGameChatMessage( MachGuiInGameChatMessage* pMessage )
{
	pStartupData_->receivedInGameChatMessage( pMessage->chatMessage_, pMessage->race_ );
}

void MachGuiMessageBroker::processIveLostMessage( MachGuiIveLostMessage* pMessage )
{
	GuiResourceString lostString( IDS_PLAYER_HAS_LOST, pMessage->playerName_ );
	MachGuiInGameChatMessages::instance().addMessage( lostString.asString() );
}

void MachGuiMessageBroker::processMachGuiMessage( NetMessage* pMessage )
{
	PRE( getSystemType( pMessage ) == SYSTEM_MACHGUI_MESSAGE );

	DEBUG_STREAM( DIAG_NETWORK, "processMachGuiMessage enter messageType " << getMessageType( pMessage ) << std::endl );

	switch( getMessageType( pMessage ) )
	{
		case MT_CHATMESSAGE:
			//processChatMessage( _REINTERPRET_CAST( MachGuiChatMessage*, pMessage->body().body() ) );
			processChatMessage( _REINTERPRET_CAST( MachGuiChatMessage*, _CONST_CAST(uint8 *, pMessage->body().body()) ) );
			break;
		case MT_JOINMESSAGE:
			//processJoinMessage( _REINTERPRET_CAST( MachGuiJoinMessage*, pMessage->body().body() ) );
			processJoinMessage( _REINTERPRET_CAST( MachGuiJoinMessage*, _CONST_CAST(uint8 *, pMessage->body().body()) ) );
			break;
		case MT_UPDATEPLAYERSMESSAGE:
			//processUpdatePlayersMessage( _REINTERPRET_CAST( MachGuiUpdatePlayersMessage*, pMessage->body().body() ) );
			processUpdatePlayersMessage( _REINTERPRET_CAST( MachGuiUpdatePlayersMessage*, _CONST_CAST(uint8 *, pMessage->body().body()) ) );
			break;
		case MT_RACECHANGEREQUESTMESSAGE:
			//processRaceChangeRequest( _REINTERPRET_CAST( MachGuiRaceChangeRequestMessage*, pMessage->body().body() ) );
			processRaceChangeRequest( _REINTERPRET_CAST( MachGuiRaceChangeRequestMessage*, _CONST_CAST(uint8 *, pMessage->body().body()) ) );
			break;
		case MT_STARTMESSAGE:
			//processStartMessage( _REINTERPRET_CAST( MachGuiStartMessage*, pMessage->body().body() ) );
			processStartMessage( _REINTERPRET_CAST( MachGuiStartMessage*, _CONST_CAST(uint8 *, pMessage->body().body()) ) );
			break;
		case MT_IMREADYMESSAGE:
			//processImReadyMessage( _REINTERPRET_CAST( MachGuiImReadyMessage*, pMessage->body().body() ) );
			processImReadyMessage( _REINTERPRET_CAST( MachGuiImReadyMessage*, _CONST_CAST(uint8 *, pMessage->body().body()) ) );
			break;
		case MT_HOSTCANCELMESSAGE:
			//processHostCancelMessage( _REINTERPRET_CAST( MachGuiHostCancelMessage*, pMessage->body().body() ) );
			processHostCancelMessage( _REINTERPRET_CAST( MachGuiHostCancelMessage*, _CONST_CAST(uint8 *, pMessage->body().body()) ) );
			break;
		case MT_CLIENTCANCELMESSAGE:
			//processClientCancelMessage( _REINTERPRET_CAST( MachGuiClientCancelMessage*, pMessage->body().body() )  );
			processClientCancelMessage( _REINTERPRET_CAST( MachGuiClientCancelMessage*, _CONST_CAST(uint8 *, pMessage->body().body()) )  );
			break;
		case MY_UPDATEGAMESETTINGSMESSAGE:
			//processUpdateGameSettingsMessage( _REINTERPRET_CAST( MachGuiUpdateGameSettingsMessage*, pMessage->body().body() )  );
			processUpdateGameSettingsMessage( _REINTERPRET_CAST( MachGuiUpdateGameSettingsMessage*, _CONST_CAST(uint8 *, pMessage->body().body()) )  );
			break;
		case MT_INGAMECHATMESSAGE:
			//processInGameChatMessage( _REINTERPRET_CAST( MachGuiInGameChatMessage*, pMessage->body().body() )  );
			processInGameChatMessage( _REINTERPRET_CAST( MachGuiInGameChatMessage*, _CONST_CAST(uint8 *, pMessage->body().body()) )  );
			break;
		case MT_HASMACHINESCDMESSAGE:
			//processHasMachinesCDMessage( _REINTERPRET_CAST( MachGuiHasMachinesCDMessage*, pMessage->body().body() )  );
			processHasMachinesCDMessage( _REINTERPRET_CAST( MachGuiHasMachinesCDMessage*, _CONST_CAST(uint8 *, pMessage->body().body()) )  );
			break;
		case MT_IVELOSTMESSAGE:
			//processIveLostMessage( _REINTERPRET_CAST( MachGuiIveLostMessage*, pMessage->body().body() )  );
			processIveLostMessage( _REINTERPRET_CAST( MachGuiIveLostMessage*, _CONST_CAST(uint8 *, pMessage->body().body()) )  );
			break;
		case MT_NAMECHANGEMESSAGE:
			//processNameChangeMessage( _REINTERPRET_CAST( MachGuiNameChangeMessage*, pMessage->body().body() ) );
			processNameChangeMessage( _REINTERPRET_CAST( MachGuiNameChangeMessage*, _CONST_CAST(uint8 *, pMessage->body().body()) ) );
			break;
		default:
			ASSERT_BAD_CASE;
	}

	DEBUG_STREAM( DIAG_NETWORK, "processMachGuiMessage done" << std::endl );
}

//virtual
void MachGuiMessageBroker::processMessage( NetMessage* pMessage )
{
	switch( getSystemType( pMessage ) )
	{
	case SYSTEM_MACHLOG_MESSAGE:
		MachLogMessageBroker::processMessage( pMessage );
		break;
	case SYSTEM_MACHGUI_MESSAGE:
		processMachGuiMessage( pMessage );
		_DELETE( pMessage );
		break;
	default:
		NETWORK_STREAM("Invalid message code detected in MGMessageBroker::processMessage\n" );
		NETWORK_STREAM(" message is :\n" << *pMessage << std::endl );
		ASSERT_BAD_CASE;
	}
}

MachGuiMessageBroker::SystemType MachGuiMessageBroker::getSystemType( NetMessage* pMessage )
{
	return (SystemType)pMessage->body().body()[0];
}

MachGuiMessageBroker::MessageType MachGuiMessageBroker::getMessageType( NetMessage* pMessage )
{
//	return (MessageType)pMessage->body().body()[1];
	return (MessageType) ((MachLogMessageHeader*)pMessage->body().body())->messageCode_;
}

void MachGuiMessageBroker::sendChatMessage( const string& chat )
{
	MachLogNetMessage* pLogMessage = _NEW( MachLogNetMessage() );
	MachGuiChatMessage* pMessage = _REINTERPRET_CAST( MachGuiChatMessage*, pLogMessage );
	pMessage->header_.systemCode_ = SYSTEM_MACHGUI_MESSAGE;
	pMessage->header_.messageCode_ = MT_CHATMESSAGE;
	pMessage->header_.totalLength_ = sizeof( MachGuiChatMessage );

	ASSERT( strlen( chat.c_str() ) <= MAX_CHATMESSAGE_LEN, "chat message too long" );

	strcpy( pMessage->chat_, chat.c_str() );

	doSend( pLogMessage );
}

void MachGuiMessageBroker::sendJoinMessage( const string& playerName, int uniqueMachineNumber )
{
	NETWORK_STREAM("MachGuiMessageBroker::sendJoinMessage\n" );
	NETWORK_INDENT( 2 );
	MachLogNetMessage* pLogMessage = _NEW( MachLogNetMessage() );
	MachGuiJoinMessage* pMessage = _REINTERPRET_CAST( MachGuiJoinMessage*, pLogMessage );
	pMessage->header_.systemCode_ = SYSTEM_MACHGUI_MESSAGE;
	pMessage->header_.messageCode_ = MT_JOINMESSAGE;
	pMessage->header_.totalLength_ = sizeof( MachGuiJoinMessage );

	ASSERT( strlen( playerName.c_str() ) <= MAX_PLAYERNAME_LEN, "player name too long" );

	strcpy( pMessage->playerName_, playerName.c_str() );

	pMessage->uniqueMachineNumber_ = uniqueMachineNumber;

	doSend( pLogMessage );
	NETWORK_INDENT( -2 );
	NETWORK_STREAM("MachGuiMessageBroker::sendJoinMessage DONE\n" );
}

void MachGuiMessageBroker::sendUpdatePlayersMessage()
{
	MachLogNetMessage* pLogMessage = _NEW( MachLogNetMessage() );
	MachGuiUpdatePlayersMessage* pMessage = _REINTERPRET_CAST( MachGuiUpdatePlayersMessage*, pLogMessage );
	pMessage->header_.systemCode_ = SYSTEM_MACHGUI_MESSAGE;
	pMessage->header_.messageCode_ = MT_UPDATEPLAYERSMESSAGE;
	pMessage->header_.totalLength_ = sizeof( MachGuiUpdatePlayersMessage );

	memcpy( pMessage->playerInfo_, pStartupData_->players(), sizeof(MachGuiStartupData::PlayerInfo) * 4 );

	doSend( pLogMessage );
}

void MachGuiMessageBroker::sendUpdateGameSettingsMessage()
{
	MachLogNetMessage* pLogMessage = _NEW( MachLogNetMessage() );
	MachGuiUpdateGameSettingsMessage* pMessage = _REINTERPRET_CAST( MachGuiUpdateGameSettingsMessage*, pLogMessage );
	pMessage->header_.systemCode_ = SYSTEM_MACHGUI_MESSAGE;
	pMessage->header_.messageCode_ = MY_UPDATEGAMESETTINGSMESSAGE;
	pMessage->header_.totalLength_ = sizeof( MachGuiUpdateGameSettingsMessage );

	memcpy( &pMessage->gameSettings_, pStartupData_->gameSettings(), sizeof(MachGuiStartupData::GameSettings) );

	doSend( pLogMessage );
}

void MachGuiMessageBroker::sendRaceChangeRequest( const string& playerName, size_t playerIndex, MachPhys::Race newRace )
{
	MachLogNetMessage* pLogMessage = _NEW( MachLogNetMessage() );
	MachGuiRaceChangeRequestMessage* pMessage = _REINTERPRET_CAST( MachGuiRaceChangeRequestMessage*, pLogMessage );
	pMessage->header_.systemCode_ = SYSTEM_MACHGUI_MESSAGE;
	pMessage->header_.messageCode_ = MT_RACECHANGEREQUESTMESSAGE;
	pMessage->header_.totalLength_ = sizeof( MachGuiRaceChangeRequestMessage );

	ASSERT( strlen( playerName.c_str() ) <= MAX_PLAYERNAME_LEN, "player name too long" );

	strcpy( pMessage->playerName_, playerName.c_str() );

	pMessage->race_ = newRace;
	pMessage->index_ = playerIndex;

	doSend( pLogMessage );
}

void MachGuiMessageBroker::sendImReadyMessage( const string& playerName, bool ready )
{
	MachLogNetMessage* pLogMessage = _NEW( MachLogNetMessage() );
	MachGuiImReadyMessage* pMessage = _REINTERPRET_CAST( MachGuiImReadyMessage*, pLogMessage );
	pMessage->header_.systemCode_ = SYSTEM_MACHGUI_MESSAGE;
	pMessage->header_.messageCode_ = MT_IMREADYMESSAGE;
	pMessage->header_.totalLength_ = sizeof( MachGuiImReadyMessage );

	ASSERT( strlen( playerName.c_str() ) <= MAX_PLAYERNAME_LEN, "player name too long" );

	strcpy( pMessage->playerName_, playerName.c_str() );

	pMessage->ready_ = ready;

	doSend( pLogMessage );
}

void MachGuiMessageBroker::sendStartMessage()
{
	MachLogNetMessage* pLogMessage = _NEW( MachLogNetMessage() );
	MachGuiStartMessage* pMessage = _REINTERPRET_CAST( MachGuiStartMessage*, pLogMessage );
	pMessage->header_.systemCode_ = SYSTEM_MACHGUI_MESSAGE;
	pMessage->header_.messageCode_ = MT_STARTMESSAGE;
	pMessage->header_.totalLength_ = sizeof( MachGuiStartMessage );

	doSend( pLogMessage );
}

void MachGuiMessageBroker::sendHostCancelMessage()
{
	MachLogNetMessage* pLogMessage = _NEW( MachLogNetMessage() );
	MachGuiHostCancelMessage* pMessage = _REINTERPRET_CAST( MachGuiHostCancelMessage*, pLogMessage );
	pMessage->header_.systemCode_ = SYSTEM_MACHGUI_MESSAGE;
	pMessage->header_.messageCode_ = MT_HOSTCANCELMESSAGE;
	pMessage->header_.totalLength_ = sizeof( MachGuiHostCancelMessage );

	doSend( pLogMessage );
}

void MachGuiMessageBroker::sendClientCancelMessage( const string& playerName )
{
	MachLogNetMessage* pLogMessage = _NEW( MachLogNetMessage() );
	MachGuiClientCancelMessage* pMessage = _REINTERPRET_CAST( MachGuiClientCancelMessage*, pLogMessage );
	pMessage->header_.systemCode_ = SYSTEM_MACHGUI_MESSAGE;
	pMessage->header_.messageCode_ = MT_CLIENTCANCELMESSAGE;
	pMessage->header_.totalLength_ = sizeof( MachGuiClientCancelMessage );

	ASSERT( strlen( playerName.c_str() ) <= MAX_PLAYERNAME_LEN, "player name too long" );

	strcpy( pMessage->playerName_, playerName.c_str() );

	doSend( pLogMessage );
}

void MachGuiMessageBroker::sendInGameChatMessage( const string& message, MachPhys::Race intendedForRace )
{
	MachLogNetMessage* pLogMessage = _NEW( MachLogNetMessage() );
	MachGuiInGameChatMessage* pMessage = _REINTERPRET_CAST( MachGuiInGameChatMessage*, pLogMessage );
	pMessage->header_.systemCode_ = SYSTEM_MACHGUI_MESSAGE;
	pMessage->header_.messageCode_ = MT_INGAMECHATMESSAGE;
	pMessage->header_.totalLength_ = sizeof( MachGuiInGameChatMessage );
	pMessage->race_ = intendedForRace;

	ASSERT( strlen( message.c_str() ) <= MAX_INGAMECHATMESSAGE_LEN, "chat message too long" );

	strcpy( pMessage->chatMessage_, message.c_str() );

	doSend( pLogMessage );
}

void MachGuiMessageBroker::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiMessageBroker& t )
{

    o << "MachGuiMessageBroker " << (void*)&t << " start" << std::endl;
    o << "MachGuiMessageBroker " << (void*)&t << " end" << std::endl;

    return o;
}

ostream& operator <<( ostream& o, const MachGuiMessageBroker::MessageType& t )
{
	switch( t )
	{
		case MachGuiMessageBroker::MT_CHATMESSAGE: o << "MT_CHATMESSAGE"; break;
		case MachGuiMessageBroker::MT_JOINMESSAGE: o << "MT_JOINMESSAGE"; break;
		case MachGuiMessageBroker::MT_UPDATEPLAYERSMESSAGE: o << "MT_UPDATEPLAYERSMESSAGE"; break;
		case MachGuiMessageBroker::MT_RACECHANGEREQUESTMESSAGE: o << "MT_RACECHANGEREQUESTMESSAGE"; break;
		case MachGuiMessageBroker::MT_IMREADYMESSAGE: o << "MT_IMREADYMESSAGE"; break;
		case MachGuiMessageBroker::MT_STARTMESSAGE: o << "MT_STARTMESSAGE"; break;
		case MachGuiMessageBroker::MT_HOSTCANCELMESSAGE: o << "MT_HOSTCANCELMESSAGE"; break;
		case MachGuiMessageBroker::MT_CLIENTCANCELMESSAGE: o << "MT_CLIENTCANCELMESSAGE"; break;
		case MachGuiMessageBroker::MY_UPDATEGAMESETTINGSMESSAGE: o << "MY_UPDATEGAMESETTINGSMESSAGE"; break;
		case MachGuiMessageBroker::MT_INGAMECHATMESSAGE: o << "MT_INGAMECHATMESSAGE"; break;
		case MachGuiMessageBroker::MT_HASMACHINESCDMESSAGE: o << "MT_HASMACHINESCDMESSAGE"; break;
		case MachGuiMessageBroker::MT_IVELOSTMESSAGE: o << "MT_IVELOSTMESSAGE"; break;
		default:
			o << "Unknown message code " << (int)t << std::endl;
	}
	return o;
}

void MachGuiMessageBroker::sendHasMachinesCDMessage( const string& playerName, bool hasMachinesCD )
{
	MachLogNetMessage* pLogMessage = _NEW( MachLogNetMessage() );
	MachGuiHasMachinesCDMessage* pMessage = _REINTERPRET_CAST( MachGuiHasMachinesCDMessage*, pLogMessage );
	pMessage->header_.systemCode_ = SYSTEM_MACHGUI_MESSAGE;
	pMessage->header_.messageCode_ = MT_HASMACHINESCDMESSAGE;
	pMessage->header_.totalLength_ = sizeof( MachGuiHasMachinesCDMessage );

	ASSERT( strlen( playerName.c_str() ) <= MAX_PLAYERNAME_LEN, "player name too long" );

	strcpy( pMessage->playerName_, playerName.c_str() );

	pMessage->hasMachinesCD_ = hasMachinesCD;

	doSend( pLogMessage );
}

void MachGuiMessageBroker::processHasMachinesCDMessage( MachGuiHasMachinesCDMessage* pMessage )
{
	PRE( pStartupData_ );

	pStartupData_->receivedHasMachinesCDMessage( pMessage->playerName_, pMessage->hasMachinesCD_ );
}

void MachGuiMessageBroker::sendIveLostMessage( const string& playerName )
{
	MachLogNetMessage* pLogMessage = _NEW( MachLogNetMessage() );
	MachGuiIveLostMessage* pMessage = _REINTERPRET_CAST( MachGuiIveLostMessage*, pLogMessage );
	pMessage->header_.systemCode_ = SYSTEM_MACHGUI_MESSAGE;
	pMessage->header_.messageCode_ = MT_IVELOSTMESSAGE;
	pMessage->header_.totalLength_ = sizeof( MachGuiIveLostMessage );

	strcpy( pMessage->playerName_, playerName.c_str() );

	doSend( pLogMessage );
}


void MachGuiMessageBroker::sendNameChangeMessage( const string& newPlayerName, int uniqueMachineNumber )
{
	NETWORK_STREAM("MachGuiMessageBroker::sendJoinMessage\n" );
	NETWORK_INDENT( 2 );
	MachLogNetMessage* pLogMessage = _NEW( MachLogNetMessage() );
	MachGuiNameChangeMessage* pMessage = _REINTERPRET_CAST( MachGuiNameChangeMessage*, pLogMessage );
	pMessage->header_.systemCode_ = SYSTEM_MACHGUI_MESSAGE;
	pMessage->header_.messageCode_ = MT_NAMECHANGEMESSAGE;
	pMessage->header_.totalLength_ = sizeof( MachGuiNameChangeMessage );

	ASSERT( strlen( newPlayerName.c_str() ) <= MAX_PLAYERNAME_LEN, "player name too long" );

	strcpy( pMessage->playerName_, newPlayerName.c_str() );

	pMessage->uniqueMachineNumber_ = uniqueMachineNumber;

	doSend( pLogMessage );
	NETWORK_INDENT( -2 );
	NETWORK_STREAM("MachGuiMessageBroker::sendJoinMessage DONE\n" );
}

void MachGuiMessageBroker::processNameChangeMessage( MachGuiNameChangeMessage* pMessage )
{
	pStartupData_->receivedNameChangeMessage( pMessage->playerName_, pMessage->uniqueMachineNumber_ );
}

/* End MESSBROK.CPP *************************************************/
