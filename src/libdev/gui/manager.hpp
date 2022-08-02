/*
 * M A N A G E R . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _GUI_MANAGER_HPP
#define _GUI_MANAGER_HPP

#include "gui/event.hpp"
#include "gui/root.hpp"

//////////////////////////////////////////////////////////////////////
class GuiDisplayable;

template<class T> class ctl_list;

class GuiColourScheme
{
public:

	static const GuiColourScheme& defaultScheme();

	///////////////////////////////

	const Gui::Colour& mainBack() const;
	const Gui::Colour& mainInk() const;

	const Gui::Colour& borderBack() const;
	const Gui::Colour& borderLo() const;
	const Gui::Colour& borderHi() const;

	const Gui::Colour& headingBack() const;
	const Gui::Colour& headingInk() const;

	const Gui::Colour& buttonBack() const;
	const Gui::Colour& buttonInk() const;
	const Gui::Colour& buttonLo() const;
	const Gui::Colour& buttonHi() const;
	const Gui::Colour& buttonDefault() const;

	///////////////////////////////

private:

	GuiColourScheme();

	Gui::Colour mainBack_;
	Gui::Colour mainInk_;
	Gui::Colour borderBack_;
	Gui::Colour borderLo_;
	Gui::Colour borderHi_;

	Gui::Colour headingBack_;
	Gui::Colour headingInk_;

	Gui::Colour buttonBack_;
	Gui::Colour buttonInk_;
	Gui::Colour buttonLo_;
	Gui::Colour buttonHi_;
	Gui::Colour buttonDefault_;
};

//////////////////////////////////////////////////////////////////////

class GuiManagerImpl;

class GuiManager 
// Singleton
{
public:

	static GuiManager& instance();
	~GuiManager();

	const GuiColourScheme& colourScheme() const;

	// Get GuiDisplayables to respond to keyboard and mouse events. As a result of this
	// a GuiDisplayable may indicate that it needs redrawing.
	bool update();

	// Redraw any GuiDisplayables modified during the update phase.
	void display();

	bool hasRoot() const;

	const GuiRoot& root() const;
	// PRE( hasRoot() );

	void changeRoot( GuiRoot* pNewRoot );

	bool keyboardFocusExists() const;

	GuiDisplayable& keyboardFocus();
	// PRE( keyboardFocusExists() );

	void keyboardFocus( GuiDisplayable * pNewKeyboardFocus );
	// PRE( pNewKeyboardFocus != NULL );
	// POST( keyboardFocusExists() );
	// POST( &keyboardFocus() == pNewKeyboardFocus );

	bool charFocusExists() const;

	GuiDisplayable& charFocus();
	// PRE( charFocusExists() );

	void charFocus( GuiDisplayable* pNewCharFocus );
	// PRE( pNewCharFocus != NULL );
	// POST( charFocusExists() );

	void removeCharFocus();
	// POST( not charFocusExists() );

private:
	GuiManager( const GuiManager& );
	bool operator ==( const GuiManager& ) const;
	GuiManager& operator =( const GuiManager& );

    // Called by the framework when pDisplayable is being created
    void isBeingCreated( GuiDisplayable* pDisplayable );
    // Called by the framework when pDisplayable is being destructed
    void isBeingDeleted( GuiDisplayable* pDisplayable );
	
	GuiRoot& root();
	// PRE( hasRoot() );

	GuiManager();

	void processEvents();
	void processMouseEvent( const GuiMouseEvent& );
	void updateMouseFocus();

    static Gui::ScrollState getScrollDirection(DevButtonEvent::Action act);

	enum { N_BUFFERS = 2 };

	// Data members
	GuiManagerImpl*	pImpl_;

	friend class GuiManagerImpl;
	friend class GuiDisplayable;
	friend void GuiRoot::becomeRoot();
};

//////////////////////////////////////////////////////////////////////

#endif	// #ifndef _GUI_MANAGER_HPP

/* End MANAGER.HPP **************************************************/
