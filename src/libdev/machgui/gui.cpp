/*
 * G U I . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */
//#include "windows.h"
#include "stdlib/string.hpp"
#include "system/pathname.hpp"
#include "network/netnet.hpp"

#include "machgui/gui.hpp"
#include "machgui/internal/strings.hpp"
#include "gui/painter.hpp"
#include "gui/gui.hpp"
#include "gui/restring.hpp"

// static
const Gui::Colour&
MachGui::mVIIIBackgroundColour()
{
	static Gui::Colour result_( 35.0/255.0, 30.0/255.0, 61.0/255.0 );
	return result_;
}

// static
const Gui::Colour&
MachGui::OFFWHITE()
{
	static Gui::Colour result_( 239.0/255.0, 238.0/255.0, 222.0/255.0 );
	return result_;
}

// static
const Gui::Colour&
MachGui::VERYDARKGREY()
{
	static Gui::Colour result_( 20.0/255.0, 20.0/255.0, 20.0/255.0 );
	return result_;
}

// static
const Gui::Colour&
MachGui::ALMOSTBLACK()
{
	static Gui::Colour result_( 8.0/255.0, 8.0/255.0, 8.0/255.0 );
	return result_;
}

// static
const Gui::Colour&
MachGui::VERYDARKPURPLE()
{
	static Gui::Colour result_( 57.0/255.0, 49.0/255.0, 77.0/255.0 );
	return result_;
}

// static
const Gui::Colour&
MachGui::LIGHTPURPLE()
{
	static Gui::Colour result_( 157.0/255.0, 156.0/255.0, 182.0/255.0 );
	return result_;
}

// static
const Gui::Colour&
MachGui::PURPLE()
{
	static Gui::Colour result_( 78.0/255.0, 86.0/255.0, 113.0/255.0 );
	return result_;
}

// static
const Gui::Colour&
MachGui::PROGRESSGREEN()
{
	static Gui::Colour result_( 0, 185.0/255.0, 0 );
	return result_;
}

//static
const Gui::Colour&
MachGui::MENUDARKGREEN()
{
	static Gui::Colour result_( 1.0/255.0, 35.0/255.0, 1.0/255.0 );
	return result_;
}

//static
const Gui::Colour&
MachGui::SANDY()
{
	static Gui::Colour result_( 226.0/255.0, 232.0/255.0, 157.0/255.0 );
	return result_;
}

//static
const Gui::Colour&
MachGui::DARKSANDY()
{
	static Gui::Colour result_( 171.0/255.0, 178.0/255.0, 93.0/255.0 );
	return result_;
}

//static
const Gui::Colour&
MachGui::DARKRED()
{
	static Gui::Colour result_( 120.0/255.0, 0, 0 );
	return result_;
}

//static
const Gui::Colour&
MachGui::DARKYELLOW()
{
	static Gui::Colour result_( 120.0/255.0, 120.0/255.0, 0 );
	return result_;
}

//static
const Gui::Colour&
MachGui::DARKGREEN()
{
	static Gui::Colour result_( 0, 120.0/255.0, 0 );
	return result_;
}

//static
const Gui::Colour&
MachGui::DARKBLUE()
{
	static Gui::Colour result_( 0, 0, 120.0/255.0 );
	return result_;
}

//static
const Gui::Colour&
MachGui::DROPDOWNDARKGREEN()
{
	static Gui::Colour result_( 7.0/255.0, 32.0/255.0, 0 );
	return result_;
}

//static
const Gui::Colour&
MachGui::DROPDOWNLIGHTGREEN()
{
	static Gui::Colour result_( 32.0/255.0, 255.0/255.0, 15.0/255.0 );
	return result_;
}

//static
void MachGui::drawNumber( GuiBitmap* pNumberArray, size_t value, Gui::Coord* pAbsCoord )
{
   	do
	{
		size_t numright = value % 10;

		value /= 10;

		pAbsCoord->x( pAbsCoord->x() - pNumberArray[numright].width() );

		GuiPainter::instance().blit( pNumberArray[numright], *pAbsCoord );

	} while ( value );
}

