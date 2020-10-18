/*
 * C O N S T R O I . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/internal/constroi.hpp"

#include "machlog/construc.hpp"
#include "sim/manager.hpp"

PER_DEFINE_PERSISTENT( MachLogConstructionImpl );

MachLogConstructionImpl::MachLogConstructionImpl()
:   constructedUnits_( 0 ),
    timeConstructedUnitsUpdated_( 0 ),
    visualisedUnitsOnUpdate_( 0 ),
    visualisedUnits_( 0 ),
    pConfigSpace_( NULL ),
    pMarker_( NULL ),
	needRebuild_( false ),
	nReservations_( 0 ),
	constructionId_( -1 ),
	originalRaceConstructionId_( -1 ),
	removedDoorPoint_( false ),
	lastHitTime_( SimManager::instance().currentTime() - MachLogConstruction::VOICE_MAIL_HIT_INTERVAL ), 	// guarantees a voicemail if hit very early on
	isComplete_( false )
{
	buildPoints_.reserve( 24 );
	notRecentlyUsedPadTimes_.reserve( 6 );
    TEST_INVARIANT;
}

MachLogConstructionImpl::~MachLogConstructionImpl()
{
	// note that buildpoints are erased in main constron.cpp destructor		
    TEST_INVARIANT;

}

void MachLogConstructionImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogConstructionImpl& t )
{

    o << "MachLogConstructionImpl " << (void*)&t << " start" << std::endl;
    o << "MachLogConstructionImpl " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogConstructionImpl& actorImpl )
{
    ostr << actorImpl.constructedUnits_;
    ostr << actorImpl.timeConstructedUnitsUpdated_;
    ostr << actorImpl.visualisedUnitsOnUpdate_;
    ostr << actorImpl.visualisedUnits_;
    //PhysConfigSpace2d* pConfigSpace_; - internal config space is recreated elsewhere
    //ostr << actorImpl.entrances_;	 - entrances are recreated elsewhere
	//ostr << actorImpl.pMarker_;
	ostr << actorImpl.constructors_;
	//ostr << actorImpl.machines_;  // machines inserted into buildings as part of machine restore
	ostr << actorImpl.buildPoints_;
	ostr << actorImpl.needRebuild_;
	ostr << actorImpl.nReservations_;
	ostr << actorImpl.constructionId_;
	ostr << actorImpl.originalRaceConstructionId_;
	ostr << actorImpl.isComplete_;
	ostr << actorImpl.lastHitTime_;
	ostr << actorImpl.removedDoorPoint_;
}

void perRead( PerIstream& istr, MachLogConstructionImpl& actorImpl )
{
    istr >> actorImpl.constructedUnits_;
    istr >> actorImpl.timeConstructedUnitsUpdated_;
    istr >> actorImpl.visualisedUnitsOnUpdate_;
    istr >> actorImpl.visualisedUnits_;
    //PhysConfigSpace2d* pConfigSpace_;        
    //istr >> actorImpl.entrances_; - entrances are recreated elsewhere
	//istr >> actorImpl.pMarker_;
	actorImpl.pMarker_ = NULL;
	istr >> actorImpl.constructors_;
	//istr >> actorImpl.machines_;
	istr >> actorImpl.buildPoints_;	
	istr >> actorImpl.needRebuild_;
	istr >> actorImpl.nReservations_;
	istr >> actorImpl.constructionId_;
	istr >> actorImpl.originalRaceConstructionId_;
	istr >> actorImpl.isComplete_;
	istr >> actorImpl.lastHitTime_;
	istr >> actorImpl.removedDoorPoint_;
	
    //Ensure the pulsing animation gets updated
	actorImpl.visualisedUnits_ -= 1.0;
}

/* End CONSTROI.CPP *************************************************/
