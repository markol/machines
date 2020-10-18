/*
 * O P R E C Y C L . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/oprecycl.hpp"

#include "phys/motchunk.hpp"

#include "machphys/harddata.hpp"
#include "machphys/machdata.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/random.hpp"
#include "machphys/station.hpp"
#include "machphys/stations.hpp"	

#include "machlog/opgolab.hpp"
#include "machlog/machine.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/move.hpp"
#include "machlog/inout.hpp"
#include "machlog/races.hpp"
#include "machlog/constron.hpp"
#include "machlog/smelter.hpp"
#include "machlog/spacial.hpp"
#include "machlog/stats.hpp"

PER_DEFINE_PERSISTENT( MachLogRecycleOperation );
/* ////////////////////////////////////////////// constructor /////////////////////////////////////////////////// */

MachLogRecycleOperation::MachLogRecycleOperation( MachLogMachine * pActor , MachLogConstruction* pSmeltingBuilding )
:	MachLogOperation( "RECYCLE_OPERATION", MachLogOperation::RECYCLE_OPERATION ),
 	pActor_( pActor ),
  	finished_( false ),
  	pSmeltingBuilding_( pSmeltingBuilding )
{
	ASSERT( pSmeltingBuilding_,"Target smelter in recycle operation is NULL\n" );
	ASSERT( pSmeltingBuilding_->objectIsCanSmelt()," pSmelter does not point to a smelter!\n" );
	pSmeltingBuilding_->attach( this );	

	selectBestRecyclePosition();	
}

/* /////////////////////////////////////////////// destructor /////////////////////////////////////////////////// */

MachLogRecycleOperation::~MachLogRecycleOperation()
{
	if( pSmeltingBuilding_ )
		pSmeltingBuilding_->detach( this );
    TEST_INVARIANT;

}

