
#include "profiler/profiler.hpp"
#include "system/winapi.hpp"

#include "network/node.hpp"
#include "network/nodeuid.hpp"
#include "network/sessuid.hpp"
#include "network/session.hpp"
#include "network/message.hpp"
#include "network/sysmess.hpp"

#include "network/internal/mapping.hpp"
#include "network/internal/sessioni.hpp"
#include "network/internal/netinet.hpp"
#include "network/internal/recorder.hpp"
#include "network/internal/compound.hpp"
#include "network/internal/nodei.hpp"

#include "recorder/recorder.hpp"

//extern void outputDPErrorToStream( ostream& o, char* sFormat, HRESULT hr );


//static
NetNode::NetNodeStatus& NetNode::currentStatus()
{
	return currentStatusNoRecord();
}

//static
NetNode::NetNodeStatus& NetNode::currentStatusNoRecord()
{
	static NetNodeStatus currentStatus = NETNODE_OK;
	return currentStatus;
}

#define CB_NetNode_DEPIMPL() \
		PRE( pImpl_ ); \
		CB_DEPIMPL( NetNodeUid*, 			pNodeUid_ ); \
		CB_DEPIMPL( NetAppSessionUid*,		pAppSessionUid_ ); \
		CB_DEPIMPL( NetMessageBuffer,		messageBuffer_ ); \
		CB_DEPIMPL( bool, 					acceptingPing_ ); \
		CB_DEPIMPL( const string, 			pingString_ ); \
		CB_DEPIMPL( NetNode::Ping,			ping_ ); \
		CB_DEPIMPL( double,					lastPingAllTime_ ); \
		CB_DEPIMPL( double,					lastCompoundTransmitTime_ ); \
		CB_DEPIMPL( DevTimer,				timer_ ); \
		CB_DEPIMPL( NetCompoundMessage*,	pCompoundMessage_ ); \
		CB_DEPIMPL( ctl_pvector< NetCompoundMessage >,	cachedCompoundMessages_ );


NetNode::NetNode( const NetNodeName& name )
:  pImpl_( _NEW( NetNodeImpl ) )
{
	CB_NetNode_DEPIMPL();

	PRE( isValidNoRecord() );
	pNodeUid_ = NULL;
	pAppSessionUid_ = NULL;
	acceptingPing_ = true;
	lastPingAllTime_ = 0;
	lastCompoundTransmitTime_ = 0;
	pCompoundMessage_ = NULL;

	//Create our default AppSession
	NetAppSession& thisAppSession = NetNetwork::instance().session();
	NetNetwork::instance().netINetwork().localPlayerName( name );
	pAppSessionUid_ = _NEW(NetAppSessionUid(thisAppSession.appSessionUid()));

    if( RecRecorder::instance().state() != RecRecorder::PLAYING )
    {
        RecRecorder::instance().recordingAllowed( false );

        unsigned uId = 0xf;

    	pNodeUid_ = _NEW(NetNodeUid(NetNetwork::instance().processUidNoRecord(), uId, name));

    	const NetProcessUid& temp = NetNetwork::instance().processUidNoRecord();

    	size_t dataSize = sizeof(int)+temp.processorUid().length()+1;
    	NetThreadUid* pPlayerData = _NEW_ARRAY(NetThreadUid, dataSize);
    	pPlayerData[0] = temp.threadUid();
    	memcpy((pPlayerData+1), temp.processorUid().c_str(), temp.processorUid().length()+1);

    	_DELETE_ARRAY(pPlayerData);

    	currentStatus() = convertStatus(0);
    	ASSERT(isValid(), "Invalid data setting");

    	NetNetwork::instance().pNetINetwork_->addLocalNode(this);

        RecRecorder::instance().recordingAllowed( true );
    }

    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
    {
        pNodeUid_ = _NEW( NetNodeUid( NetIRecorder::instance().playbackNodeUid() ) );
    }
    else
    {
        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
        {
            ASSERT( pNodeUid_ != NULL, "" );
            NetIRecorder::instance().recordNodeUid( *pNodeUid_ );
        }
    }

   	POST(pNodeUid_);
	POST( isValidNoRecord() );
}


