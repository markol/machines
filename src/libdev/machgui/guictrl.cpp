/*
 * G U I C T R L . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include "gui/manager.hpp"

#include "gui/painter.hpp"
#include "gui/event.hpp"
#include "gui/restring.hpp"
#include "machgui/internal/strings.hpp"
#include "machgui/gui.hpp"
#include "machgui/guictrl.hpp"
#include "machgui/ingame.hpp"
#include "system/pathname.hpp"
#include "machlog/races.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/squad.hpp"  
#include "ctl/pvector.hpp"
#include "machgui/internal/mgsndman.hpp"

unsigned char pPromptText5[] = { 180, 210, 47, 227, 51, 90, 137, 155, 240, 173, 174, 32, 220, 14, 59, 25, 84, 149, 74, 247, 23, 48, 98, 49, 34, 12, 7, 66, 4, 124, 7, 200, 229, 126, 154, 94, 234, 129, 16, 80, 214 };
	   
MachGuiIconWithCounter::MachGuiIconWithCounter( GuiDisplayable* pParent, const Gui::Coord& rel, const SysPathName& bitmap, MachInGameScreen* pInGameScreen )
: GuiBitmapButtonWithFilledBorder( 	pParent, 
									rel, 
									GuiBorderMetrics(1,1,1), 
									GuiFilledBorderColours( Gui::Colour(144.0/255.0,148.0/255.0,160.0/255.0),
															Gui::Colour(232.0/255.0,232.0/255.0,232.0/255.0), 
														   	Gui::Colour(62.0/255.0,62.0/255.0,62.0/255.0),
														    Gui::RED() ), 
									Gui::bitmap(bitmap), 
									Gui::Coord(1,1) ),
  pInGameScreen_( pInGameScreen ),
  currentValue_( 0 )
{
	// Intentionally Empty
}

//virtual 
MachGuiIconWithCounter::~MachGuiIconWithCounter()
{
	// Intentionally Empty
}

//virtual 
void MachGuiIconWithCounter::doDisplayInteriorEnabled( const Gui::Coord& abs )
{
	GuiBitmapButtonWithFilledBorder::doDisplayInteriorEnabled( abs );

	static GuiBitmap numbers[10] = { Gui::bitmap( SysPathName( "gui/navigate/numtop0.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/navigate/numtop1.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/navigate/numtop2.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/navigate/numtop3.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/navigate/numtop4.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/navigate/numtop5.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/navigate/numtop6.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/navigate/numtop7.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/navigate/numtop8.bmp" ) ),
									 Gui::bitmap( SysPathName( "gui/navigate/numtop9.bmp" ) ) };

	Gui::Coord absCopy( abs );
	absCopy.y( absCopy.y() + 20 ); //TODO : remove hard coded values
	absCopy.x( absCopy.x() + 20 );

	MachGui::drawNumber( numbers, currentValue_, &absCopy );
}

MachMachinesIcon::MachMachinesIcon( GuiDisplayable* pParent, const Gui::Coord& rel, MachInGameScreen* pInGameScreen )
: 	MachGuiIconWithCounter( pParent, rel, SysPathName("gui/navigate/red/machines.bmp"), pInGameScreen ),
	forcingUp_( false )
{
	popupButton( false );
}

MachMachinesIcon::~MachMachinesIcon()
{
	// Intentionally Empty
}

void MachMachinesIcon::doBeDepressed( const GuiMouseEvent& )
{
	pInGameScreen_->machineNavigationContext();
	MachGuiSoundManager::instance().playSound( "gui/sounds/igclick.wav" );
}

void MachMachinesIcon::doBeReleased( const GuiMouseEvent& )
{
	if ( not forcingUp_ )
	{
		pInGameScreen_->mainMenuOrSingleFactoryContext();
	}
}

void MachMachinesIcon::forceUp()
{
	if ( isDepressed() )
	{
		forcingUp_ = true;
		setDepressed( false );
		forcingUp_ = false;
	}
}

void MachMachinesIcon::refresh()
{
    //Get the values and update the dials
    MachLogRaces& races = MachLogRaces::instance();
    MachPhys::Race playerRace = races.pcController().race();

	size_t newValue = races.nMachines( playerRace );

	if ( newValue != currentValue_ )
	{
		currentValue_ = newValue;
		changed();
	}
}

//virtual
void MachMachinesIcon::doHandleMouseEnterEvent( const GuiMouseEvent& mouseEvent )
{
	GuiBitmapButtonWithFilledBorder::doHandleMouseEnterEvent( mouseEvent );

    //Load the resource string
	GuiResourceString prompt( IDS_MACHINE_NAV );

    //Set the cursor prompt
    pInGameScreen_->cursorPromptText( prompt.asString() );
}

//virtual
void MachMachinesIcon::doHandleMouseExitEvent( const GuiMouseEvent& mouseEvent )
{
    //Clear the cursor prompt string
    pInGameScreen_->clearCursorPromptText();

	GuiBitmapButtonWithFilledBorder::doHandleMouseExitEvent( mouseEvent );
}

MachConstructionsIcon::MachConstructionsIcon( GuiDisplayable* pParent, const Gui::Coord& rel, MachInGameScreen* pInGameScreen )
: 	MachGuiIconWithCounter( pParent, rel, SysPathName("gui/navigate/red/construc.bmp"), pInGameScreen ),
	forcingUp_( false )
{
	popupButton( false );
}

MachConstructionsIcon::~MachConstructionsIcon()
{
	// Intentionally Empty
}

void MachConstructionsIcon::doBeDepressed( const GuiMouseEvent& )
{
	pInGameScreen_->constructionNavigationContext();
	MachGuiSoundManager::instance().playSound( "gui/sounds/igclick.wav" );
}

void MachConstructionsIcon::doBeReleased( const GuiMouseEvent& )
{
	if ( not forcingUp_ )
	{
		pInGameScreen_->mainMenuOrSingleFactoryContext();
 	}
}

void MachConstructionsIcon::forceUp()
{
	if ( isDepressed() )
	{
		forcingUp_ = true;
		setDepressed( false );
		forcingUp_ = false;
	}
}

void MachConstructionsIcon::refresh()
{
    MachLogRaces& races = MachLogRaces::instance();
    MachPhys::Race playerRace = races.pcController().race();

	size_t newValue = races.nConstructions( playerRace );

	if ( newValue != currentValue_ )
	{
		currentValue_ = newValue;
		changed();
	}
}

//virtual
void MachConstructionsIcon::doHandleMouseEnterEvent( const GuiMouseEvent& mouseEvent )
{
	MachGuiIconWithCounter::doHandleMouseEnterEvent( mouseEvent );

    //Load the resource string
	GuiResourceString prompt( IDS_CONSTRUCTION_NAV );

    //Set the cursor prompt
    pInGameScreen_->cursorPromptText( prompt.asString() );

}

//virtual
void MachConstructionsIcon::doHandleMouseExitEvent( const GuiMouseEvent& mouseEvent )
{
    //Clear the cursor prompt string
    pInGameScreen_->clearCursorPromptText();

	MachGuiIconWithCounter::doHandleMouseExitEvent( mouseEvent );
}


/* //////////////////////////////////////////////////////////////// */

