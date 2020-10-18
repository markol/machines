/*
 * S T R A T E G Y . C P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#include "machlog/strategy.hpp"

#include "stdlib/string.hpp"

#include "ctl/pvector.hpp"
#include "ctl/list.hpp"		// only needed if we need to have a "path" move op in the forceAsSubOp

#include "mathex/poly2d.hpp"
#include "mathex/transf3d.hpp"

#include "sim/manager.hpp"

#include "phys/motchunk.hpp"

#include "machphys/machine.hpp"
#include "machphys/random.hpp"

#include "machlog/actor.hpp"
#include "machlog/constron.hpp"
#include "machlog/inout.hpp"
#include "machlog/machine.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/move.hpp"
#include "machlog/opautscv.hpp"
#include "machlog/opcounte.hpp"
#include "machlog/opevade.hpp"
#include "machlog/rescarr.hpp"
#include "machlog/spacial.hpp"

PER_DEFINE_PERSISTENT( MachLogStrategy );

class MachLogStrategyImpl
{
public:
	PER_MEMBER_PERSISTENT_DEFAULT( MachLogStrategyImpl );
	PER_FRIEND_READ_WRITE( MachLogStrategyImpl );

private:
	MachLogOperation*		pPendingOperation_;
	bool					mustRemoveAllOperations_;
	bool					semaphoreInUpdate_;
	MachLogStrategy::Queue	queue_;
	MachActor*				pActor_;
	PhysRelativeTime 		nextUpdateTime_;

	friend class MachLogStrategy;
	friend ostream& operator <<( ostream& o, const MachLogStrategy& s );

	MachLogStrategyImpl()
	: pPendingOperation_( NULL ),
	  mustRemoveAllOperations_( false )

	{
	};

	~MachLogStrategyImpl()
	{
	};
};

PER_DECLARE_PERSISTENT( MachLogStrategyImpl );
PER_DEFINE_PERSISTENT( MachLogStrategyImpl );

// De-pImpl macro to get access to pImpl data members
#define CB_MACHLOGSTRATEGY_DEPIMPL() \
	CB_DEPIMPL( MachActor*, pActor_ ); \
	CB_DEPIMPL( bool, semaphoreInUpdate_ ); \
	CB_DEPIMPL( bool, mustRemoveAllOperations_ ); \
	CB_DEPIMPL( MachLogStrategy::Queue, queue_ ); \
	CB_DEPIMPL( MachLogOperation*, pPendingOperation_ ); \
	CB_DEPIMPL( PhysRelativeTime, nextUpdateTime_ );

/* //////////////////////////////////////////////////////////////// */
MachLogStrategy::MachLogStrategy( MachActor* pActor )
:	pImpl_( _NEW( MachLogStrategyImpl ) )
{
	CB_MACHLOGSTRATEGY_DEPIMPL();

	pActor_ = pActor;
	semaphoreInUpdate_ = false;
    queue_.reserve( 8 );
}

MachLogStrategy::~MachLogStrategy()
{	
	CB_MACHLOGSTRATEGY_DEPIMPL();

	//HAL_STREAM("(" << pActor_->id() << ") MLStrategy is being deleted ops: " << queue_.size() << std::endl );
	while( queue_.size() > 0 )
	{
		//HAL_STREAM( *queue_.begin() );
		_DELETE( *queue_.begin() );
		queue_.erase( queue_.begin() );
		//HAL_STREAM("  MLStrategy:: after delete queue_.size() " << queue_.size() << std::endl );
	}
	if( pPendingOperation_ )
		_DELETE( pPendingOperation_ );

	_DELETE( pImpl_ );
}

bool MachLogStrategy::isFinished() const
{
	CB_MACHLOGSTRATEGY_DEPIMPL();

	return queue_.size() == 0 and pPendingOperation_ == NULL;
}

