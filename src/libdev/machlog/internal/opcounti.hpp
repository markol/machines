/*
 * O P C O U N T I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogCounterattackOperationImpl

    Implementation class for MachLogCounterattackOperation.

*/

#ifndef _MACHLOG_OPCOUNTI_HPP
#define _MACHLOG_OPCOUNTI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"

// forward declarations

class MachLogMachine;
class MachActor;
class MachLogOperation;
class MachLogCounterattackOperation;

class MachLogCounterattackOperationImpl
{
public:
	MachLogCounterattackOperationImpl( MachLogMachine* pActor, MachActor* pTarget );
	~MachLogCounterattackOperationImpl();
	PER_MEMBER_PERSISTENT( MachLogCounterattackOperationImpl );
	PER_FRIEND_READ_WRITE( MachLogCounterattackOperationImpl );
	
	void CLASS_INVARIANT;

	friend class MachLogCounterattackOperation;
    friend void perRead( PerIstream& istr, MachLogCounterattackOperation& op );
	
private:

	MachLogMachine*									pActor_;
	
	MachActor*										pTarget_;
	
	bool											finished_;
	bool											initiatedAttackOp_;	
													
													// stores the topmost op the machine was doing before
													// all this counterattack nonsense was started
	MachLogOperation*								pCachedOperation_;
	
	PhysAbsoluteTime								lastTimeTargetWasntEvading_;
	
	PhysAbsoluteTime								nextTimeINeedToCheckImClosing_;
	
	MATHEX_SCALAR									distanceBeyondWeaponRangeLastTimeIChecked_;
													
};

PER_DECLARE_PERSISTENT( MachLogCounterattackOperationImpl );

#endif

/* End OPCOUNTI.HPP ***************************************************/
