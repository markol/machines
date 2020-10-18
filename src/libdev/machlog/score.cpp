/*
 * S C O R E . C P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "device/timer.hpp"
#include "machlog/score.hpp"
#include "machlog/internal/scorei.hpp"
#include "machlog/actor.hpp"
#include "machlog/network.hpp"
#include "machlog/messbrok.hpp"

PER_DEFINE_PERSISTENT( MachLogScore );

MachLogScore::MachLogScore( MachPhys::Race race )
:	pImpl_( _NEW( MachLogScoreImpl ) )
{
	HAL_STREAM("MachLogScore::MachLogScore for race " << race << " " << (void*)this << std::endl );
	pImpl_->race_ = race;
	resetNumbers();		
	pImpl_->scoreShouldBeDisplayed_ = true;

    TEST_INVARIANT;
}

MachLogScore::~MachLogScore()
{
	HAL_STREAM("MachLogScore::~MachLogScore " << (void*)this << std::endl );
    TEST_INVARIANT;
	_DELETE( pImpl_ );
}

void MachLogScore::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

int MachLogScore::machinesBuilt() const
{
	return pImpl_->machinesBuilt_;
}

void MachLogScore::machineBuilt()
{
	++( pImpl_->machinesBuilt_ );
}

int MachLogScore::militaryMachinesBuilt() const
{
	return pImpl_->militaryMachinesBuilt_;
}

void MachLogScore::militaryMachineBuilt()
{
	++( pImpl_->militaryMachinesBuilt_ );
}


int MachLogScore::constructionsBuilt() const
{
	return pImpl_->constructionsBuilt_;
}

void MachLogScore::constructionBuilt()
{
	++( pImpl_->constructionsBuilt_ );
}


int MachLogScore::myMachinesDestroyed() const
{
	return pImpl_->myMachinesDestroyed_;
}

void MachLogScore::myMachineDestroyed()
{
	++( pImpl_->myMachinesDestroyed_ );
}

int MachLogScore::otherMachinesDestroyed() const
{
	return pImpl_->otherMachinesDestroyed_;
}

void MachLogScore::otherMachineDestroyed()
{
	++( pImpl_->otherMachinesDestroyed_ );
}

int MachLogScore::raceMachinesDestroyed( MachPhys::Race r ) const
{
	return pImpl_->raceMachinesDestroyed_[r];
}

void MachLogScore::raceMachineDestroyed( MachPhys::Race r )
{
	++( pImpl_->raceMachinesDestroyed_[r] );
}

int MachLogScore::raceMyMachinesDestroyed( MachPhys::Race r ) const
{
	return pImpl_->raceMyMachinesDestroyed_[r];
}

void MachLogScore::raceMyMachineDestroyed( MachPhys::Race r )
{
	++( pImpl_->raceMyMachinesDestroyed_[r] );
}



int MachLogScore::myConstructionsDestroyed() const
{
	return pImpl_->myConstructionsDestroyed_;
}

void MachLogScore::myConstructionDestroyed()
{
	++( pImpl_->myConstructionsDestroyed_ );
}


int MachLogScore::otherConstructionsDestroyed() const
{
	return pImpl_->otherConstructionsDestroyed_;
}

void MachLogScore::otherConstructionDestroyed()
{
	++( pImpl_->otherConstructionsDestroyed_ );
}


int MachLogScore::raceConstructionsDestroyed( MachPhys::Race r ) const
{
	return pImpl_->raceConstructionsDestroyed_[ r ];
}

void MachLogScore::raceConstructionDestroyed( MachPhys::Race r )
{
	++( pImpl_->raceConstructionsDestroyed_[ r ] );
}

int MachLogScore::raceMyConstructionsDestroyed( MachPhys::Race r ) const
{
	return pImpl_->raceMyConstructionsDestroyed_[ r ];
}

void MachLogScore::raceMyConstructionDestroyed( MachPhys::Race r )
{
	++( pImpl_->raceMyConstructionsDestroyed_[ r ] );
}



int MachLogScore::itemsResearched() const
{
	return pImpl_->itemsResearched_;
}

void MachLogScore::itemResearched()
{
	++( pImpl_->itemsResearched_ );
}

int MachLogScore::totalResearchCost() const
{
	return pImpl_->totalResearchCost_;
}

void MachLogScore::totalResearchIncreased( int increase )
{
	pImpl_->totalResearchCost_ += increase;
	changeGrossScore( increase );
}

MachPhys::BuildingMaterialUnits	MachLogScore::BMUsMined() const
{
	return pImpl_->BMUsMined_;
}

void MachLogScore::BMUsMinedIncreased( MachPhys::BuildingMaterialUnits increase	)
{
	pImpl_->BMUsMined_ += increase;
	changeGrossScore( increase );
}

int MachLogScore::grossScore() const
{
	int gross = pImpl_->grossScore_;
	gross -= pImpl_->BMUsMined_;
	gross -= pImpl_->totalResearchCost_;
	gross += ( pImpl_->BMUsMined_ / 4 );
	gross += ( pImpl_->totalResearchCost_ / 2 );
	return gross;
}

void MachLogScore::changeGrossScore( int difference )
{
	//static local as shared between all instances of score classes (don't need a member variable for this)
	static DevTimer lastTransmissionTime;
	pImpl_->grossScore_ += difference;
	MachLogNetwork& network = MachLogNetwork::instance();
	if( network.isNetworkGame() )
		if( network.remoteStatus( pImpl_->race_ ) == MachLogNetwork::LOCAL_PROCESS and lastTransmissionTime.time() > 2 )
		{
			network.messageBroker().sendEchoNewScoreMessage( *pImpl_ );
			lastTransmissionTime.time( 0 );
		}
}


void MachLogScore::actorDestroyedNoCulprit( int value, const MachActor& victimActor )
{
	adjustStatsForDestruction( VICTIM, victimActor );		
}


void MachLogScore::actorDestroyedByRace( int value, const MachActor& victimActor, MachPhys::Race destroyingRace )
{
	// no score adjustment if from fristd::endly fire, fristd::endly deconstruction etc.
	if( destroyingRace != pImpl_->race_ )
		changeGrossScore( -( value / 2) );


	adjustStatsForDestruction( VICTIM, victimActor );
	
	if( victimActor.objectIsMachine() )
			raceMyMachineDestroyed( destroyingRace );
		else if( victimActor.objectIsConstruction() )
			raceMyConstructionDestroyed( destroyingRace );				
}

void MachLogScore::destroyedAnotherRacesActor( int value, const MachActor& victimActor, MachPhys::Race victimRace )
{
	// no score adjustment if from fristd::endly fire, fristd::endly deconstruction etc.
	if( victimRace != pImpl_->race_ )
		changeGrossScore( value );

	adjustStatsForDestruction( KILLER, victimActor );	

	if( victimActor.objectIsMachine() )
			raceMachineDestroyed( victimRace );
		else if( victimActor.objectIsConstruction() )
			raceConstructionDestroyed( victimRace );
}


void MachLogScore::adjustStatsForDestruction( KillerOrVictim killVictFlag, const MachActor& victimActor )
{	
	if( killVictFlag == VICTIM )
	{
		// we have had something destroyed
		if( victimActor.objectIsMachine() )
			myMachineDestroyed();
		else if( victimActor.objectIsConstruction() )
			myConstructionDestroyed();
	}
	else
	{
		// we have destroyed someone else's stuff
		if( victimActor.objectIsMachine() )
			otherMachineDestroyed();
		else if( victimActor.objectIsConstruction() )
			otherConstructionDestroyed();
	}
}


ostream& operator <<( ostream& o, const MachLogScore& t )
{

    o << "MachLogScore " << (void*)&t << " start" << std::endl;
	o << " score " << t.grossScore() << std::endl;
	o << " machinesBuilt_ " << t.machinesBuilt() << std::endl;
	o << " militaryMachinesBuilt_ " << t.militaryMachinesBuilt() << std::endl;
	o << " constructionsBuilt_ " << t.constructionsBuilt() << std::endl;
	o << " myMachinesDestroyed_ " << t.myMachinesDestroyed() << std::endl;
	o << " otherMachinesDestroyed_ " << t.otherMachinesDestroyed() << std::endl;
	o << " myConstructionsDestroyed_ " << t.myConstructionsDestroyed() << std::endl;
	o << " otherConstructionsDestroyed_ " << t.otherConstructionsDestroyed() << std::endl;
	for( MachPhys::Race i = MachPhys::RED; i < MachPhys::N_RACES; ++((int&)i) )
		o << " raceMyMachinesDestroyed_[ " << i << " ] " << t.raceMyMachinesDestroyed( i ) << std::endl;
	for( MachPhys::Race i = MachPhys::RED; i < MachPhys::N_RACES; ++((int&)i) )
		o << " raceMachinesDestroyed_[ " << i << " ] " << t.raceMachinesDestroyed( i ) << std::endl;
	for( MachPhys::Race i = MachPhys::RED; i < MachPhys::N_RACES; ++((int&)i) )
		o << " raceMyConstructionsDestroyed_[ " << i << " ] " << t.raceMyConstructionsDestroyed( i ) << std::endl;
	for( MachPhys::Race i = MachPhys::RED; i < MachPhys::N_RACES; ++((int&)i) )
		o << " raceConstructionsDestroyed_[ " << i << " ] " << t.raceConstructionsDestroyed( i ) << std::endl;

	o << " itemsResearched_ " << t.itemsResearched() << std::endl;
	o << " totalResearchCost_ " << t.totalResearchCost() << std::endl;
	o << " BMUsMined_ " << t.BMUsMined() << std::endl;
    o << "MachLogScore " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogScore& score )
{
	HAL_STREAM("perWrite MachLogScore\n" );
	MachLogScoreImpl& impl = *score.pImpl_;
	PER_WRITE_RAW_OBJECT( ostr, impl );
}

void perRead( PerIstream& istr, MachLogScore& score )
{
	HAL_STREAM("perRead MachLogScore\n" );
	MachLogScoreImpl& impl = *score.pImpl_;
	PER_READ_RAW_OBJECT( istr, impl );
}

MachLogScore::MachLogScore( PerConstructor )
{
}

void MachLogScore::assignScoreFromImpl( const MachLogScoreImpl& newImpl ) const
{
	MachLogScore& me = _CONST_CAST( MachLogScore&, *this );
	*(me.pImpl_) = newImpl;
}

bool MachLogScore::scoreShouldBeDisplayed() const
{
	return pImpl_->scoreShouldBeDisplayed_;
}

void MachLogScore::scoreShouldBeDisplayed( bool newValue )
{
	pImpl_->scoreShouldBeDisplayed_ = newValue;
}

void MachLogScore::resetNumbers()
{
    pImpl_->grossScore_ = 0;
	pImpl_->machinesBuilt_ = 0;
	pImpl_->militaryMachinesBuilt_ = 0;
	pImpl_->constructionsBuilt_ = 0;
	pImpl_->myMachinesDestroyed_ = 0;
	pImpl_->otherMachinesDestroyed_ = 0;
	pImpl_->BMUsMined_ = 0;
	pImpl_->myConstructionsDestroyed_ = 0;
	pImpl_->otherConstructionsDestroyed_ = 0;

	pImpl_->itemsResearched_ = 0;
	pImpl_->totalResearchCost_ = 0;

	for( MachPhys::Race ridx = MachPhys::RED; ridx != MachPhys::N_RACES; ++((int&)ridx) )
	{
		pImpl_->raceMachinesDestroyed_[ ridx ] = 0;
		pImpl_->raceConstructionsDestroyed_[ ridx ] = 0;
		pImpl_->raceMyMachinesDestroyed_[ ridx ] = 0;
		pImpl_->raceMyConstructionsDestroyed_[ ridx ] = 0;
	}
}

void MachLogScore::resetPartialNumbers( MachPhys::Race ridx )
{
	pImpl_->raceMachinesDestroyed_[ ridx ] = 0;
	pImpl_->raceConstructionsDestroyed_[ ridx ] = 0;
	pImpl_->raceMyMachinesDestroyed_[ ridx ] = 0;
	pImpl_->raceMyConstructionsDestroyed_[ ridx ] = 0;
}

/* End SCORE.CPP ****************************************************/
