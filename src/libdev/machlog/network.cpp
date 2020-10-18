/*
 * N E T W O R K . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "machlog/network.hpp"

#include "profiler/stktrace.hpp"
#include "mathex/point2d.hpp"
#include "utility/linetok.hpp"
#include "network/netdefs.hpp"
#include "network/netnet.hpp"
#include "network/session.hpp"
#include "network/nodeuid.hpp"
#include "network/node.hpp"
#include "machlog/messbrok.hpp"
#include "machlog/scenario.hpp"
#include "machlog/internal/netimpl.hpp"
#include "machphys/random.hpp"

#include "system/metafile.hpp"
#include "system/metaistr.hpp"
#include <memory>

//#define DEFINE_GUID
#define INITGUID
//#include <windows.h>
//#include <initguid.h>

// {7D2DD660-D859-11d1-B2ED-0020AFEBB1CC}
//DEFINE_GUID(DPMACHINES_GUID, 0x7d2dd660, 0xd859, 0x11d1, 0xb2, 0xed, 0x0, 0x20, 0xaf, 0xeb, 0xb1, 0xcc);

void setAppUid();

// static
MachLogNetwork& MachLogNetwork::instance()
{
    static MachLogNetwork instance_;
    return instance_;
}

MachLogNetwork::MachLogNetwork()
:	pImpl_( _NEW( MachLogNetworkDataImpl ) )
{
	CB_MachLogNetwork_DEPIMPL();

	expectedPlayers_ = 2;
	pNode_ = NULL;
	pBroker_ = NULL;
	terminateAndReset();
//	NetNetwork::instance().directXProtocol( true );
//	NetNetwork::instance().asyncMessaging( true );
//	NetNetwork::instance().guaranteed( true );
	DEBUG_STREAM( DIAG_NETWORK,"MachLogNetwork::CTOR\n" );
    TEST_INVARIANT;
}

MachLogNetwork::~MachLogNetwork()
{
    TEST_INVARIANT;

	CB_MachLogNetwork_DEPIMPL();

	DEBUG_STREAM( DIAG_NETWORK,"MachLogNetwork::DTOR\n" );
	terminateAndReset();

	_DELETE( pImpl_ );
}

void MachLogNetwork::terminateAndReset()
{
	CB_MachLogNetwork_DEPIMPL();

	DEBUG_STREAM( DIAG_NETWORK,"MachLogNetwork::terminateAndReset\n" );
    NETWORK_STREAM( ProStackTracer() );

	if( pNode_ )
		_DELETE( pNode_ );
	pNode_ = NULL;
	//do not delete the broker!
//	if( pBroker_ )
//		_DELETE( pBroker_ );
//	pBroker_ = NULL;
	isNetworkGame_ = false;
	isNodeLogicalHost_ = false;
	for( MachPhys::Race i = MachPhys::RED; i != MachPhys::N_RACES; ++i)
		readyStatus_[ i ] = false;
	desiredProtocol_ = "";
	protocolChosen_ = false;
	NetNetwork::instance().resetStatus();
	DEBUG_STREAM( DIAG_NETWORK,"MachLogNetwork::terminateAndReset done\n" );
}


void MachLogNetwork::processNetworkIniSettings( const string& fileName )
{
	CB_MachLogNetwork_DEPIMPL();

	DEBUG_STREAM( DIAG_NETWORK,"MachLogNetwork::processNetworkIniSettings " << fileName << "\n" );

    SysMetaFile metaFile( "mach1.met" );

    std::auto_ptr< std::istream > pIstream;

    if( SysMetaFile::useMetaFile() )
    {
        //pIstream = _NEW( SysMetaFileIstream( metaFile, fileName, ios::text ) );
        pIstream = std::auto_ptr< std::istream > (
            _NEW( SysMetaFileIstream( metaFile, fileName, std::ios::in ) ));
    }
    else
    {
        ASSERT_FILE_EXISTS( fileName.c_str() );
        //pIstream = _NEW( ifstream( fileName.c_str(), ios::text | ios::in ) );
        pIstream = std::auto_ptr< std::istream > (
            _NEW( std::ifstream( fileName.c_str(), std::ios::in ) ));
    }

	UtlLineTokeniser parser( *pIstream, fileName );

	bool processingNetworkTokens = false;
	string protocol;
	string stringSessionId;
	while( not parser.finished() )
	{
		if( parser.tokens()[0] == "NETWORK" )
		{
			isNetworkGame_ = true;
			processingNetworkTokens = true;
		}
		if( parser.tokens()[0] == "END" )
		{
			ASSERT( parser.tokens().size() > 1,"Out of tokens on an END line" );
			if( parser.tokens()[1] == "NETWORK" )
				processingNetworkTokens = false;
		}
		if( processingNetworkTokens )
		{
			if( parser.tokens()[0] == "PROTOCOL" )
			{
				protocol = "";
				for( int i = 1; i < parser.tokens().size(); ++i )
				{
					protocol += parser.tokens()[i];
					if( i != ( parser.tokens().size() - 1 ) )
						protocol += " ";
				}

			}
			if( parser.tokens()[0] == "RACE" )
				localRace_ = MachLogScenario::machPhysRace( parser.tokens()[1] );
			if( parser.tokens()[0] == "HOST" )
				isNodeLogicalHost_ = true;
			if( parser.tokens()[0] == "SESSION" )
				stringSessionId = parser.tokens()[1];
			if( parser.tokens()[0] == "PLAYERS" )
				expectedPlayers_ = atol( parser.tokens()[1].c_str() );
			if( parser.tokens()[0] == "SET_READY" )
				readyStatus_[ MachLogScenario::machPhysRace( parser.tokens()[1] ) ] = true;
//			if( parser.tokens()[0] == "JOIN" )


		}
		parser.parseNextLine();
	}

	if( isNetworkGame_ )
	{
		DEBUG_STREAM( DIAG_NETWORK, " isNetworkGame " << std::endl );
		setAppUid();
		if( desiredProtocol( protocol ) )
		{
//			DWORD dwNameSize = 200;
			char szSessionName[200];
//			GetComputerName(szSessionName, &dwNameSize);
			if( isNodeLogicalHost_ )
				hostWithSessionId( stringSessionId, string( szSessionName ) );
			else
				joinWithSessionId( stringSessionId, string( szSessionName ) );
		}
	}

}

MachPhys::Race MachLogNetwork::localRace() const
{
	CB_DEPIMPL( MachPhys::Race, localRace_ );

	return localRace_;
}

bool MachLogNetwork::isNetworkGame() const
{
	CB_DEPIMPL( bool, isNetworkGame_ );

	return isNetworkGame_;
}

bool MachLogNetwork::isNodeLogicalHost() const
{
	CB_DEPIMPL( bool, isNodeLogicalHost_ );

	return isNodeLogicalHost_;
}

MachLogNetwork::Remote MachLogNetwork::remoteStatus( MachPhys::Race race ) const
{
	CB_DEPIMPL_ARRAY( MachLogNetwork::Remote, remote_ );

	return remote_[ race ];
}

void MachLogNetwork::remoteStatus( MachPhys::Race race, MachLogNetwork::Remote remote )
{
	CB_DEPIMPL_ARRAY( MachLogNetwork::Remote, remote_ );

	remote_[ race ] = remote;
}


const string&	MachLogNetwork::desiredProtocol() const
{
	CB_DEPIMPL( string, desiredProtocol_ );

	return desiredProtocol_;
}



void MachLogNetwork::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogNetwork& t )
{

    o << "MachLogNetwork " << (void*)&t << " start" << std::endl;
//	o << " local Race " << t.localRace_ << std::endl;
    o << "MachLogNetwork " << (void*)&t << " end" << std::endl;

    return o;
}

void setAppUid()
{
	DEBUG_STREAM( DIAG_NETWORK,"setAppUid\n" );

//	NetNetwork::setAppUid((void*)&DPMACHINES_GUID );
	DEBUG_STREAM( DIAG_NETWORK,"setAppUid done\n" );
}

void MachLogNetwork::setBroker( MachLogMessageBroker* pBroker )
{
	CB_DEPIMPL( MachLogMessageBroker*, pBroker_ );

	pBroker_ = pBroker;
}

void MachLogNetwork::update()
{
	CB_MachLogNetwork_DEPIMPL();

	if( not isNetworkGame_ )
		return;

//NETWORK_STREAM( "MachLogNetwork::update " );
//NETWORK_STREAM( "NetNetwork::instance().session().nodes().size() " << NetNetwork::instance().session().nodes().size() << std::endl );
//  I am removing this call to update the nodes as I am hoping this will give us more messages coming through
//	NetNetwork::instance().session().updateNodes();
	NetNetwork::instance().pollMessages();
	if( NetNetwork::instance().currentStatus() == NetNetwork::NETNET_CONNECTIONERROR )
	{
		//NETWORK_STREAM("NetNetwork has gone to NETNET_CONNECTIONERROR after pollmessages call\n" );
	}
	//remember that the network game may have dropped out due to system messages
	//in the pollmessages call above...therefore check that game is still running.
	if( not isNetworkGame() )
		return;
//NETWORK_STREAM( " deref node\n" );
//	NetNode& nod = node();
//NETWORK_STREAM( " pNode_ " << (void*)&nod << " asking haveMessages\n" );
	if( pBroker_ )
	{
	//	ASSERT( pBroker_ != NULL,"message broker is NULL and trying to call messageBroker method\n" );
		MachLogMessageBroker& broker = messageBroker();
		//network game status _may_ be changed by processing a message
//		while( isNetworkGame_ and nod.haveMessages() )
		while( isNetworkGame_ and NetNetwork::instance().haveMessages() )
		{
	//NETWORK_STREAM( " gunna process a message..\n" );
			broker.processMessage( NetNetwork::instance().getMessage() );
		}


		//send any messages that the broker had to cache.
	//NETWORK_STREAM( "MLNetwork::update has broker cached messages? " << broker.hasCachedOutgoingMessages() << std::endl );
		if( isNetworkGame_ and broker.hasCachedOutgoingMessages() )
			broker.sendCachedOutgoingMessages();
	}

	if( isNetworkGame_ )
	{
/*		nod.transmitCompoundMessage();
	 	if( nod.lastPingAllTime() > 5 )
		{
			nod.pingAll();
		}*/
	}

