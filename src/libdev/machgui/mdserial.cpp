/*
 * S R L M O D E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/mdserial.hpp"
#include "stdlib/string.hpp"
#include "machgui/startup.hpp"
#include "gui/gui.hpp"
#include "gui/font.hpp"
#include "machgui/dropdwnc.hpp"
#include "machgui/menutext.hpp"
#include "machgui/internal/strings.hpp"
#include "network/netnet.hpp"
#include "network/comport.hpp"

struct MachGuiSerialNetworkModeImpl
{
 	MachGuiSerialNetworkModeImpl()
	:	pComPortSelecter_( NULL ),
		pBaudRateSelecter_( NULL ),
		pParitySelecter_( NULL ),
		pStopBitsSelecter_( NULL ),
		pFlowSelecter_( NULL )
	{}
	MachGuiDropDownListBoxCreator* pComPortSelecter_;
	MachGuiDropDownListBoxCreator* pBaudRateSelecter_;
	MachGuiDropDownListBoxCreator* pParitySelecter_;
	MachGuiDropDownListBoxCreator* pStopBitsSelecter_;
	MachGuiDropDownListBoxCreator* pFlowSelecter_;
};

MachGuiSerialNetworkMode::MachGuiSerialNetworkMode( GuiDisplayable* pParent, MachGuiStartupScreens* pStartupScreens )
:	MachGuiNetworkProtocolMode( pParent, pStartupScreens )
{
	pimpl_ = _NEW( MachGuiSerialNetworkModeImpl() );
	readNetworkDetails();
    TEST_INVARIANT;
}

MachGuiSerialNetworkMode::~MachGuiSerialNetworkMode()
{
	_DELETE( pimpl_	);

    TEST_INVARIANT;
}

void MachGuiSerialNetworkMode::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

// virtual
void MachGuiSerialNetworkMode::setNetworkDetails()
{
	// Use the settings from the drop downs to configure network settings
	// Populate a com port object with user settings
	size_t newPortNumber = ( size_t ) pimpl_->pComPortSelecter_->item();

	NetComPortAddress currentPort( newPortNumber );

 	//NetComPortAddress::BaudRate newBaudRate = ( NetComPortAddress::BaudRate ) pimpl_->pBaudRateSelecter_->item();
 	NetComPortAddress::BaudRate newBaudRate = (NetComPortAddress::BaudRate)(size_t) pimpl_->pBaudRateSelecter_->item();
	currentPort.baudRate( newBaudRate );

	//NetComPortAddress::Parity newParity	= ( NetComPortAddress::Parity ) pimpl_->pParitySelecter_->item();
	NetComPortAddress::Parity newParity	= ( NetComPortAddress::Parity ) (size_t) pimpl_->pParitySelecter_->item();
	currentPort.parity( newParity );

	//NetComPortAddress::StopBits newStopBits = ( NetComPortAddress::StopBits ) pimpl_->pStopBitsSelecter_->item();
	NetComPortAddress::StopBits newStopBits = ( NetComPortAddress::StopBits ) (size_t) pimpl_->pStopBitsSelecter_->item();
	currentPort.stopBits( newStopBits );

	//NetComPortAddress::FlowControl newFlowControl = ( NetComPortAddress::FlowControl ) pimpl_->pFlowSelecter_->item();
	NetComPortAddress::FlowControl newFlowControl = ( NetComPortAddress::FlowControl ) (size_t) pimpl_->pFlowSelecter_->item();
	currentPort.flowControl( newFlowControl );

	// Set the current com port settings
//	NetNetwork::instance().comPortAddress( currentPort );

 	NetNetwork::instance().initialiseConnection();
}

//virtual
bool MachGuiSerialNetworkMode::validNetworkDetails( bool /*isHost*/ )
{
	return true;
}

#define SNM_MINX 62
#define SNM_MINY 309
#define SNM_WIDTH 82
#define SNM_MINX2 SNM_MINX + SNM_WIDTH + 15
#define SNM_DEPTH 30
#define SNM_MINY2 SNM_MINY + SNM_DEPTH
#define SNM_MINY3 SNM_MINY + (SNM_DEPTH*2)