void MachLogStrategy::newOperation( MachLogOperation * pNewOperation, bool subOperation )
{
	CB_MACHLOGSTRATEGY_DEPIMPL();

	if( pActor_->isDead() )
	{
		// Don't accept the op. In fact, delete it, instead.
		_DELETE( pNewOperation );
	}
	else
	{
		// ensure that all ops put on as new are treated as having to START.
		// this is important for cases where an old op has been restored after suspension, and
		// might have last been in an UPDATE state etc.
		pNewOperation->progress( MachLogOperation::START );
		HAL_STREAM("(" << pActor_->id() << ") MLStrategy::newOperation " << pNewOperation->operationTypeAsString() << std::endl );
		if( subOperation )
			queue_.push_back( pNewOperation );
		else
		{
			if( pPendingOperation_ )
			{
				_DELETE( pPendingOperation_ );
				pPendingOperation_ = NULL;
			}
			HAL_STREAM(" semaphoreInUpdate_ " << semaphoreInUpdate_ << std::endl );
			tryToRemoveAllOperations();

			if( queue_.size() > 0 )
			{
				HAL_STREAM(" operations still exist so setting pPendingOperation_\n" );
				pPendingOperation_ = pNewOperation;
			}
			else
			{
				queue_.push_back( pNewOperation );
			}
		}	
	}
}

bool MachLogStrategy::addOperationAsSubOperationToFollowOperation( MachLogOperation * pNewOperation )
{
	CB_MACHLOGSTRATEGY_DEPIMPL();

  	if( queue_.size() == 0 )
	{
		return false;	
	}
	
 	bool found = false;
	bool succeeded = false;
	for( Queue::iterator i = queue_.begin(); not found and i < queue_.end(); ++i )
	{
		MachLogOperation* pOp = (*i);
		 
		if( pOp->operationType() == MachLogOperation::FOLLOW_OPERATION )
		{	
			found = true;
			if( not pOp->hasSubOperation() )
			{
				pOp->subOperation( pActor_, pNewOperation );
				succeeded = true;
			}
		}
	}
	
	return succeeded;	
}

PhysRelativeTime
MachLogStrategy::update( const PhysRelativeTime& )
{
	CB_MACHLOGSTRATEGY_DEPIMPL();

	PRE( not isFinished() );

	if( 	semaphoreInUpdate_ 
		or 	pActor_->busy() )
	{
		// immediate abortion of strategy update	

		return 0.0;
	}

	if( pActor_->objectIsMachine() and pActor_->asMachine().insideAPC() )
	{
		// immediate abortion of strategy update	
		return 3.0;
	}


	PhysRelativeTime result = 2.0;
	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();
	//don't use iterators! The pointers may become invalid during the loop.
	int i = 0;
	bool finished = false;

	//do we have a pending operation? If so try to remove all the other operations on the queue
	if( pPendingOperation_ )
	{
		HAL_STREAM(" have a pPendingOp calling try to remove all ops\n" );

		//if we MUSt remove all operations (due to an explosion animation) then do this first.
		if( mustRemoveAllOperations_ )
			beInterrupted();
		tryToRemoveAllOperations();

		if( queue_.size() == 0 )
		{
			queue_.push_back( pPendingOperation_ );
			pPendingOperation_ = NULL;
			i = 0;
		}
	}

	if( queue_.size() > 0 )
	{
		HAL_STREAM(" we have some ops so calling at least one update " << queue_.size() << std::endl );
		size_t queueOrigSize = queue_.size();
		//Mark the update stack in a state where you cannot remove the current operation out of sequence.
		semaphoreInUpdate_ = true;
		while( not finished )
		{
			ASSERT_INFO( pActor_->id() );
			ASSERT_INFO( pActor_->objectType() );
			ASSERT( i < queue_.size(),"Referencing an element not there in strategy" );
			MachLogOperation* pOp = queue_[ i ];
			HAL_STREAM(" derefing first op " << pOp->operationTypeAsString() << std::endl );
			bool increment = true;
			if( pOp->isFinished() /*or pOp->doIsFinished() */)	//need to do this but bedestroyed animtaion needs upgrading
			{
				for( Queue::iterator j = queue_.begin(); j != queue_.end(); ++j )
					if( (*j)->pSubOperation() == pOp )
						(*j)->pSubOperation( NULL );
				if( pPendingOperation_ and pPendingOperation_->pSubOperation() == pOp )
					pPendingOperation_->pSubOperation( NULL );
					
				_DELETE( pOp );
				queue_.erase( queue_.begin() + i );
				increment = false;
				finished = true;
			}
			else
			{
				if( pOp->update() <= timeNow )
				{
					PhysAbsoluteTime nextTime = timeNow;
					if( pOp->progress() == MachLogOperation::START )
					{
						//doStart will have to call checkNeedLeaveBuildingOperation
						//if dostart returns false then this operation must be performed first and
						//the current operation will have to wait.
						if ( pOp->doStart() )
						{
							pOp->progress( MachLogOperation::UPDATE );
						}
						else
						{
							//if we need to do a MLLeaveBuildingOperation then swap this operation for the
							//pendingOperation. Erase operation from stack.
							if( pPendingOperation_ )
							{
								_DELETE( pPendingOperation_ );
							}
							pPendingOperation_ = pOp;
							queue_.erase( queue_.begin() + i );
							increment = false;
							finished = true;
						}
					}

					if( not finished )
					{
						MachLogOperation* pOpBefore = pOp;
						if( pOp->progress() != MachLogOperation::START )
						{
							//Two seperate checks for not finished followed by finished as it may have nowe finished
							if( not pOp->doIsFinished() )
							{
								//PhysAbsoluteTime startTime = Phys::time();
								result = pOp->doUpdate();
								nextTime += result;							
								//PhysAbsoluteTime endTime = Phys::time();
							}

							pOp->nextCallBackTime( nextTime );
							if( pOp->doIsFinished() )
							{
								pOp->progress( MachLogOperation::FINISHED );
								pOp->doFinish();
								for( Queue::iterator j = queue_.begin(); j != queue_.end(); ++j )
									if( (*j)->pSubOperation() == pOp )
										(*j)->pSubOperation( NULL );
								if( pPendingOperation_ and pPendingOperation_->pSubOperation() == pOp )
									pPendingOperation_->pSubOperation( NULL );
								_DELETE( pOp );
								queue_.erase( queue_.begin() + i );
								increment = false;
								finished = true;
							}
						}
						else
							pOp->nextCallBackTime( nextTime );
					}
				}
			}

			if( increment )
				++i;
			if( queueOrigSize > queue_.size() or i >= queue_.size() )
				finished = true;
		}
	}
	semaphoreInUpdate_ = false;
	return result;
}

