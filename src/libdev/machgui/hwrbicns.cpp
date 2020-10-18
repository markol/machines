/*
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "machgui/hwrbicns.hpp"
#include "machgui/hwrbicon.hpp"
#include "machgui/gui.hpp"
#include "machgui/ingame.hpp"
#include "machgui/controlp.hpp"
#include "machlog/hwlab.hpp"
#include "gui/manager.hpp"
#include "ctl/pvector.hpp"

/* ////////////////////////////////////////////// constructor /////////////////////////////////////////////////// */

MachHWResearchBankIcons::MachHWResearchBankIcons(	GuiDisplayable* pParent, 
													const Gui::Box& area, 
													MachLogHardwareLab* pHardwareLab,
    												MachInGameScreen* pInGameScreen )
:   GuiSimpleScrollableList( 	pParent, area, 
								MachHWResearchBankIcon::reqWidth(), 
								MachHWResearchBankIcon::reqHeight(), 1 ),
  	pHardwareLab_( pHardwareLab ),
    pInGameScreen_( pInGameScreen )
{
    //Add the icons
    updateIcons();

    TEST_INVARIANT;
}

/* /////////////////////////////////////////////// destructor /////////////////////////////////////////////////// */

MachHWResearchBankIcons::~MachHWResearchBankIcons()
{
    TEST_INVARIANT;

}

/* ////////////////////////////////////////////////////////////////////////////////////////////////////////////// */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachHWResearchBankIcons::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const MachHWResearchBankIcons& t )
{

    o << "MachHWResearchBankIcons " << (void*)&t << " start" << std::endl;
    o << "MachHWResearchBankIcons " << (void*)&t << " end" << std::endl;

    return o;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachHWResearchBankIcons::updateIcons()
{
    //Delete the existing icons
    deleteAllChildren();

    //Get the current research queue
    const MachLogResearchTree::ResearchItems& queue = pHardwareLab_->currentResearchQueue();

	MachPhys::Race race = pHardwareLab_->race();
	
    //Iterate through the queue and add an icon for each one
    for( MachLogResearchTree::ResearchItems::const_iterator it = queue.begin();
         it != queue.end(); ++it )
    {
        MachLogResearchItem* pResearchItem = (*it);
        _NEW( MachHWResearchBankIcon( this, pResearchItem, pInGameScreen_, race ) );
    }

    //Ensure redisplayed
    childrenUpdated();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachHWResearchBankIcons::cancelSelectedIcons() const
{

	for( Children::const_iterator i = children().begin(); i != children().end(); ++i )
	{
 		GuiDisplayable* pChild = (*i);
		MachHWResearchBankIcon* pHWResBankIcon = _REINTERPRET_CAST ( MachHWResearchBankIcon* , pChild);
		
		// deemed acceptable here as we know that all our children are of type MachHWResearchBankIcon;
		// alternative would be to keep wasteful personal container of MachHWResearchBankIcon pointers
		// that would constantly shadow the children collection inherited from GuiDisplayable.

		//MachLogProductionUnit& prodUnit = pHWResIcon->productionUnit();		
		if ( pHWResBankIcon->isDepressed() )
		{
			pHardwareLab_->removeResearchItem(  pHWResBankIcon->researchItem() );
		}
	}		
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachHWResearchBankIcons::doDisplay()
{
	pInGameScreen_->controlPanel().redrawAreaImmediate( *this );
	GuiSimpleScrollableList::doDisplay();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//static 
size_t MachHWResearchBankIcons::reqHeight()
{
	return MachHWResearchBankIcon::reqHeight();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//static 
size_t MachHWResearchBankIcons::reqWidth()
{
	return ( 3 * MachHWResearchBankIcon::reqWidth() );	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachHWResearchBankIcons::updateProgress( float percentageComplete )
{
	if ( not children().empty() and not canScrollBackward() )
	{
		MachHWResearchBankIcon* pHeadOfList = _REINTERPRET_CAST( MachHWResearchBankIcon*, children().front() );
		pHeadOfList->updateProgress( percentageComplete );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* End *************************************************/
