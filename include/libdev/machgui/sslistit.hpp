/*
 * S S L I S T I T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiSingleSelectionListBoxItem

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_SSLISTIT_HPP
#define _MACHGUI_SSLISTIT_HPP

#include "base/base.hpp"
#include "stdlib/string.hpp"
#include "gui/sslistit.hpp"

class GuiBmpFont;
class MachGuiStartupScreens;
class MachGuiSingleSelectionListBox;

class MachGuiSingleSelectionListBoxItem	: public GuiSingleSelectionListBoxItem
// Canonical form revoked
{
public:
    MachGuiSingleSelectionListBoxItem( MachGuiStartupScreens*, MachGuiSingleSelectionListBox* pListBox, size_t width, const string& text );
    MachGuiSingleSelectionListBoxItem( MachGuiStartupScreens*, GuiSingleSelectionListBox* pListBox, size_t width, const string& text );
    ~MachGuiSingleSelectionListBoxItem( void );

    void CLASS_INVARIANT;

	static size_t reqHeight();

protected:
	virtual void select();
	virtual void unselect();

	static GuiBmpFont getFont();
	static GuiBmpFont getUnderlineFont();
	static GuiBmpFont getHighlightFont();

	virtual void doDisplay();

	bool highlighted() const;
	const string& text() const;
	MachGuiStartupScreens* startupScreens();
	MachGuiSingleSelectionListBox* myListBox();

	virtual void doHandleMouseEnterEvent( const GuiMouseEvent& rel );
	virtual void doHandleMouseExitEvent( const GuiMouseEvent& rel );

private:
    friend ostream& operator <<( ostream& o, const MachGuiSingleSelectionListBoxItem& t );

    MachGuiSingleSelectionListBoxItem( const MachGuiSingleSelectionListBoxItem& );
    MachGuiSingleSelectionListBoxItem& operator =( const MachGuiSingleSelectionListBoxItem& );

	string text_;
	bool highlighted_;
	MachGuiStartupScreens* pStartupScreens_;
	MachGuiSingleSelectionListBox* pMyListBox_;
};


#endif

/* End SSLISTIT.HPP *************************************************/
