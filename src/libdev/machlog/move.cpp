/*
 * M O V E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include <string>

#include "machlog/move.hpp"

#include <iostream>

#include "phys/motchunk.hpp"

#include "ctl/list.hpp"

#include "mathex/abox2d.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/vec3.hpp"
#include "mathex/vec2.hpp"

#include "sim/manager.hpp"

#include "machphys/mcmovinf.hpp"

#include "machlog/animatio.hpp"
#include "machlog/machine.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/races.hpp"
#include "machlog/strategy.hpp"
#include "machlog/constron.hpp"
#include "machlog/stats.hpp"
#include "machlog/grpmvinf.hpp"

#include "machlog/internal/movei.hpp"

#include "profiler/profiler.hpp"
#include "profiler/stktrace.hpp"

/* //////////////////////////////////////////////////////////////// */
PER_DEFINE_PERSISTENT( MachLogMoveToOperation );

#define CB_MachLogMoveToOperation_DEPIMPL()	\
		PRE( pImpl_ );	\
		CB_DEPIMPL( MachLogMachine*, pActor_ ); \
    	CB_DEPIMPL( MexPoint3d, dest_ ); \
		CB_DEPIMPL( bool, checkForLeave_ ); \
		CB_DEPIMPL( MachLogMoveToOperation::Path, path_ ); \
		CB_DEPIMPL( MATHEX_SCALAR, tolerance_ ); \
		CB_DEPIMPL( size_t, commandId_ );

MachLogMoveToOperation::MachLogMoveToOperation( MachLogMachine * pActor, const MexPoint3d& dest, bool checkForLeave, MATHEX_SCALAR tolerance )
: 	MachLogOperation( "MOVE_TO_OPERATION", MachLogOperation::MOVE_TO_OPERATION ),
	pImpl_( _NEW( MachLogMoveToOperationImpl( pActor, dest, checkForLeave ) ) )
//  state_( PLANNING ),
//  pathSearch_( MachLogPlanet::instance().surface()->pSpace()->xCells() * MachLogPlanet::instance().surface()->pSpace()->yCells() )
{
	CB_MachLogMoveToOperation_DEPIMPL();

	tolerance_ = tolerance;
    path_.push_back( dest_ );
	NETWORK_STREAM("(" << pActor->id() << ") MachLogMoveToOperation::MachLogMoveToOperation (1) " << (void*)this << " callstack is:\n" << ProStackTracer() << std::endl );
}

MachLogMoveToOperation::MachLogMoveToOperation( MachLogMachine * pActor, const Path& path, bool checkForLeave, MATHEX_SCALAR tolerance )
: 	MachLogOperation( "MOVE_TO_OPERATION", MachLogOperation::MOVE_TO_OPERATION ),
	pImpl_( _NEW( MachLogMoveToOperationImpl( pActor, path, checkForLeave ) ) )

//  state_( PLANNING ),
//  pathSearch_( MachLogPlanet::instance().surface()->pSpace()->xCells() * MachLogPlanet::instance().surface()->pSpace()->yCells() )
{
	CB_MachLogMoveToOperation_DEPIMPL();

	tolerance_ = tolerance;
    dest_ = path_.front();
	NETWORK_STREAM("(" << pActor->id() << ") MachLogMoveToOperation::MachLogMoveToOperation (2) " << (void*)this << " callstack is:\n" << ProStackTracer() << std::endl );
}

MachLogMoveToOperation::MachLogMoveToOperation( MachLogMachine * pActor, const MexPoint3d& dest, size_t commandId, bool checkForLeave, MATHEX_SCALAR tolerance )
: 	MachLogOperation( "MOVE_TO_OPERATION", MachLogOperation::MOVE_TO_OPERATION ),
	pImpl_( _NEW( MachLogMoveToOperationImpl( pActor, dest, commandId, checkForLeave ) ) )

