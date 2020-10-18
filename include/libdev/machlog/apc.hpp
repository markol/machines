/*
 * A P C . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _MACHLOG_APC_HPP
#define _MACHLOG_APC_HPP

#include "machlog/machine.hpp"
#include "machlog/opdapc.hpp"

/* //////////////////////////////////////////////////////////////// */

//class ostream;
class MachPhysAPC;
class MachPhysAPCData;
class MachLogAPCImpl;

class MachLogAPC
: public MachLogMachine 
// cannonical form revoked
{
public:

	typedef ctl_pvector< MachLogMachine > Machines;

	MachLogAPC( MachLogMachine::Level hwLevel, MachLogMachine::Level swLevel, 
    						MachLogRace * pRace, const MexPoint3d& location );

	MachLogAPC( MachLogMachine::Level hwLevel, MachLogMachine::Level swLevel, 
    						MachLogRace * pRace, const MexPoint3d& location, UtlId );

	virtual ~MachLogAPC();
	
	///////////////////////////////

	//view of MachPhys data objects
	const Machines& machines() const;

	virtual const MachPhysMachineData& machineData() const;
	const MachPhysAPCData& data() const;

	const MachPhys::BuildingMaterialUnits&		amountCarried() const;
	MachPhys::BuildingMaterialUnits&	amountCarried();

	// For internally carried machines 
	bool add( MachLogMachine* );
	void remove( MachLogMachine* pMachineRemoved, const MexPoint3d& dest3d );	// For pre-ordained deployment points
	
	int	nMachinesCarried() const;
	
	// For machines en route to APC for loading
	void addIncoming( MachLogMachine* pNewIncomingMachine );
	//PRE( find( incomingMachines_.begin(), incomingMachines_.end(), pNewIncomingMachine ) == incomingMachines_.end() );
	
	void removeIncoming( MachLogMachine* pOldIncomingMachine );
	//PRE( find( incomingMachines_.begin(), incomingMachines_.end(), pOldIncomingMachine ) == incomingMachines_.end() );
	
	bool isDeploying() const;
	const MexPoint3d& dropzonePosition() const;
	bool nearDropzone() const;
	// PRE( isDeploying() );
	
	bool alignedWithDeployPoint(const MexPoint3d& dest3d);
	
	bool nextDeployPoint(MexPoint3d* dest3d,  MATHEX_SCALAR clearanceOfDeployed);	//updates dest3d as side-effect; returns true if valid point found.
	
	///////////////////////////////
	//generic AI routinue which can be called by administrators and
	//controllers alike
	static void assignAPCTask( MachLogAPC* obj );
	
	virtual PhysRelativeTime update( const PhysRelativeTime& maxCPUTime,
                                     MATHEX_SCALAR clearance );											
	
	// APC needs special case code to deal with the eventuality of dying with a bunch of machines inside it.
	virtual void beHit( const int&, MachPhys::WeaponType byType = MachPhys::N_WEAPON_TYPES,
						MachActor* pByActor = NULL, MexLine3d* pByDirection = NULL, MachActor::EchoBeHit = MachActor::ECHO );

	virtual void beHitWithoutAnimation( int damage, PhysRelativeTime physicalTimeDelay = 0, MachActor* pByActor = NULL, EchoBeHit = ECHO );

	bool roomForMachine( const MachLogMachine& machine ) const;
	
	virtual	bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData );
	
	friend class MachLogDeployAPCOperation;
	
	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogAPC );
	PER_FRIEND_READ_WRITE( MachLogAPC );

private:

	MachLogAPC( const MachLogAPC& );
	MachLogAPC& operator =( const MachLogAPC& );
	bool operator ==( const MachLogAPC& ) const;

    static MachPhysAPC* pNewPhysAPC( Level hwLevel,  Level swLevel, MachLogRace * pRace,
                                                const MexPoint3d& location );

	virtual void doOutputOperator( ostream& o ) const;
	
	void turnRearToFaceNearestIncoming( void );
	// PRE( nMachinesIncoming_ > 0 );
	
	const MachLogMachine& currentNearestMachine() const;
	// PRE( nMachinesIncoming_ > 0 );
												
	void addedDeployOp();
	void removedDeployOp();
	void dropzonePosition( const MexPoint3d& position );
					
	void specialAPCHitStuff();
	
	static const MATHEX_SCALAR&					graceDistance();			// machine must become at least X metres closer to take over as 'current
																			// nearest' - this will prevent twitchy "threshing" when a number of machines
																			// are very close by and regularly altering position and distance).

	static const MATHEX_SCALAR&					sqrCloseDistance();			// Distance at which angle becomes priority for APC, not distance

	// data members
	MachLogAPCImpl*		pImpl_;
	
};

PER_DECLARE_PERSISTENT( MachLogAPC );
/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef _MACHLOG_TRANSPRT_HPP	*/

/* End TRANSPRT.HPP *************************************************/