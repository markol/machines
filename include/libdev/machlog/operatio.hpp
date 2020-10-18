/*
 * O P E R A T I O . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#ifndef _MACH_OPERATIO_HPP
#define _MACH_OPERATIO_HPP

#include "base/tracker.hpp"
#include "phys/phys.hpp"

#include "world4d/observer.hpp"

#include "machlog/machlog.hpp"

#include "mathex/point2d.hpp"
#include "mathex/point3d.hpp"

#include "stdlib/strfwd.hpp"


/* //////////////////////////////////////////////////////////////// */

// forward refs
//class ostream;
class MachLogConstruction;
class MachActor;
class MachLogResourceCarrier;
class MachLogOperationImpl;
class MachLogGroupMoveInfo;

template<class T> class ctl_list;

// orthodox canonical (revoked)

class MachLogOperation : public W4dObserver
// cannonical form revoked
{
public:
	enum Priority {
		LOW_PRIORITY,
		NORMAL_PRIORITY,
		HIGH_PRIORITY,
		USER_PRIORITY	//The user has said to do something directly.
	};

	enum OperationType {
				MOVE_TO_OPERATION, ATTACK_OPERATION, FOLLOW_OPERATION, SEEK_AND_DESTROY_OPERATION,
				ANIMATION, ADMIN_ATTACK_OPERATION, ADMIN_CONSTRUCT_OPERATION, ADMIN_HEAL_OPERATION,
				ADMIN_LOCATE_OPERATION, CAPTURE_OPERATION, CONSTRUCT_OPERATION, DEPLOY_APC_OPERATION,
				ENTER_APC_OPERATION, GOTO_LAB_OPERATION, HEAL_OPERATION, DROPLANDMINE_OPERATION,
				LOCATE_OPERATION, MISSILE_EMPLACEMENT_ATTACK_OPERATION, PICKUP_OPERATION,
				POD_ATTACK_OPERATION, PUTDOWN_OPERATION, REFILLLANDMINES_OPERATION, REPAIR_OPERATION,
				TRANSPORT_OPERATION, TREACHERY_OPERATION, TASK_ATTACK_OPERATION,
				TASK_CONSTRUCT_OPERATION, TASK_DROPLANDMINE_OPERATION, TASK_LOCATE_OPERATION,
				PATROL_OPERATION, ENTER_OPERATION, LEAVE_OPERATION, MOVE_AND_TURN_OPERATION,
				DECONSTRUCT_OPERATION, RECYCLE_OPERATION, ADMIN_DECONSTRUCT_OPERATION, ADMIN_CAPTURE_OPERATION,
				SUPERCONSTRUCT_OPERATION, ADMIN_SUPERCONSTRUCT_OPERATION, STAND_GROUND_OPERATION,
				ADMIN_REPAIR_OPERATION, EVADE_OPERATION, CAMO_MOVE_OPERATION, NUKE_ATTACK_OPERATION,
				TASK_PATROL_OPERATION, SCAVENGE_OPERATION, COUNTERATTACK_OPERATION, AUTOSCAVENGE_OPERATION,
				N_OPERATIONS };

	enum Progress { START, UPDATE, FINISHED };

	MachLogOperation( const char*, OperationType );
	MachLogOperation( const char*, OperationType, PhysPathFindingPriority );
	virtual ~MachLogOperation();

	///////////////////////////////

	bool isFinished() const;
	virtual PhysAbsoluteTime update( );
	virtual PhysRelativeTime doUpdate( ) = 0;
	void subOperation( MachActor*, MachLogOperation* );
	bool hasSubOperation();

	virtual bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData = 0 );

    virtual void domainDeleted( W4dDomain* pDomain );

	const string& operationTypeAsString() const;
	OperationType operationType() const;

	//get/set for the priority. The priority may be reclassified.
	Priority priority() const;
	void priority( const Priority& );
	///////////////////////////////

	// redefinition required, invocation revoked
	virtual bool doBeInterrupted() = 0;

    void groupMoveInfo( const MachLogGroupMoveInfo& );

    PhysPathFindingPriority pathFindingPriority() const;

    void CLASS_INVARIANT;

	PER_MEMBER_PERSISTENT_ABSTRACT( MachLogOperation );
	PER_FRIEND_READ_WRITE( MachLogOperation );

protected:

	friend ostream& operator <<( ostream&, const Progress& );
	PER_FRIEND_ENUM_PERSISTENT( Progress );
	Progress progress() const;

	virtual bool doStart() = 0;
	// PRE( not isFinished() );
	virtual void doFinish() = 0;
	// PRE( isFinished() );

	//returns the same as the above function but doesn't do the clean up code.
	virtual bool isInterruptable();

	virtual bool doIsFinished() const = 0;

	virtual void doOutputOperator( ostream& ) const = 0;

	//if the machine is currently in a building
	//then this will determine if it needs to issue a leave building first
	//this is virtual to allow operations such as move to override the precise nature
	//of the checking.
	//If the machine needs to leave then it will be issued with a leave operation at the top of the stack.
	virtual bool checkNeedAndDoLeaveOperation( MachActor* );
	//check for leave without doing it.
	virtual bool checkNeedLeaveOperation( MachActor*, MachLogConstruction** );

	///////////////////////////////
	friend class MachLogStrategy;

	PhysAbsoluteTime	nextCallBackTime() const;
	MachLogOperation*	pSubOperation();
	const MachLogOperation*	pSubOperation() const;

	void progress( Progress );
	void pSubOperation( MachLogOperation* );
	void nextCallBackTime( const PhysAbsoluteTime& );

    const MachLogGroupMoveInfo& groupMoveInfo() const;

private:

	MachLogOperation( const MachLogOperation& );
	MachLogOperation& operator =( const MachLogOperation& );
	bool operator ==( const MachLogOperation& ) const;

	friend ostream& operator <<( ostream&, const MachLogOperation& );



	MachLogOperationImpl*	pImpl_;

	OBJECT_TRACKER( MachLogOperation );
};

PER_DECLARE_PERSISTENT( MachLogOperation );

PER_ENUM_PERSISTENT( MachLogOperation::Priority );
PER_ENUM_PERSISTENT( MachLogOperation::OperationType );

#pragma off(unreferenced);
//EXISTS( MachLogOperation );
#pragma on(unreferenced);

#endif	/*	#ifndef _MACH_OPERATIO_HPP	*/

/* End OPERATIO.HPP *************************************************/
