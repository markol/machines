/*
 * M O V E I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogMoveToOperationImpl

    Implementation class for MachLogMoveToOperation.

*/

#ifndef _MACHLOG_MOVEI_HPP
#define _MACHLOG_MOVEI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machlog/move.hpp"

#include "mathex/point3d.hpp"
#include "mathex/point2d.hpp"

#include "ctl/list.hpp"

// forward declarations

////////////////////////////////////////////////// MOVE TO impl /////////////////////////////////////////////////////

class MachLogMoveToOperationImpl
{
public:		
	MachLogMoveToOperationImpl( MachLogMachine * pActor, const MexPoint3d& dest, bool checkForLeave );
	MachLogMoveToOperationImpl( MachLogMachine * pActor, const MachLogMoveToOperation::Path& path, bool checkForLeave );
	MachLogMoveToOperationImpl( MachLogMachine * pActor, const MexPoint3d& dest, size_t commandId, bool checkForLeave );
	MachLogMoveToOperationImpl( MachLogMachine * pActor, const MachLogMoveToOperation::Path& path, size_t commandId, bool checkForLeave );
							
	~MachLogMoveToOperationImpl();
	PER_MEMBER_PERSISTENT( MachLogMoveToOperationImpl );
	PER_FRIEND_READ_WRITE( MachLogMoveToOperationImpl );
	
	void CLASS_INVARIANT;

	friend class MachLogMoveToOperation;
	
private:

	MachLogMachine*						pActor_;
	MexPoint3d							dest_;
	bool								checkForLeave_;
	MachLogMoveToOperation::Path		path_;
    size_t              				commandId_;	
	MATHEX_SCALAR 							tolerance_;
};

PER_DECLARE_PERSISTENT( MachLogMoveToOperationImpl );

///////////////////////////////////////////// MOVE AND TURN impl ////////////////////////////////////////////////////

class MachLogMoveAndTurnOperationImpl
{
public:		
	MachLogMoveAndTurnOperationImpl( MachLogMachine * pActor, 
									const MexPoint3d& dest,
									const MexPoint3d& endFacing,
									bool checkForLeave,
									PhysRelativeTime initialDelay );																
	~MachLogMoveAndTurnOperationImpl();
	PER_MEMBER_PERSISTENT( MachLogMoveAndTurnOperationImpl );
	PER_FRIEND_READ_WRITE( MachLogMoveAndTurnOperationImpl );
	
	void CLASS_INVARIANT;

	friend class MachLogMoveAndTurnOperation;
	
private:

	MachLogMoveAndTurnOperation::State 		state_;

	MachLogMachine *						pActor_;
	MexPoint3d								dest_;
	MexPoint3d								endFacing_;
	bool									checkForLeave_;		
	PhysAbsoluteTime 						dontMoveUntil_;
	PhysRelativeTime						initialDelay_;
};

PER_DECLARE_PERSISTENT( MachLogMoveAndTurnOperationImpl );

#endif

/* End MOVEI.HPP ***************************************************/