const string& MachLogStrategy::currentOperationTypeAsString() const
{
	CB_MACHLOGSTRATEGY_DEPIMPL();

	static string none("NONE");
	if( queue_.size() == 0 )
		return none;
	return queue_[ 0 ]->operationTypeAsString();
}

MachLogOperation::OperationType MachLogStrategy::currentOperationType() const
{
	CB_MACHLOGSTRATEGY_DEPIMPL();

	if( queue_.size() == 0 )
		return MachLogOperation::N_OPERATIONS;
	return queue_[0]->operationType();
}
/* //////////////////////////////////////////////////////////////// */

void MachLogStrategy::tryToRemoveAllOperations()
{
	CB_MACHLOGSTRATEGY_DEPIMPL();

	if( semaphoreInUpdate_ )
		return;

	if( queue_.size() > 0 )
	{
		int i = 0;
		bool finished = false;
		while( not finished )
		{
			ASSERT_INFO( pActor_->id() );
			ASSERT_INFO( pActor_->objectType() );
			ASSERT( i < queue_.size(),"Referencing element not there in strategy tryToRemoveAllOps\n" );
			MachLogOperation* pOp = queue_[ i ];
			if( pOp->doBeInterrupted() )
			{
				for( Queue::iterator j = queue_.begin(); j != queue_.end(); ++j )
					if( (*j)->pSubOperation() == pOp )
						(*j)->pSubOperation( NULL );
				if( pPendingOperation_ and pPendingOperation_->pSubOperation() == pOp )
					pPendingOperation_->pSubOperation( NULL );
				_DELETE( pOp );
				queue_.erase( queue_.begin() + i );
			}
			else
			{
				finished = true;
			}
			if( i >= queue_.size() )
			{
				finished = true;
			}
		}
	}
}


void MachLogStrategy::removeAllOperations()
{
	CB_MACHLOGSTRATEGY_DEPIMPL();

	PRE( not isUninterruptable() );

	if( semaphoreInUpdate_ )
		return;
				
	if( queue_.size() > 0 )
	{
		bool finished = false;
		while( not finished )
		{
			ASSERT_INFO( pActor_->id() );
			ASSERT_INFO( pActor_->objectType() );
			ASSERT( queue_.size() > 0, "Referencing element not there in strategy removeAllOperations" );
			MachLogOperation* pOp = queue_[ 0 ];
			pOp->doBeInterrupted();
			_DELETE( pOp );
		   	queue_.erase( queue_.begin() );
		   
		   	if( queue_.size() == 0 )
			{
				finished = true;
			}
		}
		if( pPendingOperation_ )
		{
			_DELETE( pPendingOperation_ );
			pPendingOperation_ = NULL;	
		}
		   
	}
	POST( queue_.size() == 0 );
}