//virtual
NetNode::~NetNode()
{
//	PRE( isValidNoRecord() );
	CB_NetNode_DEPIMPL();
	NETWORK_STREAM("NetNode::~NetNode() id " << pNodeUid_ << " network nodes size " << NetNetwork::instance().pNetINetwork_->nodes_.size() << std::endl );
    if( RecRecorder::instance().state() != RecRecorder::PLAYING )
    {
		NetAppSession& thisAppSession = NetNetwork::instance().session();
        RecRecorder::instance().recordingAllowed( false );

		useCompoundMessaging( false );
		//Cancel or async messages.
		
    	removeFromNetwork();

        RecRecorder::instance().recordingAllowed( true );
    }

	_DELETE(pNodeUid_);
	_DELETE(pAppSessionUid_);

	NETWORK_STREAM("NetNode::~NetNode() DONE\n" );
	//	PRE( isValidNoRecord() );
}

///////////////////////////////

const NetNodeUid& NetNode::nodeUid() const
{
	CB_NetNode_DEPIMPL();
	PRE(pNodeUid_);

	return (*pNodeUid_);
}

NetNodeUid* NetNode::pNodeUid()
{
	CB_NetNode_DEPIMPL();
	PRE( isValidNoRecord() );
	PRE(pNodeUid_);

	return pNodeUid_;
}

const NetAppSessionUid& NetNode::appSessionUid() const
{
	CB_NetNode_DEPIMPL();
	PRE( isValidNoRecord() );
	PRE(pAppSessionUid_);

	return (*pAppSessionUid_);
}

//virtual
void NetNode::ping( const NetNodeUid& recipientUid )
{
	CB_NetNode_DEPIMPL();
  	double thisTime = timer_.time();

    if( RecRecorder::instance().state() != RecRecorder::PLAYING )
    {
        RecRecorder::instance().recordingAllowed( false );

    	PRE( isValidNoRecord() );
    	PRE(recipientUid != (*pNodeUid_));
    	if( ping_.find( recipientUid ) == ping_.end() )
    	{
    		NetPingInformation pingInfo;
    		pingInfo.lastPingTime_ = 0;
    		pingInfo.lastTimePingAsked_ = 0;
    		pingInfo.lastProperPingTime_ = 0;
    		ping_.insert( recipientUid, pingInfo );
    		NETWORK_STREAM("NetNode::ping inserted pingInfo entry for " << recipientUid << std::endl );
    	}

    	NETWORK_STREAM("ping_.size " << ping_.size() << std::endl );
      	NETWORK_STREAM("Ping Map\n" );
    	for( NetNode::Ping::const_iterator i = ping_.begin(); i != ping_.end() ; ++i )
    	{
    		NETWORK_STREAM("Ping map Key: " << (*i).first << std::endl << "Ping map value: " << (*i).second << std::endl );
    	}
    	ASSERT( ping_.find( recipientUid ) != ping_.end(),"" );
    	NetPingInformation& pingInfo = ping_[ recipientUid ];

    	//only ping if we don't have an outstanding ping request.
    	//or it has been 2.5 seconds since the last one.
    	double timeDiff = thisTime - pingInfo.lastTimePingAsked_;
    	bool timeForNewPing = timeDiff > 2.5 or timeDiff < 0;
    	NETWORK_STREAM("NetNode::ping " << recipientUid.nodeName() << " " << pingInfo << std::endl );
    	NETWORK_STREAM("lastPingTime " << pingInfo.lastPingTime_ << " timeForNewPing " << timeForNewPing << "( thisTime " << thisTime << " lastTimePingAsked_ " << pingInfo.lastTimePingAsked_ << ") " << std::endl );
    	if( pingInfo.lastPingTime_ != -1 or timeForNewPing )
    	{
    		NETWORK_STREAM(" Doing ping " << recipientUid.nodeName() << std::endl );
    		pingInfo.lastTimePingAsked_ = thisTime;

    		NetAppSession& thisAppSession = NetNetwork::instance().session();
    		pingInfo.lastProperPingTime_ = pingInfo.lastPingTime_;
    		pingInfo.lastPingTime_ = -1;


			//Can we detect a drop out then directx will not necessariliy get....
			//We will time out if the ping times are greater than NetNetwork::instance().netINetwork().deterministicTimeoutPeriod()
			if( NetNetwork::instance().deterministicPingDropoutAllowed() )
			{
				double deterministicDropOut = NetNetwork::instance().netINetwork().deterministicTimeoutPeriod();
				if( pingInfo.lastProperPingTime_ == -1 and ( pingInfo.lastTimePingAsked_ - pingInfo.lastTimePingSet_ ) > deterministicDropOut )
				{
	    			NETWORK_STREAM("NetNode::ping returned connection lost (Deterministic)\n" );
	    			if( NetNetwork::instance().hasSystemMessageHandler() )
	    			{
	    				NETWORK_STREAM(" calling systemMessageHandler.playerHasBeenLost\n" );
	    				NetNetwork::instance().systemMessageHandler().playerHasBeenLost( recipientUid.nodeName() );
	    			}
				}
			}
//    		_DELETE_ARRAY(pBuffer);
    	}

        RecRecorder::instance().recordingAllowed( true );
    }

	PRE( isValidNoRecord() );
}

