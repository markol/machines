/*
 * O P C O U N T  E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved.
 */

#include "machlog/opcounte.hpp"

#include "machlog/internal/opcounti.hpp"

#include "sim/manager.hpp"

#include "phys/motchunk.hpp"

#include "machphys/machdata.hpp"
#include "machphys/machine.hpp"
//#include "machphys/mcmovinf.hpp"

#include "machlog/attack.hpp"
#include "machlog/machine.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/races.hpp"
#include "machlog/strategy.hpp"


PER_DEFINE_PERSISTENT( MachLogCounterattackOperation );

//dereferencing all these will cause a hit in the debug version - in the release version the compiler will remove
//any unreferenced variables.
#define CB_MachLogCounterattackOperation_DEPIMPL()	\
		PRE( pImpl_ );	\
		CB_DEPIMPL( MachLogMachine*, pActor_ ); \
		CB_DEPIMPL( MachActor*, pTarget_ ); \
    	CB_DEPIMPL( bool, finished_ ); \
		CB_DEPIMPL( bool, initiatedAttackOp_ ); \
		CB_DEPIMPL( MachLogOperation*, pCachedOperation_ ); \
		CB_DEPIMPL( PhysAbsoluteTime, nextTimeINeedToCheckImClosing_ ); \
		CB_DEPIMPL( MATHEX_SCALAR, distanceBeyondWeaponRangeLastTimeIChecked_ ); \
		CB_DEPIMPL( PhysAbsoluteTime, lastTimeTargetWasntEvading_ );

/* //////////////////////////////////////////////////////////////// */

MachLogCounterattackOperation::MachLogCounterattackOperation( MachLogMachine * pActor, MachActor* pTarget )
:	MachLogOperation( "COUNTERATTACK_OPERATION", MachLogOperation::COUNTERATTACK_OPERATION ),
	pImpl_( _NEW( MachLogCounterattackOperationImpl( pActor, pTarget ) ) )
{
	CB_MachLogCounterattackOperation_DEPIMPL();

	PRE( pActor->objectIsCanAttack() );

	pTarget_->attach( this );

	distanceBeyondWeaponRangeLastTimeIChecked_ = pActor->asCanAttack().distanceBeyondMaximumWeaponRange( *pTarget );

}
/* //////////////////////////////////////////////////////////////// */

MachLogCounterattackOperation::~MachLogCounterattackOperation()
{
	CB_MachLogCounterattackOperation_DEPIMPL();

	// if we still have a pointer to the cached op, we must be terminating through circumstances where that
	// that op has NOT been restored to the strategy. We must delete it ourselves to prevent a memory leak.
	if( pCachedOperation_ )
		_DELETE( pCachedOperation_ );

	if( pTarget_ )
		pTarget_->detach( this );

	_DELETE( pImpl_ );
}

// virtual
bool MachLogCounterattackOperation::beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData )
{
	CB_MachLogCounterattackOperation_DEPIMPL();

	ASSERT( pSubject == (W4dSubject*)pTarget_, "Notified by unexpected subject." );

	bool stayAttached = true;

	switch( event )
	{
		case W4dSubject::DELETED:
		{
			stayAttached = false;
		}
		break;

		case W4dSubject::CLIENT_SPECIFIC:
		{
			switch( clientData )
			{
				case MachLog::RACE_CHANGED:
				{
					MachLogRaces::DispositionToRace disposition = MachLogRaces::instance().dispositionToRace( pTarget_->race(), pActor_->race() );

					if( disposition == MachLogRaces::OUR_RACE or disposition == MachLogRaces::ALLY )
					{
						stayAttached = false;
					}
				}
				break;

				case MachLog::TELEPORTED_OUT_OF_WORLD:
				{
					stayAttached = false;
				}
				break;

				default:
				;
			}
		}
		break;

		default:
			;
	}

	if( not stayAttached )
	{
		pTarget_ = NULL;
		nextCallBackTime( SimManager::instance().currentTime() );
	}

	return stayAttached;
}


/* //////////////////////////////////////////////////////////////// */

