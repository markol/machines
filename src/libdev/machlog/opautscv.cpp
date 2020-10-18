/*
 * O P A U T S C V . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved.
 */

#include "machlog/opautscv.hpp"

#include "machlog/internal/opautsci.hpp"

#include "sim/manager.hpp"

#include "phys/motchunk.hpp"

//#include "machphys/machdata.hpp"
#include "machphys/machine.hpp"
#include "machphys/mcmovinf.hpp"

#include "machlog/opscav.hpp"
#include "machlog/debris.hpp"
#include "machlog/rescarr.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/strategy.hpp"


PER_DEFINE_PERSISTENT( MachLogAutoScavengeOperation );

#define CB_MachLogAutoScavengeOperation_DEPIMPL()	\
		PRE( pImpl_ );	\
		CB_DEPIMPL( MachLogResourceCarrier*, pScavenger_ ); \
		CB_DEPIMPL( MachLogDebris*, pDebris_ ); \
    	CB_DEPIMPL( bool, finished_ ); \
		CB_DEPIMPL( bool, initiatedScavengeOp_ ); \		
		CB_DEPIMPL( MachLogOperation*, pCachedOperation_ ); 
 
/* //////////////////////////////////////////////////////////////// */

MachLogAutoScavengeOperation::MachLogAutoScavengeOperation( MachLogResourceCarrier * pScavenger, MachLogDebris* pDebris )
:	MachLogOperation( "AUTOSCAVENGE_OPERATION", MachLogOperation::AUTOSCAVENGE_OPERATION ),
	pImpl_( _NEW( MachLogAutoScavengeOperationImpl( pScavenger, pDebris ) ) )	
{
	CB_MachLogAutoScavengeOperation_DEPIMPL();

	// PRE( pScavenger->isScavenger() );

	pDebris_->attach( this );

}
/* //////////////////////////////////////////////////////////////// */

MachLogAutoScavengeOperation::~MachLogAutoScavengeOperation()
{
	CB_MachLogAutoScavengeOperation_DEPIMPL();
	
	// if we still have a pointer to the cached op, we must be terminating through circumstances where that
	// that op has NOT been restored to the strategy. We must delete it ourselves to prevent a memory leak.
	if( pCachedOperation_ )
		_DELETE( pCachedOperation_ );	

	if( pDebris_ )
		pDebris_->detach( this );

	_DELETE( pImpl_ );
}

// virtual
bool MachLogAutoScavengeOperation::beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int /*clientData*/ )
{
	CB_MachLogAutoScavengeOperation_DEPIMPL();	
	
	ASSERT( pSubject == (W4dSubject*)pDebris_, "Notified by unexpected subject." );

	bool stayAttached = true;

	switch( event )
	{
	case W4dSubject::DELETED:		
		pDebris_ = NULL;
		nextCallBackTime( SimManager::instance().currentTime() ); 		
		stayAttached = false;

		break;
	
	default:
		;
	}

	return stayAttached;
}


/* //////////////////////////////////////////////////////////////// */

void MachLogAutoScavengeOperation::storeOldFirstOperation( MachLogOperation* pOldOp )
{
	CB_MachLogAutoScavengeOperation_DEPIMPL();

	pCachedOperation_ = pOldOp;	
	POST( pCachedOperation_ != NULL );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogAutoScavengeOperation::doOutputOperator( ostream& o ) const
{	
	CB_MachLogAutoScavengeOperation_DEPIMPL();

	o << "MachLogAutoScavengeOperation\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogAutoScavengeOperation::doStart()
{
   	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

PhysRelativeTime MachLogAutoScavengeOperation::doUpdate()
{
	CB_MachLogAutoScavengeOperation_DEPIMPL();

	PRE( not isFinished() );
	PRE( pScavenger_ != NULL );
	
	ASSERT( pCachedOperation_ != NULL, "MachLogAutoScavengeOperation::doUpdate : Hey, we shouldn't have a null pointer for the cached op by now!" );
	// get hold of our current strong threats
				
	// have we now met any of the conditions for terminating this operation?
	if ( shouldStopScavenging() )
	{		
		// restore previous first operation as first (and only) operation on the strategy queue.
		pScavenger_->strategy().newOperation( pCachedOperation_, false );
		pCachedOperation_ = NULL;	// so we don't try and delete it in the destructor
		finished_ = true;
		return 0.0;	
	}
		
	// gadzooks! The AutoScavenge must continue.

	ASSERT( pDebris_ != NULL, "pDebris_ shouldn't be NULL if we've decided not to break off the attack." );
	
	// nothing to do from here if we've already	started attacking except wait for termination one way or the other
	if( initiatedScavengeOp_ )
		return 2.0;

	// otherwise, it's time to START doing something.
		
	PhysRelativeTime interval = 0.0;
		
	MachLogOperation* pNewSubOp = _NEW( MachLogScavengeOperation( pScavenger_, pDebris_ ) );

	subOperation( pScavenger_, pNewSubOp );

	initiatedScavengeOp_ = true;
		   
   	return interval;			//!! Have to alter interval if we have a subop. Probably.
		
}

bool MachLogAutoScavengeOperation::shouldStopScavenging()
{
	CB_MachLogAutoScavengeOperation_DEPIMPL();

	bool result = false;

	if( pDebris_ == NULL )
		result = true;
	else if( initiatedScavengeOp_ )
	{
		if( not pSubOperation() )
			result = true;		
	}

	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogAutoScavengeOperation::doFinish()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogAutoScavengeOperation::doIsFinished() const
{
	CB_MachLogAutoScavengeOperation_DEPIMPL();

	// !! Code in here to check if strong threats are detected.	
	return finished_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogAutoScavengeOperation::doBeInterrupted()
{
	CB_MachLogAutoScavengeOperation_DEPIMPL();

	//HAL_STREAM("(" << pScavenger_->id() << ") MLHealOp::doBeInterrupted\n" );
	pScavenger_->motionSeq().stop();
	initiatedScavengeOp_ = false;
	return true;
}


/////////////////////////////////////////////////// persistence /////////////////////////////////////////////////////

void perWrite( PerOstream& ostr, const MachLogAutoScavengeOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pImpl_;
}

void perRead( PerIstream& istr, MachLogAutoScavengeOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pImpl_;

    //Reinstate observer relation
	if( op.pImpl_->pDebris_ )
		op.pImpl_->pDebris_->attach( &op );	
}

MachLogAutoScavengeOperation::MachLogAutoScavengeOperation( PerConstructor con )
:	MachLogOperation( con )
{
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////


// End of code for MachLogAutoScavengeOperation


/* End OPAUTSCV.CPP *************************************************/