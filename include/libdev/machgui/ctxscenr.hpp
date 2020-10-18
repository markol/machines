/*
 * C T X S C E N R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachGuiCtxScenario

    A brief description of the class should go in here
*/

#ifndef _MACHGUI_CTXSCENR_HPP
#define _MACHGUI_CTXSCENR_HPP

#include "base/base.hpp"
#include "machgui/startctx.hpp"
#include "machgui/animatio.hpp"

class MachGuiSingleSelectionListBox;
class MachGuiDbSystem;
class MachGuiDbPlanet;
class MachGuiDbScenario;  
class MachGuiScrollableText;
class GuiImage;

class MachGuiCtxScenario : public MachGuiStartupScreenContext
// Canonical form revoked
{
public:
    MachGuiCtxScenario( MachGuiStartupScreens* pStartupScreens );
    ~MachGuiCtxScenario( void );

    void CLASS_INVARIANT;

	virtual void update();

	void updatePlanetList( MachGuiDbSystem& );
	void updateScenarioList( MachGuiDbPlanet& );
	void updateSelectedScenario( MachGuiDbScenario& );

	void updateDisplayedInfo( const string& text, SysPathName animation );

	virtual bool okayToSwitchContext();

private:
    friend ostream& operator <<( ostream& o, const MachGuiCtxScenario& t );

    MachGuiCtxScenario( const MachGuiCtxScenario& );
    MachGuiCtxScenario& operator =( const MachGuiCtxScenario& );

	void updateSystemList();

	void workOutWhichScenarioToDefaultTo();
	
	// Data members...
	MachGuiAnimations animations_;
	MachGuiSingleSelectionListBox* pSystemList_;
	MachGuiSingleSelectionListBox* pPlanetList_;
	MachGuiSingleSelectionListBox* pScenarioList_;
	MachGuiScrollableText* pTextInfo_;
	GuiImage* pImage_;
	MachGuiDbScenario* pDefaultScenario_;
};


#endif

/* End CTXSCENR.HPP *************************************************/
