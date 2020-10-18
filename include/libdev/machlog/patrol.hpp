/*
 * P A T R O L . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#ifndef _MACHLOG_PATROL_HPP
#define _MACHLOG_PATROL_HPP

#include "ctl/vector.hpp"

#include "mathex/point2d.hpp"

#include "machlog/animatio.hpp"
#include "machlog/operatio.hpp"

/* //////////////////////////////////////////////////////////////// */
		   
// forward refs
class MachLogAdministrator;
class MachLogAggressor;
class MachLogMachine;
class MachActor;

class MachLogPatrolOperation
: public MachLogOperation
{
public:

	typedef ctl_vector< MexPoint2d > Path;

	MachLogPatrolOperation( MachLogMachine * pActor, const Path& path, const bool& loop );
	
	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogPatrolOperation );
	PER_FRIEND_READ_WRITE( MachLogPatrolOperation );

protected:

	virtual bool doStart();
	virtual void doFinish();
	
	virtual bool doIsFinished() const;
	virtual PhysRelativeTime doUpdate( );
	virtual bool doBeInterrupted();	
	virtual void doOutputOperator( ostream& ) const;
	

private:

	MachLogMachine*			pActor_;
	Path					path_;
	int						pathElement_;
	bool					finished_;
	bool					loop_;
};

PER_DECLARE_PERSISTENT( MachLogPatrolOperation );

/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef _MACHLOG_PATROL_HPP	*/

/* End PATROL.HPP ***************************************************/