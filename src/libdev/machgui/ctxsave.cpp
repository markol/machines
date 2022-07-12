/*
 * C T X S A V E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/ctxsave.hpp"
#include "machgui/menubutn.hpp"
#include "machgui/menutext.hpp"
#include "machgui/startup.hpp"
#include "machgui/dbsavgam.hpp"
#include "machgui/dbplayer.hpp"
#include "machgui/database.hpp"
#include "machgui/sslistit.hpp"
#include "machgui/sslistbx.hpp"
#include "machgui/editlist.hpp"
#include "machgui/strtdata.hpp"
#include "machgui/msgbox.hpp"
#include "machgui/ingame.hpp"
#include "machgui/lsgextra.hpp"
#include "machgui/scrolbar.hpp"
#include "machlog/races.hpp"
#include "system/pathname.hpp"
#include "machgui/internal/strings.hpp"
#include "gui/font.hpp"
#include "gui/restring.hpp"
#include "world4d/manager.hpp"
#include "world4d/scenemgr.hpp"
#include "render/device.hpp"
#include "render/display.hpp"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////////////////////////

class MachGuiSaveGameListBoxItem : public MachGuiSingleSelectionListBoxItem
// Canonical form revoked
{
public:
    MachGuiSaveGameListBoxItem( MachGuiStartupScreens* pStartupScreens,
    							MachGuiSingleSelectionListBox* pListBox,
    							size_t width,
    							MachGuiDbSavedGame& savedGame,
    							MachGuiCtxSave* pSaveCtx )
	:	MachGuiSingleSelectionListBoxItem( pStartupScreens, pListBox, width, displayText( savedGame ) ),
		savedGame_( savedGame ),
		pSaveCtx_( pSaveCtx )
	{
		disabled_ = false;

		if ( savedGame.hasPlayer() and MachGuiDatabase::instance().hasCurrentPlayer() )
		{
			MachGuiDbPlayer* pPlayer1 = &savedGame.player();
			MachGuiDbPlayer* pPlayer2 = &MachGuiDatabase::instance().currentPlayer();

			if ( pPlayer1 != pPlayer2 )
			{
				disabled_ = true;
			}
 		}
	}

    ~MachGuiSaveGameListBoxItem()
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

    void CLASS_INVARIANT;

protected:
	virtual void doHandleMouseEnterEvent( const GuiMouseEvent& rel )
	{
		if ( not disabled_ )
		{
			MachGuiSingleSelectionListBoxItem::doHandleMouseEnterEvent( rel );
		}
	}

	virtual void doHandleMouseExitEvent( const GuiMouseEvent& rel )
	{
		if ( not disabled_ )
		{
			MachGuiSingleSelectionListBoxItem::doHandleMouseExitEvent( rel );
		}
	}

	virtual void doHandleMouseClickEvent( const GuiMouseEvent& rel )
	{
		if ( not disabled_ )
		{
			MachGuiSingleSelectionListBoxItem::doHandleMouseClickEvent( rel );
		}
	}

	virtual void select()
	{
		MachGuiSingleSelectionListBoxItem::select();

		pSaveCtx_->selectedSaveGame( &savedGame_ );
	}

	virtual void unselect()
	{
		MachGuiSingleSelectionListBoxItem::unselect();

		pSaveCtx_->clearSelectedSaveGame();
	}

private:
    MachGuiSaveGameListBoxItem( const MachGuiSaveGameListBoxItem& );
    MachGuiSaveGameListBoxItem& operator =( const MachGuiSaveGameListBoxItem& );

	// Data members...
	MachGuiDbSavedGame& savedGame_;
	MachGuiCtxSave* pSaveCtx_;
	bool disabled_;
};

//////////////////////////////////////////////////////////////////////////////////////////////

class MachGuiSaveGameDeleteMessageBoxResponder : public MachGuiMessageBoxResponder
{
public:
	MachGuiSaveGameDeleteMessageBoxResponder( MachGuiCtxSave* pSaveCtx )
	:	pSaveCtx_( pSaveCtx )
	{}

	virtual bool okPressed()
	{
		pSaveCtx_->deleteSavedGame();
		return true;
	}

private:
	MachGuiCtxSave* pSaveCtx_;
};

//////////////////////////////////////////////////////////////////////////////////////////////

#define SAVE_LB_MINX  98
#define SAVE_LB_MAXX  312
#define SAVE_LB_MINY  72
#define SAVE_LB_MAXY  412
#define SCROLLBAR_WIDTH  17


MachGuiCtxSave::MachGuiCtxSave( MachGuiStartupScreens* pStartupScreens )
:	MachGuiStartupScreenContext( pStartupScreens ),
	animations_( pStartupScreens, SysPathName("gui/menu/sh_anims.anm") ),
	pSelectedSaveGame_( NULL )
{
	// Display backdrop, play correct music, switch cursor on.
	pStartupScreens->changeBackdrop( "gui/menu/sh.bmp" );
    pStartupScreens->cursorOn( true );
    pStartupScreens->desiredCdTrack( MachGuiStartupScreens::MENU_MUSIC );

	// Standard buttons...
	pOkBtn_ 	= _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 387, 111, 578, 154 ), IDS_MENUBTN_OK, MachGuiStartupScreens::BE_OK ) );
	pDeleteBtn_ = _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 387, 215, 578, 258 ), IDS_MENUBTN_DELETE, MachGuiStartupScreens::BE_DELETE ) );
	MachGuiMenuButton* pCancelBtn = _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 387, 312, 578, 356 ), IDS_MENUBTN_CANCEL, MachGuiStartupScreens::EXIT ) );

	pCancelBtn->escapeControl( true );
	pOkBtn_->defaultControl( true );

	// Display save list box heading
	GuiResourceString saveHeading( IDS_MENULB_SAVEGAME );
  	GuiBmpFont font( GuiBmpFont::getFont("gui/menu/largefnt.bmp") );
	MachGuiMenuText* pSaveText =
		_NEW( MachGuiMenuText( pStartupScreens,
							   Gui::Box( SAVE_LB_MINX, SAVE_LB_MINY,
							   			 SAVE_LB_MINX + font.textWidth( saveHeading.asString() ),
							   			 SAVE_LB_MINY + font.charHeight() + 2 ), IDS_MENULB_SAVEGAME, "gui/menu/largefnt.bmp" ) );


	// Create save game list box
	pSaveGameList_ = _NEW( MachGuiSingleSelectionListBox( pStartupScreens,
														  Gui::Box( SAVE_LB_MINX,
														  			pSaveText->absoluteBoundary().maxCorner().y() - pStartupScreens_->yMenuOffset(),
														  			SAVE_LB_MAXX - SCROLLBAR_WIDTH,
														  			SAVE_LB_MAXY ),
														  1000, MachGuiSingleSelectionListBoxItem::reqHeight(), 1 ) );

	MachGuiVerticalScrollBar::createWholeBar( 	pStartupScreens,
												Gui::Coord( SAVE_LB_MAXX - SCROLLBAR_WIDTH, pSaveText->absoluteBoundary().maxCorner().y() - pStartupScreens_->yMenuOffset() ),
												SAVE_LB_MAXY - SAVE_LB_MINY - ( pSaveText->absoluteBoundary().maxCorner().y() - pSaveText->absoluteBoundary().minCorner().y() ),
												pSaveGameList_ );

	updateSaveGameList();

    TEST_INVARIANT;
}

MachGuiCtxSave::~MachGuiCtxSave()
{
    TEST_INVARIANT;

}

void MachGuiCtxSave::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiCtxSave& t )
{

    o << "MachGuiCtxSave " << (void*)&t << " start" << std::endl;
    o << "MachGuiCtxSave " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiCtxSave::update()
{
	animations_.update();
	pNewSaveGameName_->update();
}

void MachGuiCtxSave::updateSaveGameList()
{
	clearSelectedSaveGame();
	pSaveGameList_->deleteAllItems();

	// Create special [new save game] entry in list box.
	GuiResourceString newSaveGameStr( IDS_MENU_NEWSAVEGAME );
	pNewSaveGameName_ = _NEW( MachGuiEditBoxListBoxItem( pStartupScreens_, pSaveGameList_, SAVE_LB_MAXX - SAVE_LB_MINX - SCROLLBAR_WIDTH, newSaveGameStr.asString() ) );
	pNewSaveGameName_->maxChars( MAX_SAVEGAMENAME_LEN );

	// Add previously saved games into list box.
	uint numSavedGames = MachGuiDatabase::instance().nSavedGames();

	for ( uint loop = 0; loop < numSavedGames; ++loop )
	{
		MachGuiDbSavedGame& savedGame = MachGuiDatabase::instance().savedGame( loop );
		_NEW( MachGuiSaveGameListBoxItem( pStartupScreens_, pSaveGameList_, SAVE_LB_MAXX - SAVE_LB_MINX - SCROLLBAR_WIDTH, savedGame, this ) );
	}

	// Get list to redraw.
	pSaveGameList_->childrenUpdated();
}

//virtual
bool MachGuiCtxSave::okayToSwitchContext()
{
	if ( pStartupScreens_->lastButtonEvent() == MachGuiStartupScreens::BE_OK )
	{
		// Create new save game...
		if ( pNewSaveGameName_->selected() )
		{
			if ( pNewSaveGameName_->text() != "" )
			{
				// Check for unique saved game name
				bool uniqueSaveGameName = true;
 				uint numSavedGames = MachGuiDatabase::instance().nSavedGames();

				for ( uint loop = 0; loop < numSavedGames; ++loop )
				{
					MachGuiDbSavedGame& savedGame = MachGuiDatabase::instance().savedGame( loop );
//					if ( stricmp( savedGame.userFileName().c_str(), pNewSaveGameName_->text().c_str() ) == 0 )
					if ( strcasecmp( savedGame.userFileName().c_str(), pNewSaveGameName_->text().c_str() ) == 0 )
					{
						uniqueSaveGameName = false;
					}
				}

				if ( uniqueSaveGameName )
				{
					// Save the game, the name is unique so we can save it...
					bool saveSuccess = saveGame( pNewSaveGameName_->text() );

					if ( not saveSuccess )
					{
						// Display error message, game failed to save...
						pStartupScreens_->displayMsgBox( IDS_MENUMSG_SAVEFAIL );
					}

					return saveSuccess;
				}
				else
				{
					// Display error message, name is not unique...
					pStartupScreens_->displayMsgBox( IDS_MENUMSG_SAVEDGAMENAMEEXISTS );
					return false;
				}
			}
			else
			{
				pStartupScreens_->displayMsgBox( IDS_MENUMSG_ENTERSAVEGAMENAME );
				return false;
			}
		}
		else // Select previously saved game ( save over )...
		{
			if ( not pSelectedSaveGame_ )
			{
				pStartupScreens_->displayMsgBox( IDS_MENUMSG_ENTERSAVEGAMENAME );
				return false;
			}
			else
			{
				string saveDisplayName = pSelectedSaveGame_->userFileName();
				deleteSavedGame();
				saveGame( saveDisplayName );
				return true;
			}
		}
	}

	return true;
}

//virtual
void MachGuiCtxSave::buttonEvent( MachGuiStartupScreens::ButtonEvent be )
{
	if ( be == MachGuiStartupScreens::BE_DELETE )
	{
		if ( pSelectedSaveGame_ )
		{
			pStartupScreens_->displayMsgBox( IDS_MENUMSG_DELETESAVEDGAME, _NEW( MachGuiSaveGameDeleteMessageBoxResponder( this ) ) );
		}
	}
}

void MachGuiCtxSave::selectedSaveGame( MachGuiDbSavedGame* pSavedGame )
{
	pSelectedSaveGame_ = pSavedGame;
	pDeleteBtn_->disabled( false );
}

void MachGuiCtxSave::clearSelectedSaveGame()
{
	// Disable delete button, shift focus if necessary
	pSelectedSaveGame_ = NULL;
	if ( pDeleteBtn_->hasFocusSet() )
	{
		pOkBtn_->hasFocus( true );
		pDeleteBtn_->hasFocus( false );
	}

	pDeleteBtn_->disabled( true );
}

void MachGuiCtxSave::deleteSavedGame()
{
	PRE( pSelectedSaveGame_ );

	remove( pSelectedSaveGame_->externalFileName().c_str() );
	MachGuiDatabase::instance().removeSavedGame( pSelectedSaveGame_ );
	MachGuiDatabase::instance().writeDatabase();
	updateSaveGameList();
}

bool MachGuiCtxSave::saveGame( const string& saveDisplayName )
{
	// Display saving bmp
	GuiBitmap savingBmp = Gui::bitmap( "gui/menu/saving.bmp" );
	savingBmp.enableColourKeying();
	GuiBitmap frontBuffer = W4dManager::instance().sceneManager()->pDevice()->frontSurface();
	frontBuffer.simpleBlit( savingBmp, pStartupScreens_->xMenuOffset(), pStartupScreens_->yMenuOffset() );
	W4dManager::instance().sceneManager()->pDevice()->display()->flipBuffers();

	// Create next filename...
    bool gotSavePathName = false;
    SysPathName savePathName;
    size_t count = 0;

	while ( not gotSavePathName )
	{
		char buffer[20];

        sprintf( buffer, "%04ld", count );

        //savePathName = string( "savegame/save" ) + buffer + ".sav";
        savePathName = SysPathName(string( "savegame/save" ) + buffer + ".sav");

        if( not savePathName.existsAsFile() )
            gotSavePathName = true;

        ++count;
	}

 	// Save game...
	MachGuiLoadSaveGameExtras lsgExtras( &pStartupScreens_->inGameScreen() );

   	bool saveSuccess = MachLogRaces::instance().saveGame( savePathName, &lsgExtras );

	if ( saveSuccess )
	{
		// Store reference to save file in database file...
		MachGuiDbSavedGame* pNewSaveGame = _NEW( MachGuiDbSavedGame( saveDisplayName, savePathName.c_str(), pStartupScreens_->startupData()->scenario() ) );

		if ( MachGuiDatabase::instance().hasCurrentPlayer() )
		{
			pNewSaveGame->player( &MachGuiDatabase::instance().currentPlayer() );
		}

		if ( pStartupScreens_->gameType() == MachGuiStartupScreens::CAMPAIGNGAME )
		{
			pNewSaveGame->isCampaignGame( true );
		}

		MachGuiDatabase::instance().addSavedGame( pNewSaveGame );
		MachGuiDatabase::instance().writeDatabase();
	}

	return saveSuccess;
}

/* End CTXSAVE.CPP **************************************************/
