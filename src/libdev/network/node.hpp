#ifndef _NODE_HPP
#define _NODE_HPP

#include "device/timer.hpp"
#include "network/netdefs.hpp"
#include "network/sessuid.hpp"
#include "network/message.hpp"
#include "network/nodeuid.hpp"

#include "ctl/queue.hpp"
#include "ctl/deque.hpp"
#include "ctl/map.hpp"

class NetAppSessionUid;
class NetPriority;
class NetMessageBody;
class NetMessage;
class NetPriority;
class NetINetwork;
class NetCompoundMessage;
class NetNodeImpl;

class NetNode
// cannonical form revoked
{
public:

        enum NetNodeStatus
        {
                NETNODE_OK,
                NETNODE_UNDEFINEDERROR,
                NETNODE_PLAYERCREATIONERROR,
                NETNODE_INTERNALDATAERROR,
                NETNODE_INVALIDINTERNALDATA,
                NETNODE_NETWORKERROR,
                NETNODE_INVALIDSUPPLIEDDATA,
                NETNODE_NETWORKACCESSREFUSED,
                NETNODE_MESSAGETOOBIG,
                NETNODE_INVALIDPLAYER,
                NETNODE_PLAYERDOESNOTEXIST,
                NETNODE_CONNECTIONLOST,
                NETNODE_UNSUPPORTED
        };

        static NetNodeStatus& currentStatus();
        static NetNodeStatus& currentStatusNoRecord();

        NetNode( const NetNodeName& name );

        virtual ~NetNode();

        ///////////////////////////////

        const NetNodeUid&               nodeUid() const;
        NetNodeUid*                             pNodeUid();
        const NetAppSessionUid& appSessionUid() const;

        virtual void sendMessage( const NetPriority& priority, const NetMessage::NetMessageRecipients& to, const NetMessageBody& body);
        // POST( not ctl_contains( &to, nodeUid() ) );

        virtual void receiveMessage( NetMessage* message );
        // PRE( ctl_contains( &header.recipients(), nodeUid() ) );

        virtual NetMessage* getMessage();
        virtual NetMessage* peekMessage();
        virtual bool            haveMessages();
        virtual NetMessage* peekMessage(size_t);
        virtual NetMessage* getMessage(size_t);
        virtual void            ping( const NetNodeUid& );

        void acceptPing();
        void refusePing();
        bool isAcceptingPing() const;

        int lastPingTime() const;

        struct NetPingInformation
        {
                double                                  lastTimePingAsked_;
                double                                  lastTimePingSet_;
                double                                  lastPingTime_;
                double                                  lastProperPingTime_;
                friend bool operator ==( const NetPingInformation& lhs, const NetPingInformation& rhs )
                {
                        return lhs.lastTimePingAsked_ == rhs.lastTimePingAsked_ and
                                lhs.lastTimePingSet_ == rhs.lastTimePingSet_ and
                                lhs.lastPingTime_ == rhs.lastPingTime_ and
                                lhs.lastProperPingTime_ == rhs.lastProperPingTime_ ;
                }
                friend bool operator <( const NetPingInformation& lhs, const NetPingInformation& rhs )
                {
                        return lhs.lastTimePingAsked_ < rhs.lastTimePingAsked_ and
                                lhs.lastTimePingSet_ < rhs.lastTimePingSet_ and
                                lhs.lastPingTime_ < rhs.lastPingTime_ and
                                lhs.lastProperPingTime_ < rhs.lastProperPingTime_ ;
                }
                friend ostream& operator<<( ostream& o, const NetPingInformation& pi );
        };

        typedef ctl_map< NetNodeUid, NetPingInformation, std::less< NetNodeUid > >           Ping;

        void pingAll();
        const Ping&     pingInformation() const;

        size_t                          messageQueueSize();

        //time in ms since last ping all call.
        double lastPingAllTime() const;

//      typedef ctl_queue< ctl_deque< NetMessage* > >NetMessageBuffer;
        typedef ctl_pvector< NetMessage > NetMessageBuffer;

        //To use compound messaging call this with true or false.
        //Internally this is safe and multiple calls are coped with gracefully and correctly.
        void useCompoundMessaging( bool );
        void transmitCompoundMessage();

        const ctl_pvector< NetCompoundMessage >& cachedCompoundMessages() const;

        void CLASS_INVARIANT;

private:
        friend ostream& operator<<( ostream&, const NetNodeStatus& );

        NetNode( const NetNode& );
        NetNode& operator =( const NetNode& );
        bool operator =( const NetNode& ) const;

//      NetMessageUid nextMessageUidFor( NetNodeUid toNode );
//      // PRE( toNode != nodeUid() );

        void addToNetwork();

        void removeFromNetwork();

        void leaveAppSession();

        void enterAppSession( NetAppSessionUid aUid );
        void enterAppSession(  const NetNodeName& name, NetAppSessionUid aUid );

        bool isValid() const;
        bool isValidNoRecord() const;

        NetNodeStatus convertStatus(unsigned int) const;

        size_t messageQueueSizeNoRecord();

    void recorderUpdateMessageBuffer();
        void addCachedCompoundMessage( NetCompoundMessage* );
        void actuallyTransmitCompoundMessage( NetCompoundMessage* pCompoundMessage );

//      friend NetNetwork::MethodReturnStatus NetINetwork::enterAppSession( NetNode*, NetAppSessionUid);

        ///////////////////////////////

        friend class NetINetwork;
        NetNodeImpl*                    pImpl_;
};

#endif //_NODE_HPP
