/*
 * O P A U T S C I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogAutoScavengeOperationImpl

    Implementation class for MachLogAutoScavengeOperation.

*/

#ifndef _MACHLOG_OPAUTSCI_HPP
#define _MACHLOG_OPAUTSCI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "phys/phys.hpp"

// forward declarations

class MachLogResourceCarrier;
class MachLogDebris;
class MachLogOperation;
class MachLogAutoScavengeOperation;

class MachLogAutoScavengeOperationImpl
{
public:
	MachLogAutoScavengeOperationImpl( MachLogResourceCarrier* pScavenger, MachLogDebris* pDebris );
	// 	PRE( pScavenger->isScavenger() );
	
	~MachLogAutoScavengeOperationImpl();
	PER_MEMBER_PERSISTENT( MachLogAutoScavengeOperationImpl );
	PER_FRIEND_READ_WRITE( MachLogAutoScavengeOperationImpl );
	
	void CLASS_INVARIANT;

	friend class MachLogAutoScavengeOperation;
    friend void perRead( PerIstream& istr, MachLogAutoScavengeOperation& op );
	
private:

	MachLogResourceCarrier*							pScavenger_;
	
	MachLogDebris*									pDebris_;
	
	bool											finished_;
	bool											initiatedScavengeOp_;	
													
													// stores the topmost op the machine was doing before
													// all this AutoScavenge nonsense was started
	MachLogOperation*								pCachedOperation_;
													
};

PER_DECLARE_PERSISTENT( MachLogAutoScavengeOperationImpl );

#endif

/* End OPAUTSCI.HPP ***************************************************/
