/*
 * A P C I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogAPCImpl

    APC implemenation class
*/

#ifndef _MACHLOG_APCI_HPP
#define _MACHLOG_APCI_HPP

#include "base/base.hpp"
#include "ctl/pvector.hpp"
#include "mathex/point3d.hpp"
#include "machlog/apc.hpp"

class MachLogAPCImpl
// Canonical form revoked
{
public:
    MachLogAPCImpl();
    ~MachLogAPCImpl();

    void CLASS_INVARIANT;

	PER_MEMBER_PERSISTENT_DEFAULT( MachLogAPCImpl );
	PER_FRIEND_READ_WRITE( MachLogAPCImpl );

private:
	friend class MachLogAPC;
    friend ostream& operator <<( ostream& o, const MachLogAPCImpl& t );

    MachLogAPCImpl( const MachLogAPCImpl& );
    MachLogAPCImpl& operator =( const MachLogAPCImpl& );

	int									nMachinesCarried_;
	int 								nMachinesIncoming_;
	MachPhys::BuildingMaterialUnits		amountCarried_;
	MachLogAPC::Machines				carriedMachines_;
	MachLogAPC::Machines				incomingMachines_;
	bool 								turningTowardMachine_;
	
	int									nDeployOps_;			// this bears a little explanation - there can potentially be an overlap period 
																// when a new deploy op has been constructed and before the old one is bumped off 
																// the queue and destructed. If we used a boolean "isDeploying_", the destructor of the old
																// op would set this to false even though a new op has just been put on a few moments before.
	MexPoint3d							dropzonePosition_;	

};

PER_DECLARE_PERSISTENT( MachLogAPCImpl );

#endif

/* End APCI.HPP *****************************************************/
