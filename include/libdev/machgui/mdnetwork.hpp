/*
 * N E T W M O D E . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiNetworkProtocolMode

    An abc for network details
*/

#ifndef _MACHGUI_NETWMODE_HPP
#define _MACHGUI_NETWMODE_HPP

#include "base/base.hpp"

#include "network/netnet.hpp"

class GuiDisplayable;
class MachGuiStartupScreens;

class MachGuiNetworkProtocolMode
// Canonical form revoked
{
public:

    MachGuiNetworkProtocolMode( GuiDisplayable*, MachGuiStartupScreens* );
    virtual ~MachGuiNetworkProtocolMode( void );

    void CLASS_INVARIANT;

	virtual void setNetworkDetails( void ) = 0;

	virtual bool validNetworkDetails( bool isHost ) = 0;

	virtual void updateGUI( void ) {}

	virtual void charFocus( void ) {}

	bool connectionSet( void );
    //  Use the no record version in assertions
	bool connectionSetNoRecord( void );
	
protected:
	virtual void readNetworkDetails( void ) = 0;

	GuiDisplayable& parent();
	MachGuiStartupScreens& startupScreens();

private:
    friend ostream& operator <<( ostream& o, const MachGuiNetworkProtocolMode& t );

    MachGuiNetworkProtocolMode( const MachGuiNetworkProtocolMode& );
    MachGuiNetworkProtocolMode& operator =( const MachGuiNetworkProtocolMode& );

    bool connectionSetHelper( NetNetwork::NetNetworkStatus status );

	GuiDisplayable* pParent_;
	MachGuiStartupScreens* pStartupScreens_;
};


#endif

/* End NETWMODE.HPP *************************************************/