//  state_( PLANNING ),
//  pathSearch_( MachLogPlanet::instance().surface()->pSpace()->xCells() * MachLogPlanet::instance().surface()->pSpace()->yCells() )
{
    PRE( commandId != NO_ID );

	CB_MachLogMoveToOperation_DEPIMPL();

	tolerance_ = tolerance;
    path_.push_back( dest_ );
	NETWORK_STREAM("(" << pActor->id() << ") MachLogMoveToOperation::MachLogMoveToOperation (3) " << (void*)this << " callstack is:\n" << ProStackTracer() << std::endl );
}

MachLogMoveToOperation::MachLogMoveToOperation(
    MachLogMachine * pActor,
    const Path& path,
    size_t commandId,
    PhysPathFindingPriority pathFindingPriority,
    bool checkForLeave )
: MachLogOperation( "MOVE_TO_OPERATION", MachLogOperation::MOVE_TO_OPERATION, pathFindingPriority ),
  pImpl_( _NEW( MachLogMoveToOperationImpl( pActor, path, commandId, checkForLeave ) ) )

//  state_( PLANNING ),
//  pathSearch_( MachLogPlanet::instance().surface()->pSpace()->xCells() * MachLogPlanet::instance().surface()->pSpace()->yCells() )
{
    PRE( commandId != NO_ID );

	CB_MachLogMoveToOperation_DEPIMPL();

    dest_ = path_.front();
	NETWORK_STREAM("(" << pActor->id() << ") MachLogMoveToOperation::MachLogMoveToOperation (4) " << (void*)this << " callstack is:\n" << ProStackTracer() << std::endl );
}

MachLogMoveToOperation::~MachLogMoveToOperation()
{
	CB_MachLogMoveToOperation_DEPIMPL();
	NETWORK_STREAM("(" << pActor_->id() << ") MachLogMoveToOperation::~MachLogMoveToOperation " << (void*)this << std::endl );

	while( path_.size() > 0 )
		path_.erase( path_.begin() );

	_DELETE( pImpl_ );
}

void MachLogMoveToOperation::doOutputOperator( ostream& o ) const
{
	CB_MachLogMoveToOperation_DEPIMPL();

	o << "MachLogMoveToOperation, dest " << dest_ << "  pos " << pActor_->position() << std::endl;
	o << "  range " << pActor_->position().euclidianDistance( dest_ );
	if( tolerance_ != NO_TOLERANCE )
		o << " (tolerance " << tolerance_ << ")";
	o << "  path_.size " << path_.size() << std::endl;
//	o << std::endl << pActor_->motionSeq();
	// << ' ' << ( state_ == PLANNING ? "PLANNING " : "MOVING " );
}

///////////////////////////////////

bool MachLogMoveToOperation::doStart()
{
	CB_MachLogMoveToOperation_DEPIMPL();

	if( checkForLeave_ )
	{
		MachLogConstruction* pConstruction;
		bool insideABuilding = checkNeedLeaveOperation( pActor_, &pConstruction );
		if( insideABuilding and not pConstruction->globalBoundary().contains( dest_ ) )
        {
			return not checkNeedAndDoLeaveOperation( pActor_ );
        }
	}

	MexPoint2d dest( dest_ );

	HAL_STREAM("(" << pActor_->id() << ") MLMoveToOperation::doStart move to 3D " << dest_ << " 2D " << dest << std::endl );
	HAL_STREAM(" motion sequencer before destination call " << pActor_->motionSeq() << std::endl );

    if( groupMoveInfo().valid() )
    	pActor_->motionSeq().destination( dest, groupMoveInfo() );
    else
    	pActor_->motionSeq().destination( dest );

	if( tolerance_ != NO_TOLERANCE )
		pActor_->motionSeq().destinationTolerance( tolerance_ );

    pActor_->motionSeq().pathFindingPriority( pathFindingPriority() );

    if( commandId_ != NO_ID )
        pActor_->motionSeq().commandId( commandId_ );

	HAL_STREAM(" motion sequencer AFTER destination call " << pActor_->motionSeq() << std::endl );

	return true;
//	pathSearch_.restart( pActor_->coord(), dest, MachPathSearch::INCLUSIVE );
}

