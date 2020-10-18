/*
 * C T X I N G A M . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiCtxInGameOptions

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_CTXINGAM_HPP
#define _MACHGUI_CTXINGAM_HPP

#include "base/base.hpp"
#include "machgui/startctx.hpp"
#include "machgui/animatio.hpp"

class MachGuiCtxInGameOptions : public MachGuiStartupScreenContext
// Canonical form revoked
{
public:
    MachGuiCtxInGameOptions( MachGuiStartupScreens* pStartupScreens );
    ~MachGuiCtxInGameOptions( void );

    void CLASS_INVARIANT;

	virtual void update();

	virtual bool okayToSwitchContext();

	virtual void buttonEvent( MachGuiStartupScreens::ButtonEvent );

private:
    friend ostream& operator <<( ostream& o, const MachGuiCtxInGameOptions& t );

    MachGuiCtxInGameOptions( const MachGuiCtxInGameOptions& );
    MachGuiCtxInGameOptions& operator =( const MachGuiCtxInGameOptions& );

	// Data members...
	MachGuiAnimations animations_;
};


#endif

/* End CTXINGAM.HPP *************************************************/
