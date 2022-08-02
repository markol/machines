/*
 * R O O T . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _GUITEST_ROOT_HPP
#define _GUITEST_ROOT_HPP

#include "gui/root.hpp"

/* //////////////////////////////////////////////////////////////// */

class GuiTestRoot
: public GuiRoot
// cannonical form revoked
{
public:
	
	GuiTestRoot( const Gui::Box& abs );

	virtual ~GuiTestRoot();

protected:

	// inherited from GuiRoot...
	virtual void doDisplay();

	virtual void doBecomeRoot();
	virtual void doBecomeNotRoot();

private:
	GuiTestRoot( const GuiTestRoot& );
	bool operator ==( const GuiTestRoot& ) const;
	GuiTestRoot& operator =( const GuiTestRoot& );
};

/* //////////////////////////////////////////////////////////////// */

#endif	// #ifndef _GUITEST_ROOT_HPP