//static
void MachGui::releaseMenuBmpMemory()
{
	GuiBitmap*& pBmp1 = privateLongGlowBmp();
	GuiBitmap*& pBmp2 = privateButtonGlowBmp();
	GuiBitmap*& pBmp3 = privateButtonDisableBmp();
	GuiBitmap*& pBmp4 = privateMenuScrollUpBmp();
	GuiBitmap*& pBmp5 = privateMenuScrollDownBmp();
	GuiBitmap*& pBmp6 = privateOkMsgBoxBmp();
	GuiBitmap*& pBmp7 = privateOkCancelMsgBoxBmp();
	GuiBitmap*& pBmp8 = privateTickBmp();
	GuiBitmap*& pBmp9 = privateDarkTickBmp();
	GuiBitmap*& pBmp10 = privateLargeTickBmp();
	GuiBitmap*& pBmp11 = privateLargeDarkTickBmp();
	GuiBitmap*& pBmp12 = privateDropDownBmp();
	GuiBitmap*& pBmp13 = privateHostBmp();
	GuiBitmap*& pBmp14 = privateSlideBtnBmp();
	GuiBitmap*& pBmp15 = privateLongYellowGlowBmp();

	_DELETE( pBmp1 );
	_DELETE( pBmp2 );
	_DELETE( pBmp3 );
	_DELETE( pBmp4 );
	_DELETE( pBmp5 );
	_DELETE( pBmp6 );
	_DELETE( pBmp7 );
	_DELETE( pBmp8 );
	_DELETE( pBmp9 );
	_DELETE( pBmp10 );
	_DELETE( pBmp11 );
	_DELETE( pBmp12 );
	_DELETE( pBmp13 );
	_DELETE( pBmp14 );
	_DELETE( pBmp15 );

	pBmp1 = NULL;
	pBmp2 = NULL;
	pBmp3 = NULL;
	pBmp4 = NULL;
	pBmp5 = NULL;
	pBmp6 = NULL;
	pBmp7 = NULL;
	pBmp8 = NULL;
	pBmp9 = NULL;
	pBmp10 = NULL;
	pBmp11 = NULL;
	pBmp12 = NULL;
	pBmp13 = NULL;
	pBmp14 = NULL;
	pBmp15 = NULL;
}


//static
GuiBitmap& MachGui::longGlowBmp()
{
	GuiBitmap*& pBmp = privateLongGlowBmp();

	if ( pBmp == NULL )
	{
		pBmp = _NEW( GuiBitmap( Gui::bitmap("gui/menu/longglow.bmp") ) );
	}

	return *pBmp;
}

//static
GuiBitmap& MachGui::buttonGlowBmp()
{
	GuiBitmap*& pBmp = privateButtonGlowBmp();

	if ( pBmp == NULL )
	{
   		pBmp = _NEW( GuiBitmap( Gui::bitmap("gui/menu/glow.bmp") ) );
	}

	return *pBmp;
}

//static
GuiBitmap& MachGui::buttonDisableBmp()
{
	GuiBitmap*& pBmp = privateButtonDisableBmp();

	if ( pBmp == NULL )
	{
		pBmp = _NEW( GuiBitmap( Gui::bitmap("gui/menu/disable.bmp") ) );
		pBmp->enableColourKeying();
	}

	return *pBmp;
}

//static
GuiBitmap& MachGui::menuScrollUpBmp()
{
	GuiBitmap*& pBmp = privateMenuScrollUpBmp();

	if ( pBmp == NULL )
	{
 		pBmp = _NEW( GuiBitmap( Gui::bitmap("gui/menu/scrollup.bmp") ) );
	}

	return *pBmp;
}

//static
GuiBitmap& MachGui::menuScrollDownBmp()
{
	GuiBitmap*& pBmp = privateMenuScrollDownBmp();

	if ( pBmp == NULL )
	{
 		pBmp = _NEW( GuiBitmap( Gui::bitmap("gui/menu/scrolldn.bmp") ) );
	}

	return *pBmp;
}

//static
GuiBitmap& MachGui::okMsgBoxBmp()
{
	GuiBitmap*& pBmp = privateOkMsgBoxBmp();

	if ( pBmp == NULL )
	{
		ASSERT_FILE_EXISTS( "gui/menu/msgbox.bmp" );

		pBmp = _NEW( GuiBitmap( Gui::bitmap("gui/menu/msgbox.bmp") ) );
		pBmp->enableColourKeying();
	}

	return *pBmp;

}

