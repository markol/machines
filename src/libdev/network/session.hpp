#ifndef _NETAPPSESSION_HPP
#define _NETAPPSESSION_HPP

#include "network/netdefs.hpp"
#include "ctl/pvector.hpp"

class NetNodeUid;
class NetNode;
class NetIAppSession;
class NetAppSessionUid;

class NetAppSession
{
public:
	
	enum NetSessionStatus
	{
		NETSESS_OK,
		NETSESS_INVALIDDATA,
		NETSESS_NOSESSIONS,
		NETSESS_INVALIDSUPPLIEDDATA,
		NETSESS_NETWORKERROR,
		NETSESS_ALREADYCONNECTING,
		NETSESS_INTERNALDATAERROR,
		NETSESS_UNABLETOCONNECT,
		NETSESS_UNDEFINEDERROR,
		NETSESS_INVALIDPLAYER
	};

	NetSessionStatus currentStatus() const;

	//Return this session uid
	const NetAppSessionUid&		appSessionUid() const;
	//Return this session name
	const NetAppSessionName&   	appSessionName() const;

	bool hasMember( NetNodeUid checkUid) const;
    //  Use the no record version for assertions
	bool hasMemberNoRecord( NetNodeUid checkUid) const;

	void join( NetNode* pNode );
	// PRE( not hasMember( pNode->nodeUid() ) );
	// POST( hasMember( pNode->nUid ) );

	///////////////////////////////
	
	typedef ctl_pvector< NetNodeUid > NodeIds;

//	// call periodically to ensure that nodes() remains current.
	void updateNodes();

	const NodeIds& nodes() const;
	const NodeIds& nodesNoRecord() const;

	bool isEmpty() const;
	// POST( iff( result, nodes().empty() ) );

private:
	friend ostream& operator<<( ostream&, NetSessionStatus );

	//new game session
	NetAppSession( const NetAppSessionName& );
	//joining session
	NetAppSession( const NetAppSessionUid& );
	//lobby session
	NetAppSession( );
	~NetAppSession( );

	NetIAppSession* pIAppSession_;

	friend class NetNode;
	friend class NetINetwork;
	friend class NetIAppSession;

	//Operations revoked
	NetAppSession( const NetAppSession& );
    NetAppSession& operator =( const NetAppSession& );

};

#endif //_NETAPPSESSION_HPP