/*
 * R O O T . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _GUI_ROOT_HPP
#define _GUI_ROOT_HPP

#include "gui/displaya.hpp"

//Forward refs
class GuiMouseEvent;

/* //////////////////////////////////////////////////////////////// */

class GuiRoot : public GuiDisplayable
// cannonical form revoked
{
public:
 	GuiRoot( const Gui::Box& abs );
	// POST( isRoot() );
	
	virtual ~GuiRoot();

	virtual void update() = 0;

    //True if the root is handling right mouse clicks, and they should not
    // be dispatched according to location
    virtual bool doHandleRightClickEvent( const GuiMouseEvent& ) = 0;

	void becomeRoot();
	// POST( &GuiManager::instance().root() == this );

	bool recursivelyHasChild( GuiDisplayable * ) const;
	// POST( iff( pChild != this, result ) );

protected:
 	// inherited from GuiDisplayable...
	// virtual void doDisplay() = 0;
	// virtual bool doHandleEvent( const GuiEvent& ) = 0;

	virtual void doBecomeRoot() = 0;
	virtual void doBecomeNotRoot() = 0;

private:
	GuiRoot( const GuiRoot& );
	bool operator ==( const GuiRoot& ) const;
	GuiRoot& operator =( const GuiRoot& );
};

/* //////////////////////////////////////////////////////////////// */

#endif	// #ifndef _GUI_ROOT_HPP
