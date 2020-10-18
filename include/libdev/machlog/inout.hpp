/*
 * I N O U T . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */
 
#ifndef _MACH_INOUT_HPP
#define _MACH_INOUT_HPP

#include "machlog/operatio.hpp"

/* //////////////////////////////////////////////////////////////// */

class MachLogMachine;
class MachLogConstruction;
class MachPhysStation;

class MachLogEnterBuildingOperation
: public MachLogOperation
{
public:

	MachLogEnterBuildingOperation( MachLogMachine* pActor, 
								MachLogConstruction* pConstr,
								MachPhysStation* pStation );
	//PRE( pActor->canEnterConstruction( *pConstr ) );

	MachLogEnterBuildingOperation( MachLogMachine* pActor, 
								MachLogConstruction* pConstr,
								MachPhysStation* pStation,
                                PhysPathFindingPriority );
	//PRE( pActor->canEnterConstruction( *pConstr ) );

	~MachLogEnterBuildingOperation();
	
	const MachLogConstruction& destination() const;

	virtual	bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogEnterBuildingOperation );
	PER_FRIEND_READ_WRITE( MachLogEnterBuildingOperation );

protected:

	virtual bool doStart();
	virtual void doFinish();
	
	virtual bool doIsFinished() const;
	virtual PhysRelativeTime doUpdate( );
	
	virtual void doOutputOperator( ostream& ) const;
	virtual bool doBeInterrupted();
	virtual bool isInterruptable();
	
private:

	MachLogMachine *		pActor_;
	MachLogConstruction	*	pConstruction_;
	MachPhysStation*		pStation_;
	enum Status
	{
	    STATION, GOING_TO_STATION, GOING_TO_INTERIOR, INTERNAL_POINT, 
	    GOING_TO_INTERNAL_POINT, EXTERNAL_POINT, GOING_TO_EXTERNAL_POINT, CLEAR, TERMINATE_EARLY

    };

    friend ostream& operator <<( ostream& o, Status );

    //true if the machine's clearance circle intersects the entrance interior polygon
    bool onInternalEntrancePolygon() const;

	PER_FRIEND_ENUM_PERSISTENT( Status );
	Status					status_;
	bool					buildingGone_;
	bool					currentlyAttached_;
	bool					needToGetAStation_; //obsolete
	bool					needToLockAStation_; //obsolete
	int						stationType_;
	bool					ignoreNeedToClearEntrancePolygon_;	
	bool					needToIgnoreInteriorPolygon_; //obsolete

};

PER_DECLARE_PERSISTENT( MachLogEnterBuildingOperation );
/* //////////////////////////////////////////////////////////////// */

class MachLogLeaveBuildingOperation
: public MachLogOperation
{
public:

	MachLogLeaveBuildingOperation( MachLogMachine * pActor, 
								MachLogConstruction *,
								MachPhysStation*  );
	
	~MachLogLeaveBuildingOperation();

	virtual	bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogLeaveBuildingOperation );
	PER_FRIEND_READ_WRITE( MachLogLeaveBuildingOperation );

protected:

	virtual bool doStart();
	virtual void doFinish();
	
	virtual bool doIsFinished() const;
	virtual PhysRelativeTime doUpdate( );
	
	virtual void doOutputOperator( ostream& ) const;
	virtual bool doBeInterrupted();
	virtual bool isInterruptable();
	

private:
	MachLogMachine *		pActor_;
	MachLogConstruction	*	pConstruction_;
	MachPhysStation*		pStation_;
	enum Status
	{
	    STATION, GOING_TO_INTERNAL_POINT, INTERNAL_POINT,
	    GOING_TO_EXTERNAL_POINT, EXTERNAL_POINT,
	    GOING_CLEAR, CLEAR, TERMINATE_EARLY
	};

    //True if the machine is on the external entrance polygon of the construction
    bool onExternalEntrancePolygon() const;

	PER_FRIEND_ENUM_PERSISTENT( Status );
	Status					status_;
	bool					buildingGone_;
	bool					currentlyAttached_;
	bool					needToGetAStation_; //obsolete
	bool					needToLockAStation_; //obsolete
	int						stationType_;
	bool					needToIgnoreExteriorPolygon_; //obsolete
	bool					needToIgnoreInteriorPolygon_; //obsolete
};

PER_DECLARE_PERSISTENT( MachLogLeaveBuildingOperation );

/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef _MACH_INOUT_HPP	*/

/* End INOUT.HPP ****************************************************/