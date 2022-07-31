/*
 * M O V E . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
 * Mixin for logical objects that support
 * a translational operation.
 */

#ifndef _MACHLOG_MOVE_HPP
#define _MACHLOG_MOVE_HPP

#include "phys/phys.hpp"

#include "machlog/operatio.hpp"

/* //////////////////////////////////////////////////////////////// */

// forward refs
class MachLogMachine;
class MexPoint2d;
class MexPoint3d;
class MachLogMoveToOperationImpl;
class MachLogMoveAndTurnOperationImpl;

template< class T > class ctl_list;

class MachLogMoveToOperation
: public MachLogOperation
{
public:

	enum NoId{ NO_ID = -1 };
	enum NoTolerance{ NO_TOLERANCE = -1 };

	typedef ctl_list< MexPoint2d > Path;

	// tolerance, if set, indicates the motion sequencer tolerance for approaching the destination, in metres
	MachLogMoveToOperation( MachLogMachine * pActor, const MexPoint3d& dest, bool checkForLeave = true, MATHEX_SCALAR tolerance = NO_TOLERANCE );
	MachLogMoveToOperation( MachLogMachine * pActor, const Path& dest, bool checkForLeave = true, MATHEX_SCALAR tolerance = NO_TOLERANCE );

	MachLogMoveToOperation( MachLogMachine * pActor, const MexPoint3d& dest, size_t commandId, bool checkForLeave = true, MATHEX_SCALAR tolerance = NO_TOLERANCE );
	MachLogMoveToOperation(
      MachLogMachine * pActor,
      const Path& dest,
      size_t commandId,
      PhysPathFindingPriority priority = Phys::defaultPathFindingPriority(),
      bool checkForLeave = true );

	virtual ~MachLogMoveToOperation();

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogMoveToOperation );
	PER_FRIEND_READ_WRITE( MachLogMoveToOperation );

protected:

	virtual bool doStart();
	virtual PhysRelativeTime doUpdate();
	virtual void doFinish();

	virtual bool doIsFinished() const;
	//virtual PhysRelativeTime doUpdate( );

	virtual void doOutputOperator( ostream& ) const;

	virtual bool doBeInterrupted();

private:

//	enum State { PLANNING, MOVING };

	MachLogMoveToOperationImpl* pImpl_;
};

PER_DECLARE_PERSISTENT( MachLogMoveToOperation );

/* //////////////////////////////////////////////////////////////// */

class MachLogMachine;
class MachActor;

class MachLogMoveAndTurnOperation
: public MachLogOperation
{
public:

	MachLogMoveAndTurnOperation( MachLogMachine * pActor,
								const MexPoint3d& dest,
								const MexPoint3d& endFacing,
								bool checkForLeave = true,
								PhysRelativeTime initialDelay = 0.0 );

	virtual ~MachLogMoveAndTurnOperation();

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogMoveAndTurnOperation );
	PER_FRIEND_READ_WRITE( MachLogMoveAndTurnOperation );

	friend class MachLogMoveAndTurnOperationImpl;

protected:

	virtual bool doStart();
	virtual void doFinish();

	virtual bool doIsFinished() const;
	virtual PhysRelativeTime doUpdate( );
	virtual bool doBeInterrupted();
	virtual void doOutputOperator( ostream& ) const;

private:

	enum State { MOVING, TURNING, WAITING };
	PER_FRIEND_ENUM_PERSISTENT( State );

	MachLogMoveAndTurnOperationImpl* pImpl_;
};

PER_DECLARE_PERSISTENT( MachLogMoveAndTurnOperation );
/* //////////////////////////////////////////////////////////////// */

#endif	//	#ifndef _MACHLOG_MOVE_HPP

/* End MOVE.HPP *****************************************************/

