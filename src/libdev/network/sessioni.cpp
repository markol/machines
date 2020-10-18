#include "profiler/profiler.hpp"
#include "device/timer.hpp"
#include "system/winapi.hpp"

#include "network/session.hpp"
#include "network/node.hpp"
#include "network/internal/netinet.hpp"
#include "network/internal/mapping.hpp"
#include "network/internal/sessioni.hpp"
#include "network/internal/recorder.hpp"
#include "recorder/recorder.hpp"

const int MAXPLAYERS = 10;

//static
NetAppSession::NetSessionStatus& NetIAppSession::currentStatus()
{
    NetAppSession::NetSessionStatus& status = currentStatusNoRecord();

    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
    {
        status = NetIRecorder::instance().playbackSessionStatus();
    }
    else
    {
        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
        {
            NetIRecorder::instance().recordSessionStatus( status );
        }
    }

	return status;
}

//static
NetAppSession::NetSessionStatus& NetIAppSession::currentStatusNoRecord()
{
	static NetAppSession::NetSessionStatus currentStatus_ = NetAppSession::NETSESS_OK;
	return currentStatus_;
}

const NetAppSessionUid& NetIAppSession::appSessionUid() const
{
	PRE( isValidNoRecord() );

    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
    {
        NetIAppSession* nonConstThis = _CONST_CAST( NetIAppSession*, this );

        nonConstThis->appSessionUid_ = NetIRecorder::instance().playbackAppSessionUid();
    }
    else
    {
        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
        {
            NetIRecorder::instance().recordAppSessionUid( appSessionUid_ );
        }
    }

	return appSessionUid_;
}

const NetAppSessionName& NetIAppSession::appSessionName() const
{
	PRE( isValidNoRecord() );
	return appSessionUid().appSessionName();
}

bool NetIAppSession::hasMember( const NetNodeUid& nodeUid ) const
{
	PRE( isValidNoRecord() );

	bool found = false;

    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
    {
        found = NetIRecorder::instance().playbackHasMember();
    }
    else
    {
        found = hasMemberNoRecord( nodeUid );

        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
        {
            NetIRecorder::instance().recordHasMember( found );
        }
    }


	POST( isValidNoRecord() );
	return found;
// POST( implies( result, NetNetwork::instance().hasSessionMember( nUid ) ) );
}

bool NetIAppSession::hasMemberNoRecord( const NetNodeUid& nodeUid ) const
{
	PRE( isValidNoRecord() );

	bool found = false;

	NodeIds::const_iterator i = nodeIds_.begin();
	NodeIds::const_iterator j = nodeIds_.end();

	while(i!=j && not found)
	{
		if(*(*i) == nodeUid)
			found = true;
		++i;
	}


	POST( isValidNoRecord() );
	return found;
// POST( implies( result, NetNetwork::instance().hasSessionMember( nUid ) ) );
}

void NetIAppSession::join( NetNode* pNode )
{
    if( RecRecorder::instance().state() != RecRecorder::PLAYING )
    {
        RecRecorder::instance().recordingAllowed( false );

    	PRE( isValidNoRecord() );
    	PRE(pNode);
    	PRE( not hasMember( pNode->nodeUid() ) );

    	nodeIds_.push_back( _NEW( NetNodeUid( pNode->nodeUid() ) ) );

    	POST( hasMember( pNode->nodeUid() ) );
    	POST( isValidNoRecord() );

        RecRecorder::instance().recordingAllowed( true );
    }
}
///////////////////////////////

// call periodically to ensure that nodes() remains current.
void NetIAppSession::updateNodes()
{
	PRE( isValidNoRecord() );

    if( RecRecorder::instance().state() != RecRecorder::PLAYING )
    {
        RecRecorder::instance().recordingAllowed( false );

    	NodeIds::iterator i = nodeIds_.begin();
    	NodeIds::iterator j = nodeIds_.end();

    	for(;i!=j;++i)
    	{
    		_DELETE((*i));
    	}

    	nodeIds_.erase(nodeIds_.begin(), nodeIds_.end());

        RecRecorder::instance().recordingAllowed( true );
    }

	POST( isValidNoRecord() );
}

