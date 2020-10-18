/*
 * O P P I C K U P . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/oppickup.hpp"
#include "machlog/internal/oppickui.hpp"

#include "phys/motchunk.hpp"

#include "machphys/mcmovinf.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/random.hpp"
#include "machphys/rcardata.hpp"
#include "machphys/rescarr.hpp"
#include "machphys/snddata.hpp"

#include "machlog/mcmotseq.hpp"
#include "machlog/mine.hpp"
#include "machlog/move.hpp"
#include "machlog/races.hpp"
#include "machlog/rescarr.hpp"
#include "machlog/smelter.hpp"
#include "machlog/spacial.hpp"

PER_DEFINE_PERSISTENT( MachLogPickUpOperation );

#define CB_MachLogPickUpOperation_DEPIMPL()	\
		PRE( pImpl_ );	\
		CB_DEPIMPL( MachLogResourceCarrier*, pActor_ ); \
    	CB_DEPIMPL( bool, finished_ ); \	
		CB_DEPIMPL( MexPoint2d, destinationPadPosition_ )

/* //////////////////////////////////////////////////////////////// */

MachLogPickUpOperation::~MachLogPickUpOperation()
{
	TEST_INVARIANT;

	CB_MachLogPickUpOperation_DEPIMPL();  

	pActor_->removedPickUpOp();

	// ensure motion sequencer is not locked
	pActor_->motionSeq().allowMoveOutOfWay( true );

	_DELETE( pImpl_ );
}

/* //////////////////////////////////////////////////////////////// */

MachLogPickUpOperation::MachLogPickUpOperation( MachLogResourceCarrier * pActor )
:	MachLogOperation( "PICKUP_OPERATION", MachLogOperation::PICKUP_OPERATION ),
	pImpl_( _NEW( MachLogPickUpOperationImpl( pActor ) ) )	
{	
	CB_MachLogPickUpOperation_DEPIMPL();

	PRE( pActor_->isNormalResourceCarrier() );	

	pActor->addedPickUpOp();
	
	TEST_INVARIANT;
 }


void MachLogPickUpOperation::CLASS_INVARIANT
{
	CB_MachLogPickUpOperation_DEPIMPL();

    INVARIANT( this != NULL );
	INVARIANT( pActor_->amountCarried() <= pActor_->data().capacity() );
}



void MachLogPickUpOperation::doOutputOperator( ostream& o ) const
{
	CB_MachLogPickUpOperation_DEPIMPL();

	o << "MachLogPickUpOperation ";
}

bool MachLogPickUpOperation::doStart()
{
	CB_MachLogPickUpOperation_DEPIMPL();

	return not checkNeedAndDoLeaveOperation( pActor_ );
}

///////////////////////////////////

void MachLogPickUpOperation::doFinish()
{
	/* Intentionally Empty	*/
}

bool MachLogPickUpOperation::doIsFinished() const
{
	CB_MachLogPickUpOperation_DEPIMPL();
	
	bool finished = ( finished_ and not pActor_->motionSeq().hasDestination() );

	if( finished )
	{
		// reset the resource carrier's index back to the first supplier in readiness for next time around
		pActor_->movingToFirstSupplier();			
	}
		

	return finished;
}

