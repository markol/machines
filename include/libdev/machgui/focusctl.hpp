/*
 * F O C U S C T L . H P P 
 * (c) Charybdis Limited, 1999. All Rights Reserved
 */

/*
    MachGuiFocusCapableControl

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_FOCUSCTL_HPP
#define _MACHGUI_FOCUSCTL_HPP

#include "base/base.hpp"

class MachGuiStartupScreens;
class MachGuiFocusCapableControlImpl;

class MachGuiFocusCapableControl	
// Canonical form revoked
{
public:
    MachGuiFocusCapableControl( MachGuiStartupScreens* );
    virtual ~MachGuiFocusCapableControl( void );

	enum NavKey { TAB_FOWARD, TAB_BACKWARD, UP_ARROW, DOWN_ARROW, LEFT_ARROW, RIGHT_ARROW };

	virtual bool isDisabled() const;

	// If true is returned then the control has used the NavKey and StartupScreens should do
	// no more processing unless the MachGuiFocusCapableControl** is none NULL in which case
	// this control should now get focus. If false is returned then StartupScreens will do
	// some default navigation, e.g. select the next focus capable control in its list.
	virtual bool doHandleNavigationKey( NavKey, MachGuiFocusCapableControl** );

	// Pressing the enter key will cause the control with focus to have executeControl called.
	virtual bool executeControl();
	// PRE( hasFocus() );
	// PRE( not isDisabled() );

	virtual bool isFocusControl() const;
	virtual void hasFocus( bool );
	bool hasFocusSet() const;

	// Override default tabbing order by using these functions...
	void setTabFowardNavControl( MachGuiFocusCapableControl* );
	void setTabBackwardDownNavControl( MachGuiFocusCapableControl* );
	void setLeftNavControl( MachGuiFocusCapableControl* );
	void setRightNavControl( MachGuiFocusCapableControl* );
	void setUpNavControl( MachGuiFocusCapableControl* );
	void setDownNavControl( MachGuiFocusCapableControl* );

	// When message box is displayed this function is called. It temporarily
	// disables this control
	void msgBoxIsDisplayed( bool newValue );

	// Set this focus control to be the one activated when Escape is pressed
	void escapeControl( bool );
	bool isEscapeControl() const;

	// Set this as the default control. If the isFocusControl does not respond to the
	// Enter key then this control is given a chance.
	void defaultControl( bool );
	bool isDefaultControl() const;
	    
    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MachGuiFocusCapableControl& t );

    MachGuiFocusCapableControl( const MachGuiFocusCapableControl& );
    MachGuiFocusCapableControl& operator =( const MachGuiFocusCapableControl& );

	MachGuiFocusCapableControlImpl* pImpl_;
};


#endif

/* End FOCUSCTL.HPP *************************************************/
