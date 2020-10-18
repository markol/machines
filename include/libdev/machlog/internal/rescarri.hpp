/*
 * R E S C A R R I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogResourceCarrierImpl

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_RESCARRI_HPP
#define _MACHLOG_RESCARRI_HPP

#include "base/base.hpp"
//#include "ctl/list.hpp"

#include "machlog/rescarr.hpp"

#include "ctl/pvector.hpp"

#include "mathex/point2d.hpp"

class MachLogSmelter;

class MachLogResourceCarrierImpl
// Canonical form revoked
{
public:
    MachLogResourceCarrierImpl( void );
    ~MachLogResourceCarrierImpl( void );
	
	typedef  MachLogResourceCarrier::Suppliers	Suppliers;

    void CLASS_INVARIANT;
	PER_MEMBER_PERSISTENT_DEFAULT( MachLogResourceCarrierImpl );
	PER_FRIEND_READ_WRITE( MachLogResourceCarrierImpl );
	
	// returns nearest supplier from listOfCandidateSuppliers to position
	static Suppliers::iterator iNearestSupplier( Suppliers& listOfCandidateSuppliers,
										   		  const MexPoint2d& position );
	// PRE( not listOfCandidateSuppliers.empty() );
	// POST( iNearestSupplierFound != listOfCandidateSuppliers.end() );

	
	friend class MachLogResourceCarrier;

private:
    
	MachPhys::BuildingMaterialUnits			amountCarried_;
	bool 									isTransporting_;
	bool 									isPickingUp_;
	bool 									isPuttingDown_;
	MachLogResourceCarrier::Suppliers		suppliers_;
	MachLogConstruction* 					pDestinationSmeltingBuilding_;
	bool									smeltingBuildingExplicitlyAssigned_;
	bool									protectNodesFromClearance_;
	
	// if the carrier is doing a pickup, which supplier the carrier is currently moving to pick up from. 
	size_t									currentElement_;
	
	// If this is set to FOREVER, transport ops run using this carrier
	// do not terminate unless the op is actively removed from the queue.
	int										iterations_;
};

PER_DECLARE_PERSISTENT( MachLogResourceCarrierImpl );

#endif

/* End RESCARRI.HPP *************************************************/
