/*
 * B I L D M E N U . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachBuildMenu

    Parent of the build menu items. This menu is displayed during interaction
    of the Build command for a factory, allowing editing of the factory's production queue.
*/

#ifndef _MACHGUI_BILDMENU_HPP
#define _MACHGUI_BILDMENU_HPP

#include "base/base.hpp"
#include "machgui/scrlarea.hpp"

//forward refs
class MachInGameScreen;
class MachLogFactory;
class MachProductionBank;

class MachBuildMenu : public MachGuiScrollArea
{
//orthodox canonical revoked
public:
    //ctor. Parent is pInGameScreen. Available area is area.
    //The productin bank depicting the production queue for the selected
    //factory is pProductionBank;
    MachBuildMenu( 	GuiDisplayable* pParent,
    				MachProductionBank* pProductionBank,
                   	const Gui::Coord& coord,
                   	MachInGameScreen* pInGameScreen );

    //dtor.
    ~MachBuildMenu();

    void CLASS_INVARIANT;

	static size_t reqWidth();
	static size_t reqHeight( MachInGameScreen*,	const Gui::Coord& );

protected:
	virtual GuiSimpleScrollableList* createList( GuiDisplayable* pParent, const Gui::Coord& rel, MachInGameScreen* );

private:
    // Operations deliberately revoked
    MachBuildMenu( const MachBuildMenu& );
    MachBuildMenu& operator =( const MachBuildMenu& );
    bool operator ==( const MachBuildMenu& );

    friend ostream& operator <<( ostream& o, const MachBuildMenu& t );

    //The selected factory
    MachLogFactory& selectedFactory( MachInGameScreen* pInGameScreen ) const;

    //Data members
    MachInGameScreen* pInGameScreen_; //The gui root
    MachProductionBank* pProductionBank_; //The bank depicting production queue state
    MachLogFactory* pFactory_; //The factory being edited
};


#endif

/* End BILDMENU.HPP *************************************************/
