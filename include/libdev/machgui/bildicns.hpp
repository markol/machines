/*
 * B I L D I C N S . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachBuildMenuIcons

    The construct icons displayed during the selection of a specific construction type.
*/

#ifndef _MACHGUI_BILDICNS_HPP
#define _MACHGUI_BILDICNS_HPP

#include "base/base.hpp"
#include "gui/scrolist.hpp"
#include "gui/gui.hpp"

#include "machlog/mlnotif.hpp"

//Forward refs
class MachInGameScreen;
class MachProductionBank;
class MachLogFactory;
class MachLogResearchTree;


class MachBuildMenuIcons : public GuiSimpleScrollableList, public MachLogNotifiable
//orthodox canonical (revoked)
{
public:
    //ctor. Located in pParent, covering area.
    //In game screen passed.
    MachBuildMenuIcons( GuiDisplayable* pParent, const Gui::Coord& rel,
                        MachProductionBank* pProductionBank, 
                        MachLogFactory* pFactory, 
                        MachInGameScreen* pInGameScreen );

    virtual ~MachBuildMenuIcons();

	virtual void notifiableBeNotified();
	
    void CLASS_INVARIANT;

	static size_t reqWidth();
	static size_t reqHeight( MachInGameScreen*,	const Gui::Coord& );
	
protected:
	virtual void doDisplay();

private:
    // Operations deliberately revoked
    MachBuildMenuIcons( const MachBuildMenuIcons& );
    MachBuildMenuIcons& operator =( const MachBuildMenuIcons& );
    bool operator ==( const MachBuildMenuIcons& );

    friend ostream& operator <<( ostream& o, const MachBuildMenuIcons& t );

    //Set up the list of icons
    void addIcons();

	MachInGameScreen* pInGameScreen_;
	MachProductionBank* pProductionBank_;
	MachLogFactory* pFactory_;
	
	MachLogResearchTree& researchTree_;
};


#endif

/* End BILDICNS.HPP *************************************************/
