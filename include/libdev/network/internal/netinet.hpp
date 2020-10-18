#ifndef _NETINETWORK_HPP
#define _NETINETWORK_HPP

#define IDIRECTPLAY2_OR_GREATER

#include "ctl/vector.hpp"
#include "ctl/list.hpp"
#include "ctl/map.hpp"

#include "device/timer.hpp"

#include <enet/enet.h>

#include "network/netdefs.hpp"
#include "network/netnet.hpp"
#include "network/comport.hpp"
//#include "network/pinghelp.hpp"

class NetNode;
class NetNodeUid;
class NetProcessUid;
class NetAppSession;
class NetAppSessionUid;
class NetSystemMessageHandler;

struct NetMessageShortInfo
{
	double		time_;
	int			length_;
};
bool operator<( const NetMessageShortInfo& a, const NetMessageShortInfo& b );
bool operator==( const NetMessageShortInfo& a, const NetMessageShortInfo& b );


class NetINetwork
{
public:
	///////////////////////////////

	enum UpdateType
	{
		UPDATE,
		NOUPDATE
	};

	enum ObjectActionType
	{
		RETURN,
		RESET
	};

	///////////////////////////////

	NetINetwork();
	~NetINetwork();

	///////////////////////////////

	const NetNetwork::ProtocolMap& availableProtocols( UpdateType update = UPDATE );
	void chooseProtocol(const string&, NetNetwork::InitialiseConnection );
	void initialiseConnection();

	static const NetNetwork::NetNetworkStatus& currentStatus();
    static void currentStatus( NetNetwork::NetNetworkStatus );
	static void	resetStatus();



	///////////////////////////////

	NetAppSession*					createAppSession( const NetAppSessionName& );
	NetAppSession* 					joinAppSession( const NetAppSessionUid& );
	NetAppSession* 					connectAppSession();

	NetAppUid						appUid() const;
	bool 							hasAppSession( const NetAppSessionUid& ) const;
    //  Use the no record version in assertions
	bool 							hasAppSessionNoRecord( const NetAppSessionUid& ) const;
	NetAppSession& 					session();
	void 							update();
	void 							pollMessages();
	const NetNetwork::Sessions& 	sessions() const;

	bool							isValid() const;

	void 							systemMessageHandler( NetSystemMessageHandler* );
	bool 							hasSystemMessageHandler() const;
	NetSystemMessageHandler&		systemMessageHandler();


	//mainly used with lobby technology - but the fields should be valid for normal create/join mechanism as well
	bool							isLobbiedGame() const;
	bool							isLogicalHost() const;
	const string&					localPlayerName() const;
	void							localPlayerName( const string& );

	///////////////////////////////
	bool							imStuffed() const;
	bool							imStuffedNoRecord() const;

	bool							deterministicPingDropoutAllowed() const;
	void							deterministicPingDropoutAllowed( bool );

private:

	///////////////////////////////

	typedef ctl_vector<NetNode*> Nodes;
	typedef ctl_vector<ENetPeer*> Peers;
	typedef ctl_pvector< NetMessage > NetMessageBuffer;

	///////////////////////////////

	void 						clearProtocols();
	const string& 				protocolName();
	string& 				    protocolNameNoRecord();
    void                        protocolName( const string& );
	void 						setAppUid( );

	///////////////////////////////

	bool 						hasAppSession( const NetAppSessionName& ) const;
    //  Use the no record version in assertions
	bool 						hasAppSessionNoRecord( const NetAppSessionName& ) const;
	NetAppSession* 				createAppSession( const NetAppSessionName&, NetNode* );
	void 						updateSessions();
	bool 						hasLocalNode( const NetNode*  ) const;
	void 						addLocalNode( NetNode* );
	void 						removeLocalNode( NetNode* );
	void						enterAppSession( NetNode* , NetAppSessionUid );
	void						doAbort( const string& reasonCode );
	void                        initHost(bool asServer = false);
	NetMessage*                 getMessage();
	bool                        haveMessages();
	void                        sendMessage( const NetPriority& priority,
                                             const NetMessage::NetMessageRecipients& /*to*/,
                                             const NetMessageBody& body
                                           );

	NetNetwork::NetworkProtocol	currentProtocol() const;

	//Ip addresses may be in the form of numerical IP addresses or domain net
	const string&				IPAddress() const;
	void 						IPAddress( const string&);

	bool						isValidNoRecord() const;
	bool 						hasLocalNodeNoRecord( const NetNode*  ) const;
	static NetNetwork::NetNetworkStatus& currentStatusNoRecord();

	NetAppUid					appUidNoRecord() const;

	NetProcessUid&			    processUidMaster() const;
	void						addSentMessage( int length );
	void						computeSentMessageStuffedNess();
	double						deterministicTimeoutPeriod() const;
	size_t						maxSentMessagesPerSecond() const;
	void						autoAdjustMaxSentMessagesPerSecond( size_t numberOfPlayers );

	void						disableNewPlayers();
	bool						pingAllAllowed() const;

	void determineStandardSendFlags();

	void						messageThrottlingActive( bool );
	bool						messageThrottlingActive() const;


	///////////////////////////////

	static NetAppUid	appUid_;

	///////////////////////////////

	NetNode::NetMessageBuffer		messageBuffer_;
	NetNetwork::Sessions			sessions_;
	Nodes							nodes_;
	Peers							peers_;
	NetAppSession*					pLocalSession_;
	NetSystemMessageHandler*		pSystemMessageHandler_;

	bool							lobbyAware_;		//default is false

	//lobby description fields
	string							descAppName_;
	string 							descFileName_;
	string 							descCommandLine_;
	string 							descPath_;
	string 							descCurrentDirectory_;
	string 							descDescription_;


	bool							isLobbiedGame_;
	bool							isLogicalHost_;
	string							localPlayerName_;

	string							IPAddress_;

	NetNetwork::NetworkProtocol		currentProtocol_;

	bool							imStuffed_;

//	NetPingHelper					pingHelper_;

    ENetAddress                     address_;
    ENetHost*                       pHost_;

	bool							deterministicPingDropoutAllowed_;

	///////////////////////////////

	friend class NetNode;
	friend class NetNetwork;
	friend class NetIAppSession;
	friend class NetSystemMessageHandler;
	///////////////////////////////
	DevTimer						timer_;

	typedef ctl_list< NetMessageShortInfo* >	SentMessages;
	SentMessages								sentMessages_;

	int											maxBytesPerSecond_;
	bool										pingAllAllowed_;
	int											maxSentMessagesPerSecond_;
	int											originalMaxSentMessagesPerSecond_;


	bool										messageThrottlingActive_;
	NetNetwork::ProtocolMap						availableProtocols_;


};


#endif //_NetINetwork_HPP
