/*
 * C T X C A M P . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/ctxcamp.hpp"
#include "machgui/menubutn.hpp"
#include "machgui/menutext.hpp"
#include "machgui/startup.hpp"
#include "machgui/editlist.hpp"
#include "machgui/sslistbx.hpp"
#include "machgui/database.hpp"
#include "machgui/dbplayer.hpp"
#include "machgui/dbsavgam.hpp"
#include "machgui/strtdata.hpp"
#include "machgui/dbplayer.hpp"
#include "machgui/msgbox.hpp"
#include "machgui/dbplyscn.hpp"
#include "machgui/dbscenar.hpp"
#include "system/registry.hpp"
#include "gui/font.hpp"
#include "gui/restring.hpp"
#include "machgui/internal/strings.hpp"
#include "ctl/vector.hpp"

#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////////////////////////

class MachGuiPlayerListBoxItem : public MachGuiSingleSelectionListBoxItem
// Canonical form revoked
{
public:
    MachGuiPlayerListBoxItem( 	MachGuiStartupScreens* pStartupScreens,
    							MachGuiSingleSelectionListBox* pListBox,
    							size_t width,
    							MachGuiDbPlayer& player,
    							MachGuiCtxCampaign* pCampaignCtx )
	:	MachGuiSingleSelectionListBoxItem( pStartupScreens, pListBox, width, player.name() ),
		player_( player ),
		pCampaignCtx_( pCampaignCtx )
	{
		// Select this list box item if it was the last one selected
		if ( player.name() == SysRegistry::instance().queryStringValue( "Misc", "Current Player Name", SysRegistry::CURRENT_USER ) )
		{
			selectThisItem();
		}
	}

    ~MachGuiPlayerListBoxItem()
	{}

    void CLASS_INVARIANT;

protected:
	virtual void select()
	{
		MachGuiSingleSelectionListBoxItem::select();

		pCampaignCtx_->selectedPlayer( &player_ );
		MachGuiDatabase::instance().currentPlayer( &player_ );

		// Store the fact that this item was just selected
		SysRegistry::instance().setStringValue( "Misc", "Current Player Name", player_.name(), SysRegistry::CURRENT_USER );
	}

	virtual void unselect()
	{
		MachGuiSingleSelectionListBoxItem::unselect();

		pCampaignCtx_->clearSelectedPlayer();
	}

private:
    MachGuiPlayerListBoxItem( const MachGuiPlayerListBoxItem& );
    MachGuiPlayerListBoxItem& operator =( const MachGuiPlayerListBoxItem& );

	// Data members...
	MachGuiDbPlayer& player_;
	MachGuiCtxCampaign* pCampaignCtx_;
};

//////////////////////////////////////////////////////////////////////////////////////////////

class MachGuiPlayerDeleteMessageBoxResponder : public MachGuiMessageBoxResponder
{
public:
	MachGuiPlayerDeleteMessageBoxResponder( MachGuiCtxCampaign* pCampaignCtx )
	:	pCampaignCtx_( pCampaignCtx )
	{}

	virtual bool okPressed()
	{
		pCampaignCtx_->deletePlayer();
		return true;
	}

private:
	MachGuiCtxCampaign* pCampaignCtx_;
};

//////////////////////////////////////////////////////////////////////////////////////////////

MachGuiCtxCampaign::MachGuiCtxCampaign( MachGuiStartupScreens* pStartupScreens )
:	MachGuiStartupScreenContext( pStartupScreens ),
	animations_( pStartupScreens, SysPathName("gui/menu/sm_anims.anm") ),
	pSelectedPlayer_( NULL ),
	pScenarioText_( NULL ),
	pLastScenarioText_( NULL ),
	pScoreText_( NULL ),
	pLastScoreText_( NULL ),
	pNumScenariosPlayedText_( NULL ),
	pNumScenariosText_( NULL ),
	pTotalScoreHeadingText_( NULL ),
	pTotalScoreText_( NULL )
{
	const int PLAYERS_LB_MINX = 84;
	const int PLAYERS_LB_MINY = 93;
	const int PLAYERS_LB_MAXX = 237;
	const int PLAYERS_LB_MAXY = 378;
	const int CURRENTSTATUS_MINX = 378;
	const int CURRENTSTATUS_MINY = 225;
	const int CURRENTSTATUS_MAXX = 532;
	const int CURRENTSTATUS_MAXY = 376;

	MachGuiMenuButton* pOkBtn = _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 83, 408, 255, 445 ), IDS_MENUBTN_OK, MachGuiStartupScreens::BE_OK ) );
	MachGuiMenuButton* pCancelBtn = _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 327, 408, 497, 445 ), IDS_MENUBTN_CANCEL, MachGuiStartupScreens::EXIT ) );
	_NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 376, 40, 546, 77 ), IDS_MENUBTN_NEW, MachGuiStartupScreens::BE_NEW ) );
	_NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 376, 134, 546, 172 ), IDS_MENUBTN_DELETE, MachGuiStartupScreens::BE_DELETE ) );

	pCancelBtn->escapeControl( true );
	pOkBtn->defaultControl( true );

	// Display list box heading
	GuiResourceString players( IDS_MENULB_PLAYERS );
  	GuiBmpFont font( GuiBmpFont::getFont("gui/menu/largefnt.bmp") );
	MachGuiMenuText* pPlayersText =
		_NEW( MachGuiMenuText( pStartupScreens,
							   Gui::Box( PLAYERS_LB_MINX, PLAYERS_LB_MINY,
							   			 PLAYERS_LB_MINX + font.textWidth( players.asString() ),
							   			 PLAYERS_LB_MINY + font.charHeight() + 2 ), IDS_MENULB_PLAYERS, "gui/menu/largefnt.bmp" ) );

	// Display current status heading
	GuiResourceString status( IDS_MENU_PLAYERSTATUS );
	MachGuiMenuText* pCurrentStatusHeading =
		_NEW( MachGuiMenuText( pStartupScreens,
							   Gui::Box( CURRENTSTATUS_MINX, CURRENTSTATUS_MINY,
							   			 CURRENTSTATUS_MINX + font.textWidth( status.asString() ),
							   			 CURRENTSTATUS_MINY + font.charHeight() + 2 ), IDS_MENU_PLAYERSTATUS, "gui/menu/largefnt.bmp" ) );

	// Create players list box
	pPlayersList_ = _NEW( MachGuiSingleSelectionListBox( pStartupScreens,
														 Gui::Box( PLAYERS_LB_MINX, pPlayersText->absoluteBoundary().maxCorner().y() - pStartupScreens_->yMenuOffset(), PLAYERS_LB_MAXX, PLAYERS_LB_MAXY),
														 1000, MachGuiSingleSelectionListBoxItem::reqHeight(), 1 ) );
	updatePlayersList();

	// Display backdrop, play correct music, switch cursor on.
	pStartupScreens->changeBackdrop( "gui/menu/sm.bmp" );
    pStartupScreens->cursorOn( true );
    pStartupScreens->desiredCdTrack( MachGuiStartupScreens::MENU_MUSIC );

    TEST_INVARIANT;
}

MachGuiCtxCampaign::~MachGuiCtxCampaign()
{
    TEST_INVARIANT;

}

void MachGuiCtxCampaign::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiCtxCampaign& t )
{

    o << "MachGuiCtxCampaign " << (void*)&t << " start" << std::endl;
    o << "MachGuiCtxCampaign " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiCtxCampaign::update()
{
	animations_.update();
	pNewPlayerName_->update();
}

void MachGuiCtxCampaign::updatePlayersList()
{
	const int PLAYERS_LB_MINX = 84;
	const int PLAYERS_LB_MINY = 93;
	const int PLAYERS_LB_MAXX = 237;
	const int PLAYERS_LB_MAXY = 378;

	pPlayersList_->deleteAllItems();
	pSelectedPlayer_ = NULL;

	// Create special [new player name] entry in list box.
	GuiResourceString newPlayerNameStr( IDS_MENU_NEWPLAYERNAME );
	pNewPlayerName_ = _NEW( MachGuiEditBoxListBoxItem( pStartupScreens_, pPlayersList_, PLAYERS_LB_MAXX - PLAYERS_LB_MINX, newPlayerNameStr.asString() ) );
	pNewPlayerName_->maxChars( MAX_PLAYERNAME_LEN );

	// Add previously created players into list box.
	uint numPlayers = MachGuiDatabase::instance().nPlayers();

	for ( uint loop = 0; loop < numPlayers; ++loop )
	{
		MachGuiDbPlayer& player = MachGuiDatabase::instance().player( loop );
		_NEW( MachGuiPlayerListBoxItem( pStartupScreens_, pPlayersList_, PLAYERS_LB_MAXX - PLAYERS_LB_MINX, player, this ) );
	}

	// Get list to redraw.
	pPlayersList_->childrenUpdated();

	displayCurrentStatus();
}


//virtual
bool MachGuiCtxCampaign::okayToSwitchContext()
{
	if ( pStartupScreens_->lastButtonEvent() == MachGuiStartupScreens::BE_OK )
	{
		// Create new player...
		if ( pNewPlayerName_->selected() )
		{
			if ( pNewPlayerName_->text() != "" )
			{
				// Store the name of the last selected player in the registry
				SysRegistry::instance().setStringValue( "Misc", "Current Player Name", pNewPlayerName_->text(), SysRegistry::CURRENT_USER );

				MachGuiDbPlayer& newPlayer = MachGuiDatabase::instance().addPlayer( pNewPlayerName_->text() );
				MachGuiDatabase::instance().currentPlayer( &newPlayer );
				MachGuiDatabase::instance().writeDatabase();

				return true;
			}
			else
			{
				pStartupScreens_->displayMsgBox( IDS_MENUMSG_ENTERPLAYERNAME );
				return false;
			}
		}
		else // Select previously created player...
		{
			if ( not pSelectedPlayer_ )
			{
				pStartupScreens_->displayMsgBox( IDS_MENUMSG_ENTERPLAYERNAME );
				return false;
			}
		}
	}
	else if ( pStartupScreens_->lastButtonEvent() == MachGuiStartupScreens::EXIT )
	{
		MachGuiDatabase::instance().clearCurrentPlayer();
	}

	return true;
}

void MachGuiCtxCampaign::selectedPlayer( MachGuiDbPlayer* pPlayer )
{
	pSelectedPlayer_ = pPlayer;
	displayCurrentStatus();
}

void MachGuiCtxCampaign::clearSelectedPlayer()
{
	pSelectedPlayer_ = NULL;
	displayCurrentStatus();
}

//virtual
void MachGuiCtxCampaign::buttonEvent( MachGuiStartupScreens::ButtonEvent be )
{
	if ( be == MachGuiStartupScreens::BE_NEW )
	{
		pNewPlayerName_->selectThisItem();
		pStartupScreens_->displayMsgBox( IDS_MENUMSG_NEWPLAYERNAME );
	}
	else if ( be == MachGuiStartupScreens::BE_DELETE )
	{
		if ( pSelectedPlayer_ )
		{
			// Confirm deletion. Display players name in delete msg.
			GuiStrings strings;
			strings.push_back( pSelectedPlayer_->name() );
			pStartupScreens_->displayMsgBox( IDS_MENUMSG_DELETEPLAYER, _NEW( MachGuiPlayerDeleteMessageBoxResponder( this ) ), strings );
		}
		else
		{
			if ( pNewPlayerName_->selected() )
			{
				// Reset new player name
				pNewPlayerName_->text("");
			}
		}
	}
}

void MachGuiCtxCampaign::deletePlayer()
{
	PRE( pSelectedPlayer_ );

	// Delete player
	MachGuiDatabase::instance().removePlayer( pSelectedPlayer_ );

    //Construct a list of saved games with no player attached
    typedef ctl_vector< MachGuiDbSavedGame* > SavedGames;
    SavedGames savedGamesWithNoPlayer;
    savedGamesWithNoPlayer.reserve( 64 );

	uint numSavedGames = MachGuiDatabase::instance().nSavedGames();

	for ( uint loop = 0; loop < numSavedGames; ++loop )
	{
		MachGuiDbSavedGame& savedGame = MachGuiDatabase::instance().savedGame( loop );
		if ( savedGame.isCampaignGame() and not savedGame.hasPlayer() )
            savedGamesWithNoPlayer.push_back( &savedGame );
	}

    //Now delete these saved games.
    //Note this 2-stage process is necessary, otherwise we will be modifying a collection
    //whilst iterating through it.
	for ( uint loop = 0; loop < savedGamesWithNoPlayer.size(); ++loop )
	{
        MachGuiDbSavedGame* pSavedGame = savedGamesWithNoPlayer[ loop ];
		remove( pSavedGame->externalFileName().c_str() );
		MachGuiDatabase::instance().removeSavedGame( pSavedGame );
    }

	// Update persistent version of database
	MachGuiDatabase::instance().writeDatabase();
	updatePlayersList();
}

void MachGuiCtxCampaign::displayCurrentStatus()
{
	const int CURRENTSTATUS_MINX = 378;
	const int CURRENTSTATUS_MINY = 260;
	const int CURRENTSTATUS_MAXX = 532;
	const int CURRENTSTATUS_MAXY = 376;
	const int CURRENTSTATUS_WIDTH = CURRENTSTATUS_MAXX - CURRENTSTATUS_MINX;

	// Clean up old text
	_DELETE( pScenarioText_ );
	_DELETE( pLastScenarioText_ );
	_DELETE( pScoreText_ );
	_DELETE( pLastScoreText_ );
	_DELETE( pNumScenariosPlayedText_ );
	_DELETE( pNumScenariosText_ );
	_DELETE( pTotalScoreHeadingText_ );
	_DELETE( pTotalScoreText_ );
	pScenarioText_ = NULL;
	pLastScenarioText_ = NULL;
	pScoreText_ = NULL;
	pLastScoreText_ = NULL;
	pNumScenariosPlayedText_ = NULL;
	pNumScenariosText_ = NULL;
	pTotalScoreHeadingText_ = NULL;
	pTotalScoreText_ = NULL;

	pStartupScreens_->changed();

	// Only display information if a player has been selected
	if ( pSelectedPlayer_ )
	{
		// Load resource strings
		GuiResourceString scenarioStr( IDS_MENU_STATUSSCENARIO );
	 	GuiResourceString scoreStr( IDS_MENU_STATUSSCORE );
		GuiResourceString numScenariosPlayedStr( IDS_MENU_STATUSNUMSCENARIOS );
		GuiResourceString totalScoreStr( IDS_MENU_STATUSTOTALSCORE );
		GuiResourceString lastScenarioGuiStr( IDS_MENU_STATUSNOTPLAYED );

		// Default text
		string lastScenarioStr( lastScenarioGuiStr.asString() );
		string lastScoreStr("0");
		string numScenariosPlayed("0");
		string totalScore("0");

		if ( pSelectedPlayer_->nPlayerScenarios() != 0 )
		{
		  	MachGuiDbPlayerScenario& lastScenario = pSelectedPlayer_->mostRecentPlayerScenario();

			// Info about last scenario played
			lastScenarioStr = lastScenario.scenario().menuString();

			char buffer[ 255 ];

//			lastScoreStr = itoa( lastScenario.lastScore(), buffer, 10 );
			sprintf(buffer, "%d", lastScenario.lastScore());
			lastScoreStr = buffer;
//			numScenariosPlayed = itoa( pSelectedPlayer_->nPlayerScenarios(), buffer, 10 );
            sprintf(buffer, "%d", pSelectedPlayer_->nPlayerScenarios());
            numScenariosPlayed = buffer;

			// Total all the scores for scenarios played
			int nTotalScore = 0;
			for ( int loop = 0; loop < pSelectedPlayer_->nPlayerScenarios(); ++loop )
			{
				nTotalScore += pSelectedPlayer_->playerScenario( loop ).lastScore();
			}
            sprintf(buffer, "%d", nTotalScore);
            totalScore = buffer;
//			totalScore = itoa( nTotalScore, buffer, 10 );

		}

		GuiBmpFont font( GuiBmpFont::getFont("gui/menu/smallfnt.bmp") );

		Gui::Box scenarioTextBox( Gui::Coord( CURRENTSTATUS_MINX, CURRENTSTATUS_MINY ), CURRENTSTATUS_WIDTH, font.charHeight() );

		DEBUG_STREAM( DIAG_NEIL, "pScenarioText_ " << scenarioTextBox << std::endl );

		pScenarioText_ = _NEW( MachGuiMenuText( pStartupScreens_,
								   				scenarioTextBox,
								   			 	scenarioStr.asString(),
								   			 	"gui/menu/smallfnt.bmp",
								   			 	MachGuiMenuText::LEFT_JUSTIFY ) );

		Gui::Box lastScenarioTextBox( Gui::Coord( CURRENTSTATUS_MINX, pScenarioText_->absoluteBoundary().maxCorner().y() + 2 - pStartupScreens_->yMenuOffset() ), CURRENTSTATUS_WIDTH, (2 * font.charHeight()) + 1 );

		DEBUG_STREAM( DIAG_NEIL, "pLastScenarioText_ " << lastScenarioTextBox << std::endl );

		pLastScenarioText_ = _NEW( MachGuiMenuText( pStartupScreens_,
								   					lastScenarioTextBox,
								   					lastScenarioStr,
								   					"gui/menu/smalwfnt.bmp",
								   					MachGuiMenuText::LEFT_JUSTIFY ) );

		Gui::Box scoreTextBox( Gui::Coord( CURRENTSTATUS_MINX, pLastScenarioText_->absoluteBoundary().maxCorner().y() + 4 - pStartupScreens_->yMenuOffset() ), CURRENTSTATUS_WIDTH, font.charHeight() );

		DEBUG_STREAM( DIAG_NEIL, "pScoreText_ " << scoreTextBox << std::endl );

		pScoreText_ = _NEW( MachGuiMenuText(	pStartupScreens_,
								   				scoreTextBox,
								   				scoreStr.asString(),
								   				"gui/menu/smallfnt.bmp",
								   				MachGuiMenuText::LEFT_JUSTIFY ) );

		Gui::Box lastScoreTextBox( Gui::Coord( CURRENTSTATUS_MINX, pScoreText_->absoluteBoundary().maxCorner().y() + 2 - pStartupScreens_->yMenuOffset() ), CURRENTSTATUS_WIDTH, font.charHeight() );

		DEBUG_STREAM( DIAG_NEIL, "pLastScoreText_ " << lastScoreTextBox << std::endl );

		pLastScoreText_ = _NEW( MachGuiMenuText(	pStartupScreens_,
									   				lastScoreTextBox,
									   				lastScoreStr,
									   				"gui/menu/smalwfnt.bmp",
									   				MachGuiMenuText::LEFT_JUSTIFY ) );

		Gui::Box numScenariosPlayedStrBox( Gui::Coord( CURRENTSTATUS_MINX, pLastScoreText_->absoluteBoundary().maxCorner().y() + 6 - pStartupScreens_->yMenuOffset() ), CURRENTSTATUS_WIDTH, font.charHeight() );

		DEBUG_STREAM( DIAG_NEIL, "pNumScenariosPlayedText_ " << numScenariosPlayedStrBox << std::endl );

		pNumScenariosPlayedText_ = _NEW( MachGuiMenuText(	pStartupScreens_,
											   				numScenariosPlayedStrBox,
											   				numScenariosPlayedStr.asString(),
											   				"gui/menu/smallfnt.bmp",
											   				MachGuiMenuText::LEFT_JUSTIFY ) );

		Gui::Box numScenariosPlayedBox( Gui::Coord( CURRENTSTATUS_MINX, pNumScenariosPlayedText_->absoluteBoundary().maxCorner().y() + 2 - pStartupScreens_->yMenuOffset() ), CURRENTSTATUS_WIDTH, font.charHeight() );

		DEBUG_STREAM( DIAG_NEIL, "pNumScenariosText_ " << numScenariosPlayedBox << std::endl );

		pNumScenariosText_ = _NEW( MachGuiMenuText(	pStartupScreens_,
									   				numScenariosPlayedBox,
									   				numScenariosPlayed,
									   				"gui/menu/smalwfnt.bmp",
									   				MachGuiMenuText::LEFT_JUSTIFY ) );

		Gui::Box totalScoreStrBox( Gui::Coord( CURRENTSTATUS_MINX, pNumScenariosText_->absoluteBoundary().maxCorner().y() + 6 - pStartupScreens_->yMenuOffset() ), CURRENTSTATUS_WIDTH, font.charHeight() );

		DEBUG_STREAM( DIAG_NEIL, "pTotalScoreHeadingText_ " << totalScoreStrBox << std::endl );

		pTotalScoreHeadingText_ = _NEW( MachGuiMenuText(	pStartupScreens_,
											   				totalScoreStrBox,
											   				totalScoreStr.asString(),
											   				"gui/menu/smallfnt.bmp",
											   				MachGuiMenuText::LEFT_JUSTIFY ) );

		Gui::Box totalScoreBox( Gui::Coord( CURRENTSTATUS_MINX, pTotalScoreHeadingText_->absoluteBoundary().maxCorner().y() + 2 - pStartupScreens_->yMenuOffset() ), CURRENTSTATUS_WIDTH, font.charHeight() );

		DEBUG_STREAM( DIAG_NEIL, "pTotalScoreText_ " << totalScoreBox << std::endl );

		pTotalScoreText_ = _NEW( MachGuiMenuText(	pStartupScreens_,
									   				totalScoreBox,
									   				totalScore,
									   				"gui/menu/smalwfnt.bmp",
									   				MachGuiMenuText::LEFT_JUSTIFY ) );
	}
}




/* End CTXCAMP.CPP **************************************************/
