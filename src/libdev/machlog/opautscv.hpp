/*
 * O P A U T S C V . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */
 
/*
 *	AutoScavenge operation
 * 	Supervising class for a self-initiated scavenge op, which stores a pending operation (what the scavenger was doing
 *  before this began) and restores it when the scavenge op itself is finished.
 */

#ifndef _MACHLOG_OPAUTSCV_HPP
#define _MACHLOG_OPAUTSCV_HPP

#include "phys/phys.hpp"

#include "machlog/operatio.hpp"

class MachLogAutoScavengeOperationImpl;

/* //////////////////////////////////////////////////////////////// */

/* //////////////////////////////////////////////////////////////// */

// forward refs
class MachLogStrategy;
class MachLogResourceCarrier;
class MachLogDebris;


// orthodox canonical (revoked)

class MachLogAutoScavengeOperation
: public MachLogOperation
{
public:
	
	virtual ~MachLogAutoScavengeOperation();

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogAutoScavengeOperation );
	PER_FRIEND_READ_WRITE( MachLogAutoScavengeOperation );
	
protected:

	virtual bool doStart();
	virtual void doFinish();
	
	virtual bool doIsFinished() const;
	virtual PhysRelativeTime doUpdate();

	virtual void doOutputOperator( ostream& ) const;
	
	virtual bool doBeInterrupted();
	
	void storeOldFirstOperation( MachLogOperation* );
	
	virtual bool beNotified( W4dSubject* pSubject, 
							 W4dSubject::NotificationEvent event, int clientData );


private:

	// Operations deliberately revoked
    MachLogAutoScavengeOperation( const MachLogAutoScavengeOperation& );
    MachLogAutoScavengeOperation& operator =( const MachLogAutoScavengeOperation& );
    bool operator ==( const MachLogAutoScavengeOperation& );
	
	// private constructor - can only be built from within MachLogStrategy.
	friend class MachLogStrategy;

	MachLogAutoScavengeOperation( MachLogResourceCarrier* pScavenger, MachLogDebris* pDebris );
	// PRE( pScavenger->isScavenger() );
	
	bool shouldStopScavenging();

	// data members
	MachLogAutoScavengeOperationImpl*		pImpl_;	  
};


PER_DECLARE_PERSISTENT( MachLogAutoScavengeOperation );

/* //////////////////////////////////////////////////////////////// */
	
#endif	/*	#ifndef 	_MACHLOG_OPAUTSCV_HPP	*/

/* End OPAUTSCV.HPP ***************************************************/
