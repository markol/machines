/*
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "machgui/hwremenu.hpp"
#include "machgui/hwreicns.hpp"
#include "machgui/hwrebank.hpp"
#include "machgui/ingame.hpp"
#include "machlog/hwlab.hpp"
#include "world4d/subject.hpp"
#include "ctl/pvector.hpp"

MachHWResearchMenu::MachHWResearchMenu( GuiDisplayable* pParent, 
										MachHWResearchBank* pHWResearchBank,
									    const Gui::Coord& relCoord,
									    MachInGameScreen* pInGameScreen )
:   MachGuiScrollArea( pParent, Gui::Box( relCoord, reqWidth(), reqHeight( pInGameScreen,relCoord ) ), pInGameScreen ),
    pInGameScreen_( pInGameScreen ),
    pHWResearchBank_( pHWResearchBank ),
    pHardwareLab_( &selectedLab( pInGameScreen ) )
{
    PRE( pHWResearchBank != NULL );
 
    TEST_INVARIANT;
}

MachHWResearchMenu::~MachHWResearchMenu()
{
    TEST_INVARIANT;
}

void MachHWResearchMenu::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

//virtual 
GuiSimpleScrollableList* MachHWResearchMenu::createList( GuiDisplayable* pParent, const Gui::Coord& rel, MachInGameScreen* pInGameScreen )
{
	return _NEW( MachHWResearchIcons( pParent, rel, pHWResearchBank_, pHardwareLab_, pInGameScreen ) );
}

ostream& operator <<( ostream& o, const MachHWResearchMenu& t )
{

    o << "MachHWResearchMenu " << (void*)&t << " start" << std::endl;
    o << "MachHWResearchMenu " << (void*)&t << " end" << std::endl;

    return o;
}

MachLogHardwareLab& MachHWResearchMenu::selectedLab( MachInGameScreen* pInGameScreen ) const
{
    //Ensure there is a single selected factory
    const MachInGameScreen::Actors& selectionSet = pInGameScreen->selectedActors();
    ASSERT( selectionSet.size() == 1, "" );

    return selectionSet.front()->asHardwareLab();
}

//static 
size_t MachHWResearchMenu::reqWidth()
{
	return MachGuiScrollBar::reqWidth() + MachHWResearchIcons::reqWidth() + MachGuiScrollBar::reqWidth();
}

//static 
size_t MachHWResearchMenu::reqHeight( MachInGameScreen* pInGameScreen, const Gui::Coord& relCoord )
{
	return MachHWResearchIcons::reqHeight( pInGameScreen, relCoord ) + 2;
}

/* End  *************************************************/
