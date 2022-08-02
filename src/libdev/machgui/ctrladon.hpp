/*
 * C T R L A D O N . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiControlPanelAddOn

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_CTRLADON_HPP
#define _MACHGUI_CTRLADON_HPP

#include "base/base.hpp"
#include "gui/displaya.hpp"

class MachGuiControlPanelAddOnImpl;
class MachInGameScreen;

class MachGuiControlPanelAddOn : public GuiDisplayable
// Canonical form revoked
{
public:
    MachGuiControlPanelAddOn( GuiDisplayable* pParent, const Gui::Coord& coord, MachInGameScreen* pInGameScreen );
    ~MachGuiControlPanelAddOn();

	virtual void doDisplay();

	void refresh();

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachGuiControlPanelAddOn& t );

    MachGuiControlPanelAddOn( const MachGuiControlPanelAddOn& );
    MachGuiControlPanelAddOn& operator =( const MachGuiControlPanelAddOn& );

	// Data members...
	MachGuiControlPanelAddOnImpl* pImpl_;
};


#endif

/* End CTRLADON.HPP *************************************************/
