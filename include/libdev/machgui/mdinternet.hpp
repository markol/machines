/*
 * I N E T M O D E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiInternetNetworkMode

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_INETMODE_HPP
#define _MACHGUI_INETMODE_HPP

#include "base/base.hpp"
#include "machgui/mdnetwork.hpp"

class GuiDisplayable;
class MachGuiStartupScreens;

class MachGuiInternetNetworkMode : public MachGuiNetworkProtocolMode
// Canonical form revoked
{
public:
    MachGuiInternetNetworkMode( GuiDisplayable* , MachGuiStartupScreens* );
    ~MachGuiInternetNetworkMode( void );

    void CLASS_INVARIANT;

	virtual void setNetworkDetails( void );

	virtual bool validNetworkDetails( bool isHost );

	virtual void updateGUI( void );

	virtual void charFocus( void );

protected:
	virtual void readNetworkDetails( void );

private:
    friend ostream& operator <<( ostream& o, const MachGuiInternetNetworkMode& t );

    MachGuiInternetNetworkMode( const MachGuiInternetNetworkMode& );
    MachGuiInternetNetworkMode& operator =( const MachGuiInternetNetworkMode& );

	class MachGuiInternetNetworkModeImpl* pimpl_;
};


#endif

/* End INETMODE.HPP *************************************************/
