/*
 * O P P U T D W I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogPutDownOperationImpl

    Implementation class for MachLogPutDownOperation.

*/

#ifndef _MACHLOG_OPPUTDWI_HPP
#define _MACHLOG_OPPUTDWI_HPP

#include "machlog/opputdwn.hpp"

#include "base/base.hpp"
#include "base/persist.hpp"

#include "mathex/point3d.hpp"

// forward declarations

class MachLogResourceCarrier;

class MachLogPutDownOperationImpl
{
public:		
	MachLogPutDownOperationImpl( MachLogResourceCarrier* );
							
	~MachLogPutDownOperationImpl();
	PER_MEMBER_PERSISTENT( MachLogPutDownOperationImpl );
	PER_FRIEND_READ_WRITE( MachLogPutDownOperationImpl );
	
	void CLASS_INVARIANT;

	friend class MachLogPutDownOperation;
	
private:

	MachLogResourceCarrier*		pActor_;
	MexPoint2d					destinationPadPosition_;
	bool						finished_;	
};

PER_DECLARE_PERSISTENT( MachLogPutDownOperationImpl );

#endif

/* End OPPUTDWI.HPP ***************************************************/
