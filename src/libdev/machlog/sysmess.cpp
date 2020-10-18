/*
 * S Y S M E S S . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "ctl/list.hpp"
#include "machlog/sysmess.hpp"
#include "machlog/network.hpp"
#include "machlog/races.hpp"
#include "machlog/actor.hpp"

MachLogSystemMessageHandler::MachLogSystemMessageHandler()
{

    TEST_INVARIANT;
}

MachLogSystemMessageHandler::~MachLogSystemMessageHandler()
{
    TEST_INVARIANT;

}

void MachLogSystemMessageHandler::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogSystemMessageHandler& t )
{

    o << "MachLogSystemMessageHandler " << (void*)&t << " start" << std::endl;
    o << "MachLogSystemMessageHandler " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual 
bool	MachLogSystemMessageHandler::handleHostMessage()
{
	return false;
}

//virtual 
bool	MachLogSystemMessageHandler::handleDestroyPlayerMessage( const string& /*playerString*/ )
{
	return false;
}

//virtual 
bool	MachLogSystemMessageHandler::handleSessionLostMessage()
{
	return false;
}

bool MachLogSystemMessageHandler::logicalHandleDestroyPlayerMessage( MachPhys::Race r )
{
	//someone has fallen out of the network so destroy all their objects
//	if( MachLogNetwork::instance().isNodeLogicalHost() )
	{
		for( MachLogRaces::Objects::iterator i = MachLogRaces::instance().raceObjects( r ).begin(); i != MachLogRaces::instance().raceObjects( r ).end(); ++i )
			(*i)->beHit( 10000 );
	}
	return true;
}

/* End SYSMESS.CPP **************************************************/
