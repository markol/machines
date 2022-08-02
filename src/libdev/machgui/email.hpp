/*
 * E M A I L . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _MACH_EMAIL_HPP
#define _MACH_EMAIL_HPP

#include "gui/gui.hpp"
#include "gui/icon.hpp"
#include "gui/root.hpp"
#include "gui/displaya.hpp"
#include "machgui/gui.hpp"

class MachInGameScreen;

class MachEmailWindow : public GuiDisplayable
{
public:

	virtual ~MachEmailWindow() {};

protected:
	// inherited from GuiDisplayable...
	virtual void doDisplay() {};
};

class MachMenuBar : public GuiDisplayable
{
public:

	MachMenuBar( MachInGameScreen * pParent, const Gui::Coord& rel );

	virtual ~MachMenuBar() {};

protected:
	// inherited from GuiDisplayable...
	virtual void doDisplay() {};
};

class MachEmailArrivalBar : public GuiDisplayable
{
public:

	MachEmailArrivalBar( MachInGameScreen * pParent, 
							const Gui::Coord& rel );

	virtual ~MachEmailArrivalBar() {};

protected:
	// inherited from GuiDisplayable...
	virtual void doDisplay() {};
};

/* //////////////////////////////////////////////////////////////// */


/* //////////////////////////////////////////////////////////////// */

#endif // #ifndef _MACH_INGAME_HPP

/* End EMAIL.HPP ***************************************************/