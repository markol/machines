/*
 * M E N U B U T N . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiMenuButton

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_MENUBUTN_HPP
#define _MACHGUI_MENUBUTN_HPP

#include "base/base.hpp"
#include "gui/displaya.hpp"
#include "machgui/startup.hpp"
#include "machgui/focusctl.hpp"

class MachGuiMenuButton	: public GuiDisplayable, public MachGuiFocusCapableControl
// Canonical form revoked
{
public:
    MachGuiMenuButton( MachGuiStartupScreens* pParent, const Gui::Box& box, unsigned int stringId, MachGuiStartupScreens::ButtonEvent );
    MachGuiMenuButton( MachGuiStartupScreens*, const Gui::Box& box, unsigned int stringId, MachGuiStartupScreens::ButtonEvent, GuiDisplayable* pParent );
    ~MachGuiMenuButton();

    void CLASS_INVARIANT;

	// Disable/Enable the button.
	void disabled( bool );

	bool highlighted() const;
	uint stringId() const;

	// Inherited from MachGuiFocusCapableControl...
	virtual bool isDisabled() const;

	virtual bool executeControl();
	// PRE( hasFocus() );
	// PRE( not isDisabled() );

	virtual void hasFocus( bool );

protected:
	virtual void doHandleMouseClickEvent( const GuiMouseEvent& rel );
	virtual void doHandleMouseExitEvent( const GuiMouseEvent& rel );
	virtual void doHandleMouseEnterEvent( const GuiMouseEvent& rel );
	virtual void doDisplay();

private:
    friend ostream& operator <<( ostream& o, const MachGuiMenuButton& t );

    MachGuiMenuButton( const MachGuiMenuButton& );
    MachGuiMenuButton& operator =( const MachGuiMenuButton& );

	MachGuiStartupScreens* pStartupScreens_;
	unsigned int stringId_;
	bool highlighted_;
	bool flash_;
	bool disabled_;
	MachGuiStartupScreens::ButtonEvent buttonEvent_;
};

#endif

/* End MENUBUTN.HPP *************************************************/
