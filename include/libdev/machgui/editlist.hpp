/*
 * E D I T L I S T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiEditBoxListBoxItem

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_EDITLIST_HPP
#define _MACHGUI_EDITLIST_HPP

#include "base/base.hpp"
#include "machgui/sslistit.hpp"

class GuiSingleLineEditBox;

class MachGuiEditBoxListBoxItem : public MachGuiSingleSelectionListBoxItem
// Canonical form revoked
{
public:
    MachGuiEditBoxListBoxItem( MachGuiStartupScreens* pStartupScreens, MachGuiSingleSelectionListBox* pListBox, size_t width, const string& text );
    ~MachGuiEditBoxListBoxItem();

    void CLASS_INVARIANT;

	// Call every frame to get the edit box caret to flash.
	void update();

	// Get the last text that was entered into the edit box.
	string text() const;

	// Set the text displayed in the edit box.
	void text( const string& str );

	// Set max chars that can be entered into edit box. zero = no limit
	size_t maxChars() const;
	void maxChars( size_t );

protected:
	virtual void select();
	virtual void unselect();

	GuiSingleLineEditBox* singleLineEditBox(); 

private:
    friend ostream& operator <<( ostream& o, const MachGuiEditBoxListBoxItem& t );

    MachGuiEditBoxListBoxItem( const MachGuiEditBoxListBoxItem& );
    MachGuiEditBoxListBoxItem& operator =( const MachGuiEditBoxListBoxItem& );

	// Data members...
	GuiSingleLineEditBox* pSingleLineEditBox_;
	string text_;
	size_t maxChars_;
};


#endif

/* End EDITLIST.HPP *************************************************/
