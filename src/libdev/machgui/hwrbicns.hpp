/*
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachHWResearchBankIcons

    The research queue for a hardware lab depicted as a sequence of icons
*/

#ifndef _MACHGUI_HWRBICNS_HPP
#define _MACHGUI_HWRBICNS_HPP

#include "base/base.hpp"
#include "gui/scrolist.hpp"
#include "gui/gui.hpp"

//Forward refs
class MachInGameScreen;
class MachLogHardwareLab;

//orthodox canonical (revoked)
class MachHWResearchBankIcons : public GuiSimpleScrollableList
{
public:
    //ctor. Located in pParent, covering area.
    //In game screen passed.
    MachHWResearchBankIcons( GuiDisplayable* pParent, const Gui::Box& area,
                             MachLogHardwareLab* pHardwareLab, MachInGameScreen* pInGameScreen );

    //dtor.
    virtual ~MachHWResearchBankIcons();

    //Set up the list of icons
    void updateIcons();

	static size_t reqHeight();
	static size_t reqWidth();

	void updateProgress( float percentageComplete );
	
	//Remove from the bank any and all selected icons
	//This also removes the associated production units from this class's associated factory's production line.
	void cancelSelectedIcons() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachHWResearchBankIcons& t );

protected:
	virtual void doDisplay();

private:
    // Operations deliberately revoked
    MachHWResearchBankIcons( const MachHWResearchBankIcons& );
    MachHWResearchBankIcons& operator =( const MachHWResearchBankIcons& );
    bool operator ==( const MachHWResearchBankIcons& );

    //data members
    MachLogHardwareLab* pHardwareLab_; //The hw lab whose research queue is depicted
    MachInGameScreen* pInGameScreen_; //The gui root
};


#endif

/* End *************************************************/
