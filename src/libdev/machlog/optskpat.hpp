/*
 * O P T S K P A T . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.


		AI squadron module.
		Designed for the generic Patrol squadrons. This squadron task
		may not be limited to the AI - hence do not make assumptions about the
		controller interface.
 */

#ifndef _MACHLOG_OPTSKPAT_HPP
#define _MACHLOG_OPTSKPAT_HPP

#include "ctl/vector.hpp"
#include "phys/phys.hpp"
#include "mathex/point2d.hpp"
#include "machlog/machlog.hpp"
#include "machlog/optsk.hpp"


/* //////////////////////////////////////////////////////////////// */

class MachLogSquadron;

// canonical form revoked

class MachLogTaskPatrolOperation
: public MachLogTaskOperation
{
public:

	typedef ctl_vector< MexPoint2d > Path;

	MachLogTaskPatrolOperation( MachLogSquadron *  pActor, const Path& path, const PhysRelativeTime& initialDelay );

	virtual ~MachLogTaskPatrolOperation( );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogTaskPatrolOperation );
	PER_FRIEND_READ_WRITE( MachLogTaskPatrolOperation );

protected:

	virtual bool doStart();
	virtual void doFinish();
	
	virtual bool doIsFinished() const;
	virtual PhysRelativeTime doUpdate();
	
	virtual void doOutputOperator( ostream& ) const;
	virtual bool doBeInterrupted();


private:

	// Operations deliberately revoked
    MachLogTaskPatrolOperation( const MachLogTaskPatrolOperation& );
    MachLogTaskPatrolOperation& operator =( const MachLogTaskPatrolOperation& );
    bool operator ==( const MachLogTaskPatrolOperation& );
	
	PhysRelativeTime doUpdateWithCommander();
	PhysRelativeTime doUpdateWithoutCommander();

	MachLogSquadron *	pActor_;
    bool 				complete_;
	Path				path_;
	PhysRelativeTime	initialDelay_;
	bool				initialDelayProcessed_;
	PhysAbsoluteTime	startTime_;

};

PER_DECLARE_PERSISTENT( MachLogTaskPatrolOperation );

/* //////////////////////////////////////////////////////////////// */

#endif
