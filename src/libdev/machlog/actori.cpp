/*
 * A C T O R I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions


#include "machlog/internal/actori.hpp"

#include "mathex/point2d.hpp"

#include "sim/manager.hpp"

#include "machlog/race.hpp"
#include "machlog/races.hpp"
#include "machlog/strategy.hpp"


PER_DEFINE_PERSISTENT( MachActorImpl );

MachActorImpl::MachActorImpl( MachActor* pActor, MachLogRace* pLogRace, MachLog::ObjectType ot )
:	pStrategy_( _NEW( MachLogStrategy( pActor ) ) ),
	isInSpecialUpdateActorsList_( false ),
	selfDestructActive_( false ),
	pRace_( pLogRace ),
	pOriginalRace_( pLogRace ),
	selectionState_( MachLog::NOT_SELECTED ),
	changeRaceStartTime_( 0 ),
	nextSOSOpportunity_( 0 ),
	nextFirstPersonSOSOpportunity_( 0 ),
	nextPredictedPositionRecalculateTime_( 0 ),
	nextSpecialUpdateOpportunity_( 0 ),
	lastBeHitFrame_( 0 ),
	objectType_( ot ),
	localStrength_( 0 ),
	selfDestructTime_( 0 ),
	hp_( 1 ),
	startTimeInSpecialUpdateActors_( 0 ),
	maximumTimeInSpecialUpdateActors_( 0 ),
	isIn1stPersonView_( false ),
	displayMapAndIconRace_( pLogRace->race() )
{
	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();

	nextSOSOpportunity_ = timeNow;
	nextSpecialUpdateOpportunity_ = timeNow;
	nextFirstPersonSOSOpportunity_ = timeNow;
	nextPredictedPositionRecalculateTime_ = timeNow;
	busyUntil_ = timeNow;

	actorsThreateningMe_.reserve( 10 );

    TEST_INVARIANT;
}

MachActorImpl::~MachActorImpl()
{
    TEST_INVARIANT;

}

void MachActorImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachActorImpl& t )
{

    o << "MachActorImpl " << (void*)&t << " start" << std::endl;
    o << "MachActorImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachActorImpl& actorImpl )
{
	ostr << actorImpl.isInSpecialUpdateActorsList_;
	//the strategy is written out seperately.
	//ostr << actor.pStrategy_;
	ostr << actorImpl.isIn1stPersonView_;
	ostr << actorImpl.objectType_;
	ostr << actorImpl.armour_;
	ostr << actorImpl.hp_;
	ostr << actorImpl.selfDestructActive_;
	ostr << actorImpl.selfDestructTime_;
	MachPhys::Race race = actorImpl.pRace_->race();
	//PER_WRITE_RAW_OBJECT( ostr, race );
	PER_WRITE_RAW_OBJECT( ostr, (unsigned char&)race );
	race = actorImpl.pOriginalRace_->race();
	//PER_WRITE_RAW_OBJECT( ostr, race );
	PER_WRITE_RAW_OBJECT( ostr, (unsigned char&)race );
	ostr << actorImpl.changeRaceStartTime_;
	ostr << actorImpl.lastBeHitFrame_;
	ostr << actorImpl.localStrength_;
	ostr << actorImpl.actorsThreateningMe_;
	ostr << actorImpl.basicTargetOffset_;
	ostr << actorImpl.cachedTargetOffset_;
	ostr << actorImpl.busyUntil_;
	ostr << actorImpl.displayMapAndIconRace_;
}

void perRead( PerIstream& istr, MachActorImpl& actorImpl )
{
	istr >> actorImpl.isInSpecialUpdateActorsList_;
	//the strategy is read in seperately.
	//istr >> actor.pStrategy_;
	istr >> actorImpl.isIn1stPersonView_;
	istr >> actorImpl.objectType_;
	istr >> actorImpl.armour_;
	istr >> actorImpl.hp_;
	istr >> actorImpl.selfDestructActive_;
	istr >> actorImpl.selfDestructTime_;
	MachPhys::Race currentRace;
	//PER_READ_RAW_OBJECT( istr, (int&)currentRace );
	PER_READ_RAW_OBJECT( istr, (unsigned char&)currentRace );
	MachPhys::Race originalRace;
	//PER_READ_RAW_OBJECT( istr, (int&)originalRace );
	PER_READ_RAW_OBJECT( istr, (unsigned char&)originalRace );
	istr >> actorImpl.changeRaceStartTime_;
	istr >> actorImpl.lastBeHitFrame_;
	istr >> actorImpl.localStrength_;
	istr >> actorImpl.actorsThreateningMe_;
	istr >> actorImpl.basicTargetOffset_;
	istr >> actorImpl.cachedTargetOffset_;
	istr >> actorImpl.busyUntil_;
	istr >> actorImpl.displayMapAndIconRace_;

	actorImpl.pRace_ = &MachLogRaces::instance().race( currentRace );
	actorImpl.pOriginalRace_ = &MachLogRaces::instance().race( originalRace );
	actorImpl.selectionState_ = MachLog::NOT_SELECTED;
	actorImpl.startTimeInSpecialUpdateActors_ = 0;
	actorImpl.maximumTimeInSpecialUpdateActors_ = 0;

	// just set all these values to be the time now
	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();

	actorImpl.nextSOSOpportunity_ = timeNow;
	actorImpl.nextFirstPersonSOSOpportunity_ = timeNow;
	actorImpl.nextPredictedPositionRecalculateTime_ = timeNow;
	actorImpl.nextSpecialUpdateOpportunity_ = timeNow;

}

MachActorImpl::MachActorImpl( PerConstructor )
{
}


/* End ACTORI.CPP ***************************************************/
