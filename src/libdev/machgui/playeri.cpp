/*
 * P L A Y E R I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/internal/playeri.hpp"
#include "gui/gui.hpp"
#include "gui/font.hpp"
#include "gui/painter.hpp"
#include "machgui/gui.hpp"
#include "machgui/menutext.hpp"

#define PNL_BOX_WIDTH 77
#define PNL_INDENT 6

MachGuiPlayerNameList::MachGuiPlayerNameList( GuiDisplayable *pParent, const Gui::Coord& topLeft, const string& fontName, uint title )
:	GuiDisplayable( pParent, Gui::Boundary( topLeft, PNL_BOX_WIDTH, 77 ) ),
	isWhite_( false )
{
	// Create title text
	_NEW( MachGuiMenuText( this, 
						   Gui::Box( Gui::Coord( 0, 0 ), PNL_BOX_WIDTH, 35 ), 
						   title, fontName, MachGuiMenuText::CENTRE_JUSTIFY ) );

    TEST_INVARIANT;
}

MachGuiPlayerNameList::MachGuiPlayerNameList(  GuiDisplayable *pParent, const Gui::Boundary& boundary, const string& fontName, uint title, bool isWhite )
:	GuiDisplayable( pParent, boundary ),
	isWhite_( isWhite )
{
	// Create title text
	_NEW( MachGuiMenuText( pParent, 
						   Gui::Box( boundary.minCorner(), 
						   			 boundary.maxCorner().x() - boundary.minCorner().x(), 35 ), 
						   title, fontName, MachGuiMenuText::CENTRE_JUSTIFY ) );

    TEST_INVARIANT;
}

MachGuiPlayerNameList::~MachGuiPlayerNameList()
{
    TEST_INVARIANT;

}

void MachGuiPlayerNameList::names( const string& player1, const string& player2, const string& player3, const string& player4 )
{
	
	GuiBmpFont font( GuiBmpFont::getFont("gui/menu/smalwfnt.bmp") );
	
	uint spacing = font.charHeight() + 2;
	const uint maxWidth = PNL_BOX_WIDTH - ( PNL_INDENT*2 ) - 10;

	string newPlayer1 = truncate( player1, maxWidth );
	string newPlayer2 = truncate( player2, maxWidth );
	string newPlayer3 = truncate( player3, maxWidth );
	string newPlayer4 = truncate( player4, maxWidth );

	_NEW( MachGuiMenuText( this, 
						   Gui::Box( Gui::Coord( PNL_INDENT,38 ),
						   			 font.textWidth( newPlayer1 ), spacing ), 
						   newPlayer1, "gui/menu/smalwfnt.bmp" , MachGuiMenuText::LEFT_JUSTIFY ) );
	
	_NEW( MachGuiMenuText( this, 
						   Gui::Box( Gui::Coord( PNL_INDENT,38 + spacing ),
						   			 font.textWidth( newPlayer2 ), spacing ), 
						   newPlayer2, "gui/menu/smalwfnt.bmp" , MachGuiMenuText::LEFT_JUSTIFY ) );

	_NEW( MachGuiMenuText( this, 
						   Gui::Box( Gui::Coord( PNL_INDENT,38 + ( spacing*2 ) ),
						   			 font.textWidth( newPlayer3 ), spacing ), 
						   newPlayer3, "gui/menu/smalwfnt.bmp" , MachGuiMenuText::LEFT_JUSTIFY ) );

	_NEW( MachGuiMenuText( this, 
						   Gui::Box( Gui::Coord( PNL_INDENT,38 + ( spacing*3 ) ),
						   			 font.textWidth( newPlayer4 ), spacing ), 
						   newPlayer4, "gui/menu/smalwfnt.bmp" , MachGuiMenuText::LEFT_JUSTIFY ) );

}

// virtual
void MachGuiPlayerNameList::doDisplay( void )
{
	const Gui::Boundary& boundary = absoluteBoundary();

	if( isWhite_ )
	{
		// Draw bounding box
		GuiPainter::instance().hollowRectangle( boundary, MachGui::OFFWHITE(), 1 );
		// Line across box
		GuiPainter::instance().line( Gui::Coord( boundary.minCorner().x(), boundary.minCorner().y() + 34 ),
								 	Gui::Coord( boundary.maxCorner().x(), boundary.minCorner().y()+ 34 ),
								 	MachGui::OFFWHITE(), 1 );
	}
	else
	{
		GuiPainter::instance().hollowRectangle( boundary, MachGui::DROPDOWNLIGHTGREEN(), 1 );
		// Line across box
		GuiPainter::instance().line( Gui::Coord( boundary.minCorner().x(), boundary.minCorner().y() + 34 ),
								 	Gui::Coord( boundary.maxCorner().x(), boundary.minCorner().y()+ 34 ),
								 	MachGui::DROPDOWNLIGHTGREEN(), 1 );
	}
}

void MachGuiPlayerNameList::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

// static
string MachGuiPlayerNameList::truncate( const string& name, uint maxWidth)
{
	GuiBmpFont font( GuiBmpFont::getFont("gui/menu/smalwfnt.bmp") );
	string truncatedName = name;

	const uint maxChars = maxWidth / font.maxCharWidth() + font.spacing();

	if( font.textWidth( name ) > maxWidth )
	{
		truncatedName.resize( maxChars );		
	}
	POST( truncatedName.length() <= maxChars );

	return truncatedName;
}

ostream& operator <<( ostream& o, const MachGuiPlayerNameList& t )
{

    o << "MachGuiPlayerNameList " << (void*)&t << " start" << std::endl;
    o << "MachGuiPlayerNameList " << (void*)&t << " end" << std::endl;

    return o;
}

/* End PLAYERS.CPP **************************************************/
