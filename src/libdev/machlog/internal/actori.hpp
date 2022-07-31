/*
 * A C T O R I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachActorImpl

    Implementation class for MachActor.

*/

#ifndef _MACHLOG_ACTORI_HPP
#define _MACHLOG_ACTORI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "ctl/pvector.hpp"
#include "mathex/point3d.hpp"
#include "machlog/actor.hpp"

class MachActorImpl
{
public:
	MachActorImpl( MachActor* pActor, MachLogRace* pLogRace, MachLog::ObjectType );
	~MachActorImpl();
	PER_MEMBER_PERSISTENT( MachActorImpl );
	PER_FRIEND_READ_WRITE( MachActorImpl );

	void CLASS_INVARIANT;

	friend class MachActor;
private:
	MachLogStrategy *				pStrategy_;
	bool							isInSpecialUpdateActorsList_;
	PhysRelativeTime				maximumTimeInSpecialUpdateActors_;
	PhysAbsoluteTime				startTimeInSpecialUpdateActors_;
	MachActor::Actors	  			actorsThreateningMe_;
	bool 							isIn1stPersonView_;
	MachLog::ObjectType				objectType_;	//Unique object type..Note NOT the same as MachPhys::....Type
	MachPhys::ArmourUnits			armour_;
	MachPhys::HitPointUnits			hp_;
	bool							selfDestructActive_;
	PhysAbsoluteTime				selfDestructTime_;
	MachLogRace*					pRace_;
	MachLog::SelectionState         selectionState_; //Current state
	MachLogRace*					pOriginalRace_;
	PhysAbsoluteTime				changeRaceStartTime_;
	int								localStrength_; 	
	ulong							lastBeHitFrame_;
	PhysAbsoluteTime				nextSOSOpportunity_;
	PhysAbsoluteTime				nextSpecialUpdateOpportunity_;
	PhysAbsoluteTime				nextFirstPersonSOSOpportunity_;	
	PhysAbsoluteTime				nextPredictedPositionRecalculateTime_;
	MexPoint3d						basicTargetOffset_;
	MexPoint3d						cachedTargetOffset_;
	PhysAbsoluteTime				busyUntil_;
	MachPhys::Race					displayMapAndIconRace_;
};

PER_DECLARE_PERSISTENT( MachActorImpl );

#endif

/* End ACTORI.HPP ***************************************************/
