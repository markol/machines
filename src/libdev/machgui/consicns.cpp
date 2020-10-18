/*
 * C O N S I C N S . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "machgui/consmenu.hpp"
#include "machgui/consicns.hpp"
#include "machgui/conselic.hpp"
#include "machgui/gui.hpp"
#include "machgui/ingame.hpp"
#include "machgui/controlp.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/constree.hpp"
#include "machlog/consitem.hpp"
#include "machlog/machlog.hpp"
#include "machlog/races.hpp"
#include "machphys/machphys.hpp"
#include "ctl/pvector.hpp"

MachConstructMenuIcons::MachConstructMenuIcons( GuiDisplayable* pParent, 
												const Gui::Coord& rel, 
												MachInGameScreen* pInGameScreen )
: GuiSimpleScrollableList( 	pParent, 
							Gui::Box( rel, reqWidth(), reqHeight( pInGameScreen ) ), 
							MachConstructionSelectIcon::reqWidth(), 
							MachConstructionSelectIcon::reqHeight(), 3 ),
  MachLogNotifiable(  MachLogRaces::instance().pcController().race() ),   // What race is the PC controller controlling?							
  consTree_(MachLogRaces::instance().constructionTree() ),
  pInGameScreen_( pInGameScreen )
{
	consTree_.addMe( this );
	
    //Construct the construction icons
    refreshConstructIcons();

    TEST_INVARIANT;
}

MachConstructMenuIcons::~MachConstructMenuIcons()
{
    TEST_INVARIANT;
	consTree_.removeMe( this );
}

void MachConstructMenuIcons::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachConstructMenuIcons& t )
{

    o << "MachConstructMenuIcons " << (void*)&t << " start" << std::endl;
    o << "MachConstructMenuIcons " << (void*)&t << " end" << std::endl;

    return o;
}

//static 
size_t MachConstructMenuIcons::reqHeight( MachInGameScreen* pInGameScreen )
{
	int height = pInGameScreen->controlPanel().getVisibleHeight() - 2;

	// Make height a multiple of MachBuildMachineIcon::reqHeight
	height -= height % MachConstructionSelectIcon::reqHeight();

	return height;	
}

//static 
size_t MachConstructMenuIcons::reqWidth()
{
	return ( 3 * MachConstructionSelectIcon::reqWidth() );	
}

// virtual
void MachConstructMenuIcons::doDisplay()
{
	pInGameScreen_->controlPanel().redrawAreaImmediate( *this );

	GuiSimpleScrollableList::doDisplay();
}

// virtual
void MachConstructMenuIcons::notifiableBeNotified()
{
	deleteAllChildren();
	refreshConstructIcons();
	
	TEST_INVARIANT;	
}

void MachConstructMenuIcons::refreshConstructIcons()
{
	//Construct the construction icons
  	  	
	MachLogConstructionTree::ConstructionItems& consItems = consTree_.constructionItems();
	MachLogConstructionTree::ConstructionItems::iterator i = consItems.begin();
	//The activated check has to occur inside the loop as individual items may be deactivated
	//This is slightly less efficient as we will now trawl through the whole lot.
	while (i != consItems.end() /*and (*i)->activated( race() )*/ )
	{
		if( (*i)->activated( race() ) )
			 _NEW( MachConstructionSelectIcon( 	this, 
			 									pInGameScreen_,
												*(*i) ) );
		++i;	
	}
	
	childrenUpdated();
	
	TEST_INVARIANT;	
}


/* End CONSICNS.CPP *************************************************/