//virtual
void NetNode::sendMessage( const NetPriority& priority, const NetMessage::NetMessageRecipients& /*to*/, const NetMessageBody& body)
{
	CB_NetNode_DEPIMPL();
	if( pCompoundMessage_ )
	{
		if( not pCompoundMessage_->addNetMessageBody( body ) )
		{
			//Implicit destruction of contents inside addCachedCompoundMessage
			addCachedCompoundMessage( pCompoundMessage_ );
			pCompoundMessage_->addNetMessageBody( body );
		}
	}
	else
	{
	    //  This line must stay outside the if in order for the recording to work properly.
		NetNetwork::NetworkProtocol currentProtocol = NetNetwork::instance().currentProtocol();
		//Need to get status of nodes.
		NetNetwork::instance().pollMessages();

	    if( RecRecorder::instance().state() != RecRecorder::PLAYING )
	    {
	        RecRecorder::instance().recordingAllowed( false );

	        RecRecorder::instance().recordingAllowed( true );
	    }
	}
}

void NetNode::acceptPing()
{
	CB_NetNode_DEPIMPL();
	acceptingPing_ = true;
}

void NetNode::refusePing()
{
	CB_NetNode_DEPIMPL();
	acceptingPing_ = false;
}

bool NetNode::isAcceptingPing() const
{
	CB_NetNode_DEPIMPL();
	return acceptingPing_;
}

int NetNode::lastPingTime() const
{
	CB_NetNode_DEPIMPL();
	return 0;
}

//virtual
void NetNode::receiveMessage( NetMessage* message )
{
	CB_NetNode_DEPIMPL();
    PRE( RecRecorder::instance().state() != RecRecorder::PLAYING )

	PRE( isValidNoRecord() );
	PRE(message);

	bool queueMessage = true;
	//check for a message raised by the network library
	if( message->body().body()[0] == NETWORK_LIBRARY_MESSAGE )
		queueMessage = false;


	if(queueMessage)
	{
		messageBuffer_.push_back(message);
		NetMessage* pFrontMessage = messageBuffer_.front();
	}
	else
	{
		_DELETE( message );
	}

	PRE( isValidNoRecord() );
}

//virtual
bool NetNode::haveMessages()
{
	CB_NetNode_DEPIMPL();
    recorderUpdateMessageBuffer();

	return (messageBuffer_.size() > 0);
}

//virtual
NetMessage* NetNode::getMessage()
{
	CB_NetNode_DEPIMPL();
    recorderUpdateMessageBuffer();

	PRE( isValidNoRecord() );

	NetMessage* pFrontMessage = messageBuffer_.front();
	messageBuffer_.erase(messageBuffer_.begin());

	POST(pFrontMessage);
	PRE( isValidNoRecord() );
	return pFrontMessage;
}

//virtual
NetMessage* NetNode::getMessage(size_t index)
{
	CB_NetNode_DEPIMPL();
    recorderUpdateMessageBuffer();

	PRE( isValidNoRecord() );
	PRE(index < messageQueueSizeNoRecord());

	NetMessage* pIndexedMessage = messageBuffer_[index];
	NetMessageBuffer::iterator i = (messageBuffer_.begin() + index);
	messageBuffer_.erase(i);

	POST(pIndexedMessage);
	POST( isValidNoRecord() );
	return pIndexedMessage;
}

size_t NetNode::messageQueueSize()
{
	CB_NetNode_DEPIMPL();
	PRE( isValidNoRecord() );

    recorderUpdateMessageBuffer();

	return messageBuffer_.size();
}

size_t NetNode::messageQueueSizeNoRecord()
{
	CB_NetNode_DEPIMPL();
	PRE( isValidNoRecord() );

	return messageBuffer_.size();
}

//virtual
NetMessage* NetNode::peekMessage()
{
	CB_NetNode_DEPIMPL();
	PRE( isValidNoRecord() );

    recorderUpdateMessageBuffer();

	return messageBuffer_.front();
}

