/*
 * A T T A C K . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */
 
/*
 * Attack operation - for attacking one thing with another thing.
 */

#ifndef _MACHLOG_ATTACK_HPP
#define _MACHLOG_ATTACK_HPP

#include "phys/phys.hpp"

#include "machlog/canattac.hpp"
#include "machlog/operatio.hpp"

class MachActor;
class MachLogAttackOperationImpl;
/* //////////////////////////////////////////////////////////////// */

/* //////////////////////////////////////////////////////////////// */

// forward refs
class MachLogMachine;
class MachLogAggressor;
class MachLogAdministrator;
class MachActor;

class MachLogAttackOperation
: public MachLogOperation
{
public:
	enum RaceChangeTerminate { TERMINATE_ON_CHANGE, DO_NOT_TERMINATE_ON_CHANGE };

	MachLogAttackOperation( MachLogMachine * pActor,
							MachActor * pDirectObject,
							RaceChangeTerminate rct = DO_NOT_TERMINATE_ON_CHANGE );

	MachLogAttackOperation( MachLogMachine * pActor,
							MachActor * pDirectObject,
							size_t commandId,
							RaceChangeTerminate rct = DO_NOT_TERMINATE_ON_CHANGE );

	~MachLogAttackOperation();

	virtual	bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData );

	//Possible actions decided upon in an update call
    enum Action { WAIT_FOR_INTERVAL, MOVE_CLOSER, TURN_TO_FACE, MOVE_TO_SIDE, FIRE };
	
	// Period after a move initiated as a result of being behind cover in which no turn to face target is allowed.
	// This prevents constant threshing if the move pulls the machine's body out of horizontal angular range of
	// the target, which it may of course do.
	static PhysRelativeTime behindCoverMoveGracePeriod();
							 
	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogAttackOperation );
	PER_FRIEND_READ_WRITE( MachLogAttackOperation );
	
	friend class MachLogAttackOperationImpl;

protected:

	virtual bool doStart();
	virtual void doFinish();
	
	virtual bool doIsFinished() const;
	virtual PhysRelativeTime doUpdate();

	virtual void doOutputOperator( ostream& ) const;

	MachActor& directObject();
	const MachActor& directObject() const;
	
	virtual bool doBeInterrupted();
	
private:

	// Operations deliberately revoked
    MachLogAttackOperation( const MachLogAttackOperation& );
    MachLogAttackOperation& operator =( const MachLogAttackOperation& );
    bool operator ==( const MachLogAttackOperation& );

	
	void dt( const char* text, bool start );    
    
    //Attempt to move closer to the target. returns suggested update time obtained from
    //the motion sequencer.
    PhysRelativeTime moveCloserToTarget();

    //Attempt to turn the whole attacking machine (Not just its head) to face the target.
    PhysRelativeTime turnToFaceTarget();

    //Do an avoidance move with respect to the target
    PhysRelativeTime moveToSide();
	
	// removes threat from target, and clears current target if that was the target of this op
	void stopTargetting();
	
	bool currentlyAttached() const;
	void currentlyAttached( bool status );
	
	Action actionForDisposition( MachLogCanAttack::WeaponDisposition worstDisposition, const MachLogCanAttack& attacker ) const;
	PhysRelativeTime calculateNewApproachPoint( const MexPoint2d& targetPositionNow, const MexPoint2d& attackerPositionNow, bool chasingEvader, MATHEX_SCALAR distanceToTargetNow );
	
	MachActor* pDirectObject();

	MachLogAttackOperationImpl* 	pImpl_;
	
};

ostream& operator <<( ostream& o, MachLogAttackOperation::Action );

PER_ENUM_PERSISTENT( MachLogAttackOperation::Action );
PER_ENUM_PERSISTENT( MachLogAttackOperation::RaceChangeTerminate );
PER_DECLARE_PERSISTENT( MachLogAttackOperation );

/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef 	_MACHLOG_ATTACK_HPP	*/

/* End ATTACK.HPP ***************************************************/
