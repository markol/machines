/*
 * C T X J O I N . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiCtxJoin

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_CTXJOIN_HPP
#define _MACHGUI_CTXJOIN_HPP

#include "base/base.hpp"
#include "machgui/startctx.hpp"
#include "machgui/animatio.hpp"

class MachGuiSingleSelectionListBox;
class MachGuiNewGameName;
class MachGuiNetworkProtocolMode;
class MachGuiMenuButton;

class MachGuiCtxJoin : public MachGuiStartupScreenContext
// Canonical form revoked
{
public:
    MachGuiCtxJoin( MachGuiStartupScreens* pStartupScreens );
    ~MachGuiCtxJoin();

	virtual void buttonEvent( MachGuiStartupScreens::ButtonEvent );

	virtual bool okayToSwitchContext();

	virtual void update();
	
	void changeFocus();

	bool validNetworkSettings( bool isHost );

	void editingGameName( bool );
	void joinGameSelected( bool );

    void CLASS_INVARIANT;

protected:
	void showGames();
	
private:
    friend ostream& operator <<( ostream& o, const MachGuiCtxJoin& t );

    MachGuiCtxJoin( const MachGuiCtxJoin& );
    MachGuiCtxJoin& operator =( const MachGuiCtxJoin& );

	// Data members...
	MachGuiSingleSelectionListBox* pGamesList_;
	MachGuiNewGameName* pNewGameName_;
	MachGuiAnimations animations_;
	MachGuiNetworkProtocolMode* pNetworkProtocol_;
	bool editingGameName_;
	bool joinGameSelected_;
	MachGuiMenuButton* pJoinBtn_;
	MachGuiMenuButton* pCreateBtn_;
	MachGuiMenuButton* pShowGamesBtn_;
};


#endif

/* End CTXJOIN.HPP **************************************************/
