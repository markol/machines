/*
 * G U I . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _MACH_GUI_HPP
#define _MACH_GUI_HPP

#include "gui/gui.hpp"
#include "stdlib/string.hpp"

class MachGui
{
public:
	// Draws a number onto the screen. numberArray is an array of bitmaps, each
	// bitmap representing a number. The index position in the array is the same
	// as the number shown in the bitmap.
	static void drawNumber( GuiBitmap* pNumberArray, size_t value, Gui::Coord* pAbsCoord );
	// PRE( pAbsCoord );

	static const Gui::Colour& mVIIIBackgroundColour();
	static const Gui::Colour& OFFWHITE();
	static const Gui::Colour& VERYDARKGREY();
	static const Gui::Colour& ALMOSTBLACK();
	static const Gui::Colour& VERYDARKPURPLE();
	static const Gui::Colour& LIGHTPURPLE();
	static const Gui::Colour& PURPLE();
	static const Gui::Colour& PROGRESSGREEN();

	static const Gui::Colour& MENUDARKGREEN();
	static const Gui::Colour& SANDY();
	static const Gui::Colour& DARKSANDY();
	static const Gui::Colour& DARKRED();
	static const Gui::Colour& DARKYELLOW();
	static const Gui::Colour& DARKGREEN();
	static const Gui::Colour& DARKBLUE();
	static const Gui::Colour& DROPDOWNDARKGREEN();
	static const Gui::Colour& DROPDOWNLIGHTGREEN();


	static void releaseMenuBmpMemory();
	// Cached menu bitmaps
	static GuiBitmap& longGlowBmp();
	static GuiBitmap& longYellowGlowBmp();
	static GuiBitmap& buttonGlowBmp();
	static GuiBitmap& buttonDisableBmp();
	static GuiBitmap& menuScrollUpBmp();
	static GuiBitmap& menuScrollDownBmp();
	static GuiBitmap& okMsgBoxBmp();
	static GuiBitmap& okCancelMsgBoxBmp();
	static GuiBitmap& tickBmp();
	static GuiBitmap& darkTickBmp();
	static GuiBitmap& largeTickBmp();
	static GuiBitmap& largeDarkTickBmp();
	static GuiBitmap& dropDownBmp();
	static GuiBitmap& hostBmp();
	static GuiBitmap& slideBtnBmp();
	static GuiBitmap& controlPanelBmp();
	static GuiBitmap& inTransitBmp();

	static const string& closedText();
	static const string& openText();
	static const string& computerText();
	static const string& unavailableText();
	static const string& wonSymbolText();

	// Various gui layout values
	static int controlPanelInXPos();
	static int controlPanelOutXPos();
	static int controlPanelSlideInSpeed();
	static int controlPanelSlideOutSpeed();
	static int promptTextXOffset();
	static int promptTextYOffset();
	static int mapAreaHeight();

    //Context enumerations
    enum ControlPanelContext
    {
        MAIN_MENU,
        SQUADRON_MENU,
        MACHINE_NAVIGATION_MENU,
        CONSTRUCTION_NAVIGATION_MENU,
        CONSTRUCT_COMMAND,
        BUILD_COMMAND,
        HARDWARE_RESEARCH,
        SOFTWARE_RESEARCH,
        FORM_SQUADRON_COMMAND,
		SINGLE_FACTORY
    };

    //Cursor enumeration
    enum Cursor2dType
    {
        MENU_CURSOR,
        INVALID_CURSOR,
        MOVETO_CURSOR,
        ATTACK_CURSOR,
        CONSTRUCT_CURSOR,
        LOCATETO_CURSOR,
        PATROL_CURSOR,
        DEPLOY_CURSOR,
        PICKUP_CURSOR,
        TRANSPORT_CURSOR,
        ENTER_BUILDING_CURSOR,
        ENTER_APC_CURSOR,
        FOLLOW_CURSOR,
        SELECT_CURSOR,
		DROPLANDMINE_CURSOR,
		HEAL_CURSOR,
		DECONSTRUCT_CURSOR,
		RECYCLE_CURSOR,
		STANDGROUND_CUSROR,
		REPAIR_CURSOR,
		CAPTURE_CURSOR,
		JOINCONSTRUCT_CURSOR,
		ASSEMBLEPOINT_CURSOR,
		ION_ATTACK_CURSOR,
		NUKE_ATTACK_CURSOR,
		SCROLL_N_CURSOR,
		SCROLL_S_CURSOR,
		SCROLL_E_CURSOR,
		SCROLL_W_CURSOR,
		SCROLL_NE_CURSOR,
		SCROLL_NW_CURSOR,
		SCROLL_SE_CURSOR,
		SCROLL_SW_CURSOR,
		SCAVENGE_CURSOR,
		CHOOSE_CONST_CURSOR,
		TREACHERY_CURSOR
    };

	static int convertNetworkError( int );

	static void releaseInGameBmpMemory();

	// Funtions that check for existance of CD
	static bool getCDRomDriveContainingFile( string& cdRomDrive, const string& fileToCheckFor );

	static bool getCDRomDriveContainingMachinesCD( string& cdRomDrive, int CDNumber );
	//PRE( CDNumber > and 0 CDNumber < 3 );

	static bool machinesCDIsAvailable( int CDNumber );
	//PRE( CDNumber > and 0 CDNumber < 3 );

private:
	static GuiBitmap*& privateLongGlowBmp();
	static GuiBitmap*& privateLongYellowGlowBmp();
	static GuiBitmap*& privateButtonGlowBmp();
	static GuiBitmap*& privateButtonDisableBmp();
	static GuiBitmap*& privateMenuScrollUpBmp();
	static GuiBitmap*& privateMenuScrollDownBmp();
	static GuiBitmap*& privateOkMsgBoxBmp();
	static GuiBitmap*& privateOkCancelMsgBoxBmp();
	static GuiBitmap*& privateTickBmp();
	static GuiBitmap*& privateDarkTickBmp();
	static GuiBitmap*& privateLargeTickBmp();
	static GuiBitmap*& privateLargeDarkTickBmp();
	static GuiBitmap*& privateDropDownBmp();
	static GuiBitmap*& privateHostBmp();
	static GuiBitmap*& privateSlideBtnBmp();
	static GuiBitmap*& privateControlPanelBmp();
	static GuiBitmap*& privateInTransitBmp();
};

#endif	// #ifndef _MACH_GUI_HPP

/* End GUI.HPP ******************************************************/
