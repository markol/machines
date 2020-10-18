/*
 * S S L I S T B X . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    GuiSingleSelectionListBox

    A brief description of the class should go in here
*/

#ifndef _GUI_SSLISTBX_HPP
#define _GUI_SSLISTBX_HPP

#include "base/base.hpp"
#include "gui/scrolist.hpp"

class GuiSingleSelectionListBoxItem;

class GuiSingleSelectionListBox	: public GuiSimpleScrollableList
// Canonical form revoked
{
public:
    GuiSingleSelectionListBox( 	GuiDisplayable* pParent, const Gui::Box&, 
    							size_t horizontalSpacing, size_t verticalSpacing,
    							size_t scrollInc );
    ~GuiSingleSelectionListBox( void );

    void CLASS_INVARIANT;

	enum SelectItemWhen { PRESSED, RELEASED };
	void selectItemWhen( SelectItemWhen );
	SelectItemWhen selectItemWhen() const;

	void deleteAllItems();

protected:
	friend class GuiSingleSelectionListBoxItem;
	
	void notifyListItemSelection( GuiSingleSelectionListBoxItem* pNewSelection ); 

private:
    friend ostream& operator <<( ostream& o, const GuiSingleSelectionListBox& t );

    GuiSingleSelectionListBox( const GuiSingleSelectionListBox& );
    GuiSingleSelectionListBox& operator =( const GuiSingleSelectionListBox& );

	GuiSingleSelectionListBoxItem* pCurrentSelection_;
	SelectItemWhen selectItemWhen_;
};


#endif

/* End SSLISTBX.HPP *************************************************/
