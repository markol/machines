/*
 * S R L M O D E . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiSerialNetworkMode

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_SRLMODE_HPP
#define _MACHGUI_SRLMODE_HPP

#include "machgui/mdnetwork.hpp"
#include "base/base.hpp"

class GuiDisplayable;
class MachGuiStartupScreens;

class MachGuiSerialNetworkMode : public MachGuiNetworkProtocolMode
// Canonical form revoked
{
public:
    MachGuiSerialNetworkMode( GuiDisplayable* , MachGuiStartupScreens* );
    ~MachGuiSerialNetworkMode( void );

    void CLASS_INVARIANT;

	virtual void setNetworkDetails( void );

	virtual bool validNetworkDetails( bool isHost );

protected:
	virtual void readNetworkDetails( void );

private:
    friend ostream& operator <<( ostream& o, const MachGuiSerialNetworkMode& t );

    MachGuiSerialNetworkMode( const MachGuiSerialNetworkMode& );
    MachGuiSerialNetworkMode& operator =( const MachGuiSerialNetworkMode& );

	class MachGuiSerialNetworkModeImpl* pimpl_;
};


#endif

/* End SRLMODE.HPP **************************************************/
