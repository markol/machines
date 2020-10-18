/*
 * B M U T E X T . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "machgui/bmutext.hpp"
#include "machgui/map.hpp"
#include "machgui/gui.hpp"
#include "machgui/ingame.hpp"
#include "gui/restring.hpp"
#include "machgui/internal/strings.hpp"
#include "machlog/races.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machphys/machphys.hpp"
#include "gui/painter.hpp"
#include "machgui/internal/mgsndman.hpp"

MachGuiBmuText::MachGuiBmuText( GuiDisplayable* pParent, const Gui::Coord& relPos )
:	GuiDisplayable( pParent, Gui::Boundary( relPos, 126, 10 ) )
{
	DEBUG_STREAM( DIAG_NEIL, "Creating MachGuiBmuText" << std::endl );

    TEST_INVARIANT;
}

MachGuiBmuText::~MachGuiBmuText()
{
    TEST_INVARIANT;

}

void MachGuiBmuText::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void MachGuiBmuText::refresh()
{
    MachLogRaces& races = MachLogRaces::instance();
    MachPhys::Race playerRace = races.pcController().race();

    curValue_ = races.nBuildingMaterialUnits( playerRace );
	maxValue_ = races.nMaxBuildingMaterialUnits( playerRace );
}

//virtual
void MachGuiBmuText::doDisplay()
{
	static GuiBitmap numbers[10] = { Gui::bitmap( SysPathName( "gui/navigate/numbmu0.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/navigate/numbmu1.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/navigate/numbmu2.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/navigate/numbmu3.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/navigate/numbmu4.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/navigate/numbmu5.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/navigate/numbmu6.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/navigate/numbmu7.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/navigate/numbmu8.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/navigate/numbmu9.bmp" ) ) };
	static GuiBitmap slash( Gui::bitmap( SysPathName( "gui/navigate/numbmus.bmp" ) ) );

	Gui::Coord absCopy( absoluteBoundary().minCorner().x() + width(), absoluteBoundary().minCorner().y() );

	for ( size_t loop = 0; loop < 2; ++loop )
	{
		MachGui::drawNumber( numbers, loop == 0 ? maxValue_ : curValue_, &absCopy );

		if ( loop == 0 )
		{
			absCopy.x( absCopy.x() - slash.width() );

			GuiPainter::instance().blit( slash, absCopy );
		}
	}
}

//virtual 
void MachGuiBmuText::doHandleMouseClickEvent( const GuiMouseEvent& rel )
{
	parent().doHandleMouseClickEvent( rel );
}

//virtual 
void MachGuiBmuText::doHandleMouseEnterEvent( const GuiMouseEvent& rel )
{
	parent().doHandleMouseEnterEvent( rel );
}

//virtual 
void MachGuiBmuText::doHandleMouseExitEvent( const GuiMouseEvent& rel )
{
	parent().doHandleMouseExitEvent( rel );
}

//virtual 
void MachGuiBmuText::doHandleContainsMouseEvent( const GuiMouseEvent& rel )
{
	parent().doHandleContainsMouseEvent( rel );
}

ostream& operator <<( ostream& o, const MachGuiBmuText& t )
{

    o << "MachGuiBmuText " << (void*)&t << " start" << std::endl;
    o << "MachGuiBmuText " << (void*)&t << " end" << std::endl;

    return o;
}



MachGuiBmuButton::MachGuiBmuButton( GuiDisplayable* pParent,
									const Gui::Coord& rel,
									const SysPathName& bitmap,
									MachGuiBmuText* pBmuText,
									MachContinentMap* pContinentMap,
									MachInGameScreen *pInGameScreen )
: 	GuiBitmapButtonWithFilledBorder(pParent, 
									rel, 
									GuiBorderMetrics(1,1,1), 
									GuiFilledBorderColours( Gui::Colour(144.0/255.0,148.0/255.0,160.0/255.0), 
															Gui::Colour(232.0/255.0,232.0/255.0,232.0/255.0), 
														   	Gui::Colour(62.0/255.0,62.0/255.0,62.0/255.0),
														    Gui::RED() ), 
									Gui::bitmap(bitmap), 
									Gui::Coord(1,1) ),
 	pBmuText_( pBmuText ),
 	pContinentMap_( pContinentMap ),
	pInGameScreen_( pInGameScreen )
{
	popupButton( false );
}

//virtual 
MachGuiBmuButton::~MachGuiBmuButton() 
{}

//virtual 
void MachGuiBmuButton::doBeDepressed( const GuiMouseEvent& ) 
{
	MachGuiSoundManager::instance().playSound( "gui/sounds/igclick.wav" );
	pBmuText_->isVisible( true );
	pContinentMap_->forceUpdate();
}

//virtual 
void MachGuiBmuButton::doBeReleased( const GuiMouseEvent& )
{
	MachGuiSoundManager::instance().playSound( "gui/sounds/igclick.wav" );
	pBmuText_->isVisible( false );
	pContinentMap_->forceUpdate();
}

//virtual
void MachGuiBmuButton::doHandleMouseEnterEvent( const GuiMouseEvent& mouseEvent )
{
	GuiBitmapButtonWithFilledBorder::doHandleMouseEnterEvent( mouseEvent );

    //Load the resource string
	GuiResourceString prompt( IDS_BMU_BUTTON );

    //Set the cursor prompt
    pInGameScreen_->cursorPromptText( prompt.asString() );
}

//virtual
void MachGuiBmuButton::doHandleMouseExitEvent( const GuiMouseEvent& mouseEvent )
{
    //Clear the cursor prompt string
    pInGameScreen_->clearCursorPromptText();

	GuiBitmapButtonWithFilledBorder::doHandleMouseExitEvent( mouseEvent );
}

// Forced recompile 19/2/99 CPS
/* End BMUTEXT.CPP **************************************************/
