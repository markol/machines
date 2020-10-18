/*
 * C T X H O T K Y . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/ctxhotky.hpp"
#include "ctl/vector.hpp"
#include "gui/font.hpp"
#include "system/pathname.hpp"
#include "machgui/menubutn.hpp"
#include "machgui/menutext.hpp"
#include "machgui/startup.hpp"
#include "machgui/ingame.hpp"
#include "machgui/scrltext.hpp"
#include "machgui/internal/strings.hpp"
#include "utility/linetok.hpp"
#include "render/device.hpp"
#include "render/display.hpp"
#include "ani/smacker.hpp"
#include "device/cd.hpp"
#include <fstream>

MachGuiCtxHotKeys::MachGuiCtxHotKeys( MachGuiStartupScreens* pStartupScreens )
:	MachGuiStartupScreenContext( pStartupScreens ),
	animations_( pStartupScreens, SysPathName("gui/menu/so_anims.anm") )
{
	pStartupScreens->changeBackdrop( "gui/menu/so.bmp" );
    pStartupScreens->cursorOn( true );
    pStartupScreens->desiredCdTrack( MachGuiStartupScreens::MENU_MUSIC );

	const uint HOTKEY_MIN_X 		= 128;
	const uint HOTKEY_MIN_Y 		= 15;
	const uint HOTKEY_MAX_Y			= 350;
	const uint HOTKEY_ACTION_WIDTH 	= 111;
	const uint HOTKEY_KEY_WIDTH 	= 131;
	const uint HOTKEY_SEPARATION	= 2;
	const uint HOTKEY_KEY_X 		=	HOTKEY_MIN_X +
										HOTKEY_ACTION_WIDTH;
	const uint HOTKEY_2NDCOLUMN_X 	= 	HOTKEY_MIN_X +
										HOTKEY_ACTION_WIDTH +
										HOTKEY_KEY_WIDTH +
										HOTKEY_SEPARATION;

	uint smallFontHeight = GuiBmpFont::getFont( SysPathName("gui/menu/smallfnt.bmp") ).charHeight() + 2;
	uint largeFontHeight = GuiBmpFont::getFont( SysPathName("gui/menu/largefnt.bmp") ).charHeight() + 2;

	// Display First Person Control heading
	//
	GuiBmpFont font = GuiBmpFont::getFont( SysPathName("gui/menu/largefnt.bmp") );
	GuiResourceString optionsHeading( IDS_MENU_FIRSTPERSONCONTROL );
	uint headingMaxY = HOTKEY_MIN_Y + largeFontHeight;
	_NEW( MachGuiMenuText( pStartupScreens,
						   Gui::Box( HOTKEY_MIN_X, HOTKEY_MIN_Y,
						   			 HOTKEY_MIN_X + font.textWidth( optionsHeading.asString() ),
						   			 headingMaxY ),
						   IDS_MENU_FIRSTPERSONCONTROL, "gui/menu/largefnt.bmp",  MachGuiMenuText::LEFT_JUSTIFY ) );



	// Create First Person Actions text below First Person Control heading
	//
	uint noLines;
	string hotKey1stPersonActions;
	readHotkeyData( "gui/menu/hk1pAction.dat", hotKey1stPersonActions, noLines );

	uint fstPersonWindowMaxY = headingMaxY + ( noLines * smallFontHeight );

	_NEW( MachGuiMenuText( 	pStartupScreens,
							Gui::Box( 	HOTKEY_MIN_X,
										headingMaxY,
										HOTKEY_KEY_X,
										fstPersonWindowMaxY ),
							hotKey1stPersonActions, "gui/menu/smallfnt.bmp", MachGuiMenuText::LEFT_JUSTIFY ) );



	// Create First Person Keys text next to First Person Action text
	//
	string hotKey1stPersonKeys;
	readHotkeyData( "gui/menu/hk1pKeys.dat", hotKey1stPersonKeys, noLines );
	_NEW( MachGuiMenuText( 	pStartupScreens,
							Gui::Box( 	HOTKEY_KEY_X,
										headingMaxY,
										HOTKEY_KEY_X + HOTKEY_KEY_WIDTH,
										fstPersonWindowMaxY ),
							hotKey1stPersonKeys, "gui/menu/smalwfnt.bmp", MachGuiMenuText::LEFT_JUSTIFY ) );



	// Display General Controls heading underneath First Person Controls hot keys
	//
	font = GuiBmpFont::getFont( SysPathName("gui/menu/largefnt.bmp") );
	GuiResourceString optionsGeneralHeading( IDS_MENU_GENERALCONTROL );
	uint genHeadingMaxY = fstPersonWindowMaxY + largeFontHeight;

	_NEW( MachGuiMenuText( pStartupScreens,
						   Gui::Box(	HOTKEY_MIN_X,
						   				fstPersonWindowMaxY,
						   			 	HOTKEY_MIN_X + font.textWidth( optionsGeneralHeading.asString() ),
						   			 	genHeadingMaxY ),
						   IDS_MENU_GENERALCONTROL, "gui/menu/largefnt.bmp", MachGuiMenuText::LEFT_JUSTIFY ) );



	// Calculate the number of lines of General hotkeys that can be displayed under
	// the First Person hotkeys
	font = GuiBmpFont::getFont( SysPathName("gui/menu/smallfnt.bmp") );

	uint noDisplayableLines = ( HOTKEY_MAX_Y - genHeadingMaxY ) / smallFontHeight;

	string hotKeyGeneralActions;

	readHotkeyData( "gui/menu/hkGenActions.dat", hotKeyGeneralActions, noLines );

   	strings choppedText;

	MachGuiMenuText::chopUpText( hotKeyGeneralActions, 200, font, &choppedText );

	uint noRemainingLines = noLines - noDisplayableLines;

	WAYNE_STREAM( "	noDisplayableLines " << noDisplayableLines << std::endl );
	WAYNE_STREAM( " noRemainingLines - " << noRemainingLines << std::endl );

	string headString, remainderString;

	// Create new strings to be displayed
	for( uint i = 0; i < noDisplayableLines; i++ )
	{
		headString += choppedText[ i ];
		headString += "\n";
	}
	uint stringSize = choppedText.size();

	for( uint i = noDisplayableLines; i < stringSize; i++ )
	{
		remainderString += choppedText[ i ];
		remainderString += "\n";
	}
	WAYNE_STREAM( "headString - " << headString	<< std::endl );
	WAYNE_STREAM( "remainderString - " << remainderString	<< std::endl );

	uint generalWindowMaxY = genHeadingMaxY + ( noDisplayableLines * smallFontHeight	);

	WAYNE_STREAM( "generalHeadingMaxY - " << genHeadingMaxY << std::endl );
	WAYNE_STREAM( "generalWindowMaxY - " << generalWindowMaxY << std::endl );

	WAYNE_STREAM( 	"Head box dimensions: " << HOTKEY_MIN_X << "," << genHeadingMaxY << "," << HOTKEY_MIN_X << ","
					<< generalWindowMaxY << std::endl );

	_NEW( MachGuiMenuText( 	pStartupScreens,
							Gui::Box(	HOTKEY_MIN_X ,
										genHeadingMaxY,
										HOTKEY_KEY_X,
										generalWindowMaxY ),
							headString, "gui/menu/smallfnt.bmp", MachGuiMenuText::LEFT_JUSTIFY ) );

	uint generalRemainderMaxY = HOTKEY_MIN_Y + ( noRemainingLines * smallFontHeight );

	WAYNE_STREAM( "generalRemainderMaxY - " << generalRemainderMaxY << std::endl );

	WAYNE_STREAM( 	" Remainder box dimensions: " << HOTKEY_2NDCOLUMN_X << "," <<  HOTKEY_MIN_Y << "," << HOTKEY_2NDCOLUMN_X
					<< "," <<  generalRemainderMaxY << std::endl );

	_NEW( MachGuiMenuText( 	pStartupScreens,
							Gui::Box(	HOTKEY_2NDCOLUMN_X,
										HOTKEY_MIN_Y,
										HOTKEY_2NDCOLUMN_X + HOTKEY_ACTION_WIDTH,
										generalRemainderMaxY ),
							remainderString, "gui/menu/smallfnt.bmp", MachGuiMenuText::LEFT_JUSTIFY ) );

    string hotKeyGeneralKeys;
	readHotkeyData( "gui/menu/hkGenKeys.dat", hotKeyGeneralKeys, noLines );

	strings choppedupText;
	MachGuiMenuText::chopUpText( hotKeyGeneralKeys, 200, font, &choppedupText );


	headString = remainderString = "";

	// Create new strings to be displayed
	for( uint i = 0; i < noDisplayableLines; i++ )
	{
		headString += choppedupText[ i ];
		headString += "\n";
	}
	for( uint i = noDisplayableLines; i < stringSize; i++ )
	{
		remainderString += choppedupText[ i ];
		remainderString += "\n";
	}
	_NEW( MachGuiMenuText( 	pStartupScreens,
							Gui::Box(	HOTKEY_KEY_X,
										genHeadingMaxY,
										HOTKEY_KEY_X + HOTKEY_KEY_WIDTH,
										generalWindowMaxY ),
							headString, "gui/menu/smalwfnt.bmp", MachGuiMenuText::LEFT_JUSTIFY ) );


	_NEW( MachGuiMenuText( 	pStartupScreens,
							Gui::Box(	HOTKEY_2NDCOLUMN_X + HOTKEY_ACTION_WIDTH ,
										HOTKEY_MIN_Y,
										HOTKEY_2NDCOLUMN_X + HOTKEY_ACTION_WIDTH + HOTKEY_KEY_WIDTH,
										generalRemainderMaxY ),
							remainderString, "gui/menu/smalwfnt.bmp", MachGuiMenuText::LEFT_JUSTIFY ) );

	// Add flick to bottom left of window
	SysPathName hotkeySmackerFile( "flics/gui/hotkeys.smk" );

	// Get flic off hard-disk or CD-Rom
	if ( not hotkeySmackerFile.existsAsFile() )
	{
		// Make sure the cd is stopped before accessing files on it.
		if ( DevCD::instance().isPlayingAudioCd() )
		{
			DevCD::instance().stopPlaying();
		}

		string cdRomDrive;

		if ( MachGui::getCDRomDriveContainingFile( cdRomDrive, "flics/gui/hotkeys.smk" ) )
		{
			hotkeySmackerFile = SysPathName( cdRomDrive + "flics/gui/hotkeys.smk" );

			// Can't play music and smacker anim off CD at same time
			if ( hotkeySmackerFile.existsAsFile() )
			{
				pStartupScreens_->desiredCdTrack( MachGuiStartupScreens::DONT_PLAY_CD );
			}
		}
	}

	// Play animation only if it exists
	if ( hotkeySmackerFile.existsAsFile() )
	{
//		HWND targetWindow = RenDevice::current()->display()->window();
//	 	AniSmacker* pSmackerAnimation = _NEW( AniSmacker( hotkeySmackerFile, targetWindow, 430 + pStartupScreens_->xMenuOffset(), 199 + pStartupScreens_->yMenuOffset() ) );
	 	AniSmacker* pSmackerAnimation = _NEW( AniSmacker( hotkeySmackerFile, 430 + pStartupScreens_->xMenuOffset(), 199 + pStartupScreens_->yMenuOffset() ) );
		pStartupScreens_->addSmackerAnimation( pSmackerAnimation );
 	 }

   	MachGuiMenuButton* pContinueBtn = _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 351, 420, 553, 464 ), IDS_MENUBTN_CONTINUE, MachGuiStartupScreens::EXIT ) );
	_NEW( MachGuiMenuText( pStartupScreens, Gui::Box( 42, 353, 224, 464 ), IDS_MENU_HOTKEYS, "gui/menu/largefnt.bmp" ) );

	pContinueBtn->escapeControl( true );

    TEST_INVARIANT;
}

MachGuiCtxHotKeys::~MachGuiCtxHotKeys()
{
    TEST_INVARIANT;

}

void MachGuiCtxHotKeys::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiCtxHotKeys& t )
{

    o << "MachGuiCtxHotKeys " << (void*)&t << " start" << std::endl;
    o << "MachGuiCtxHotKeys " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiCtxHotKeys::update()
{
	animations_.update();
}

void MachGuiCtxHotKeys::readHotkeyData( const string& hotKeyDataFileName, string& hotkeyString, uint& linesInString )
{
	std::ifstream hotKeyFile( hotKeyDataFileName.c_str() );
	ASSERT( hotKeyFile, hotKeyDataFileName.c_str() );
	char nextChar;
	uint noLines = 0;

	while( hotKeyFile.get( nextChar ) )
	{
		if ( nextChar == '\n' )
			++noLines;
		hotkeyString += nextChar;
	}
	linesInString = noLines;
}
/* End CTXHOTKY.CPP *************************************************/
