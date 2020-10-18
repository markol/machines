/*
 * O P P U T D W N . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <string>
#include "machlog/opputdwn.hpp"

#include "machlog/internal/opputdwi.hpp"

#include "ctl/list.hpp"

#include "phys/motchunk.hpp"

#include "machphys/mcmovinf.hpp"

#include "machphys/random.hpp"
#include "machphys/rescarr.hpp"

#include "machlog/mcmotseq.hpp"
#include "machlog/move.hpp"
#include "machlog/rescarr.hpp"
#include "machlog/races.hpp"
#include "machlog/smelter.hpp"
#include "machlog/spacial.hpp"

PER_DEFINE_PERSISTENT( MachLogPutDownOperation );

#define CB_MachLogPutDownOperation_DEPIMPL()	\
		PRE( pImpl_ );	\
		CB_DEPIMPL( MachLogResourceCarrier*, pActor_ ); \
    	CB_DEPIMPL( MexPoint2d, destinationPadPosition_ ); \
		CB_DEPIMPL( bool, finished_ ); 

/* //////////////////////////////////////////////////////////////// */

MachLogPutDownOperation::MachLogPutDownOperation( MachLogResourceCarrier * pActor )
:	MachLogOperation( "PUTDOWN_OPERATION", MachLogOperation::PUTDOWN_OPERATION ),
	pImpl_( _NEW( MachLogPutDownOperationImpl( pActor ) ) )	
{
	CB_MachLogPutDownOperation_DEPIMPL();

	PRE( pActor_->isNormalResourceCarrier() );

	PRE( pActor->hasSmeltingBuilding() );

	chooseMostViablePad();

	pActor->addedPutDownOp();
}

MachLogPutDownOperation::~MachLogPutDownOperation()
{
	CB_MachLogPutDownOperation_DEPIMPL();

	pActor_->removedPutDownOp();

	// ensure motion sequencer is not locked
	pActor_->motionSeq().allowMoveOutOfWay( true );

	_DELETE( pImpl_ );
}

void MachLogPutDownOperation::doOutputOperator( ostream& o ) const
{
	CB_MachLogPutDownOperation_DEPIMPL();

	o << "MachLogPutDownOperation " << std::endl;
	o << " Range to destination " << pActor_->position().euclidianDistance( destinationPadPosition_ ) << std::endl;
}

bool MachLogPutDownOperation::doStart()
{
	CB_MachLogPutDownOperation_DEPIMPL();

	return not checkNeedAndDoLeaveOperation( pActor_ );
}

///////////////////////////////////

void MachLogPutDownOperation::doFinish()
{
	/* Intentionally Empty	*/
}

bool MachLogPutDownOperation::doIsFinished() const
{
	CB_MachLogPutDownOperation_DEPIMPL();

	return ( finished_ and not pActor_->motionSeq().hasDestination() ) or ( not pActor_->hasSmeltingBuilding() );
}

PhysRelativeTime
MachLogPutDownOperation::doUpdate( )
{
	CB_MachLogPutDownOperation_DEPIMPL();

	PhysRelativeTime interval = 2.0;

	if( finished_ )
		return 0.0;

	MexPoint2d actorPos( pActor_->position() );	

	// am I still too far away from the destination to dump my ore?	
	//within 12 metres will do.

	MATHEX_SCALAR carrierClearance = pActor_->highClearence()*1.5; 
	MATHEX_SCALAR sqrDistanceFromPad = actorPos.sqrEuclidianDistance( destinationPadPosition_ );
	MATHEX_SCALAR sqrDropOffDistance = sqr( carrierClearance );

	if( sqrDistanceFromPad > sqrDropOffDistance )
	{
		// too far away to drop off.

		// ensure motion sequencer is not locked
		pActor_->motionSeq().allowMoveOutOfWay( true );

		MexPoint2d oldDestinationPadPosition = destinationPadPosition_;

		// now, if we're within 45m, we'd better make sure we're going to the nearest *unoccupied* pad

		if( sqrDistanceFromPad < sqr( 45 ) )
			possiblyChangeDestinationPad();
			
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
			attemptToGuaranteeClearPutDownPoint( actorPos, carrierClearance, &clearPickupPoint );
			
			subOperation( pActor_, _NEW( MachLogMoveToOperation( pActor_, clearPickupPoint ) ) );
			interval = 0.0;
		}
	}
	else
	{
		// okay, we're at the smelter.	

		//pSubOperation()->doBeInterrupted();
		//pActor_->motionSeq().stop()	;

		// lock the motion sequencer right now.
		pActor_->motionSeq().allowMoveOutOfWay( false );

		// is the smelter complete?

		MachLogConstruction* pDestinationSmeltingBuilding = pActor_->pDestinationSmeltingBuilding();

		if( not pDestinationSmeltingBuilding->isComplete() )
			interval = 10.0;
		else
		{
			// smelter is complete - let's try dropping off the ore.

			MachPhys::BuildingMaterialUnits amountToAdd = pActor_->amountCarried();
			amountToAdd = MachLogRaces::instance().smartAddBMUs( pActor_->race(), amountToAdd );
		  	pActor_->amountCarried() -= amountToAdd;
		  	
			//If we dropped off something and this is a genuine smelter (not a pod), tell it to do smelting animation
			if( amountToAdd > 0 )
			{
				interval = pActor_->doLoading();			

				if( pDestinationSmeltingBuilding->objectType() == MachLog::SMELTER )
					pDestinationSmeltingBuilding->asSmelter().droppedOffOre();
			}
			
			if( pSubOperation() )
				pSubOperation()->doBeInterrupted();


			// we need to register as having used this pad recently.
			for( size_t pointNum = 0; pointNum < pDestinationSmeltingBuilding->nPutDownPoints(); ++pointNum )
			{
				MexPoint2d padPosition = pDestinationSmeltingBuilding->putDownPoint( pointNum );
				if( padPosition == destinationPadPosition_ )
				{
					pDestinationSmeltingBuilding->usedPickupPoint( pointNum );			
					break;
				}
					
			}

			if( pActor_->amountCarried() == 0 )
				finished_ = true;
		}
	}
	
	return interval;
}

