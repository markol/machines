/*
 * O P E V A D E I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogEvadeOperationImpl

    Implementation class for MachLogEvadeOperation.

*/

#ifndef _MACHLOG_OPEVADEI_HPP
#define _MACHLOG_OPEVADEI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machlog/opevade.hpp"

// forward declarations

class MachLogEvadeOperationImpl
{
public:
	MachLogEvadeOperationImpl( MachLogMachine * pActor );
	~MachLogEvadeOperationImpl();
	PER_MEMBER_PERSISTENT( MachLogEvadeOperationImpl );
	PER_FRIEND_READ_WRITE( MachLogEvadeOperationImpl );
	
	void CLASS_INVARIANT;

	friend class MachLogEvadeOperation;
	
private:

	MachLogMachine*									pActor_;
	bool											finished_;
													
													// stores the topmost op the machine was doing before
													// all this evade nonsense was started
	MachLogOperation*								pCachedOperation_;
			
	MachLogEvadeOperation::EvadeOpType				currentEvadeOpType_;
	
	PhysAbsoluteTime								lastTimeIHadStrongThreat_;	
													
};

PER_DECLARE_PERSISTENT( MachLogEvadeOperationImpl );

#endif

/* End OPEVADEI.HPP ***************************************************/
