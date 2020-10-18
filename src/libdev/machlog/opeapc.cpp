/*
 * O P E A P C . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/angle.hpp"
#include "mathex/transf3d.hpp"

#include "machlog/opeapc.hpp"

#include "machlog/apc.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/move.hpp"
#include "machlog/planet.hpp"
#include "machlog/spacial.hpp"

PER_DEFINE_PERSISTENT( MachLogEnterAPCOperation );
/* //////////////////////////////////////////////////////////////// */

MachLogEnterAPCOperation::MachLogEnterAPCOperation( MachLogMachine * pActor , MachLogAPC* pAPC )
:	MachLogOperation( "ENTER_APC_OPERATION", MachLogOperation::ENTER_APC_OPERATION ),
 	pActor_( pActor ),
 	pAPC_( pAPC ),
	finished_( false ),
	status_( PREPARING_TO_GO_TO_APC ),
	desiredRange_( 15 )
{
	ASSERT( pAPC, "Target APC in enter APC operation is NULL\n" );
	pAPC_->attach( this );
}

MachLogEnterAPCOperation::MachLogEnterAPCOperation(
  MachLogMachine * pActor,
  MachLogAPC* pAPC,
  PhysPathFindingPriority pathFindingPriority )
:	MachLogOperation( "ENTER_APC_OPERATION", MachLogOperation::ENTER_APC_OPERATION, pathFindingPriority ),
 	pActor_( pActor ),
 	pAPC_( pAPC ),
	finished_( false ),
	status_( PREPARING_TO_GO_TO_APC ),
	desiredRange_( 15 )
{
	ASSERT( pAPC, "Target APC in enter APC operation is NULL\n" );
	pAPC_->attach( this );
}

/* //////////////////////////////////////////////////////////////// */

MachLogEnterAPCOperation::~MachLogEnterAPCOperation()
{ 
	ASSERT ( pActor_->id(), "Something's wrong as we should still have a valid pActor_ pointer." );

	
	if( pAPC_ ) 
	{	
		pAPC_->detach( this );

		checkAndDoStopGoingToAPC();		
	}

	// ensure actor's motion sequencer is not locked
	pActor_->motionSeq().allowMoveOutOfWay( true );
}

/* //////////////////////////////////////////////////////////////// */

/////////////////////////////////////////////////////////////////////////

//virtual
bool MachLogEnterAPCOperation::beNotified( W4dSubject* pSubject,
                 W4dSubject::NotificationEvent event, int clientData )
{
	ASSERT( pSubject == (W4dSubject*)pAPC_, "Notified by unexpected subject!" ); 

	bool stayAttached = true;

	switch( event )
	{
	case W4dSubject::DELETED:
		//APC has been deleted...
		checkAndDoStopGoingToAPC();
		pAPC_ = NULL;
		finished_ = true;
		stayAttached = false;
		break;
	case W4dSubject::CLIENT_SPECIFIC:
		switch( clientData )
		{
			case MachLog::RACE_CHANGED:
				checkAndDoStopGoingToAPC();
				pAPC_ = NULL;
				finished_ = true;
				stayAttached = false;
				break;
			default:
				;
		}
		break;
	default:
		;
	}
	return stayAttached;
}

/////////////////////////////////////////////////////////////////////////

void MachLogEnterAPCOperation::doOutputOperator( ostream& o ) const
{
	o << "MachLogEnterAPCOperation " << std::endl;
	if( pAPC_ )
		o << " pAPC_->id() " << pAPC_->id() << std::endl;
	else
		o << " pAPC has been destroyed!" << std::endl;
	switch( status_ )
	{
		case PREPARING_TO_GO_TO_APC:
			o << "PREPARING_TO_GO_TO_APC\n";
			break;
		case GOING_TO_APC:
			o << "GOING_TO_APC\n";
			break;
		case ENTERING_APC:
			o << "ENTERING_APC\n";
			break;
		default:
			o << "Unknown status\n";
	}
}

/////////////////////////////////////////////////////////////////////////

bool MachLogEnterAPCOperation::doStart()
{
	bool result = true;

	if( pAPC_ and status_ == PREPARING_TO_GO_TO_APC )
	{
		pAPC_->addIncoming( pActor_ );
		status_ = GOING_TO_APC;
		result = not checkNeedAndDoLeaveOperation( pActor_ );	
	}

	return result;
}

/////////////////////////////////////////////////////////////////////////

void MachLogEnterAPCOperation::doFinish()
{
	/* Intentionally Empty	*/
}

