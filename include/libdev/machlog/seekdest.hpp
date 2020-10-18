/*
 * S E E K D E S T . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#ifndef _MACHLOG_SEEKDEST_HPP
#define _MACHLOG_SEEKDEST_HPP

#include "mathex/point3d.hpp"
#include "machlog/operatio.hpp"
#include "machlog/machlog.hpp"

class MachLogMachine;

/* //////////////////////////////////////////////////////////////// */
    

class MachLogSeekAndDestroyOperation
: public MachLogOperation
{
public:

	MachLogSeekAndDestroyOperation( MachLogMachine * pActor, 
							MachLog::TargetSystemType targetSystemType = MachLog::TARGET_NORMAL, 
							MachLog::ObjectType objectType = MachLog::AGGRESSOR, 
							const MexPoint3d& searchStartingPoint = MexPoint3d(0,0,0) );

	MachLogSeekAndDestroyOperation( MachLogMachine * pActor, 
							size_t commandId,
							MachLog::TargetSystemType targetSystemType = MachLog::TARGET_NORMAL, 
							MachLog::ObjectType objectType = MachLog::AGGRESSOR, 
							const MexPoint3d& searchStartingPoint = MexPoint3d(0,0,0) );

	~MachLogSeekAndDestroyOperation();
	
	virtual	bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData );

	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogSeekAndDestroyOperation );
	PER_FRIEND_READ_WRITE( MachLogSeekAndDestroyOperation );

protected:

	virtual bool doStart();
	virtual void doFinish();
	
	virtual bool doIsFinished() const;
	
	virtual void doOutputOperator( ostream& ) const;
	
	virtual PhysRelativeTime doUpdate( );

	virtual bool doBeInterrupted();


private:

	void aggressorUpdateWithTarget( MachActor* pTarget );
	void administratorUpdateWithTarget( MachActor* pTarget );
	int issueOrderToSquadronMachine( MachLogMachine* pSquadronMachine, 
							  		 MachActor* pTarget, 
							  		 bool issueAttack,
							  		 int convoyIndex );
									 
	bool canAttackEvenIfInsideBuilding( const MachLogMachine& targetMachine ) const;

	bool							currentlyAttached_;
	MachActor*						pCurrentTarget_;
	MachLogMachine *				pActor_;
	MexPoint3d						searchStartingPoint_;
	MachLog::TargetSystemType 		targetSystemType_;
	MachLog::ObjectType				objectType_;
	size_t							commandId_;
};

PER_DECLARE_PERSISTENT( MachLogSeekAndDestroyOperation );
/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef _MACHLOG_SEEKDEST_HPP	*/

/* End SEEKDEST.HPP *************************************************/