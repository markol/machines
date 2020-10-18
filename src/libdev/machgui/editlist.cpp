/*
 * E D I T L I S T . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/editlist.hpp"
#include "machgui/sslistbx.hpp"
#include "machgui/gui.hpp"
#include "gui/editbox.hpp"
#include "gui/manager.hpp"
#include "gui/painter.hpp"
#include "machgui/internal/mgsndman.hpp"

class MachGuiEditBoxItem : public GuiSingleLineEditBox
{
public:
	MachGuiEditBoxItem( GuiDisplayable* pParent, const Gui::Box& box, const GuiBmpFont& font, MachGuiSingleSelectionListBox* pListBox )
	:	GuiSingleLineEditBox( pParent, box, font ),
		pListBox_( pListBox )
	{
		changed();
	}

	virtual void drawBackground()
	{
		if ( pListBox_ and pListBox_->isFocusControl() )
		{
			GuiPainter::instance().blit( MachGui::longYellowGlowBmp(), Gui::Box(0,0,width(),height()), absoluteBoundary().minCorner() );
		}
		else
		{
			GuiPainter::instance().blit( MachGui::longGlowBmp(), Gui::Box(0,0,width(),height()), absoluteBoundary().minCorner() );
		}
	}

private:
	MachGuiSingleSelectionListBox* pListBox_;
};

MachGuiEditBoxListBoxItem::MachGuiEditBoxListBoxItem( MachGuiStartupScreens* pStartupScreens, MachGuiSingleSelectionListBox* pListBox, size_t width, const string& text )
:	MachGuiSingleSelectionListBoxItem( pStartupScreens, pListBox, width, text ),
	pSingleLineEditBox_( NULL ),
	maxChars_( 0 ) 
{

    TEST_INVARIANT;
}

MachGuiEditBoxListBoxItem::~MachGuiEditBoxListBoxItem()
{
    TEST_INVARIANT;

	_DELETE( pSingleLineEditBox_ );
	GuiManager::instance().removeCharFocus();
}

void MachGuiEditBoxListBoxItem::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiEditBoxListBoxItem& t )
{

    o << "MachGuiEditBoxListBoxItem " << (void*)&t << " start" << std::endl;
    o << "MachGuiEditBoxListBoxItem " << (void*)&t << " end" << std::endl;

    return o;
}

void MachGuiEditBoxListBoxItem::update()
{
	if ( pSingleLineEditBox_ )
	{
		pSingleLineEditBox_->update();
	}
}

//virtual 
void MachGuiEditBoxListBoxItem::select()
{
	// Play select sound
	MachGuiSoundManager::instance().playSound( "gui/sounds/listclik.wav" );

	pSingleLineEditBox_ = _NEW( MachGuiEditBoxItem( this, Gui::Box(1,1,width() - 1,height() - 1), getUnderlineFont(), myListBox() ) );
	pSingleLineEditBox_->caretColour( Gui::BLACK() );
	pSingleLineEditBox_->maxChars( maxChars_ );
	pSingleLineEditBox_->text( text_ );
	GuiManager::instance().charFocus( pSingleLineEditBox_ );
}

//virtual 
void MachGuiEditBoxListBoxItem::unselect()
{
	// Update text_ data member
	text();

	_DELETE( pSingleLineEditBox_ );
	pSingleLineEditBox_ = NULL;
	GuiManager::instance().removeCharFocus();
	changed();
}

GuiSingleLineEditBox* MachGuiEditBoxListBoxItem::singleLineEditBox()
{
	return pSingleLineEditBox_;
}

string MachGuiEditBoxListBoxItem::text() const
{
	MachGuiEditBoxListBoxItem* pThis = _CONST_CAST( MachGuiEditBoxListBoxItem*, this );
	/// Update text_ data member if we are currently in edit mode
	if ( pThis->singleLineEditBox() )
	{
		pThis->text_ = pThis->singleLineEditBox()->text();
	}

	return text_;
}

void MachGuiEditBoxListBoxItem::text( const string& str )
{
	if ( singleLineEditBox() )
	{
		singleLineEditBox()->text( str );
	}

	text_ = str;
}

size_t MachGuiEditBoxListBoxItem::maxChars() const
{
	return maxChars_;
}

void MachGuiEditBoxListBoxItem::maxChars( size_t newMaxChars )
{
	maxChars_ = newMaxChars;
	if ( singleLineEditBox() )
	{
		singleLineEditBox()->maxChars( maxChars_ );
	}
}


/* End EDITLIST.CPP *************************************************/
