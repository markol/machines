/*
 * D R O P D W N C . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/dropdwnc.hpp"
#include "machgui/gui.hpp"
#include "machgui/startup.hpp"
#include "machgui/sslistit.hpp"
#include "gui/font.hpp"
#include "gui/painter.hpp"
#include "gui/event.hpp"

MachGuiDropDownListBoxCreator::MachGuiDropDownListBoxCreator( 	GuiDisplayable* pParent,
																MachGuiStartupScreens* pStartupScreens,
																int width,
																const GuiStrings& availText )
: 	GuiDisplayable( pParent, Gui::Box(0,0,width,reqHeight() ) ),
	MachGuiFocusCapableControl( pStartupScreens ),
	strings_( availText ),
	pStartupScreens_( pStartupScreens ),
	highlighted_( false ),
	whiteFont_( false ),
	border_( false )
{
	text_ = availText[0];

    TEST_INVARIANT;
}

MachGuiDropDownListBoxCreator::MachGuiDropDownListBoxCreator( 	GuiDisplayable* pParent,
																MachGuiStartupScreens* pStartupScreens,
																int width,
																const GuiStrings& availText,
																bool whiteFont )
: 	GuiDisplayable( pParent, Gui::Box( 0,0,width,reqHeight() ) ),
	MachGuiFocusCapableControl( pStartupScreens ),
	strings_( availText ),
	pStartupScreens_( pStartupScreens ),
	highlighted_( false ),
	whiteFont_( whiteFont ),
	border_( false )
{
	text_ = availText[0];

    TEST_INVARIANT;
}

MachGuiDropDownListBoxCreator::MachGuiDropDownListBoxCreator( 	GuiDisplayable* pParent,
																MachGuiStartupScreens* pStartupScreens,
																const Gui::Coord& relCoord,
																int width,
																const GuiStrings& availText,
																bool whiteFont,
																bool border )
: 	GuiDisplayable( pParent, Gui::Box( relCoord, width, reqHeight( border ) ) ),
	MachGuiFocusCapableControl( pStartupScreens ),
	strings_( availText ),
	pStartupScreens_( pStartupScreens ),
	highlighted_( false ),
	whiteFont_( whiteFont ),
	border_( border )
{
	PRE(availText.size() > 0);
	text_ = availText[0];

    TEST_INVARIANT;
}

MachGuiDropDownListBoxCreator::~MachGuiDropDownListBoxCreator()
{
    TEST_INVARIANT;

}

void MachGuiDropDownListBoxCreator::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiDropDownListBoxCreator& t )
{

    o << "MachGuiDropDownListBoxCreator " << (void*)&t << " start" << std::endl;
    o << "MachGuiDropDownListBoxCreator " << (void*)&t << " end" << std::endl;

    return o;
}

const string& MachGuiDropDownListBoxCreator::text() const
{
	return text_;
}

void MachGuiDropDownListBoxCreator::text( const string& newText )
{
	text_ = newText;
	changed();
}

const MachGuiDropDownListBoxCreator::DropDownListBoxItem MachGuiDropDownListBoxCreator::item() const
{
	// Find value assosciated with currently selected text
	PRE( items() );

	uint i;

	for(  i = 0; i < strings_.size() and strings_[i] != text(); ++i )
	{
		// Intentionally empty
	}

	ASSERT_INFO( text() );
	ASSERT( strings_[i] == text(), "The selected text in the listbox does not match any of the available text items in the listbox" );

	const DropDownListBoxItem returnItem = items_[i];

	return returnItem;
}

bool MachGuiDropDownListBoxCreator::items() const
{
	return ( items_.size() != 0 );
}

//static
size_t MachGuiDropDownListBoxCreator::reqHeight(  bool border /*= false*/ )
{
	size_t height = getFont().charHeight() + 4;

	if ( border )
		height += 2;

	return height;
}

//virtual
void MachGuiDropDownListBoxCreator::doHandleMouseEnterEvent( const GuiMouseEvent& /*rel*/ )
{
	highlighted_ = true;
	changed();
}

