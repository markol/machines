/*
 * G E N S T A T S . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "machgui/genstats.hpp"
#include "gui/displaya.hpp"
#include "gui/font.hpp"
#include "gui/painter.hpp"
#include "machgui/gui.hpp"
#include "machgui/menutext.hpp"

#define GS_BOX_WIDTH 77
#define GS_INDENT 6

MachGuiGeneralStatistics::MachGuiGeneralStatistics( GuiDisplayable *pParent, Gui::Coord topLeft, uint titleString, MachPhys::Race race )
: 	GuiDisplayable( pParent, Gui::Boundary( topLeft, 77, 77 ) ),
   	statDisplay_( pParent, Gui::Coord( topLeft.x()+1, topLeft.y()+35 ), race )
{
	// Create title text
	_NEW( MachGuiMenuText( pParent, 
						   Gui::Box( topLeft, 77, 35 ), 
						   titleString, "gui/menu/smallfnt.bmp", MachGuiMenuText::CENTRE_JUSTIFY ) );

    TEST_INVARIANT;
}

MachGuiGeneralStatistics::MachGuiGeneralStatistics( GuiDisplayable *pParent, Gui::Coord topLeft, uint titleString, MachPhys::Race race, const string& player )
: 	GuiDisplayable( pParent, Gui::Boundary( topLeft, 77, 77 ) ),
   	statDisplay_( pParent, Gui::Coord( topLeft.x()+1, topLeft.y()+35 ), race )
{
	GuiBmpFont font( GuiBmpFont::getFont("gui/menu/smalwfnt.bmp") );
		
	// Create title text
	_NEW( MachGuiMenuText( pParent, 
						   Gui::Box( topLeft, 77, 33 - font.charHeight() - 2 ), 
						   titleString, "gui/menu/smallfnt.bmp", MachGuiMenuText::CENTRE_JUSTIFY ) );

	string newPlayer = truncate( player );
	// Create player name text
	_NEW( MachGuiMenuText( pParent, 
						   Gui::Box( Gui::Coord( topLeft.x(), topLeft.y() + 33 - font.charHeight() - 2 ), 
						   			 77, font.charHeight() + 2 ), 
						   newPlayer, "gui/menu/smalwfnt.bmp", MachGuiMenuText::CENTRE_JUSTIFY ) );

    TEST_INVARIANT;
}

MachGuiGeneralStatistics::~MachGuiGeneralStatistics()
{
    TEST_INVARIANT;

}

// virtual
void MachGuiGeneralStatistics::update()
{
	if( statDisplay_.redraw() )
	{
		statDisplay_.update();
	}
}

// virtual
void MachGuiGeneralStatistics::setStatistics( int stat1, int stat2, int stat3, int stat4 )
{
	statDisplay_.setStatistics( stat1, stat2, stat3, stat4 );	
}

// virtual 
void MachGuiGeneralStatistics::doDisplay()
{
	const Gui::Boundary& boundary = absoluteBoundary();

	// Draw bounding box
	GuiPainter::instance().hollowRectangle( boundary, MachGui::DROPDOWNLIGHTGREEN(), 1 );

	// Line across box
	GuiPainter::instance().line( Gui::Coord( boundary.minCorner().x(), boundary.minCorner().y() + 34 ),
								 Gui::Coord( boundary.maxCorner().x(), boundary.minCorner().y()+ 34 ),
								 MachGui::DROPDOWNLIGHTGREEN(), 1 );
}

void MachGuiGeneralStatistics::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

// static
string MachGuiGeneralStatistics::truncate( const string& name )
{
	GuiBmpFont font( GuiBmpFont::getFont("gui/menu/smalwfnt.bmp") );
	string truncatedName = name;

	const uint maxWidth = GS_BOX_WIDTH - ( GS_INDENT*2 ) - 10;
	const uint maxChars = maxWidth / font.maxCharWidth() + font.spacing();

	if( font.textWidth( name ) > maxWidth )
	{
		truncatedName.resize( maxChars );		
	}
	POST( truncatedName.length() <= maxChars );

	return truncatedName;
}

ostream& operator <<( ostream& o, const MachGuiGeneralStatistics& t )
{

    o << "MachGuiGeneralStatistics " << (void*)&t << " start" << std::endl;
    o << "MachGuiGeneralStatistics " << (void*)&t << " end" << std::endl;

    return o;
}

/* End GENSTATS.CPP *************************************************/