PhysRelativeTime
MachLogPickUpOperation::doUpdate()
{
	CB_MachLogPickUpOperation_DEPIMPL();

	PhysRelativeTime interval = 2.0;

	if( finished_ )
	{
		if( pSubOperation() )
			pSubOperation()->doBeInterrupted();
		return 0.0;
	}

	MachLogResourceCarrier::Suppliers& suppliers = pActor_->suppliers();		// access given via friend relationship

	// am I already full, or have I already dealt with all suppliers in my list?	
	if( pActor_->amountCarried() == pActor_->data().capacity() or pActor_->doneAllSuppliers() )
	{
 		terminateOp();
 		return 0.0;
	}

	// if this supplier is not complete, or is a mine and has no ore, attempt to reassign to a supplier 
	// that IS complete and (if a mine) has ore.
	// note that if no complete alternative is found, the carrier will be quite happy to continue
	// tootling to the incomplete mine and waiting for it to be completed.

	MachLogConstruction& tempCurrentSupplier = pActor_->currentSupplier();

	if( not( tempCurrentSupplier.isComplete() )
		or( tempCurrentSupplier.objectType() == MachLog::MINE
			and tempCurrentSupplier.asMine().ore() == 0 ) )
	{
		size_t testElement = pActor_->currentSupplierIndex();
		bool found = false;
		while( not found and testElement < suppliers.size() )
		{
			if( suppliers[ testElement ]->isComplete() 
				and( suppliers[ testElement ]->objectType() != MachLog::MINE
				  	 or suppliers[ testElement ]->asMine().ore() > 0 ) )
			{
				// note that we have to actually set our resource carrier's index here, rather
				// than merely our local copy.
				pActor_->currentSupplierIndex( testElement );
				found = true;	
				
				// we might have been bimbling towards an incomplete mine before, but now that
				// a better alternative is ready, stop that subop in its tracks.
				if( pSubOperation() )
					pSubOperation()->doBeInterrupted();
			}
			++testElement;
		}			
		if( not found )
		{
			if( pActor_->amountCarried() > 0 and pActor_->hasSmeltingBuilding() )
			{
				// Don't bother hanging around waiting for a mine to be completed or extract ore
				// if we already have cargo to take back to our smelter.
				terminateOp();
				return 0.0;
			}
			if( not tempCurrentSupplier.asMine().hasMineralSite() )
			{
				// We'd better end the op here, otherwise we run the risk of getting stuck on later suppliers
				// even if earlier ones in the supplier list are valid.
				terminateOp();
				return 0.0;	
			}			
		}
	}

	// okay, now this ref is the one we'll deal with for the rest of the duration of this method.
	MachLogConstruction& currentSupplier = pActor_->currentSupplier();

	ASSERT_INFO( currentSupplier.objectType() );
	ASSERT_INFO( pActor_->id() );
	ASSERT ( currentSupplier.objectType() == MachLog::MINE 				
				or ( currentSupplier.objectIsCanSmelt() 
					 and currentSupplier.race() != pActor_->race() ), 
				"MachLogPickUpOperation::doUpdate : we are trying to pick up from something that is neither mine nor enemy smelter." );
	
	MATHEX_SCALAR sqrDistanceToClosestFreePad=1000000000;		//used in cases of mines and smelters to determine which loading bay is closest
	MATHEX_SCALAR sqrDistanceToTestPad;

	MexPoint2d actorPos = pActor_->position();	

	MATHEX_SCALAR carrierClearance = pActor_->highClearence()*1.5; 
		
	MATHEX_SCALAR sqrDropOffDistance = sqr( carrierClearance );

	if( currentSupplier.objectType() == MachLog::MINE )
	{
		// 		=========== P I C K I N G   U P   F R O M   M I N E ===========
		
		MachLogMine& mine =  currentSupplier.asMine();
		size_t bestPointNum = 0;
		bool found = false;
		MexPoint2d oldDestinationPadPosition = destinationPadPosition_;

		for( size_t pointNum = 0; pointNum < mine.nPickUpPoints(); ++pointNum )
		{
			MexPoint2d testDest = mine.pickUpPoint( pointNum );
			sqrDistanceToTestPad = actorPos.sqrEuclidianDistance( testDest );
			
			if( sqrDistanceToTestPad > sqr( 45 )
			    or sqrDistanceToTestPad <= sqrDropOffDistance // if we're that close, the thing blocking the pad will be us!
		 	    or( MachLogSpacialManipulation::pointIsFree( testDest, 5.0 ) 
					and not( mine.pickupPointUsedRecently( pointNum ) ) )
			   )
			{

				if( testDest == oldDestinationPadPosition )
				{
					// if my current destination is still free, choose that in preference to any others, even if they
					// are nearer.
					destinationPadPosition_ = oldDestinationPadPosition;
					found = true;
					bestPointNum = pointNum;
					break;				}	
				else if( sqrDistanceToTestPad <= sqrDistanceToClosestFreePad )	
				{
					destinationPadPosition_ = testDest;
					sqrDistanceToClosestFreePad = sqrDistanceToTestPad;
					found = true;
					bestPointNum = pointNum;
				} 	   	
			}
		}

		if( not found )
		{
			// no points are free - just choose a point at random.
			destinationPadPosition_ = mine.pickUpPoint( MachPhysRandom::randomInt( 0, _STATIC_CAST( int, mine.nPickUpPoints() ) ) );
		}		

		//are we at the mine yet?

		MATHEX_SCALAR sqrDistanceFromMine = actorPos.sqrEuclidianDistance( destinationPadPosition_ );

		if( sqrDistanceFromMine > sqrDropOffDistance )
		{
			// not at the mine.

			// ensure motion sequencer is not locked
			pActor_->motionSeq().allowMoveOutOfWay( true );

			if( destinationPadPosition_ != oldDestinationPadPosition )
			{
				//our old destination is now a bunch of old trash. Cancel our current subop so we can set up a
				// nice new one
				if( pSubOperation() )
				{
					pSubOperation()->doBeInterrupted();
					pActor_->motionSeq().stop()	;
				}
			}

			if( not pSubOperation() )
			{
				// need to initiate a move to the pad.

				MexPoint2d clearPickupPoint = destinationPadPosition_;

				// okay, now we need to try and ensure the destination point is in clear space (especially for those 
				// horrible big transporters)
				attemptToGuaranteeClearPickupPoint( actorPos, carrierClearance, &clearPickupPoint );	

				subOperation( pActor_, _NEW( MachLogMoveToOperation( pActor_, clearPickupPoint ) ) );
				interval = 0.0;
			}				
		}
		else
		{
			// yes, we're at the mine.

			// lock the motion sequencer right now.
			pActor_->motionSeq().allowMoveOutOfWay( false );

			
			// okay, we're pretty much on top of the pad. Notify the mine that we've used its pad.
			mine.usedPickupPoint( bestPointNum );			

			//mine must be complete before we pick up from it. (ore should be zero any way but this is still a valid check)
			if( not mine.isComplete() )
				return 5.0;

			//Yes...Note that if the mine has no ore ready then we will wait until mine.ore > 0
			if( mine.ore() > 0 )
			{
				interval = pActor_->doLoading();			
			
				MachPhys::BuildingMaterialUnits amountToAdd = pActor_->data().capacity() - pActor_->amountCarried();

				amountToAdd = mine.removeOre( amountToAdd );

				MachLogRaces::instance().nOre( pActor_->race() ) -= amountToAdd;
				pActor_->amountCarried() += amountToAdd;
				
				// finished with this mine go onto the next element if possible 
				if( not moveToNextSupplier() )
				{
					terminateOp();
				}
			}
		}
	}	
	else
	{
		// 		=========== P I C K I N G   U P   F R O M   S M E L T E R / P O D ===========
				
		ASSERT_INFO( currentSupplier.objectType() );
		ASSERT( currentSupplier.objectIsCanSmelt(), "Expected smelter or pod type" );

		MachLogConstruction& smeltingBuilding =  currentSupplier.asSmelter();		
				
		//are we at the smeltingBuilding yet?
		size_t bestPointNum = 0;		
		bool found = false;
		for( size_t pointNum = 0; pointNum < smeltingBuilding.nPutDownPoints(); ++pointNum )
		{
			sqrDistanceToTestPad = actorPos.sqrEuclidianDistance( MexPoint2d( smeltingBuilding.putDownPoint( pointNum ) ) );
			if (  sqrDistanceToTestPad <= sqrDistanceToClosestFreePad /*and not smeltingBuilding.pickupPointUsedRecently( pointNum )*/ )
			{
				destinationPadPosition_ = smeltingBuilding.putDownPoint( pointNum );
				sqrDistanceToClosestFreePad = sqrDistanceToTestPad;
				found = true;
				bestPointNum = pointNum;
			}
		}

		ASSERT( found, "No pickup point was found!" );
		
		//are we at the smeltingBuilding yet?
		if( actorPos.sqrEuclidianDistance( destinationPadPosition_ ) > sqrDropOffDistance )
		{
			// not at the smeltingBuilding
			
			if( pSubOperation() )
			{
				// already moving there - do nothing further
				interval = 2.0;
			}
			else
			{
				// need to initiate a move to the pad.

				MexPoint2d clearPickupPoint = destinationPadPosition_;

				// okay, now we need to try and ensure the destination point is in clear space (especially for those 
				// horrible big transporters)
				attemptToGuaranteeClearPickupPoint( actorPos, carrierClearance, &clearPickupPoint );	

				// ensure motion sequencer is not locked
				pActor_->motionSeq().allowMoveOutOfWay( true );

				subOperation( pActor_, _NEW( MachLogMoveToOperation( pActor_, clearPickupPoint ) ) );
				interval = 0.0;	
			}
		}
		else
		{
			// yes, we're at the smelter.

			// lock the motion sequencer right now.
			pActor_->motionSeq().allowMoveOutOfWay( false );


			// okay, we're pretty much on top of the pad. Notify the smeltingBuilding that we've used its pad.
			smeltingBuilding.usedPickupPoint( bestPointNum );

			//enemy smeltingBuildings must be complete before we pick up from them.
			if( not smeltingBuilding.isComplete() )
				return 5.0;

				
			// steal as much as we possibly can	
			MachPhys::BuildingMaterialUnits amountToSteal = pActor_->data().capacity() - pActor_->amountCarried();
			amountToSteal = MachLogRaces::instance().smartSubtractBMUs( smeltingBuilding.race(), amountToSteal ); 
		   
		   	pActor_->amountCarried() += amountToSteal;
			if( pActor_->amountCarried() > 0 )
			{
				interval = pActor_->doLoading();									
			}
			//finished with this smeltingBuilding go onto the next element if possible 
			if( not moveToNextSupplier() )
			{
				terminateOp();
			}
		}
	}

	TEST_INVARIANT;
	return interval;
}

