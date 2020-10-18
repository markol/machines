/*
 * O P C O N S T I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogLabourOperationImpl

    Implementation class for MachLogLabourOperation.

*/

#ifndef _MACHLOG_OPLABOUI_HPP
#define _MACHLOG_OPLABOUI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machlog/oplabour.hpp"

// forward declarations

class MachLogBuildPoint;

class MachLogLabourOperationImpl
{
public:
	MachLogLabourOperationImpl( MachLogConstructor * pActor, MachLogConstruction * pConstr );
	~MachLogLabourOperationImpl();
	PER_MEMBER_PERSISTENT( MachLogLabourOperationImpl );
	PER_FRIEND_READ_WRITE( MachLogLabourOperationImpl );
	
	void CLASS_INVARIANT;

	friend class MachLogLabourOperation;
	
private:

	MachLogConstructor*						pActor_;
	MachLogConstruction*					pConstruction_;
	MachLogLabourOperation::State			state_;
	PhysAbsoluteTime						lastUpdateTime_;
	bool									currentlyAttached_;
	bool									hasBuildPoint_;
	MachLogBuildPoint*						pBuildPoint_;	
													
};

PER_DECLARE_PERSISTENT( MachLogLabourOperationImpl );

#endif

/* End OPLABOUI.HPP ***************************************************/
