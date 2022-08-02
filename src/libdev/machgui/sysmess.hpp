/*
 * S Y S M E S S . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiSystemMessageHandler

    Handles network system messages at the machgui level.
	If ingame then the messages will be bounced down to machlog
*/

#ifndef _MACHGUI_SYSMESS_HPP
#define _MACHGUI_SYSMESS_HPP

#include "base/base.hpp"
#include "machlog/sysmess.hpp"

class MachGuiStartupScreens;

class MachGuiSystemMessageHandler : public MachLogSystemMessageHandler
// Canonical form revoked
{
public:
    MachGuiSystemMessageHandler( MachGuiStartupScreens* );
    virtual ~MachGuiSystemMessageHandler();

	//return code indicates whether or not the handler has handled the function or if the network
	//library should terminate.

	virtual bool	handleHostMessage();
	virtual bool	handleDestroyPlayerMessage( const string& );
	virtual bool	handleSessionLostMessage();

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachGuiSystemMessageHandler& t );

    MachGuiSystemMessageHandler( const MachGuiSystemMessageHandler& );
    MachGuiSystemMessageHandler& operator =( const MachGuiSystemMessageHandler& );
	bool respondAsInGame() const;

	MachGuiStartupScreens*	pStartupScreens_;
};


#endif

/* End SYSMESS.HPP **************************************************/
