/*
 * O P E R A T I O . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */


#include "machlog/operatio.hpp"

//#include "ctl/algorith.hpp"
#include "ctl/list.hpp"

#include "machlog/internal/operatii.hpp"

#include "machlog/machine.hpp"
#include "machlog/strategy.hpp"
#include "machlog/constron.hpp"
#include "machlog/inout.hpp"
#include "machlog/grpmvinf.hpp"

#include "profiler/profiler.hpp"

/* //////////////////////////////////////////////////////////////// */
PER_DEFINE_PERSISTENT_ABSTRACT( MachLogOperation );

MachLogOperation::MachLogOperation( const char* typeOp, OperationType typeOpEnum )
: pImpl_( _NEW( MachLogOperationImpl( typeOp, typeOpEnum, Phys::defaultPathFindingPriority() ) ) )
{
    LOG_CONSTRUCTION;

	/* Intentionally Empty	*/
}

MachLogOperation::MachLogOperation(
  const char* typeOp,
  OperationType typeOpEnum,
  PhysPathFindingPriority pathFindingPriority )

: pImpl_( _NEW( MachLogOperationImpl( typeOp, typeOpEnum, pathFindingPriority  ) ) )
{
    LOG_CONSTRUCTION;

	/* Intentionally Empty	*/
}

MachLogOperation::~MachLogOperation()
{
    LOG_DESTRUCTION;
	//HAL_STREAM("MLOperation::DTOR\n" );
	_DELETE( pImpl_ );
}

bool MachLogOperation::isFinished() const
{
	CB_DEPIMPL( Progress, progress_ );
    TEST_INVARIANT;

	return progress_ == FINISHED;
}

MachLogOperation::Progress MachLogOperation::progress() const
{
	CB_DEPIMPL( Progress, progress_ );

	return progress_;
}

//virtual
bool MachLogOperation::checkNeedLeaveOperation( MachActor* pActor, MachLogConstruction** pConstruction )
{
    TEST_INVARIANT;

	bool found = false;
	if(  pActor->asMachine().insideBuilding() )
	{
		*pConstruction = &pActor->asMachine().insideWhichBuilding();
		found = true;
	}

	return found;
}

//virtual
bool MachLogOperation::checkNeedAndDoLeaveOperation( MachActor* pActor )
{
    TEST_INVARIANT;


	MachLogConstruction* pConstruction;
	bool found = checkNeedLeaveOperation( pActor, &pConstruction );
	if( found )
	{
		MachLogMachine& mlm = pActor->asMachine();

        //If the machine has a station locked, need to pass it in
        MachPhysStation* pStation = (mlm.hasStationLocked() ? &mlm.stationLocked() : NULL );
		subOperation( &mlm, _NEW( MachLogLeaveBuildingOperation( &mlm, pConstruction, pStation ) ) );
	}

	return found;
}

PhysAbsoluteTime MachLogOperation::update()
{
	CB_DEPIMPL( PhysAbsoluteTime, nextCallBackTime_ );
    TEST_INVARIANT;

	return nextCallBackTime_;
}

ostream& operator <<( ostream& o, const MachLogOperation& op )
{
	op.doOutputOperator( o );
	if( op.pSubOperation() != NULL )
	{
		op.pSubOperation()->doOutputOperator( o );
		if( op.pSubOperation()->pSubOperation() != NULL )
		{
			op.pSubOperation()->pSubOperation()->doOutputOperator( o );
			if( op.pSubOperation()->pSubOperation()->pSubOperation() != NULL )
				op.pSubOperation()->pSubOperation()->pSubOperation()->doOutputOperator( o );
		}
	}
	return o;
}

void MachLogOperation::subOperation( MachActor* pActor, MachLogOperation* pOp )
{
	PRE( pOp != NULL );

	CB_DEPIMPL( MachLogOperation*, pSubOperation_ );
    TEST_INVARIANT;

	pSubOperation_ = pOp;
	pActor->strategy().newOperation( pOp, true );
}

bool MachLogOperation::hasSubOperation()
{
	CB_DEPIMPL( MachLogOperation*, pSubOperation_ );

	return ( pSubOperation_ != NULL );
}

EXISTS( MachLogOperation );

void MachLogOperation::CLASS_INVARIANT
{
    INVARIANT_THIS_EXISTS;
}

//virtual
bool MachLogOperation::beNotified( W4dSubject*,
	                         W4dSubject::NotificationEvent, int )
{
    TEST_INVARIANT;

	return false;
}

//virtual
void MachLogOperation::domainDeleted( W4dDomain*  )
{
    TEST_INVARIANT;

	//inentionally empty...override as necessary

}

const string&
MachLogOperation::operationTypeAsString() const
{
	CB_DEPIMPL( string, operationTypeAsString_ );
    TEST_INVARIANT;

	return operationTypeAsString_;
}

MachLogOperation::OperationType MachLogOperation::operationType() const
{
	CB_DEPIMPL( OperationType, operationType_ );

	return operationType_;
}


PhysAbsoluteTime MachLogOperation::nextCallBackTime() const
{
	CB_DEPIMPL( PhysAbsoluteTime, nextCallBackTime_ );

	return nextCallBackTime_;
}

MachLogOperation* MachLogOperation::pSubOperation()
{
	CB_DEPIMPL( MachLogOperation*, pSubOperation_ );

	return pSubOperation_;
}

const MachLogOperation* MachLogOperation::pSubOperation() const
{
	CB_DEPIMPL( MachLogOperation*, pSubOperation_ );

	return pSubOperation_;
}

/* //////////////////////////////////////////////////////////////// */

//virtual
bool MachLogOperation::isInterruptable()
{
	return true;
}

void MachLogOperation::progress( Progress newProgress )
{
	CB_DEPIMPL( Progress, progress_ );

	progress_ = newProgress;
}

void MachLogOperation::pSubOperation( MachLogOperation* pSubOperation )
{
	CB_DEPIMPL( MachLogOperation*, pSubOperation_ );

	pSubOperation_ = pSubOperation;
}

void MachLogOperation::nextCallBackTime( const PhysAbsoluteTime& newNextCallBackTime )
{
	CB_DEPIMPL( PhysAbsoluteTime, nextCallBackTime_ );

	nextCallBackTime_ = newNextCallBackTime;
}

MachLogOperation::MachLogOperation( PerConstructor )
{
}


void perWrite( PerOstream& ostr, const MachLogOperation& op )
{
	const W4dObserver& base1 = op;

	ostr << base1;
	ostr << op.pImpl_;
}

void perRead( PerIstream& istr, MachLogOperation& op )
{
	W4dObserver& base1 = op;

	istr >> base1;
	istr >> op.pImpl_;
}

ostream& operator <<( ostream& o, const MachLogOperation::Progress& progress )
{
    switch( progress )
    {
        case MachLogOperation::START:
            o << "START";
            break;
        case MachLogOperation::UPDATE:
            o << "UPDATE";
            break;
        case MachLogOperation::FINISHED:
            o << "FINISHED";
            break;
    }

    return o;
}


void MachLogOperation::groupMoveInfo( const MachLogGroupMoveInfo& info )
{
    pImpl_->groupMoveInfo_ = info;
}

const MachLogGroupMoveInfo& MachLogOperation::groupMoveInfo() const
{
    return pImpl_->groupMoveInfo_;
}

PhysPathFindingPriority MachLogOperation::pathFindingPriority() const
{
    return pImpl_->pathFindingPriority_;
}

/* End OPERATIO.CPP *************************************************/