/////////////////////////////////////////////////////////////////////////

bool MachLogEnterAPCOperation::doIsFinished() const
{
	return finished_;
}

/////////////////////////////////////////////////////////////////////////

PhysRelativeTime
MachLogEnterAPCOperation::doUpdate( )
{
	//have we got an active sub operation at the moment?
	//HAL_STREAM("(" << pActor_->id() << ") MLEnterAPCOp\n" );

	bool closeEnoughToEnter = pActor_->position().sqrEuclidianDistance( pAPC_->position() ) < sqr( pAPC_->highClearence() + desiredRange_ );

	if( not closeEnoughToEnter and pSubOperation() )
		return 2.0;

	//if the APC is inside a building then exit
	if( pAPC_->insideBuilding() )
	{
		finished_ = true;
		return 0.0;
	}

	if( status_ == GOING_TO_APC )
	{
		if( not closeEnoughToEnter )
		{
			// too far away to enter......ensure motion sequencer is not locked
			pActor_->motionSeq().allowMoveOutOfWay( true );

			MexPoint2d apcPosition( pAPC_->position() );
			MexPoint2d machinePosition( pActor_->position() );
			MATHEX_SCALAR useClearance = pActor_->highClearence() * 1.5;

			PhysConfigSpace2d& cs = MachLogPlanet::instance().configSpace();

			MexTransform3d apcToMachineTransform = MachLogSpacialManipulation::sourceToDestination3dTransform( apcPosition, machinePosition, MachLogSpacialManipulation::SOURCE_END_IS_CENTRE );

			MexPoint2d loadPoint;
		
	        bool foundSpace = false;
			MATHEX_SCALAR apcClearance = pAPC_->highClearence() + 7;
			foundSpace = MachLogSpacialManipulation::getNearestFreeSpacePoint( apcToMachineTransform, apcClearance, useClearance, &loadPoint, desiredRange_ - 7 );

			if( not foundSpace )
				finished_ = true;
			else  
				subOperation( pActor_, _NEW( MachLogMoveToOperation( pActor_, loadPoint, true, 5.0 ) ) );	//5m tolerance
		}
		else
		{
			//HAL_STREAM(" close enough set status to ENTER\n" );
			// Close enough to enter APC...........stand still and lock the motion sequencer so
			//  we won't move out of the way of other machines jostling us..........
			
			pActor_->motionSeq().stop();
			pActor_->motionSeq().allowMoveOutOfWay( false );
			
			// ........but are we at the right angle?
			MexRadians entryAngle = MachLogSpacialManipulation::angleToTurnToFace( *pAPC_, pActor_->position() );
			if ( entryAngle>MexRadians( 2.8 ) or entryAngle<MexRadians( -2.8 )  ) 
				status_ = ENTERING_APC;
		}
	}
	if( status_ == ENTERING_APC )
	{
		//HAL_STREAM(" status_ is ENTERING_APC calling pApc_->add\n" );
		pAPC_->add( pActor_ );
		finished_ = true;
	}
	return 1.0;
}

/////////////////////////////////////////////////////////////////////////

//virtual
bool MachLogEnterAPCOperation::doBeInterrupted()
{
	return true;
}

/* //////////////////////////////////////////////////////////////// */

ostream& operator <<( ostream& o, const MachLogEnterAPCOperation& t )
{

    o << "MachLogEnterAPCOperation " << (void*)&t << " start" << std::endl;
    o << "MachLogEnterAPCOperation " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogEnterAPCOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pActor_;
	ostr << op.pAPC_;
	ostr << op.finished_;
	ostr << op.desiredRange_;
	ostr << op.status_;
}

void perRead( PerIstream& istr, MachLogEnterAPCOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pActor_;
	istr >> op.pAPC_;
	istr >> op.finished_;
	istr >> op.desiredRange_;
	istr >> op.status_;
}

MachLogEnterAPCOperation::MachLogEnterAPCOperation( PerConstructor con )
:	MachLogOperation( con )
{
}

void MachLogEnterAPCOperation::checkAndDoStopGoingToAPC()
{
	// remove ourselves if we still have a pointer to the APC AND we've actually added 
	// ourselves to the APC's incoming list by setting out toward it
	if( pAPC_ and status_ != PREPARING_TO_GO_TO_APC ) 
	{	
		pAPC_->removeIncoming( pActor_ );					
	}	
}	 

/////////////////////////////////////////////////////////////////////////

/* End OPEAPC.CPP *************************************************/
