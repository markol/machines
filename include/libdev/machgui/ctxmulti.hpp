/*
 * C T X M U L T I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiCtxMultiplayer

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_CTXMULTI_HPP
#define _MACHGUI_CTXMULTI_HPP

#include "base/base.hpp"
#include "machgui/startctx.hpp"
#include "machgui/animatio.hpp"

class MachGuiSingleLineEditBox;

class MachGuiCtxMultiplayer	: public MachGuiStartupScreenContext
// Canonical form revoked
{
public:
    MachGuiCtxMultiplayer( MachGuiStartupScreens* );
    ~MachGuiCtxMultiplayer();

	virtual bool okayToSwitchContext();
	virtual void update();

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachGuiCtxMultiplayer& t );

    MachGuiCtxMultiplayer( const MachGuiCtxMultiplayer& );
    MachGuiCtxMultiplayer& operator =( const MachGuiCtxMultiplayer& );

	MachGuiSingleLineEditBox* pSingleLineEditBox_;
	MachGuiAnimations animations_;
};


#endif

/* End CTXMULTI.HPP *************************************************/