//static
GuiBitmap& MachGui::okCancelMsgBoxBmp()
{
	GuiBitmap*& pBmp = privateOkCancelMsgBoxBmp();

	if ( pBmp == NULL )
	{
		ASSERT_FILE_EXISTS( "gui/menu/msgbox2.bmp" );

		pBmp = _NEW( GuiBitmap( Gui::bitmap("gui/menu/msgbox2.bmp") ) );
		pBmp->enableColourKeying();
	}

	return *pBmp;
}

//static
GuiBitmap& MachGui::tickBmp()
{
	GuiBitmap*& pBmp = privateTickBmp();

	if ( pBmp == NULL )
	{
		pBmp = _NEW( GuiBitmap( Gui::bitmap("gui/menu/tick.bmp") ) );
		pBmp->enableColourKeying();
	}

	return *pBmp;
}

//static
GuiBitmap& MachGui::darkTickBmp()
{
	GuiBitmap*& pBmp = privateDarkTickBmp();

	if ( pBmp == NULL )
	{
		pBmp = _NEW( GuiBitmap( Gui::bitmap("gui/menu/tickd.bmp") ) );
		pBmp->enableColourKeying();
	}

	return *pBmp;
}

//static
GuiBitmap& MachGui::largeDarkTickBmp()
{
	GuiBitmap*& pBmp = privateLargeDarkTickBmp();

	if ( pBmp == NULL )
	{
 		pBmp = _NEW( GuiBitmap( Gui::bitmap("gui/menu/tickld.bmp") ) );
		pBmp->enableColourKeying();
	}

	return *pBmp;
}

//static
GuiBitmap& MachGui::largeTickBmp()
{
	GuiBitmap*& pBmp = privateLargeTickBmp();

	if ( pBmp == NULL )
	{
 		pBmp = _NEW( GuiBitmap( Gui::bitmap("gui/menu/tickl.bmp") ) );
		pBmp->enableColourKeying();
	}

	return *pBmp;
}

//static
GuiBitmap& MachGui::dropDownBmp()
{
	GuiBitmap*& pBmp = privateDropDownBmp();

	if ( pBmp == NULL )
	{
		pBmp = _NEW( GuiBitmap( Gui::bitmap("gui/menu/dropdown.bmp") ) );
		pBmp->enableColourKeying();
	}

	return *pBmp;
}

//static
GuiBitmap& MachGui::hostBmp()
{
	GuiBitmap*& pBmp = privateHostBmp();

	if ( pBmp == NULL )
	{
		ASSERT_FILE_EXISTS( "gui/menu/host.bmp" );

		pBmp = _NEW( GuiBitmap( Gui::bitmap("gui/menu/host.bmp") ) );
		pBmp->enableColourKeying();
	}

	return *pBmp;
}

//static
GuiBitmap& MachGui::slideBtnBmp()
{
	GuiBitmap*& pBmp = privateSlideBtnBmp();

	if ( pBmp == NULL )
	{
		ASSERT_FILE_EXISTS( "gui/menu/slidebtn.bmp" );

		pBmp = _NEW( GuiBitmap( Gui::bitmap("gui/menu/slidebtn.bmp") ) );
		pBmp->enableColourKeying();
	}

	return *pBmp;
}

//static
GuiBitmap& MachGui::controlPanelBmp()
{
	GuiBitmap*& pBmp = privateControlPanelBmp();

	if ( pBmp == NULL )
	{
		ASSERT_FILE_EXISTS( "gui/misc/backtile.bmp" );

	   pBmp = _NEW( GuiBitmap( Gui::bitmap("gui/misc/backtile.bmp") ) );
	}

	return *pBmp;
}

//static
GuiBitmap& MachGui::inTransitBmp()
{
	GuiBitmap*& pBmp = privateInTransitBmp();

	if ( pBmp == NULL )
	{
		ASSERT_FILE_EXISTS( "gui/misc/transit.bmp" );

   		pBmp = _NEW( GuiBitmap( Gui::bitmap("gui/misc/transit.bmp") ) );
		pBmp->enableColourKeying();
	}

	return *pBmp;
}

//static
GuiBitmap*& MachGui::privateLongGlowBmp()
{
	static GuiBitmap* pBmp = NULL;

	return pBmp;
}

