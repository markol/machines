/*
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachHWResearchMenu

    Parent of the hw research menu items. This menu is displayed during interaction
    of the research command for a hw lab, allowing editing of the lab's production queue.
*/

#ifndef _MACHGUI_HWREMENU_HPP
#define _MACHGUI_HWREMENU_HPP

#include "base/base.hpp"
#include "machgui/scrlarea.hpp"

//forward refs
class MachInGameScreen;
class MachLogHardwareLab;
class MachHWResearchBank;

//orthodox canonical revoked
class MachHWResearchMenu : public MachGuiScrollArea
{
public:
    //ctor. Parent is pInGameScreen. Available area is area.
    MachHWResearchMenu( GuiDisplayable* pParent, 
    					MachHWResearchBank* pHWResearchBank,
                        const Gui::Coord& relCoord,
                        MachInGameScreen* pInGameScreen );

    //dtor.
    ~MachHWResearchMenu();

    void CLASS_INVARIANT;

	static size_t reqWidth();
	static size_t reqHeight( MachInGameScreen*, const Gui::Coord& );

protected:
	virtual GuiSimpleScrollableList* createList( GuiDisplayable* pParent, const Gui::Coord& rel, MachInGameScreen* );

private:
    // Operations deliberately revoked
    MachHWResearchMenu( const MachHWResearchMenu& );
    MachHWResearchMenu& operator =( const MachHWResearchMenu& );
    bool operator ==( const MachHWResearchMenu& );

    friend ostream& operator <<( ostream& o, const MachHWResearchMenu& t );

    //The selected hardware lab
    MachLogHardwareLab& selectedLab( MachInGameScreen* pInGameScreen ) const;

    //Data members
    MachInGameScreen* pInGameScreen_; //The gui root
    MachHWResearchBank* pHWResearchBank_; //The bank depicting research queue state
    MachLogHardwareLab* pHardwareLab_; //The hw lab being edited
};


#endif

/* End  *************************************************/
