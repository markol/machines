/*
 * A T T A C K . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */
 
/*
 * Heal Operation
 */

#ifndef _MACHLOG_Heal_HPP
#define _MACHLOG_Heal_HPP

#include "phys/phys.hpp"

#include "machlog/animatio.hpp"
#include "machlog/operatio.hpp"

class MachActor;
/* //////////////////////////////////////////////////////////////// */

/* //////////////////////////////////////////////////////////////// */

// forward refs
class MachLogMachine;
class MachLogCanHeal;
class MachLogAggressor;
class MachLogAdministrator;
class MachActor;

// orthodox canonical (revoked)

class MachLogHealOperation
: public MachLogOperation
{
public:

	MachLogHealOperation( MachLogAdministrator * pActor,
							MachActor * pDirectObject );

	~MachLogHealOperation();

	virtual	bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogHealOperation );
	PER_FRIEND_READ_WRITE( MachLogHealOperation );

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
    MachLogHealOperation( const MachLogHealOperation& );
    MachLogHealOperation& operator =( const MachLogHealOperation& );
    bool operator ==( const MachLogHealOperation& );
	
    //Possible actions decided upon in an update call
    enum Action { WAIT_FOR_INTERVAL, MOVE_CLOSER, TURN_TO_FACE, MOVE_TO_SIDE, FIRE };
    
    //Attempt to move closer to the target. returns suggested update time obtained from
    //the motion sequencer.
    PhysRelativeTime moveCloserToTarget();

    //Attempt to turn the whole attacking machine (Not just its head) to face the target.
    PhysRelativeTime turnToFaceTarget();

    //Do an avoidance move with respect to the target
    PhysRelativeTime moveToSide();
	
	// clears current target if currently identical to the target of this heal op
	void clearHealTarget();

	bool					currentlyAttached_;
	MachLogAdministrator*	pActor_;
	MachActor *				pDirectObject_;
    MexPoint2d          	lastTargetPosition_;
	bool					targetBehindCover_;
	Action					lastAction_;
};

PER_DECLARE_PERSISTENT( MachLogHealOperation );


#endif	/*	#ifndef 	_MACHLOG_Heal_HPP	*/

/* End Heal.HPP ***************************************************/
