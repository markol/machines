/*
 * S C R L T E X T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/scrltext.hpp"
#include "machgui/startup.hpp"
#include "machgui/menutext.hpp"
#include "machgui/text.hpp"
#include "gui/restring.hpp"
#include "gui/font.hpp"
#include "gui/painter.hpp"

MachGuiScrollableText::MachGuiScrollableText( MachGuiStartupScreens* pParent, const Gui::Box& box, uint stringId )
:	GuiSimpleScrollableList( pParent, box, 1000, GuiBmpFont::getFont("gui/menu/smallfnt.bmp").charHeight() + 1, 1 ),
	pStartupScreens_( pParent )
{
	setText( stringId );

    TEST_INVARIANT;
}

MachGuiScrollableText::MachGuiScrollableText( MachGuiStartupScreens* pParent, const Gui::Box& box, const string& text )
:	GuiSimpleScrollableList( pParent, box, 1000, GuiBmpFont::getFont("gui/menu/smallfnt.bmp").charHeight() + 1, 1 ),
	pStartupScreens_( pParent )
{
	setText( text );

    TEST_INVARIANT;
}


MachGuiScrollableText::MachGuiScrollableText( MachGuiStartupScreens* pParent, const Gui::Box& box )
:	GuiSimpleScrollableList( pParent, box, 1000, GuiBmpFont::getFont("gui/menu/smallfnt.bmp").charHeight() + 1, 1 ),
	pStartupScreens_( pParent )
{
    TEST_INVARIANT;
}

MachGuiScrollableText::MachGuiScrollableText( MachGuiStartupScreens* pParent, const Gui::Box& box, uint columnWidth, const string& text )
:	GuiSimpleScrollableList( pParent, box, columnWidth, GuiBmpFont::getFont("gui/menu/smallfnt.bmp").charHeight() + 1, 1 ),
	pStartupScreens_( pParent )
{
	setText( text );

    TEST_INVARIANT;
}

void MachGuiScrollableText::setText( uint stringId )
{
	GuiResourceString text( stringId );
	setText( text.asString() );
}

void MachGuiScrollableText::setText( const string& text )
{
	deleteAllChildren();

	strings linesOfText;
    linesOfText.reserve( 64 );
	MachGuiMenuText::chopUpText( text, width(), GuiBmpFont::getFont("gui/menu/smallfnt.bmp"), &linesOfText );

	for ( strings::iterator iter = linesOfText.begin(); iter != linesOfText.end(); ++iter )
	{
		string lineOfText = *iter;

		NEIL_STREAM( lineOfText << std::endl );

		if ( strncasecmp(&lineOfText.c_str()[0], "<w>",3) == 0 )
		{
			_NEW( MachGuiText( this, pStartupScreens_, width(), &lineOfText.c_str()[3], "gui/menu/smalwfnt.bmp" ) );
		}
		else
		{
			_NEW( MachGuiText( this, pStartupScreens_, width(), lineOfText ) );
		}
	}

	childrenUpdated();
}

MachGuiScrollableText::~MachGuiScrollableText()
{
    TEST_INVARIANT;

}

void MachGuiScrollableText::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiScrollableText& t )
{

    o << "MachGuiScrollableText " << (void*)&t << " start" << std::endl;
    o << "MachGuiScrollableText " << (void*)&t << " end" << std::endl;

    return o;
}

// virtual
void MachGuiScrollableText::doDisplay()
{
	// Blit background to list box item
	pStartupScreens_->blitBackdrop( absoluteBoundary(),
									absoluteBoundary().minCorner() );
}

/* End SCRLTEXT.CPP *************************************************/
