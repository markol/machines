/*
 * P R O M T E X T . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPromptText

    Implements the prompt text displayed in the prompt text area of the in-game screen.
*/

#ifndef _MACHGUI_PROMTEXT_HPP
#define _MACHGUI_PROMTEXT_HPP

#include "base/base.hpp"
#include "stdlib/string.hpp"
#include "gui/displaya.hpp"
#include "gui/gui.hpp"
#include "gui/editbox.hpp"

class MachCameras;
class MachPromptTextImpl;
class MachInGameScreen;

//orthodox canonical (revoked)
class MachPromptText : public GuiSingleLineEditBox
{
public:
    //ctor. Owner is pParent. Area covered is bounded by relativeBoundary.
    MachPromptText( MachInGameScreen* pParent, const Gui::Boundary& relativeBoundary, MachCameras*, GuiDisplayable* pPassEventsTo );

    //dtor.
    ~MachPromptText();

    //Set/get the prompt text related to cursor position
    void cursorPromptText( const string& prompt );
	void cursorPromptText( const string& prompt, bool restartScroll );
    const string& cursorPromptText() const;

    //Clear any prompt text associated with the cursor position
    void clearCursorPromptText();

    //Set/get the prompt text related to current command state
    void commandPromptText( const string& prompt );
    const string& commandPromptText() const;

    //Clear any prompt text associated with the current command state
    void clearCommandPromptText();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPromptText& t );

	enum PromptDisplayed { CURSORTEXT, COMMANDTEXT, NOTEXT }; 

	virtual bool doHandleKeyEvent( const GuiKeyEvent& event );

protected:

    //Inherited from GuiDisplayable...
 	virtual void doDisplay();

	//virtual void doHandleContainsMouseEvent( const GuiMouseEvent& rel );
	//virtual void doHandleMouseClickEvent( const GuiMouseEvent& rel );
	//virtual void doHandleMouseEnterEvent( const GuiMouseEvent& rel );
	//virtual void doHandleMouseExitEvent( const GuiMouseEvent& rel );

	virtual bool doHandleCharEvent( const GuiCharEvent& event );

	void displayChatMessage();

	virtual int maxWidth() const;

	// Pass mouse position in as relative to this GuiDisplayable. Updates
	// mouse position to be relative to pPassEventsTo_.
	//bool passEventTo( GuiMouseEvent* );

	virtual bool processesMouseEvents() const;

private:
    // Operations deliberately revoked
    MachPromptText( const MachPromptText& );
    MachPromptText& operator =( const MachPromptText& );
    bool operator ==( const MachPromptText& );

    //Data members...
	MachPromptTextImpl* pImpl_;
};


#endif

/* End PROMTEXT.HPP *************************************************/
