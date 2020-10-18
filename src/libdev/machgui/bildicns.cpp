/*
 * B I L D I C N S . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "machgui/consmenu.hpp"
#include "machgui/bildicns.hpp"
#include "machgui/buildmci.hpp"
#include "machgui/gui.hpp"
#include "machgui/ingame.hpp"
#include "machgui/controlp.hpp"
#include "machgui/prodbank.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/factory.hpp"
#include "machlog/races.hpp"
#include "machlog/restree.hpp"
#include "machlog/resitem.hpp"
#include "machphys/machphys.hpp"
#include "ctl/pvector.hpp"

MachBuildMenuIcons::MachBuildMenuIcons(	GuiDisplayable* pParent, const Gui::Coord& rel,
									    MachProductionBank* pProductionBank_,
									    MachLogFactory* pFactory_,
									    MachInGameScreen* pInGameScreen_ )
:   GuiSimpleScrollableList( 	pParent,
								Gui::Box( rel, reqWidth(), reqHeight( pInGameScreen_, pParent->relativeCoord() ) ),
								MachBuildMachineIcon::reqWidth(),
								MachBuildMachineIcon::reqHeight(),
								3 ),
	MachLogNotifiable(  MachLogRaces::instance().pcController().race() ),   // What race is the PC controller controlling?
	researchTree_( MachLogRaces::instance().researchTree() ),
	pInGameScreen_( pInGameScreen_ ),
	pFactory_( pFactory_ ),
	pProductionBank_ (pProductionBank_ )

{
	researchTree_.addMe( this );

    //Add the icons
    addIcons();

    TEST_INVARIANT;
}

MachBuildMenuIcons::~MachBuildMenuIcons()
{
    TEST_INVARIANT;
	researchTree_.removeMe( this );

}

void MachBuildMenuIcons::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachBuildMenuIcons& t )
{

    o << "MachBuildMenuIcons " << (void*)&t << " start" << std::endl;
    o << "MachBuildMenuIcons " << (void*)&t << " end" << std::endl;

    return o;
}

void MachBuildMenuIcons::addIcons()
{
    //Get the list of valid build items for the factory using the research item list
	const MachLogResearchTree::ResearchItems& items = pFactory_->buildItems();

    //Iterate through the items and add an icon for each one that has been researched
    for( MachLogResearchTree::ResearchItems::const_iterator it = items.begin();
         it != items.end(); ++it )
    {
        //Check researched
        const MachLogResearchItem& item = *(*it);
        if( item.researched( pFactory_->race() ) )
        {
            _NEW( MachBuildMachineIcon( this, pInGameScreen_, pProductionBank_, pFactory_, item ) );
        }
    }

	childrenUpdated();
}

//static
size_t MachBuildMenuIcons::reqWidth()
{
	return ( 3 * MachBuildMachineIcon::reqWidth() );
}

//static
size_t MachBuildMenuIcons::reqHeight( MachInGameScreen* pInGameScreen, const Gui::Coord& relCoord )
{
	int height = pInGameScreen->controlPanel().getVisibleHeight() - ( 2 + relCoord.y() + MachProductionBank::height() );

	// Make height a multiple of MachBuildMachineIcon::reqHeight
	height -= height % MachBuildMachineIcon::reqHeight();

	return height;
}

// virtual
void MachBuildMenuIcons::doDisplay()
{
	pInGameScreen_->controlPanel().redrawAreaImmediate( *this );

	GuiSimpleScrollableList::doDisplay();
}

// virtual
void MachBuildMenuIcons::notifiableBeNotified()
{
	deleteAllChildren();
	addIcons();
}


/* End BILDICNS.CPP *************************************************/
