#include "stdlib/string.hpp"
#include "profiler/profiler.hpp"
#include "ctl/algorith.hpp"
#include "utility/linetok.hpp"

#include "network/node.hpp"
#include "network/nodeuid.hpp"
#include "network/sessuid.hpp"
#include "network/session.hpp"
#include "network/messbody.hpp"
#include "network/message.hpp"
#include "network/sysmess.hpp"

#include "network/internal/sessioni.hpp"
#include "network/internal/mapping.hpp"
#include "network/internal/netinet.hpp"
#include "network/internal/recorder.hpp"
#include "network/internal/compound.hpp"

#include "recorder/recorder.hpp"

#include "system/metafile.hpp"
#include "system/metaistr.hpp"
#include "system/registry.hpp"
#include <memory>

#define MAXNAMELEN 200

#define INITGUID

NetINetwork::~NetINetwork()
{
	NETWORK_STREAM("NetINetwork::~NetINetwork " << (void*)this << std::endl );
	NETWORK_INDENT( 2 );
	NETWORK_STREAM("ask record what state it is in...\n" );
    if( RecRecorder::instance().state() != RecRecorder::PLAYING )
    {
        RecRecorder::instance().recordingAllowed( false );

		NETWORK_STREAM("call to NetINetwork::clearProtocols" << std::endl );
	    NetINetwork::clearProtocols();

		NETWORK_STREAM("delete sessions" << std::endl );
    	NetNetwork::Sessions::iterator i = sessions_.begin();
	    NetNetwork::Sessions::iterator j = sessions_.end();

    	for(;i!=j;++i)
    	{
    		_DELETE(*i);
    	}
    	i = sessions_.begin();
    	if(i!=j)
    		sessions_.erase(i,j);

		NETWORK_STREAM("delete nodes" << std::endl );

    	Nodes::iterator m = nodes_.begin();
    	Nodes::iterator n = nodes_.end();

    	int index = 0;

    	while(m!=n)
    	{
    		Nodes::iterator p = m;
    		++m;
    		_DELETE(*p);
    	}

		NETWORK_STREAM(" delete message handler" << std::endl );
    	if( pSystemMessageHandler_ )
    		_DELETE( pSystemMessageHandler_ );

        enet_deinitialize();

        RecRecorder::instance().recordingAllowed( true );
    }
	NETWORK_INDENT( -2 );
	NETWORK_STREAM("NetINetwork::~NetINetwork DONE " << (void*)this << std::endl );
}

NetProcessUid& NetINetwork::processUidMaster() const
{
	PRE( isValidNoRecord() );
	static bool initialised = false;

	static NetProcessUid thisProcessUid("temp", 0);

	return thisProcessUid;
}

bool NetINetwork::hasAppSession( const NetAppSessionUid& aUid ) const
{
	PRE( isValidNoRecord() );

	bool found = false;

    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
    {
        found = NetIRecorder::instance().playbackHasAppSession();
    }
    else
    {
        found = hasAppSessionNoRecord( aUid );

        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
        {
            NetIRecorder::instance().recordHasAppSession( found );
        }
    }

	POST( isValidNoRecord() );

	return found;
}

///////////////////////////////

bool NetINetwork::hasAppSessionNoRecord( const NetAppSessionUid& aUid ) const
{
	PRE( isValidNoRecord() );

	bool found = false;

	NetNetwork::Sessions::const_iterator i = sessions_.begin();
	NetNetwork::Sessions::const_iterator j = sessions_.end();

	while(i!=j && !found)
    {
		if((*(*i)) == aUid)
	    	found = true;
    	++i;
	}

	POST( isValidNoRecord() );

	return found;
}

///////////////////////////////

// call periodically to ensure that network information remains current.
void NetINetwork::update()
{
	PRE( isValidNoRecord() );

	NETWORK_STREAM("NetINetwork::update \n" );
    ProProfiler::instance().traceStack( Diag::instance().networkStream(), true, 0, "" );

	updateSessions();

	POST( isValidNoRecord() );
}

const NetNetwork::Sessions& NetINetwork::sessions() const
{
	PRE( isValidNoRecord() );

    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
    {
        NetINetwork* nonConstThis = _CONST_CAST( NetINetwork*, this );

        NetIRecorder::instance().playbackSessions( &nonConstThis->sessions_ );
    }
    else
    {
        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
        {
            NetIRecorder::instance().recordSessions( sessions_ );
        }
    }

	return sessions_;
// POST( for_each( result.begin(), result.end(), negate( mem_fun( NetAppSession::isEmpty ) ) ) );
// i.e. every app session contains at least one node.
}

///////////////////////////////


///////////////////////////////
NetAppUid NetINetwork::appUid() const
{
	PRE( isValidNoRecord() );

    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
    {
        appUid_ = NetIRecorder::instance().playbackAppUid();
    }
    else
    {
        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
        {
            NetIRecorder::instance().recordAppUid( appUid_ );
        }
    }

	return appUid_;
}

