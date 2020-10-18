/*
 * M D E M M O D E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/mdmodem.hpp"
#include "stdlib/string.hpp"
#include "gui/gui.hpp"
#include "gui/font.hpp"
#include "gui/manager.hpp"
#include "machgui/dropdwnc.hpp"
#include "machgui/menutext.hpp"
#include "machgui/editbox.hpp"
#include "machgui/startup.hpp"
#include "machgui/gui.hpp"
#include "machgui/internal/strings.hpp"
#include "network/netnet.hpp"
#include "machgui/msgbox.hpp"

struct MachGuiModemNetworkModeImpl
{
	MachGuiModemNetworkModeImpl( void )
	:	pModemSelector_( NULL ),
		pTelNoEntryBox_( NULL )
	{}
	MachGuiDropDownListBoxCreator* pModemSelector_;
	MachGuiSingleLineEditBox* pTelNoEntryBox_;
};

MachGuiModemNetworkMode::MachGuiModemNetworkMode( GuiDisplayable* pParent, MachGuiStartupScreens* pStartupScreens )
:	MachGuiNetworkProtocolMode( pParent, pStartupScreens )
{
	pimpl_ = _NEW( MachGuiModemNetworkModeImpl() );

	readNetworkDetails();

    TEST_INVARIANT;
}

MachGuiModemNetworkMode::~MachGuiModemNetworkMode()
{
	TEST_INVARIANT;

	_DELETE( pimpl_ );
}

void MachGuiModemNetworkMode::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

// virtual
void MachGuiModemNetworkMode::setNetworkDetails( void )
{
	// This function will use the settings from the drop downs to configure network settings
	string currentModem = pimpl_->pModemSelector_->text();
	string serverTelNo = pimpl_->pTelNoEntryBox_->text();

//	NetNetwork::instance().phone( serverTelNo );
//	NetNetwork::instance().modem( currentModem );
	NetNetwork::instance().initialiseConnection();
}

// virtual
bool MachGuiModemNetworkMode::validNetworkDetails( bool isHost )
{
	bool valid = true;

	if ( ( pimpl_->pTelNoEntryBox_->text() == "" ) and ( not isHost ) )
	{
		startupScreens().displayMsgBox( IDS_MENUMSG_ENTERSERVERTELNO );
		valid = false;
	}
	return valid;
}

// virtual
void MachGuiModemNetworkMode::updateGUI( void )
{
	// calling the update method on the text entry box will cause its caret to flash
	pimpl_->pTelNoEntryBox_->update();
}

#define MNM_MINX 62
#define MNM_MINY 309
#define MNM_WIDTH 110
#define MNM_DEPTH 30
#define MNM_MINY2 MNM_MINY + MNM_DEPTH
#define MAX_TELNO_LEN 20

// virtual
void MachGuiModemNetworkMode::readNetworkDetails( void )
{
	GuiResourceString modemHeading( IDS_MENU_MODEMTYPE );
  	GuiBmpFont font( GuiBmpFont::getFont("gui/menu/smalwfnt.bmp") );
	const int textHeight = font.charHeight() + 2;


//	NetNetwork::Modems availableModems = NetNetwork::instance().modems();
	NetNetwork::Modems availableModems;

//	ASSERT( availableModems.size(), "no modems");

//	WAYNE_STREAM( "Modems" << std::endl << availableModems << std::endl );

 	MachGuiMenuText* pModemText =
		_NEW( MachGuiMenuText( &parent(),
							   Gui::Box( Gui::Coord( MNM_MINX, MNM_MINY ),
							   			 font.textWidth( modemHeading.asString() ),
							   			 textHeight ),
							   IDS_MENU_MODEMTYPE, "gui/menu/smallfnt.bmp" ) );

	pimpl_->pModemSelector_ = _NEW( MachGuiDropDownListBoxCreator ( &parent(), &startupScreens(), Gui::Coord( MNM_MINX, MNM_MINY+textHeight ), MNM_WIDTH, availableModems, true, true ) );

	GuiResourceString telephoneNoHeading( IDS_MENU_TELNO );

	MachGuiMenuText* pTelephoneNoText =
		_NEW( MachGuiMenuText( &parent(),
							   Gui::Box( Gui::Coord( MNM_MINX, MNM_MINY2 ),
							   			 font.textWidth( telephoneNoHeading.asString() ),
							   			 textHeight ),
							   IDS_MENU_TELNO, "gui/menu/smallfnt.bmp" ) );

	pimpl_->pTelNoEntryBox_ = _NEW( MachGuiSingleLineEditBox( 	&startupScreens(),
																Gui::Box( Gui::Coord( MNM_MINX, MNM_MINY2+textHeight ),
																		  MNM_WIDTH,
																		  textHeight + 4 ), font ) );
	pimpl_->pTelNoEntryBox_->ignoreSpaceAtBeginning( true );
	pimpl_->pTelNoEntryBox_->border( true );
	pimpl_->pTelNoEntryBox_->borderColour( MachGui::DARKSANDY() );

	GuiManager::instance().charFocus( pimpl_->pTelNoEntryBox_ );
}

ostream& operator <<( ostream& o, const MachGuiModemNetworkMode& t )
{

    o << "MachGuiModemNetworkMode " << (void*)&t << " start" << std::endl;
    o << "MachGuiModemNetworkMode " << (void*)&t << " end" << std::endl;

    return o;
}

/* End MDEMMODE.CPP *************************************************/