//virtual
NetMessage* NetNode::peekMessage(size_t index)
{
	CB_NetNode_DEPIMPL();
    recorderUpdateMessageBuffer();

	PRE( isValidNoRecord() );
	PRE(index < messageQueueSizeNoRecord());

	return messageBuffer_[index];
}

void NetNode::CLASS_INVARIANT
{
#ifndef NDEBUG
	bool enterThis = false;

	if(enterThis)
	{
		std::cout << "Error at " << file << " line " << line << std::endl;
		ASSERT(false, "");
	}
#endif //NDEBUG
//	NetNetwork::instance().session( appSessionUid() ).hasMember( nodeUid() );
}

////////////////PRIVATE/////////////////

//NetMessageUid NetNode::nextMessageUidFor( NetNodeUid toNode )
//{
//// PRE( toNode != nodeUid() );
//	NetMessageUid	nextId;
//	return nextId;
//}

//NetAppSessionUid NetNode::createDefaultAppSession( const NetAppSessionName& name )
//{
//	NetAppSession* result = NetNetwork::instance().createAppSession( name, this );
//	return result->appSessionUid();
//}

void NetNode::addToNetwork()
{
	PRE( isValidNoRecord() );
//	NetNetwork::instance().addLocalNode( this );
}

void NetNode::removeFromNetwork()
{
	CB_NetNode_DEPIMPL();
	PRE( isValidNoRecord() );

    if( RecRecorder::instance().state() != RecRecorder::PLAYING )
    	NetNetwork::instance().pNetINetwork_->removeLocalNode( this );

	PRE( isValidNoRecord() );
}

void NetNode::leaveAppSession()
{
	CB_NetNode_DEPIMPL();
	PRE( isValidNoRecord() );

    if( RecRecorder::instance().state() != RecRecorder::PLAYING )
    {
        RecRecorder::instance().recordingAllowed( false );

    	NetAppSession& thisAppSession = NetNetwork::instance().session();
        RecRecorder::instance().recordingAllowed( true );
    }

	PRE( isValidNoRecord() );
}

void NetNode::enterAppSession( NetAppSessionUid aUid )
{
	CB_NetNode_DEPIMPL();
	PRE( isValidNoRecord() );

    if( RecRecorder::instance().state() != RecRecorder::PLAYING )
    	enterAppSession(pNodeUid_->nodeName(), aUid);

	PRE( isValidNoRecord() );
}

//NetNetwork::MethodReturnStatus NetNode::enterAppSession(  const NetNodeName& name, NetAppSessionUid aUid )
void NetNode::enterAppSession(  const NetNodeName& name, NetAppSessionUid aUid )
{
	CB_NetNode_DEPIMPL();
	PRE( isValidNoRecord() );

    if( RecRecorder::instance().state() != RecRecorder::PLAYING )
    {
        RecRecorder::instance().recordingAllowed( false );

        RecRecorder::instance().recordingAllowed( true );
    }

	PRE( isValidNoRecord() );
}

bool NetNode::isValid() const
{
	return (currentStatus() == NETNODE_OK);
}

bool NetNode::isValidNoRecord() const
{
	return (currentStatusNoRecord() == NETNODE_OK);
}

NetNode::NetNodeStatus NetNode::convertStatus(unsigned int castThis) const
{
    NetNodeStatus returnVal = NETNODE_OK;

	if( returnVal != NETNODE_OK )
	{
//		NETWORK_STREAM( "NetNode:: convertStatus : return code is " << returnVal << " dperror " << hr << " Stack:" << std::endl );
	    ProProfiler::instance().traceStack( Diag::instance().networkStream(), true, 0, "NetNode::ConvertStatus" );
		NETWORK_STREAM( "NetNode:: convertStatus Done " << std::endl );

	}

	return returnVal;
}