//NETWORK_STREAM(  "MachLogNetwrok::update DONE\n" );
}

NetNode& MachLogNetwork::node()
{
	CB_DEPIMPL( NetNode*, pNode_ );

	return *pNode_;
}

MachLogMessageBroker&	MachLogNetwork::messageBroker()
{
	CB_DEPIMPL( MachLogMessageBroker*, pBroker_ );

	return *pBroker_;
}

void MachLogNetwork::ready( MachPhys::Race race, bool newValue )
{
	CB_DEPIMPL_ARRAY( bool, readyStatus_ );

	readyStatus_[ race ] = newValue;
}

bool MachLogNetwork::ready( MachPhys::Race race ) const
{
	CB_DEPIMPL_ARRAY( bool, readyStatus_ );

	return readyStatus_[ race ];
}

int MachLogNetwork::expectedPlayers() const
{
	CB_DEPIMPL( int, expectedPlayers_ );

	return expectedPlayers_;
}


bool MachLogNetwork::desiredProtocol( const string& protocol, MachLogNetwork::InitialiseConnection initConnection )
{
	CB_MachLogNetwork_DEPIMPL();

	desiredProtocol_ = protocol;
INSPECT_ON( std::cout, desiredProtocol_ );
	const NetNetwork::ProtocolMap& availableProtocols = NetNetwork::availableProtocols();
	NetNetwork::ProtocolMap::const_iterator findWhere = availableProtocols.find( desiredProtocol_ );
	NetNetwork::InitialiseConnection netInitConnection = ( initConnection == INITIALISE_CONNECTION ) ? NetNetwork::INITIALISE_CONNECTION : NetNetwork::DO_NOT_INITIALISE_CONNECTION;
#ifndef NDEBUG
WHERE;
#endif
	if( findWhere != availableProtocols.end() )
	{
#ifndef NDEBUG
WHERE;
#endif
		NetNetwork::chooseProtocol( desiredProtocol_, netInitConnection );
		protocolChosen_ = true;
		return true;
	}
	return false;
}