PhysRelativeTime MachLogMoveToOperation::doUpdate( )
{
	CB_MachLogMoveToOperation_DEPIMPL();

    //HAL_STREAM(" (" << pActor_->id() << ") MachLogMoveToOperation::doUpdate " << (void*)pActor_ << std::endl;)
	static PhysRelativeTime moveCPU = MachLogRaces::instance().stats().maxMoveTime();
	PhysRelativeTime returnTime = pActor_->motionSeq().update( moveCPU );
	if( not pActor_->motionSeq().hasDestination() )
	{
		if( path_.size() > 0 )
			path_.erase( path_.begin() );

		if( path_.size() > 0 )
		{
			//HAL_STREAM("MLMoveToOp::assigning new target based on path element.\n" );
			dest_ = path_.front();
			MexPoint2d dest( dest_ );


            if( groupMoveInfo().valid() )
    			pActor_->motionSeq().destination( dest, groupMoveInfo() );
            else
    			pActor_->motionSeq().destination( dest );

			if( tolerance_ != NO_TOLERANCE )
				pActor_->motionSeq().destinationTolerance( tolerance_ );

            pActor_->motionSeq().pathFindingPriority( pathFindingPriority() );

            if( commandId_ != NO_ID )
                pActor_->motionSeq().commandId( commandId_ );
		}
	}

	//HAL_STREAM("  return time " << returnTime << std::endl );
//	//HAL_STREAM( pActor_->motionSeq() );
	return std::min( returnTime, 2.0 );
}

void MachLogMoveToOperation::doFinish()
{
//	pActor_->motionSeq().stop();
//	pActor_->motionSeq().update( 0.01 );
    //JON_STREAM("MachLogMoveToOperation::doFinish entry" << (void*)pActor_ << std::endl;)
//	nextCallbackTime_ = 0;
    //JON_STREAM("MachLogMoveToOperation::doFinish exit" << std::endl;)
}

bool MachLogMoveToOperation::doIsFinished() const
{
	CB_MachLogMoveToOperation_DEPIMPL();

    //JON_STREAM("MachLogMoveToOperation::doIsFinished entry" << (void*)pActor_ << std::endl;)
    bool result = not pActor_->motionSeq().hasDestination() and path_.size() == 0;
    //JON_STREAM("MachLogMoveToOperation::doFinish exit: result = " << result << std::endl;)
	return result;
}

bool MachLogMoveToOperation::doBeInterrupted()
{
	CB_MachLogMoveToOperation_DEPIMPL();

	pActor_->motionSeq().stop();
	while( path_.size() > 0 )
		path_.erase( path_.begin() );
	return not pActor_->motionSeq().hasDestination();
}

void perWrite( PerOstream& ostr, const MachLogMoveToOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pImpl_;
}

void perRead( PerIstream& istr, MachLogMoveToOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pImpl_;
}

MachLogMoveToOperation::MachLogMoveToOperation( PerConstructor con )
:	MachLogOperation( con )
{
}



/* //////////////////////////////////////////////////////////////// */

static
bool withinEpsilonOf( MATHEX_SCALAR a, MATHEX_SCALAR b )
{
	return Mathex::abs( a - b ) <= MexEpsilon::instance();
}

