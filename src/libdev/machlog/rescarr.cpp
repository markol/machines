/*
 * R E S C A R R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include "machphys/rescarr.hpp"
#include "machlog/internal/rescarri.hpp"

#include <iostream>

#include "stdlib/string.hpp"

#include "ctl/algorith.hpp"
#include "ctl/list.hpp"
#include "ctl/pvector.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/point2d.hpp"

#include "sim/manager.hpp"

#include "world4d/domain.hpp"
#include "world4d/soundman.hpp"

#include "machphys/rcardata.hpp"
#include "machphys/rescarr.hpp"
#include "machphys/random.hpp"

#include "machlog/cntrl.hpp"
#include "machlog/debris.hpp"
#include "machlog/mine.hpp"
#include "machlog/messbrok.hpp"
#include "machlog/network.hpp"
#include "machlog/opscav.hpp"
#include "machlog/optransp.hpp"
#include "machlog/oppickup.hpp"
#include "machlog/opputdwn.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/planet.hpp"
#include "machlog/pod.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"
#include "machlog/rescarr.hpp"
#include "machlog/smelter.hpp"
#include "machlog/spacial.hpp"
#include "machlog/strategy.hpp"
#include "machlog/vmman.hpp"
#include "machlog/vmdata.hpp"


#define CB_MachLogResourceCarrier_DEPIMPL()	\
		PRE( pImpl_ );	\
		CB_DEPIMPL( MachPhys::BuildingMaterialUnits, amountCarried_ ); \
   		CB_DEPIMPL( bool, isTransporting_ ); \
		CB_DEPIMPL( bool, isPickingUp_ ); \
		CB_DEPIMPL( bool, isPuttingDown_ ); \
		CB_DEPIMPL( MachLogResourceCarrier::Suppliers, suppliers_ ); \
   		CB_DEPIMPL( MachLogConstruction*, pDestinationSmeltingBuilding_ ); \
		CB_DEPIMPL( bool, smeltingBuildingExplicitlyAssigned_ ); \
		CB_DEPIMPL( bool, protectNodesFromClearance_ ); \
		CB_DEPIMPL( size_t, currentElement_ ); \
		CB_DEPIMPL( int, iterations_ );


PER_DEFINE_PERSISTENT( MachLogResourceCarrier );
/* //////////////////////////////////////////////////////////////// */

MachLogResourceCarrier::MachLogResourceCarrier( MachLogMachine::Level hwLevel, MachLogMachine::Level swLevel,
    								MachLogRace * pRace, const MexPoint3d& location )
: MachLogMachine( MachPhys::RESOURCE_CARRIER, hwLevel, swLevel, pRace,
                  pNewPhysResourceCarrier( hwLevel, swLevel, pRace, location ),
                  MachLog::RESOURCE_CARRIER ),
  pImpl_( _NEW( MachLogResourceCarrierImpl( ) ) )
{
	hp( data().hitPoints() );
	armour( data().armour() );
	ctl_append( &MachLogRaces::instance().resourceCarriers( pRace->race() ), this );
}

MachLogResourceCarrier::MachLogResourceCarrier( MachLogMachine::Level hwLevel, MachLogMachine::Level swLevel,
    								MachLogRace * pRace, const MexPoint3d& location, UtlId withId )
: MachLogMachine( MachPhys::RESOURCE_CARRIER, hwLevel, swLevel, pRace,
                  pNewPhysResourceCarrier( hwLevel, swLevel, pRace, location ),
                  MachLog::RESOURCE_CARRIER, withId ),
  pImpl_( _NEW( MachLogResourceCarrierImpl( ) ) )
{
	hp( data().hitPoints() );
	armour( data().armour() );
	ctl_append( &MachLogRaces::instance().resourceCarriers( pRace->race() ), this );
}

MachLogResourceCarrier::~MachLogResourceCarrier()
{
	CB_MachLogResourceCarrier_DEPIMPL();

	clearSmeltingBuilding();
	clearSuppliers();

	_DELETE( pImpl_ );
}

PhysRelativeTime MachLogResourceCarrier::pickUp()
{
	return 1.0;
}

PhysRelativeTime MachLogResourceCarrier::putDown()
{
	return 1.0;
}

void MachLogResourceCarrier::addSuppliers( const Suppliers& listOfSuppliers )
{
	CB_MachLogResourceCarrier_DEPIMPL();

	PRE( not listOfSuppliers.empty() );

	StartWithPickUpOrPutDown startType;

	if( isPuttingDown() )
		startType = START_WITH_PUTDOWN;
	else
		startType = START_WITH_PICKUP;

	bool atLeastOneDifferentSupplier = false;

	//copy list of suppliers into internal list and register with each one
	for( Suppliers::const_iterator i = listOfSuppliers.begin(); i!= listOfSuppliers.end(); ++i )
	{
		ASSERT_INFO( (*i)->objectType() );
		ASSERT( (*i)->objectType() == MachLog::MINE
		 		or ( (*i)->objectIsCanSmelt()
		 	  		 and (*i)->race() != race() ),
		 	  	"Not a valid supplier type." );

		// don't add a duplicate
		Suppliers::iterator iDuplicate = find( suppliers_.begin(), suppliers_.end(), (*i) );
		bool duplicate = iDuplicate != suppliers_.end();

		if( not duplicate )
		{
			suppliers_.push_back( (*i) );
			(*i)->attach( this );
			(*i)->addThreat( id() );
			atLeastOneDifferentSupplier = true;
		}
	}

	if( atLeastOneDifferentSupplier )
	{
		// we MIGHT want to make this defcon-dependent.....if defcon HIGH, do NOT auto-reorder.

		// if we have a smelter that has NOT been explicitly set, try a findBestSmeltingBuilding() again
		// as the addition of a new supplier may make a different smelter the optimum one.

		if( not smeltingBuildingExplicitlyAssigned_ )
			findBestSmeltingBuilding();

		if( hasSmeltingBuilding() )
				reorderTransportRoute();

		mayNeedNewOp( startType );
	}

	POST( hasSuppliers() );
}

