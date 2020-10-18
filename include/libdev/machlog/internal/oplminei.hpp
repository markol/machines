/*
 * O P L M I N E I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogDropLandMineOperationImpl

    Implementation class for MachLogDropLandMineOperation.

*/

#ifndef _MACHLOG_OPLMINEI_HPP
#define _MACHLOG_OPLMINEI_HPP

#include "machlog/oplmine.hpp"

#include "base/base.hpp"
#include "base/persist.hpp"

#include "mathex/point3d.hpp"
#include "mathex/point2d.hpp"

#include "ctl/list.hpp"

// forward declarations

class MachLogSpyLocator;

class MachLogDropLandMineOperationImpl
{
public:		
	MachLogDropLandMineOperationImpl( MachLogSpyLocator * pActor, const MexPoint3d& dest );
	MachLogDropLandMineOperationImpl( MachLogSpyLocator * pActor, const MachLogDropLandMineOperation::Path& externalPath );
							
	~MachLogDropLandMineOperationImpl();
	PER_MEMBER_PERSISTENT( MachLogDropLandMineOperationImpl );
	PER_FRIEND_READ_WRITE( MachLogDropLandMineOperationImpl );
	
	void CLASS_INVARIANT;

	friend class MachLogDropLandMineOperation;
	
private:

	MachLogSpyLocator*						pActor_;
	MexPoint3d								dest_;
	MachLogDropLandMineOperation::Path		path_;
	int										failedAttempts_;
	bool									moved_;	
};

PER_DECLARE_PERSISTENT( MachLogDropLandMineOperationImpl );

#endif

/* End OPLMINEI.HPP ***************************************************/