/* //////////////////////////////////////////////////////////////// */

bool MachLogStrategy::isUninterruptable()
{
	CB_MACHLOGSTRATEGY_DEPIMPL();

	if( queue_.size() == 0 )
	{
		return false;	
	}
		
	
	int i = 0;
 	bool found = false;
	for( int i = 0; not found and i < queue_.size(); ++i )
	{
		ASSERT_INFO( pActor_->id() );
		ASSERT_INFO( pActor_->objectType() );
		ASSERT( i < queue_.size() , "Referencing element not there in strategy isUninterruptable" );
		MachLogOperation* pOp = queue_[ i ];
		 
		if( not pOp->isInterruptable() )
		{	
			found = true;
		}
	}
	
	return found;
}

/* //////////////////////////////////////////////////////////////// */

void MachLogStrategy::changeToEvadeMode()
{
	CB_MACHLOGSTRATEGY_DEPIMPL();

	HAL_STREAM("(" << pActor_->id() << ") MachLogStrategy::changeToEvadeMode\n" );
	PRE( pActor_->objectIsMachine() );
	PRE( not isUninterruptable() );	
	
	MachLogMachine* pMachine = &( pActor_->asMachine() );
	
	MachLogEvadeOperation* pEvadeOp = _NEW( MachLogEvadeOperation( pMachine ) );

	MachLogOperation* pOldTopOp = NULL;

	if( extractTopOperation( &pOldTopOp ) )
		removeAllOperations();
	else
	{
		pOldTopOp = _NEW( MachLogMoveToOperation( pMachine, pMachine->position(), true, 10.0 ) );	//10m tolerance		
		// removeAllOperations();	
	}

	ASSERT( pOldTopOp, "Unexpected NULL for pOldTopOp!" );

	pEvadeOp->storeOldFirstOperation( pOldTopOp );
		
	// make the evade operation the new (and only) op on the queue. The "old first op" will be restored 
	// if the evade op terminates with the actor still alive	
	newOperation( pEvadeOp, false );	
}

void MachLogStrategy::changeToCounterattackMode( MachActor* pTarget )
{
	CB_MACHLOGSTRATEGY_DEPIMPL();

	HAL_STREAM("(" << pActor_->id() << ") MachLogStrategy::changeToEvadeMode\n" );
	PRE( pActor_->objectIsMachine() );
	PRE( not isUninterruptable() );	
	
	MachLogMachine* pMachine = &( pActor_->asMachine() );
	
	MachLogCounterattackOperation* pCounterattackOp = _NEW( MachLogCounterattackOperation( pMachine, pTarget ) );
	
	MachLogOperation* pOldTopOp = NULL;

	if( extractTopOperation( &pOldTopOp ) )
		removeAllOperations();
	else
	{
		/*
		MexPoint3d point2mForwardInDirectionFaced( 2, 0, 0 );
		pMachine->physObject().globalTransform().transform( &point2mForwardInDirectionFaced );
		*/

		// this puts a bit of nice variation in the time taken to stand and survey the battlefield by
		// a group of counterattacking machines after vanquishing enemies before wheeling back to positions.
		// Also, note that more heavily-wounded machines will tend to retreat first.
		MATHEX_SCALAR postKillPause = ( 2.0 * pActor_->hpRatio() ) + MachPhysRandom::randomDouble( 0, 2.0 );

		// now turns towards point first sighted enemy at after returning to initial position
		pOldTopOp = _NEW( MachLogMoveAndTurnOperation( pMachine, pMachine->position(), pTarget->position(), true, postKillPause ) );		
		// removeAllOperations();	
	}

	ASSERT( pOldTopOp, "Unexpected NULL for pOldTopOp!" );

	pCounterattackOp->storeOldFirstOperation( pOldTopOp );
		
	// make the counterattack operation the new (and only) op on the queue. The "old first op" will be restored 
	// if the counterattack op terminates with the actor still alive	
	newOperation( pCounterattackOp, false );	
}


