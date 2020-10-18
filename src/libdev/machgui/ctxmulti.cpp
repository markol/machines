/*
 * C T X M U L T I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/ctxmulti.hpp"
#include "machgui/menubutn.hpp"
#include "machgui/strtdata.hpp"
#include "machgui/menutext.hpp"
#include "machgui/sslistit.hpp"
#include "machgui/sslistbx.hpp"
#include "machgui/editbox.hpp"
#include "machgui/gui.hpp"
#include "machgui/internal/strings.hpp"
#include "gui/manager.hpp"
#include "gui/font.hpp"
#include "gui/painter.hpp"
#include "gui/restring.hpp"
#include "network/session.hpp"
#include "network/netnet.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////
class MachGuiProtocolListBoxItem : public MachGuiSingleSelectionListBoxItem
// Canonical form revoked
{
public:
    MachGuiProtocolListBoxItem( MachGuiStartupScreens* pStartupScreens, MachGuiSingleSelectionListBox* pListBox, size_t width, const string& text )
	:	MachGuiSingleSelectionListBoxItem( pStartupScreens, pListBox, width, text )
	{}

    ~MachGuiProtocolListBoxItem( void )
	{}

	bool isSelectedProtocol()
	{
		//returns whether or not this item matches the currectly selected protocol
		//This will enable client code to reselect it as necessary
//		if ( stricmp( text().c_str(), startupScreens()->startupData()->connectionType().c_str() ) == 0 )
		if ( strcasecmp( text().c_str(), startupScreens()->startupData()->connectionType().c_str() ) == 0 )
		{
			return true;
		}
		return false;
	}

protected:
	virtual void select()
	{
		DEBUG_STREAM( DIAG_NETWORK,"MachGuiProtocolListBoxItem::select()\n" );
		MachGuiSingleSelectionListBoxItem::select();

		startupScreens()->startupData()->connectionType( text(), MachGuiStartupData::DO_NOT_INITIALISE_CONNECTION  );
		DEBUG_STREAM( DIAG_NETWORK,"MachGuiProtocolListBoxItem::select() DONE\n" );
	}

private:
    MachGuiProtocolListBoxItem( const MachGuiProtocolListBoxItem& );
    MachGuiProtocolListBoxItem& operator =( const MachGuiProtocolListBoxItem& );
};

//////////////////////////////////////////////////////////////////////////////////////////////

MachGuiCtxMultiplayer::MachGuiCtxMultiplayer( MachGuiStartupScreens* pStartupScreens )
:	MachGuiStartupScreenContext( pStartupScreens ),
	animations_( pStartupScreens, SysPathName("gui/menu/sb_anims.anm") )
{
	_NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 56, 407, 246, 449 ), IDS_MENUBTN_OK, MachGuiStartupScreens::JOIN ) );
	MachGuiMenuButton* pExitBtn = _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 311,407, 502, 449 ), IDS_MENUBTN_CANCEL, MachGuiStartupScreens::EXIT ) );
	pExitBtn->escapeControl( true );
	GuiBmpFont font( GuiBmpFont::getFont("gui/menu/largefnt.bmp") );
	pSingleLineEditBox_ = _NEW( MachGuiSingleLineEditBox( pStartupScreens, Gui::Box( 106, 34, 444, 34 + font.charHeight() ), font ) );
	pSingleLineEditBox_->maxChars( MAX_PLAYERNAME_LEN );

	if ( pStartupScreens_->startupData()->playerName() == "" )
	{
		GuiResourceString enterName( IDS_MENU_ENTERNAME );
		pSingleLineEditBox_->text( enterName.asString() );
		pSingleLineEditBox_->clearTextOnNextChar( true );
	}
	else
	{
		pSingleLineEditBox_->text( pStartupScreens_->startupData()->playerName() );
	}

	GuiManager::instance().charFocus( pSingleLineEditBox_ );

	GuiResourceString connectionType( IDS_MENULB_CONNECTIONTYPE );

	size_t startY = 123;
	_NEW( MachGuiMenuText( pStartupScreens, Gui::Box( 61, startY, 61 + font.textWidth( connectionType.asString() ), startY + font.charHeight() + 2 ), IDS_MENULB_CONNECTIONTYPE, "gui/menu/largefnt.bmp" ) );
	startY += font.charHeight() + 3;
	MachGuiSingleSelectionListBox* pListBox = _NEW( MachGuiSingleSelectionListBox( pStartupScreens, Gui::Box(61,startY,474,287), 1000, MachGuiSingleSelectionListBoxItem::reqHeight(), 1 ) );

	// Get available protocols
	const NetNetwork::ProtocolMap& availableProtocols = NetNetwork::availableProtocols();

	NetNetwork::ProtocolMap::const_iterator i = availableProtocols.begin();
	NetNetwork::ProtocolMap::const_iterator j = availableProtocols.end();
	int k = 0;
	MachGuiProtocolListBoxItem* pSelectedItem = NULL;
	while(i != j)
	{
		MachGuiProtocolListBoxItem* pItem = _NEW( MachGuiProtocolListBoxItem( pStartupScreens, pListBox, 413, (*i).first ) );
		if( pItem->isSelectedProtocol() )
			pSelectedItem = pItem;
		++i;
	}

	if( pSelectedItem )
		pSelectedItem->selectThisItem();

	pListBox->childrenUpdated();


	pStartupScreens->changeBackdrop( "gui/menu/sb.bmp" );
    pStartupScreens->cursorOn( true );
    pStartupScreens->desiredCdTrack( MachGuiStartupScreens::MENU_MUSIC );

    TEST_INVARIANT;
}

MachGuiCtxMultiplayer::~MachGuiCtxMultiplayer()
{
    TEST_INVARIANT;

	GuiManager::instance().charFocus( pStartupScreens_ );
}

//virtual
bool MachGuiCtxMultiplayer::okayToSwitchContext()
{
	// Trim spaces off beginning of name
	string playerName = pSingleLineEditBox_->text();
	while ( playerName.length() > 0 and playerName[0] == ' ' )
	{
		playerName = &playerName[1];
	}

	// Was JOIN pressed?...
	if ( pStartupScreens_->lastButtonEvent() == MachGuiStartupScreens::JOIN )
	{
		if ( playerName == "" or 		  // No name entered
			 pSingleLineEditBox_->clearTextOnNextChar() )
		{
			pStartupScreens_->displayMsgBox( IDS_MENUMSG_ENTERNAME );

			return false;
		}//stricmp
		else if ( strcasecmp( pSingleLineEditBox_->text().c_str(), MachGui::openText().c_str() ) == 0 or // Invalid name ( open/closed/computer reserved )
		 		  strcasecmp( pSingleLineEditBox_->text().c_str(), MachGui::closedText().c_str() ) == 0 or
				  strcasecmp( pSingleLineEditBox_->text().c_str(), MachGui::computerText().c_str() ) == 0 or
				  strcasecmp( pSingleLineEditBox_->text().c_str(), MachGui::unavailableText().c_str() ) == 0 )
		{
			pStartupScreens_->displayMsgBox( IDS_MENUMSG_INVALIDNAME );

			return false;
		}
		else if ( pStartupScreens_->startupData()->connectionType() == "" ) // No connection type specified
		{
			pStartupScreens_->displayMsgBox( IDS_MENUMSG_CONNECTIONTYPE );

			return false;
		}
	 	else if( NetNetwork::instance().currentStatus() == NetNetwork::NETNET_CONNECTIONERROR )
		{
			uint errorId;

			switch( NetNetwork::instance().currentProtocol() )
			{
				case NetNetwork::MODEM: errorId = IDS_MENUMSG_NOMODEM; break;
				case NetNetwork::IPX: errorId = IDS_MENUMSG_NOIPX; break;
				case NetNetwork::TCPIP: errorId = IDS_MENUMSG_NOTCPIP; break;
				case NetNetwork::SERIAL: errorId = IDS_MENUMSG_NOSERIAL; break;
				default: errorId = IDS_MENUMSG_NETUNDEFINEDERROR; break;
			}
			pStartupScreens_->displayMsgBox( errorId );

			return false;
		}
	}

	if ( playerName != "" and
		 not pSingleLineEditBox_->clearTextOnNextChar() )
	{
		pStartupScreens_->startupData()->playerName( playerName );
	}

	return true;
}

//virtual
void MachGuiCtxMultiplayer::update()
{
	pSingleLineEditBox_->update();
	animations_.update();
}

void MachGuiCtxMultiplayer::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiCtxMultiplayer& t )
{

    o << "MachGuiCtxMultiplayer " << (void*)&t << " start" << std::endl;
    o << "MachGuiCtxMultiplayer " << (void*)&t << " end" << std::endl;

    return o;
}

/* End CTXMULTI.CPP *************************************************/
