/*
 * N O D E I . H P P 
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

/*
    NetNodeImpl

    A brief description of the class should go in here
*/

#ifndef _NETWORK_NODEI_HPP
#define _NETWORK_NODEI_HPP

#include "device/timer.hpp"
#include "network/netdefs.hpp"
#include "network/sessuid.hpp"
#include "network/message.hpp"
#include "network/nodeuid.hpp"
#include "network/node.hpp"

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


class NetNodeImpl
// Canonical form revoked
{
public:
    ~NetNodeImpl( void );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const NetNodeImpl& t );
	friend class NetNode;

    NetNodeImpl( void );
    NetNodeImpl( const NetNodeImpl& );
    NetNodeImpl& operator =( const NetNodeImpl& );

	NetNodeUid*							pNodeUid_;		// ORDER DEPENDENCY
	NetAppSessionUid*					pAppSessionUid_;	// ORDER DEPENDENCY

	NetNode::NetMessageBuffer			messageBuffer_;

	bool 								acceptingPing_;

	const string 						pingString_;

	NetNode::Ping						ping_;
	double								lastPingAllTime_;
	double								lastCompoundTransmitTime_;
	DevTimer							timer_;
	NetCompoundMessage*					pCompoundMessage_;
	ctl_pvector< NetCompoundMessage >	cachedCompoundMessages_;

};


#endif

/* End NODEI.HPP ****************************************************/
