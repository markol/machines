/*
 * E V E N T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include "gui/event.hpp"

/* //////////////////////////////////////////////////////////////// */

GuiEventBase::GuiEventBase( ModifierKeys modifierKeys )
: modifierKeys_( modifierKeys )
{
	// Intentionally Empty
}

bool GuiEventBase::isAltPressed() const
{
	return modifierKeys_ & ALT;
}

bool GuiEventBase::isCtrlPressed() const
{
	return modifierKeys_ & CTRL;
}

bool GuiEventBase::isShiftPressed() const
{
	return modifierKeys_ & SHIFT;
}

GuiEventBase::ModifierKeys 
GuiEventBase::modifierKeys() const
{
	return modifierKeys_;
}

/* //////////////////////////////////////////////////////////////// */

GuiMouseEvent::GuiMouseEvent(  const Gui::Coord& at,
						Gui::ButtonState left, 
						Gui::ButtonState right,
						ModifierKeys keys )
: GuiEventBase( keys ),
  coord_( at ),
  leftButton_( left ),
  rightButton_( right )
{
	// Intentionally Empty
}

Gui::ButtonState 
GuiMouseEvent::leftButton() const
{
	return leftButton_;
}

Gui::ButtonState 
GuiMouseEvent::rightButton() const
{
	return rightButton_;
}
	
const Gui::Coord& 
GuiMouseEvent::coord() const
{
	return coord_;
}

void GuiMouseEvent::translate( const Gui::Vec& v )
{
	POST_DATA( Gui::Coord oldCoord = coord() );
	coord_ += v;
	POST( coord().x() == oldCoord.x() + v.x() );
	POST( coord().y() == oldCoord.y() + v.y() );
}

/* //////////////////////////////////////////////////////////////// */

GuiKeyEvent::GuiKeyEvent( const GuiKey& k, Gui::ButtonState s, ModifierKeys keys, const DevButtonEvent& buttonEvent )
: GuiEventBase( keys ),
  key_( k ),
  state_( s ),
  buttonEvent_( buttonEvent )
{
	// Intentionally Empty
}

const GuiKey& 
GuiKeyEvent::key() const
{
	return key_;
}

Gui::ButtonState 
GuiKeyEvent::state() const
{
	return state_;
}

 const DevButtonEvent& GuiKeyEvent::buttonEvent() const
 {
	return buttonEvent_;
 }


/* End EVENT.CPP ****************************************************/
