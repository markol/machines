/*
 * C T X H O T K Y . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiCtxHotKeys

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_CTXHOTKY_HPP
#define _MACHGUI_CTXHOTKY_HPP

#include "base/base.hpp"
#include "machgui/startctx.hpp"
#include "machgui/animatio.hpp"


class MachGuiCtxHotKeys : public MachGuiStartupScreenContext
// Canonical form revoked
{
public:
    MachGuiCtxHotKeys( MachGuiStartupScreens* pStartupScreens );
    ~MachGuiCtxHotKeys( void );

    void CLASS_INVARIANT;

	virtual void update();

private:
    friend ostream& operator <<( ostream& o, const MachGuiCtxHotKeys& t );

    MachGuiCtxHotKeys( const MachGuiCtxHotKeys& );
    MachGuiCtxHotKeys& operator =( const MachGuiCtxHotKeys& );

	void readHotkeyData( const string& hotKeyDataFileName, string& hotkeyString, uint& linesInString );

	// Data members...
	MachGuiAnimations animations_;
};


#endif

/* End CTXHOTKY.HPP *************************************************/
