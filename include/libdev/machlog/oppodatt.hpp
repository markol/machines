/*
 * O P P O D A T T . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */
 
/*
 * Mixin for logical objects that support an
 * attack operation.
 */

#ifndef _MACHLOG_PODATTACK_HPP
#define _MACHLOG_PODATTACK_HPP

#include "phys/phys.hpp"

#include "machlog/animatio.hpp"
#include "machlog/operatio.hpp"

class MachActor;
/* //////////////////////////////////////////////////////////////// */

/* //////////////////////////////////////////////////////////////// */

// forward refs
class MachLogPod;
class MachLogCanAttack;
class MachActor;

// canonical form revoked

class MachLogPodAttackOperation
: public MachLogOperation
{
public:

	MachLogPodAttackOperation( MachLogPod * pActor,
    						   const MexPoint3d& targetPosition );
    

	MachLogPodAttackOperation( MachLogPod * pActor,
							   MachActor * pDirectObject );

	~MachLogPodAttackOperation();

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogPodAttackOperation );
	PER_FRIEND_READ_WRITE( MachLogPodAttackOperation );

protected:

	virtual bool doStart();
	virtual void doFinish();
	
	virtual bool doIsFinished() const;
	virtual PhysRelativeTime doUpdate();

	virtual void doOutputOperator( ostream& ) const;
	
	virtual bool doBeInterrupted();


private:

	// Operations deliberately revoked
    MachLogPodAttackOperation( const MachLogPodAttackOperation& );
    MachLogPodAttackOperation& operator =( const MachLogPodAttackOperation& );
    bool operator ==( const MachLogPodAttackOperation& );

	void dealWithVoiceMails();
	
	// data members

	MachLogPod*                             pActor_;
    MexPoint3d                              targetPosition_;
    MexPoint2d                  			lastTargetPosition_;
	
};

PER_DECLARE_PERSISTENT( MachLogPodAttackOperation );
/* //////////////////////////////////////////////////////////////// */

class MachLogPodAttackAnimation
: public MachLogAnimation
// cannonical form revoked
{
public:
	
	MachLogPodAttackAnimation( MachLogPod * pActor, const MexPoint3d& targetPosition );

	virtual ~MachLogPodAttackAnimation();
	
	///////////////////////////////

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogPodAttackAnimation );
	PER_FRIEND_READ_WRITE( MachLogPodAttackAnimation );

protected:
	
	virtual PhysRelativeTime doStartAnimation();
	
	virtual void doOutputOperator( ostream& ) const;

private:

	MachLogPodAttackAnimation( const MachLogPodAttackAnimation& );
	MachLogPodAttackAnimation& operator =( const MachLogPodAttackAnimation& );
	bool operator ==( const MachLogPodAttackAnimation& ) const;

	// data members
	
	MachLogPod *                    pActor_;
    MexPoint3d                      targetPosition_;
    
    
};

PER_DECLARE_PERSISTENT( MachLogPodAttackAnimation );

/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef 	_MACHLOG_PODATTACK_HPP  	*/

/* End OPPODATT.HPP ***************************************************/
