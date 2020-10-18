/*
 * C O N S M E N U . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machgui/consmenu.hpp"
#include "machgui/consicns.hpp"
#include "machgui/ingame.hpp"

MachConstructMenu::MachConstructMenu( GuiDisplayable* pParent, const Gui::Coord& relCoord, MachInGameScreen* pInGameScreen )
:   MachGuiScrollArea( pParent, Gui::Box( relCoord, reqWidth(), reqHeight( pInGameScreen ) ), pInGameScreen )
{

    TEST_INVARIANT;
}

MachConstructMenu::~MachConstructMenu()
{
    TEST_INVARIANT;

}

void MachConstructMenu::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachConstructMenu& t )
{

    o << "MachConstructMenu " << (void*)&t << " start" << std::endl;
    o << "MachConstructMenu " << (void*)&t << " end" << std::endl;

    return o;
}

//static 
size_t MachConstructMenu::reqWidth()
{
	return MachGuiScrollBar::reqWidth() + MachConstructMenuIcons::reqWidth() + MachGuiScrollBar::reqWidth();
}

//static 
size_t MachConstructMenu::reqHeight( MachInGameScreen* pInGameScreen )
{
	return MachConstructMenuIcons::reqHeight( pInGameScreen ) + 2;
}

//virtual 
GuiSimpleScrollableList* MachConstructMenu::createList( GuiDisplayable* pParent, const Gui::Coord& rel, MachInGameScreen* pInGameScreen )
{
	return _NEW( MachConstructMenuIcons( pParent, rel, pInGameScreen ) );
}	

/* End CONSMENU.CPP *************************************************/
