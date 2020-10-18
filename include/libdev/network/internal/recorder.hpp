/*
 * R E C O R D E R . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    NetIRecorder

    Helper class for the network recording functionality. This helper
    class means that the recorder library doesn't have to make use of
    lots of network data types so frees it up from ties to the network
    library. This sort of helper class should probably be added to
    other libraries that make use of the recorder.
*/

#ifndef _NETWORK_RECORDER_HPP
#define _NETWORK_RECORDER_HPP

#include "base/base.hpp"

#include "network/netdefs.hpp"
#include "network/netnet.hpp"
#include "network/session.hpp"
#include "network/node.hpp"

class NetIRecorder
// Canonical form revoked
{
public:

    static NetIRecorder& instance();

    NetProcessUid playbackProcessUid() const;
    void recordProcessUid( NetProcessUid ) const;

    bool playbackHasAppSession() const;
    void recordHasAppSession( bool ) const;

    void playbackSessions( NetNetwork::Sessions* ) const;
    void recordSessions( const NetNetwork::Sessions& ) const;

    NetAppUid playbackAppUid() const;
    void recordAppUid( NetAppUid ) const;

    bool playbackIsLogicalHost() const;
    void recordIsLogicalHost( bool ) const;

    string playbackLocalPlayerName() const;
    void recordLocalPlayerName( const string& ) const;

    bool playbackHasLocalNode() const;
    void recordHasLocalNode( bool ) const;

    void playbackAvailableProtocols( NetNetwork::ProtocolMap* ) const;
    void recordAvailableProtocols( const NetNetwork::ProtocolMap& ) const;

    string playbackCurrentProtocolName() const;
    void recordCurrentProtocolName( const string& ) const;

    NetNetwork::NetNetworkStatus playbackCurrentStatus() const;
    void recordCurrentStatus( const NetNetwork::NetNetworkStatus& ) const;

    bool playbackIsLobbiedGame() const;
    void recordIsLobbiedGame( bool ) const;

    NetNetwork::NetworkProtocol playbackCurrentProtocol() const;
    void recordCurrentProtocol( const NetNetwork::NetworkProtocol& ) const;

    string playbackIPAddress() const;
    void recordIPAddress( const string& ) const;

    string playbackModem() const;
    void recordModem( const string& ) const;

    string playbackPhone() const;
    void recordPhone( const string& ) const;

    NetNetwork::Modems playbackModems() const;
    void recordModems( const NetNetwork::Modems& ) const;

    bool playbackImStuffed() const;
    void recordImStuffed( bool ) const;

    NetAppSession::NetSessionStatus playbackSessionStatus() const;
    void recordSessionStatus( const NetAppSession::NetSessionStatus& ) const;

    NetAppSessionUid playbackAppSessionUid() const;
    void recordAppSessionUid( const NetAppSessionUid& ) const;

    bool playbackHasMember() const;
    void recordHasMember( bool ) const;

    void playbackNodeIds( NetAppSession::NodeIds* ) const;
    void recordNodeIds( const NetAppSession::NodeIds& ) const;

    NetNodeUid playbackNodeUid() const;
    void recordNodeUid( const NetNodeUid& ) const;

    NetNode::Ping playbackPing() const;
    void recordPing( const NetNode::Ping& ) const;

    double playbacklastPingAllTime() const;
    void recordLastPingAllTime( double ) const;

    void playbackMessageBuffer( NetNode::NetMessageBuffer* );
    void recordMessageBuffer( const NetNode::NetMessageBuffer& );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const NetIRecorder& t );

    NetIRecorder( const NetIRecorder& );
    NetIRecorder& operator =( const NetIRecorder& );

    NetIRecorder( void );
    ~NetIRecorder( void );

    bool reachedEndOfMessageBufferUpdate() const;
    size_t readMessageBufferSize() const;

    void recordMessage( const NetMessage& message ) const;
    NetMessage playbackMessage() const;
//    size_t headerSize( const NetMessageHeader& header ) const;
//    size_t bodySize( const NetMessageBody& body ) const;
//    void packMessage( const NetMessage& message, uint8* buffer ) const;
    void recordHeader( const NetMessageHeader& header ) const;
    void recordBody( const NetMessageBody& messageBody ) const;
//    void packNetNodeUid( const NetNodeUid& nodeUid, uint8* buffer ) const;
    size_t netNodeUidSize( const NetNodeUid& nodeUid ) const;

    NetMessageHeader playbackHeader() const;
//    NetNodeUid unpackNetNodeUid( const uint8* buffer ) const;
    NetMessageBody playbackBody() const;

    bool equal( const NetAppSession::NodeIds& a, const NetAppSession::NodeIds& b ) const;

    enum { MESSAGE, END_OF_BUFFER };
    enum { NODE_IDS_EQUAL, NODE_IDS_NOT_EQUAL };

    NetNode::NetMessageBuffer   previousMessageBuffer_;
    NetAppSession::NodeIds      previousNodeIds_;
};


#endif

/* End RECORDER.HPP *************************************************/
