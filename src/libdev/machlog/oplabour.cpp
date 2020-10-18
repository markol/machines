/*
 * O P L A B O U R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/oplabour.hpp"
#include "machlog/internal/oplaboui.hpp"

//#include "machphys/plansurf.hpp"
#include "ctl/algorith.hpp"

#include "mathex/abox2d.hpp"
#include "mathex/transf3d.hpp"

#include "phys/cspace2.hpp"

#include "sim/manager.hpp"

#include "machphys/mphydata.hpp"
#include "machphys/ctordata.hpp"

#include "machlog/animatio.hpp"
#include "machlog/buildpnt.hpp"
#include "machlog/constron.hpp"
#include "machlog/construc.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/move.hpp"
#include "machlog/planet.hpp"
#include "machlog/spacial.hpp"

PER_DEFINE_PERSISTENT_ABSTRACT( MachLogLabourOperation );

#define CB_MachLogLabourOperation_DEPIMPL()	\
		PRE( pImpl_ );	\
		CB_DEPIMPL( MachLogConstructor*,					pActor_ ); \
		CB_DEPIMPL( MachLogConstruction*,					pConstruction_ ); \
    	CB_DEPIMPL( State,									state_ ); \
		CB_DEPIMPL( PhysAbsoluteTime,						lastUpdateTime_ ); \
		CB_DEPIMPL( MachLogBuildPoint*,						pBuildPoint_ ); \
		CB_DEPIMPL( bool,									currentlyAttached_ ); \
		CB_DEPIMPL( bool,									hasBuildPoint_ );

MachLogLabourOperation::MachLogLabourOperation( MachLogConstructor * pActor,
												MachLogConstruction * pConstr,
												const char* opName,
												MachLogOperation::OperationType opType )
:	MachLogOperation( opName, opType ),
	pImpl_( _NEW( MachLogLabourOperationImpl( pActor, pConstr ) ) )
{
	CB_MachLogLabourOperation_DEPIMPL();

	tryToAssignToNearestBuildPoint();

	pConstruction_->attach( this );
	pConstruction_->addConstructor( pActor_ );

}

MachLogLabourOperation::~MachLogLabourOperation()
{
	CB_MachLogLabourOperation_DEPIMPL();

	pActor_->constructing( false );

	if( hasBuildPoint_ )
	{
		ASSERT( pConstruction_, "If we have no associated construction, we should NOT have a buildpoint." );
		giveUpBuildPoint();
	}

	if( currentlyAttached_ )
	{
		pConstruction_->detach( this );
		pConstruction_->removeConstructor( pActor_ );
	}

	pActor_->motionSeq().allowMoveOutOfWay( true );

	_DELETE( pImpl_ );
}

void MachLogLabourOperation::doOutputOperator( ostream& o ) const
{
	CB_MachLogLabourOperation_DEPIMPL();

	o << "MachLogConstructOperation\n";
	MachPhys::BuildingMaterialUnits UNITS = pConstructor()->data().constructionRate();
	o << " Labour rate " << UNITS << std::endl;

	o << " ";
	switch( state() )
	{
		case MOVING:
			o << "MOVING" << std::endl;
			break;
		case INTERACTING:
			o << "INTERACTING" << std::endl;
			break;
		default:
			o << "UNKNOWN STATE " << (int)state() << std::endl;
	}

	if( hasBuildPoint_ )
	{
		MexPoint2d dest = pBuildPoint_->position();
		o << "Assigned to build point at " << dest;
		o << ", euclidian distance " << pConstructor()->position().euclidianDistance( dest ) << std::endl;
	}
}

///////////////////////////////////

bool MachLogLabourOperation::doStart()
{
	CB_MachLogLabourOperation_DEPIMPL();

	bool result = not checkNeedAndDoLeaveOperation( pActor_ );
	if( result )
		state_ = MOVING;
	//HAL_STREAM("(" << pActor_->id() << ") MLConstructOperation::doStart return " << result << "\n" );
	return result;
}

PhysRelativeTime
MachLogLabourOperation::doUpdate()
{
	// WARNING this method has multiple exit points

	CB_MachLogLabourOperation_DEPIMPL();

	PhysRelativeTime interval = 2.0;
	if( not pConstruction_ )
		return 0.0;

	if( pSubOperation() )
	{
		bool abortOut = true;

		// if we have a build point and we're near it and facing it, DON'T abort out.
		if( hasBuildPoint_ )
		{
			MexPoint2d buildPointPosition( pBuildPoint_->position() );
			MexPoint2d constructorPosition( pActor_->position() );

			MATHEX_SCALAR sqrAcceptableDistance = sqr( pActor_->highClearence() * ( 2.6 + MachLogLabourOperation::proximityLeewayMultiplier() ) );
			MATHEX_SCALAR sqrActualDistance = constructorPosition.sqrEuclidianDistance( buildPointPosition );

			if( sqrActualDistance <= sqrAcceptableDistance and constructorFacingBuilding() )
			{
				abortOut = false;
			}
		}

		if( abortOut )
		{
			return interval;
		}

	}

	MexPoint2d constructorPosition( pActor_->position() );
	MexPoint2d constructionPosition = pConstruction_->position();

	if( not hasBuildPoint_ )
	{
		tryToAssignToNearestBuildPoint();
	}


	// and if we STILL don't have a build point.....
	if( not hasBuildPoint_ )
	{

		// Just try to move to a point about 35m away from the construction. Note that if we're too CLOSE, we
		// actually want to back out again before reattempting to acquire a build point rather than staying in
		// tight to the building and gumming up not only other builders' space, but possibly the space
		// we're currently occupying which may be preventing our attempt to find a clear build point from succeeding.

		MATHEX_SCALAR sqrDistanceToBuilding = constructorPosition.sqrEuclidianDistance( constructionPosition );

		if( sqrDistanceToBuilding > sqr(45) or sqrDistanceToBuilding < sqr(30) )
		{
			bool foundSpace = false;

			MexTransform3d constructionPointAsTransform = MachLogSpacialManipulation::sourceToDestination3dTransform( constructionPosition, constructorPosition, MachLogSpacialManipulation::SOURCE_END_IS_CENTRE );

			MexPoint2d approachPosition;

			MATHEX_SCALAR radius = 35;
			while( not foundSpace )
			{
				if( not MachLogSpacialManipulation::getNearestFreeSpacePoint( constructionPointAsTransform, radius , pActor_->highClearence(), &approachPosition ) )
					radius += 20;
				else
					foundSpace = true;
			}

			pActor_->motionSeq().stop();
			subOperation( pActor_, _NEW( MachLogMoveAndTurnOperation( pActor_, approachPosition, constructionPosition ) ) );

			return 0.0;

		}
		else
			// If we're already close enough, let's just stay close by to monitor the situation and wait for any
			// build points to become available.
			return 3.0;
	}

	// okay, we definitely have a build point.

	MexPoint3d buildPointPosition( pBuildPoint_->position() );


	switch( state_ )
	{
		case MOVING :
		{
            // no build bonus from accumulated time if we're moving
            lastUpdateTime_ = SimManager::instance().currentTime();

			pActor_->constructing( false );

			// In order to switch to build mode, we need to be sufficiently close to the building,
			// and pretty much facing it.

			MATHEX_SCALAR sqrAcceptableDistance = sqr( pActor_->highClearence() * ( 2.6 + MachLogLabourOperation::proximityLeewayMultiplier() ) );
			MATHEX_SCALAR sqrActualDistance = constructorPosition.sqrEuclidianDistance( buildPointPosition );

			if( sqrActualDistance <= sqrAcceptableDistance )
			{
				pActor_->motionSeq().stop();
				if( constructorFacingBuilding() )
				{
					state_ = INTERACTING;
					//HAL_STREAM(" reached target switching to building\n" );
					interval = attemptToInteractWithBuilding();
				}
				else
				{
					// close enough - we simply need to do a turn animation towards the building in question

					// lock the motion sequencer right now.
					pActor_->motionSeq().allowMoveOutOfWay( false );

					MexPoint3d constructionCentre = constructionPosition;
					MexRadians turnAngle = MachLogSpacialManipulation::angleToTurnToFace( *pActor_, constructionCentre ).asScalar();

					subOperation( pActor_, _NEW( MachLogTurnAnimation( pActor_, turnAngle ) ) );
					interval = 0.5;
				}
			}
			else
			{
				// need to move in closer to the building, Charlie.

				pActor_->motionSeq().allowMoveOutOfWay( true );

				PhysConfigSpace2d& cs = MachLogPlanet::instance().configSpace();
				MexPoint2d actualBuildPosition;

				MexTransform3d constructionPointAsTransform = MachLogSpacialManipulation::sourceToDestination3dTransform( pConstruction()->position(), buildPointPosition, MachLogSpacialManipulation::DESTINATION_END_IS_CENTRE );

				MATHEX_SCALAR radius = pActor_->highClearence() *1.8;
				bool actorCanFitThere = MachLogSpacialManipulation::getNearestFreeSpacePoint( constructionPointAsTransform, radius , pActor_->highClearence(), &actualBuildPosition, pActor_->highClearence() * MachLogLabourOperation::proximityLeewayMultiplier() );

				if( not actorCanFitThere )
				{
					// give up our build point. We'll try and find an alternative next time round.
					giveUpBuildPoint();
				}
				else
				{
					pActor_->motionSeq().stop();
	  				subOperation( pActor_, _NEW( MachLogMoveAndTurnOperation( pActor_, actualBuildPosition, constructionPosition ) ) );
				}
			}
		}
		break;

		case INTERACTING :
		{
 			interval = attemptToInteractWithBuilding();
		}
		break;

		default :	ASSERT_BAD_CASE;	break;
	}

	/*
	if( pActor_->motionSeq().hasDestination() )
		interval = pActor_->motionSeq().update( 0.001 );
	*/

	return interval;
}