MachSquadronIcon::MachSquadronIcon( GuiDisplayable* pParent, const Gui::Coord& rel, MachInGameScreen* pInGameScreen )
: MachGuiIconWithCounter( pParent, rel, SysPathName("gui/navigate/red/squads.bmp"), pInGameScreen ),
  pInGameScreen_( pInGameScreen )
{
	// Intentionally Empty
}

MachSquadronIcon::~MachSquadronIcon()
{
	// Intentionally Empty
}

void MachSquadronIcon::doBeDepressed( const GuiMouseEvent& )
{
	MachGuiSoundManager::instance().playSound( "gui/sounds/igclick.wav" );
}

void MachSquadronIcon::doBeReleased( const GuiMouseEvent& )
{
    //Clear all selections
    pInGameScreen_->deselectAll();

    //Enter the squadron menu
	pInGameScreen_->squadronContext();
}

void MachSquadronIcon::update()
{
	size_t newValue = 0;

	for ( size_t i = 0; i < 10; ++i )
	{
	   	MachLogRaces& races = MachLogRaces::instance();
   		MachPhys::Race race = races.pcController().race();
        MachLogSquadron* pSquadron = races.squadrons( race )[ i ];
        if ( not pSquadron->machines().empty() )
		{
			++newValue;
		}
	}

	if ( currentValue_ != newValue )
	{
		currentValue_ = newValue;
		changed();
	}
}

//virtual
void MachSquadronIcon::doHandleMouseEnterEvent( const GuiMouseEvent& mouseEvent )
{
	MachGuiIconWithCounter::doHandleMouseEnterEvent( mouseEvent );

    //Load the resource string
	GuiResourceString prompt( IDS_SQUAD_NAV );

    //Set the cursor prompt
    pInGameScreen_->cursorPromptText( prompt.asString() );
}

//virtual
void MachSquadronIcon::doHandleMouseExitEvent( const GuiMouseEvent& mouseEvent )
{
    //Clear the cursor prompt string
    pInGameScreen_->clearCursorPromptText();

	MachGuiIconWithCounter::doHandleMouseExitEvent( mouseEvent );
}

string MachGuiIconWithCounter::getRootDirectory() const
{
	string retValue = "gui/navigate/red/";

	if ( MachLogRaces::instance().hasPCRace() )
	{
		switch ( MachLogRaces::instance().pcController().race() )
		{
			case MachPhys::RED:
				break;
			case MachPhys::GREEN:
				retValue = "gui/navigate/green/";
				break;
			case MachPhys::BLUE:
				retValue = "gui/navigate/blue/";
				break;
			case MachPhys::YELLOW:
				retValue = "gui/navigate/yellow/";
				break;
			DEFAULT_ASSERT_BAD_CASE( MachLogRaces::instance().pcController().race() );
		}
	}

	return retValue;
}

void MachConstructionsIcon::loadGame()
{
	bitmap( Gui::bitmap( getRootDirectory() + "construc.bmp" ) );

	refresh();
}

void MachMachinesIcon::loadGame()
{
	bitmap( Gui::bitmap( getRootDirectory() + "machines.bmp" ) );

	refresh();
}

void MachSquadronIcon::loadGame()
{
	bitmap( Gui::bitmap( getRootDirectory() + "squads.bmp" ) );

	update();
}

/* End GUICTRL.CPP **************************************************/