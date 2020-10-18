/*
 * O P T S K A T T . H P P
 * (c) Charybdis Limited, 1996. All Rights Reserved.


		AI squadron module.
		Designed for the generic attack squadrons. This squadron task
		may not be limited to the AI - hence do not make assumptions about the
		controller interface.
 */

#ifndef _MACHLOG_OPTSKATT_HPP
#define _MACHLOG_OPTSKATT_HPP

#include "phys/phys.hpp"
#include "mathex/point3d.hpp"
#include "machlog/machlog.hpp"
#include "machlog/optsk.hpp"

/* //////////////////////////////////////////////////////////////// */

class MachLogSquadron;

// canonical form revoked

class MachLogTaskAttackOperation
: public MachLogTaskOperation
{
public:

	enum State{ ASSEMBLING, RAMPAGING };

	//CTORs are used as follows:
	//This following CTOR will set the squadron going with simple seek and destroy operations
	MachLogTaskAttackOperation( MachLogSquadron *  pActor, PhysRelativeTime initialDelay );

	virtual ~MachLogTaskAttackOperation( );

	void startingPoint( const MexPoint3d& );
	void targetSystemType( MachLog::TargetSystemType );
	void objectType( MachLog::ObjectType, int subType );
	void useNuke();
	void useIonCannon();
	void nMinimumMachines( int );
	void assemblyPoint( int );
	void doCount( int );
	//the intial delay may be overridden by MLActiveSquadronsAction
	void initialDelay( const PhysRelativeTime& );
	//If this is true then the check for a group near the assmebly point will be supressed.
	void supressProximityCheck();
	
	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogTaskAttackOperation );
	PER_FRIEND_READ_WRITE( MachLogTaskAttackOperation );

protected:

	virtual bool doStart();
	virtual void doFinish();
	
	virtual bool doIsFinished() const;
	virtual PhysRelativeTime doUpdate();
	
	virtual void doOutputOperator( ostream& ) const;
	virtual bool doBeInterrupted();


private:

	// Operations deliberately revoked
    MachLogTaskAttackOperation( const MachLogTaskAttackOperation& );
    MachLogTaskAttackOperation& operator =( const MachLogTaskAttackOperation& );
    bool operator ==( const MachLogTaskAttackOperation& );
	
	friend ostream& operator <<( ostream&, const MachLogTaskAttackOperation::State& );
	
	PhysRelativeTime doUpdateWithCommander();
	PhysRelativeTime doUpdateWithoutCommander();
	MachPhys::Race enemyRace();
	
	void tryIonAttack();
	void tryNukeAttack();
	
	// returns true if all machines are within 70m of the assembly point
	bool allRampagingMachinesCloseToPoint( const MexPoint2d& point ) const;
	
	// as allSquadronMachinesCloseToPoint, but also issues move op (to assembly point) 
	// to any machines who are not close enough
	bool checkAllCloseEnoughAndDoAssemble();
	
	MexPoint2d positionOfRampagingMachineNearestTargetPoint( const MexPoint2d& targetPoint ) const;
	// PRE( not pActor_->machines().empty() );
	
	void issueSeekAndDestroysToAllMachinesWithoutTargets();
	
	PhysRelativeTime attackSpecificTarget( const MachActor& targetActor );
	
	void switchToAssembleMode();
	void switchToRampageMode();
	
	bool noRampagingMachines() const;

	bool setCompleteState();

	MachLogSquadron *	pActor_;
    bool 				complete_;
	MATHEX_SCALAR		maxRangeFromPod_;
	bool				setStartingPoint_;
	MexPoint3d			startingPoint_;
	MachLog::TargetSystemType	targetSystemType_;
	bool				specificObjectType_;
	MachLog::ObjectType	objectType_;
	int					subType_;
	bool				initialising_;
	PhysRelativeTime	initialDelay_;
	bool				initialDelayProcessed_;
	PhysAbsoluteTime	startTime_;
	bool				useNuke_;
	bool				useIonCannon_;
	int					nMinimumMachines_;
	int					assemblyPoint_;
	int					doCount_;
	bool				doProximityCheck_;
	State				state_;
};

PER_ENUM_PERSISTENT( MachLogTaskAttackOperation::State );
PER_DECLARE_PERSISTENT( MachLogTaskAttackOperation );

/* //////////////////////////////////////////////////////////////// */

#endif