//static
MATHEX_SCALAR MachLogLabourOperation::proximityLeewayMultiplier()
{
	static MATHEX_SCALAR multiplier = 0.8;
	return multiplier;
}

void MachLogLabourOperation::doFinish()
{
	/* Intentionally Empty	*/
}

PhysRelativeTime MachLogLabourOperation::attemptToInteractWithBuilding()
{
	CB_MachLogLabourOperation_DEPIMPL();

	MexPoint2d constructorPosition( pConstructor()->position() );
	MexPoint3d buildPointPosition( pBuildPoint_->position() );

	PhysRelativeTime interval = 2.0;

	if( pConstructor()->motionSeq().hasDestination() )
		pConstructor()->motionSeq().stop();

	if( constructorPosition.sqrEuclidianDistance( buildPointPosition ) >= sqr( pConstructor()->highClearence() * ( 2.6 + MachLogLabourOperation::proximityLeewayMultiplier() ) ) or not constructorFacingBuilding() )
	{
		// hey, we must have been interrupted (by an evade op etc.) and now
		// that we've resumed our construct op, we're no longer at our build point!
		state( MOVING );

		//refresh our "nearest build point" as the old one may no longer be the nearest
		tryToAssignToNearestBuildPoint();

		interval = 0.1;
	}
	else
	{
		// we can happily do the interaction proper.

		pConstructor()->motionSeq().allowMoveOutOfWay( false );

		// call virtual interaction method
		interval = interactWithBuilding();
	}

	return interval;
}

