/*
 * C H C K B O X . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiCheckBox

    Check box class.
*/

#ifndef _MACHGUI_CHCKBOX_HPP
#define _MACHGUI_CHCKBOX_HPP

#include "base/base.hpp"
#include "stdlib/string.hpp"
#include "gui/gui.hpp"
#include "gui/font.hpp"
#include "gui/displaya.hpp"
#include "machgui/focusctl.hpp"

class MachGuiCheckBox : public GuiDisplayable, public MachGuiFocusCapableControl
// Canonical form revoked
{
public:
	// The coord specifying the postion of the box is the top left corner of the box, 
	// not the text label to the right of it.
    MachGuiCheckBox( MachGuiStartupScreens*, GuiDisplayable* pParent, const Gui::Coord& boxTopLeft, const string& label, bool isChecked = false );
    MachGuiCheckBox( MachGuiStartupScreens*, GuiDisplayable* pParent, const Gui::Coord& boxTopLeft, uint idsLabel, bool isChecked = false );
    ~MachGuiCheckBox();

	// Check/uncheck box
	void check( bool isChecked ) { isChecked_ = isChecked; }

	// Determine if checked
	bool checked() const { return isChecked_; }

    void CLASS_INVARIANT;

	virtual bool executeControl();
	virtual void hasFocus( bool );

protected:
	virtual void doDisplay();
	virtual void doHandleMouseClickEvent( const GuiMouseEvent& );
	virtual void doHandleMouseEnterEvent( const GuiMouseEvent& );
	virtual void doHandleMouseExitEvent( const GuiMouseEvent& );

private:
    friend ostream& operator <<( ostream& o, const MachGuiCheckBox& t );

    MachGuiCheckBox( const MachGuiCheckBox& );
    MachGuiCheckBox& operator =( const MachGuiCheckBox& );

	string label_;
	bool isChecked_;
	bool isHighlighted_;
	GuiBmpFont font_;
	GuiBitmap checkBmp_;
	GuiBitmap uncheckBmp_;
	GuiBitmap checkFocusBmp_;
	GuiBitmap uncheckFocusBmp_;
	GuiBitmap checkHighlightBmp_;
	GuiBitmap uncheckHighlightBmp_;
};


#endif

/* End CHCKBOX.HPP **************************************************/
