/*
 * O P M E A T T A . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */
 
/*
 * Attack operation specific to missile emplacement actors
 */

#ifndef _MACHLOG_MEATTACK_HPP
#define _MACHLOG_MEATTACK_HPP

#include "phys/phys.hpp"

#include "machlog/attack.hpp"

#include "machlog/operatio.hpp"

/* //////////////////////////////////////////////////////////////// */

// forward refs
class MachLogMissileEmplacement;
class MachLogCanAttack;
class MachActor;
class MachLogMissileEmplacementAttackOperationImpl;

// canonical form revoked

class MachLogMissileEmplacementAttackOperation
: public MachLogOperation
{
public:

	MachLogMissileEmplacementAttackOperation( MachLogMissileEmplacement * pActor,
							MachActor * pDirectObject, MachLogAttackOperation::RaceChangeTerminate=MachLogAttackOperation::DO_NOT_TERMINATE_ON_CHANGE );

	~MachLogMissileEmplacementAttackOperation();

	virtual	bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogMissileEmplacementAttackOperation );
	PER_FRIEND_READ_WRITE( MachLogMissileEmplacementAttackOperation );

protected:

	virtual bool doStart();
	virtual void doFinish();
	
	virtual bool doIsFinished() const;
	virtual PhysRelativeTime doUpdate();

	virtual void doOutputOperator( ostream& ) const;

	MachActor& directObject();
	const MachActor& directObject() const;
	
	virtual bool doBeInterrupted();

	void CLASS_INVARIANT;

private:

	// Operations deliberately revoked
    MachLogMissileEmplacementAttackOperation( const MachLogMissileEmplacementAttackOperation& );
    MachLogMissileEmplacementAttackOperation& operator =( const MachLogMissileEmplacementAttackOperation& );
    bool operator ==( const MachLogMissileEmplacementAttackOperation& );
	
	bool currentlyAttached() const;
	
	MachActor* pDirectObject();
	
	// removes threat from target, and clears current target if that was the target of this op
	void stopTargetting();
	
	// data members
	MachLogMissileEmplacementAttackOperationImpl* pImpl_;	
};

PER_DECLARE_PERSISTENT( MachLogMissileEmplacementAttackOperation );

/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef 	_MACHLOG_MEATTACK_HPP	*/

/* End ATTACK.HPP ***************************************************/