void MachLogResourceCarrier::setSuppliers( const Suppliers& listOfSuppliers )
{
	CB_MachLogResourceCarrier_DEPIMPL();

	PRE( not listOfSuppliers.empty() );

	StartWithPickUpOrPutDown startType;

	if( isPuttingDown() )
		startType = START_WITH_PUTDOWN;
	else
		startType = START_WITH_PICKUP;

	// okay......if we're currently in the middle of a pickup, it may be necessary to clear the strategy queue
	// in light of the altered suppliers list.
	// HOWEVER, if our current list is only of size 1 and we're actually replacing the supplier with
	// an identical copy of itself (i.e. the user has redundantly re-clicked on the same supplier), don't
	// clear the queue.

	if( isPickingUp() )
	{
		if( suppliers_.size() == 1
			and listOfSuppliers.size() == 1
			and ( *suppliers_.begin() == *listOfSuppliers.begin() ) )
		{
			// do nothing
		}
		else if( not evading() )
		{
			// stop in our tracks.

			// this will protect all our supplier and smelter lists from being wiped out as a result
			// of the removal of the transport op (as would normally be the case)
			protectNodesFromClearance_ = true;

			strategy().tryToRemoveAllOperations();

			protectNodesFromClearance_ = false;
		}

	}

	clearSuppliers();

	//copy list of suppliers into internal list and register with each one
	for( Suppliers::const_iterator i = listOfSuppliers.begin(); i!= listOfSuppliers.end(); ++i )
	{
		ASSERT_INFO( (*i)->objectType() );
		ASSERT( (*i)->objectType() == MachLog::MINE
		 		or ( (*i)->objectIsCanSmelt()
		 	  		 and (*i)->race() != race() ),
		 	  	"Not a valid supplier type." );


		suppliers_.push_back( *i );
		(*i)->attach( this );
		(*i)->addThreat( id() );
	}

	if( not smeltingBuildingExplicitlyAssigned_ )
		findBestSmeltingBuilding();

	if( hasSmeltingBuilding() )
			reorderTransportRoute();

	mayNeedNewOp( startType );

	POST( hasSuppliers() );
}

void MachLogResourceCarrier::setSmeltingBuilding( MachLogConstruction* pSmeltingBuilding )
{
	CB_MachLogResourceCarrier_DEPIMPL();

	PRE( pSmeltingBuilding != NULL );

	internalSetSmeltingBuilding( pSmeltingBuilding );

	smeltingBuildingExplicitlyAssigned_ = true;
}

void MachLogResourceCarrier::internalSetSmeltingBuilding( MachLogConstruction* pSmeltingBuilding )
{
	CB_MachLogResourceCarrier_DEPIMPL();

	PRE( pSmeltingBuilding != NULL );

	StartWithPickUpOrPutDown startType;

	if( isPuttingDown() )
		startType = START_WITH_PUTDOWN;
	else
		startType = START_WITH_PICKUP;

	if( pSmeltingBuilding != pDestinationSmeltingBuilding_ )
	{
		if( hasSmeltingBuilding() )
		{
			clearSmeltingBuilding();
		}

		if( isPuttingDown() and not evading() )
		{
			// As we were engaged in a putdown, and our smelter has changed, no point carrying on towards the
			// old one (or the location it occupied if now destroyed) if we're not actually going to use it.
			// Cancel all our current ops - they'll be rebuilt with the new data in a moment.

			// this will protect all our supplier list from being wiped out as a result
			// of the removal of the transport op (contrary to what would normally be the case)
			protectNodesFromClearance_ = true;

			strategy().tryToRemoveAllOperations();

			protectNodesFromClearance_ = false;
		}

		pDestinationSmeltingBuilding_ = pSmeltingBuilding;
		pDestinationSmeltingBuilding_->attach( this );

		if( hasSuppliers() )
			reorderTransportRoute();

		mayNeedNewOp( startType );
	}
}

bool MachLogResourceCarrier::hasSmeltingBuilding() const
{
	CB_MachLogResourceCarrier_DEPIMPL();

	return pDestinationSmeltingBuilding_ != NULL;
}

bool MachLogResourceCarrier::hasSuppliers() const
{
	CB_MachLogResourceCarrier_DEPIMPL();

	return not suppliers_.empty();
}

MachLogResourceCarrier::Suppliers& MachLogResourceCarrier::suppliers()
{
	CB_MachLogResourceCarrier_DEPIMPL();

	return suppliers_;
}

MachLogConstruction* MachLogResourceCarrier::pDestinationSmeltingBuilding()
{
	CB_MachLogResourceCarrier_DEPIMPL();

	return pDestinationSmeltingBuilding_;
}

void MachLogResourceCarrier::clearSuppliers()
{
	CB_MachLogResourceCarrier_DEPIMPL();

	while( hasSuppliers() )
	{
		suppliers_.back()->detach( this );
		suppliers_.back()->removeThreat( id() );
		suppliers_.pop_back();
	}

	currentElement_ = 0;
}

void MachLogResourceCarrier::clearSmeltingBuilding()
{
	CB_MachLogResourceCarrier_DEPIMPL();

	if( pDestinationSmeltingBuilding_ != NULL )
	{
		pDestinationSmeltingBuilding_->detach( this );
		pDestinationSmeltingBuilding_ = NULL;
	}

	smeltingBuildingExplicitlyAssigned_ = false;
}

