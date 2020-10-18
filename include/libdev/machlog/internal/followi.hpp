/*
 * F O L L O W I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogFollowOperationImpl

    Implementation class for MachLogFollowOperation.

*/

#ifndef _MACHLOG_FOLLOWI_HPP
#define _MACHLOG_FOLLOWI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machlog/follow.hpp"

// forward declarations

class MachLogFollowOperationImpl
{
public:
	MachLogFollowOperationImpl( MachLogMachine * pActor, 
							MachLogMachine* pTarget,
                            const MexPoint2d& offset, 
							MachLogFollowOperation::CamouflagedOp camoStatus,
                            MachLogFollowOperation::TerminateFlag terminateFlag );
	~MachLogFollowOperationImpl();
	PER_MEMBER_PERSISTENT( MachLogFollowOperationImpl );
	PER_FRIEND_READ_WRITE( MachLogFollowOperationImpl );
	
	void CLASS_INVARIANT;

	friend class MachLogFollowOperation;
	
private:

	MachLogMachine* 							pActor_; //the object doing the following
    MachLogMachine* 							pTarget_; //the object being followed
    MexPoint2d 									offset_; //The offset
	MachLogFollowOperation::CamouflagedOp 		camoStatus_;	// whether or not this operation is to be executed with camouflage enabled
	MachLogFollowOperation::TerminateFlag       terminateFlag_; // whether or not we should terminate the op when sufficiently close to the target
	
};

PER_DECLARE_PERSISTENT( MachLogFollowOperationImpl );

#endif

/* End FOLLOWI.HPP ***************************************************/
