/*
 * O P T R E A C H . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */
 
/*
 * Mixin for logical objects that support an
 * Treachery operation.
 */

#ifndef _MACHLOG_Treachery_HPP
#define _MACHLOG_Treachery_HPP

#include "phys/phys.hpp"

#include "machlog/animatio.hpp"
#include "machlog/operatio.hpp"

class MachActor;
/* //////////////////////////////////////////////////////////////// */

/* //////////////////////////////////////////////////////////////// */

// forward refs
class MachLogMachine;
class MachLogCanTreachery;
class MachLogAggressor;
class MachLogAdministrator;
class MachActor;

// canonical form revoked

class MachLogTreacheryOperation
: public MachLogOperation
{
public:

	MachLogTreacheryOperation( MachLogMachine * pActor,
							MachActor * pDirectObject );

	~MachLogTreacheryOperation();

	virtual	bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogTreacheryOperation );
	PER_FRIEND_READ_WRITE( MachLogTreacheryOperation );

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
    MachLogTreacheryOperation( const MachLogTreacheryOperation& );
    MachLogTreacheryOperation& operator =( const MachLogTreacheryOperation& );
    bool operator ==( const MachLogTreacheryOperation& );
	
    //Possible actions decided upon in an update call
    enum Action { WAIT_FOR_INTERVAL, MOVE_CLOSER, TURN_TO_FACE, MOVE_TO_SIDE, FIRE };
    
    //Attempt to move closer to the target. returns suggested update time obtained from
    //the motion sequencer.
    PhysRelativeTime moveCloserToTarget();

    //Attempt to turn the whole attacking machine (Not just its head) to face the target.
    PhysRelativeTime turnToFaceTarget();

    //Do an avoidance move with respect to the target
    PhysRelativeTime moveToSide();
	
	// removes threat from target, and clears current target if that was the target of this op
	void stopTargetting();

	bool					currentlyAttached_;
	MachLogMachine *		pActor_;
	MachLogAggressor*		pAgg_;
	MachLogAdministrator*	pAdm_;
	MachActor *				pDirectObject_;
    MexPoint2d          	lastTargetPosition_;
	bool					targetBehindCover_;
	Action					lastAction_;
};

PER_DECLARE_PERSISTENT( MachLogTreacheryOperation );

#endif	/*	#ifndef 	_MACHLOG_Treachery_HPP	*/

/* End Treachery.HPP ***************************************************/
