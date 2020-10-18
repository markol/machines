/*
 * S S L I S T I T . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/sslistit.hpp"
#include "machgui/sslistbx.hpp"
#include "machgui/startup.hpp"
#include "machgui/gui.hpp"
#include "gui/font.hpp"
#include "gui/painter.hpp"
#include "machgui/internal/mgsndman.hpp"

MachGuiSingleSelectionListBoxItem::MachGuiSingleSelectionListBoxItem( MachGuiStartupScreens* pStartupScreens, MachGuiSingleSelectionListBox* pListBox, size_t width, const string& text )
: 	GuiSingleSelectionListBoxItem( pListBox, width, reqHeight() ),
	text_( text ),
	highlighted_( false ),
	pStartupScreens_( pStartupScreens ),
	pMyListBox_( pListBox )
{
	pMyListBox_->addListItem( this );

    TEST_INVARIANT;
}

MachGuiSingleSelectionListBoxItem::~MachGuiSingleSelectionListBoxItem()
{
    TEST_INVARIANT;

	if ( pMyListBox_ )
		pMyListBox_->removeListItem( this );
}

void MachGuiSingleSelectionListBoxItem::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiSingleSelectionListBoxItem& t )
{

    o << "MachGuiSingleSelectionListBoxItem " << (void*)&t << " start" << std::endl;
    o << "MachGuiSingleSelectionListBoxItem " << (void*)&t << " end" << std::endl;

    return o;
}

//static 
size_t MachGuiSingleSelectionListBoxItem::reqHeight()
{
	size_t myReqHeight = getFont().charHeight();

	myReqHeight += 5;

	return myReqHeight;
}

//static 
GuiBmpFont MachGuiSingleSelectionListBoxItem::getFont()
{
	GuiBmpFont bmpFont = GuiBmpFont::getFont( SysPathName("gui/menu/smallfnt.bmp") );

	return bmpFont;
}

//static 
GuiBmpFont MachGuiSingleSelectionListBoxItem::getHighlightFont()
{
	GuiBmpFont bmpFont = GuiBmpFont::getFont( SysPathName("gui/menu/smaldfnt.bmp") );

	return bmpFont;
}

//static 
GuiBmpFont MachGuiSingleSelectionListBoxItem::getUnderlineFont()
{
	GuiBmpFont bmpFont = GuiBmpFont::getFont( SysPathName("gui/menu/smaldfnt.bmp") );

	bmpFont.underline( true );

	return bmpFont;
}

//virtual 
void MachGuiSingleSelectionListBoxItem::select()
{
	// Play select sound
	MachGuiSoundManager::instance().playSound( "gui/sounds/listclik.wav" );

	changed();
}

//virtual 
void MachGuiSingleSelectionListBoxItem::unselect()
{
	changed();
}

//virtual 
void MachGuiSingleSelectionListBoxItem::doDisplay()
{
	if ( selected() )
	{
		if ( pMyListBox_ and pMyListBox_->isFocusControl() )
		{
			GuiPainter::instance().blit( MachGui::longYellowGlowBmp(), Gui::Box(0,0,width(),height() - 1), absoluteBoundary().minCorner() );
		}
		else
		{
			GuiPainter::instance().blit( MachGui::longGlowBmp(), Gui::Box(0,0,width(),height() - 1), absoluteBoundary().minCorner() );
		}

		getUnderlineFont().drawText( text_, Gui::Coord( absoluteBoundary().minCorner().x() + 1, absoluteBoundary().minCorner().y() + 1 ), width() );
	}
	else if ( highlighted() )
	{
		if ( pMyListBox_ and pMyListBox_->isFocusControl() )
		{
			GuiPainter::instance().blit( MachGui::longYellowGlowBmp(), Gui::Box(0,0,width(),height() - 1), absoluteBoundary().minCorner() );
		}
		else
		{
			GuiPainter::instance().blit( MachGui::longGlowBmp(), Gui::Box(0,0,width(),height() - 1), absoluteBoundary().minCorner() );
		}

		getHighlightFont().drawText( text_, Gui::Coord( absoluteBoundary().minCorner().x() + 1, absoluteBoundary().minCorner().y() + 1 ), width() );
	}
	else
	{
		// Blit background to list box item
		pStartupScreens_->blitBackdrop( absoluteBoundary(), 
										absoluteBoundary().minCorner() );

		// Draw list box item text
		getFont().drawText( text_, Gui::Coord( absoluteBoundary().minCorner().x() + 1, absoluteBoundary().minCorner().y() + 1 ), width() );
	}
}

bool MachGuiSingleSelectionListBoxItem::highlighted() const
{
	return highlighted_;
}	   

//virtual 
void MachGuiSingleSelectionListBoxItem::doHandleMouseEnterEvent( const GuiMouseEvent& /*rel*/ )
{
	// Play enter sound
	MachGuiSoundManager::instance().playSound( "gui/sounds/listhigh.wav" );

	highlighted_ = true;

	if ( not selected() )
		changed();
}

//virtual 
void MachGuiSingleSelectionListBoxItem::doHandleMouseExitEvent( const GuiMouseEvent& /*rel*/ )
{
	highlighted_ = false;

	if ( not selected() )
		changed();
}

const string& MachGuiSingleSelectionListBoxItem::text() const
{
	return text_;
}

MachGuiStartupScreens* MachGuiSingleSelectionListBoxItem::startupScreens()
{
	return pStartupScreens_;
}

MachGuiSingleSelectionListBoxItem::MachGuiSingleSelectionListBoxItem( MachGuiStartupScreens* pStartupScreens, GuiSingleSelectionListBox* pListBox, size_t width, const string& text )
: 	GuiSingleSelectionListBoxItem( pListBox, width, reqHeight() ),
	text_( text ),
	highlighted_( false ),
	pStartupScreens_( pStartupScreens ),
	pMyListBox_( NULL )
{
    TEST_INVARIANT;
}

MachGuiSingleSelectionListBox* MachGuiSingleSelectionListBoxItem::myListBox()
{
	return pMyListBox_;
}

/* End SSLISTIT.CPP *************************************************/