bool MachLogResourceCarrier::movingToNextSupplier()
{
	CB_MachLogResourceCarrier_DEPIMPL();

	ASSERT( currentElement_ < suppliers_.size(), "The currentElement_ has been allowed to exceed the suppliers size somewhere." );

	if( currentElement_ + 1 >= suppliers_.size() )
		return false;
	else
	{
		++currentElement_;
		return true;
	}
}

void MachLogResourceCarrier::movingToFirstSupplier()
{
	CB_MachLogResourceCarrier_DEPIMPL();

 	currentElement_ = 0;
}

size_t MachLogResourceCarrier::currentSupplierIndex() const
{
	CB_MachLogResourceCarrier_DEPIMPL();

	return currentElement_;
}

void MachLogResourceCarrier::currentSupplierIndex( size_t newIndex )
{
	CB_MachLogResourceCarrier_DEPIMPL();

	PRE( newIndex < suppliers_.size() );

	currentElement_ = newIndex;
}

void MachLogResourceCarrier::clearSuppliersAndSmeltingBuilding()
{
	clearSuppliers();
	clearSmeltingBuilding();
}

void MachLogResourceCarrier::mayNeedNewOp( StartWithPickUpOrPutDown startType )
{
	CB_MachLogResourceCarrier_DEPIMPL();

	if( not isTransporting() )
	{
		// okay, should we now initiate a transport op?
		if( hasSuppliers() or hasSmeltingBuilding() )
			newOperation( _NEW( MachLogTransportOperation( this, startType ) ) );
	}
}

bool MachLogResourceCarrier::findBestSmeltingBuilding()
{
	CB_MachLogResourceCarrier_DEPIMPL();

	bool found = false;

	bool alreadyHadSmeltingBuilding = hasSmeltingBuilding();
	MachLogConstruction* pOldSmeltingBuilding = pDestinationSmeltingBuilding_;

	MachLogRaces::Smelters& smelters = MachLogRaces::instance().smelters( race() );
	MachLogRaces::Pods& pods = MachLogRaces::instance().pods( race() );

	// okay, here's the rub. You'll like this. Choose the smelter or pod with the lowest total distance to all suppliers.
	// also, do not consider for selection any smelter/pod which is over 10000m away from ALL suppliers.
	// (i.e. effectively, no maximum distance checks at the moment)
	MATHEX_SCALAR lowestDistanceFound = 100000000;

	// first, do the smelters
	if( not smelters.empty() )
	{
		for( MachLogRaces::Smelters::iterator iSmelt = smelters.begin(); iSmelt != smelters.end(); ++iSmelt )
		{
			MachLogSmelter& candidateSmelter = (**iSmelt);

			// don't consider smelters that are incomplete or dead
			if( candidateSmelter.isComplete() and not candidateSmelter.isDead() )
			{
				MATHEX_SCALAR totalDistance = 0;

				bool tooFarFromAllSuppliers = true;

				for( Suppliers::iterator iSup = suppliers_.begin(); iSup != suppliers_.end(); ++iSup )
				{
					ASSERT( (*iSup)->objectIsConstruction(), "Supplier was not a construction!" );
					MachLogConstruction& supplierConstruction = (**iSup);

					// don't bother taking this supplier into account if it's a currently-dormant mine
					if( supplierConstruction.objectType() != MachLog::MINE
						or supplierConstruction.asMine().worthVisiting() )
					{
						MATHEX_SCALAR smelterDistanceFromThisSupplier =
								supplierConstruction.position().euclidianDistance( candidateSmelter.position() );

						totalDistance += smelterDistanceFromThisSupplier;
						if( smelterDistanceFromThisSupplier < 10000 )
							tooFarFromAllSuppliers = false;
					}

				}

				if( (not tooFarFromAllSuppliers) and totalDistance < lowestDistanceFound )
				{
					lowestDistanceFound = totalDistance;
					found = true;
					internalSetSmeltingBuilding( &candidateSmelter );
				}
			}
		}
	}
	// next, the pods
	if( not pods.empty() )
	{
		for( MachLogRaces::Pods::iterator iPod = pods.begin(); iPod != pods.end(); ++iPod )
		{
			MachLogPod& candidatePod = (**iPod);

			// don't consider pods that are incomplete or dead
			if( candidatePod.isComplete() and not candidatePod.isDead() )
			{
				MATHEX_SCALAR totalDistance = 0;

				bool tooFarFromAllSuppliers = true;

				for( Suppliers::iterator iSup = suppliers_.begin(); iSup != suppliers_.end(); ++iSup )
				{
					ASSERT( (*iSup)->objectIsConstruction(), "Supplier was not a construction!" );
					MachLogConstruction& supplierConstruction = (**iSup);

					// don't bother taking this supplier into account if it's a currently-dormant mine
					if( supplierConstruction.objectType() != MachLog::MINE
						or supplierConstruction.asMine().worthVisiting() )
					{
						MATHEX_SCALAR podDistanceFromThisSupplier =
								supplierConstruction.position().euclidianDistance( candidatePod.position() );

						totalDistance += podDistanceFromThisSupplier;
						if( podDistanceFromThisSupplier < 10000 )
							tooFarFromAllSuppliers = false;
					}
				}

				if( (not tooFarFromAllSuppliers) and totalDistance < lowestDistanceFound )
				{
					lowestDistanceFound = totalDistance;
					found = true;
					internalSetSmeltingBuilding( &candidatePod );
				}
			}
		}
	}

	// give voicemail if we used to have a smelter, and we also have one now, but it's now a different one.
	if( alreadyHadSmeltingBuilding
		and hasSmeltingBuilding()
		and pOldSmeltingBuilding != pDestinationSmeltingBuilding_ )
	{
		giveAlternativeDestinationVoicemail();
	}

	smeltingBuildingExplicitlyAssigned_ = false;

	return found;
}