ostream& operator<<( ostream& o, const NetNode::NetNodeStatus& t )
{
	switch( t )
	{
		case NetNode::NETNODE_OK: o << "NETNODE_OK"; break;
		case NetNode::NETNODE_UNDEFINEDERROR: o << "NETNODE_UNDEFINEDERROR"; break;
		case NetNode::NETNODE_PLAYERCREATIONERROR: o << "NETNODE_PLAYERCREATIONERROR"; break;
		case NetNode::NETNODE_INTERNALDATAERROR: o << "NETNODE_INTERNALDATAERROR"; break;
		case NetNode::NETNODE_INVALIDINTERNALDATA: o << "NETNODE_INVALIDINTERNALDATA"; break;
		case NetNode::NETNODE_NETWORKERROR: o << "NETNODE_NETWORKERROR"; break;
		case NetNode::NETNODE_INVALIDSUPPLIEDDATA: o << "NETNODE_INVALIDSUPPLIEDDATA"; break;
		case NetNode::NETNODE_NETWORKACCESSREFUSED: o << "NETNODE_NETWORKACCESSREFUSED"; break;
		case NetNode::NETNODE_MESSAGETOOBIG: o << "NETNODE_MESSAGETOOBIG"; break;
		case NetNode::NETNODE_INVALIDPLAYER: o << "NETNODE_INVALIDPLAYER"; break;
		case NetNode::NETNODE_PLAYERDOESNOTEXIST: o << "NETNODE_PLAYERDOESNOTEXIST"; break;
		case NetNode::NETNODE_CONNECTIONLOST: o << "NETNODE_CONNECTIONLOST"; break;
		case NetNode::NETNODE_UNSUPPORTED: o << "NETNODE_UNSUPPORTED"; break;
	}
	return o;
}

void NetNode::pingAll()
{
	CB_NetNode_DEPIMPL();
  	if( NetNetwork::instance().imStuffed() )
   		return;
	if( not NetNetwork::instance().netINetwork().pingAllAllowed() )
		return;

  	NetAppSession& thisAppSession = NetNetwork::instance().session();

    if( RecRecorder::instance().state() != RecRecorder::PLAYING )
    {
        RecRecorder::instance().recordingAllowed( false );

    	NETWORK_STREAM("NetNode::pingAll\n" );

        RecRecorder::instance().recordingAllowed( true );
    }
	//Make sure node hasn't disappeared.
	NetNetwork::instance().pollMessages();

   	const NetAppSession::NodeIds& nodes = NetNetwork::instance().session().nodes();

   	NetAppSession::NodeIds::const_iterator i = nodes.begin();
   	NetAppSession::NodeIds::const_iterator j = nodes.end();

	for( ; i != j; ++i )
	{
		if( (**i) != *pNodeUid_ )
			ping( (**i) );
	}

   	lastPingAllTime_ = timer_.time();
}

const NetNode::Ping&	NetNode::pingInformation() const
{
	CB_NetNode_DEPIMPL();
    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
    {
        NetNode* nonConstThis = _CONST_CAST( NetNode*, this );

        nonConstThis->pImpl_->ping_ = NetIRecorder::instance().playbackPing();
    }
    else
    {
        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
        {
            NetIRecorder::instance().recordPing( ping_ );
        }
    }

	return ping_;
}

double NetNode::lastPingAllTime() const
{
	CB_NetNode_DEPIMPL();
    //  The call to time must be made here otherwise the recording breaks
    double r = timer_.time() - lastPingAllTime_;

    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
    {
        r = NetIRecorder::instance().playbacklastPingAllTime();
    }
    else
    {
        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
        {
            NetIRecorder::instance().recordLastPingAllTime( r );
        }
    }

//	NETWORK_STREAM("NetNode::lastPingAllTime() " << r << std::endl );
	return r;
}

ostream& operator<<( ostream& o, const NetNode::NetPingInformation& pi )
{
	o << "lastTimePingAsked  " << pi.lastTimePingAsked_ << std::endl;
	o << "lastTimePingSet    " << pi.lastTimePingSet_ << std::endl;
	o << "lastPingTime       " << pi.lastPingTime_ << std::endl;
	o << "lastProperPingTime " << pi.lastProperPingTime_ << std::endl;
	return o;
}

void NetNode::recorderUpdateMessageBuffer()
{
	CB_NetNode_DEPIMPL();
    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
    {
        NetIRecorder::instance().playbackMessageBuffer( &messageBuffer_ );
    }
    else
    {
        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
        {
            NetIRecorder::instance().recordMessageBuffer( messageBuffer_ );
        }
    }
}

void NetNode::useCompoundMessaging( bool value )
{
	CB_NetNode_DEPIMPL();
	NETWORK_STREAM("NetNode::useCompoundMessaging " << value << std::endl );
	//Do we want a compound messaging system?
	if( value )
	{
		NETWORK_STREAM(" activating\n" );
		//Yes but not already got one
		if( not pCompoundMessage_ )
		{
//			size_t bufferSize = NetNetwork::instance().netINetwork().currentProtocolDPCAPS_.dwMaxBufferSize;
//			if( bufferSize > 1024 )
//				bufferSize = 1024;
//			pCompoundMessage_ = _NEW( NetCompoundMessage( bufferSize ) );
		}
	}
	else
	{
		NETWORK_STREAM(" destroying\n" );
		//do we have one?
		if( pCompoundMessage_ )
		{
			transmitCompoundMessage();
			_DELETE( pCompoundMessage_ );
			pCompoundMessage_ = NULL;
		}
	}
}

