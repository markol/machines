/*
 * O P L A B O U R . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogLabourOperation

    This is an ABSTRACT BASE CLASS for all operations involving constructors interacting with buildings.
*/

#ifndef _MACHLOG_OPLABOUR_HPP
#define _MACHLOG_OPLABOUR_HPP

#include "base/base.hpp"
#include "phys/phys.hpp"
#include "machlog/operatio.hpp"


class MachLogConstructor;
class MachLogConstruction;
class MachLogLabourOperationImpl;

// orthodox canonical (revoked)

class MachLogLabourOperation
: public MachLogOperation
{
public:

	MachLogLabourOperation( MachLogConstructor * pActor, MachLogConstruction *, const char*, MachLogOperation::OperationType );
	virtual ~MachLogLabourOperation();
	
	PER_MEMBER_PERSISTENT_ABSTRACT( MachLogLabourOperation );
	PER_FRIEND_READ_WRITE( MachLogLabourOperation );
	
	virtual	bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData );
	
	// further distance than basic highclearence a constructor is allowed to be from its buildpoint
	// to be allowed to interact with the building.
	static MATHEX_SCALAR proximityLeewayMultiplier();
	
	friend class MachLogLabourOperationImpl;

protected:

	virtual bool doStart();
	virtual void doFinish();
	virtual bool doBeInterrupted();
	
	virtual void doOutputOperator( ostream& ) const;
	virtual PhysRelativeTime doUpdate( );		
	
	MachLogConstruction* pConstruction();
	const MachLogConstruction* pConstruction() const;
	void pConstruction( MachLogConstruction* );
	
	MachLogConstructor* pConstructor();
	const MachLogConstructor* pConstructor() const;
	
	bool currentlyAttached() const;
	void currentlyAttached( bool );
	
	PhysAbsoluteTime lastUpdateTime() const;
	void lastUpdateTime( PhysAbsoluteTime );
	
	bool constructorFacingBuilding() const;

	void tryToAssignToNearestBuildPoint();
	void giveUpBuildPoint();
	enum State { MOVING, INTERACTING };
	
	State state() const;
	void state( State newState );
	
	virtual bool clientSpecificNotification( int clientData ) = 0;

private:

	// Operations deliberately revoked
    MachLogLabourOperation( const MachLogLabourOperation& );
    MachLogLabourOperation& operator =( const MachLogLabourOperation& );
    bool operator ==( const MachLogLabourOperation& );
	
	PhysRelativeTime attemptToInteractWithBuilding();
	
	// pure virtual method
	virtual PhysRelativeTime interactWithBuilding() = 0;
	
	
	PER_FRIEND_ENUM_PERSISTENT( State );
		
	// data members
	MachLogLabourOperationImpl* pImpl_;
	
};

PER_DECLARE_PERSISTENT( MachLogLabourOperation );

#endif

/* End OPLABOUR.HPP *************************************************/
