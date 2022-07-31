/*
 * O P S C A V I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogScavengeOperationImpl

    Implementation class for MachLogScavengeOperation.

*/

#ifndef _MACHLOG_OPSCAVI_HPP
#define _MACHLOG_OPSCAVI_HPP

#include "machlog/opscav.hpp"

#include "base/base.hpp"
#include "base/persist.hpp"

#include "ctl/pvector.hpp"

#include "mathex/point3d.hpp"

#include "machlog/actor.hpp"

// forward declarations

class MachLogResourceCarrier;
class MachActor;

class MachLogScavengeOperationImpl
{
public:		
	MachLogScavengeOperationImpl( MachLogResourceCarrier* );
							
	~MachLogScavengeOperationImpl();
	PER_MEMBER_PERSISTENT( MachLogScavengeOperationImpl );
	PER_FRIEND_READ_WRITE( MachLogScavengeOperationImpl );
	
	void CLASS_INVARIANT;

	friend class MachLogScavengeOperation;
	
private:

	MachLogResourceCarrier *				pActor_;
	bool									finished_;
	MachLogScavengeOperation::Suppliers		suppliers_;
	size_t									currentElement_;
	MexPoint3d								dest_;	
};

PER_DECLARE_PERSISTENT( MachLogScavengeOperationImpl );

#endif

/* End OPSCAVI.HPP ***************************************************/