const NetAppSession::NodeIds& NetIAppSession::nodes() const
{
	PRE( isValidNoRecord() );

    if( RecRecorder::instance().state() == RecRecorder::PLAYING )
    {
        NetIAppSession* nonConstThis = _CONST_CAST( NetIAppSession*, this );

        NetIRecorder::instance().playbackNodeIds( &nonConstThis->nodeIds_ );
    }
    else
    {
        if( RecRecorder::instance().state() == RecRecorder::RECORDING )
        {
            NetIRecorder::instance().recordNodeIds( nodeIds_ );
        }
    }

	return nodeIds_;
}

const NetAppSession::NodeIds& NetIAppSession::nodesNoRecord() const
{
	PRE( isValidNoRecord() );

	return nodeIds_;
}

bool NetIAppSession::isEmpty() const
{
	PRE( isValidNoRecord() );

	return nodes().empty();
}

void NetIAppSession::init()
{

}

NetIAppSession::~NetIAppSession()
{
	PRE( isValidNoRecord() );
	NodeIds::iterator i = nodeIds_.begin();
	NodeIds::iterator j = nodeIds_.end();

	for(;i!=j;++i)
	{
		if(*i)
			_DELETE((*i));
	}

	i = nodeIds_.begin();

	if(i!=j)
		nodeIds_.erase(nodeIds_.begin(), j);

	POST( isValidNoRecord() );
}

NetIAppSession::NetIAppSession( const NetAppSessionName& sessionName )
:	appSessionUid_(NetNetwork::instance().appUid(), 0, "default")
{
	NETWORK_STREAM("NetIAppSession::NetIAppSession( " << sessionName <<" )\n" );

	PRE( isValidNoRecord() );

    if( RecRecorder::instance().state() != RecRecorder::PLAYING )
    {
        RecRecorder::instance().recordingAllowed( false );

    	init();

        RecRecorder::instance().recordingAllowed( true );
    }

	POST(isValidNoRecord());
}

NetIAppSession::NetIAppSession( const NetAppSessionUid& newUid )
: appSessionUid_(newUid)
{
	NETWORK_STREAM("NetIAppSession::NetIAppSession( const NetAppSessionUid& )\n" );
	PRE( isValidNoRecord() );

    if( RecRecorder::instance().state() != RecRecorder::PLAYING )
    {
        RecRecorder::instance().recordingAllowed( false );

        init();
        RecRecorder::instance().recordingAllowed( true );
    }

//	POST( isValidNoRecord() );
}

NetIAppSession::NetIAppSession()
:	appSessionUid_(NetNetwork::instance().appUid(), 0, "default")
{
   	NETWORK_STREAM("NetIAppSession::NetIAppSession - lobby connection\n" );
   	NETWORK_INDENT( 2 );
  	PRE( isValidNoRecord() );

    if( RecRecorder::instance().state() != RecRecorder::PLAYING )
    {
        RecRecorder::instance().recordingAllowed( false );

        init();
        RecRecorder::instance().recordingAllowed( true );
    }

	POST( isValidNoRecord() );
	NETWORK_INDENT( -2 );
	NETWORK_STREAM("NetIAppSession::NetIAppSession - lobby connection DONE\n" );
}

void NetIAppSession::leave( NetNode* pNode )
{
    if( RecRecorder::instance().state() != RecRecorder::PLAYING )
    {
        RecRecorder::instance().recordingAllowed( false );

    	PRE( hasMemberNoRecord( pNode->nodeUid() ) );

    	pNode = NULL;
    	ASSERT(false, "Not yet implemented");

    	POST( not hasMemberNoRecord( pNode->nodeUid() ) );

        RecRecorder::instance().recordingAllowed( true );
    }
}

bool NetIAppSession::isValid() const
{
	return (currentStatusNoRecord() == NetAppSession::NETSESS_OK);
}

bool NetIAppSession::isValidNoRecord() const
{
	return (currentStatusNoRecord() == NetAppSession::NETSESS_OK);
}