//virtual
void MachGuiDropDownListBoxCreator::doHandleMouseExitEvent( const GuiMouseEvent& /*rel*/ )
{
	highlighted_ = false;
	changed();
}

//virtual
void MachGuiDropDownListBoxCreator::doHandleMouseClickEvent( const GuiMouseEvent& rel )
{
	if( strings_.size() )
	{
		if ( rel.leftButton() == Gui::RELEASED )
		{
			size_t dropDownHeight = strings_.size() * MachGuiSingleSelectionListBoxItem::reqHeight();

			Gui::Coord coord = absoluteBoundary().minCorner();

			// Make sure that selected string appears first in list
			GuiStrings orderedStrings;
			orderedStrings.push_back( text_ );

			for ( GuiStrings::iterator iter = strings_.begin(); iter != strings_.end(); ++iter )
			{
				// Ignore text_, do not add into list again.
//				if ( stricmp( (*iter).c_str(), text_.c_str() ) != 0 )
				if ( strcasecmp( (*iter).c_str(), text_.c_str() ) != 0 )
				{
					orderedStrings.push_back( (*iter) );
				}
			}

			createDropDownList(	pStartupScreens_,
								Gui::Box( Gui::Coord( coord.x() - pStartupScreens_->xMenuOffset(),
											coord.y() - pStartupScreens_->yMenuOffset() ),
											width(), dropDownHeight ),
								1000,
								MachGuiSingleSelectionListBoxItem::reqHeight() - 1 /* slight overlap*/,
								1,
								width(),
								orderedStrings,
								this );
		}
	}
}

//virtual
void MachGuiDropDownListBoxCreator::doDisplay()
{
	int offset = 0;
	if ( border_ )
	{
		offset = 1;
	}
	int offset2 = 2*offset;

	Gui::Coord textPos( absoluteBoundary().minCorner().x() + 1 + offset, absoluteBoundary().minCorner().y() + 1 + offset );

	if ( highlighted_ )
	{
		if ( isFocusControl() )
		{
			GuiPainter::instance().blit( 	MachGui::longYellowGlowBmp(),
											Gui::Box( 0,0,width(),height() - 1 ),
											absoluteBoundary().minCorner() );
		}
		else
		{
			GuiPainter::instance().blit( 	MachGui::longGlowBmp(),
											Gui::Box( 0,0,width(),height() - 1 ),
											absoluteBoundary().minCorner() );
		}
		getHighlightFont().drawText( text_, textPos, width() - offset2 );
	}
	else
	{
		if ( isFocusControl() )
		{
			GuiPainter::instance().blit( 	MachGui::longYellowGlowBmp(),
											Gui::Box( 0,0,width(),height() - 1 ),
											absoluteBoundary().minCorner() );

			getHighlightFont().drawText( text_, textPos, width() - offset2 );
		}
		else
		{
			// Blit background to list box item
			pStartupScreens_->blitBackdrop( absoluteBoundary(),
											absoluteBoundary().minCorner() );

			// Draw list box item text
			if ( whiteFont_ )
			{
				getWhiteFont().drawText( text_, textPos, width() - offset2 );
			}
			else
			{
				getFont().drawText( text_, textPos, width() - offset2 );
			}
		}
	}

	if ( border_ )
	{
		GuiPainter::instance().hollowRectangle( absoluteBoundary(), MachGui::DARKSANDY(), 1 );
	}
}

//virtual
MachGuiDropDownList* MachGuiDropDownListBoxCreator::createDropDownList(	MachGuiStartupScreens* pParent,
															const Gui::Box& box,
					    									size_t horizontalSpacing,
					    									size_t verticalSpacing,
															size_t scrollInc, size_t itemWidth,
															const ctl_vector<string>& itemText,
															MachGuiDropDownListBoxCreator* listBoxCreator )
{
	return _NEW( MachGuiDropDownList(	pParent, box, horizontalSpacing, verticalSpacing,
										scrollInc, itemWidth, itemText, listBoxCreator,
										whiteFont_ ) );
}

