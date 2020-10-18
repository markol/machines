/*
 * O P E R A T I I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogAttackOperationImpl

    Implementation class for MachLogAttackOperation.

*/

#ifndef _MACHLOG_ATTACKI_HPP
#define _MACHLOG_ATTACKI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machlog/attack.hpp"

// forward declarations

class MachLogRace;

class MachLogAttackOperationImpl
{
public:
	MachLogAttackOperationImpl( MachLogMachine * pActor, MachActor * pDirectObject, MachLogAttackOperation::RaceChangeTerminate rct );
	MachLogAttackOperationImpl( MachLogMachine * pActor, MachActor * pDirectObject, size_t commandId, MachLogAttackOperation::RaceChangeTerminate rct );
	~MachLogAttackOperationImpl();
	PER_MEMBER_PERSISTENT( MachLogAttackOperationImpl );
	PER_FRIEND_READ_WRITE( MachLogAttackOperationImpl );
	
	void CLASS_INVARIANT;

	friend class MachLogAttackOperation;
	
private:
	bool											currentlyAttached_;
	MachLogMachine *								pActor_;
	MachLogAggressor*								pAgg_;
	MachLogAdministrator*							pAdm_;
	MachActor *										pDirectObject_;
    MexPoint2d          							lastTargetPosition_;
	bool											targetBehindCover_;
	MachLogAttackOperation::Action					lastAction_; //Action decided on in previous update
	MachLogAttackOperation::RaceChangeTerminate	 	raceChangeTerminate_;
    size_t              							commandId_;
	bool											hasGorillaPunch_;
	PhysAbsoluteTime								nextHardCoverCheckTime_;

	
};

PER_DECLARE_PERSISTENT( MachLogAttackOperationImpl );

#endif

/* End ACTORI.HPP ***************************************************/
