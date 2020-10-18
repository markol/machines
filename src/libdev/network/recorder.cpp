/*
 * R E C O R D E R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "network/internal/recorder.hpp"
#include "recorder/private/recpriv.hpp"

// static
NetIRecorder& NetIRecorder::instance()
{
    static NetIRecorder instance_;
    return instance_;
}

NetIRecorder::NetIRecorder()
{

    TEST_INVARIANT;
}

NetIRecorder::~NetIRecorder()
{
    TEST_INVARIANT;

}

void NetIRecorder::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

NetProcessUid NetIRecorder::playbackProcessUid() const
{
    NetProcessUid uid;

    //RecRecorderPrivate::instance().playbackNetworkData( _STATIC_CAST( uint8*, &uid ), sizeof( uid ) );
    RecRecorderPrivate::instance().playbackNetworkData( _REINTERPRET_CAST( uint8*, &uid ), sizeof( uid ) );

    return uid;
}

void NetIRecorder::recordProcessUid( NetProcessUid uid ) const
{
    //RecRecorderPrivate::instance().recordNetworkData( _STATIC_CAST( const uint8*, &uid ), sizeof( uid ) );
    RecRecorderPrivate::instance().recordNetworkData( _REINTERPRET_CAST( const uint8*, &uid ), sizeof( uid ) );
}

bool NetIRecorder::playbackHasAppSession() const
{
    return RecRecorderPrivate::instance().playbackNetworkBool();
}

void NetIRecorder::recordHasAppSession( bool value ) const
{
    RecRecorderPrivate::instance().recordNetworkBool( value );
}

void NetIRecorder::playbackSessions( NetNetwork::Sessions* pSessions ) const
{
    //  Delete all existing sessions
    for( NetNetwork::Sessions::iterator i = pSessions->begin();
	  i != pSessions->end(); ++i )
  	{
   		_DELETE( *i );
   	}

	pSessions->erase( pSessions->begin(), pSessions->end() );

    //  Now read in the current set of sessions

    const uint nSessions = RecRecorderPrivate::instance().playbackNetworkUint();

    for( size_t i = 0; i < nSessions; ++i )
    {
        pSessions->push_back( _NEW( NetAppSessionUid( playbackAppSessionUid() ) ) );
    }
}

void NetIRecorder::recordSessions( const NetNetwork::Sessions& sessions ) const
{
    RecRecorderPrivate::instance().recordNetworkUint( sessions.size() );

    for( size_t i = 0; i < sessions.size(); ++i )
    {
        recordAppSessionUid( *sessions[ i ] );
    }
}

NetAppUid NetIRecorder::playbackAppUid() const
{
    NetAppUid uid;
    // replaced here to reinterpret cast
    RecRecorderPrivate::instance().playbackNetworkData( _REINTERPRET_CAST( uint8*, &uid ), sizeof( uid ) );

    return uid;
}

void NetIRecorder::recordAppUid( NetAppUid uid ) const
{
    // and here...
    RecRecorderPrivate::instance().recordNetworkData( _REINTERPRET_CAST( const uint8*, &uid ), sizeof( uid ) );
}

bool NetIRecorder::playbackIsLogicalHost() const
{
    return RecRecorderPrivate::instance().playbackNetworkBool();
}

void NetIRecorder::recordIsLogicalHost( bool value ) const
{
    RecRecorderPrivate::instance().recordNetworkBool( value );
}

string NetIRecorder::playbackLocalPlayerName() const
{
    return RecRecorderPrivate::instance().playbackNetworkString();
}

void NetIRecorder::recordLocalPlayerName( const string& str ) const
{
    RecRecorderPrivate::instance().recordNetworkString( str );
}

bool NetIRecorder::playbackHasLocalNode() const
{
    return RecRecorderPrivate::instance().playbackNetworkBool();
}

void NetIRecorder::recordHasLocalNode( bool value ) const
{
    RecRecorderPrivate::instance().recordNetworkBool( value );
}

void NetIRecorder::playbackAvailableProtocols( NetNetwork::ProtocolMap* pMap ) const
{
    NetNetwork::ProtocolMap map;

    const uint size = RecRecorderPrivate::instance().playbackNetworkUint();

    for( size_t i = 0; i < size; ++i )
    {
        string str = RecRecorderPrivate::instance().playbackNetworkString();
        int ii = RecRecorderPrivate::instance().playbackNetworkInt();

        map.insert( str, ii );
    }

    *pMap = map;
}

void NetIRecorder::recordAvailableProtocols( const NetNetwork::ProtocolMap& protocolMap ) const
{
    RecRecorderPrivate::instance().recordNetworkUint( protocolMap.size() );

    for( NetNetwork::ProtocolMap::const_iterator i = protocolMap.begin();
      i != protocolMap.end(); ++i )
    {
        RecRecorderPrivate::instance().recordNetworkString( (*i).first );
        RecRecorderPrivate::instance().recordNetworkInt( (*i).second );
    }
}

string NetIRecorder::playbackCurrentProtocolName() const
{
    return RecRecorderPrivate::instance().playbackNetworkString();
}

void NetIRecorder::recordCurrentProtocolName( const string& str ) const
{
    RecRecorderPrivate::instance().recordNetworkString( str );
}

NetNetwork::NetNetworkStatus NetIRecorder::playbackCurrentStatus() const
{
    NetNetwork::NetNetworkStatus status;

    RecRecorderPrivate::instance().playbackNetworkData( _REINTERPRET_CAST( uint8*, &status ), sizeof( status ) );

    return status;
}

void NetIRecorder::recordCurrentStatus( const NetNetwork::NetNetworkStatus& status ) const
{
    RecRecorderPrivate::instance().recordNetworkData( _REINTERPRET_CAST( const uint8*, &status ), sizeof( status ) );
}

bool NetIRecorder::playbackIsLobbiedGame() const
{
    return RecRecorderPrivate::instance().playbackNetworkBool();
}

void NetIRecorder::recordIsLobbiedGame( bool value ) const
{
    RecRecorderPrivate::instance().recordNetworkBool( value );
}

NetNetwork::NetworkProtocol NetIRecorder::playbackCurrentProtocol() const
{
    NetNetwork::NetworkProtocol protocol;

    RecRecorderPrivate::instance().playbackNetworkData( _REINTERPRET_CAST( uint8*, &protocol ), sizeof( protocol ) );

    return protocol;
}

void NetIRecorder::recordCurrentProtocol( const NetNetwork::NetworkProtocol& protocol ) const
{
    RecRecorderPrivate::instance().recordNetworkData( _REINTERPRET_CAST( const uint8*, &protocol ), sizeof( protocol ) );
}

string NetIRecorder::playbackIPAddress() const
{
    return RecRecorderPrivate::instance().playbackNetworkString();
}

void NetIRecorder::recordIPAddress( const string& str ) const
{
    RecRecorderPrivate::instance().recordNetworkString( str );
}

string NetIRecorder::playbackModem() const
{
    return RecRecorderPrivate::instance().playbackNetworkString();
}

void NetIRecorder::recordModem( const string& str ) const
{
    RecRecorderPrivate::instance().recordNetworkString( str );
}

string NetIRecorder::playbackPhone() const
{
    return RecRecorderPrivate::instance().playbackNetworkString();
}

void NetIRecorder::recordPhone( const string& str ) const
{
    RecRecorderPrivate::instance().recordNetworkString( str );
}

NetNetwork::Modems NetIRecorder::playbackModems() const
{
    NetNetwork::Modems modems;

    const uint size = RecRecorderPrivate::instance().playbackNetworkUint();

    for( size_t i = 0; i < size; ++i )
    {
        modems.push_back( RecRecorderPrivate::instance().playbackNetworkString() );
    }

    return modems;
}

void NetIRecorder::recordModems( const NetNetwork::Modems& modems ) const
{
    RecRecorderPrivate::instance().recordNetworkUint( modems.size() );

    for( size_t i = 0; i < modems.size(); ++i )
    {
        RecRecorderPrivate::instance().recordNetworkString( modems[ i ]);
    }
}

bool NetIRecorder::playbackImStuffed() const
{
    return RecRecorderPrivate::instance().playbackNetworkBool();
}

void NetIRecorder::recordImStuffed( bool value ) const
{
    RecRecorderPrivate::instance().recordNetworkBool( value );
}

NetAppSession::NetSessionStatus NetIRecorder::playbackSessionStatus() const
{
    NetAppSession::NetSessionStatus status;

    RecRecorderPrivate::instance().playbackNetworkData( _REINTERPRET_CAST( uint8*, &status ), sizeof( status ) );

    return status;
}

void NetIRecorder::recordSessionStatus( const NetAppSession::NetSessionStatus& status ) const
{
    RecRecorderPrivate::instance().recordNetworkData( _REINTERPRET_CAST( const uint8*, &status ), sizeof( status ) );
}

NetAppSessionUid NetIRecorder::playbackAppSessionUid() const
{
    const uint bufferSize = RecRecorderPrivate::instance().playbackNetworkUint();
    uint8* buffer = _NEW_ARRAY( uint8, bufferSize );
    RecRecorderPrivate::instance().playbackNetworkData( buffer, bufferSize );

    NetAppSessionUid uid( buffer );
    _DELETE_ARRAY( buffer );

    return uid;
}

void NetIRecorder::recordAppSessionUid( const NetAppSessionUid& uid ) const
{
    const size_t bufferSize = uid.rawBufferSize();
    uint8* buffer = _NEW_ARRAY( uint8, bufferSize );
    uid.rawBuffer( buffer );

    RecRecorderPrivate::instance().recordNetworkUint( bufferSize );
    RecRecorderPrivate::instance().recordNetworkData( buffer, bufferSize );

    _DELETE_ARRAY( buffer );
}

bool NetIRecorder::playbackHasMember() const
{
    return RecRecorderPrivate::instance().playbackNetworkBool();
}

void NetIRecorder::recordHasMember( bool value ) const
{
    RecRecorderPrivate::instance().recordNetworkBool( value );
}

void NetIRecorder::playbackNodeIds( NetAppSession::NodeIds* pNodeIds ) const
{
    if( RecRecorderPrivate::instance().playbackNetworkUint() == NODE_IDS_EQUAL )
    {
        //  No update needed, there was no change
    }
    else
    {
        for( size_t i = 0; i < pNodeIds->size(); ++i )
            _DELETE( (*pNodeIds)[ i ] );

        pNodeIds->erase( pNodeIds->begin(), pNodeIds->end() );

        const uint size = RecRecorderPrivate::instance().playbackNetworkUint();

        for( size_t i = 0; i < size; ++i )
            pNodeIds->push_back( _NEW( NetNodeUid( playbackNodeUid() ) ) );
    }
}

void NetIRecorder::recordNodeIds( const NetAppSession::NodeIds& nodeIds ) const
{
    if( equal( nodeIds, previousNodeIds_ ) )
    {
        RecRecorderPrivate::instance().recordNetworkUint( NODE_IDS_EQUAL );
    }
    else
    {
        RecRecorderPrivate::instance().recordNetworkUint( NODE_IDS_NOT_EQUAL );

        RecRecorderPrivate::instance().recordNetworkUint( nodeIds.size() );

        for( size_t i = 0; i < nodeIds.size(); ++i )
            recordNodeUid( *nodeIds[ i ] );

        NetIRecorder* nonConstThis = _CONST_CAST( NetIRecorder*, this );

        nonConstThis->previousNodeIds_ = nodeIds;
    }
}

bool NetIRecorder::equal( const NetAppSession::NodeIds& a, const NetAppSession::NodeIds& b ) const
{
    bool result = false;

    if( a == b )
    {
        result = true;

        for( size_t i = 0; i < a.size() and result; ++i )
        {
            if( *a[ i ] != *b[ i ] )
            {
                result = false;
            }
        }
    }

    return result;
}

NetNodeUid NetIRecorder::playbackNodeUid() const
{
    const NetProcessorUid     processorUid = RecRecorderPrivate::instance().playbackNetworkString();
    NetThreadUid        threadUid;
    NetInterProcessUid  interprocessUid;

    RecRecorderPrivate::instance().playbackNetworkData( _REINTERPRET_CAST( uint8*, &threadUid ), sizeof( NetThreadUid ) );
    RecRecorderPrivate::instance().playbackNetworkData( _REINTERPRET_CAST( uint8*, &interprocessUid ), sizeof( NetInterProcessUid ) );
    const string nodeName = RecRecorderPrivate::instance().playbackNetworkString();

    const NetNodeUid uid( processorUid, threadUid, interprocessUid, nodeName );

    return uid;
}

void NetIRecorder::recordNodeUid( const NetNodeUid& nodeUid ) const
{
    RecRecorderPrivate::instance().recordNetworkString( nodeUid.processUid().processorUid() );
    RecRecorderPrivate::instance().recordNetworkData( _REINTERPRET_CAST( uint8*, _CONST_CAST(NetThreadUid*, &nodeUid.processUid().threadUid() )), sizeof( NetThreadUid ) );
    const NetInterProcessUid  interprocessUid = nodeUid.interprocessUid();
    RecRecorderPrivate::instance().recordNetworkData( _REINTERPRET_CAST( uint8*, _CONST_CAST(NetThreadUid*, &interprocessUid )), sizeof( NetInterProcessUid ) );
    RecRecorderPrivate::instance().recordNetworkString( nodeUid.nodeName() );
}

NetNode::Ping NetIRecorder::playbackPing() const
{
    const uint size = RecRecorderPrivate::instance().playbackNetworkUint();

    NetNode::Ping ping;

    for( size_t i = 0; i < size; ++i )
    {
        NetNodeUid nodeUid = playbackNodeUid();
        NetNode::NetPingInformation info;
        RecRecorderPrivate::instance().playbackNetworkData( _REINTERPRET_CAST( uint8*, &info ), sizeof( info ) );
        ping.insert( nodeUid, info );
    }

    return ping;
}

void NetIRecorder::recordPing( const NetNode::Ping& ping ) const
{
    RecRecorderPrivate::instance().recordNetworkUint( ping.size() );

    for( NetNode::Ping::const_iterator i = ping.begin(); i != ping.end(); ++i )
    {
        recordNodeUid( (*i).first );
        const NetNode::NetPingInformation& info = (*i).second;
        RecRecorderPrivate::instance().recordNetworkData( _REINTERPRET_CAST( const uint8*, &info ), sizeof( info ) );
    }
}

double NetIRecorder::playbacklastPingAllTime() const
{
    return RecRecorderPrivate::instance().playbackNetworkDouble();
}

void NetIRecorder::recordLastPingAllTime( double d ) const
{
    RecRecorderPrivate::instance().recordNetworkDouble( d );
}

void NetIRecorder::playbackMessageBuffer( NetNode::NetMessageBuffer* pMessageBuffer )
{
    while( not reachedEndOfMessageBufferUpdate() )
    {
        pMessageBuffer->push_back( _NEW( NetMessage( playbackMessage() ) ) );
    }

    size_t messageBufferSize = RecRecorderPrivate::instance().playbackNetworkUint();

    ASSERT_INFO( messageBufferSize );
    ASSERT_INFO( pMessageBuffer->size() );
    ASSERT( messageBufferSize == pMessageBuffer->size(), "" );
}

bool NetIRecorder::reachedEndOfMessageBufferUpdate() const
{
    return RecRecorderPrivate::instance().playbackNetworkInt() == END_OF_BUFFER;
}

void NetIRecorder::recordMessageBuffer( const NetNode::NetMessageBuffer& messageBuffer )
{
    //  Find the first item in the message buffer that is new (i.e. is not in the
    //  previous message buffer). We then write out this item and all items after
    //  it. We are relying on the fact that messages are always added to the end
    //  of the buffer - an assertion checks for this.

    bool    foundNewItem = false;
    NetNode::NetMessageBuffer::const_iterator newItem;

    for( NetNode::NetMessageBuffer::const_iterator i = messageBuffer.begin();
      i != messageBuffer.end() and not foundNewItem; ++i )
    {
        const NetMessage* pMessage = (*i);

        if( find( previousMessageBuffer_.begin(), previousMessageBuffer_.end(), pMessage ) == previousMessageBuffer_.end() )
        {
            foundNewItem = true;
            newItem = i;
        }
    }

    if( foundNewItem )
    {
        for( NetNode::NetMessageBuffer::const_iterator i = newItem;
          i != messageBuffer.end(); ++i )
        {
            const NetMessage* pMessage = (*i);
            ASSERT( find( previousMessageBuffer_.begin(), previousMessageBuffer_.end(), pMessage ) == previousMessageBuffer_.end(),
              "A new message was inserted in the middle of the buffer" );

            RecRecorderPrivate::instance().recordNetworkInt( MESSAGE );

            recordMessage( *pMessage );
        }
    }

    RecRecorderPrivate::instance().recordNetworkInt( END_OF_BUFFER );

    RecRecorderPrivate::instance().recordNetworkUint( messageBuffer.size() );

    previousMessageBuffer_ = messageBuffer;
}

void NetIRecorder::recordMessage( const NetMessage& message ) const
{
    recordHeader( message.header() );
    recordBody( message.body() );
}

NetMessage NetIRecorder::playbackMessage() const
{
    const NetMessageHeader header = playbackHeader();
    const NetMessageBody body = playbackBody();

    NetMessage message( header, body );

    return message;
}

void NetIRecorder::recordHeader( const NetMessageHeader& header ) const
{
//    recordNodeUid( header.sender() );

    RecRecorderPrivate::instance().recordNetworkData( _REINTERPRET_CAST( uint8*, _CONST_CAST(NetPriority*, &header.priority() )), sizeof( NetPriority ) );
}

NetMessageHeader NetIRecorder::playbackHeader() const
{
    const NetNodeUid sender = playbackNodeUid();

    NetPriority priority;

    RecRecorderPrivate::instance().playbackNetworkData( _REINTERPRET_CAST( uint8*, &priority ), sizeof( NetPriority ) );

//    const NetMessageHeader header( sender, priority );
    const NetMessageHeader header( NULL, priority );

    return header;
}

void NetIRecorder::recordBody( const NetMessageBody& messageBody ) const
{
    RecRecorderPrivate::instance().recordNetworkUint( messageBody.lengthInBytes() );
    RecRecorderPrivate::instance().recordNetworkData( _CONST_CAST( uint8*, messageBody.body() ), messageBody.lengthInBytes() );
}

NetMessageBody NetIRecorder::playbackBody() const
{
    const size_t bodySize = RecRecorderPrivate::instance().playbackNetworkUint();

    uint8* tempBuffer = _NEW_ARRAY( uint8, bodySize );

    RecRecorderPrivate::instance().playbackNetworkData( tempBuffer, bodySize );

    NetMessageBody body( tempBuffer, bodySize );

    _DELETE_ARRAY( tempBuffer );

    return body;
}

ostream& operator <<( ostream& o, const NetIRecorder& t )
{

    o << "NetIRecorder " << (void*)&t << " start" << std::endl;
    o << "NetIRecorder " << (void*)&t << " end" << std::endl;

    return o;
}

/* End RECORDER.CPP *************************************************/
