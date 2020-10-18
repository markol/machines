/*
 * SWLAB . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/transf3d.hpp"
#include "mathex/abox2d.hpp"
#include "phys/phys.hpp"
#include "sim/manager.hpp"
#include "world4d/domain.hpp"
#include "machphys/swlab.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/softdata.hpp"
#include "machphys/techdata.hpp"

#include "machlog/machlog.hpp"
#include "machlog/swlab.hpp"
#include "machlog/planet.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"
#include "machlog/restree.hpp"
#include "machlog/resitem.hpp"
#include "machlog/technici.hpp"
#include "machlog/animres.hpp"

#ifndef _INLINE
    #include "machlog/swlab.ipp"
#endif

//////////////////////////////////////////////////////////////////////////////////////////

MachLogSoftwareLab::MachLogSoftwareLab( MachLogRace* pRace, uint level,
                    const MexPoint3d& location, const MexRadians& angle )
:   MachLogConstruction( pRace, pNewPhysSoftwareLab( pRace, level, location, angle ),
                         MachPhysData::instance().softwareLabData( level ) ),
	lastUpdateTime_( SimManager::instance().currentTime() )
{

    TEST_INVARIANT;
    objectType( MachLog::SOFTWARE_LAB );
	hp( data().hitPoints() );
	armour( data().armour() );
	MachLogRaces::instance().softwareLabs( pRace->race() ).push_back( this );
}
//////////////////////////////////////////////////////////////////////////////////////////

MachLogSoftwareLab::~MachLogSoftwareLab()
{
	while( availableResearchItems_.size() > 0 )
		availableResearchItems_.erase( availableResearchItems_.begin() );

	while( currentResearchQueue_.size() > 0 )
		currentResearchQueue_.erase( currentResearchQueue_.begin() );
    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogSoftwareLab::CLASS_INVARIANT
{
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogSoftwareLab::doOutputOperator( ostream& o ) const
{
	MachActor::doOutputOperator( o );
	o << *this;
}

ostream& operator <<( ostream& o, const MachLogSoftwareLab& t )
{

    o << "MachLogSoftwareLab " << (void*)&t << " start" << std::endl;
	o << " MLRT.size() " << MachLogRaces::instance().researchTree().researchItems().size() << std::endl;
	o << " availableResearchItems_.size " << t.availableResearchItems_.size() << std::endl;
	if( t.availableResearchItems_.size() )
	{
		for( MachLogResearchTree::ResearchItems::const_iterator i = t.availableResearchItems_.begin(); i < t.availableResearchItems_.end(); ++i )
			o << (*i)->objectType() << " sub " << (*i)->subType() << (*i)->hwLevel() << std::endl;
	}
	o << " CurrentResearchQueue_.size " << t.currentResearchQueue_.size() << std::endl;
	MachLogResearchItem* pRi = NULL;
	if( t.currentlyResearching( &pRi ) )
	{
		o << " Current Research " << pRi->objectType() << " sub" << pRi->subType() << " hw " << pRi->hwLevel() << " tech " << pRi->technologyLevel() << std::endl;
		o << " amount researched " << pRi->amountResearched( t.race() ) << " built " << pRi->amountBuilt( t.race() ) << std::endl;
	}
	else
		o << " Not researching.\n";
    o << "MachLogSoftwareLab " << (void*)&t << " end" << std::endl;

    return o;
}
//////////////////////////////////////////////////////////////////////////////////////////
//static
MachPhysConstruction* MachLogSoftwareLab::pNewPhysSoftwareLab
(
    MachLogRace* pRace, uint level, const MexPoint3d& location, const MexRadians& zAngle
)
{
    //get domain and transform to use
    MexTransform3d localTransform;
    W4dDomain* pDomain =
        MachLogPlanetDomains::pDomainPosition( location, zAngle, &localTransform );

    //Construct the smelter
    return _NEW( MachPhysSoftwareLab( pDomain, localTransform, level, pRace->race() ) );
}
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////

MachPhysSoftwareLab* MachLogSoftwareLab::pPhysSoftwareLab()
{
    return (MachPhysSoftwareLab*)( &physObject() );
}

const MachPhysSoftwareLab* MachLogSoftwareLab::pPhysSoftwareLab() const
{
    return (MachPhysSoftwareLab*)( &physObject() );
}
//////////////////////////////////////////////////////////////////////////////////////////

//virtual
const MachPhysConstructionData& MachLogSoftwareLab::constructionData() const
{
	return data();
}

const MachPhysSoftwareLabData& MachLogSoftwareLab::data() const
{
	return pPhysSoftwareLab()->data();
}

//virtual
PhysRelativeTime MachLogSoftwareLab::update( const PhysRelativeTime& alteredMaxCPUTime, MATHEX_SCALAR junk )
{
    bool complete = isComplete();

	PhysRelativeTime interval = 5.0;

    if( not updateCompletionVisualisation() )
	{
		interval = 0.1;
		pPhysSoftwareLab()->isWorking( false );
	}

	if( complete )
	{
		if( currentResearchQueue_.size() )
		{
			pPhysSoftwareLab()->isWorking( true );
	//		HAL_STREAM("(" << id() << ") MLSoftwareLab::update...have something in currentReserachQueue\n" );
			MachLogResearchItem* pRi = currentResearchQueue_.front();
			int nextLevel = pRi->swLevel( race() ) + 1;
			if( nextLevel == 6 or pRi->swResearched( race(), nextLevel ) or not pRi->swAvailable( race(), nextLevel ) )
			{
	//			HAL_STREAM(" oops already researched..erasing and continueing\n" );
				currentResearchQueue_.erase( currentResearchQueue_.begin() );

			}
			else
			{
				//determine amount of technicians in the lab.
				int nextLevel = pRi->swLevel( race() ) + 1;
	//			HAL_STREAM(" Looking for some techincians\n" );
				int availableResearchUnits = 0;
				int availableBuildUnits = 0;
				for( MachLogRaces::Technicians::const_iterator i = MachLogRaces::instance().technicians( race() ).begin();
					i != MachLogRaces::instance().technicians( race() ).end(); ++i)
					if( (*i)->insideBuilding() and (*i)->insideWhichBuilding().id() == id() )
					{
						if( (*i)->isIdle() )
							(*i)->newOperation( _NEW( MachLogResearchAnimation( *i ) ) );
						availableResearchUnits += (*i)->data().researchRate();
					}
	//			HAL_STREAM(" found a total of " << availableResearchUnits << " research units to use\n" );
				availableResearchUnits = ( SimManager::instance().currentTime() - lastUpdateTime_ ) * availableResearchUnits / 60;
				if( availableResearchUnits > 0 )
					lastUpdateTime_ = SimManager::instance().currentTime();
				availableBuildUnits = availableResearchUnits;
				//check for already built.
				if( availableBuildUnits + pRi->swAmountBuilt( race(), nextLevel ) > pRi->swBuildingCost( nextLevel ) )
					availableBuildUnits = pRi->swBuildingCost( nextLevel ) - pRi->swAmountBuilt( race(), nextLevel );
				if( availableBuildUnits > MachLogRaces::instance().nBuildingMaterialUnits( race() ) )
					availableBuildUnits = MachLogRaces::instance().nBuildingMaterialUnits( race() );
				MachLogRaces::instance().nBuildingMaterialUnits( race() ) -= availableBuildUnits;
				//HAL_STREAM(" going to call advanceResearch for " << (*pRi) << std::endl );			
				pRi->swAdvanceResearch( race(), nextLevel, availableResearchUnits, availableBuildUnits );
				//are we already inside a hardware lab with research options...
				//if so then posts a MachLogResearchAnimation to the strategy
				//loop through all technicians and animate them

				//obj->newOperation( _NEW( MachLogResearchAnimation( obj ) ) );

				//HAL_STREAM(" after advanceResearch for " << (*pRi) << std::endl );			
			}
		}
		else
		{
			lastUpdateTime_ = SimManager::instance().currentTime();
			pPhysSoftwareLab()->isWorking( false );
		}
	}
	MachActor::update( alteredMaxCPUTime, junk );
    return interval;
}
MachLogResearchTree::ResearchItems& MachLogSoftwareLab::availableResearchItems()
{
//	HAL_STREAM("(" << id() << ") MLSoftwareLab::availableResearchItems\n" );
	if( availableResearchItems_.size() > 0 )
		return availableResearchItems_;
	initialiseResearchItems();
	return availableResearchItems_;
}

void MachLogSoftwareLab::initialiseResearchItems()
{
//	HAL_STREAM("(" << id() << ") MLSoftwareLab::initialiseResearchItems\n" );
	int highestTechLevel = 0;
	for( MachLogRaces::Technicians::const_iterator j = MachLogRaces::instance().technicians( race() ).begin();
		j != MachLogRaces::instance().technicians( race() ).end(); ++j )
			if( (*j)->technologyLevel() > highestTechLevel )
				highestTechLevel = (*j)->technologyLevel();

//	HAL_STREAM(" highest technology level for race " << (int)race() << " is " << highestTechLevel << std::endl );
	while( availableResearchItems_.size() > 0 )
		availableResearchItems_.erase( availableResearchItems_.begin() );

	const MachLogResearchTree::ResearchItems& items = MachLogRaces::instance().researchTree().researchItems();
//	HAL_STREAM(" scanning the tree for stuff. no items " << items.size() << std::endl );
	for( MachLogResearchTree::ResearchItems::const_iterator i = items.begin(); i!=items.end(); ++i)
	{
		MachLogResearchItem* pRi = *i;
		bool foundAtLeastOne = false;
//		if( pRi->available( race() ) ) HAL_STREAM(" checking swAvailable and swResearched flags for the following RI\n" << (**i) << std::endl );
		for( int l = 2; l < 6 and not foundAtLeastOne; ++l )
			if( pRi->swAvailable( race(), l ) and not pRi->swResearched( race(), l ) )
				foundAtLeastOne = true;
		//must have atleast one swAvailable flag
		//must have researched the hardware as well.
		//mustn't have reached the top of the chain
//		HAL_STREAM(" foundAtLeastOne " << foundAtLeastOne << std::endl );
		if( foundAtLeastOne and 
			pRi->available( race() ) and 
			pRi->researched( race() ) and 
			pRi->swLevel( race() ) < 5 and 
			pRi->swTechnologyLevel( pRi->swLevel( race() ) + 1 ) <= highestTechLevel )
		{
//			HAL_STREAM(" adding onto the availableResearchItems\n" );
			availableResearchItems_.push_back( pRi );
		}
	}
}

MachLogResearchTree::ResearchItems& MachLogSoftwareLab::currentResearchQueue()
{
	return currentResearchQueue_;
}

bool MachLogSoftwareLab::currentlyResearching( MachLogResearchItem** ppMLRI ) const
{
	if( currentResearchQueue_.size() )
	{
		*ppMLRI = currentResearchQueue_.front();
		return true;
	}
	*ppMLRI = NULL;
	return false;
}

bool MachLogSoftwareLab::addResearchItem( const MachLogResearchItem& mlri )
{
	//HAL_STREAM("(" << id() << ") MLSoftwareLab::addResearchItem item is\n" << mlri << std::endl );
	bool found = false;
	for( MachLogResearchTree::ResearchItems::iterator i = availableResearchItems_.begin();
		i != availableResearchItems_.end() and not found; ++i)
		if( (*i)->objectType() == mlri.objectType() and (*i)->subType() == mlri.subType() and (*i)->hwLevel() == mlri.hwLevel() )
		{
			//HAL_STREAM(" found a match in the available items list\n" );
			found = true;
			currentResearchQueue_.push_back( (*i) );
		}
	return found;
}

bool MachLogSoftwareLab::removeResearchItem( const MachLogResearchItem& mlri )
{
	bool found = false;
	for( MachLogResearchTree::ResearchItems::iterator i = currentResearchQueue_.begin();
		i != currentResearchQueue_.end() and not found; ++i)
		if( (*i) == &mlri )
		{
			found = true;
			//remove any researched amounts from the item.
			MachLogRaces::instance().nBuildingMaterialUnits( race() ) += (*i)->amountBuilt( race() );
			(*i)->advanceResearch( race(), -1 * (*i)->amountResearched( race() ), -1 * (*i)->amountBuilt( race() ) );

			currentResearchQueue_.erase( i );
		}
	return found;
}

void MachLogSoftwareLab::cancelCurrentResearch()
{
	if( currentResearchQueue_.size() > 0 )
		removeResearchItem( *currentResearchQueue_.front() );
}

MachPhys::BuildingMaterialUnits MachLogSoftwareLab::amountBuilt() const
{	
	if( currentResearchQueue_.size() > 0 )
	{
		return currentResearchQueue_.front()->amountBuilt( race() );
	}
	return 0;
}

MachPhys::ResearchUnits MachLogSoftwareLab::amountResearched() const
{	
	if( currentResearchQueue_.size() > 0 )
	{
		return currentResearchQueue_.front()->amountResearched( race() );
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////
/* End SWLAB.CPP **************************************************/
