/*
 * O P E V A D E . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */
 
/*
 * Heal Operation
 */

#ifndef _MACHLOG_OPEVADE_HPP
#define _MACHLOG_OPEVADE_HPP

#include "phys/phys.hpp"

#include "machlog/operatio.hpp"

class MachLogEvadeOperationImpl;

/* //////////////////////////////////////////////////////////////// */

/* //////////////////////////////////////////////////////////////// */

// forward refs
class MachLogStrategy;
class MachLogMachine;
class MachActor;

/*
class MachLogCanHeal;
class MachLogAggressor;
class MachLogAdministrator;
class MachActor;
*/


// orthodox canonical (revoked)

class MachLogEvadeOperation
: public MachLogOperation
{
public:
	
	virtual ~MachLogEvadeOperation();

	enum EvadeOpType { SEEK_GARRISON, SEEK_AGGRESSIVE, SEEK_TURRET, SEEK_POD, SEEK_SAFER_GROUND };

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogEvadeOperation );
	PER_FRIEND_READ_WRITE( MachLogEvadeOperation );
	
protected:

	virtual bool doStart();
	virtual void doFinish();
	
	virtual bool doIsFinished() const;
	virtual PhysRelativeTime doUpdate();

	virtual void doOutputOperator( ostream& ) const;
	
	virtual bool doBeInterrupted();
	
	void storeOldFirstOperation( MachLogOperation* );


private:

	// Operations deliberately revoked
    MachLogEvadeOperation( const MachLogEvadeOperation& );
    MachLogEvadeOperation& operator =( const MachLogEvadeOperation& );
    bool operator ==( const MachLogEvadeOperation& );
	
	// private constructor - can only be built from within MachLogStrategy.
	friend class MachLogStrategy;

	MachLogEvadeOperation( MachLogMachine * pActor );

	// data members
	MachLogEvadeOperationImpl*		pImpl_;	  
};


PER_DECLARE_PERSISTENT( MachLogEvadeOperation );
PER_ENUM_PERSISTENT( MachLogEvadeOperation::EvadeOpType );

/* //////////////////////////////////////////////////////////////// */
	
#endif	/*	#ifndef 	_MACHLOG_OPEVADE_HPP	*/

/* End Heal.HPP ***************************************************/