//virtual
bool MachLogLabourOperation::doBeInterrupted()
{
	CB_MachLogLabourOperation_DEPIMPL();

	pActor_->motionSeq().stop();
	pActor_->constructing( false );
	return not pActor_->motionSeq().hasDestination();
}

void MachLogLabourOperation::tryToAssignToNearestBuildPoint()
{
	CB_MachLogLabourOperation_DEPIMPL();

	if( hasBuildPoint_ )
	{
		giveUpBuildPoint();
	}

	hasBuildPoint_ = pConstruction_->bestAvailableBuildPoint( *pActor_, &pBuildPoint_ );

	if( hasBuildPoint_ )
		pBuildPoint_->reserve( pActor_->subType() );
}

void MachLogLabourOperation::giveUpBuildPoint()
{
	CB_MachLogLabourOperation_DEPIMPL();

	PRE( hasBuildPoint_ );

	pBuildPoint_->unReserve();
	pBuildPoint_ = NULL;
	hasBuildPoint_ = false;
}

bool MachLogLabourOperation::constructorFacingBuilding() const
{
	CB_MachLogLabourOperation_DEPIMPL();

	MexPoint3d constructionCentre = pConstruction_->position();

	MexRadians absTempRelativeAngle = fabs( MachLogSpacialManipulation::angleToTurnToFace( *pActor_, constructionCentre ).asScalar() );

	return ( absTempRelativeAngle.asScalar() < 0.3 );
}


