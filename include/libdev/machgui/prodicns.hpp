/*
 * P R O D I C N S . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachProductionIcons

    The production queue for a factory depicted as a sequence of icons
*/

#ifndef _MACHGUI_PRODICNS_HPP
#define _MACHGUI_PRODICNS_HPP

#include "base/base.hpp"
#include "gui/scrolist.hpp"
#include "gui/gui.hpp"

//Forward refs
class MachInGameScreen;
class MachLogFactory;

//orthodox canonical (revoked)
class MachProductionIcons : public GuiSimpleScrollableList
{
public:
    //ctor. Located in pParent, covering area.
    //In game screen passed.
    MachProductionIcons( GuiDisplayable* pParent, const Gui::Box& area,
                         MachLogFactory* pFactory, MachInGameScreen* pInGameScreen );

    //dtor.
    virtual ~MachProductionIcons();

    //Set up the list of icons
    void updateIcons();
	
	//Remove from the bank any and all selected icons
	//This also removes the associated production units from this class's associated factory's production line.
	void cancelSelectedIcons() const;

	static size_t height();
	static size_t width();

	void updateProgress( float percentageComplete );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachProductionIcons& t );

protected:
	virtual void doDisplay();

private:
    // Operations deliberately revoked
    MachProductionIcons( const MachProductionIcons& );
    MachProductionIcons& operator =( const MachProductionIcons& );
    bool operator ==( const MachProductionIcons& );

    //data members
    MachLogFactory* pFactory_; //The factory whose production queu is depicted
    MachInGameScreen* pInGameScreen_; //The gui root
};


#endif

/* End BILDICNS.HPP *************************************************/