void MachLogPickUpOperation::terminateOp()
{
	CB_MachLogPickUpOperation_DEPIMPL();

	finished_ = true;
	// clear any sub op
	if( pSubOperation() )
		pSubOperation()->doBeInterrupted();

	pActor_->movingToFirstSupplier();	
}

//virtual
bool MachLogPickUpOperation::doBeInterrupted()
{
	return true;
}

bool MachLogPickUpOperation::moveToNextSupplier()
{
	CB_MachLogPickUpOperation_DEPIMPL();

	return pActor_->movingToNextSupplier();
}




/////////////////////////////////////////////////// persistence /////////////////////////////////////////////////////

void perWrite( PerOstream& ostr, const MachLogPickUpOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pImpl_;	
}

void perRead( PerIstream& istr, MachLogPickUpOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pImpl_;
}

MachLogPickUpOperation::MachLogPickUpOperation( PerConstructor con )
:	MachLogOperation( con )
{
}

bool MachLogPickUpOperation::attemptToGuaranteeClearPickupPoint( const MexPoint2d& actorPos, MATHEX_SCALAR carrierClearance, MexPoint2d* pClearPickupPoint )
{
	CB_MachLogPickUpOperation_DEPIMPL();

	const MexTransform3d testTransform = 
      ( actorPos == *pClearPickupPoint ) ?
      MexTransform3d( actorPos ) :
      MachLogSpacialManipulation::sourceToDestination3dTransform( *pClearPickupPoint, actorPos, MachLogSpacialManipulation::SOURCE_END_IS_CENTRE );

	const bool foundSpace = MachLogSpacialManipulation::getNearestFreeSpacePoint( testTransform, 0.0, carrierClearance, pClearPickupPoint, carrierClearance );

	return foundSpace;
}

/* //////////////////////////////////////////////////////////////// */

/* End OPPICKUP.CPP *************************************************/
