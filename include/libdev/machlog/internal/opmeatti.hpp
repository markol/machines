/*
 * O P M E A T T I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogMissileEmplacementAttackOperationImpl

    Implementation class for MachLogMissileEmplacementAttackOperation.

*/

#ifndef _MACHLOG_OPMEATTI_HPP
#define _MACHLOG_OPMEATTI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "machlog/opmeatta.hpp"

// forward declarations

class MachLogMissileEmplacementAttackOperationImpl
{
public:
	MachLogMissileEmplacementAttackOperationImpl( MachLogMissileEmplacement * pActor,
												  MachActor * pDirectObject, 
												  MachLogAttackOperation::RaceChangeTerminate );
	~MachLogMissileEmplacementAttackOperationImpl();
	PER_MEMBER_PERSISTENT( MachLogMissileEmplacementAttackOperationImpl );
	PER_FRIEND_READ_WRITE( MachLogMissileEmplacementAttackOperationImpl );
	
	void CLASS_INVARIANT;

	friend class MachLogMissileEmplacementAttackOperation;
	
private:

	bool											currentlyAttached_;
	MachLogMissileEmplacement*						pActor_;
	MachActor *										pDirectObject_;
    MexPoint2d          							lastTargetPosition_;
	bool											targetBehindCover_;
	MachLogAttackOperation::RaceChangeTerminate		raceChangeTerminate_;	
													
};

PER_DECLARE_PERSISTENT( MachLogMissileEmplacementAttackOperationImpl );

#endif

/* End OPMEATTI.HPP ***************************************************/
