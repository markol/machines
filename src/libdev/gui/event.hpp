/*
 * E V E N T . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _GUI_EVENT_HPP
#define _GUI_EVENT_HPP

#include "device/butevent.hpp"
#include "device/keyboard.hpp"
#include "device/mouse.hpp"
#include "gui/gui.hpp"

//class DevButtonEvent;

// GuiCharEvent. We may want to add a separate class at some point.
typedef DevButtonEvent GuiCharEvent;

/* //////////////////////////////////////////////////////////////// */

class GuiEventBase
// memberwise cannonical
{
public:

    ///////////////////////////////

    GuiEventBase();
    // POST( isMouseEvent() );
    // POST( not isAltPressed() );
    // POST( not isCtrlPressed() );
    // POST( not isShiftPressed() );

    enum ModiferKey
    {
        ALT        = 0x0001,
        CTRL    = 0x0002,
        SHIFT    = 0x0004
    } ;

    typedef int ModifierKeys;

    GuiEventBase( ModifierKeys );

    ///////////////////////////////

    bool isAltPressed() const;
    bool isCtrlPressed() const;
    bool isShiftPressed() const;

    ///////////////////////////////

protected:

    ModifierKeys modifierKeys() const;

private:

    ModifierKeys    modifierKeys_;
} ;

/* //////////////////////////////////////////////////////////////// */

class GuiMouseEvent
: private GuiEventBase
// memberwise cannonical
{
public:

    GuiMouseEvent(  const Gui::Coord& at,
        Gui::ButtonState left,
        Gui::ButtonState right,
        ModifierKeys );

    GuiMouseEvent(  const Gui::Coord& at,
        Gui::ButtonState left,
        Gui::ButtonState right,
        Gui::ScrollState scroll,
        ModifierKeys );

    ///////////////////////////////

    Gui::ButtonState leftButton() const;
    Gui::ButtonState rightButton() const;
    Gui::ScrollState scrollDirection() const;

    // absolute coord
    const Gui::Coord& coord() const;

    ///////////////////////////////

    void translate( const Gui::Vec& );
    // POST( coord() == old( coord() ) + rel );

    ///////////////////////////////

    using GuiEventBase::isAltPressed;
    using GuiEventBase::isCtrlPressed;
    using GuiEventBase::isShiftPressed;

    ///////////////////////////////

private:

    Gui::Coord            coord_;
    Gui::ButtonState    leftButton_;
    Gui::ButtonState    rightButton_;
    Gui::ScrollState    scroll_;

    friend bool operator <( const GuiMouseEvent& a, const GuiMouseEvent& b );
    friend bool operator ==( const GuiMouseEvent& a, const GuiMouseEvent& b );
} ;

/* //////////////////////////////////////////////////////////////// */

typedef int GuiKey;

class GuiKeyEvent
: private GuiEventBase
// memberwise cannonical
{
public:

    GuiKeyEvent( const GuiKey& key, Gui::ButtonState state, ModifierKeys, const DevButtonEvent& );

    ///////////////////////////////

    const GuiKey& key() const;
    Gui::ButtonState state() const;
    const DevButtonEvent& buttonEvent() const;

    ///////////////////////////////

    using GuiEventBase::isAltPressed;
    using GuiEventBase::isCtrlPressed;
    using GuiEventBase::isShiftPressed;

    ///////////////////////////////

private:

    GuiKey                    key_;
    Gui::ButtonState        state_;
    const DevButtonEvent&     buttonEvent_;

    friend bool operator  <( const GuiKeyEvent& a, const GuiKeyEvent& b );
    friend bool operator ==( const GuiKeyEvent& a, const GuiKeyEvent& b );
} ;

/* //////////////////////////////////////////////////////////////// */

#endif // #ifndef _GUI_EVENT_HPP