void NetNode::addCachedCompoundMessage( NetCompoundMessage* pCompoundMessage )
{
	CB_NetNode_DEPIMPL();
	//Copy message...
	NetCompoundMessage* pNewCompound = _NEW( NetCompoundMessage( pCompoundMessage->buffer() ) );
	//Clear old message
	pCompoundMessage->clearAll();
	cachedCompoundMessages_.push_back( pNewCompound );
	//Do we have more messages than we could possibly transmit in one second?
	NetINetwork& netINetwork = NetNetwork::instance().netINetwork();
	if( cachedCompoundMessages_.size() > netINetwork.maxSentMessagesPerSecond() )
	{
		NetNetwork::instance().imStuffed( true );
	}
}

void NetNode::transmitCompoundMessage()
{
	CB_NetNode_DEPIMPL();
	//last transmit time...
	NetINetwork& netINetwork = NetNetwork::instance().netINetwork();
	double minimumTransmitRateTimeGap = 1.0 / (double)netINetwork.maxSentMessagesPerSecond_;
	if( timer_.time() - lastCompoundTransmitTime_ < minimumTransmitRateTimeGap )
	{
		return;
	}

	//Check to see if we have any pending compounds to go out.
	//if we become stuffed at any time then we will exit this function
	if( not netINetwork.imStuffed() )
	{
		if( cachedCompoundMessages_.size() )
		{
			while( cachedCompoundMessages_.size() and not netINetwork.imStuffed() )
			{
				NetCompoundMessage*	pCompoundMessage = cachedCompoundMessages_.front();
				actuallyTransmitCompoundMessage( pCompoundMessage );
				//Remove the memory associated with this object
				_DELETE( pCompoundMessage );
				cachedCompoundMessages_.erase( cachedCompoundMessages_.begin() );
			}
		//Update time if we do some work
		lastCompoundTransmitTime_ = timer_.time();
		}
		else
		{
			if( pCompoundMessage_ and pCompoundMessage_->subMessages() > 0 )
			{
				actuallyTransmitCompoundMessage( pCompoundMessage_ );
				pCompoundMessage_->clearAll();
				//Update time if we do some work
				lastCompoundTransmitTime_ = timer_.time();
			}
		}
	}

//	while( not netINetwork.imStuffed() and cachedCompoundMessages_.size() )
//	{
//		NetCompoundMessage*	pCompoundMessage = cachedCompoundMessages_.front();
//		actuallyTransmitCompoundMessage( pCompoundMessage );
//		//Remove the memory associated with this object
//		_DELETE( pCompoundMessage );
//		cachedCompoundMessages_.erase( cachedCompoundMessages_.begin() );
//	}
}

void NetNode::actuallyTransmitCompoundMessage( NetCompoundMessage* pCompoundMessage )
{
	CB_NetNode_DEPIMPL();
	NetMessageBody message( pCompoundMessage->buffer(), pCompoundMessage->length() );

	//Go get the list of other nodes to send to and send through a bodgy priority (priorities
	//in compound messages are not yet supported - might do it one day).
	NetPriority priority( 1 );

	NetMessage::NetMessageRecipients to;
	NetAppSession::NodeIds::const_iterator i = NetNetwork::instance().session().nodes().begin();
	NetAppSession::NodeIds::const_iterator j = NetNetwork::instance().session().nodes().end();
	for( ; i != j; ++i )
		to.push_back( (*i) );

	//This is REALLY hacky - i'm setting the pointer to NULL here so that it will go through sendMessage correctly.
	//God what a bodge!!
	//(Excuse - No time - week until machines goes out and the demo falls over on the Zone with four players).
	NetCompoundMessage* pSaveAddress = pCompoundMessage_;
	pCompoundMessage_ = NULL;
	//Do the actual send here
	sendMessage( priority, to, message );
	pCompoundMessage_ = pSaveAddress;
	NET_ANALYSIS_STREAM("COMPOUND,out," << NetNetwork::instance().netINetwork().timer_.time() << "," << pCompoundMessage->length() << "," << (int)pCompoundMessage->subMessages() << std::endl );
}


const ctl_pvector< NetCompoundMessage >& NetNode::cachedCompoundMessages() const
{
	CB_NetNode_DEPIMPL();
	return cachedCompoundMessages_;
}
