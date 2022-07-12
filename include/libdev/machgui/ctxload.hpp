/*
 * C T X L O A D . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiCtxLoad

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_CTXLOAD_HPP
#define _MACHGUI_CTXLOAD_HPP

#include "base/base.hpp"
#include "machgui/startctx.hpp"
#include "machgui/animatio.hpp"

class MachGuiDbSavedGame;
class MachGuiSingleSelectionListBox;
class MachGuiMenuButton;

class MachGuiCtxLoad : public MachGuiStartupScreenContext
// Canonical form revoked
{
public:
    MachGuiCtxLoad( MachGuiStartupScreens* pStartupScreens );
    ~MachGuiCtxLoad();

    void CLASS_INVARIANT;

	virtual void update();

	virtual bool okayToSwitchContext();

	virtual void buttonEvent( MachGuiStartupScreens::ButtonEvent );

	void selectedSaveGame( MachGuiDbSavedGame* );

	void deleteSavedGame();

	void autoLoadGame();

protected:
	void updateSaveGameList();

private:
    friend ostream& operator <<( ostream& o, const MachGuiCtxLoad& t );

    MachGuiCtxLoad( const MachGuiCtxLoad& );
    MachGuiCtxLoad& operator =( const MachGuiCtxLoad& );

	// Data members...
	MachGuiAnimations animations_;
	MachGuiSingleSelectionListBox* pSaveGameList_;
	MachGuiDbSavedGame* pSelectedSaveGame_;
	bool autoLoadGame_;
	MachGuiMenuButton* pDeleteBtn_;
	MachGuiMenuButton* pOkBtn_;
};


#endif

/* End CTXLOAD.HPP **************************************************/
