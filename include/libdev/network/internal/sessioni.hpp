#ifndef _NETIAPPSESSION_HPP
#define _NETIAPPSESSION_HPP

#include "network/sessuid.hpp"

//#include "dplay.h"
#include <enet/enet.h>

class NetIAppSession
{
public:

	static NetAppSession::NetSessionStatus& currentStatus();

	const NetAppSessionUid&		appSessionUid() const;
	const NetAppSessionName&   	appSessionName() const;

	bool hasMember( const NetNodeUid& checkUid) const;
	// POST( implies( result, NetNetwork::instance().hasSessionMember( nUid ) ) );

	void join( NetNode* pNode );
	// PRE( not hasMember( pNode->nodeUid() ) );
	// POST( hasMember( pNode->nUid ) );

	///////////////////////////////

	typedef ctl_pvector< NetNodeUid > NodeIds;

	const NodeIds& nodes() const;
	const NodeIds& nodesNoRecord() const;

	bool isEmpty() const;
	// POST( iff( result, nodes().empty() ) );


private:
	//used for NEW
	NetIAppSession( const NetAppSessionName& );
	//used for JOIN
	NetIAppSession( const NetAppSessionUid& );
	//used for LOBBY
	NetIAppSession();

	~NetIAppSession();

	NetAppSessionUid	appSessionUid_;
	NodeIds				nodeIds_;

	void init( );
	void leave( NetNode* pNode );
	// PRE( hasMember( pNode->nodeUid() ) );
	// POST( not hasMember( pNode->nUid ) );

	// call periodically to ensure that nodes() remains current.
	void updateNodes();

	bool isValid() const;
	bool isValidNoRecord() const;

	bool hasMemberNoRecord( const NetNodeUid& checkUid) const;

	static NetAppSession::NetSessionStatus& currentStatusNoRecord();

	friend class NetNode;
	friend class NetINetwork;
	friend class NetAppSession;
};

#endif //_NetIAppSession_HPP
