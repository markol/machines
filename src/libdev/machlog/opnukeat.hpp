/*
 * O P N U K E A T . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */
 
/*
 * Mixin for logical objects that support an
 * attack operation.
 */

#ifndef _MACHLOG_OPNUKEAT_HPP
#define _MACHLOG_OPNUKEAT_HPP

#include "phys/phys.hpp"

#include "machlog/animatio.hpp"
#include "machlog/operatio.hpp"

class MachActor;
/* //////////////////////////////////////////////////////////////// */

/* //////////////////////////////////////////////////////////////// */

// forward refs
class MachLogMissileEmplacement;
class MachLogCanAttack;
class MachActor;

// canonical form revoked

class MachLogNukeAttackOperation
: public MachLogOperation
{
public:

	MachLogNukeAttackOperation( MachLogMissileEmplacement * pActor,
							MachActor * pDirectObject );
	
	MachLogNukeAttackOperation( MachLogMissileEmplacement * pActor,
							const MexPoint3d& targetPosition );						
							
	~MachLogNukeAttackOperation();
	
	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogNukeAttackOperation );
	PER_FRIEND_READ_WRITE( MachLogNukeAttackOperation );

protected:

	virtual bool doStart();
	virtual void doFinish();
	
	virtual bool doIsFinished() const;
	virtual PhysRelativeTime doUpdate();

	virtual void doOutputOperator( ostream& ) const;

	MachActor& directObject();
	const MachActor& directObject() const;
	
	virtual bool doBeInterrupted();


private:

	// Operations deliberately revoked
    MachLogNukeAttackOperation( const MachLogNukeAttackOperation& );
    MachLogNukeAttackOperation& operator =( const MachLogNukeAttackOperation& );
    bool operator ==( const MachLogNukeAttackOperation& );
	
	MachLogMissileEmplacement*				pActor_;
	MexPoint3d								targetPosition_;
};

PER_DECLARE_PERSISTENT( MachLogNukeAttackOperation );


/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef 	_MACHLOG_OPNUKEAT_HPP	*/

/* End OPNUKEAT.HPP ***************************************************/