//virtual
bool MachLogPutDownOperation::doBeInterrupted()
{
	CB_MachLogPutDownOperation_DEPIMPL();

	pActor_->motionSeq().stop();
	return not pActor_->motionSeq().hasDestination();
}

void perWrite( PerOstream& ostr, const MachLogPutDownOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pImpl_;
}

void perRead( PerIstream& istr, MachLogPutDownOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pImpl_;	
}

MachLogPutDownOperation::MachLogPutDownOperation( PerConstructor con )
:	MachLogOperation( con )
{
}

void MachLogPutDownOperation::chooseMostViablePad()
{
	CB_MachLogPutDownOperation_DEPIMPL();

	MexPoint2d oldDest = destinationPadPosition_;

	// calculate which smelter pad is closest.
	MATHEX_SCALAR closestPointDistance=1000000000;		//used to determine which loading bay is closest	
	
	MachLogConstruction* pDestinationSmeltingBuilding = pActor_->pDestinationSmeltingBuilding();

	ASSERT( pDestinationSmeltingBuilding, "Resource carrier has no destination building!" );

	MexPoint2d actorPos( pActor_->position() );	
	
	bool found = false;

	for( int j = 0; j < pDestinationSmeltingBuilding->nPutDownPoints(); ++j )
	{		
		MexPoint2d	testDest = pDestinationSmeltingBuilding->putDownPoint( j );
		MATHEX_SCALAR testPointDistance = actorPos.sqrEuclidianDistance( testDest ); 

		bool seemsFree = MachLogSpacialManipulation::pointIsFree( testDest, 5.0 ) 
						 and not( pDestinationSmeltingBuilding->pickupPointUsedRecently( j ) );

		if( seemsFree )
		{
			if( testDest == oldDest )
			{
				// if my current destination is still free, choose that in preference to any others.
				destinationPadPosition_ = oldDest;
				found = true;
				break;
			}
			else if ( testPointDistance <= closestPointDistance )
			{
				// note that destinationPadPosition_ may have its value reassigned several times as better candidates are found
				destinationPadPosition_ = testDest;
				closestPointDistance = testPointDistance;
				found = true;
			}
		}
	}

	if( not found )
	{
		// no points are free - just choose a point at random.
		destinationPadPosition_ = pDestinationSmeltingBuilding->putDownPoint( MachPhysRandom::randomInt( 0, _STATIC_CAST( int, pDestinationSmeltingBuilding->nPutDownPoints() ) ) );
	}
}

void MachLogPutDownOperation::possiblyChangeDestinationPad()
{
	CB_MachLogPutDownOperation_DEPIMPL();

	MexPoint2d oldDest = destinationPadPosition_;

	chooseMostViablePad();		// may alter destinationPadPosition_;

	if( destinationPadPosition_ != oldDest )
	{
		// we've changed location.
		if( pSubOperation() )
		{
			pSubOperation()->doBeInterrupted();
			pActor_->motionSeq().stop()	;
		}
	}
}

bool MachLogPutDownOperation::attemptToGuaranteeClearPutDownPoint( const MexPoint2d& actorPos, MATHEX_SCALAR carrierClearance, MexPoint2d* pClearPutDownPoint )
{
	CB_MachLogPutDownOperation_DEPIMPL();

	const MexTransform3d testTransform = 
		( actorPos == *pClearPutDownPoint ) ?
		MexTransform3d( actorPos ) :
 		MachLogSpacialManipulation::sourceToDestination3dTransform( *pClearPutDownPoint, actorPos, MachLogSpacialManipulation::SOURCE_END_IS_CENTRE );

	const bool foundSpace = MachLogSpacialManipulation::getNearestFreeSpacePoint( testTransform, 0.0, carrierClearance, pClearPutDownPoint, carrierClearance );

	return foundSpace;
}


/* End OPPUTDWN.CPP *************************************************/
