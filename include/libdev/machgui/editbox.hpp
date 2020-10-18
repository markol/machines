/*
 * E D I T B O X . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiSingleLineEditBox

    Constructs a GuiDisplayable that displays text on a single line as it is
	typed in by the user. Note that you must call GuiManager::instance().setCharFocus( this ) in
	order for this EditBox to get character messages ( WM_CHAR ).
*/

#ifndef _MACHGUI_EDITBOX_HPP
#define _MACHGUI_EDITBOX_HPP

#include "base/base.hpp"
#include "gui/editbox.hpp"

class MachGuiStartupScreens;

class MachGuiSingleLineEditBox : public GuiSingleLineEditBox
{
public:
	MachGuiSingleLineEditBox( MachGuiStartupScreens* pStartupScreens, const Gui::Box& box, const GuiBmpFont& font );
	virtual ~MachGuiSingleLineEditBox( void );

	void clearTextOnNextChar( bool );
	bool clearTextOnNextChar() const;

	// Do/Don't allow space characters to be entered at beginning of edit box.
	void ignoreSpaceAtBeginning( bool );

protected:
	virtual void drawBackground( void );
	virtual bool doHandleCharEvent( const GuiCharEvent& e );
	
private:
    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachGuiSingleLineEditBox& t );

    MachGuiSingleLineEditBox( const MachGuiSingleLineEditBox& );
    MachGuiSingleLineEditBox& operator =( const MachGuiSingleLineEditBox& );

	MachGuiStartupScreens* pStartupScreens_;
	bool clearTextOnNextChar_;
	bool ignoreSpaceAtBeginning_;
};


#endif

/* End EDITBOX.HPP **************************************************/
