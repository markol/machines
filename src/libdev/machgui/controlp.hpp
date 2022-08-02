/*
 * C O N T R O L P . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiControlPanel

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_CONTROLP_HPP
#define _MACHGUI_CONTROLP_HPP

#include "base/base.hpp"
#include "gui/displaya.hpp"

class MachGuiControlPanelImpl;
class MachInGameScreen;

class MachGuiControlPanel : public GuiDisplayable
// Canonical form revoked
{
public:
    MachGuiControlPanel( GuiDisplayable* pParent, const Gui::Box&, MachInGameScreen* );
    ~MachGuiControlPanel();

    void CLASS_INVARIANT;

	virtual void doDisplay();

	// Tell control panel to redraw part of it's background that
	// was obscured by the displayable passed in.
	void redrawArea( const GuiDisplayable& );

	// Tell control panel to redraw part of it's background 
	// tile ( as described by 'area' ) for the next two frames.
	void redrawArea( const Gui::Box& area );

	// Tell control panel to redraw part of it's background 
	// tile ( as described by 'area' ). The redraw is done immediately
	// and should really only be called from within a GuiDisplayables 'doDisplay'
	// routine.
	void redrawAreaImmediate( const Gui::Box& area );

	// Tell control panel to redraw part of it's background that
	// was obscured by the displayable passed in. The redraw is done immediately
	// and should really only be called from within a GuiDisplayables 'doDisplay'
	// routine.
	void redrawAreaImmediate( const GuiDisplayable& );

	// Redraw the background to the control panel.
	void redrawAllAreas();

	// Get height of control panel that is visible on screen. Control panel may
	// extend below the bottom of the screen.
	int getVisibleHeight() const;

	// Setup coords for decals. Should be called after screen resolution change.
	void setupDecalCoords();

	// Setup bitmaps used for decals. Should be called player race has changed.
	void setupDecalBitmaps();

	// Reposition navigators after a resolution change
	void repositionNavigators( GuiDisplayable* pMachineNav, GuiDisplayable* pConstructionNav, GuiDisplayable* pSquadNav );

private:
    friend ostream& operator <<( ostream& o, const MachGuiControlPanel& t );

    MachGuiControlPanel( const MachGuiControlPanel& );
    MachGuiControlPanel& operator =( const MachGuiControlPanel& );

	string getDecalRootDirectory() const;

	// Data members
	MachGuiControlPanelImpl* pImpl_;
};


#endif

/* End CONTROLP.HPP *************************************************/
