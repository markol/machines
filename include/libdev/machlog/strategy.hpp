/*
 * S T R A T E G Y . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#ifndef _MACH_STRATEGY_HPP
#define _MACH_STRATEGY_HPP

#include "base/base.hpp"
#include "machlog/operatio.hpp"

/* //////////////////////////////////////////////////////////////// */

// forward refs
//class ostream;
class MachOperation;
class MachActor;
class MachLogDebris;
class MachLogMachine;
class MachLogStrategyImpl;
template < class T > class ctl_pvector;

/* //////////////////////////////////////////////////////////////// */

class MachLogStrategy
{
public:

	MachLogStrategy( MachActor * pActor );

	virtual ~MachLogStrategy();

	//destroys any remaining queue_ if it can;
	void beInterrupted();

	bool queueIsEmpty() const;

	// queue is empty AND no pending operations
	bool isFinished() const;

	PhysRelativeTime update( const PhysRelativeTime& maxCPUTime );
	// PRE( not isFinished() );

	void newOperation( MachLogOperation * pNewOperation, bool subOperation = false );

	// this returns false if no follow op was on the queue, or only a follow op which already had
	// a suboperation.
	// IT IS THE CLIENT'S RESPONSIBILITY TO DELETE AN OPERATION WHICH FAILED TO BE ADDED TO THE QUEUE.
	bool addOperationAsSubOperationToFollowOperation( MachLogOperation * pNewOperation );

	// these return the current "boss" operation at the front of the queue
	const string& currentOperationTypeAsString() const;
	MachLogOperation::OperationType currentOperationType() const;

	// this returns to potential "slave" operation executing right now
	const MachLogOperation& operationCurrentlyExecuting() const;

	void tryToRemoveAllOperations();
	//any operations with update status - set back to start status
	void setAllOperationsToRestart();

	// PRE ( not isUninterruptable() );
	void removeAllOperations();

	bool isUninterruptable();		//returns true if ANY of the ops on	the queue are uninterruptable.

	// PRE ( not isUninterruptable() );
	void changeToEvadeMode();		//creates an evade op which will resume with topmost op after it has
									//finished. All other ops are deleted.

	// PRE ( not isUninterruptable() );
	void changeToCounterattackMode( MachActor* pTarget );
									//creates a counterattack op which will resume with topmost op after it has
									//finished. All other ops are deleted.

									//creates an autoscavenge op which will resume with topmost op after it has
									//finished. All other ops are deleted.
	void changeToScavengeMode( MachLogDebris* pDebris );
	//PRE( pActor_->objectType() == MachLog::RESOURCE_CARRIER and pActor_->asResourceCarrier().isScavenger() );
	//PRE( not isUninterruptable() );

	//Give client code the ability to get hold of the front of the operation stack directly.
	MachLogOperation& currentOperation();
	const MachLogOperation& currentOperation() const;

	// useful for attacking machines to know that their target is about to leave a building.
	bool isDoingLeaveOperation() const;

	// returns true if owner is currently entering building specified
	bool isEnteringBuilding( const MachLogConstruction& constron ) const;

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogStrategy );
	PER_FRIEND_READ_WRITE( MachLogStrategy );

	typedef ctl_pvector< MachLogOperation > Queue;

private:
	MachActor& actor();
	const MachActor& actor() const;

	//typedef ctl_pvector< MachLogOperation > Queue;

	const Queue& queue() const;
	const MachLogOperation* pPendingOperation() const;

	// attempts to extract either the pending op (if any) or, failing that, the first op on the queue (if any)
	// from the stored operations, updates the pointer to point to this operation if found, and returns the found
	// status. If you were to call removeAllOperations() after extracting an operation, it would NOT be deleted.
	// Responsibility for deletion becomes the client's.
	bool extractTopOperation( MachLogOperation** ppOldTopOp );

	friend ostream& operator <<( ostream& o, const MachLogStrategy& s );

	MachLogStrategyImpl*	pImpl_;
};

PER_DECLARE_PERSISTENT( MachLogStrategy );

/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef _MACH_STRATEGY_HPP	*/

/* End STRATEGY.HPP *************************************************/
