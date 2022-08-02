/*
 * M O U S E . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#ifndef _GUI_MOUSE_HPP
#define _GUI_MOUSE_HPP

#include "gui/gui.hpp"

/* //////////////////////////////////////////////////////////////// */

class GuiMouse
// singleton
{
public:

	static GuiMouse& instance();

	~GuiMouse();

	void update( const Gui::Coord& mouseCoord );

	void cursor( const GuiBitmap& newCursor );

	static const GuiBitmap& defaultCursor();

private:
	GuiMouse( const GuiMouse& );
	bool operator ==( const GuiMouse& ) const;
	GuiMouse& operator =( const GuiMouse& );

	GuiMouse();

	void pickUpBackground( unsigned frame );
	void putDownBackground( unsigned frame );

	DevMouse::Position	position_[ 2 ];
	int					frame_;
};

/* //////////////////////////////////////////////////////////////// */

#endif	// #ifndef _GUI_MOUSE_HPP

/* End MOUSE.HPP ****************************************************/