void MachLogResourceCarrier::reorderTransportRoute()
{
	CB_MachLogResourceCarrier_DEPIMPL();

	PRE( hasSuppliers() );
	PRE( hasSmeltingBuilding() );

	// abort reordering if defcon is set to HIGH. This allows the player the option of
	// explicitly setting the visiting order if that is desired.
	if( defCon() == MachLog::DEFCON_HIGH )
		return;

	// heh heh....this is great.
	// essentially, just run a simple "nearest first" heuristic, from the smelter to the nearest supplier,
	// that supplier to ITS nearest supplier etc.
	// In many ways, this is actually BETTER than a proper optimal TSP solution, because the carrier will
	// quite possibly be full up long before it finished its route, and nearest-first will outperform optimal TSP
	// in most of these cases

	Suppliers reorderedSuppliers;

	int nSuppliers = suppliers_.size();

	MexPoint2d currentCheckPosition = pDestinationSmeltingBuilding_->position();

	MachLogConstruction* pCurrentSupplier = NULL;

	bool dealingWithSupplier = not doneAllSuppliers();

	if( dealingWithSupplier )
		// store a pointer to the actor our current supplier index points to.
		pCurrentSupplier = &currentSupplier();

	while( reorderedSuppliers.size() < nSuppliers )
	{
		MATHEX_SCALAR lowestDistanceFound = 100000000;

		Suppliers::iterator iNextClosestSupplier = suppliers_.end();

		for( Suppliers::iterator iSup = suppliers_.begin(); iSup != suppliers_.end(); ++iSup )
		{
			MATHEX_SCALAR sqrDistanceFromCurrentCheckPosition =
				currentCheckPosition.sqrEuclidianDistance( (*iSup)->position() );

			if( sqrDistanceFromCurrentCheckPosition < lowestDistanceFound )
			{
				iNextClosestSupplier = iSup;
				lowestDistanceFound = sqrDistanceFromCurrentCheckPosition;
			}
		}

		ASSERT( iNextClosestSupplier != suppliers_.end(), "Didn't manage to find a next nearest supplier!" );

		currentCheckPosition = (*iNextClosestSupplier)->position();
		reorderedSuppliers.push_back( *iNextClosestSupplier );
		suppliers_.erase( iNextClosestSupplier );
	}

	ASSERT( suppliers_.empty(), "The original suppliers_ list should be empty by now." );

	// now need to rebuild our suppliers_ container in the new order.

	for( Suppliers::iterator iReordered = reorderedSuppliers.begin(); iReordered != reorderedSuppliers.end(); ++iReordered )
	{
		suppliers_.push_back( *iReordered );
	}

	// note that we will reset our current supplier index to match our stored current supplier
	// pointer's new position in the shuffled supplers_ container (provided we actually HAVE a current
	// supplier we're actively dealing with).
	if( dealingWithSupplier )
		reassignIndex( pCurrentSupplier );

	POST( suppliers_.size() == nSuppliers );
}

void MachLogResourceCarrier::informOfNewSmeltingBuildingCompletion()
{
	CB_MachLogResourceCarrier_DEPIMPL();

	if( not smeltingBuildingExplicitlyAssigned_ and hasSuppliers() )
		findBestSmeltingBuilding();
}


/* //////////////////////////////////////////////////////////////// */

// virtual
void MachLogResourceCarrier::doOutputOperator( ostream& o ) const
{
	CB_MachLogResourceCarrier_DEPIMPL();

	o << "Amount Carried " << amountCarried() << std::endl;
	o << "Capacity " << data().capacity() << std::endl;
	if( isTransporting() )
		o << "Is transporting" << std::endl;
	else
		o << "Not transporting" << std::endl;
	if( isPickingUp() )
	{
		o << "Is picking up" << std::endl;
		o << " current element " << currentElement_ << std::endl;
	}

	if( isPuttingDown() )
		o << "Is putting down" << std::endl;

	o << " nSuppliers " << nSuppliers() << std::endl;

	if( hasSmeltingBuilding() )
		o << " SmeltingBuilding id: " << pDestinationSmeltingBuilding_->id() << std::endl;
	else
		o << " No smelter assigned." << std::endl;

	if( isScavenger() )
		o << " Debris sites on planet = " << MachLogPlanet::instance().debrisSites().size() << std::endl;


	MachLogMachine::doOutputOperator( o );
}

////////////////////////////////////////////////////////////////////

bool MachLogResourceCarrier::isScavenger() const
{
	return( hwLevel() == 3 );
}

bool MachLogResourceCarrier::isNormalResourceCarrier() const
{
	return not isScavenger();
}


/* //////////////////////////////////////////////////////////////// */
//static
MachPhysResourceCarrier* MachLogResourceCarrier::pNewPhysResourceCarrier
(
    Level hwLevel, Level swLevel, MachLogRace * pRace, const MexPoint3d& location
)
{
    //get domain and transform to use
    MexTransform3d localTransform;
    W4dDomain* pDomain =
        MachLogPlanetDomains::pDomainPosition( location, 0, &localTransform );

    //Construct the physical machine
    return _NEW( MachPhysResourceCarrier( pDomain, localTransform, hwLevel, swLevel, pRace->race() ) );
}
//virtual
const MachPhysMachineData& MachLogResourceCarrier::machineData() const
{
	return data();
}

const MachPhysResourceCarrierData& MachLogResourceCarrier::data() const
{
    // TODO check
	//return  _STATIC_CAST( MachPhysResourceCarrierData, physMachine().machineData() );
	return  _STATIC_CAST( const MachPhysResourceCarrierData&, physMachine().machineData() );
	//return  _REINTERPRET_CAST( MachPhysResourceCarrierData&,
      //  _CONST_CAST(MachPhysMachineData&, physMachine().machineData()) );
}
/* //////////////////////////////////////////////////////////////// */

