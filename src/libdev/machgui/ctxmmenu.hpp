/*
 * C T X M M E N U . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiCtxMainMenu

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_CTXMMENU_HPP
#define _MACHGUI_CTXMMENU_HPP

#include "base/base.hpp"
#include "machgui/startctx.hpp"
#include "machgui/animatio.hpp"

class MachGuiCtxMainMenu : public MachGuiStartupScreenContext
// Canonical form revoked
{
public:
    MachGuiCtxMainMenu( MachGuiStartupScreens* pStartupScreens );
    ~MachGuiCtxMainMenu();

    void CLASS_INVARIANT;

	virtual void update();

	virtual bool okayToSwitchContext();

	void quit();

private:
    friend ostream& operator <<( ostream& o, const MachGuiCtxMainMenu& t );

    MachGuiCtxMainMenu( const MachGuiCtxMainMenu& );
    MachGuiCtxMainMenu& operator =( const MachGuiCtxMainMenu& );

	// Data members...
	MachGuiAnimations animations_;
	bool quit_;
};


#endif

/* End CTXMMENU.HPP *************************************************/
