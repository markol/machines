/*
 * C T X M M E N U . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/ctxmmenu.hpp"
#include "machgui/menubutn.hpp"
#include "machgui/menutext.hpp"
#include "machgui/startup.hpp"
#include "machgui/animatio.hpp"
#include "machgui/msgbox.hpp"
#include "machgui/internal/strings.hpp"
#include "gui/font.hpp"
#include "system/pathname.hpp"

///////////////////////////////////////////////////////////////////////////////////////////

class MachGuiMainMenuMessageBoxResponder : public MachGuiMessageBoxResponder
{
public:
	MachGuiMainMenuMessageBoxResponder( MachGuiCtxMainMenu* pMainMenuCtx )
	:	pMainMenuCtx_( pMainMenuCtx )
	{}
	
	virtual bool okPressed()
	{
		pMainMenuCtx_->quit();
		return true;
	}
	
private:
	MachGuiCtxMainMenu* pMainMenuCtx_;	
};

///////////////////////////////////////////////////////////////////////////////////////////

MachGuiCtxMainMenu::MachGuiCtxMainMenu( MachGuiStartupScreens* pStartupScreens )
:	MachGuiStartupScreenContext( pStartupScreens ),
	animations_( pStartupScreens, SysPathName("gui/menu/sa_anims.anm") ),
	quit_( false )
{
	// Access all menu fonts to make sure they are loaded ready for other context to use. This will
	// stop annoying pauses as other context access and use fonts for the first time.
	GuiBmpFont::getFont("gui/menu/smallfnt.bmp");
	GuiBmpFont::getFont("gui/menu/smaldfnt.bmp");
	GuiBmpFont::getFont("gui/menu/largefnt.bmp");
	GuiBmpFont::getFont("gui/menu/largdfnt.bmp");
	
#ifdef DEMO
   	_NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 368, 40,  527, 75  ), IDS_MENUBTN_CAMPAIGN, MachGuiStartupScreens::CAMPAIGN ) );
	_NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 368, 130, 527, 165 ), IDS_MENUBTN_MULTIPLAYER, MachGuiStartupScreens::MULTIPLAYER ) );
	_NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 368, 217, 527, 252 ), IDS_MENUBTN_SKIRMISH, MachGuiStartupScreens::SKIRMISH ) );
	_NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 368, 297, 527, 332 ), IDS_MENUBTN_OPTIONS, MachGuiStartupScreens::OPTIONS ) );
	MachGuiMenuButton* pExitBtn = _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 117, 410, 275, 445 ), IDS_MENUBTN_EXIT, MachGuiStartupScreens::BE_DUMMY_OK ) );

#else
   	_NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 368, 40,  527, 75  ), IDS_MENUBTN_SINGLEPLAYER, MachGuiStartupScreens::SINGLEPLAYER ) );
	_NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 368, 130, 527, 165 ), IDS_MENUBTN_MULTIPLAYER, MachGuiStartupScreens::MULTIPLAYER ) );
	_NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 368, 217, 527, 252 ), IDS_MENUBTN_OPTIONS, MachGuiStartupScreens::OPTIONS ) );
	MachGuiMenuButton* pExitBtn = _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 368, 297, 527, 332 ), IDS_MENUBTN_EXIT, MachGuiStartupScreens::BE_DUMMY_OK ) );

#endif

	// Create exit button, responds to escape...
	pExitBtn->escapeControl( true );

	_NEW( MachGuiMenuText( pStartupScreens, Gui::Box( 97, 32, 243, 122 ), IDS_MENU_MAINMENU, "gui/menu/largefnt.bmp" ) );

	pStartupScreens->changeBackdrop( "gui/menu/sa.bmp" ); 
    pStartupScreens->cursorOn( true );
	pStartupScreens->desiredCdTrack( MachGuiStartupScreens::MENU_MUSIC );

    TEST_INVARIANT;
}

MachGuiCtxMainMenu::~MachGuiCtxMainMenu()
{
    TEST_INVARIANT;

}

void MachGuiCtxMainMenu::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiCtxMainMenu& t )
{

    o << "MachGuiCtxMainMenu " << (void*)&t << " start" << std::endl;
    o << "MachGuiCtxMainMenu " << (void*)&t << " end" << std::endl;

    return o;
}

void MachGuiCtxMainMenu::update()
{
	if ( quit_ )
	{
		pStartupScreens_->buttonAction( MachGuiStartupScreens::EXIT );
	}
	else
	{
		animations_.update();
	}
}

// virtual
bool MachGuiCtxMainMenu::okayToSwitchContext()
{
	if ( pStartupScreens_->lastButtonEvent() == MachGuiStartupScreens::BE_DUMMY_OK )
	{
		pStartupScreens_->displayMsgBox( IDS_MENUMSG_QUIT, _NEW( MachGuiMainMenuMessageBoxResponder(this) ), true );
		return false;	
	}

	return true;
}

void MachGuiCtxMainMenu::quit()
{
	quit_ = true;
}	 

/* End CTXMMENU.CPP *************************************************/
