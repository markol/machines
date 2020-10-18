/*
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachHWResearchIcons

    The icon sequence displayed in the Hardware research menu depicting the
    available research items.
*/

#ifndef _MACHGUI_HWREICNS_HPP
#define _MACHGUI_HWREICNS_HPP

#include "base/base.hpp"
#include "gui/scrolist.hpp"
#include "gui/gui.hpp"

#include "machlog/mlnotif.hpp"

//Forward refs
class MachInGameScreen;
class MachHWResearchBank;
class MachLogHardwareLab;
class MachLogResearchTree;

class MachHWResearchIcons : public GuiSimpleScrollableList, public MachLogNotifiable
//orthodox canonical (revoked)
{
public:
    //ctor. Located in pParent, covering area.
    //In game screen passed.
    MachHWResearchIcons( GuiDisplayable* pParent, const Gui::Coord& rel,
                         MachHWResearchBank* pHWResearchBank, 
                         MachLogHardwareLab* pHardwareLab, 
                         MachInGameScreen* pInGameScreen );
    virtual ~MachHWResearchIcons();

	virtual void notifiableBeNotified();
	
    void CLASS_INVARIANT;

	static size_t reqWidth();
	static size_t reqHeight( MachInGameScreen*, const Gui::Coord& );

protected:	
	virtual void doDisplay();

private:
    // Operations deliberately revoked
    MachHWResearchIcons( const MachHWResearchIcons& );
    MachHWResearchIcons& operator =( const MachHWResearchIcons& );
    bool operator ==( const MachHWResearchIcons& );

    friend ostream& operator <<( ostream& o, const MachHWResearchIcons& t );

    //Set up the list of icons
    void addIcons( MachLogHardwareLab* pHardwareLab, MachInGameScreen* pInGameScreen,
                   MachHWResearchBank* pHWResearchBank );

	// Data members...
	MachHWResearchBank* pHWResearchBank_;
    MachLogHardwareLab* pHardwareLab_;
	MachInGameScreen* pInGameScreen_;
	
	MachLogResearchTree& researchTree_;
};


#endif

/* End  *************************************************/
