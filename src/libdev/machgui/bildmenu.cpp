/*
 * B I L D M E N U . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "machgui/bildmenu.hpp"
#include "machgui/bildicns.hpp"
#include "machgui/prodbank.hpp"
#include "machgui/ingame.hpp"
#include "machlog/factory.hpp"
#include "world4d/subject.hpp"
#include "ctl/pvector.hpp"

MachBuildMenu::MachBuildMenu( 	GuiDisplayable* pParent, 
								MachProductionBank* pProductionBank, 
								const Gui::Coord& coord,
								MachInGameScreen* pInGameScreen )
:   MachGuiScrollArea( pParent, Gui::Box( coord, reqWidth(), reqHeight( pInGameScreen, coord ) ), pInGameScreen ),
    pInGameScreen_( pInGameScreen ),
    pProductionBank_( pProductionBank ),
    pFactory_( &selectedFactory( pInGameScreen ) )
{
    TEST_INVARIANT;
}

MachBuildMenu::~MachBuildMenu()
{
    TEST_INVARIANT;
}

void MachBuildMenu::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachBuildMenu& t )
{

    o << "MachBuildMenu " << (void*)&t << " start" << std::endl;
    o << "MachBuildMenu " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual 
GuiSimpleScrollableList* MachBuildMenu::createList( GuiDisplayable* pParent, const Gui::Coord& rel, MachInGameScreen* pInGameScreen )
{
	return _NEW( MachBuildMenuIcons( pParent, rel, pProductionBank_, pFactory_, pInGameScreen ) );
}	

MachLogFactory& MachBuildMenu::selectedFactory( MachInGameScreen* pInGameScreen ) const
{
    //Ensure there is a single selected factory
    const MachInGameScreen::Actors& selectionSet = pInGameScreen->selectedActors();
    ASSERT( selectionSet.size() == 1, "" );

    return selectionSet.front()->asFactory();
}

//static 
size_t MachBuildMenu::reqWidth()
{
	return MachGuiScrollBar::reqWidth() + MachBuildMenuIcons::reqWidth() + MachGuiScrollBar::reqWidth();
}

//static 
size_t MachBuildMenu::reqHeight( MachInGameScreen* pInGameScreen, const Gui::Coord& relCoord )
{
	return MachBuildMenuIcons::reqHeight( pInGameScreen, relCoord ) + 2;
}

/* End BILDMENU.CPP *************************************************/