MexRadians
angleToTurnToFace( const MachLogMachine& actor, const MexPoint3d& pos )
{
//Old	MexPoint2d actorPos( actor.position() );
//Old
//Old	MexVec2 bearingToTarget( pos.x() - actorPos.x(), pos.y() - actorPos.y() );
//Old
//Old	MexVec3 xBasis3;
//Old	actor.globalTransform().xBasis( &xBasis3 );
//Old
//Old	MexVec2 xBasis2( xBasis3 );
//Old	return xBasis2.angleBetween( bearingToTarget );

	HAL_STREAM("angleToTurnToFace actor at " << actor.position() << " to pos " << pos << std::endl );
	const MexTransform3d& trans = actor.globalTransform();
	MexPoint3d localPos( pos );
	//get the point in the local coordinate system of the machine.
	trans.transformInverse( &localPos );
	HAL_STREAM(" the point " << pos << " in the machine local space is given by " << localPos << std::endl );
	//once we have the point in local space we can construct a local space vector:
	//by definition the machine is at the origin
	MexVec3 d( localPos );
	//once we have the vector, we can construct the normal to the plane described by
	//the z axis of the machine and the direction vector d. In the local space the z-axis is of course
	//defined as 0,0,1
	MexVec3 n = MexVec3::crossProduct( d, MexVec3( 0,0,1) );
	HAL_STREAM(" normal to plane of z and d is " << n << std::endl );
	//once we have the normal to this plane it follows that the x' vector (the direction we
	//want x basis to point in after rotation must be given by e = z X n
	MexVec3 x = MexVec3::crossProduct( MexVec3( 0,0,1 ), n );
	HAL_STREAM(" x' is " << x << std::endl );
	//the angle to rotate around the z axis then becomes the angle between x-axis (defined of course as 1,0,0) and x'
	MexRadians result = MexVec3( 1,0,0 ).signedAngleBetween( x );
	HAL_STREAM( " angle between x-axis and x' is " << result << std::endl );
	return result;

}

/////////////////////////////////////////	MOVE   AND    TURN   STUFF /////////////////////////////////////////////////

PER_DEFINE_PERSISTENT( MachLogMoveAndTurnOperation );

#define CB_MachLogMoveAndTurnOperation_DEPIMPL()	\
		PRE( pImpl_ );	\
		CB_DEPIMPL( State, state_ ); \
    	CB_DEPIMPL( MachLogMachine *, pActor_ ); \
		CB_DEPIMPL( MexPoint3d, dest_ ); \
		CB_DEPIMPL( MexPoint3d, endFacing_ ); \
		CB_DEPIMPL( PhysAbsoluteTime, dontMoveUntil_ ); \
		CB_DEPIMPL( PhysRelativeTime, initialDelay_ ); \
		CB_DEPIMPL( bool, checkForLeave_ );

MachLogMoveAndTurnOperation::MachLogMoveAndTurnOperation( MachLogMachine * pActor,
												const MexPoint3d& dest,
												const MexPoint3d& endFacing,
												bool checkForLeave,
												PhysRelativeTime initialDelay )
: 	MachLogOperation( "MOVE_AND_TURN_OPERATION", MachLogOperation::MOVE_AND_TURN_OPERATION ),
	pImpl_( _NEW( MachLogMoveAndTurnOperationImpl( pActor, dest, endFacing, checkForLeave, initialDelay ) ) )
{
	// Intentionally Empty
}

MachLogMoveAndTurnOperation::~MachLogMoveAndTurnOperation()
{
	_DELETE( pImpl_ );
}

void MachLogMoveAndTurnOperation::doOutputOperator( ostream& o ) const
{
	CB_MachLogMoveAndTurnOperation_DEPIMPL();

	o << "MachLogMoveAndTurnOperation, " << dest_ << std::endl;
	o << " angleToTurnToFace " << angleToTurnToFace( *pActor_, endFacing_ ).asScalar() << std::endl;
	o << " Old angle to turn to face ";
	const MachLogMachine& actor = *pActor_;
	const MexPoint3d pos( endFacing_ );
	MexPoint2d actorPos( actor.position() );
	MexVec2 bearingToTarget( pos.x() - actorPos.x(), pos.y() - actorPos.y() );
	MexVec3 xBasis3;
	actor.globalTransform().xBasis( &xBasis3 );
	MexVec2 xBasis2( xBasis3 );

	o << xBasis2.angleBetween( bearingToTarget ) << std::endl;

	o << " pSubOperation_ is " << (void*)pSubOperation() << std::endl;

	o << pActor_->motionSeq() << std::endl;
}