/* ////////////////////////////////////////////////////////////////////////////////////////////////////////////// */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogRecycleOperation::doOutputOperator( ostream& o ) const
{
	o << "MachLogRecycleOperation " << std::endl;

	if( pSmeltingBuilding_ )
		o << " pSmeltingBuilding_->id() " << pSmeltingBuilding_->id() << std::endl;
	else
		o << " Smelting building has been destroyed!" << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogRecycleOperation::doStart()
{
	return not checkNeedAndDoLeaveOperation( pActor_ );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogRecycleOperation::doFinish()
{
	/* Intentionally Empty	*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogRecycleOperation::doIsFinished() const
{
	return finished_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PhysRelativeTime
MachLogRecycleOperation::doUpdate( )
{
	PhysRelativeTime interval = 2.0;
  	
	if( pSubOperation() )
	{
		interval = 2.0;
	}
	else
	{
		// either recycle oursleves if we're close enough, or initiate a move to a pad
		
		MATHEX_SCALAR machineClearance = pActor_->highClearence(); 
		MexPoint2d actorPos( pActor_->position() );

		HAL_STREAM("(" << pActor_->id() << ") MachLogRecycleOperation::doUpdate\n" );
		if( actorPos.sqrEuclidianDistance( nearestPadPosition() ) > sqr( machineClearance * 2.2 ) )
		{
			// the situation MAY have changed such that our old "best recycle position" is 
			// actually no longer the best one. Therefore destinationPadPosition_ may change as a 
			// result of this method being called.
			selectBestRecyclePosition();

			subOperation( pActor_, _NEW( MachLogMoveToOperation( pActor_, destinationPadPosition_, true, machineClearance * 2.0 ) ) );
			interval = 1.0;
		}
		else
		{
			// we must be close enough to top ourselves.

			pActor_->motionSeq().stop();

			MachPhys::BuildingMaterialUnits amountToAdd = pActor_->machineOriginalCost() * MachLogRaces::instance().stats().secondhandRefundablePercentage();
					
			MachLogRaces::instance().smartAddBMUs( pSmeltingBuilding_->race(), amountToAdd ); 
		   
		   	//If this is a true smelter rather than a pod, tell it to do smelting animation
			if( pSmeltingBuilding_->objectType() == MachLog::SMELTER )
				pSmeltingBuilding_->asSmelter().droppedOffOre();

			pActor_->smeltMe();

			finished_ = true;			
			pSmeltingBuilding_->detach( this );			
					
			interval = 0.0;	
		}
	}

	return interval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
bool MachLogRecycleOperation::doBeInterrupted()
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogRecycleOperation::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
bool MachLogRecycleOperation::beNotified( W4dSubject* pSubject,
                 W4dSubject::NotificationEvent event, int /*clientData*/ )
{
	PRE( pSubject == (W4dSubject*)pSmeltingBuilding_ );

	bool stayAttached = true;
	
	switch( event )
	{
	case W4dSubject::DELETED:
	
	   	//smelter has been deleted...can't continue with the op
	   	pSmeltingBuilding_ = NULL;
		finished_ = true;	
	   	stayAttached = false;
		
		break;
	default:
		;
	}

	return stayAttached;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const MachLogRecycleOperation& t )
{

    o << "MachLogRecycleOperation " << (void*)&t << " start" << std::endl;
    o << "MachLogRecycleOperation " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogRecycleOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pActor_;
	ostr << op.pSmeltingBuilding_;
	ostr << op.destinationPadPosition_;
	ostr << op.finished_;
}

void perRead( PerIstream& istr, MachLogRecycleOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pActor_;
	istr >> op.pSmeltingBuilding_;
	istr >> op.destinationPadPosition_;
	istr >> op.finished_;
	if( op.pSmeltingBuilding_ )
		op.pSmeltingBuilding_->attach( &op );
}

MachLogRecycleOperation::MachLogRecycleOperation( PerConstructor con )
:	MachLogOperation( con )
{
}

bool MachLogRecycleOperation::attemptToGuaranteeClearRecyclePoint( const MexPoint2d& actorPos, MATHEX_SCALAR machineClearance, MexPoint2d* pClearRecyclePoint )
{
	MexTransform3d testTransform = MachLogSpacialManipulation::sourceToDestination3dTransform( *pClearRecyclePoint, actorPos, MachLogSpacialManipulation::SOURCE_END_IS_CENTRE );

	bool foundSpace = MachLogSpacialManipulation::getNearestFreeSpacePoint( testTransform, 0.0, machineClearance, pClearRecyclePoint, machineClearance );

	return foundSpace;
}

bool MachLogRecycleOperation::selectBestRecyclePosition()
{
	MATHEX_SCALAR machineClearance = pActor_->highClearence(); 
	MexPoint2d actorPos( pActor_->position() );

	// calculate which smelter pad is closest.
	MATHEX_SCALAR closestPointDistance=1000000000;		//used to determine which loading bay is closest	
	
	// give this some default value which will hopefully be replaced shortly.
	destinationPadPosition_	= pSmeltingBuilding_->putDownPoint( 0 );

	bool found = false;

	for( int j = 0; j < pSmeltingBuilding_->nPutDownPoints(); ++j )
	{		
		MexPoint2d	candidatePadPosition = pSmeltingBuilding_->putDownPoint( j );
		MATHEX_SCALAR candidatePointDistance = actorPos.sqrEuclidianDistance( candidatePadPosition ); 
		if (  candidatePointDistance <= closestPointDistance )
		{
			// okay, it's closer than the nearest viable pad found so far. But is this pad itself useable (does it have enough space)?

			// if we're actually within 2.2 X clearance of this pad point, accept the pad position unconditionally.

			if( actorPos.sqrEuclidianDistance( candidatePadPosition ) <= sqr( machineClearance * 2.2 ) )
			{
				// note that destinationPadPosition_ may have its value reassigned several times as better candidates are found
				destinationPadPosition_ = candidatePadPosition;
				closestPointDistance = candidatePointDistance;
				found = true;	
			}
			else
			{
				// okay, we're not THAT close...we need to try and ensure the destination point is in 
				// clear space (especially for horrible big machines)	

				MexPoint2d clearRecyclePoint = candidatePadPosition;

				if( attemptToGuaranteeClearRecyclePoint( actorPos, machineClearance, &clearRecyclePoint ) )
				{
					// note that destinationPadPosition_ may have its value reassigned several times as better candidates are found
					destinationPadPosition_ = clearRecyclePoint;
					closestPointDistance = candidatePointDistance;
					found = true;		
				}
			}			
 		}	
	}
	
	return found;
}

MexPoint2d MachLogRecycleOperation::nearestPadPosition() const
{
	
	MexPoint2d actorPos( pActor_->position() );

	// calculate which smelter pad is closest.
	MATHEX_SCALAR closestPointDistance=1000000000;		//used to determine which loading bay is closest	
	
	// give this some default value
	MexPoint2d nearestPadPos = pSmeltingBuilding_->putDownPoint( 0 );

	bool found = false;

	for( int j = 0; j < pSmeltingBuilding_->nPutDownPoints(); ++j )
	{		
		MexPoint2d	candidatePadPosition = pSmeltingBuilding_->putDownPoint( j );
		MATHEX_SCALAR candidatePointDistance = actorPos.sqrEuclidianDistance( candidatePadPosition ); 
		if (  candidatePointDistance <= closestPointDistance )
		{
			// note that nearestPadPos may have its value reassigned several times as better candidates are found
			nearestPadPos = candidatePadPosition;
			closestPointDistance = candidatePointDistance;
			found = true;		
 		}	
	}
	
	ASSERT( found, "No pad was found!" );

	return nearestPadPos;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* End OPRECYCL.CPP **************************************************/
