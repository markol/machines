/*
 * E D I T B O X . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    GuiSingleLineEditBox

    A brief description of the class should go in here
*/

#ifndef _GUI_EDITBOX_HPP
#define _GUI_EDITBOX_HPP

#include "base/base.hpp"
#include "stdlib/string.hpp"
#include "gui/displaya.hpp"
#include "gui/font.hpp"
#include "gui/gui.hpp"

class GuiSingleLineEditBox : public GuiDisplayable
// Canonical form revoked
{
public:
    GuiSingleLineEditBox( GuiDisplayable* pParent, const Gui::Box& box, const GuiBmpFont& font );
    ~GuiSingleLineEditBox();

	 // Sets the maximum number of characters that can be typed into the edit box.
	 // zero = no limit
	void maxChars( size_t maxChars );

	// Call every frame. Controls the flashing of the caret etc.
	void update();

	void backgroundColour( const GuiColour& colour );
	void caretColour( const GuiColour& colour );

	// Get/Set text that is displayed in edit box
	void text( const string& );
	// PRE( string.length() <= maxChars_ );
	string text() const;

	// Turn edit box border on.
	void border( bool );

	// Select edit box border colour ( default is white ).
	void borderColour( const GuiColour& colour );
		
    void CLASS_INVARIANT;

protected:
	virtual void doDisplay();
	virtual bool doHandleCharEvent( const GuiCharEvent& e );
	virtual bool doHandleKeyEvent( const GuiKeyEvent& e );
	virtual void doHandleMouseClickEvent( const GuiMouseEvent& e );

	void leftArrowEvent();
	void rightArrowEvent();
	void homeEvent();
	void endEvent();
	void backspaceEvent();
	void deleteEvent();

	void forceRedraw();

	virtual void drawBackground();

	const string& leftText() const;
	const string& rightText() const;
	Gui::XCoord caretPos() const;
	bool showCaret() const;
	const GuiColour& caretColour() const;

	virtual int maxWidth() const;

private:
    friend ostream& operator <<( ostream& o, const GuiSingleLineEditBox& t );

    GuiSingleLineEditBox( const GuiSingleLineEditBox& );
    GuiSingleLineEditBox& operator =( const GuiSingleLineEditBox& );

	// Data members...
	string rightText_;
	string leftText_;
	Gui::XCoord caretPos_;
	GuiBmpFont font_;
	bool showCaret_;
	size_t maxChars_;
	GuiColour backgroundColour_;
	GuiColour caretColour_;
	bool dontUpdate_;
	bool border_;
	GuiColour borderColour_;
};


#endif

/* End EDITBOX.HPP **************************************************/
