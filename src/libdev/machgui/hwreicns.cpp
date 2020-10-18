/*
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "machgui/hwreicns.hpp"
#include "machgui/hwreicon.hpp"
#include "machgui/hwrebank.hpp"
#include "machgui/gui.hpp"
#include "machgui/ingame.hpp"
#include "machgui/controlp.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/hwlab.hpp"
#include "machlog/races.hpp"
#include "machlog/resitem.hpp"
#include "machlog/restree.hpp"
#include "ctl/pvector.hpp"

/* ////////////////////////////////////////////// constructor /////////////////////////////////////////////////// */

MachHWResearchIcons::MachHWResearchIcons
(
    GuiDisplayable* pParent, const Gui::Coord& rel,
    MachHWResearchBank* pHWResearchBank, 
    MachLogHardwareLab* pHardwareLab,
    MachInGameScreen* pInGameScreen
)
:   GuiSimpleScrollableList( pParent, Gui::Box( rel,
												MachHWResearchIcons::reqWidth(),
												MachHWResearchIcons::reqHeight( pInGameScreen, pParent->relativeCoord() ) ), 
												MachHWResearchIcon::reqWidth(), 
												MachHWResearchIcon::reqHeight(), 
												3 ),
	MachLogNotifiable(  MachLogRaces::instance().pcController().race() ),   // What race is the PC controller controlling?
	researchTree_( MachLogRaces::instance().researchTree() ),
	pHWResearchBank_( pHWResearchBank ),
	pHardwareLab_( pHardwareLab ),
	pInGameScreen_( pInGameScreen )
{
	researchTree_.addMe( this );

    //Add the icons
    addIcons( pHardwareLab_, pInGameScreen_, pHWResearchBank_ );

    TEST_INVARIANT;
}

/* /////////////////////////////////////////////// destructor /////////////////////////////////////////////////// */

MachHWResearchIcons::~MachHWResearchIcons()
{
    TEST_INVARIANT;
	researchTree_.removeMe( this );
}

/* ////////////////////////////////////////////////////////////////////////////////////////////////////////////// */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//static 
size_t MachHWResearchIcons::reqWidth()
{
	return ( 3 * MachHWResearchIcon::reqWidth() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//static 
size_t MachHWResearchIcons::reqHeight( MachInGameScreen* pInGameScreen, const Gui::Coord& relCoord )
{
	int height = pInGameScreen->controlPanel().getVisibleHeight() - ( 2 + relCoord.y() + MachHWResearchBank::reqHeight() );

	// Make height a multiple of MachHWResearchIcon::reqHeight
	height -= height % MachHWResearchIcon::reqHeight();

	return height;	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachHWResearchIcons::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const MachHWResearchIcons& t )
{

    o << "MachHWResearchIcons " << (void*)&t << " start" << std::endl;
    o << "MachHWResearchIcons " << (void*)&t << " end" << std::endl;

    return o;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachHWResearchIcons::addIcons( MachLogHardwareLab* pHardwareLab, 
									MachInGameScreen* pInGameScreen,
									MachHWResearchBank* pHWResearchBank )
{
    // Get the list of valid research items for the lab using the research item list
	const MachLogResearchTree::ResearchItems& items = pHardwareLab->availableResearchItems();

    // Iterate through the items and add an icon for each one that hasn't been researched
    for( MachLogResearchTree::ResearchItems::const_iterator it = items.begin();
         it != items.end(); ++it )
    {
        //Check not already researched
        MachLogResearchItem* pResearchItem = (*it);
        if( not pResearchItem->researched( pHardwareLab->race() ) )
        {
            _NEW( MachHWResearchIcon( this, pInGameScreen, pHWResearchBank, pHardwareLab,
                                      pResearchItem ) );
        }
    }
// JERRY_STREAM( WHERE_STR << std::endl );
	childrenUpdated();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachHWResearchIcons::doDisplay()
{
	pInGameScreen_->controlPanel().redrawAreaImmediate( *this );	
	GuiSimpleScrollableList::doDisplay();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// virtual
void MachHWResearchIcons::notifiableBeNotified()
{
	deleteAllChildren();
	addIcons( pHardwareLab_, pInGameScreen_, pHWResearchBank_ );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* End *************************************************/
