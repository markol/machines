/*
 * S S L I S T I T . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "gui/sslistit.hpp"
#include "gui/sslistbx.hpp"
#include "gui/event.hpp"

GuiSingleSelectionListBoxItem::GuiSingleSelectionListBoxItem( GuiSingleSelectionListBox* pListBox, size_t width, size_t height )
:	GuiDisplayable( pListBox, Gui::Box( 0, 0, width, height ) ),
	pListBox_( pListBox ),
	selected_( false )
{

    TEST_INVARIANT;
}

GuiSingleSelectionListBoxItem::~GuiSingleSelectionListBoxItem()
{
    TEST_INVARIANT;

}

void GuiSingleSelectionListBoxItem::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const GuiSingleSelectionListBoxItem& t )
{

    o << "GuiSingleSelectionListBoxItem " << (void*)&t << " start" << std::endl;
    o << "GuiSingleSelectionListBoxItem " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual 
void GuiSingleSelectionListBoxItem::doHandleMouseClickEvent( const GuiMouseEvent& rel )
{
	PRE( pListBox_);

	if ( ( rel.leftButton() == Gui::PRESSED and pListBox_->selectItemWhen() == GuiSingleSelectionListBox::PRESSED ) or
	 	 ( rel.leftButton() == Gui::RELEASED and pListBox_->selectItemWhen() == GuiSingleSelectionListBox::RELEASED ) )
	{
		selectThisItem();
	}
}

void GuiSingleSelectionListBoxItem::selected( bool selected )
{
	selected_ = selected;
}

bool GuiSingleSelectionListBoxItem::selected() const
{
	return selected_;
}

GuiSingleSelectionListBox& GuiSingleSelectionListBoxItem::listBox()
{
	PRE( pListBox_);

	return *pListBox_;
}

void GuiSingleSelectionListBoxItem::selectThisItem()
{
	pListBox_->notifyListItemSelection( this );
}

/* End SSLISTIT.CPP *************************************************/