NetAppUid NetINetwork::appUidNoRecord() const
{
	PRE( isValidNoRecord() );

	return appUid_;
}

NetINetwork::NetINetwork()
:	pLocalSession_(NULL),
	pSystemMessageHandler_( NULL ),
	lobbyAware_( false ),
	descFileName_( "" ),
	descCommandLine_( "" ),
	descPath_( "" ),
	descCurrentDirectory_( "" ),
	descDescription_( "" ),
	isLogicalHost_( false ),
	localPlayerName_( "" ),
	deterministicPingDropoutAllowed_( false ),
	maxBytesPerSecond_( 9000 ),
	imStuffed_( false ),
	pingAllAllowed_( true ),
	maxSentMessagesPerSecond_( 40 ),
	messageThrottlingActive_( false ),
	originalMaxSentMessagesPerSecond_( 40 ),
	isLobbiedGame_(false),
	pHost_(NULL)
{
   	PRE( isValidNoRecord() );

    if( RecRecorder::instance().state() != RecRecorder::PLAYING )
    {
        RecRecorder::instance().recordingAllowed( false );

        RecRecorder::instance().recordingAllowed( true );
    }

  	POST( isValidNoRecord() );
}

void NetINetwork::initHost(bool asServer)
{
    if(pHost_ != NULL)
    {
        Peers::const_iterator i = peers_.begin();
        Peers::const_iterator j = peers_.end();
        while(i != j)
        {
            enet_peer_disconnect (*i, 0);
            ++i;
        }
        peers_.clear();
        enet_host_destroy (pHost_);
        pHost_ = NULL;
    }

    ENetAddress address;
    if(asServer)
    {
        NETWORK_STREAM("NetINetwork::initHost Server\n" );
        // b. Create a host using enet_host_create
        address.host = ENET_HOST_ANY;
        address.port = 1234;

        pHost_ = enet_host_create(&address/* the address to bind the server host to */,
                         32      /* allow up to 32 clients and/or outgoing connections */,
                          2      /* allow up to 2 channels to be used, 0 and 1 */,
                          0      /* assume any amount of incoming bandwidth */,
                          0      /* assume any amount of outgoing bandwidth */);

        if (pHost_ == NULL)
        {
            std::cerr << "An error occured while trying to create an ENet server host" << std::endl;
            //exit(EXIT_FAILURE);
            currentStatus(NetNetwork::NETNET_NODEERROR);
            return;
        }
    }
    else
    {
        NETWORK_STREAM("NetINetwork::initHost Client\n" );
        // b. Create a host using enet_host_create
        pHost_ = enet_host_create(NULL, 16, 2, /*57600/8*/0, /*14400/8*/0);

        if (pHost_ == NULL)
        {
            std::cerr << "An error occured while trying to create an ENet server host" << std::endl;
            //exit(EXIT_FAILURE);
            currentStatus(NetNetwork::NETNET_NODEERROR);
            return;
        }
    }
}

void NetINetwork::pollMessages()
{
    //  This line must be outside the if for the recorder to work properly

    if( RecRecorder::instance().state() != RecRecorder::PLAYING )
    {
        RecRecorder::instance().recordingAllowed( false );

    	NETWORK_STREAM("NetINetwork::pollMessages\n" );
    	imStuffed_ = false;

    	//The poll message call will also ask the node how much data is insiode the node outgoing buffer.
    	//certain values will then set the "I'm stuffed flag" so that external clients will stop trying to stuff more in.

        ENetEvent event;
        while(enet_host_service(pHost_, &event, 0) > 0)
        {
            //eventStatus = enet_host_service(client, &event, 50000);
            // If we had some event that interested us
            switch(event.type)
            {
                case ENET_EVENT_TYPE_CONNECT:
                    NETWORK_STREAM("Got a new connection from " << event.peer->address.host << " " << event.peer->connectID << std::endl);
                    peers_.push_back(event.peer);
                    break;

                case ENET_EVENT_TYPE_RECEIVE:
                    NETWORK_STREAM("Got message from : " << event.peer->address.host << std::endl);
                    // First packet is an peer name
                    if(!event.peer->data)
                    {
                        char *data = _NEW_ARRAY(char, strlen((char*) event.packet->data)+1);
                        strcpy(data, (char*) event.packet->data);
                        event.peer->data = data;
                        NETWORK_STREAM("This is intruduction from: " << data << std::endl);
                    }
                    else
                    {
                    // Lets broadcast this message to all except sender
                    Peers::const_iterator i = peers_.begin();
                    Peers::const_iterator j = peers_.end();
                    while(i != j)
                    {
                        if((*i) != event.peer)
                        {
                            ENetPacket* packet = enet_packet_create(event.packet->data, event.packet->dataLength, 0);
                            enet_peer_send(*i, 0, packet);
                        }
                        ++i;
                    }

                    NetMessageBody messageBody( (uint8*)event.packet->data, event.packet->dataLength );
                    NetPriority priority(1);
                    NetMessageHeader thisMessageHeader(event.peer, priority);
                    NetMessage* pThisMessage = _NEW(NetMessage(thisMessageHeader, messageBody));
                    messageBuffer_.push_back(pThisMessage);
                    }
                    enet_packet_destroy(event.packet);
                    break;

                case ENET_EVENT_TYPE_DISCONNECT:
                    NETWORK_STREAM("Peer disconnected: " << event.peer->address.host << " "<< (char*)event.peer->data << std::endl);

                    // Reset client's information
                    _DELETE_ARRAY((char*)event.peer->data);
                    event.peer->data = NULL;
                    peers_.erase(std::remove(peers_.begin(), peers_.end(), event.peer), peers_.end());
                    if(peers_.empty())
                    {
                        if( pSystemMessageHandler_ )
                        bool shouldAbort = not pSystemMessageHandler_->handleSessionLostMessage();
                    }
                    break;
            }
        }
        NETWORK_STREAM("NetINetwork::pollMessages DONE\n" );
        RecRecorder::instance().recordingAllowed( true );
    }
}

