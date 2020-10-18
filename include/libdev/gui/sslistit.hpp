/*
 * S S L I S T I T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    GuiSingleSelectionListBoxItem

    A brief description of the class should go in here
*/

#ifndef _GUI_SSLISTIT_HPP
#define _GUI_SSLISTIT_HPP

#include "base/base.hpp"
#include "gui/displaya.hpp"

class GuiSingleSelectionListBox;

class GuiSingleSelectionListBoxItem : public GuiDisplayable
// Canonical form revoked
{
public:
    GuiSingleSelectionListBoxItem( GuiSingleSelectionListBox* pListBox, size_t width, size_t height );
    ~GuiSingleSelectionListBoxItem( void );

	// True if this is the selected item in the list box.
	bool selected() const;

	// Notifies list box of selection change. Calling this will indirectly cause
	// "select" to get called. This is the only way to select a list box item
	// other than using the mouse pointer.
	void selectThisItem();

    void CLASS_INVARIANT;

protected:
	// Called when the list box item has been unselected. The selected status is set
	// before this function is called.
	virtual void unselect() = 0;
	// PRE( selected() == false );

	// Called when the list box item has been selected. The selected status is set
	// before this function is called.
	virtual void select() = 0;
	// PRE( selected() == true );

	virtual void doHandleMouseClickEvent( const GuiMouseEvent& rel );

	// Get the list box that this list item is associated with.
	GuiSingleSelectionListBox& listBox();
	// PRE( pListBox_);

private:
	friend ostream& operator <<( ostream& o, const GuiSingleSelectionListBoxItem& t );

    GuiSingleSelectionListBoxItem( const GuiSingleSelectionListBoxItem& );
    GuiSingleSelectionListBoxItem& operator =( const GuiSingleSelectionListBoxItem& ); 
	
	friend class GuiSingleSelectionListBox;

	void selected( bool );

	bool selected_;
	GuiSingleSelectionListBox* pListBox_;
};


#endif

/* End SSLISTIT.HPP *************************************************/
