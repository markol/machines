/*
 * C T X S I N G L . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/ctxsingl.hpp"
#include "machgui/menubutn.hpp"
#include "machgui/menutext.hpp"
#include "machgui/startup.hpp"
#include "machgui/internal/strings.hpp"

MachGuiCtxSinglePlayer::MachGuiCtxSinglePlayer(MachGuiStartupScreens* pStartupScreens )
:	MachGuiStartupScreenContext( pStartupScreens ),
	animations_( pStartupScreens, SysPathName("gui/menu/si_anims.anm") )
{
  	_NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 401, 62,  588, 105 ), IDS_MENUBTN_CAMPAIGN, MachGuiStartupScreens::CAMPAIGN ) );
	_NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 401, 169, 588, 211 ), IDS_MENUBTN_SKIRMISH, MachGuiStartupScreens::SKIRMISH ) );
	_NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 401, 272, 588, 314 ), IDS_MENUBTN_LOADGAME, MachGuiStartupScreens::LOADGAME ) );
	MachGuiMenuButton* pCancelBtn = _NEW( MachGuiMenuButton( pStartupScreens, Gui::Box( 401, 370, 588, 412 ), IDS_MENUBTN_CANCEL, MachGuiStartupScreens::EXIT ) );
	_NEW( MachGuiMenuText( pStartupScreens, Gui::Box( 82, 292, 262, 399 ), IDS_MENU_SINGLEPLAYER, "gui/menu/largefnt.bmp" ) );
	pCancelBtn->escapeControl( true );

	pStartupScreens->changeBackdrop( "gui/menu/si.bmp" ); 
    pStartupScreens->cursorOn( true );
    pStartupScreens->desiredCdTrack( MachGuiStartupScreens::MENU_MUSIC );

    TEST_INVARIANT;
}

MachGuiCtxSinglePlayer::~MachGuiCtxSinglePlayer()
{
    TEST_INVARIANT;

}

void MachGuiCtxSinglePlayer::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiCtxSinglePlayer& t )
{

    o << "MachGuiCtxSinglePlayer " << (void*)&t << " start" << std::endl;
    o << "MachGuiCtxSinglePlayer " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiCtxSinglePlayer::update()
{
	animations_.update();
}


/* End CTXSINGL.CPP *************************************************/