MachLogConstruction* MachLogLabourOperation::pConstruction()
{
	CB_MachLogLabourOperation_DEPIMPL();

	return pConstruction_;
}

const MachLogConstruction* MachLogLabourOperation::pConstruction() const
{
	CB_MachLogLabourOperation_DEPIMPL();

	return pConstruction_;
}

void MachLogLabourOperation::pConstruction( MachLogConstruction* pCons )
{
	CB_MachLogLabourOperation_DEPIMPL();

	pConstruction_ = pCons;
}


MachLogConstructor* MachLogLabourOperation::pConstructor()
{
	CB_MachLogLabourOperation_DEPIMPL();

	return pActor_;
}

const MachLogConstructor* MachLogLabourOperation::pConstructor() const
{
	CB_MachLogLabourOperation_DEPIMPL();

	return pActor_;
}

bool MachLogLabourOperation::currentlyAttached() const
{
	CB_MachLogLabourOperation_DEPIMPL();

	return currentlyAttached_;
}

void MachLogLabourOperation::currentlyAttached( bool status )
{
	CB_MachLogLabourOperation_DEPIMPL();

	currentlyAttached_ = status;
}

PhysAbsoluteTime MachLogLabourOperation::lastUpdateTime() const
{
	CB_MachLogLabourOperation_DEPIMPL();

	return lastUpdateTime_;
}

void MachLogLabourOperation::lastUpdateTime( PhysAbsoluteTime newTime )
{
	CB_MachLogLabourOperation_DEPIMPL();

	lastUpdateTime_ = newTime;
}

MachLogLabourOperation::State MachLogLabourOperation::state() const
{
	CB_MachLogLabourOperation_DEPIMPL();

	return state_;
}

void MachLogLabourOperation::state( State newState )
{
	CB_MachLogLabourOperation_DEPIMPL();

	state_ = newState;
}

void perWrite( PerOstream& ostr, const MachLogLabourOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pImpl_;
}

void perRead( PerIstream& istr, MachLogLabourOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pImpl_;

	if( op.currentlyAttached() )
		op.pConstruction()->attach( &op );
}

MachLogLabourOperation::MachLogLabourOperation( PerConstructor con )
:	MachLogOperation( con )
{
}


//virtual
bool MachLogLabourOperation::beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData )
{
	CB_MachLogLabourOperation_DEPIMPL();

	bool stayAttached = true;

	switch( event )
	{
	case W4dSubject::DELETED:
		if( pSubject == (W4dSubject*)pConstruction() )
		{
        /*
			//construction has been deleted
			pConstruction()->removeConstructor( pConstructor() );
        */

			stayAttached = false;
		}
		break;
	case W4dSubject::CLIENT_SPECIFIC:
		stayAttached = clientSpecificNotification( clientData );
	default:
		;
	}

	if( not stayAttached )
	{
		pActor_->constructing( false );

		// op is aborted - stop moving out of sheer shock and disbelief
		pActor_->motionSeq().stop();

		// tidy up loose ends with the construction
		if( hasBuildPoint_ )
		{
			giveUpBuildPoint();
		}

		pConstruction( NULL );
		currentlyAttached( false );
	}

	return stayAttached;
}

// Forced recompile 11/1/99 CPS
/* End OPLABOUR.CPP *************************************************/
