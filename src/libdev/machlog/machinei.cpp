/*
 * M A C H I N E I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/internal/machinei.hpp"
#include "machlog/apc.hpp"
#include "machlog/races.hpp"
#include "machlog/squad.hpp"
#include "machlog/missilem.hpp"
#include "machlog/entrance.hpp"


#include "sim/manager.hpp"

#include "machphys/healaura.hpp"
#include "machphys/station.hpp"
#include "machphys/stations.hpp"

PER_DEFINE_PERSISTENT( MachLogMachineImpl )

MachLogMachineImpl::MachLogMachineImpl
(
	MachPhys::MachineType t, MachLogMachine::Level hwLevel, MachLogMachine::Level swLevel, MachPhys::Race race
)
:	type_( t ),
	hwLevel_( hwLevel ),
	swLevel_( swLevel ),
	state_( MachLogMachine::NOT_EXPLODING ),
	pLockedEntrance_( NULL ),
	pLockedStation_( NULL ),
	pSquadron_( NULL ),
	pOriginalSquadron_( NULL ),
	lastUpdateTime_( SimManager::instance().currentTime() ),
	pMarker_( NULL ),
	defCon_( MachLogRaces::instance().defCon( race ) ),
	pHealAura_( NULL ),
	healAuraReferences_( 0 ),
	insideAPC_( false ),
	standGround_( false ),
	evading_( false ),
	camouflaged_( false ),
	lastStrengthEstimateTime_ ( SimManager::instance().currentTime() - MachLogMachine::militaryStrengthEstimateInterval() ),	// guarantees an evaluation on first calling
	lastSafestMachineEstimateTime_ ( SimManager::instance().currentTime() - MachLogMachine::militaryStrengthEstimateInterval() ),	// guarantees an evaluation on first calling
	lastSafestMissileEmplacementEstimateTime_ ( SimManager::instance().currentTime() - MachLogMachine::militaryStrengthEstimateInterval() ),	// guarantees an evaluation on first calling
	lastHitTime_( SimManager::instance().currentTime() - MachLogMachine::hitVoiceMailInterval() ), 	// guarantees a voicemail if hit very early on
	nextTrueDefConTime_( SimManager::instance().currentTime() ),	
	pSafestMissileEmplacement_( NULL ),
	pSafestMachine_( NULL ),
	pAPCImInside_( NULL ),
    pInOrOnPadConstruction_( NULL ),
    needToIgnoreEntrancePolygon_( false ),
    needToIgnoreConstructionPolygon_( false ),
    restoreInsideBuilding_( false ),
	usingNVG_( false ),
	isTeleporting_( false ),
	pFirstPersonMachineHandler_( NULL ),
	teleportFinishTime_( SimManager::instance().currentTime() ),
	rampaging_( false )							
{

    TEST_INVARIANT;
}

MachLogMachineImpl::~MachLogMachineImpl()
{
    TEST_INVARIANT;

}

void MachLogMachineImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogMachineImpl& t )
{

    o << "MachLogMachineImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogMachineImpl " << (void*)&t << " end" << std::endl;

    return o;
}


void perWrite( PerOstream& ostr, const MachLogMachineImpl& actorImpl )
{
    ostr << actorImpl.type_;
    ostr << actorImpl.state_;
	ostr << actorImpl.swLevel_;
	ostr << actorImpl.hwLevel_;
	ostr << actorImpl.pSquadron_;
	ostr << actorImpl.pOriginalSquadron_;
	ostr << actorImpl.lastUpdateTime_;
	ostr << actorImpl.defCon_;
//do not persist these - they will be reconstructed when read in
//	ostr << actorImpl.pHealAura_;
//	ostr << actorImpl.healAuraReferences_;
	ostr << actorImpl.insideAPC_;
	ostr << actorImpl.standGround_;
	ostr << actorImpl.evading_;	
	ostr << actorImpl.camouflaged_;				 
	ostr << actorImpl.lastHitTime_;
   	ostr << actorImpl.lastStrengthEstimateTime_;
   	ostr << actorImpl.lastSafestMachineEstimateTime_;
   	ostr << actorImpl.lastSafestMissileEmplacementEstimateTime_;
   	ostr << actorImpl.pSafestMissileEmplacement_;
   	ostr << actorImpl.pSafestMachine_;

	ostr << actorImpl.pInOrOnPadConstruction_;
	ostr << actorImpl.needToIgnoreEntrancePolygon_;
	ostr << actorImpl.needToIgnoreConstructionPolygon_;
	ostr << actorImpl.restoreInsideBuilding_;
	ostr << actorImpl.pTempConstruction_;
	ostr << actorImpl.pAPCImInside_;
	ostr << actorImpl.usingNVG_;
	ostr << actorImpl.isTeleporting_;
	ostr << actorImpl.teleportFinishTime_;
	ostr << actorImpl.rampaging_;

    //Write flag if any locked entrance (always entrance 0 at the moment)
    int hasLockedEntrance = actorImpl.pLockedEntrance_ != NULL;
    PER_WRITE_RAW_OBJECT( ostr, hasLockedEntrance );

    //Write flag and station coords if have a locked station
    int hasLockedStation = actorImpl.pLockedStation_ != NULL;
    PER_WRITE_RAW_OBJECT( ostr, hasLockedStation );
    if( hasLockedStation )
    {
        MexPoint2d stationCoords = actorImpl.pLockedStation_->position();
        PER_WRITE_RAW_OBJECT( ostr, stationCoords );
    }
}

void perRead( PerIstream& istr, MachLogMachineImpl& actorImpl )
{
    istr >> actorImpl.type_;
    istr >> actorImpl.state_;
	istr >> actorImpl.swLevel_;
	istr >> actorImpl.hwLevel_;
	istr >> actorImpl.pSquadron_;
	istr >> actorImpl.pOriginalSquadron_;
	istr >> actorImpl.lastUpdateTime_;
	actorImpl.pMarker_ = NULL;
	istr >> actorImpl.defCon_;
//	istr >> actorImpl.pHealAura_;
//	istr >> actorImpl.healAuraReferences_;
	actorImpl.pHealAura_ = NULL;
	actorImpl.healAuraReferences_ = 0;
	istr >> actorImpl.insideAPC_;
	istr >> actorImpl.standGround_;	
	istr >> actorImpl.evading_;	
	istr >> actorImpl.camouflaged_;				 
	istr >> actorImpl.lastHitTime_;
   	istr >> actorImpl.lastStrengthEstimateTime_;
   	istr >> actorImpl.lastSafestMachineEstimateTime_;
   	istr >> actorImpl.lastSafestMissileEmplacementEstimateTime_;
   	istr >> actorImpl.pSafestMissileEmplacement_;
   	istr >> actorImpl.pSafestMachine_;

	istr >> actorImpl.pInOrOnPadConstruction_;
	istr >> actorImpl.needToIgnoreEntrancePolygon_;
	istr >> actorImpl.needToIgnoreConstructionPolygon_;
	istr >> actorImpl.restoreInsideBuilding_;
	istr >> actorImpl.pTempConstruction_;
    MachLogConstruction* pTempConstruction = actorImpl.pTempConstruction_;

	istr >> actorImpl.pAPCImInside_;
	istr >> actorImpl.usingNVG_;
	istr >> actorImpl.isTeleporting_;
	istr >> actorImpl.teleportFinishTime_;
	istr >> actorImpl.rampaging_;   	

    //restore any locked entrance. has to be doen indirectly via the temporary
    //construction pointer	
    int hasLockedEntrance;
    PER_READ_RAW_OBJECT( istr, hasLockedEntrance );
    if( hasLockedEntrance )
    {
        ASSERT( pTempConstruction != NULL, "" );
        actorImpl.pLockedEntrance_ = &pTempConstruction->entrance( 0 );

        ASSERT( not actorImpl.pLockedEntrance_->locked(), "Entrance already locked" );
        actorImpl.pLockedEntrance_->lock();
    }
    else
        actorImpl.pLockedEntrance_ = NULL;

    //restore any locked station. has to be done indirectly via the temporary
    //construction pointer	
    int hasLockedStation;
    PER_READ_RAW_OBJECT( istr, hasLockedStation );
    if( hasLockedStation )
    {
        //get the coords
        MexPoint2d stationCoords;
        PER_READ_RAW_OBJECT( istr, stationCoords );

        //now find the one with those coords
        ASSERT( pTempConstruction != NULL, "" );
        MachPhysStation* pStation;
        MachPhysConstructionData& consData
            = _CONST_CAST( MachPhysConstructionData&, pTempConstruction->constructionData() );
        bool found = consData.stations().nearStation( stationCoords, &pStation );
        ASSERT( found, "Invalid station coords" );
        ASSERT( not pStation->locked(), "Station already locked" );

        //Now lock it
        pStation->lock( true );
        actorImpl.pLockedStation_ = pStation;
    }
    else
        actorImpl.pLockedStation_ = NULL;

}


MachLogMachineImpl::MachLogMachineImpl( PerConstructor )
:	pLockedStation_( NULL )		//this is something that we must initialise here - can't do in perRead
{
}

MachLogConstruction* MachLogMachineImpl::pTempConstruction() const
{
	return pTempConstruction_;
}

void MachLogMachineImpl::pTempConstruction( MachLogConstruction* pConstruction )
{
	pTempConstruction_ = pConstruction;
}

bool MachLogMachineImpl::needToIgnoreEntrancePolygon() const
{
	return needToIgnoreEntrancePolygon_;
}

void MachLogMachineImpl::needToIgnoreEntrancePolygon( bool newValue )
{
	needToIgnoreEntrancePolygon_ = newValue;
}

bool MachLogMachineImpl::needToIgnoreConstructionPolygon() const
{
	return needToIgnoreConstructionPolygon_;
}

void MachLogMachineImpl::needToIgnoreConstructionPolygon( bool newValue )
{
	needToIgnoreConstructionPolygon_ = newValue;
}

bool MachLogMachineImpl::restoreInsideBuilding() const
{
	return restoreInsideBuilding_;
}
void MachLogMachineImpl::restoreInsideBuilding( bool newValue )
{
	restoreInsideBuilding_ = newValue;
}

MachLogMissileEmplacement* MachLogMachineImpl::pSafestMissileEmplacement() const
{
	return pSafestMissileEmplacement_;
}

MachLogMachine* MachLogMachineImpl::pSafestMachine() const
{
	return pSafestMachine_;
}

MachLogConstruction* MachLogMachineImpl::pInOrOnPadConstruction() const
{
	return pInOrOnPadConstruction_;
}

MachLogEntrance* MachLogMachineImpl::pLockedEntrance() const
{
	return pLockedEntrance_;
}

/* End MACHINEI.CPP *************************************************/
