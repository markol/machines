#ifndef _NETNETWORK_HPP
#define _NETNETWORK_HPP

#include "ctl/pvector.hpp"
#include "ctl/map.hpp"

#include "network/netdefs.hpp"
#include "network/message.hpp"

class NetNode;
class NetNodeUid;
class NetProcessUid;
class NetAppSession;
class NetAppSessionUid;
class NetINetwork;
class NetSystemMessageHandler;
class NetComPortAddress;

class NetNetwork
{
public:
        ///////////////////////////////

    typedef ctl_map< string, unsigned int, std::less<string> > ProtocolMap;
        typedef ctl_pvector< NetAppSessionUid > Sessions;
        typedef ctl_vector< string >    Modems;

        enum NetNetworkStatus
        {
                NETNET_OK,
                NETNET_CONNECTIONERROR,
                NETNET_INVALIDCONNECTIONDATA,
                NETNET_INVALIDDATA,
                NETNET_UNDEFINEDERROR,
                NETNET_CANTINITIALIZEDIRECTPLAY,
                NETNET_MEMORYERROR,
                NETNET_SESSIONERROR,
                NETNET_NODEERROR,
        };

        enum NetworkProtocol { UDP, IPX, TCPIP, MODEM, SERIAL, ZONE, OTHER };

        ///////////////////////////////

        ~NetNetwork();

        ///////////////////////////////

        static NetNetwork&                      instance();
        static NetInterProcessUid       nextInterProcessUid();
        static const ProtocolMap&       availableProtocols();

        //Normally a protocol may be chosen with initialise set and the default dialogs will be displayed.
        //If you wish to override the default dialogs then set to NOT_INITIALISE, set the extra parameters via the
        //interface below and then finally call initialiseConnection explicitly.
        enum InitialiseConnection { INITIALISE_CONNECTION, DO_NOT_INITIALISE_CONNECTION };
        static void                                     chooseProtocol(const string&, InitialiseConnection );
        void                                            initialiseConnection();

        static NetNetworkStatus         currentStatus();
    //  Use the no record version in assertions
        static NetNetworkStatus         currentStatusNoRecord();
        static void                             resetStatus();

        ///////////////////////////////

        NetAppUid                               appUid() const;
        NetAppUid                               appUidNoRecord() const;
        bool                                    hasAppSession( NetAppSessionUid ) const;
        void                                    update();
        void                                    pollMessages();
        const Sessions&                 sessions() const;
        //If hosting conventional game then use createAppSession
        NetAppSession*                  createAppSession( const NetAppSessionName& );
        //If joining a conventional game then use joinAppSession
        NetAppSession*                  joinAppSession( const NetAppSessionUid& );
        //if connecting via lobby then use connectAppSession (handles join and create)
        NetAppSession*                  connectAppSession();
        void                                    systemMessageHandler( NetSystemMessageHandler* );
        bool                                    hasSystemMessageHandler() const;
        NetSystemMessageHandler&                                        systemMessageHandler();


        void                                    doAbort( const string& reason );
        NetINetwork&                    netINetwork() const;

        bool                                    isLobbiedGame() const;
        bool                                    isLogicalHost() const;
        const string&                   localPlayerName() const;
        void                            localPlayerName(const string&);
        //All this does is call into NetINetwork lpDiretcPlayLobby -
        // which will ensure that internal flags are set correctly
        void                                    instantiateLobby();

        NetworkProtocol                 currentProtocol() const;

        //Ip addresses may be in the form of numerical IP addresses or domain net
        const string&                   IPAddress() const;
        void                                    IPAddress( const string&);


        bool                                    imStuffed() const;
        bool                                    imStuffedNoRecord() const;
        void                                    imStuffed( bool );

        bool                                    deterministicPingDropoutAllowed() const;
        void                                    deterministicPingDropoutAllowed( bool );

        void                                    disableNewPlayers();
        void                                    outputStuffedInfo( ostream& );
        void                                    outputCurrentStatistics( ostream& );

        void                                    messageThrottlingActive( bool );
        bool                                    messageThrottlingActive() const;

        void                                    autoAdjustMaxPacketsPerSecond( size_t numberOfPlayers );

        int                                     maxSentMessagesPerSecond() const;
        ///////////////////////////////
        NetMessage*                             getMessage();
        bool                                    haveMessages();
        void                                    sendMessage( const NetPriority& priority,
                                                             const NetMessageBody& body );

private:
        friend ostream& operator<<( ostream&, NetworkProtocol );
        ///////////////////////////////

        NetNetwork();

        ///////////////////////////////

        bool isValid() const;
        bool isValidNoRecord() const;


        ///////////////////////////////

        NetINetwork*    pNetINetwork_;

        ///////////////////////////////

        friend class NetINetwork;
        friend class NetNode;

        ///////////////////////////////
};

#endif //_NETNETWORK_HPP
