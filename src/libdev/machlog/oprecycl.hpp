/*
 * O P R E C Y C L . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogRecycleOperation

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_OPRECYCL_HPP
#define _MACHLOG_OPRECYCL_HPP

#include "base/base.hpp"
#include "mathex/point3d.hpp"
#include "machlog/operatio.hpp"


class MachLogMachine;
class MachLogSmelter;

// canonical form revoked

class MachLogRecycleOperation
: public MachLogOperation
{
public:

	MachLogRecycleOperation( MachLogMachine* pActor, MachLogConstruction* pSmeltingBuilding  );
	~MachLogRecycleOperation();

	bool beNotified( W4dSubject* pSubject, W4dSubject::NotificationEvent event, int clientData );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogRecycleOperation );
	PER_FRIEND_READ_WRITE( MachLogRecycleOperation );

protected:

	virtual bool doStart();
	// PRE( not isFinished() );
	virtual void doFinish();
	// PRE( isFinished() );

	virtual bool doIsFinished() const;
	virtual PhysRelativeTime doUpdate( );

	virtual void doOutputOperator( ostream& ) const;

	virtual bool doBeInterrupted();
	///////////////////////////////

	void CLASS_INVARIANT;

private:

	// Operations deliberately revoked
    MachLogRecycleOperation( const MachLogRecycleOperation& );
    MachLogRecycleOperation& operator =( const MachLogRecycleOperation& );
    bool operator ==( const MachLogRecycleOperation& );

	// will perform a find space and change *pClearRecyclePoint if necessary to that of a position near its original
	// value that is guaranteed to be both within recycle range of the pad and clear of all obstacles for the clearance
	// designated
	bool attemptToGuaranteeClearRecyclePoint( const MexPoint2d& actorPos, MATHEX_SCALAR machineClearance, MexPoint2d* pClearRecyclePoint );

	// will give destinationPadPosition_ a (potentially poor) default value of the first pad position if no pad
	// is found with adequate space around it for this machine
	bool selectBestRecyclePosition();

	// nearest pad's position irrespective of clear space requirements
	MexPoint2d nearestPadPosition() const;

	MachLogMachine *		pActor_;
	MachLogConstruction *	pSmeltingBuilding_;
	MexPoint2d				destinationPadPosition_;
	bool					finished_;
};

PER_DECLARE_PERSISTENT( MachLogRecycleOperation );


#endif

/* End OPRECYCL.HPP *************************************************/
