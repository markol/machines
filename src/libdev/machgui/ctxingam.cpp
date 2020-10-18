/*
 * C T X I N G A M . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "network/netnet.hpp"
#include "machlog/network.hpp"
#include "machgui/ctxingam.hpp"
#include "system/pathname.hpp"
#include "machgui/menubutn.hpp"
#include "machgui/database.hpp"
#include "machgui/startup.hpp"
#include "machgui/msgboxei.hpp"
#include "machgui/internal/strings.hpp"

#include "world4d/soundman.hpp"

MachGuiCtxInGameOptions::MachGuiCtxInGameOptions( MachGuiStartupScreens* pStartupScreens )
:	MachGuiStartupScreenContext( pStartupScreens ),
	animations_( pStartupScreens, SysPathName("gui/menu/se_anims.anm") )
{
	// Setup all the buttons on this screen.
  	MachGuiMenuButton* pContinueBtn	= _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 90, 72,  281, 115  ), IDS_MENUBTN_CONTINUE, MachGuiStartupScreens::CONTINUE ) );
	MachGuiMenuButton* pBriefingBtn = _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 90, 178, 281, 222  ), IDS_MENUBTN_BRIEFING, MachGuiStartupScreens::BRIEFING ) );
	MachGuiMenuButton* pRestartBtn 	= _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 90, 282, 281, 326  ), IDS_MENUBTN_RESTART, MachGuiStartupScreens::RESTART ) );
	MachGuiMenuButton* pHotkeysBtn 	= _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 90, 377, 281, 420  ), IDS_MENUBTN_HOTKEYS, MachGuiStartupScreens::HOTKEYS ) );
	MachGuiMenuButton* pOptionsBtn	= _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 344, 72,  536, 114 ), IDS_MENUBTN_OPTIONS, MachGuiStartupScreens::OPTIONS ) );
	MachGuiMenuButton* pSaveBtn 	= _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 344, 178, 536, 221 ), IDS_MENUBTN_SAVEGAME, MachGuiStartupScreens::SAVEGAME ) );
	MachGuiMenuButton* pLoadBtn 	= _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 344, 282, 536, 325 ), IDS_MENUBTN_LOADGAME, MachGuiStartupScreens::LOADGAME ) );
	MachGuiMenuButton* pExitBtn;
	if( not NetNetwork::instance().isLobbiedGame() )
		pExitBtn = _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 344, 377, 536, 419 ), IDS_MENUBTN_EXITGAME, MachGuiStartupScreens::EXIT ) );
	else
		pExitBtn = _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 344, 377, 536, 419 ), IDS_MENUBTN_EXIT_TO_ZONE, MachGuiStartupScreens::EXIT ) );

	// Setup nav overrides
	pContinueBtn->setRightNavControl( pOptionsBtn );
	pContinueBtn->setLeftNavControl( pOptionsBtn );
	pContinueBtn->setUpNavControl( pHotkeysBtn );
	pContinueBtn->setDownNavControl( pBriefingBtn );

	pBriefingBtn->setRightNavControl( pSaveBtn );
	pBriefingBtn->setLeftNavControl( pSaveBtn );
	pBriefingBtn->setUpNavControl( pContinueBtn );
	pBriefingBtn->setDownNavControl( pRestartBtn );

	pRestartBtn->setRightNavControl( pLoadBtn );
	pRestartBtn->setLeftNavControl( pLoadBtn );
	pRestartBtn->setUpNavControl( pBriefingBtn );
	pRestartBtn->setDownNavControl( pHotkeysBtn );

	pHotkeysBtn->setRightNavControl( pExitBtn );
	pHotkeysBtn->setLeftNavControl( pExitBtn );
	pHotkeysBtn->setUpNavControl( pRestartBtn );
	pHotkeysBtn->setDownNavControl( pContinueBtn );

	pOptionsBtn->setRightNavControl( pContinueBtn );
	pOptionsBtn->setLeftNavControl( pContinueBtn );
	pOptionsBtn->setUpNavControl( pExitBtn );
	pOptionsBtn->setDownNavControl( pSaveBtn );

	pSaveBtn->setRightNavControl( pBriefingBtn );
	pSaveBtn->setLeftNavControl( pBriefingBtn );
	pSaveBtn->setUpNavControl( pOptionsBtn );
	pSaveBtn->setDownNavControl( pLoadBtn );

	pLoadBtn->setRightNavControl( pRestartBtn );
	pLoadBtn->setLeftNavControl( pRestartBtn );
	pLoadBtn->setUpNavControl( pSaveBtn );
	pLoadBtn->setDownNavControl( pExitBtn );

	pExitBtn->setRightNavControl( pHotkeysBtn );
	pExitBtn->setLeftNavControl( pHotkeysBtn );
	pExitBtn->setUpNavControl( pLoadBtn );
	pExitBtn->setDownNavControl( pOptionsBtn );

	// Disable buttons that don't make sense in particular game modes
#ifdef DEMO
	pLoadBtn->disabled( true );
	pSaveBtn->disabled( true );
	pHotkeysBtn->disabled( true );
#endif
	if ( pStartupScreens->gameType() == MachGuiStartupScreens::MULTIGAME )
	{
		pBriefingBtn->disabled( true );
		pRestartBtn->disabled( true );
		pSaveBtn->disabled( true );
		pLoadBtn->disabled( true );
	}
	else if ( pStartupScreens->gameType() == MachGuiStartupScreens::SKIRMISHGAME )
	{
		pBriefingBtn->disabled( true );
	}

	pStartupScreens->changeBackdrop( "gui/menu/se.bmp" ); 
    pStartupScreens->cursorOn( true );
    pStartupScreens->desiredCdTrack( MachGuiStartupScreens::MENU_MUSIC );

    TEST_INVARIANT;
}

MachGuiCtxInGameOptions::~MachGuiCtxInGameOptions()
{
    TEST_INVARIANT;

}

void MachGuiCtxInGameOptions::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiCtxInGameOptions& t )
{

    o << "MachGuiCtxInGameOptions " << (void*)&t << " start" << std::endl;
    o << "MachGuiCtxInGameOptions " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiCtxInGameOptions::update()
{
	animations_.update();
}

//virtual 
bool MachGuiCtxInGameOptions::okayToSwitchContext()
{
	if ( pStartupScreens_->lastButtonEvent() == MachGuiStartupScreens::EXIT )
	{
		if( NetNetwork::instance().isLobbiedGame() )
		{
		 	pStartupScreens_->displayMsgBox( 
		 		IDS_MENUMSG_QUIT, 
		 		_NEW( MachGuiExitToInternetMessageBoxResponder( 
		 			pStartupScreens_, 
		 			MachGuiExitToInternetMessageBoxResponder::UNLOAD_GAME )
		 		)
		 		, true );
			return false;
		}

		pStartupScreens_->unloadGame();
		W4dSoundManager::instance().unloadAll();
		//if we got into this screen via a lobby session then we need to terminate correctly at this point.
	}

	return true;
}

//virtual 
void MachGuiCtxInGameOptions::buttonEvent( MachGuiStartupScreens::ButtonEvent be )
{
	PRE( pStartupScreens_->gameType() != MachGuiStartupScreens::NOGAME );
	PRE( pStartupScreens_->gameType() != MachGuiStartupScreens::MULTIGAME );

	if ( be == MachGuiStartupScreens::RESTART )
	{
		pStartupScreens_->restartGame();
	}
}


/* End CTXINGAM.CPP *************************************************/