void MachLogCounterattackOperation::storeOldFirstOperation( MachLogOperation* pOldOp )
{
	CB_MachLogCounterattackOperation_DEPIMPL();

	pCachedOperation_ = pOldOp;
	POST( pCachedOperation_ != NULL );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogCounterattackOperation::doOutputOperator( ostream& o ) const
{
	CB_MachLogCounterattackOperation_DEPIMPL();

	o << "MachLogCounterattackOperation\n";
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogCounterattackOperation::doStart()
{
   	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

PhysRelativeTime MachLogCounterattackOperation::doUpdate()
{
	CB_MachLogCounterattackOperation_DEPIMPL();

	PRE( not isFinished() );
	PRE( pActor_ != NULL );

	ASSERT( pCachedOperation_ != NULL, "MachLogCounterattackOperation::doUpdate : Hey, we shouldn't have a null pointer for the cached op by now!" );
	// get hold of our current strong threats

	// have we now met any of the conditions for terminating this operation?
	if ( shouldBreakOffAttack() )
	{
		// restore previous first operation as first (and only) operation on the strategy queue.
		pActor_->strategy().newOperation( pCachedOperation_, false );
		pCachedOperation_ = NULL;	// so we don't try and delete it in the destructor
		finished_ = true;
		return 0.0;
	}

	// gadzooks! The counterattack must continue.

	ASSERT( pTarget_ != NULL, "pTarget_ shouldn't be NULL if we've decided not to break off the attack." );

	// nothing to do from here if we've already	started attacking except wait for termination one way or the other
	if( initiatedAttackOp_ )
		return 2.0;

	// otherwise, it's time to START doing something.

	PhysRelativeTime interval = 0.0;

	MachLogOperation* pNewSubOp = _NEW( MachLogAttackOperation( pActor_, pTarget_, MachLogAttackOperation::TERMINATE_ON_CHANGE ) );

	subOperation( pActor_, pNewSubOp );

	initiatedAttackOp_ = true;


   	return interval;			//!! Have to alter interval if we have a subop. Probably.

}

bool MachLogCounterattackOperation::shouldBreakOffAttack()
{
	CB_MachLogCounterattackOperation_DEPIMPL();

	bool giveUp = false;

	if( pTarget_ == NULL )
		giveUp = true;
	else if( initiatedAttackOp_ )
	{
		if( not pSubOperation() )
			giveUp = true;
		else if( pTarget_->objectIsMachine() )
		{
			// give up chase if the target machine has been evading for at least 10 seconds	**REVOKED**
			// Or we haven't managed to close significantly on it in the last 15 seconds

			const MachLogMachine& targetMachine = pTarget_->asMachine();

			PhysAbsoluteTime timeNow = SimManager::instance().currentTime();


			if( not targetMachine.evading() )
				lastTimeTargetWasntEvading_ = timeNow;

			MATHEX_SCALAR speedDifferential = targetMachine.physMachine().machineData().speed() - pActor_->physMachine().machineData().speed();

			PhysRelativeTime giveUpPeriod = std::max( 5.0, 10.0 - speedDifferential );

			if( pActor_->virtualDefCon() == MachLog::DEFCON_LOW )
				giveUpPeriod += 5.0;

			giveUp = ( timeNow - lastTimeTargetWasntEvading_ >= giveUpPeriod );


			if( not giveUp and timeNow > nextTimeINeedToCheckImClosing_ )
			{
				// will still give up if I haven't managed to close on the target in the last 20 seconds.
				MATHEX_SCALAR distanceTargetIsBeyondWeaponRangeNow = pActor_->asCanAttack().distanceBeyondMaximumWeaponRange( *pTarget_ );

				if( distanceTargetIsBeyondWeaponRangeNow > 35.0
					and distanceTargetIsBeyondWeaponRangeNow > ( distanceBeyondWeaponRangeLastTimeIChecked_ * 0.7 ) )
				{
					giveUp = true;
				}
				else
				{
					distanceBeyondWeaponRangeLastTimeIChecked_ = distanceTargetIsBeyondWeaponRangeNow;
					nextTimeINeedToCheckImClosing_ = timeNow + distanceBeyondWeaponRangeCheckInterval();
				}
			}

			if( giveUp and not pActor_->isStandingGround() )
			{
				// prevents the aggressor from doing CAACT for a few seconds, otherwise it'd just
				// immediately reacquire the target in most cases.
				pActor_->asCanAttack().suppressAttackingUrges();
			}
		}
	}

	return giveUp;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogCounterattackOperation::doFinish()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogCounterattackOperation::doIsFinished() const
{
	CB_MachLogCounterattackOperation_DEPIMPL();

	// !! Code in here to check if strong threats are detected.
	return finished_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogCounterattackOperation::doBeInterrupted()
{
	CB_MachLogCounterattackOperation_DEPIMPL();

	//HAL_STREAM("(" << pActor_->id() << ") MLHealOp::doBeInterrupted\n" );
	pActor_->motionSeq().stop();
	initiatedAttackOp_ = false;
	return true;
}


/////////////////////////////////////////////////// persistence /////////////////////////////////////////////////////

void perWrite( PerOstream& ostr, const MachLogCounterattackOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pImpl_;
}

void perRead( PerIstream& istr, MachLogCounterattackOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pImpl_;

    //Reinstate observer relation
	if( op.pImpl_->pTarget_ )
		op.pImpl_->pTarget_->attach( &op );
}

MachLogCounterattackOperation::MachLogCounterattackOperation( PerConstructor con )
:	MachLogOperation( con )
{
}

// static
MATHEX_SCALAR MachLogCounterattackOperation::distanceBeyondWeaponRangeCheckInterval()
{
	static MATHEX_SCALAR interval = 15.0;
	return interval;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////


// End of code for MachLogCounterattackOperation


/* End OPEVADE.CPP *************************************************/
