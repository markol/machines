#include "network/session.hpp"
#include "network/node.hpp"

#include "network/internal/mapping.hpp"
#include "network/internal/sessioni.hpp"

const int MAXPLAYERS = 10;

NetAppSession::NetAppSession( const NetAppSessionName& sessionName)
{
	pIAppSession_ = _NEW( NetIAppSession( sessionName ) );
}

NetAppSession::NetAppSession( const NetAppSessionUid& sessionUid)
{
	pIAppSession_ = _NEW( NetIAppSession( sessionUid ) );
}

NetAppSession::NetAppSession()
{
	pIAppSession_ = _NEW( NetIAppSession() );
}

NetAppSession::~NetAppSession()
{
	_DELETE(pIAppSession_);
}

NetAppSession::NetSessionStatus NetAppSession::currentStatus() const
{
	return NetIAppSession::currentStatus();
}

const NetAppSessionUid& NetAppSession::appSessionUid() const
{
	return pIAppSession_->appSessionUid();
}

const NetAppSessionName& NetAppSession::appSessionName() const
{
	return pIAppSession_->appSessionName();
}

bool NetAppSession::hasMember( NetNodeUid nodeUid ) const
{
	return pIAppSession_->hasMember(nodeUid);
}

bool NetAppSession::hasMemberNoRecord( NetNodeUid nodeUid ) const
{
	return pIAppSession_->hasMemberNoRecord(nodeUid);
}

void NetAppSession::join( NetNode* pNode )
{
	PRE( not hasMemberNoRecord( pNode->nodeUid() ) );

	pIAppSession_->join(pNode);

	POST( hasMemberNoRecord( pNode->nodeUid() ) );
}
///////////////////////////////

//// call periodically to ensure that nodes() remains current.
void NetAppSession::updateNodes()
{
	pIAppSession_->updateNodes();
}

const NetAppSession::NodeIds& NetAppSession::nodes() const
{
	return pIAppSession_->nodes();
}

const NetAppSession::NodeIds& NetAppSession::nodesNoRecord() const
{
	return pIAppSession_->nodesNoRecord();
}

bool NetAppSession::isEmpty() const
{
	return pIAppSession_->isEmpty();
}

ostream& operator<<( ostream& o, NetAppSession::NetSessionStatus status )
{
	switch( status )
	{
		case NetAppSession::NETSESS_OK: o << "NETSESS_OK"; break;
		case NetAppSession::NETSESS_INVALIDDATA: o << "NETSESS_INVALIDDATA"; break;
		case NetAppSession::NETSESS_NOSESSIONS: o << "NETSESS_NOSESSIONS"; break;
		case NetAppSession::NETSESS_INVALIDSUPPLIEDDATA: o << "NETSESS_INVALIDSUPPLIEDDATA"; break;
		case NetAppSession::NETSESS_NETWORKERROR: o << "NETSESS_NETWORKERROR"; break;
		case NetAppSession::NETSESS_ALREADYCONNECTING: o << "NETSESS_ALREADYCONNECTING"; break;
		case NetAppSession::NETSESS_INTERNALDATAERROR: o << "NETSESS_INTERNALDATAERROR"; break;
		case NetAppSession::NETSESS_UNABLETOCONNECT: o << "NETSESS_UNABLETOCONNECT"; break;
		case NetAppSession::NETSESS_UNDEFINEDERROR: o << "NETSESS_UNDEFINEDERROR"; break;
		case NetAppSession::NETSESS_INVALIDPLAYER: o << "NETSESS_INVALIDPLAYER"; break;
		default: o << "Unknown status in NetAppSession " << (int)status << std::endl;
	}
	return o;
}