bool NetINetwork::haveMessages()
{
	return (messageBuffer_.size() > 0);
}

NetMessage* NetINetwork::getMessage()
{

	PRE( isValidNoRecord() );

	NetMessage* pFrontMessage = messageBuffer_.front();
	messageBuffer_.erase(messageBuffer_.begin());

	POST(pFrontMessage);
	PRE( isValidNoRecord() );
	return pFrontMessage;
}

void NetINetwork::sendMessage( const NetPriority& priority, const NetMessage::NetMessageRecipients& /*to*/, const NetMessageBody& body)
{
    enet_uint32 flag = 0;
    if(priority.urgency() < 2)
        flag = ENET_PACKET_FLAG_RELIABLE;
    /* Create a reliable packet */
    ENetPacket * packet = enet_packet_create (body.body(),
                                          body.lengthInBytes(),
                                          flag);

    // Lets broadcast this message to all
    enet_host_broadcast(pHost_, 0, packet);

    NetNetwork::instance().netINetwork().addSentMessage( body.lengthInBytes() );
}

// Host a game
NetAppSession* NetINetwork::createAppSession( const NetAppSessionName& sessionName )
{
	PRE( isValidNoRecord() );

    initHost(true);
    return NULL;
}

NetAppSession* NetINetwork::joinAppSession( const NetAppSessionUid& sessionUid )
{
	PRE( isValidNoRecord() );

    initHost();

    // c. Connect and user service
    ENetAddress address;
    enet_address_set_host(&address, IPAddress().c_str());
    address.port = 1234;

    ENetPeer *pPeer;
    pPeer = enet_host_connect(pHost_, &address, 2, 0);

    if (pPeer == NULL)
    {
       std::cerr << "No available peers for initializing an ENet connection" << std::endl;
        //exit(EXIT_FAILURE);
        currentStatus(NetNetwork::NETNET_CONNECTIONERROR);
        return NULL;
    }

    // Introduce yourself - send a name packet
    ENetEvent  event;
    if (enet_host_service(pHost_, &event, 3000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT)
    {
      NETWORK_STREAM("Joining session succeeded.\n");
      //peers_.push_back(pPeer);

      ENetPacket *packet = enet_packet_create(localPlayerName_.c_str(), localPlayerName_.length()+1,
          ENET_PACKET_FLAG_RELIABLE);
      enet_peer_send(pPeer, 0, packet);
    }
    else
    {
      enet_peer_reset(pPeer);
      NETWORK_STREAM("Could not join session.\n");
      currentStatus(NetNetwork::NETNET_SESSIONERROR);
      return NULL;
    }
	return NULL;
}

NetAppSession* NetINetwork::connectAppSession()
{
	NETWORK_STREAM("NetINetwork::connectAppSession()\n" );
	PRE( isValidNoRecord() );

    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
    {
        isLogicalHost_ = NetIRecorder::instance().playbackIsLogicalHost();
        localPlayerName_ = NetIRecorder::instance().playbackLocalPlayerName();
    }
    else
    {

    }

	POST( isValidNoRecord() );
	POST(pLocalSession_)

	NETWORK_STREAM("NetINetwork::connectAppSession() DONE\n" );
	return pLocalSession_;
}

NetAppSession& NetINetwork::session()
{
	PRE( isValidNoRecord() );
	PRE(pLocalSession_);
	return *pLocalSession_;
}

// called periodically by update() to ensure that sessions_ remains current.
void NetINetwork::updateSessions()
{
    if( RecRecorder::instance().state() != RecRecorder::PLAYING )
    {
        RecRecorder::instance().recordingAllowed( false );

    	NETWORK_STREAM("NetINetwork::updateSessions() sessions before call " << sessions_.size() << std::endl );
    	PRE( isValidNoRecord() );
    	NetNetwork::Sessions::iterator i = sessions_.begin();
    	NetNetwork::Sessions::iterator j = sessions_.end();

    	for(;i!=j;++i)
    	{
    		_DELETE((*i));
    	}

     	sessions_.erase(sessions_.begin(), sessions_.end());

        sessions_.push_back(_NEW(NetAppSessionUid(0, 0, "game")));
        RecRecorder::instance().recordingAllowed( true );
    }

//	POST( isValidNoRecord() );
}

const NetNetwork::ProtocolMap& NetINetwork::availableProtocols(UpdateType update)
{
	NETWORK_STREAM("NetINetwork::availableProtocols\n" );
	NETWORK_INDENT( 2 );
	PRE(currentStatusNoRecord() == NetNetwork::NETNET_OK);

	NetNetwork::ProtocolMap& availableProtocols = NetNetwork::instance().netINetwork().availableProtocols_;

	if(update == UPDATE)
	{
		NETWORK_STREAM("UPDATE\n" );
		NetNetwork::ProtocolMap::iterator i = availableProtocols.begin();
		NetNetwork::ProtocolMap::iterator j = availableProtocols.end();

		if(i!=j)
			availableProtocols.erase(i, j);

        if( RecRecorder::instance().state() == RecRecorder::PLAYING )
        {
            NetIRecorder::instance().playbackAvailableProtocols( &availableProtocols );
        }
        else
        {
    		NetNetwork::ProtocolMap* pAvailableProtocols = &availableProtocols;
    		availableProtocols.insert("UDP connection for IP v4", 0);


            if( RecRecorder::instance().state() == RecRecorder::RECORDING )
            {
                NetIRecorder::instance().recordAvailableProtocols( availableProtocols );
            }
        }

		i = availableProtocols.begin();
		j = availableProtocols.end();

	}

	POST(currentStatusNoRecord() == NetNetwork::NETNET_OK);
	NETWORK_INDENT( -2 );
	NETWORK_STREAM("NetINetwork::availableProtocols return " << (void*)&availableProtocols << " elements " << availableProtocols.size() << std::endl );
	return availableProtocols;
}

void NetINetwork::clearProtocols()
{
	NETWORK_STREAM("NetINetwork::clearProtocols\n" );
	NETWORK_INDENT( 2 );
	PRE(currentStatusNoRecord() == NetNetwork::NETNET_OK);

	NETWORK_STREAM("there are " << availableProtocols_.size() << " entries in the map" << std::endl );

	NetNetwork::ProtocolMap::iterator i = availableProtocols_.begin();
	NetNetwork::ProtocolMap::iterator j = availableProtocols_.end();

	NETWORK_STREAM(" iterator to begin is " << (void*)&(*i) << std::endl );
	NETWORK_STREAM(" iterator to end   is " << (void*)&(*j) << std::endl );
	NETWORK_STREAM(" iterator i == iterator j " << bool( i == j ) << std::endl );

	NETWORK_STREAM(" delete all the array data " << std::endl );
	for(;i!=j;++i)
	{
		NETWORK_STREAM("delete for " << (*i).first << std::endl );
	}

	i = availableProtocols_.begin();

	NETWORK_STREAM(" erase elements\n" );
	if(i!=j)
		availableProtocols_.erase(i, j);

	POST(currentStatusNoRecord() == NetNetwork::NETNET_OK);
	NETWORK_INDENT( -2 );
	NETWORK_STREAM("NetINetwork::clearProtocols DONE\n" );
}

void NetINetwork::chooseProtocol(const string& protoName, NetNetwork::InitialiseConnection /*initConnection*/ )
{
   	const NetNetwork::ProtocolMap& availableProtocolsConst = NetINetwork::availableProtocols();
  	NetNetwork::ProtocolMap& availableProtocols = _CONST_CAST(NetNetwork::ProtocolMap&, availableProtocolsConst);

    if( RecRecorder::instance().state() != RecRecorder::PLAYING )
    {
        RecRecorder::instance().recordingAllowed( false );

    	NETWORK_STREAM("NetINetwork::chooseProtocol\n" );
    	PRE(currentStatusNoRecord() == NetNetwork::NETNET_OK);

    	protocolName( protoName );

    	if( strstr( protoName.c_str(),"UDP" ) )
    	{
    		currentProtocol_ = NetNetwork::UDP;
    		NETWORK_STREAM(" NetNetwork::UDP\n" );
    	}
    	else if( strstr( protoName.c_str(),"IPX " ) )
    	{
    		currentProtocol_ = NetNetwork::IPX;
    		NETWORK_STREAM(" NetNetwork::IPX\n" );
    	}
    	else if( strstr( protoName.c_str()," TCP/IP " ) )
    	{
    		currentProtocol_ = NetNetwork::TCPIP;
    		NETWORK_STREAM(" NetNetwork::TCPIP\n" );
    	}
    	else if( strstr( protoName.c_str(),"Serial " ) )
    	{
    		currentProtocol_ = NetNetwork::SERIAL;
    		NETWORK_STREAM(" NetNetwork::SERIAL\n" );
    	}
    	else if( strstr( protoName.c_str(),"Modem " ) )
    	{
    		currentProtocol_ = NetNetwork::MODEM;
    		NETWORK_STREAM(" NetNetwork::MODEM\n" );
    	}
    	else if( strstr( protoName.c_str()," Zone " ) )
    	{
    		currentProtocol_ = NetNetwork::ZONE;
    		NETWORK_STREAM(" NetNetwork::ZONE\n" );
    	}
    	else
    	{
    		currentProtocol_ = NetNetwork::OTHER;
    		NETWORK_STREAM(" NetNetwork::OTHER\n" );
    	}

    	//if( initConnection == NetNetwork::INITIALISE_CONNECTION )
    	{
    		// a. Initialize enet
    		NETWORK_STREAM("NetINetwork::chooseProtocol init ENet\n" );
            if (enet_initialize() != 0)
            {
                std::cerr << "An error occured while initializing ENet." << std::endl;
                //return EXIT_FAILURE;
                return;
            }

            atexit(enet_deinitialize);

    	}
        RecRecorder::instance().recordingAllowed( true );
    }
}

void NetINetwork::initialiseConnection()
{
   	const NetNetwork::ProtocolMap& availableProtocolsConst = NetINetwork::availableProtocols();
  	NetNetwork::ProtocolMap& availableProtocols = _CONST_CAST(NetNetwork::ProtocolMap&, availableProtocolsConst);

    if( RecRecorder::instance().state() != RecRecorder::PLAYING )
    {
        RecRecorder::instance().recordingAllowed( false );

    	NETWORK_STREAM("NetINetwork::initialiseConnection\n" );
    	
    	NETWORK_STREAM("NetINetwork::initialiseConnection DONE\n" );

        RecRecorder::instance().recordingAllowed( true );
    }
}


bool NetINetwork::hasAppSession( const NetAppSessionName& appSessionName ) const
{
	PRE( isValidNoRecord() );

	bool found = false;

    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
    {
        found = NetIRecorder::instance().playbackHasAppSession();
    }
    else
    {
        found = hasAppSessionNoRecord( appSessionName );

        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
        {
            NetIRecorder::instance().recordHasAppSession( found );
        }
    }

	POST( isValidNoRecord() );

	return found;
}

bool NetINetwork::hasAppSessionNoRecord( const NetAppSessionName& appSessionName ) const
{
	PRE( isValidNoRecord() );

	bool found = false;

	NetNetwork::Sessions::const_iterator i = sessions_.begin();
	NetNetwork::Sessions::const_iterator j = sessions_.end();

	while(i!=j && not found)
	{
		if((*i)->appSessionName() == appSessionName)
			found = true;
		++i;
	}

	POST( isValidNoRecord() );

	return found;
}

bool NetINetwork::isValid() const
{
	return (currentStatus() == NetNetwork::NETNET_OK);
}

bool NetINetwork::isValidNoRecord() const
{
	return (currentStatusNoRecord() == NetNetwork::NETNET_OK);
}

//static
const string& NetINetwork::protocolName()
{
	string& currentProtocolName = protocolNameNoRecord();

    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
    {
        currentProtocolName = NetIRecorder::instance().playbackCurrentProtocolName();
    }
    else
    {
        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
        {
            NetIRecorder::instance().recordCurrentProtocolName( currentProtocolName );
        }
    }

	return currentProtocolName;
}

//static
string& NetINetwork::protocolNameNoRecord()
{
	static string currentProtocolName;

	return currentProtocolName;
}

//static
void NetINetwork::protocolName( const string& name )
{
    protocolNameNoRecord() = name;
}

void NetINetwork::setAppUid()
{
    if( RecRecorder::instance().state() != RecRecorder::PLAYING )
    {
        RecRecorder::instance().recordingAllowed( false );

    	NETWORK_STREAM("NetINetwork::setAppUid\n" );
        const SysPathName pathName( "appguid.ini" );

        SysMetaFile metaFile( "mach1.met" );

        std::auto_ptr< istream > pGuidStream;

        if( SysMetaFile::useMetaFile() )
        {
            pGuidStream = std::auto_ptr< istream >
                (_NEW( SysMetaFileIstream( metaFile, pathName, std::ios::in ) ));
        }
        else
        {
            ASSERT_FILE_EXISTS( pathName.c_str() );
            pGuidStream = std::auto_ptr< istream >
                (_NEW( std::ifstream( pathName.c_str(), std::ios::in ) ));
        }

    	NETWORK_STREAM(" close appguid.ini\n" );

        std::auto_ptr< istream > pGuidStream2;

        if( SysMetaFile::useMetaFile() )
        {
            pGuidStream2 = std::auto_ptr< istream >
                (_NEW( SysMetaFileIstream( metaFile, pathName, std::ios::in ) ));
        }
        else
        {
            ASSERT_FILE_EXISTS( pathName.c_str() );
            pGuidStream2 = std::auto_ptr< istream >
                (_NEW( std::ifstream( pathName.c_str(), std::ios::in ) ));
        }

    	NETWORK_STREAM(" open line tokeniser with appguid.ini\n" );
    	UtlLineTokeniser parser( *pGuidStream2, pathName.c_str() );

    	static string appName = "";
    	static string fileName = "";
    	static string commandLine = "";
    	static string description = "";
    	while( not parser.finished() )
    	{
    		const string& token = parser.tokens()[0];
    		if( token == "application" )
    		{
    			appName = "";
    			for( int i = 1; i < parser.tokens().size(); ++i )
    			{
    				appName += parser.tokens()[i];
    				if( i < parser.tokens().size() - 1 )
    					appName += " ";
    			}
    		}
    		else if( token == "filename" )
    		{
    			fileName = "";
    			for( int i = 1; i < parser.tokens().size(); ++i )
    			{
    				fileName += parser.tokens()[i];
    				if( i < parser.tokens().size() - 1 )
    					fileName += " ";
    			}
    		}
    		else if( token == "commandline" )
    		{
    			commandLine = "";
    			for( int i = 1; i < parser.tokens().size(); ++i )
    			{
    				commandLine += parser.tokens()[i];
    				if( i < parser.tokens().size() - 1 )
    					commandLine += " ";
    			}
    		}
    		else if( token == "description" )
    		{
    			description = "";
    			for( int i = 1; i < parser.tokens().size(); ++i )
    			{
    				description += parser.tokens()[i];
    				if( i < parser.tokens().size() - 1 )
    					description += " ";
    			}
    		}
    		else if( token == "maxBytesPerSecond" )
    		{
				maxBytesPerSecond_ = atol( parser.tokens()[1].c_str() );
    		}
    		parser.parseNextLine();
    	}
    	ASSERT( appName.length() > 0, "The application name must be supplied to register the app.\n" );
    	NETWORK_STREAM(" file parsed\n" );

        RecRecorder::instance().recordingAllowed( true );
    }
}

//static
void NetINetwork::resetStatus()
{
    if( RecRecorder::instance().state() != RecRecorder::PLAYING )
    {
        RecRecorder::instance().recordingAllowed( false );

    	currentStatus( NetNetwork::NETNET_OK );
//	    NetIAppSession::currentStatusNoRecord() = NetAppSession::NETSESS_OK;
//    	NetNode::currentStatusNoRecord() = NetNode::NETNODE_OK;

        RecRecorder::instance().recordingAllowed( true );
    }
}


//static
const NetNetwork::NetNetworkStatus& NetINetwork::currentStatus()
{
    NetNetwork::NetNetworkStatus& status = currentStatusNoRecord();

    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
    {
        status = NetIRecorder::instance().playbackCurrentStatus();
    }
    else
    {
        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
        {
            NetIRecorder::instance().recordCurrentStatus( status );
        }
    }

	return status;
}

NetNetwork::NetNetworkStatus& NetINetwork::currentStatusNoRecord()
{
	static NetNetwork::NetNetworkStatus currentStatus = NetNetwork::NETNET_OK;

	if(currentStatus == NetNetwork::NETNET_OK)
	{
/*		if(NetIAppSession::currentStatusNoRecord() != NetAppSession::NETSESS_OK)
			currentStatus = NetNetwork::NETNET_SESSIONERROR;
		else if(NetNode::currentStatusNoRecord() != NetNode::NETNODE_OK)
			currentStatus = NetNetwork::NETNET_NODEERROR;*/
	}

	return currentStatus;
}

void NetINetwork::currentStatus( NetNetwork::NetNetworkStatus status )
{
    currentStatusNoRecord() = status;
}

//static
NetAppUid NetINetwork::appUid_ = 0;


void NetINetwork::systemMessageHandler( NetSystemMessageHandler* pMessageHandler )
{
	if( pSystemMessageHandler_ )
		_DELETE( pSystemMessageHandler_ );
	pSystemMessageHandler_ = pMessageHandler;
}

void NetINetwork::doAbort( const string& reason )
{
	NETWORK_STREAM("NetINetwork::doAbort " << reason << std::endl );

}

bool NetINetwork::isLobbiedGame() const
{
    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
    {
        NetINetwork* nonConstThis = _CONST_CAST( NetINetwork*, this );

        nonConstThis->isLobbiedGame_ = NetIRecorder::instance().playbackIsLobbiedGame();
    }
    else
    {
        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
        {
            NetIRecorder::instance().recordIsLobbiedGame( isLobbiedGame_ );
        }
    }

	return isLobbiedGame_;
}

bool NetINetwork::isLogicalHost() const
{
    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
    {
        NetINetwork* nonConstThis = _CONST_CAST( NetINetwork*, this );

        nonConstThis->isLogicalHost_ = NetIRecorder::instance().playbackIsLogicalHost();
    }
    else
    {
        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
        {
            NetIRecorder::instance().recordIsLogicalHost( isLogicalHost_ );
        }
    }

	return isLogicalHost_;
}

const string& NetINetwork::localPlayerName() const
{
    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
    {
        NetINetwork* nonConstThis = _CONST_CAST( NetINetwork*, this );

        nonConstThis->localPlayerName_ = NetIRecorder::instance().playbackLocalPlayerName();
    }
    else
    {
        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
        {
            NetIRecorder::instance().recordLocalPlayerName( localPlayerName_ );
        }
    }

	return localPlayerName_;
}

void NetINetwork::localPlayerName( const string& newName )
{
	localPlayerName_ = newName;
}

NetNetwork::NetworkProtocol	NetINetwork::currentProtocol() const
{
    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
    {
        NetINetwork* nonConstThis = _CONST_CAST( NetINetwork*, this );

        nonConstThis->currentProtocol_ = NetIRecorder::instance().playbackCurrentProtocol();
    }
    else
    {
        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
        {
            NetIRecorder::instance().recordCurrentProtocol( currentProtocol_ );
        }
    }

	return currentProtocol_;
}

const string& NetINetwork::IPAddress() const
{
    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
    {
        NetINetwork* nonConstThis = _CONST_CAST( NetINetwork*, this );

        nonConstThis->IPAddress_ = NetIRecorder::instance().playbackIPAddress();
    }
    else
    {
        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
        {
            NetIRecorder::instance().recordIPAddress( IPAddress_ );
        }
    }

	return IPAddress_;
}

void NetINetwork::IPAddress( const string& newIPAddress )
{
	IPAddress_ = newIPAddress;
}

bool NetINetwork::imStuffed() const
{
    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
    {
        NetINetwork* nonConstThis = _CONST_CAST( NetINetwork*, this );

        nonConstThis->imStuffed_ = NetIRecorder::instance().playbackImStuffed();
    }
    else
    {
        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
        {
            NetIRecorder::instance().recordImStuffed( imStuffed_ );
        }
    }

	return imStuffed_;
}

bool NetINetwork::imStuffedNoRecord() const
{
	return imStuffed_;
}

bool NetINetwork::hasSystemMessageHandler() const
{
	return pSystemMessageHandler_ != NULL;
}

NetSystemMessageHandler& NetINetwork::systemMessageHandler()
{
	return *pSystemMessageHandler_;
}

bool NetINetwork::deterministicPingDropoutAllowed() const
{
	return deterministicPingDropoutAllowed_;
}

void NetINetwork::deterministicPingDropoutAllowed( bool newValue )
{
	deterministicPingDropoutAllowed_ = newValue;
}

bool operator<( const NetMessageShortInfo& a, const NetMessageShortInfo& b )
{
	if( a.time_ < b.time_ )
		return true;
	if( a.time_ != b.time_ )
		if( a.length_ < b.length_ )
			return true;
	return false;
}
bool operator==( const NetMessageShortInfo& a, const NetMessageShortInfo& b )
{
	if( a.time_ == b.time_ )
		return true;
	if( a.time_ != b.time_ )
		if( a.length_ == b.length_ )
			return true;
	return false;
}

void NetINetwork::addSentMessage( int length )
{
	double time = timer_.time();
	NetMessageShortInfo* pInfo = _NEW( NetMessageShortInfo );
	pInfo->time_ = time;
	pInfo->length_ = length;
	sentMessages_.push_back( pInfo );
	computeSentMessageStuffedNess();
}

void NetINetwork::computeSentMessageStuffedNess()
{
	if( messageThrottlingActive_ )
	{
		double time = timer_.time();
		double targetTime = time - 1.0;
		SentMessages messagesForDeletion;
		//Now iterate through list and remove any elements older than one second from now.
		for( SentMessages::iterator i = sentMessages_.begin(); i != sentMessages_.end(); ++i )
			if( (*i)->time_ < targetTime )
				messagesForDeletion.push_back( *i );
		for( SentMessages::iterator i = messagesForDeletion.begin(); i != messagesForDeletion.end(); ++i )
		{
			sentMessages_.erase( find( sentMessages_.begin(), sentMessages_.end(), *i ) );
			_DELETE( *i );
		}
		//Now iterator through the current collection and add up all the lengths - thats the current bandwidth usage
		//if it goes above maxBytesPerSecond_ bytes sec -1 then set stuffed to true.
		int totalLength = 0;
		for( SentMessages::iterator i = sentMessages_.begin(); i != sentMessages_.end(); ++i )
			totalLength += (*i)->length_;
		if( totalLength > maxBytesPerSecond_ or sentMessages_.size() > maxSentMessagesPerSecond() )
			imStuffed_ = true;
	}
//	NETWORK_STREAM("NetINetwork::computeSentMessageStuffedNess No. messages " << sentMessages_.size() << " totalLength " << totalLength << " maxBytesPerSecond " << maxBytesPerSecond_ << " maxSentMessagesPerSecond() " << maxSentMessagesPerSecond() << std::endl );
}


double NetINetwork::deterministicTimeoutPeriod() const
{
	//5 minutes allowed before IPX will blow up.
	if( currentProtocol() == NetNetwork::IPX )
		return 300;
	//10 minutes of lag allowed for Zone type games
	return 600;
}

size_t NetINetwork::maxSentMessagesPerSecond() const
{
	static bool initialisedFromRegistry = false;
	if( not initialisedFromRegistry )
	{
		NetINetwork *pMe = _CONST_CAST( NetINetwork *, this );
		bool fromLan = ( currentProtocol() == NetNetwork::IPX );
		string keyValue = "Max packets per second";

		if( fromLan )
			keyValue += " (IPX)";

		int possibleValue = SysRegistry::instance().queryIntegerValue("Network", keyValue );

		if( possibleValue == 0 )
		{
			if( fromLan )
				pMe->maxSentMessagesPerSecond_ = 100;
			else
				pMe->maxSentMessagesPerSecond_ = 40;
			SysRegistry::instance().setIntegerValue("Network", keyValue, maxSentMessagesPerSecond_ );
			possibleValue = maxSentMessagesPerSecond_;
		}

		if( possibleValue > 400 )
			possibleValue = 400;
		else if( possibleValue < 40 )
			possibleValue = 40;
		pMe->maxSentMessagesPerSecond_ = possibleValue;

		keyValue = "Max bytes per second";
		if( fromLan )
			keyValue += " (IPX)";
		possibleValue = SysRegistry::instance().queryIntegerValue("Network", keyValue );

		if( possibleValue == 0 )
		{
			if( fromLan )
				pMe->maxBytesPerSecond_ = 60000;
			else
				pMe->maxBytesPerSecond_ = 6000;
			SysRegistry::instance().setIntegerValue("Network", keyValue, maxBytesPerSecond_ );
			possibleValue = maxBytesPerSecond_;
		}
		if( possibleValue > 60000 )
			possibleValue = 60000;
		else if( possibleValue < 6000 )
			possibleValue = 6000;
		pMe->maxBytesPerSecond_ = possibleValue;

		NETWORK_STREAM("NetINetwork::maxSentMessagesPerSecond initialisation maxPackets " << maxSentMessagesPerSecond_ << " max Bytes " << maxBytesPerSecond_ << std::endl );

		initialisedFromRegistry = true;

		pMe->originalMaxSentMessagesPerSecond_ = maxSentMessagesPerSecond_;
	}


	return maxSentMessagesPerSecond_;
}

void NetINetwork::disableNewPlayers()
{
	NETWORK_STREAM("NetINetwork::disableNewPlayers()\n");
}

bool NetINetwork::pingAllAllowed() const
{
	return pingAllAllowed_;
}


void NetINetwork::determineStandardSendFlags()
{
	NETWORK_STREAM("NetINetwork::determineStandardSendFlags\n" );
	NETWORK_INDENT( 2 );
	NETWORK_INDENT( -2 );
}

void NetINetwork::messageThrottlingActive( bool value )
{
	messageThrottlingActive_ = value;
}

bool NetINetwork::messageThrottlingActive() const
{
	return messageThrottlingActive_;
}

void NetINetwork::autoAdjustMaxSentMessagesPerSecond( size_t numberOfPlayers )
{
	NETWORK_STREAM("NetINetwork::autoAdjustMaxSentMessagesPerSecond " << numberOfPlayers << std::endl );
	//ensure values are correctly initialised from the registry
	maxSentMessagesPerSecond();

	int thresholdValue = 50;

	if( currentProtocol() != NetNetwork::IPX )
	{
		thresholdValue = 40;
	}

	maxSentMessagesPerSecond_ = originalMaxSentMessagesPerSecond_;
	//Check for lower threshold value up front.
	if( maxSentMessagesPerSecond_ <= thresholdValue )
		return;
	if( numberOfPlayers < 3 )
		return;
	maxSentMessagesPerSecond_ *= ( 2.0f / numberOfPlayers );
	//Check for lowest threshold value.
	if( maxSentMessagesPerSecond_ < thresholdValue )
		maxSentMessagesPerSecond_ = thresholdValue;
	NETWORK_STREAM(" maxSentMessagesPerSecond " << maxSentMessagesPerSecond_ << " orig " << originalMaxSentMessagesPerSecond_ << " threshold " << thresholdValue << std::endl );
	NETWORK_STREAM("NetINetwork::autoAdjustMaxSentMessagesPerSecond DONE\n" );
}
