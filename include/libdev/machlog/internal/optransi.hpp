/*
 * O P T R A N S I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogTransportOperationImpl

    Implementation class for MachLogTransportOperation.

*/

#ifndef _MACHLOG_OPTRANSI_HPP
#define _MACHLOG_OPTRANSI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "ctl/pvector.hpp"

#include "machlog/actor.hpp"
#include "machlog/optransp.hpp"

// forward declarations

class MachLogTransportOperationImpl
{
public:
	MachLogTransportOperationImpl( MachLogResourceCarrier* );
	~MachLogTransportOperationImpl();
	PER_MEMBER_PERSISTENT( MachLogTransportOperationImpl );
	PER_FRIEND_READ_WRITE( MachLogTransportOperationImpl );
	
	void CLASS_INVARIANT;

	friend class MachLogTransportOperation;
	
private:

	MachLogResourceCarrier*						pActor_;
	bool										finished_;
	bool										donePickUp_;	
													
};

PER_DECLARE_PERSISTENT( MachLogTransportOperationImpl );

#endif

/* End OPTRANSI.HPP ***************************************************/
