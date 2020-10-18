/*
 * P R O D I C N S . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "ctl/list.hpp"
#include "ctl/vector.hpp"
#include "gui/manager.hpp"
#include "machgui/prodicns.hpp"
#include "machgui/prodicon.hpp"
#include "machgui/gui.hpp"
#include "machgui/ingame.hpp"
#include "machgui/controlp.hpp"
#include "machlog/factory.hpp"
#include "machlog/produnit.hpp"

/* ////////////////////////////////////////////// constructor /////////////////////////////////////////////////// */

MachProductionIcons::MachProductionIcons(	GuiDisplayable * pParent, 
											const Gui::Box& area, 
											MachLogFactory* pFactory,
											MachInGameScreen * pInGameScreen )
: GuiSimpleScrollableList( 	pParent, area, 
							MachProductionIcon::buttonWidth(), 
							MachProductionIcon::buttonHeight(), 1 ),
  pFactory_( pFactory ),
  pInGameScreen_( pInGameScreen )
{
    //Add the icons
    updateIcons();

    TEST_INVARIANT;
}

/* /////////////////////////////////////////////// destructor /////////////////////////////////////////////////// */

MachProductionIcons::~MachProductionIcons()
{
    TEST_INVARIANT;
}

/* ////////////////////////////////////////////////////////////////////////////////////////////////////////////// */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachProductionIcons::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachProductionIcons::updateIcons()
{
    // Delete the existing icons
    deleteAllChildren();

    // Get the current production queue
    const MachLogFactory::ProductionLine& queue = pFactory_->productionLine();

    // Iterate through the queue and add an icon for each one
	MachLogFactory::ProductionLine::const_iterator it = queue.begin();
	int index = 1;

    for( ; it != queue.end(); ++it )
    {
        const MachLogProductionUnit& item = *(*it);
        _NEW( MachProductionIcon( this, pInGameScreen_, &( _CONST_CAST( MachLogProductionUnit&, item ) ), index++ ) );
    }

    // Ensure redisplayed
    childrenUpdated();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachProductionIcons::doDisplay()
{
	pInGameScreen_->controlPanel().redrawAreaImmediate( *this );
	GuiSimpleScrollableList::doDisplay();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachProductionIcons::cancelSelectedIcons() const
{

	for( Children::const_iterator i = children().begin(); i != children().end(); ++i )
	{
		GuiDisplayable* pChild = (*i);
		MachProductionIcon* pProdIcon = _REINTERPRET_CAST ( MachProductionIcon* , pChild);
		
		// deemed acceptable here as we know that all our children are of type MachProductionIcon;
		// alternative would be to keep wasteful personal container of MachProductionIcon pointers
		// that would constantly shadow the children collection inherited from GuiDisplayable.

		//MachLogProductionUnit& prodUnit = pProdIcon->productionUnit();		
		if ( pProdIcon->isDepressed() )
		{
			pFactory_->cancelProductionUnit( &( pProdIcon->productionUnit() ) );
		}
	}		
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//static 
size_t MachProductionIcons::height()
{
	return MachProductionIcon::buttonHeight();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//static 
size_t MachProductionIcons::width()
{
	return ( 3 * MachProductionIcon::buttonWidth() );	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachProductionIcons::updateProgress( float percentageComplete )
{
	if ( not children().empty() and not canScrollBackward() )
	{
		MachProductionIcon* pHeadOfList = _REINTERPRET_CAST( MachProductionIcon*, children().front() );
		pHeadOfList->updateProgress( percentageComplete );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const MachProductionIcons& t )
{
    o << "MachProductionIcons " << (void*)&t << " start" << std::endl;
    o << "MachProductionIcons " << (void*)&t << " end" << std::endl;
    return o;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* End PRODICNS.CPP *************************************************/
