/*
 * C T X L O A D . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/ctxload.hpp"
#include "machgui/menubutn.hpp"
#include "machgui/menutext.hpp"
#include "machgui/startup.hpp"
#include "machgui/sslistit.hpp"
#include "machgui/sslistbx.hpp"
#include "machgui/database.hpp"
#include "machgui/dbsavgam.hpp"
#include "machgui/dbplayer.hpp"
#include "machgui/msgbox.hpp"
#include "machgui/gui.hpp"
#include "machgui/scrolbar.hpp"
#include "system/pathname.hpp"
#include "machgui/internal/strings.hpp"
#include "gui/font.hpp"
#include "gui/restring.hpp"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////////////////////////

class MachGuiLoadGameListBoxItem : public MachGuiSingleSelectionListBoxItem
// Canonical form revoked
{
public:
    MachGuiLoadGameListBoxItem( MachGuiStartupScreens* pStartupScreens,
    							MachGuiSingleSelectionListBox* pListBox,
    							size_t width,
    							MachGuiDbSavedGame& savedGame,
    							MachGuiCtxLoad* pLoadCtx )
	:	MachGuiSingleSelectionListBoxItem( pStartupScreens, pListBox, width, displayText( savedGame ) ),
		savedGame_( savedGame ),
		pLoadCtx_( pLoadCtx )
	{}

    ~MachGuiLoadGameListBoxItem()
	{}

	static string displayText( MachGuiDbSavedGame& savedGame )
	{
		string retVal;
		retVal = savedGame.userFileName();

		if ( savedGame.hasPlayer() )
		{
		    MachGuiDbPlayer& player = savedGame.player();
			retVal += " (";
			retVal += player.name();
			retVal += ")";
		}
		return retVal;
	}

protected:
	virtual void select()
	{
		MachGuiSingleSelectionListBoxItem::select();

		pLoadCtx_->selectedSaveGame( &savedGame_ );
	}

private:
    MachGuiLoadGameListBoxItem( const MachGuiLoadGameListBoxItem& );
    MachGuiLoadGameListBoxItem& operator =( const MachGuiLoadGameListBoxItem& );

	// Data members...
	MachGuiDbSavedGame& savedGame_;
	MachGuiCtxLoad* pLoadCtx_;
};

//////////////////////////////////////////////////////////////////////////////////////////////

class MachGuiLoadGameDeleteMessageBoxResponder : public MachGuiMessageBoxResponder
{
public:
	MachGuiLoadGameDeleteMessageBoxResponder( MachGuiCtxLoad* pLoadCtx )
	:	pLoadCtx_( pLoadCtx )
	{}

	virtual bool okPressed()
	{
		pLoadCtx_->deleteSavedGame();
		return true;
	}

private:
	MachGuiCtxLoad* pLoadCtx_;
};

//////////////////////////////////////////////////////////////////////////////////////////////

#define LOAD_LB_MINX 75
#define LOAD_LB_MAXX 291
#define LOAD_LB_MINY 88
#define LOAD_LB_MAXY 429
#define SCROLLBAR_WIDTH  17

MachGuiCtxLoad::MachGuiCtxLoad( MachGuiStartupScreens* pStartupScreens )
:	MachGuiStartupScreenContext( pStartupScreens ),
	animations_( pStartupScreens, SysPathName("gui/menu/sg_anims.anm") ),
	autoLoadGame_( false )
{
	// Display backdrop, play correct music, switch cursor on.
	pStartupScreens->changeBackdrop( "gui/menu/sg.bmp" );
    pStartupScreens->cursorOn( true );
    pStartupScreens->desiredCdTrack( MachGuiStartupScreens::MENU_MUSIC );

	// Standard buttons...
 	pOkBtn_ 	= _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 364, 127, 555, 170 ), IDS_MENUBTN_OK, MachGuiStartupScreens::BE_DUMMY_OK ) );
	pDeleteBtn_ = _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 364, 230, 555, 274 ), IDS_MENUBTN_DELETE, MachGuiStartupScreens::BE_DELETE ) );
	MachGuiMenuButton* pCancelBtn = _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 364, 328, 555, 371 ), IDS_MENUBTN_CANCEL, MachGuiStartupScreens::EXIT ) );

	pCancelBtn->escapeControl( true );
	pOkBtn_->defaultControl( true );

	// Display save list box heading
	GuiResourceString loadHeading( IDS_MENULB_LOADGAME );
  	GuiBmpFont font( GuiBmpFont::getFont("gui/menu/largefnt.bmp") );
	MachGuiMenuText* pLoadText =
		_NEW( MachGuiMenuText( pStartupScreens,
							   Gui::Box( LOAD_LB_MINX, LOAD_LB_MINY,
							   			 LOAD_LB_MINX + font.textWidth( loadHeading.asString() ),
							   			 LOAD_LB_MINY + font.charHeight() + 2 ), IDS_MENULB_LOADGAME, "gui/menu/largefnt.bmp" ) );

	// Create save game list box
	pSaveGameList_ = _NEW( MachGuiSingleSelectionListBox( pStartupScreens,
														  Gui::Box( LOAD_LB_MINX,
														  			pLoadText->absoluteBoundary().maxCorner().y() - pStartupScreens_->yMenuOffset(),
														  			LOAD_LB_MAXX - SCROLLBAR_WIDTH,
														  			LOAD_LB_MAXY ),
														  1000, MachGuiSingleSelectionListBoxItem::reqHeight(), 1 ) );

	MachGuiVerticalScrollBar::createWholeBar( 	pStartupScreens,
												Gui::Coord( LOAD_LB_MAXX - SCROLLBAR_WIDTH, pLoadText->absoluteBoundary().maxCorner().y() - pStartupScreens_->yMenuOffset() ),
												LOAD_LB_MAXY - LOAD_LB_MINY - ( pLoadText->absoluteBoundary().maxCorner().y() - pLoadText->absoluteBoundary().minCorner().y() ),
												pSaveGameList_ );

	updateSaveGameList();

    TEST_INVARIANT;
}

MachGuiCtxLoad::~MachGuiCtxLoad()
{
    TEST_INVARIANT;

}

void MachGuiCtxLoad::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiCtxLoad& t )
{

    o << "MachGuiCtxLoad " << (void*)&t << " start" << std::endl;
    o << "MachGuiCtxLoad " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiCtxLoad::update()
{
	if ( autoLoadGame_ )
	{
		pStartupScreens_->buttonAction( MachGuiStartupScreens::BE_OK );
	}
	else
	{
		animations_.update();
	}
}

void MachGuiCtxLoad::updateSaveGameList()
{
	// Disable delete button, shift focus if necessary
	if ( pDeleteBtn_->hasFocusSet() )
	{
		pOkBtn_->hasFocus( true );
		pDeleteBtn_->hasFocus( false );
	}
	pDeleteBtn_->disabled( true );

	pSelectedSaveGame_ = NULL;
	pSaveGameList_->deleteAllItems();

	// Add previously saved games into list box.
	uint numSavedGames = MachGuiDatabase::instance().nSavedGames();

	for ( uint loop = 0; loop < numSavedGames; ++loop )
	{
		MachGuiDbSavedGame& savedGame = MachGuiDatabase::instance().savedGame( loop );
		_NEW( MachGuiLoadGameListBoxItem( pStartupScreens_, pSaveGameList_, LOAD_LB_MAXX - LOAD_LB_MINX - SCROLLBAR_WIDTH, savedGame, this ) );
	}

	// Get list to redraw.
	pSaveGameList_->childrenUpdated();
}

//virtual
bool MachGuiCtxLoad::okayToSwitchContext()
{
	if ( pStartupScreens_->lastButtonEvent() == MachGuiStartupScreens::BE_OK )
	{
		// Unload game if one is currently being played.
		pStartupScreens_->unloadGame();

		// Load game
		pStartupScreens_->loadSavedGame( pSelectedSaveGame_ );
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////

class MachGuiLoadMessageBoxResponder : public MachGuiMessageBoxResponder
{
public:
	MachGuiLoadMessageBoxResponder( MachGuiCtxLoad* pLoadCtx )
	:	pLoadCtx_( pLoadCtx )
	{}

	virtual bool okPressed()
	{
		//if ( MachGui::machinesCDIsAvailable( 2 ) )
		{
		 	pLoadCtx_->autoLoadGame();
			return true;
		}
		//return false;
	}

private:
	MachGuiCtxLoad* pLoadCtx_;
};

///////////////////////////////////////////////////////////////////////////////////////////


//virtual
void MachGuiCtxLoad::buttonEvent( MachGuiStartupScreens::ButtonEvent be )
{
	if ( be == MachGuiStartupScreens::BE_DELETE )
	{
		if ( pSelectedSaveGame_ )
		{
			// Check that current player created this game and can therefore delete it.
			if ( MachGuiDatabase::instance().hasCurrentPlayer() and
				 pSelectedSaveGame_->hasPlayer() and
				 &pSelectedSaveGame_->player() != &MachGuiDatabase::instance().currentPlayer() )
			{
				GuiStrings strings;
				strings.push_back( MachGuiDatabase::instance().currentPlayer().name() );
				pStartupScreens_->displayMsgBox( IDS_MENUMSG_SAVEDGAMENOTPLAYERS, strings );
			}
			else
 			{
				pStartupScreens_->displayMsgBox( IDS_MENUMSG_DELETESAVEDGAME, _NEW( MachGuiLoadGameDeleteMessageBoxResponder( this ) ) );
			}
		}
	}
	else if ( be == MachGuiStartupScreens::BE_DUMMY_OK )
	{
		#ifndef DEMO
		if ( not pSelectedSaveGame_ )
		{
		 	pStartupScreens_->displayMsgBox( IDS_MENUMSG_SELECTSAVEDGAME );
		}
		//else if ( MachGui::machinesCDIsAvailable( 2 )
		#ifndef PRODUCTION
		//	 or getenv("cb_dontcheckcd") )
		#else
		//	)
		#endif
		else
		{
		 	pStartupScreens_->buttonAction( MachGuiStartupScreens::BE_OK );
		}
		//else
		//{
			//pStartupScreens_->displayMsgBox( IDS_MENUMSG_INSERTCD, _NEW( MachGuiLoadMessageBoxResponder(this) ) );
		//}
		#else // DEMO defined
		if ( not pSelectedSaveGame_ )
		{
		 	pStartupScreens_->displayMsgBox( IDS_MENUMSG_SELECTSAVEDGAME );
		}
		else
		{
		 	pStartupScreens_->buttonAction( MachGuiStartupScreens::BE_OK );
		}
		#endif
	}
}

void MachGuiCtxLoad::selectedSaveGame( MachGuiDbSavedGame* pSavedGame )
{
	pSelectedSaveGame_ = pSavedGame;
	pDeleteBtn_->disabled( false );
}

void MachGuiCtxLoad::deleteSavedGame()
{
	PRE( pSelectedSaveGame_ );

	remove( pSelectedSaveGame_->externalFileName().c_str() );
	MachGuiDatabase::instance().removeSavedGame( pSelectedSaveGame_ );
	MachGuiDatabase::instance().writeDatabase();
	updateSaveGameList();
}

void MachGuiCtxLoad::autoLoadGame()
{
	autoLoadGame_ = true;
}


/* End CTXLOAD.CPP **************************************************/