//static
GuiBmpFont MachGuiDropDownListBoxCreator::getFont()
{
	GuiBmpFont bmpFont = GuiBmpFont::getFont( SysPathName("gui/menu/smallfnt.bmp") );

	return bmpFont;
}

//static
GuiBmpFont MachGuiDropDownListBoxCreator::getWhiteFont()
{
	GuiBmpFont bmpFont = GuiBmpFont::getFont( SysPathName("gui/menu/smalwfnt.bmp") );

	return bmpFont;
}

//static
GuiBmpFont MachGuiDropDownListBoxCreator::getHighlightFont()
{
	GuiBmpFont bmpFont = GuiBmpFont::getFont( SysPathName("gui/menu/smaldfnt.bmp") );

	return bmpFont;
}

MachGuiDropDownList::MachGuiDropDownList(	MachGuiStartupScreens* pParent, const Gui::Box& box,
										    size_t horizontalSpacing, size_t verticalSpacing,
											size_t scrollInc, size_t itemWidth, const ctl_vector<string>& itemText,
											MachGuiDropDownListBoxCreator* pCreator )
:	MachGuiDropDownListBox( pParent, box, horizontalSpacing, verticalSpacing, scrollInc, itemWidth, itemText ),
	pCreator_( pCreator )
{}

MachGuiDropDownList::MachGuiDropDownList(	MachGuiStartupScreens* pParent, const Gui::Box& box,
										    size_t horizontalSpacing, size_t verticalSpacing,
											size_t scrollInc, size_t itemWidth, const ctl_vector<string>& itemText,
											MachGuiDropDownListBoxCreator* pCreator, bool whiteFont )
:	MachGuiDropDownListBox( pParent, box, horizontalSpacing, verticalSpacing, scrollInc, itemWidth, itemText, whiteFont ),
	pCreator_( pCreator )
{}

//virtual
void MachGuiDropDownList::itemSelected( const string& text )
{
	pCreator_->text( text );

	MachGuiDropDownListBox::itemSelected( text );
}

void MachGuiDropDownListBoxCreator::items( const DropDownListBoxItems& items )
{
	items_ = items;
}

//virtual
bool MachGuiDropDownListBoxCreator::doHandleNavigationKey( NavKey navKey, MachGuiFocusCapableControl** )
{
	bool retValue = false;

	if ( navKey == MachGuiFocusCapableControl::UP_ARROW )
	{
		string previousItem;
		string currentItem;
		bool itemSelected = false;

		// Find item before one selected
		for ( 	GuiStrings::iterator i = strings_.begin();
			 	i != strings_.end() and not itemSelected;
			 	++i )
		{
			previousItem = currentItem;

			currentItem = *i;

			itemSelected = currentItem == text();
		}

		// Didn't find previous item (first item was selected or there isn't any items to select)
		if ( previousItem == "" and strings_.size() >= 1 )
		{
			previousItem = strings_.back();
		}

		// Select the new item
		if ( previousItem != "" )
		{
			text( previousItem );
			retValue = true;
		}
	}
	else if ( navKey == MachGuiFocusCapableControl::DOWN_ARROW )
	{
		string nextItem;
		bool itemSelected = false;

		// Find item after one selected
		GuiStrings::iterator i = strings_.begin();

		for ( 	/*empty*/;
				i != strings_.end() and not itemSelected;
			 	++i )
		{
			itemSelected = (*i) == text();
		}

		// Found selected item, no select one after it...
		if ( itemSelected )
		{
			if ( i != strings_.end() )
			{
				text( *i );
			}
			else
			{
				text( strings_.front() );
			}

			retValue = true;
		}
	}

	return retValue;
}

//virtual
void MachGuiDropDownListBoxCreator::hasFocus( bool newValue )
{
	MachGuiFocusCapableControl::hasFocus( newValue );

	changed();
}

/* End DROPDWNC.CPP *************************************************/