ostream& operator <<( ostream& o, const MachLogStrategy& s )
{
	if( s.queue().size() == 0 )	
	{
		o << "Strategy: idle\n";
		if( s.pPendingOperation() )
			o << " PendingOperation available\n";
	}
	else
	{
		o << "Strategy Queue Entries: " << s.queue().size()<< " Time Now " << SimManager::instance().currentTime() << std::endl;
		if( s.pPendingOperation() )
			o << " PendingOperation available\n";
		if( s.queue().size() > 0 )
		{
			for( int k = 0; k < s.queue().size(); ++k )
			{
				MachLogOperation* pOp = s.queue()[ k ];
				o << " " << k << " " << pOp->update() << " " << (void*)pOp << " " << pOp->operationTypeAsString();
				if( k < 20 )
					o << std::endl;
				else
					o << "::";
			}
			o << std::endl << " " << *s.queue()[ 0 ] << std::endl;
		}
	}

	return o;
}

void MachLogStrategy::beInterrupted()
{
	CB_MACHLOGSTRATEGY_DEPIMPL();

	if( pActor_->objectIsMachine() and pActor_->asMachine().motionSeq().hasDestination() )
	{
		pActor_->asMachine().motionSeq().stop();
	}

	while( queue_.size() > 0 and not semaphoreInUpdate_ )
	{
		_DELETE( *queue_.begin() );
		queue_.erase( queue_.begin() );
	}
	//if mustRemoveAllOperations is true then the pPending is still needed.
	if( pPendingOperation_ and not mustRemoveAllOperations_ )
	{
		_DELETE( pPendingOperation_ );
		pPendingOperation_ = NULL;
	}

	if( semaphoreInUpdate_ )
		mustRemoveAllOperations_ = true;
}

MachLogOperation& MachLogStrategy::currentOperation()
{
	CB_MACHLOGSTRATEGY_DEPIMPL();

	PRE( queue_.size() > 0 );
	return *queue_[ 0 ];
}

const MachLogOperation& MachLogStrategy::currentOperation() const
{
	CB_MACHLOGSTRATEGY_DEPIMPL();

	PRE( queue_.size() > 0 );
	return *queue_[ 0 ];
}

void MachLogStrategy::setAllOperationsToRestart()
{
	CB_MACHLOGSTRATEGY_DEPIMPL();

	for( int i = 0; i < queue_.size(); ++i )
		if( queue_[i]->progress() == MachLogOperation::UPDATE )
			queue_[i]->progress( MachLogOperation::START );
}

MachActor& MachLogStrategy::actor() 
{
	CB_MACHLOGSTRATEGY_DEPIMPL();

	return *pActor_; 
}

const MachActor& MachLogStrategy::actor() const 
{
	CB_MACHLOGSTRATEGY_DEPIMPL();
 
	return *pActor_; 
}

const MachLogStrategy::Queue& MachLogStrategy::queue() const
{
	CB_MACHLOGSTRATEGY_DEPIMPL();

	return queue_;
}

const MachLogOperation* MachLogStrategy::pPendingOperation() const
{
	CB_MACHLOGSTRATEGY_DEPIMPL();

	return pPendingOperation_;
}

void perWrite( PerOstream& ostr, const MachLogStrategy& strategy )
{
 	ostr << strategy.pImpl_;
}

void perRead( PerIstream& istr, MachLogStrategy& strategy )
{
	istr >> strategy.pImpl_;

	//A lot of operations will go horribly wrong if we don't force restart them
	strategy.setAllOperationsToRestart();
}

MachLogStrategy::MachLogStrategy( PerConstructor )
{
}

void perWrite( PerOstream& ostr, const MachLogStrategyImpl& impl )
{
	ostr << impl.pPendingOperation_;
	ostr << impl.mustRemoveAllOperations_;
	ostr << impl.queue_;
	ostr << impl.pActor_;
	ostr << impl.nextUpdateTime_;
	ostr << impl.semaphoreInUpdate_;
}

