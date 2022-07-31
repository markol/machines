/*
 * O P C O N S T I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogConstructOperationImpl

    Implementation class for MachLogConstructOperation.

*/

#ifndef _MACHLOG_OPCONSTI_HPP
#define _MACHLOG_OPCONSTI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machlog/opconstr.hpp"

// forward declarations

class MachLogBuildPoint;

class MachLogConstructOperationImpl
{
public:
	MachLogConstructOperationImpl( MachLogConstructor * pActor, MachLogConstruction * pConstr );
	~MachLogConstructOperationImpl();
	PER_MEMBER_PERSISTENT( MachLogConstructOperationImpl );
	PER_FRIEND_READ_WRITE( MachLogConstructOperationImpl );
	
	void CLASS_INVARIANT;

	friend class MachLogConstructOperation;
	
private:

	MachLogConstructor *					pActor_;
	MexPoint3d								dest_;
	MachLogConstruction *					pConstruction_;
	MachLogConstructOperation::State		state_;
	PhysAbsoluteTime						lastUpdateTime_;
	bool									currentlyAttached_;
	bool									hasBuildPoint_;
	MachLogBuildPoint*						pBuildPoint_;	
													
};

PER_DECLARE_PERSISTENT( MachLogConstructOperationImpl );

#endif

/* End OPCONSTI.HPP ***************************************************/
