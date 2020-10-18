/*
 * O P E R A T I I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogOperationImpl

    Implementation class for MachLogOperation.

*/

#ifndef _MACHLOG_OPERATII_HPP
#define _MACHLOG_OPERATII_HPP

#include "base/base.hpp"
#include "stdlib/string.hpp"
#include "base/persist.hpp"
#include "machlog/operatio.hpp"
#include "machlog/grpmvinf.hpp"



class MachLogOperationImpl
{
public:
	MachLogOperationImpl( const char* typeOp, MachLogOperation::OperationType typeOpEnum, PhysPathFindingPriority );
	~MachLogOperationImpl();
	PER_MEMBER_PERSISTENT( MachLogOperationImpl );
	PER_FRIEND_READ_WRITE( MachLogOperationImpl );
	
	void CLASS_INVARIANT;

	friend class MachLogOperation;
	
private:
	MachLogOperation::Progress			progress_;
	MachLogOperation::Priority			priority_;
	MachLogOperation::OperationType		operationType_;
	
	PhysAbsoluteTime				nextCallBackTime_;
	MachLogOperation*				pSubOperation_;
	string							operationTypeAsString_;
	MachLogGroupMoveInfo            groupMoveInfo_;
    PhysPathFindingPriority         pathFindingPriority_;
};

PER_DECLARE_PERSISTENT( MachLogOperationImpl );

#endif

/* End OPERATII.HPP ***************************************************/