MachPhys::BuildingMaterialUnits
MachLogResourceCarrier::amountCarried() const
{
	CB_MachLogResourceCarrier_DEPIMPL();
	return amountCarried_;
}

MachPhys::BuildingMaterialUnits&
MachLogResourceCarrier::amountCarried()
{
	CB_MachLogResourceCarrier_DEPIMPL();

	return amountCarried_;
}

void MachLogResourceCarrier::iterations( int i )
{
	CB_MachLogResourceCarrier_DEPIMPL();

	iterations_ = i;
}

int MachLogResourceCarrier::iterations() const
{
	CB_MachLogResourceCarrier_DEPIMPL();

	return iterations_;
}

bool MachLogResourceCarrier::isTransporting() const
{
	CB_MachLogResourceCarrier_DEPIMPL();

	return( isTransporting_ );
}

void MachLogResourceCarrier::addedTransportOp()
{
	CB_MachLogResourceCarrier_DEPIMPL();

	PRE( not isTransporting_ );

	isTransporting_ = true;
}

void MachLogResourceCarrier::removedTransportOp()
{
	CB_MachLogResourceCarrier_DEPIMPL();

	PRE( isTransporting_ );

	isTransporting_ = false;

	if( not	protectNodesFromClearance_ )
		clearSuppliersAndSmeltingBuilding();
}

bool MachLogResourceCarrier::isPickingUp() const
{
	CB_MachLogResourceCarrier_DEPIMPL();

	return( isPickingUp_ );
}

void MachLogResourceCarrier::addedPickUpOp()
{
	CB_MachLogResourceCarrier_DEPIMPL();

	PRE( not isPickingUp_ );

	isPickingUp_ = true;
}

void MachLogResourceCarrier::removedPickUpOp()
{
	CB_MachLogResourceCarrier_DEPIMPL();

	PRE( isPickingUp_ );

	isPickingUp_ = false;
}

bool MachLogResourceCarrier::isPuttingDown() const
{
	CB_MachLogResourceCarrier_DEPIMPL();

	return( isPuttingDown_ );
}

void MachLogResourceCarrier::addedPutDownOp()
{
	CB_MachLogResourceCarrier_DEPIMPL();

	PRE( not isPuttingDown_ );

	isPuttingDown_ = true;
}

void MachLogResourceCarrier::removedPutDownOp()
{
	CB_MachLogResourceCarrier_DEPIMPL();

	PRE( isPuttingDown_ );

	isPuttingDown_ = false;
}