void perRead( PerIstream& istr, MachLogStrategyImpl& impl )
{
	istr >> impl.pPendingOperation_;
	istr >> impl.mustRemoveAllOperations_;
	istr >> impl.queue_;
	istr >> impl.pActor_;
	istr >> impl.nextUpdateTime_;
	istr >> impl.semaphoreInUpdate_;
}

bool MachLogStrategy::extractTopOperation( MachLogOperation** ppOldTopOp )
{
	CB_MACHLOGSTRATEGY_DEPIMPL();

	bool hadTopOp = false;

 	if( pPendingOperation_ )
	{
		*ppOldTopOp = pPendingOperation_;
		pPendingOperation_ = NULL;	

		hadTopOp = true;
	}	
	else if( queue_.size() > 0 )
	{
		MachLogOperation* pOldTopOp = queue_[ 0 ];
		
		//ensure that the old top op doesn't have a dangling subop pointer - if it has a subop,
		// that's now about to be deleted......
		pOldTopOp->pSubOperation( NULL );
		
		pOldTopOp->doBeInterrupted();
		
		// knock the pointer to the top op from the queue.
		queue_.erase( queue_.begin() );	

		*ppOldTopOp = pOldTopOp;

		hadTopOp = true;
	}

	return hadTopOp;
}



void MachLogStrategy::changeToScavengeMode( MachLogDebris* pDebris )
{
	CB_MACHLOGSTRATEGY_DEPIMPL();

	HAL_STREAM("(" << pActor_->id() << ") MachLogStrategy::changeToEvadeMode\n" );
	PRE( pActor_->objectType() == MachLog::RESOURCE_CARRIER and pActor_->asResourceCarrier().isScavenger() );
	PRE( not isUninterruptable() );	
	
	MachLogResourceCarrier* pScavenger = &( pActor_->asResourceCarrier() );
	
	MachLogAutoScavengeOperation* pAutoScavengeOp = _NEW( MachLogAutoScavengeOperation( pScavenger, pDebris ) );
	
	MachLogOperation* pOldTopOp = NULL;

	if( extractTopOperation( &pOldTopOp ) )
		removeAllOperations();
	else
	{
		pOldTopOp = _NEW( MachLogMoveToOperation( pScavenger, pScavenger->position(), true, 10.0 ) );	//10m tolerance		
	}

	ASSERT( pOldTopOp, "Unexpected NULL for pOldTopOp!" );

	pAutoScavengeOp->storeOldFirstOperation( pOldTopOp );
		
	// make the scavenge operation the new (and only) op on the queue. The "old first op" will be restored 
	// if the scavenge op terminates with the actor still alive	
	newOperation( pAutoScavengeOp, false );	
}

bool MachLogStrategy::isDoingLeaveOperation() const
{
	CB_MACHLOGSTRATEGY_DEPIMPL();

  	if( queue_.size() == 0 )
	{
		return false;	
	}
	
 	bool found = false;

	for( Queue::iterator i = queue_.begin(); not found and i < queue_.end(); ++i )
	{
		MachLogOperation* pOp = (*i);
		 
		if( pOp->operationType() == MachLogOperation::LEAVE_OPERATION )
		{	
			found = true;
		}
	}
	
	return found;	
}

bool MachLogStrategy::isEnteringBuilding( const MachLogConstruction& constron ) const
{
	CB_MACHLOGSTRATEGY_DEPIMPL();

  	if( queue_.size() == 0 )
	{
		return false;	
	}
	
 	bool result = false;

	for( Queue::iterator i = queue_.begin(); not result and i < queue_.end(); ++i )
	{
		MachLogOperation* pOp = (*i);
		 
		if( pOp->operationType() == MachLogOperation::ENTER_OPERATION
			and _STATIC_CAST( const MachLogEnterBuildingOperation*, pOp )->destination().id() == constron.id() )
		{	
			result = true;
		}
	}
	
	return result;	
}

bool MachLogStrategy::queueIsEmpty() const
{
	CB_MACHLOGSTRATEGY_DEPIMPL();
	
	return queue_.size() == 0;	
}	

const MachLogOperation& MachLogStrategy::operationCurrentlyExecuting() const
{
	CB_MACHLOGSTRATEGY_DEPIMPL();

	PRE( queue_.size() > 0 );
	return *queue_[ queue_.size()-1 ];
}

/* //////////////////////////////////////////////////////////////// */

/* End STRATEGY.CTP *************************************************/
