/*
 * O P A D L O C I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogAdminLocateOperationImpl

    Implementation class for MachLogAdminLocateOperation.

*/

#ifndef _MACHLOG_OPADLOCI_HPP
#define _MACHLOG_OPADLOCI_HPP

#include "machlog/opadloc.hpp"

#include "base/base.hpp"
#include "base/persist.hpp"

#include "mathex/point3d.hpp"

// forward declarations

class MachLogAdministrator;

class MachLogAdminLocateOperationImpl
{
public:		

	MachLogAdminLocateOperationImpl( MachLogAdministrator *  pActor );
	//single point
	MachLogAdminLocateOperationImpl( MachLogAdministrator *  pActor, const MexPoint3d& dest );
	//way point path
	MachLogAdminLocateOperationImpl( MachLogAdministrator *  pActor, const MachLogAdminLocateOperation::Path& externalPath );
							
	~MachLogAdminLocateOperationImpl();
	PER_MEMBER_PERSISTENT( MachLogAdminLocateOperationImpl );
	PER_FRIEND_READ_WRITE( MachLogAdminLocateOperationImpl );
	
	void CLASS_INVARIANT;

	friend class MachLogAdminLocateOperation;
	
private:

	MachLogAdministrator*				pActor_;
    bool 								complete_;
	MexPoint3d							dest_;
	MachLogAdminLocateOperation::Path	path_;
	MachLogAdminLocateOperation::Path	externalPath_;
	bool								hasExternalPath_;
	int									currentElement_;	
};

PER_DECLARE_PERSISTENT( MachLogAdminLocateOperationImpl );

#endif

/* End OPADLOCI.HPP ***************************************************/
