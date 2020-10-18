/*
 * S Y S M E S S . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "network/session.hpp"
#include "network/sessuid.hpp"
#include "network/sysmess.hpp"
#include "network/netnet.hpp"
#include "network/node.hpp"
#include "network/netdefs.hpp"
#include "network/internal/netinet.hpp"
#include "network/internal/sessioni.hpp"


NetSystemMessageHandler::NetSystemMessageHandler()
{

    TEST_INVARIANT;
}

NetSystemMessageHandler::~NetSystemMessageHandler()
{
    TEST_INVARIANT;

}

void NetSystemMessageHandler::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const NetSystemMessageHandler& t )
{

    o << "NetSystemMessageHandler " << (void*)&t << " start" << std::endl;
    o << "NetSystemMessageHandler " << (void*)&t << " end" << std::endl;

    return o;
}

void NetSystemMessageHandler::playerHasBeenLost( const string& name )
{
	NETWORK_STREAM("NetSystemMessageHandler::playerHasBeenLost " << name << std::endl );
	NETWORK_INDENT( 2 );
//	NetNetwork::instance().netINetwork().pLocalSession_->updateNodes();

	handleDestroyPlayerMessage( name );
	NETWORK_INDENT( -2 );
	NETWORK_STREAM("NetSystemMessageHandler::playerHasBeenLost DONE\n" );
}

/* End SYSMESS.CPP **************************************************/