///////////////////////////////////

bool MachLogMoveAndTurnOperation::doStart()
{
	CB_MachLogMoveAndTurnOperation_DEPIMPL();

	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();

	// refresh dontMoveUntil_ parameter
	dontMoveUntil_ = timeNow + initialDelay_;


	if( checkForLeave_ )
	{
		MachLogConstruction* pConstruction;
		bool insideABuilding = checkNeedLeaveOperation( pActor_ , &pConstruction );
		if( insideABuilding and not pConstruction->globalBoundary().contains( dest_ ) )
			return not checkNeedAndDoLeaveOperation( pActor_ );
	}



	if( timeNow >= dontMoveUntil_ )
	{
		MexPoint2d dest( dest_ );

		if( groupMoveInfo().valid() )
	    	pActor_->motionSeq().destination( dest, groupMoveInfo() );
	    else
	    	pActor_->motionSeq().destination( dest );

		state_ = MOVING;
	}


    pActor_->motionSeq().pathFindingPriority( pathFindingPriority() );

	return true;
}

PhysRelativeTime MachLogMoveAndTurnOperation::doUpdate(  )
{
	CB_MachLogMoveAndTurnOperation_DEPIMPL();

	if( state_ == WAITING )
	{
		// take account of possible initial delay.
		PhysAbsoluteTime timeNow = SimManager::instance().currentTime();
		if( timeNow < dontMoveUntil_ )
			return( dontMoveUntil_ - timeNow );
		else
		{
			MexPoint2d dest( dest_ );

			// time to start moving
			if( groupMoveInfo().valid() )
		    	pActor_->motionSeq().destination( dest, groupMoveInfo() );
		    else
		    	pActor_->motionSeq().destination( dest );

			state_ = MOVING;
		}
	}


	PhysRelativeTime interval = 5.0;
	if( pActor_->motionSeq().hasDestination() )
		interval = pActor_->motionSeq().update( 0.007 );
	else
	{
		MexRadians turnBy = angleToTurnToFace( *pActor_, endFacing_ );
		if( not withinEpsilonOf( angleToTurnToFace( *pActor_, endFacing_ ).asScalar(), 0.0 ) )
			subOperation( pActor_, _NEW( MachLogTurnAnimation( pActor_, turnBy ) ) );
	}
	return interval;
}

void MachLogMoveAndTurnOperation::doFinish()
{
	// Intentionally Empty
}

bool MachLogMoveAndTurnOperation::doIsFinished() const
{
	CB_MachLogMoveAndTurnOperation_DEPIMPL();

	// if actor is standing ground, always return true to cancel move op UNLESS one of superops is enter/exit
	//	building op, in which
	if ( pActor_->isStandingGround()
				and
			not ( pActor_->strategy().currentOperationType() == MachLogOperation::ENTER_OPERATION
				or  pActor_->strategy().currentOperationType() == MachLogOperation::LEAVE_OPERATION ) )
	{
		return true;
	}

	// otherwise, use this test to return boolean value
	return ( not pActor_->motionSeq().hasDestination()
		and withinEpsilonOf( angleToTurnToFace( *pActor_, endFacing_ ).asScalar(), 0.0 ) );
}

bool MachLogMoveAndTurnOperation::doBeInterrupted()
{
	CB_MachLogMoveAndTurnOperation_DEPIMPL();

	state_ = WAITING;
	pActor_->motionSeq().stop();
	return true;
}

void perWrite( PerOstream& ostr, const MachLogMoveAndTurnOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pImpl_;
}

void perRead( PerIstream& istr, MachLogMoveAndTurnOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pImpl_;
}

MachLogMoveAndTurnOperation::MachLogMoveAndTurnOperation( PerConstructor con )
:	MachLogOperation( con )
{
}

/* //////////////////////////////////////////////////////////////// */

/* End MOVE.CPP *****************************************************/