//static
GuiBitmap*& MachGui::privateButtonGlowBmp()
{
	static GuiBitmap* pBmp = NULL;

	return pBmp;
}

//static
GuiBitmap*& MachGui::privateButtonDisableBmp()
{
	static GuiBitmap* pBmp = NULL;

	return pBmp;
}

//static
GuiBitmap*& MachGui::privateMenuScrollUpBmp()
{
	static GuiBitmap* pBmp = NULL;

	return pBmp;
}

//static
GuiBitmap*& MachGui::privateMenuScrollDownBmp()
{
	static GuiBitmap* pBmp = NULL;

	return pBmp;
}

//static
GuiBitmap*& MachGui::privateTickBmp()
{
	static GuiBitmap* pBmp = NULL;

	return pBmp;
}

//static
GuiBitmap*& MachGui::privateDarkTickBmp()
{
	static GuiBitmap* pBmp = NULL;

	return pBmp;
}

//static
GuiBitmap*& MachGui::privateLargeTickBmp()
{
	static GuiBitmap* pBmp = NULL;

	return pBmp;
}

//static
GuiBitmap*& MachGui::privateLargeDarkTickBmp()
{
	static GuiBitmap* pBmp = NULL;

	return pBmp;
}

//static
GuiBitmap*& MachGui::privateDropDownBmp()
{
	static GuiBitmap* pBmp = NULL;

	return pBmp;
}

//static
GuiBitmap*& MachGui::privateHostBmp()
{
	static GuiBitmap* pBmp = NULL;

	return pBmp;
}

//static
GuiBitmap*& MachGui::privateOkMsgBoxBmp()
{
	static GuiBitmap* pBmp = NULL;

	return pBmp;
}

//static
GuiBitmap*& MachGui::privateOkCancelMsgBoxBmp()
{
	static GuiBitmap* pBmp = NULL;

	return pBmp;
}

//static
GuiBitmap*& MachGui::privateSlideBtnBmp()
{
	static GuiBitmap* pBmp = NULL;

	return pBmp;
}

//static
GuiBitmap*& MachGui::privateControlPanelBmp()
{
	static GuiBitmap* pBmp = NULL;

	return pBmp;
}

//static
GuiBitmap*& MachGui::privateInTransitBmp()
{
	static GuiBitmap* pBmp = NULL;

	return pBmp;
}

//static
const string& MachGui::closedText()
{
	static string text;
	if ( text.length() == 0 )
	{
		GuiResourceString str( IDS_MENU_CLOSED );
		text = str.asString();
	}

	return text;
}

//static
const string& MachGui::openText()
{
	static string text;
	if ( text.length() == 0 )
	{
		GuiResourceString str( IDS_MENU_OPEN );
		text = str.asString();
	}

	return text;
}

//static
const string& MachGui::computerText()
{
	static string text;
	if ( text.length() == 0 )
	{
		GuiResourceString str( IDS_MENU_COMPUTER );
		text = str.asString();
	}

	return text;
}

//static
const string& MachGui::unavailableText()
{
	static string text;
	if ( text.length() == 0 )
	{
		GuiResourceString str( IDS_MENU_UNAVAILABLE );
		text = str.asString();
	}

	return text;
}

//static
int MachGui::controlPanelInXPos()
{
	return 6;
}

//static
int MachGui::controlPanelOutXPos()
{
	return 169;
}

//static
int MachGui::controlPanelSlideInSpeed()
{
	return 15;
}

//static
int MachGui::controlPanelSlideOutSpeed()
{
	return 20;
}

//static
int MachGui::promptTextXOffset()
{
	return -7;
}

//static
int MachGui::promptTextYOffset()
{
	return -24;
}

//static
int MachGui::mapAreaHeight()
{
	return 129;
}

