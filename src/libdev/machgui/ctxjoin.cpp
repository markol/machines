/*
 * C T X J O I N . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/manager.hpp"
#include "device/time.hpp"
#include "machgui/ctxjoin.hpp"
#include "machgui/sslistit.hpp"
#include "machgui/menubutn.hpp"
#include "machgui/menutext.hpp"
#include "machgui/startup.hpp"
#include "machgui/strtdata.hpp"
#include "machgui/sslistbx.hpp"
#include "machgui/messbrok.hpp"
#include "machgui/gui.hpp"
#include "machgui/editbox.hpp"
#include "machgui/scrolbar.hpp"
#include "machgui/editlist.hpp"
#include "machgui/database.hpp"
#include "machgui/dbsystem.hpp"
#include "machgui/dbplanet.hpp"
#include "machgui/dbscenar.hpp"
#include "machlog/network.hpp"
#include "machgui/mdnetwork.hpp"
#include "machgui/mdserial.hpp"
#include "machgui/mdmodem.hpp"
#include "machgui/mdinternet.hpp"
#include "machgui/mdipx.hpp"
#include "gui/font.hpp"
#include "gui/scrolbut.hpp"
#include "gui/scrolbar.hpp"
#include "gui/painter.hpp"
#include "gui/restring.hpp"
#include "gui/manager.hpp"
#include "machgui/internal/strings.hpp"
#include "network/netnet.hpp"
#include "network/sessuid.hpp"
#include "network/session.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////

class MachGuiCurGamesListBoxItem : public MachGuiSingleSelectionListBoxItem
// Canonical form revoked
{
public:
    MachGuiCurGamesListBoxItem( MachGuiStartupScreens* pStartupScreens, MachGuiSingleSelectionListBox* pListBox, size_t width, const string& text, MachGuiCtxJoin& joinContext )
	:	MachGuiSingleSelectionListBoxItem( pStartupScreens, pListBox, width, text ),
		joinContext_( joinContext )
	{}

    ~MachGuiCurGamesListBoxItem( void )
	{
//		joinContext_.joinGameSelected( false ); // This produced some freed memory references
	}

    void CLASS_INVARIANT;

protected:
	virtual void select()
	{
		MachGuiSingleSelectionListBoxItem::select();

		startupScreens()->startupData()->joinGame( text() );

	 	joinContext_.joinGameSelected( true );
	}

	virtual void unselect()
	{
		MachGuiSingleSelectionListBoxItem::unselect();

		joinContext_.joinGameSelected( false );
 	}

private:
    MachGuiCurGamesListBoxItem( const MachGuiCurGamesListBoxItem& );
    MachGuiCurGamesListBoxItem& operator =( const MachGuiCurGamesListBoxItem& );

	// Data members...
	MachGuiCtxJoin& joinContext_;
};
//////////////////////////////////////////////////////////////////////////////////////////////

class MachGuiNewGameName : public MachGuiEditBoxListBoxItem
{
public:
	MachGuiNewGameName( MachGuiStartupScreens* pStartupScreens, MachGuiSingleSelectionListBox* pListBox, size_t width, const string& text, MachGuiCtxJoin& joinContext )
	:	MachGuiEditBoxListBoxItem( pStartupScreens, pListBox, width, text ),
		joinContext_( joinContext )
	{}

	virtual ~MachGuiNewGameName()
	{
		updateNewGameName();

//		joinContext_.editingGameName( false ); // Why it was there?
	}

	void updateNewGameName()
	{
		if ( text() != "" )
			startupScreens()->startupData()->newGameName( text() );
	}

protected:
	virtual void select()
	{
		MachGuiEditBoxListBoxItem::select();

		singleLineEditBox()->maxChars( MAX_GAMENAME_LEN );
		singleLineEditBox()->text( startupScreens()->startupData()->newGameName() );
		startupScreens()->startupData()->joinGame( "" );
		joinContext_.editingGameName( true );
	}

	virtual void unselect()
	{
		startupScreens()->startupData()->newGameName( singleLineEditBox()->text() );

		MachGuiEditBoxListBoxItem::unselect();
		joinContext_.changeFocus();
		joinContext_.editingGameName( false );
	}
private:
	MachGuiCtxJoin& joinContext_;
};

//////////////////////////////////////////////////////////////////////////////////////////////

#define JOINGAME_LB_MINX 48
#define JOINGAME_LB_MAXX 262
#define JOINGAME_LB_MINY 56
#define JOINGAME_LB_MAXY 300
#define SCROLLBAR_WIDTH  17

MachGuiCtxJoin::MachGuiCtxJoin( MachGuiStartupScreens* pStartupScreens )
:	MachGuiStartupScreenContext( pStartupScreens ),
	animations_( pStartupScreens, SysPathName("gui/menu/sc_anims.anm") ),
	pNetworkProtocol_( NULL ),
	editingGameName_( false ),
	joinGameSelected_( false )
{

WAYNE_STREAM("JOINGAME_LB_MAXY: " << JOINGAME_LB_MAXY << std::endl );
 	pJoinBtn_ = _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 414, 51,  605, 93 ), IDS_MENUBTN_JOIN, MachGuiStartupScreens::JOIN ) );
	pCreateBtn_ = _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 414, 157, 605, 200 ), IDS_MENUBTN_CREATE, MachGuiStartupScreens::CREATE ) );
 	pShowGamesBtn_ = _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 414, 251, 605, 293 ), IDS_MENUBTN_SHOWGAMES, MachGuiStartupScreens::SHOWGAMES ) );
	MachGuiMenuButton* pCancelBtn = _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 414, 348, 605, 390 ), IDS_MENUBTN_CANCEL, MachGuiStartupScreens::EXIT ) );

	pCancelBtn->escapeControl( true );
	pShowGamesBtn_->defaultControl( true );

	// Display list box heading
	GuiResourceString currentGames( IDS_MENULB_GAMES );
  	GuiBmpFont font( GuiBmpFont::getFont("gui/menu/largefnt.bmp") );
	MachGuiMenuText* pCurrentGamesText = _NEW( MachGuiMenuText( pStartupScreens,
																Gui::Box( 	JOINGAME_LB_MINX,
																			JOINGAME_LB_MINY,
																			JOINGAME_LB_MINX + font.textWidth( currentGames.asString() ),
																			55 + font.charHeight() + 2 ),
																IDS_MENULB_GAMES, "gui/menu/largefnt.bmp" ) );

	// Create games list box
	pGamesList_ = _NEW( MachGuiSingleSelectionListBox( 	pStartupScreens,
														Gui::Box( 	JOINGAME_LB_MINX,
																	pCurrentGamesText->absoluteBoundary().maxCorner().y() - pStartupScreens_->yMenuOffset(),
																	JOINGAME_LB_MAXX - SCROLLBAR_WIDTH,
																	JOINGAME_LB_MAXY),
														1000, MachGuiSingleSelectionListBoxItem::reqHeight(), 1 ) );

	MachGuiVerticalScrollBar::createWholeBar( 	pStartupScreens,
												Gui::Coord( JOINGAME_LB_MAXX - SCROLLBAR_WIDTH, JOINGAME_LB_MINY ),
												JOINGAME_LB_MAXY - JOINGAME_LB_MINY,
												pGamesList_ );

	// Clear join game info.
	pStartupScreens->startupData()->joinGame( "" );

	// Setup edit box for new game name to be entered into...
	GuiResourceString enterName( IDS_MENU_ENTERGAMENAME );
	pNewGameName_ = _NEW( MachGuiNewGameName( pStartupScreens_, pGamesList_, JOINGAME_LB_MAXX - JOINGAME_LB_MINX - SCROLLBAR_WIDTH, enterName.asString(), *this ) );
	pGamesList_->childrenUpdated();

	// Standard context setting stuff...
	pStartupScreens->changeBackdrop( "gui/menu/sc.bmp" );
    pStartupScreens->cursorOn( true );
    pStartupScreens->desiredCdTrack( MachGuiStartupScreens::MENU_MUSIC );

	// Query network to find which protocol has been selected
	NetNetwork::NetworkProtocol protocol = NetNetwork::instance().currentProtocol();

    switch( protocol )
	{
		case NetNetwork::UDP: pNetworkProtocol_ = _NEW( MachGuiInternetNetworkMode( pStartupScreens, pStartupScreens ) ); break;
		case NetNetwork::IPX: pNetworkProtocol_ = _NEW( MachGuiIPXNetworkMode( pStartupScreens, pStartupScreens ) ); break;
		case NetNetwork::TCPIP: pNetworkProtocol_ = _NEW( MachGuiInternetNetworkMode( pStartupScreens, pStartupScreens ) ); break;
		case NetNetwork::MODEM: pNetworkProtocol_ = _NEW( MachGuiModemNetworkMode( pStartupScreens, pStartupScreens ) ); break;
		case NetNetwork::SERIAL: pNetworkProtocol_ = _NEW( MachGuiSerialNetworkMode( pStartupScreens, pStartupScreens ) ); break;
		default: ASSERT( false, "Network protocol not currently supported by GUI" );
	}

    TEST_INVARIANT;
}

MachGuiCtxJoin::~MachGuiCtxJoin()
{
    TEST_INVARIANT;

	GuiManager::instance().charFocus( pStartupScreens_ );
}

void MachGuiCtxJoin::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiCtxJoin& t )
{

    o << "MachGuiCtxJoin " << (void*)&t << " start" << std::endl;
    o << "MachGuiCtxJoin " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiCtxJoin::buttonEvent( MachGuiStartupScreens::ButtonEvent be )
{
	if ( be == MachGuiStartupScreens::SHOWGAMES )
	{
		showGames();
	}
}

void MachGuiCtxJoin::showGames()
{
	PRE( pNetworkProtocol_->connectionSetNoRecord() );

	bool isHost = false;

	if( validNetworkSettings( isHost ) )
	{
		// Display PleaseWait
		//if ( NetNetwork::instance().currentProtocol() != NetNetwork::IPX )
		//{
		//GuiBitmap loadingBmp = Gui::bitmap( "gui/menu/pleaseWait.bmp" );
		//loadingBmp.enableColourKeying();
		//GuiBitmap frontBuffer = W4dManager::instance().sceneManager()->pDevice()->frontSurface();
		//frontBuffer.simpleBlit( loadingBmp, Ren::Rect( 0,0,loadingBmp.width(),loadingBmp.height() ) );
		//}

		// Clear games list box
		pGamesList_->deleteAllItems();

		// Set up special enter-name edit box
		GuiResourceString enterName( IDS_MENU_ENTERGAMENAME );
		pNewGameName_ = _NEW( MachGuiNewGameName( pStartupScreens_, pGamesList_, JOINGAME_LB_MAXX - JOINGAME_LB_MINX - SCROLLBAR_WIDTH, enterName.asString(), *this ) );

		// Call children updated here to clean network status before polling for new sessions.
		pGamesList_->childrenUpdated();

		// Update network status
		NETWORK_STREAM("MachGuiCtxJoin::showGames() calling update to get most uptodate session list\n" );
		NetNetwork::instance().update();

		const NetNetwork::Sessions& sessions = NetNetwork::instance().sessions();

		NETWORK_STREAM(" sessions.size " << sessions.size() << std::endl );

		for ( NetNetwork::Sessions::const_iterator iter = sessions.begin(); iter != sessions.end(); ++iter )
		{
			_NEW( MachGuiCurGamesListBoxItem( pStartupScreens_, pGamesList_, JOINGAME_LB_MAXX - JOINGAME_LB_MINX - SCROLLBAR_WIDTH, (*iter)->appSessionName(), *this ) );
			NETWORK_STREAM("  item added.\n" );
		}
		NETWORK_STREAM(" items added to the list box.\n" );

		pGamesList_->childrenUpdated();

		//if ( NetNetwork::instance().currentProtocol() != NetNetwork::IPX )
		//	pStartupScreens_->changed();
	}
}

//virtual
bool MachGuiCtxJoin::okayToSwitchContext()
{
	// Initialise multiplayer settings ( assumes at least 1 planet and 1 scenario in Small map size ).
	MachGuiDbSystem& system = MachGuiDatabase::instance().multiPlayerSystem( MachGuiDatabase::SMALL );
	MachGuiDbPlanet& planet = system.planet( 0 );
	MachGuiDbScenario& scenario = planet.scenario( 0 );
	pStartupScreens_->startupData()->scenario( &scenario );
	// Initialise player slots to unavailable if scenario is limited in number of players.
	// Based on the maximum number of players allowed in this multiplayer game update the
	// player slots to "open" or "unavailable".
	for ( int loop = 0; loop < 4; ++loop )
	{
		if ( loop < scenario.maxPlayers() )
		{
			pStartupScreens_->startupData()->players()[loop].setPlayerName( MachGui::openText() );
		}
		else
		{
			pStartupScreens_->startupData()->players()[loop].setPlayerName( MachGui::unavailableText() );
		}
	}

	// Extract typed in new game name and store in startup data
	pNewGameName_->updateNewGameName();

	// Was CREATE pressed...
	if ( pStartupScreens_->lastButtonEvent() == MachGuiStartupScreens::CREATE )
	{
		bool isHost = true;

		if ( pStartupScreens_->startupData()->newGameName() == "" or
			 not editingGameName_ or
			 pNewGameName_->text() == "" )
		{
			// Display message box. Type in game name to create game.
			pStartupScreens_->displayMsgBox( IDS_MENUMSG_ENTERGAMENAME );
		   	return false;
		} // Can the network connection be established
		else if( not validNetworkSettings( isHost ) )
		{
			return false;
		}
		else
		{
			//Now pass through player name as input in gui to network library
			MachLogNetwork::instance().hostWithSessionId
			(
				pStartupScreens_->startupData()->newGameName(), pStartupScreens_->startupData()->playerName()
			);
	   		pStartupScreens_->startupData()->hostGame( true );

			return true;
		}
	} // Was JOIN pressed...
	else if ( pStartupScreens_->lastButtonEvent() == MachGuiStartupScreens::JOIN )
	{
		bool isHost = false;
		if ( pStartupScreens_->startupData()->joinGame() == "" or not joinGameSelected_ )
		{
			// Display message box. Must choose game to join.
			pStartupScreens_->displayMsgBox( IDS_MENUMSG_SELECTGAMETOJOIN );
			return false;
		} // Can the network connection be established
// Complete rubbish! the connection settings have already been set. Hal 19/11/98
//		else if ( not validNetworkSettings( isHost ) )
//		{
//			return false;
//		}
		else
		{
			if( not MachLogNetwork::instance().joinWithSessionId( pStartupScreens_->startupData()->joinGame(), pStartupScreens_->startupData()->playerName() ) )
				if( not MachLogNetwork::instance().joinWithSessionId( pStartupScreens_->startupData()->joinGame(), pStartupScreens_->startupData()->playerName() ) )
				{
					pStartupScreens_->displayMsgBox( IDS_MENUMSG_NETSESSIONERROR );
					return false;
				}

			if( NetNetwork::currentStatus() != NetNetwork::NETNET_OK )
			{
				pStartupScreens_->displayMsgBox( MachGui::convertNetworkError( NetNetwork::currentStatus() ) );
				return false;
			}
			// Reset join game incase player cancels "I'm Ready" context and wants to join a different game.
			pStartupScreens_->startupData()->joinGame( "" );
			pStartupScreens_->startupData()->hostGame( false );
			// Send join message so that host can update availablePlayers list etc.
			pStartupScreens_->messageBroker().sendJoinMessage( pStartupScreens_->startupData()->playerName(), pStartupScreens_->startupData()->uniqueMachineNumber() );
			return true;
		}
	}
	return true;
}

//virtual
void MachGuiCtxJoin::update()
{
	// Get caret to flash in edit box.
	pNewGameName_->update();
	animations_.update();
	pNetworkProtocol_->updateGUI();

	// Update available games automatically on IPX
	if ( NetNetwork::instance().currentProtocol() == NetNetwork::IPX )
	{
		static double time = 0.0;

		if ( DevTime::instance().time() - time > 5.0 and
			 not editingGameName_ and
			 not joinGameSelected_ and
			 NetNetwork::instance().currentStatus() == NetNetwork::NETNET_OK )
		{
			showGames();
			time = DevTime::instance().time();
		}
	}
}


void MachGuiCtxJoin::changeFocus( void )
{
	pNetworkProtocol_->charFocus();
}

bool MachGuiCtxJoin::validNetworkSettings( bool isHost )
{
	// If network details are not valid, the class handling the protocol will display
	// the appropriate dialog box
	if ( not pNetworkProtocol_->validNetworkDetails( isHost ) )
	{
	    return false;
	}
	else
	{
		// Set network protocol settings to be the ones from the GUI
		pNetworkProtocol_->setNetworkDetails();
	}
	// Test for successful network connection
	if(	not pNetworkProtocol_->connectionSet() )
	{
		return false;
	}
	return true;
}

void MachGuiCtxJoin::editingGameName( bool egn )
{
	editingGameName_ = egn;

	if ( editingGameName_ )
	{
		pShowGamesBtn_->defaultControl( false );
		pJoinBtn_->defaultControl( false );
		pCreateBtn_->defaultControl( true );

		if ( pJoinBtn_->hasFocusSet() )
		{
			pJoinBtn_->hasFocus( false );
			pCreateBtn_->hasFocus( true );
		}
	}
	else
	{
		pShowGamesBtn_->defaultControl( true );
		pJoinBtn_->defaultControl( false );
		pCreateBtn_->defaultControl( false );
	}
}

void MachGuiCtxJoin::joinGameSelected( bool jsg )
{
	joinGameSelected_ = jsg;

	if ( joinGameSelected_ )
	{
		pShowGamesBtn_->defaultControl( false );
		pJoinBtn_->defaultControl( true );
		pCreateBtn_->defaultControl( false );

		if ( pCreateBtn_->hasFocusSet() )
		{
			pCreateBtn_->hasFocus( false );
			pJoinBtn_->hasFocus( true );
		}
	}
	else
	{
		pShowGamesBtn_->defaultControl( true );
		pJoinBtn_->defaultControl( false );
		pCreateBtn_->defaultControl( false );
	}
}

/* End CTXJOIN.CPP **************************************************/
