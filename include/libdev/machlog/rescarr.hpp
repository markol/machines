/*
 * T R A N R E S . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _MACHLOG_TRANRES_HPP
#define _MACHLOG_TRANRES_HPP

#include "machlog/machine.hpp"

/* //////////////////////////////////////////////////////////////// */

//class ostream;
class MachPhysResourceCarrier;
class MachPhysResourceCarrierData;
class MachLogResourceCarrierImpl;

class MachLogConstruction;
template < class X > class ctl_pvector;

class MachLogResourceCarrier
: public MachLogMachine // ,
//  public MachLogCanPickUp,
//  public MachLogCanPutDown
// cannonical form revoked
{
public:

	typedef ctl_pvector< MachLogConstruction >	Suppliers;
	
	enum SpecialIterations { FOREVER = -1 };
	enum StartWithPickUpOrPutDown { START_WITH_PUTDOWN, START_WITH_PICKUP };
	MachLogResourceCarrier( MachLogMachine::Level hwLevel, MachLogMachine::Level swLevel, 
    						MachLogRace * pRace, const MexPoint3d& location );

	MachLogResourceCarrier( MachLogMachine::Level hwLevel, MachLogMachine::Level swLevel, 
    						MachLogRace * pRace, const MexPoint3d& location, UtlId );

	virtual ~MachLogResourceCarrier();
		
	///////////////////////////////
	// right - these demand some (lengthy) explanation. No two ways about it, this is hacky, non-OO garbage
	// authorised by the highest authority as being the least of many alternative evils.
	// We are to treat a scavenger as a special type of resource carrier, even though this is now
	// fundamentally incorrect in terms of the class hierarchy. It violates the Lyskov Substitution
	// Principle horribly as resource carriers can't do what scavengers can, and scavengers can't do what
	// resource carriers can. To say that a scavenger IS-A resource carrier is a load of old cobblers, really,
	// but needs must......hence we must have a bunch of switches or if-thens all over the shop to fire different
	// code depending on whether we're "really" dealing with a resource carrier, or whether it's actually a scavenger.
	//
	// n.b. isNormalResourceCarrier is 100% equivalent to ( not isScavenger ), but both forms can be used to improve
	// semantic clarity in different contexts
	bool isScavenger() const;
	bool isNormalResourceCarrier() const;
	
	///////////////////////////////

	//view of MachPhys data objects
	virtual const MachPhysMachineData& machineData() const;
	const MachPhysResourceCarrierData& data() const;

	// inherited from MachLogCanPickUp
	virtual PhysRelativeTime pickUp();

	// inherited from MachLogCanPutDown
	virtual PhysRelativeTime putDown();

	MachPhys::BuildingMaterialUnits		amountCarried() const;
	MachPhys::BuildingMaterialUnits&	amountCarried();
	
	// whether or not the resource carrier is currently engaged in any of
	// a transport, pickup or putdown op
	bool isTransporting() const;	
	bool isPickingUp() const;	
	bool isPuttingDown() const;	
	
	
	// this adds to any existing list of suppliers
	void addSuppliers( const Suppliers& listOfSuppliers );
	
	// this totally replaces any existing list of suppliers
	void setSuppliers( const Suppliers& listOfSuppliers );
	
	void setSmeltingBuilding( MachLogConstruction* pSmeltingBuilding );
	
	void clearSuppliers();
	void clearSmeltingBuilding();
	void clearSuppliersAndSmeltingBuilding();
	
	bool hasSmeltingBuilding() const;
	bool hasSuppliers() const;
	int nSuppliers() const;
	
	void iterations( int );
	int iterations() const;
	
	// if free to self-assign smelter, will test all smelters for optimality as the 
	// new smelter may now be the best candidate
	void informOfNewSmeltingBuildingCompletion();
	
	///////////////////////////////
	//generic AI routinue which can be called by administrators and
	//controllers alike
	static void assignResourceCarrierTask( MachLogResourceCarrier* obj );
	
	// inherited from SimActor and MachActor
	virtual PhysRelativeTime update( const PhysRelativeTime& maxCPUTime, MATHEX_SCALAR clearanceFromDisplayedVolume );

	bool beNotified( W4dSubject* pSubject, W4dSubject::NotificationEvent event, int );
	
	const MachPhysResourceCarrier& physResourceCarrier() const;
	
	// does animation and sound, and returns interval for the duration of the animation
	PhysRelativeTime doLoading();
	
	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogResourceCarrier );
	PER_FRIEND_READ_WRITE( MachLogResourceCarrier );

	friend class MachLogPickUpOperation;
	friend class MachLogPutDownOperation;
	friend class MachLogScavengeOperation;
	friend class MachLogTransportOperation;

private:

	// operations deliberately revoked
	MachLogResourceCarrier( const MachLogResourceCarrier& );
	MachLogResourceCarrier& operator =( const MachLogResourceCarrier& );
	bool operator ==( const MachLogResourceCarrier& ) const;

	// ops only intended to be used by friend classes----------
	void addedTransportOp();
	void removedTransportOp();
	void addedPickUpOp();
	void removedPickUpOp();
	void addedPutDownOp();
	void removedPutDownOp();
	size_t currentSupplierIndex() const;
	void currentSupplierIndex( size_t newIndex );
	// PRE( newIndex < suppliers_.size() );
	
	void reassignIndex( MachLogConstruction* pSupplier );
	// PRE( pSupplier );	
	
	bool movingToNextSupplier();
	void movingToFirstSupplier();
	Suppliers& suppliers();
	MachLogConstruction* pDestinationSmeltingBuilding();
	
	// non-const ref to the supplier the index currently refers to
	MachLogConstruction& currentSupplier();
	// PRE( not doneAllSuppliers() );
	
	bool doneAllSuppliers();
	
	// helper function for observer pattern beNotified method.
	bool mayHaveLostRouteElement( W4dSubject* pSubject );
		
	// --------------------------------------------------------
	
	
	void internalSetSmeltingBuilding( MachLogConstruction* pSmeltingBuilding );
	void mayNeedNewOp( StartWithPickUpOrPutDown );
	bool findBestSmeltingBuilding();
	void reorderTransportRoute();
			
	void giveTransportRouteFailureVoicemail() const;
	void giveAlternativeDestinationVoicemail() const;
	
    static MachPhysResourceCarrier* pNewPhysResourceCarrier( Level hwLevel,  Level swLevel, MachLogRace * pRace,
                                                const MexPoint3d& location );
												
	MachPhysResourceCarrier& physResourceCarrier();

	virtual void doOutputOperator( ostream& o ) const;
	
	MachLogResourceCarrierImpl* 	pImpl_;
	};

PER_DECLARE_PERSISTENT( MachLogResourceCarrier );

/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef _MACHLOG_TRANSPRT_HPP	*/

/* End TRANSPRT.HPP *************************************************/