//static
void MachLogResourceCarrier::assignResourceCarrierTask( MachLogResourceCarrier* obj )
{
	bool failedToAssign = true;

	MachLogRaces& races = MachLogRaces::instance();

	if( obj->isScavenger() )
	{
		// do nothing......(may change this later)
	}
	else
	{
		// must be a normal transporter....try to assign a transport task.

		const MachPhys::Race race = obj->race();
		MachLogRaces::Mines& mines = races.mines( race );
		MachLogRaces::Smelters& smelters = races.smelters( race );
		MachLogRaces::Pods& pods = races.pods( race );

		int nMyTotalMines = mines.size();
		if( nMyTotalMines > 0 and ( smelters.size() > 0 or pods.size() > 0 ) )
		{
			// which mines are worth visiting?

			//check for complete mines.
			Suppliers poolOfWorthwhileMines;
			poolOfWorthwhileMines.reserve( nMyTotalMines );
			MachLogRaces::Mines::const_iterator i = mines.begin();
			for( ; i != mines.end(); ++i )
			{
				MachLogMine* pCandidateMine = (*i);
				if( pCandidateMine->worthVisiting() )
				{
					poolOfWorthwhileMines.push_back( pCandidateMine );
				}
			}
			int nMyWorthwhileMines = poolOfWorthwhileMines.size();

   			if( nMyWorthwhileMines > 0 )
			{
				// right, we need to select a subset of these mines. Let's try going for, I dunno, roughly a third of them.
				int nMinesToVisit = ( nMyWorthwhileMines / 3 ) + 1;		// guaranteed at least 1 mine.

				Suppliers approvedItineraryList;
				approvedItineraryList.reserve( nMinesToVisit );

				Suppliers::iterator iFirst = poolOfWorthwhileMines.end();		// "null" assignment

				PhysAbsoluteTime timeNow = SimManager::instance().currentTime();

				if( timeNow <= 300.0 )
				{
					// if this is within the first 5 minutes of the game, always select the mine you're closest to
					// as the first of your chosen group (this ensures carriers placed in specific locations at the
					// start of a scenario will always choose the nearest mine.....for a while)

					iFirst = MachLogResourceCarrierImpl::iNearestSupplier( poolOfWorthwhileMines, obj->position() );
				}
				else
				{
					// select one of them at random to be our first mine.

					int firstMineIndex = MachPhysRandom::randomInt( 0, nMyWorthwhileMines );

					iFirst = poolOfWorthwhileMines.begin() + firstMineIndex;
				}

				ASSERT( iFirst != poolOfWorthwhileMines.end(), "No worthwhile mine was found." );

				// that one becomes the first mine in our itinerary, and is removed from the pool of remaining candidates
			   	approvedItineraryList.push_back( *iFirst );
				poolOfWorthwhileMines.erase( iFirst );

				// okay, now choose the remaining mines (if any) on a "nearest best" basis.

				MexPoint2d currentCheckPosition = (*iFirst)->position();

				while( approvedItineraryList.size() < nMinesToVisit )
				{
					MATHEX_SCALAR lowestDistanceFound = 100000000;

					Suppliers::iterator iNextClosestSupplier = poolOfWorthwhileMines.end();

					for( Suppliers::iterator iSup = poolOfWorthwhileMines.begin(); iSup != poolOfWorthwhileMines.end(); ++iSup )
					{
						MATHEX_SCALAR sqrDistanceFromCurrentCheckPosition =
							currentCheckPosition.sqrEuclidianDistance( (*iSup)->position() );

						if( sqrDistanceFromCurrentCheckPosition < lowestDistanceFound )
						{
							iNextClosestSupplier = iSup;
							lowestDistanceFound = sqrDistanceFromCurrentCheckPosition;
						}
					}

					ASSERT( iNextClosestSupplier != poolOfWorthwhileMines.end(), "Didn't manage to find a next nearest supplier!" );

					// add this mine to our itinerary, and remove it from the pool of remaining candidates
					MachLogConstruction* pNextClosestSupplier = (*iNextClosestSupplier);
					currentCheckPosition = pNextClosestSupplier->position();
					approvedItineraryList.push_back( pNextClosestSupplier );
					poolOfWorthwhileMines.erase( iNextClosestSupplier );
				}

				// okay, the itinerary is complete - now need to set it as the suppliers list for the resource carrier
				obj->clearSmeltingBuilding();

				// note that this call will now automatically assign the nearest smelter as default.
			   	obj->setSuppliers( approvedItineraryList );

				// do anything between 5 and 10 complete transport circuits before choosing a new route
				int circuitsToComplete = 5 + MachPhysRandom::randomInt( 0, 6 );
				obj->iterations( circuitsToComplete );

				failedToAssign = false;
			}
		}

		// if there aren't at least one mine and one smelter left, there may at least be one or the other.
		// if so, it's still worth doing a one-shot pick-up or put-down if possible and applicable.
		if( failedToAssign and ( nMyTotalMines > 0 or ( smelters.size() > 0 or pods.size() > 0 ) ) )
		{
			//carrying anything?
			if( obj->amountCarried() == 0 )
			{
				//Nope..find a mine with some ore and go and get it.
				if( nMyTotalMines > 0 )
				{
					MATHEX_SCALAR closestPointDistance=1000000000;
					MATHEX_SCALAR testPointDistance;

					Suppliers poolOfWorthwhileMines;
					MachLogMine* pNearestMine = NULL;
					bool found = false;

					for( MachLogRaces::Mines::const_iterator i = mines.begin(); i != mines.end(); ++i )
					{
						MachLogMine* pCandidateMine = (*i);

						if( pCandidateMine->worthVisiting() )
						{
							testPointDistance = obj->position().sqrEuclidianDistance( pCandidateMine->position() ) < closestPointDistance;
							if( testPointDistance < closestPointDistance )
							{
								closestPointDistance = testPointDistance;
								pNearestMine = pCandidateMine;
								found = true;
							}
						}
					}

					if( found )
					{
						poolOfWorthwhileMines.push_back( pNearestMine );
						obj->setSuppliers( poolOfWorthwhileMines );
						obj->newOperation( _NEW( MachLogPickUpOperation( obj ) ) );
						failedToAssign = false;
					}
				}
			}
			else
			{
				//carrying something go and drop it off at a smelter, so long as smelters aren't full.
				if( races.nBuildingMaterialUnits( race ) < races.nMaxBuildingMaterialUnits( race )
					and ( smelters.size() > 0 or pods.size() > 0 ) )
				{
					obj->clearSuppliers();
					obj->findBestSmeltingBuilding();

					failedToAssign = false;
				}
			}
		}
		// END if( failedToAssign and ( nMyTotalMines > 0 or ( smelters.size() > 0 or pods.size() > 0 ) ) )
	}

	//scavengers won't do anything.
	if( failedToAssign and not obj->isScavenger() )
	{
		HAL_STREAM("still failedToAssign so issuing move out of the way\n" );
		races.controller( obj->race() ).moveOutOfTheWay( obj );
	}
}

//virtual
bool MachLogResourceCarrier::beNotified( W4dSubject* pSubject,
                 W4dSubject::NotificationEvent event, int clientData )
{

	CB_MachLogResourceCarrier_DEPIMPL();

	bool stayAttached = MachLogMachine::beNotified( pSubject, event, clientData );

	switch( event )
	{
	case W4dSubject::DELETED:
		stayAttached = mayHaveLostRouteElement( pSubject );
		break;

	case W4dSubject::CLIENT_SPECIFIC:
		switch( clientData )
		{
			case MachLog::RACE_CHANGED:
			{
				ASSERT( _STATIC_CAST( MachActor*, pSubject )->objectIsConstruction(), "pSubject was unexpectedly not a construction!" );
				MachLogConstruction& construction = _STATIC_CAST( MachActor*, pSubject )->asConstruction();
				if( construction.race() != race() )
				{
					stayAttached = mayHaveLostRouteElement( pSubject );
				}
				else
				{
					// it is now our building, and used to be someone else's.
					// If it is a mine, nae bother - no problem in continuing to use it as a supplier.
					// But if it is a pod or smelter, we should not use it as a supplier as we'd only be stealing
					// from ourselves - it should now be removed from the list of suppliers.
					if( construction.objectIsCanSmelt() )
					{
						stayAttached = mayHaveLostRouteElement( pSubject );
					}
				}
			}
			break;

			case MachLog::MINE_CHANGED_OPERATIONAL_STATUS:
			{
				Suppliers::iterator i = find( suppliers_.begin(), suppliers_.end(), pSubject );

				bool found = i != suppliers_.end();

				if( found )
				{
					ASSERT( (*i)->objectType() == MachLog::MINE, "Received MINE_CHANGED_OPERATIONAL_STATUS message from a non-mine actor." );

					// We will want to re-evaluate the best smelter (if possible) in light of the changed status of
					// this mine.

					if( not smeltingBuildingExplicitlyAssigned_ )
							findBestSmeltingBuilding();
				}
			}
			break;

			default:
				;
		}
	default:
		;
	}
	////HAL_STREAM(" returning true\n" );
	return stayAttached;
}

