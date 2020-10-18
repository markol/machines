/*
 * M D E M M O D E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiModemNetworkMode

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_MDEMMODE_HPP
#define _MACHGUI_MDEMMODE_HPP

#include "base/base.hpp"
#include "machgui/mdnetwork.hpp"

class GuiDisplayable;
class MachGuiStartupScreens;

class MachGuiModemNetworkMode : public MachGuiNetworkProtocolMode
// Canonical form revoked
{
public:
    MachGuiModemNetworkMode( GuiDisplayable* , MachGuiStartupScreens* );
    ~MachGuiModemNetworkMode( void );

    void CLASS_INVARIANT;

	virtual void setNetworkDetails( void );

	virtual void updateGUI( void );

	bool validNetworkDetails( bool isHost );

protected:
	virtual void readNetworkDetails( void );

private:
	//void previousTransition( void );

    friend ostream& operator <<( ostream& o, const MachGuiModemNetworkMode& t );

    MachGuiModemNetworkMode( const MachGuiModemNetworkMode& );
    MachGuiModemNetworkMode& operator =( const MachGuiModemNetworkMode& );

	//friend class MachGuiModemOkResponder;

	class MachGuiModemNetworkModeImpl* pimpl_;
};



#endif

/* End MDEMMODE.HPP *************************************************/
