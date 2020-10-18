/*
 * O P A D M O V E . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 * 
 *	Administrator move operation. mainly for use with squadrons.
 *  This operation will move machines in a convoy centred around the
 *  Administrator
 */

#ifndef _MACHLOG_OPADMOVE_HPP
#define _MACHLOG_OPADMOVE_HPP

//#include "ctl/list.hpp"

#include "phys/phys.hpp"

//#include "mathex/point2d.hpp"
//#include "mathex/point3d.hpp"
#include "machlog/machlog.hpp"
#include "machlog/operatio.hpp"
#include "machlog/follow.hpp"

/* //////////////////////////////////////////////////////////////// */

class MachLogAdministrator;
class MachLogGeoLocator;
class MachLogAdminMoveToOperationImpl;

// orthodox canonical (revoked)

class MachLogAdminMoveToOperation
: public MachLogOperation
{
public:

	typedef ctl_list< MexPoint2d > Path;

	MachLogAdminMoveToOperation( MachLogAdministrator * pActor, const MexPoint3d& dest, bool checkForLeave = true, MachLogFollowOperation::CamouflagedOp camoStatus = MachLogFollowOperation::NOT_CAMOUFLAGED );
	MachLogAdminMoveToOperation(
      MachLogAdministrator * pActor,
      const Path& dest,
      PhysPathFindingPriority priority = Phys::defaultPathFindingPriority(),
      bool checkForLeave = true,
      MachLogFollowOperation::CamouflagedOp camoStatus = MachLogFollowOperation::NOT_CAMOUFLAGED );

	virtual ~MachLogAdminMoveToOperation();

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogAdminMoveToOperation );
	PER_FRIEND_READ_WRITE( MachLogAdminMoveToOperation );

protected:

	virtual bool doStart();
	virtual void doFinish();
	
	virtual bool doIsFinished() const;
	virtual PhysRelativeTime doUpdate();
	
	virtual void doOutputOperator( ostream& ) const;
	virtual bool doBeInterrupted();


private:

	// Operations deliberately revoked
    MachLogAdminMoveToOperation( const MachLogAdminMoveToOperation& );
    MachLogAdminMoveToOperation& operator =( const MachLogAdminMoveToOperation& );
    bool operator ==( const MachLogAdminMoveToOperation& );

	MachLogAdminMoveToOperationImpl* pImpl_;
};

PER_DECLARE_PERSISTENT( MachLogAdminMoveToOperation );

/* //////////////////////////////////////////////////////////////// */

#endif