void MachLogNetwork::initialiseConnection()
{
	NetNetwork::instance().initialiseConnection();
}


const string& MachLogNetwork::sessionId() const
{
	CB_DEPIMPL( string, sessionId_ );

	return sessionId_;
}

bool MachLogNetwork::hostWithSessionId( const string& gameName, const string& playerName )
{
	CB_MachLogNetwork_DEPIMPL();

	PRE( protocolChosen_ );
	isNetworkGame_ = true;
	isNodeLogicalHost_ = true;
	sessionId_ = gameName;
	NetNetwork::instance().createAppSession( sessionId_ );
//	DWORD dwNameSize = 200;
//	char szSessionName[200];
//	GetComputerName(szSessionName, &dwNameSize);
	NetNodeName name( playerName.c_str() );
	NetNetwork::instance().localPlayerName(name);
//	pNode_ = _NEW(NetNode(name));
//	pNode_->useCompoundMessaging( true );
	return true;
}

bool MachLogNetwork::joinWithSessionId( const string& gameName, const string& playerName )
{
	CB_MachLogNetwork_DEPIMPL();

	PRE( protocolChosen_ );
	sessionId_ = gameName;
	NetAppSessionUid* pSelectedSessionUid = NULL;
	PhysAbsoluteTime startTime = Phys::time();
	std::cout << "\n";
	PhysRelativeTime timeout = 30;
	if( NetNetwork::instance().currentProtocol() == NetNetwork::MODEM )
		timeout = 1;
	while( not pSelectedSessionUid and ( ( Phys::time() - startTime ) < timeout ) )
	{
		std::cout << "TimeOut in : " << timeout - ( Phys::time() - startTime ) << " seconds.                 \r";
		//issue an update to the network which will go and populate the sessions collection.
//		if( NetNetwork::instance().currentProtocol() != NetNetwork::MODEM )
//			NetNetwork::instance().update();
		const NetNetwork::Sessions& availableSessions = NetNetwork::instance().sessions();
		NetAppSessionName selectedSessionName( sessionId_ );
		NetNetwork::Sessions::const_iterator i = availableSessions.begin();
		NetNetwork::Sessions::const_iterator j = availableSessions.end();

		for( ; i != j and not pSelectedSessionUid; ++i )
		{
			if( (*i)->appSessionName() == selectedSessionName )
			{
				pSelectedSessionUid = *i;
			}
		}
//		SysWindowsAPI::sleep( 1000 );

	}
	std::cout << std::endl;
	std::cout << "Sessions " << NetNetwork::instance().sessions().size() << std::endl;

	if( pSelectedSessionUid )
	{
		NetNetwork::instance().joinAppSession(*pSelectedSessionUid);

//		DWORD dwNameSize = 200;
//		char szSessionName[200];
//		GetComputerName(szSessionName, &dwNameSize);

		NetNodeName name( playerName.c_str() );
		NetNetwork::instance().localPlayerName(name);
//		pNode_ = _NEW(NetNode(name));
//		pNode_->useCompoundMessaging( true );
		isNetworkGame_ = true;
		return true;
	}
	else
	{
		ASSERT_INFO( pImpl_->sessionId_ );
		ASSERT(pSelectedSessionUid, "Invalid session (for joining) ID");
		isNetworkGame_ = false;
	}
	return false;
}

