/*
 * O P C O U N T E . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */
 
/*
*	Counterattack operation
 * 	Operation that attacks a target, but is not committed to follow it to the death.
 * 	Will break off the attack if the target retreats sufficiently. Also manages
 *	cached operations to resume, which can include returning to the original location.
 */

#ifndef _MACHLOG_OPCOUNTE_HPP
#define _MACHLOG_OPCOUNTE_HPP

#include "phys/phys.hpp"

#include "machlog/operatio.hpp"

class MachLogCounterattackOperationImpl;

/* //////////////////////////////////////////////////////////////// */

/* //////////////////////////////////////////////////////////////// */

// forward refs
class MachLogStrategy;
class MachLogMachine;
class MachActor;


// orthodox canonical (revoked)

class MachLogCounterattackOperation
: public MachLogOperation
{
public:
	
	virtual ~MachLogCounterattackOperation();
	
	// how regularly do we need to check that we're closing on the target?
	static MATHEX_SCALAR distanceBeyondWeaponRangeCheckInterval();

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogCounterattackOperation );
	PER_FRIEND_READ_WRITE( MachLogCounterattackOperation );
	
protected:

	virtual bool doStart();
	virtual void doFinish();
	
	virtual bool doIsFinished() const;
	virtual PhysRelativeTime doUpdate();

	virtual void doOutputOperator( ostream& ) const;
	
	virtual bool doBeInterrupted();
	
	void storeOldFirstOperation( MachLogOperation* );
	
	virtual bool beNotified( W4dSubject* pSubject, 
							 W4dSubject::NotificationEvent event, int clientData );


private:

	// Operations deliberately revoked
    MachLogCounterattackOperation( const MachLogCounterattackOperation& );
    MachLogCounterattackOperation& operator =( const MachLogCounterattackOperation& );
    bool operator ==( const MachLogCounterattackOperation& );
	
	// private constructor - can only be built from within MachLogStrategy.
	friend class MachLogStrategy;

	MachLogCounterattackOperation( MachLogMachine* pActor, MachActor* pTarget );
	// PRE( pActor->objectIsCanAttack() );
	
	bool shouldBreakOffAttack();

	// data members
	MachLogCounterattackOperationImpl*		pImpl_;	  
};


PER_DECLARE_PERSISTENT( MachLogCounterattackOperation );

/* //////////////////////////////////////////////////////////////// */
	
#endif	/*	#ifndef 	_MACHLOG_OPCOUNTE_HPP	*/

/* End OPCOUNTE.HPP ***************************************************/
