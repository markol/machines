/*
 * N E T I M P L . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogNetworkDataImpl

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_NETIMPL_HPP
#define _MACHLOG_NETIMPL_HPP

#include "base/base.hpp"
#include "machlog/network.hpp"

class MachLogNetworkDataImpl
// Canonical form revoked
{
public:
    ~MachLogNetworkDataImpl();

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachLogNetworkDataImpl& t );
	friend class MachLogNetwork;

    MachLogNetworkDataImpl();

    MachLogNetworkDataImpl( const MachLogNetworkDataImpl& );
    MachLogNetworkDataImpl& operator =( const MachLogNetworkDataImpl& );

	bool							isNetworkGame_;
	bool							isNodeLogicalHost_;
	MachLogNetwork::Remote			remote_[ MachPhys::N_RACES ];
	MachPhys::Race					localRace_;
	string							desiredProtocol_;
	string							sessionId_;
	NetNode*						pNode_;
	MachLogMessageBroker*			pBroker_;
	bool							readyStatus_[ MachPhys::N_RACES ];
	bool							syncSimCycles_;
	bool							simUpdateReceived_[ MachPhys::N_RACES ];
	bool							simUpdateNeeded_[ MachPhys::N_RACES ];
	int								expectedPlayers_;
	bool							processedStartup_[ MachPhys::N_RACES ];
	bool							lockedForStartup_;
	bool							protocolChosen_;
};

#define CB_MachLogNetwork_DEPIMPL() \
	CB_DEPIMPL( bool, isNetworkGame_ ); \
	CB_DEPIMPL( bool, isNodeLogicalHost_ ); \
	CB_DEPIMPL_ARRAY( MachLogNetwork::Remote, remote_ ); \
	CB_DEPIMPL( MachPhys::Race, localRace_ ); \
	CB_DEPIMPL( string, desiredProtocol_ ); \
	CB_DEPIMPL( string, sessionId_ ); \
	CB_DEPIMPL( NetNode*, pNode_ ); \
	CB_DEPIMPL( MachLogMessageBroker*, pBroker_ ); \
	CB_DEPIMPL_ARRAY( bool, readyStatus_ ); \
	CB_DEPIMPL( bool, syncSimCycles_ ); \
	CB_DEPIMPL_ARRAY( bool, simUpdateReceived_ ); \
	CB_DEPIMPL_ARRAY( bool, simUpdateNeeded_ ); \
	CB_DEPIMPL( int, expectedPlayers_ ); \
	CB_DEPIMPL_ARRAY( bool, processedStartup_ ); \
	CB_DEPIMPL( bool, lockedForStartup_ ); \
	CB_DEPIMPL( bool, protocolChosen_ );
	

#endif

/* End NETIMPL.HPP **************************************************/
