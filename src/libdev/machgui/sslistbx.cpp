/*
 * S S L I S T B X . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/sslistbx.hpp"
#include "machgui/sslistit.hpp"
#include "machgui/startup.hpp"
#include "gui/painter.hpp"
#include <algorithm>

MachGuiSingleSelectionListBox::MachGuiSingleSelectionListBox(	MachGuiStartupScreens* pParent, const Gui::Box& box,
    															size_t horizontalSpacing, size_t verticalSpacing,
    															size_t scrollInc)
:	GuiSingleSelectionListBox( pParent, box, horizontalSpacing, verticalSpacing, scrollInc ),
	MachGuiFocusCapableControl( pParent ),
	pStartupScreens_( pParent )
{

    TEST_INVARIANT;
}

MachGuiSingleSelectionListBox::~MachGuiSingleSelectionListBox()
{
    TEST_INVARIANT;

	deleteAllChildren();
}

void MachGuiSingleSelectionListBox::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachGuiSingleSelectionListBox& t )
{

    o << "MachGuiSingleSelectionListBox " << (void*)&t << " start" << std::endl;
    o << "MachGuiSingleSelectionListBox " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
void MachGuiSingleSelectionListBox::doDisplay()
{
	// Blit background to list box
	pStartupScreens_->blitBackdrop( absoluteBoundary(),
									absoluteBoundary().minCorner() );
}

void MachGuiSingleSelectionListBox::addListItem( MachGuiSingleSelectionListBoxItem* pItem )
{
	listItems_.push_back( pItem );
}

void MachGuiSingleSelectionListBox::removeListItem( MachGuiSingleSelectionListBoxItem* pItem )
{
	ctl_pvector< MachGuiSingleSelectionListBoxItem >::iterator i = find( listItems_.begin(), listItems_.end(), pItem );

	ASSERT( i != listItems_.end(), "Trying to remove pItem which does not exist" );

	listItems_.erase( i );
}

//virtual
void MachGuiSingleSelectionListBox::hasFocus( bool newValue )
{
	MachGuiFocusCapableControl::hasFocus( newValue );

	changed();

	// If no items are selected in list then select first one
	if ( isFocusControl() )
	{
		bool itemSelected = false;

		for ( 	ctl_pvector< MachGuiSingleSelectionListBoxItem >::iterator i = listItems_.begin();
			 	i != listItems_.end() and not itemSelected;
			 	++i )
		{
			itemSelected = (*i)->selected();
		}

		// Select first item
		if ( not itemSelected and listItems_.size() >= 1 )
		{
			listItems_.front()->selectThisItem();
		}
	}
}

//virtual
bool MachGuiSingleSelectionListBox::doHandleNavigationKey( MachGuiFocusCapableControl::NavKey navKey, MachGuiFocusCapableControl** )
{
	bool retValue = false;

	if ( navKey == MachGuiFocusCapableControl::UP_ARROW )
	{
		MachGuiSingleSelectionListBoxItem* pPreviousItem = NULL;
		MachGuiSingleSelectionListBoxItem* pCurrentItem = NULL;
		bool itemSelected = false;

		// Find item before one selected
		for ( 	ctl_pvector< MachGuiSingleSelectionListBoxItem >::iterator i = listItems_.begin();
			 	i != listItems_.end() and not itemSelected;
			 	++i )
		{
			pPreviousItem = pCurrentItem;

			pCurrentItem = *i;

			itemSelected = pCurrentItem->selected();
		}

		// Didn't find previous item (first item was selected or there isn't any items to select)
		if ( not pPreviousItem and listItems_.size() >= 1 )
		{
			pPreviousItem = listItems_.back();
		}

		// Select the new item
		if ( pPreviousItem )
		{
			doNavSelectNewItem( pPreviousItem );
			retValue = true;
		}
	}
	else if ( navKey == MachGuiFocusCapableControl::DOWN_ARROW )
	{
		MachGuiSingleSelectionListBoxItem* pNextItem = NULL;
		bool itemSelected = false;

		// Find item after one selected
		ctl_pvector< MachGuiSingleSelectionListBoxItem >::iterator i = listItems_.begin();

		for ( 	/*empty*/;
				i != listItems_.end() and not itemSelected;
			 	++i )
		{
			itemSelected = (*i)->selected();
		}

		// Found selected item, no select one after it...
		if ( itemSelected )
		{
			if ( i != listItems_.end() )
			{
				doNavSelectNewItem( (*i) );
			}
			else
			{
				doNavSelectNewItem( listItems_.front() );
			}

			retValue = true;
		}
	}

	return retValue;
}

//virtual
bool MachGuiSingleSelectionListBox::isDisabled() const
{
	return MachGuiFocusCapableControl::isDisabled() or ( listItems_.size() == 0 );
}

ctl_pvector< MachGuiSingleSelectionListBoxItem >& MachGuiSingleSelectionListBox::listItems()
{
	return listItems_;
}

//virtual
void MachGuiSingleSelectionListBox::doNavSelectNewItem( MachGuiSingleSelectionListBoxItem* pItem )
{
	pItem->selectThisItem();
}

/* End SSLISTBX.CPP *************************************************/
