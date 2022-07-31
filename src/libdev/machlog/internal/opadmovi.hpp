/*
 * O P A D M O V I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogAdminMoveToOperationImpl

    Implementation class for MachLogAdminMoveToOperation.

*/

#ifndef _MACHLOG_OPADMOVI_HPP
#define _MACHLOG_OPADMOVI_HPP

#include "machlog/opadmove.hpp"

#include "base/base.hpp"
#include "base/persist.hpp"

#include "ctl/list.hpp"
#include "mathex/point3d.hpp"

// forward declarations

class MachLogAdministrator;

class MachLogAdminMoveToOperationImpl
{
public:		

	MachLogAdminMoveToOperationImpl( MachLogAdministrator* pActor, const MexPoint3d& dest, bool checkForLeave, MachLogFollowOperation::CamouflagedOp camoStatus );
	MachLogAdminMoveToOperationImpl( MachLogAdministrator* pActor, const MachLogAdminMoveToOperation::Path& dest, bool checkForLeave, MachLogFollowOperation::CamouflagedOp camoStatus );
							
	~MachLogAdminMoveToOperationImpl();
	PER_MEMBER_PERSISTENT( MachLogAdminMoveToOperationImpl );
	PER_FRIEND_READ_WRITE( MachLogAdminMoveToOperationImpl );
	
	void CLASS_INVARIANT;

	friend class MachLogAdminMoveToOperation;
	
private:

	MachLogAdministrator*						pActor_;
    bool 										complete_;
	MexPoint3d									dest_;
	bool										checkForLeave_;
	MachLogAdminMoveToOperation::Path 			path_;
	MachLogFollowOperation::CamouflagedOp 		camoStatus_;	// whether or not this operation is to be executed with camouflage enabled
};

PER_DECLARE_PERSISTENT( MachLogAdminMoveToOperationImpl );

#endif

/* End OPADMOVI.HPP ***************************************************/