// virtual
void MachGuiSerialNetworkMode::readNetworkDetails()
{
	GuiStrings comStrings;
	MachGuiDropDownListBoxCreator::DropDownListBoxItems comSettings;

	comStrings.push_back("COM 1");
	comSettings.push_back( ( void* ) 1 );
	comStrings.push_back("COM 2");
	comSettings.push_back( ( void* ) 2 );
	comStrings.push_back("COM 3");
	comSettings.push_back( ( void* ) 3 );
	comStrings.push_back("COM 4");
	comSettings.push_back( ( void* ) 4 );

	GuiResourceString comPortHeading( IDS_MENU_COMPORT );
  	GuiBmpFont font( GuiBmpFont::getFont("gui/menu/smallfnt.bmp") );
	const int textHeight = font.charHeight() + 2;

  	MachGuiMenuText* pComPortText =
		_NEW( MachGuiMenuText( &startupScreens(),
							   Gui::Box( Gui::Coord(SNM_MINX, SNM_MINY),
							   			 font.textWidth( comPortHeading.asString() ),
							   			 textHeight ), IDS_MENU_COMPORT, "gui/menu/smallfnt.bmp" ) );

	pimpl_->pComPortSelecter_ = _NEW( MachGuiDropDownListBoxCreator ( &parent(), &startupScreens(), Gui::Coord( SNM_MINX, SNM_MINY+textHeight ), SNM_WIDTH, comStrings, true, true ) );
	pimpl_->pComPortSelecter_->items( comSettings );

	MachGuiDropDownListBoxCreator::DropDownListBoxItems baudSettings;

	GuiStrings baudStrings;
	baudStrings.push_back( "14400" );
	baudSettings.push_back( ( void* ) NetComPortAddress::NET_BAUD_14400 );
	baudStrings.push_back( "19200" );
	baudSettings.push_back( ( void* ) NetComPortAddress::NET_BAUD_19200 );
	baudStrings.push_back( "38400" );
	baudSettings.push_back( ( void* ) NetComPortAddress::NET_BAUD_38400 );
	baudStrings.push_back( "56000" );
	baudSettings.push_back( ( void* ) NetComPortAddress::NET_BAUD_56000 );
	baudStrings.push_back( "57600" );
	baudSettings.push_back( ( void* ) NetComPortAddress::NET_BAUD_57600 );
	baudStrings.push_back( "115200" );
	baudSettings.push_back( ( void* ) NetComPortAddress::NET_BAUD_115200 );
	baudStrings.push_back( "128000" );
	baudSettings.push_back( ( void* ) NetComPortAddress::NET_BAUD_128000 );
	baudStrings.push_back( "256000" );
	baudSettings.push_back( ( void* ) NetComPortAddress::NET_BAUD_256000 );

	GuiResourceString baudHeading( IDS_MENU_BAUD );

	MachGuiMenuText* pBaudText =
  		_NEW( MachGuiMenuText( &startupScreens(),
							   Gui::Box( Gui::Coord(SNM_MINX, SNM_MINY2),
							   			 font.textWidth( baudHeading.asString() ),
							   			 textHeight ), IDS_MENU_BAUD, "gui/menu/smallfnt.bmp" ) );

	pimpl_->pBaudRateSelecter_ = _NEW( MachGuiDropDownListBoxCreator ( &parent(), &startupScreens(), Gui::Coord( SNM_MINX, SNM_MINY2+textHeight ), SNM_WIDTH, baudStrings, true, true ) );
	pimpl_->pBaudRateSelecter_->items( baudSettings );

	MachGuiDropDownListBoxCreator::DropDownListBoxItems paritySettings;

	GuiStrings parityStrings;
	parityStrings.push_back( "NO PARITY" );
	paritySettings.push_back( ( void* ) NetComPortAddress::NO_PARITY );
	parityStrings.push_back( "ODD" );
	paritySettings.push_back( ( void* ) NetComPortAddress::ODD_PARITY );
	parityStrings.push_back( "EVEN" );
	paritySettings.push_back( ( void* ) NetComPortAddress::EVEN_PARITY );
	parityStrings.push_back( "MARK" );
	paritySettings.push_back( ( void* ) NetComPortAddress::MARK_PARITY );

	GuiResourceString parityHeading( IDS_MENU_PARITY );

	MachGuiMenuText* pParityText =
		_NEW( MachGuiMenuText( &startupScreens(),
							   Gui::Box( Gui::Coord(SNM_MINX2, SNM_MINY2),
							   			 font.textWidth( parityHeading.asString() ),
							   			 textHeight ), IDS_MENU_PARITY, "gui/menu/smallfnt.bmp" ) );

	pimpl_->pParitySelecter_ = _NEW( MachGuiDropDownListBoxCreator ( &parent(), &startupScreens(), Gui::Coord( SNM_MINX2, SNM_MINY2+textHeight ), SNM_WIDTH, parityStrings, true, true ) );
	pimpl_->pParitySelecter_->items( paritySettings );

	MachGuiDropDownListBoxCreator::DropDownListBoxItems bitSettings;
	GuiStrings bitStrings;

	bitStrings.push_back("1 BIT" );
	bitSettings.push_back( ( void * ) NetComPortAddress::ONE_STOPBIT );
	bitStrings.push_back("1.5 BITS" );
	bitSettings.push_back( ( void * ) NetComPortAddress::ONE5_STOPBIT );
	bitStrings.push_back("2 BITS" );
	bitSettings.push_back( ( void * ) NetComPortAddress::TWO_STOPBITS );

	GuiResourceString stopBitsHeading( IDS_MENU_STOPBITS );

	MachGuiMenuText* pStopBitsText =
		_NEW( MachGuiMenuText( &startupScreens(),
							   Gui::Box( Gui::Coord(SNM_MINX, SNM_MINY3),
							   			 font.textWidth( stopBitsHeading.asString() ),
							   			 textHeight ), IDS_MENU_STOPBITS, "gui/menu/smallfnt.bmp" ) );


	pimpl_->pStopBitsSelecter_ = _NEW( MachGuiDropDownListBoxCreator ( &parent(), &startupScreens(), Gui::Coord( SNM_MINX, SNM_MINY3+textHeight ), SNM_WIDTH, bitStrings, true, true ) );
	pimpl_->pStopBitsSelecter_->items( bitSettings );

	MachGuiDropDownListBoxCreator::DropDownListBoxItems flowSettings;
	GuiStrings flowStrings;

	flowStrings.push_back( "NONE" );
	flowSettings.push_back( ( void* ) NetComPortAddress::DTR_FLOW );
	flowStrings.push_back( "DTR" );
	flowSettings.push_back( ( void* ) NetComPortAddress::NO_FLOW );
	flowStrings.push_back( "RTS/DTR" );
	flowSettings.push_back( ( void* ) NetComPortAddress::RTSDTR_FLOW );
	flowStrings.push_back( "RTS" );
	flowSettings.push_back( ( void* ) NetComPortAddress::RTS_FLOW );
	flowStrings.push_back( "XONXOFF" );
	flowSettings.push_back( ( void* ) NetComPortAddress::XONXOFF_FLOW );

	GuiResourceString flowHeading( IDS_MENU_FLOW );

	MachGuiMenuText* pFlowText =
		_NEW( MachGuiMenuText( &startupScreens(),
							   Gui::Box( Gui::Coord(SNM_MINX2, SNM_MINY3),
							   			 font.textWidth( flowHeading.asString() ),
							   			 textHeight ), IDS_MENU_FLOW, "gui/menu/smallfnt.bmp" ) );

	pimpl_->pFlowSelecter_ = _NEW( MachGuiDropDownListBoxCreator ( &parent(), &startupScreens(), Gui::Coord( SNM_MINX2, SNM_MINY3+textHeight ), SNM_WIDTH, flowStrings, true, true ) );
	pimpl_->pFlowSelecter_->items( flowSettings );
}

ostream& operator <<( ostream& o, const MachGuiSerialNetworkMode& t )
{

    o << "MachGuiSerialNetworkMode " << (void*)&t << " start" << std::endl;
    o << "MachGuiSerialNetworkMode " << (void*)&t << " end" << std::endl;

    return o;
}

/* End SRLMODE.CPP **************************************************/