//static
int MachGui::convertNetworkError( int errorCode )
{
	switch( (NetNetwork::NetNetworkStatus)errorCode )
	{
		case NetNetwork::NETNET_OK: return IDS_MENUMSG_NETSUCCESS;
		case NetNetwork::NETNET_CONNECTIONERROR: return IDS_MENUMSG_NETCONNECTIONERROR;
		case NetNetwork::NETNET_INVALIDCONNECTIONDATA: return IDS_MENUMSG_NETINVALIDCONNECTIONDATA;
		case NetNetwork::NETNET_INVALIDDATA: return IDS_MENUMSG_NETINVALIDDATA;
		case NetNetwork::NETNET_UNDEFINEDERROR: return IDS_MENUMSG_NETUNDEFINEDERROR;
		case NetNetwork::NETNET_CANTINITIALIZEDIRECTPLAY: return IDS_MENUMSG_NETCANTINITIALIZEDIRECTPLAY;
		case NetNetwork::NETNET_MEMORYERROR: return IDS_MENUMSG_NETMEMORYERROR;
		case NetNetwork::NETNET_SESSIONERROR: return IDS_MENUMSG_NETSESSIONERROR;
		case NetNetwork::NETNET_NODEERROR: return IDS_MENUMSG_NETNODEERROR;
	}
	return IDS_MENUMSG_NETSUCCESS;
}

// static
void MachGui::releaseInGameBmpMemory()
{
	GuiBitmap*& pBmp1 = privateInTransitBmp();
	GuiBitmap*& pBmp2 = privateControlPanelBmp();

	_DELETE( pBmp1 );
	_DELETE( pBmp2 );

	pBmp1 = NULL;
	pBmp2 = NULL;
}

//static
bool MachGui::getCDRomDriveContainingMachinesCD( string& cdRomDrive, int CDNumber )
{
	PRE( CDNumber > 0 and CDNumber < 3 );

/*	if ( CDNumber == 1 )
		return getCDRomDriveContainingFile( cdRomDrive, "setupx.bin" );
	else
		return getCDRomDriveContainingFile( cdRomDrive, "setupy.bin" );*/
    return CDNumber == 2;
}

//static
bool MachGui::machinesCDIsAvailable( int CDNumber )
{
	PRE( CDNumber > 0 and CDNumber < 3 );

	string cdRomDrive;

	return 	getCDRomDriveContainingMachinesCD( cdRomDrive, CDNumber );
}

// static
bool MachGui::getCDRomDriveContainingFile( string& cdRomDrive, const string& fileToCheckFor )
{
	char buffer[255];

//	GetLogicalDriveStrings( sizeof(buffer) - 1, buffer );

	int nBuffer = 0;
	char* bufferPos = &buffer[nBuffer];

	bool found = false;

/*	while ( bufferPos and not found )
	{
		NEIL_STREAM( "checking " << bufferPos << std::endl );
		if ( GetDriveType( bufferPos ) == DRIVE_CDROM )
		{
			NEIL_STREAM( "CD-ROM " << bufferPos << std::endl );
			string cdRomFile( bufferPos );
			cdRomFile += fileToCheckFor;
			SysPathName sysFileToCheckFor( cdRomFile );

			NEIL_STREAM( "CD-ROM checking for " << sysFileToCheckFor.c_str() << std::endl );

			if ( sysFileToCheckFor.existsAsFile() )
			{
				NEIL_STREAM( "CD-ROM found file" << std::endl );
				cdRomDrive = bufferPos;
				found = true;
			}
		}

		while ( bufferPos and not found )
		{
			++nBuffer;
		    bufferPos = &buffer[nBuffer];
			if ( strcmp( bufferPos, "" ) == 0 )
			{
				++nBuffer;
			    bufferPos = &buffer[nBuffer];

				if ( strcmp( bufferPos, "" ) == 0 )
				{
					bufferPos = NULL;
				}
				else
				{
					break;
				}
			}
		}
	}

	NEIL_STREAM( "CD-ROM " << found << " " << cdRomDrive << std::endl );
*/
	return found;
}

//static
GuiBitmap& MachGui::longYellowGlowBmp()
{
	GuiBitmap*& pBmp = privateLongYellowGlowBmp();

	if ( pBmp == NULL )
	{
		pBmp = _NEW( GuiBitmap( Gui::bitmap("gui/menu/lonyglow.bmp") ) );
	}

	return *pBmp;
}

//static
GuiBitmap*& MachGui::privateLongYellowGlowBmp()
{
	static GuiBitmap* pBmp = NULL;

	return pBmp;
}

//static
const string& MachGui::wonSymbolText()
{
    static const char wonChar[2] = { (char)127, 0};
	//static string wonSymbol( (char)127 );
	static string wonSymbol( wonChar );

	return wonSymbol;
}

/* End GUI.CPP ******************************************************/
