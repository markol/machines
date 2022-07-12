/*
 * N E T W M O D E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/mdnetwork.hpp"
#include "machgui/internal/strings.hpp"
#include "network/netnet.hpp"
#include "machgui/startup.hpp"
#include "machlog/network.hpp"

MachGuiNetworkProtocolMode::MachGuiNetworkProtocolMode(	GuiDisplayable* pParent,
														MachGuiStartupScreens* pStartupScreens )
:	pParent_( pParent ),
	pStartupScreens_( pStartupScreens )
{

    TEST_INVARIANT;
}

MachGuiNetworkProtocolMode::~MachGuiNetworkProtocolMode()
{
    TEST_INVARIANT;

}

void MachGuiNetworkProtocolMode::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

bool MachGuiNetworkProtocolMode::connectionSet()
{
	return connectionSetHelper( NetNetwork::instance().currentStatus() );
}

bool MachGuiNetworkProtocolMode::connectionSetNoRecord()
{
	return connectionSetHelper( NetNetwork::instance().currentStatusNoRecord() );
}

//  This function exists purely to allow connectionSet and connectionSetNoRecord
//  to share code. I am not happy with the way the whole network recording thing
//  is working - it's all getting too complicated.
bool MachGuiNetworkProtocolMode::connectionSetHelper( NetNetwork::NetNetworkStatus status )
{
	uint error = IDS_MENUMSG_NETCONNECTIONERROR;
	bool valid = false;

	switch( status )
	{
		case NetNetwork::NETNET_CONNECTIONERROR: error = IDS_MENUMSG_NETCONNECTIONERROR; break;
		case NetNetwork::NETNET_INVALIDCONNECTIONDATA: error = IDS_MENUMSG_NETINVALIDCONNECTIONDATA; break;
		case NetNetwork::NETNET_INVALIDDATA: error = IDS_MENUMSG_NETINVALIDDATA; break;
		case NetNetwork::NETNET_UNDEFINEDERROR:	error = IDS_MENUMSG_NETUNDEFINEDERROR; break;
		case NetNetwork::NETNET_CANTINITIALIZEDIRECTPLAY: error = IDS_MENUMSG_NETCANTINITIALIZEDIRECTPLAY; break;
		case NetNetwork::NETNET_MEMORYERROR: error = IDS_MENUMSG_NETMEMORYERROR; break;
		case NetNetwork::NETNET_SESSIONERROR: error = IDS_MENUMSG_NETSESSIONERROR; break;
		case NetNetwork::NETNET_NODEERROR: error = IDS_MENUMSG_NETNODEERROR; break;
		case NetNetwork::NETNET_OK: error = IDS_MENUMSG_NETSUCCESS; valid = true; break;
	}
	if( not valid )
	{
		startupScreens().displayMsgBox( error );
		// Set network to a valid state
		MachLogNetwork::instance().terminateAndReset();
	}

	POST( NetNetwork::instance().currentStatusNoRecord() == NetNetwork::NETNET_OK );

	return valid;
}

GuiDisplayable& MachGuiNetworkProtocolMode::parent()
{
	return *pParent_;
}

MachGuiStartupScreens& MachGuiNetworkProtocolMode::startupScreens()
{
	return *pStartupScreens_;
}

ostream& operator <<( ostream& o, const MachGuiNetworkProtocolMode& t )
{

    o << "MachGuiNetworkProtocolMode " << (void*)&t << " start" << std::endl;
    o << "MachGuiNetworkProtocolMode " << (void*)&t << " end" << std::endl;

    return o;
}

/* End NETWMODE.CPP *************************************************/