void MachLogResourceCarrier::reassignIndex( MachLogConstruction* pSupplier )
{
	CB_MachLogResourceCarrier_DEPIMPL();

	PRE( pSupplier );

	size_t newIndex = 0;

	bool found = 0;

	for( Suppliers::iterator i = suppliers_.begin(); not found and i != suppliers_.end(); ++i )
	{
		if( (*i) == pSupplier )
			currentSupplierIndex( newIndex );

		++newIndex;
	}
}

bool MachLogResourceCarrier::doneAllSuppliers()
{
	CB_MachLogResourceCarrier_DEPIMPL();

	return currentSupplierIndex() >= suppliers_.size();
}

MachLogConstruction& MachLogResourceCarrier::currentSupplier()
{
	CB_MachLogResourceCarrier_DEPIMPL();

	PRE( not doneAllSuppliers() );

	return *suppliers_[ currentSupplierIndex() ];
}

//virtual
PhysRelativeTime MachLogResourceCarrier::update( const PhysRelativeTime& maxCPUTime,
                                     MATHEX_SCALAR clearance )
{
	// NOTE this update is only of genuine relevence to the scavenger type.

	CB_MachLogResourceCarrier_DEPIMPL();

	PhysRelativeTime result = MachLogMachine::update( maxCPUTime, clearance );

	if( isScavenger() )
	{
		// scavengers get guaranteed fast-ish callback times
		result = std::min( 1.5, result );

		if(	virtualDefCon() != MachLog::DEFCON_HIGH
			and not evading()
			and not isStandingGround()
			and strategy().currentOperationType() != MachLogOperation::SCAVENGE_OPERATION
			and strategy().currentOperationType() != MachLogOperation::AUTOSCAVENGE_OPERATION )
			/*and MachPhysRandom::randomInt( 0, 3 ) == 0
			and( isIdle()
				 or
				  ( virtualDefCon() == MachLog::DEFCON_LOW
					and strategy().currentOperationType() != MachLogOperation::SCAVENGE_OPERATION
					and strategy().currentOperationType() != MachLogOperation::AUTOSCAVENGE_OPERATION
			 	  )
			    )
			)
			*/
		{
			// we may want to do some scavenger-specific pro-active scavenging stuff.

			// initialised value is that of DefCon NORMAL state.
			MATHEX_SCALAR scavengingRange = 80;
			if( defCon() == MachLog::DEFCON_LOW )
				scavengingRange = 140;

			MATHEX_SCALAR sqrScavengingRange = sqr( scavengingRange );
			// let's take a shoofty about and see if there's any debris in range to go and pick up.

			MATHEX_SCALAR sqrClosestFoundSoFar = 99999999;
			MachLogDebris* pChosenDebris = NULL;
			const MachLogPlanet::DebrisSites& sites = MachLogPlanet::instance().debrisSites();

			bool found = false;

			for( MachLogPlanet::DebrisSites::const_iterator i = sites.begin();
													  		i != sites.end();
															++i )
			{
				MachLogDebris* pCandidateDebris = (*i);
				MexPoint2d candidateDebrisPosition = pCandidateDebris->position();
				MATHEX_SCALAR sqrDistanceToCandidate = position().sqrEuclidianDistance( candidateDebrisPosition );
				if( sqrDistanceToCandidate < sqrScavengingRange
					and sqrDistanceToCandidate < sqrClosestFoundSoFar
					and MachLogSpacialManipulation::pointIsFree( candidateDebrisPosition, 2.0 ) )
				{
					sqrClosestFoundSoFar = sqrDistanceToCandidate;
					pChosenDebris = pCandidateDebris;
					found = true;
				}
			}

			if( found and not strategy().isUninterruptable() )
			{
				// let's go and scavenge the fella!
				strategy().changeToScavengeMode( pChosenDebris );
				result = 0.0;
			}
		}
	}


	return result;
}

//////////////////////////////////////////////// persistence ////////////////////////////////////////////////

void perWrite( PerOstream& ostr, const MachLogResourceCarrier& actor )
{
	const MachLogMachine& base1 = actor;

	ostr << base1;
	ostr << actor.pImpl_;

	MexTransform3d trans = actor.globalTransform();

    //If we don't do this, the large coords of the transform cause are used when read back in to find
    //a planet domain, which of course falls over big time.
    if( actor.insideAPC() )
        trans.position( MexPoint3d(10,10,0) );

	PER_WRITE_RAW_OBJECT( ostr, trans );
	const W4dId id = actor.id();
	PER_WRITE_RAW_OBJECT( ostr, id );
	actor.persistenceWriteStrategy( ostr );
}

void perRead( PerIstream& istr, MachLogResourceCarrier& actor )
{
	MachLogMachine& base1 = actor;

	istr >> base1;
	istr >> actor.pImpl_;

	MexTransform3d trans;
	W4dId id;
	PER_READ_RAW_OBJECT( istr, trans );
	PER_READ_RAW_OBJECT( istr, id );

	MachPhysResourceCarrier* pPhysResourceCarrier = MachLogResourceCarrier::pNewPhysResourceCarrier( base1.hwLevel(), base1.swLevel(), &actor.logRace(), trans.position() );
	actor.setObjectPtr( pPhysResourceCarrier, trans );
	actor.id( id );
	actor.machineCreated();
	actor.persistenceReadStrategy( istr );

	if( actor.hasSmeltingBuilding() )
		actor.pDestinationSmeltingBuilding()->attach( &actor );

	for( MachLogResourceCarrier::Suppliers::iterator i = actor.suppliers().begin(); i!= actor.suppliers().end(); ++i )
		(*i)->attach( &actor );
}

