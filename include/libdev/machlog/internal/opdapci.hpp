/*
 * O P D A P C I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogDeployAPCOperationImpl

    Implementation class for MachLogDeployAPCOperation.

*/

#ifndef _MACHLOG_OPDAPCI_HPP
#define _MACHLOG_OPDAPCI_HPP

#include "machlog/opdapc.hpp"

#include "base/base.hpp"
#include "base/persist.hpp"

#include "mathex/point3d.hpp"

// forward declarations

class MachLogAPC;

class MachLogDeployAPCOperationImpl
{
public:		
	MachLogDeployAPCOperationImpl( MachLogAPC* pAPC );
							
	~MachLogDeployAPCOperationImpl();
	PER_MEMBER_PERSISTENT( MachLogDeployAPCOperationImpl );
	PER_FRIEND_READ_WRITE( MachLogDeployAPCOperationImpl );
	
	void CLASS_INVARIANT;

	friend class MachLogDeployAPCOperation;
	
private:

	MachLogAPC*								pAPC_;
	bool 									finished_;
	MachLogDeployAPCOperation::Status		status_;	
};

PER_DECLARE_PERSISTENT( MachLogDeployAPCOperationImpl );

#endif

/* End OPDAPCI.HPP ***************************************************/