void MachLogNetwork::localRace( MachPhys::Race newLocalRace )
{
	CB_DEPIMPL( MachPhys::Race, localRace_ );

	localRace_ = newLocalRace;
}

bool MachLogNetwork::launchFromLobbyInfo()
{
	NETWORK_STREAM("MachLogNetwork::launchFromLobbyInfo()\n" );
	pImpl_->isNetworkGame_ = true;
	NETWORK_STREAM(" call connect App Session\n" );
	NETWORK_INDENT( 2 );
	NetNetwork::instance().connectAppSession();
	if( NetNetwork::instance().isLogicalHost() )
		pImpl_->isNodeLogicalHost_ = true;

	NetNodeName name( NetNetwork::instance().localPlayerName() );
	NetNetwork::instance().localPlayerName(name);
//	pImpl_->pNode_ = _NEW(NetNode(name));
//	pImpl_->pNode_->useCompoundMessaging( true );
	NETWORK_INDENT( -2 );
	NETWORK_STREAM("MachLogNetwork::launchFromLobbyInfo() DONE\n" );
	return true;
}

void MachLogNetwork::isNodeLogicalHost( bool newValue )
{
	pImpl_->isNodeLogicalHost_ = newValue;
}

void MachLogNetwork::setNodeCompoundStatus( bool value )
{
	//pImpl_->pNode_->useCompoundMessaging( value );
}

/* End NETWORK.CPP **************************************************/