MachLogResourceCarrier::MachLogResourceCarrier( PerConstructor con )
:	MachLogMachine( con )
{
}


MachPhysResourceCarrier& MachLogResourceCarrier::physResourceCarrier()
{
	return _STATIC_CAST( MachPhysResourceCarrier&, physObject() );
}

const MachPhysResourceCarrier& MachLogResourceCarrier::physResourceCarrier() const
{
	return _STATIC_CAST( const MachPhysResourceCarrier&, physObject() );
}

PhysRelativeTime MachLogResourceCarrier::doLoading()
{
	CB_MachLogResourceCarrier_DEPIMPL();

	PhysRelativeTime interval;

	W4dEntity& physObj = physObject();
	W4dSoundManager::instance().play(&physObj, SID_RESOURCE_PICKUP, PhysAbsoluteTime(0), 1);

	if( isScavenger() )
		interval = physResourceCarrier().doScavenge( SimManager::instance().currentTime() );
	else
		interval = physResourceCarrier().doLoading( SimManager::instance().currentTime() );

	MachLogNetwork& network = MachLogNetwork::instance();
	if( network.isNetworkGame() and network.remoteStatus( race() ) == MachLogNetwork::LOCAL_PROCESS )
		network.messageBroker().sendPlayNormalObjectAnimationMessage( id(), true );

	notifyObservers( W4dSubject::CLIENT_SPECIFIC, MachLog::CHANGED_MINERALS_CARRIED );

	return interval;
}

bool MachLogResourceCarrier::mayHaveLostRouteElement( W4dSubject* pSubject )
{
	CB_MachLogResourceCarrier_DEPIMPL();

	bool stayAttached = true;

	if( pSubject == (W4dSubject*)pDestinationSmeltingBuilding_ )
	{
		//	T H E   S M E L T E R   H A S   B E E N    L O S T

		//smelter has been deleted...don't care for now as we will try to find a new one later.
		clearSmeltingBuilding();
		stayAttached = false;

		// if we still have at least one supplier,
		// try to run a reordering on the remaining transport nodes. If we are free
		// to assign a new smelter, try that as well as the best one may now be different
		// in light of the new distribution of suppliers.

		if( hasSuppliers() )
		{
			if( not smeltingBuildingExplicitlyAssigned_ )
				findBestSmeltingBuilding();

			if( hasSmeltingBuilding() )
			{
				reorderTransportRoute();
				// we found an alternative smelter, so announce this to the user.
				giveAlternativeDestinationVoicemail();
			}

		}

		// stop in our tracks if we now have no smelter and we were previously moving toward it
		// (UNLESS we are evading)
		if( not( hasSmeltingBuilding() ) )
		{
			giveTransportRouteFailureVoicemail();

			if( isPuttingDown() and not evading() )
			{
				// this will protect all our supplier and smelter lists from being wiped out as a result
				// of the removal of the transport op (as would normally be the case)
				protectNodesFromClearance_ = true;

				strategy().tryToRemoveAllOperations();

				protectNodesFromClearance_ = false;
			}
		}
	}
	else
	{
		//	O N E   O F   O U R   S U P P L I E R S   M A Y    H A V E   B E E N    D E L E T E D

		Suppliers::iterator i = find( suppliers_.begin(), suppliers_.end(), pSubject );

		bool found = i != suppliers_.end();

		if( found )
		{
			MachLogConstruction& lostSupplier = (**i);
			lostSupplier.removeThreat( id() );
			suppliers_.erase( i );
			stayAttached = false;

			// if we still have at least one supplier,
			// try to run a reordering on the remaining transport nodes. If we are free
			// to assign a new smelter, try that as well as the best one may now be different
			// in light of the new distribution of suppliers.

			if( hasSuppliers() )
			{
				if( not smeltingBuildingExplicitlyAssigned_ )
					findBestSmeltingBuilding();

				if( hasSmeltingBuilding() )
						reorderTransportRoute();
			}
			else
				giveTransportRouteFailureVoicemail();

			if( not( hasSuppliers() ) or doneAllSuppliers() )
			{
				// nothing more remains to be picked up - stop dead in our tracks. Reissue transport op
				// if we do still have suppliers left - this will flip round to a putdown immediately
				// as we will now have completed pickup from all remaining suppliers.
				// Note that none of this is relevant if we are currently evading - don't want to interfere
				// with that.
				if( isPickingUp() and not evading() )
				{
					// this will protect all our supplier and smelter lists from being wiped out as a result
					// of the removal of the transport op (as would normally be the case)
					protectNodesFromClearance_ = true;

					strategy().tryToRemoveAllOperations();

					protectNodesFromClearance_ = false;

					if( hasSuppliers() )
						mayNeedNewOp( START_WITH_PICKUP );
				}

			}
		}
	}

	return stayAttached;
}

void MachLogResourceCarrier::giveAlternativeDestinationVoicemail() const
{
	MachLogVoiceMailManager::instance().postNewMail( VID_RESOURCE_CARRIER_ALTERNATIVE_DESTINATION, id(), race() );
}

void MachLogResourceCarrier::giveTransportRouteFailureVoicemail() const
{
	switch( MachPhysRandom::randomInt( 0, 2 ) )
	{
		case 0:
			MachLogVoiceMailManager::instance().postNewMail( VID_RESOURCE_CARRIER_ROUTE_FAILURE0, id(), race() );
			break;
		default:
			MachLogVoiceMailManager::instance().postNewMail( VID_RESOURCE_CARRIER_ROUTE_FAILURE1, id(), race() );

	}
}

int MachLogResourceCarrier::nSuppliers() const
{
	CB_MachLogResourceCarrier_DEPIMPL();

	return suppliers_.size();
}


// Forced recompile 19/2/99 CPS
/* End RESCARR.CPP *************************************************/
