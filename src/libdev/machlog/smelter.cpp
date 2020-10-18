/*
 * S M E L T E R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "stdlib/string.hpp"
#include "ctl/list.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/abox2d.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/cvexpgon.hpp"
#include "phys/phys.hpp"
#include "world4d/domain.hpp"
#include "machphys/smelter.hpp"
#include "machphys/smeldata.hpp"
#include "machphys/mphydata.hpp"
#include "sim/manager.hpp"
#include "machlog/machlog.hpp"
#include "machlog/smelter.hpp"
#include "machlog/mine.hpp"
#include "machlog/planet.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"
#include "machlog/network.hpp"
#include "machlog/messbrok.hpp"

#ifndef _INLINE
    #include "machlog/smelter.ipp"
#endif

PER_DEFINE_PERSISTENT( MachLogSmelter );
//////////////////////////////////////////////////////////////////////////////////////////

MachLogSmelter::MachLogSmelter( MachLogRace* pRace, uint level,
                    const MexPoint3d& location, const MexRadians& angle )
:   MachLogConstruction( pRace, pNewPhysSmelter( pRace, level, location, angle ),
						 MachLog::SMELTER,
                         MachPhysData::instance().smelterData( level ) ),
	inStorage_( 0 ),
	droppedOffOreTime_( 0 ),
	addedBMUStorageToRace_( false )
{
	MachLogRaces::instance().smelters( pRace->race() ).push_back( this );
	armour( data().armour() );

    TEST_INVARIANT;
}

MachLogSmelter::MachLogSmelter( MachLogRace* pRace, uint level,
                    const MexPoint3d& location, const MexRadians& angle, UtlId withId )
:   MachLogConstruction( pRace, pNewPhysSmelter( pRace, level, location, angle ),
						 MachLog::SMELTER,
						 withId,
                         MachPhysData::instance().smelterData( level ) ),
	inStorage_( 0 ),
	droppedOffOreTime_( 0 ),
	addedBMUStorageToRace_( false )
{
	MachLogRaces::instance().smelters( pRace->race() ).push_back( this );
	armour( data().armour() );

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

MachLogSmelter::~MachLogSmelter()
{
	TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogSmelter::CLASS_INVARIANT
{
	INVARIANT( this );
}

//////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const MachLogSmelter& t )
{

    o << "MachLogSmelter " << (void*)&t << " start" << std::endl;
    o << "MachLogSmelter " << (void*)&t << " end" << std::endl;

    return o;
}
//////////////////////////////////////////////////////////////////////////////////////////
//static
MachPhysSmelter* MachLogSmelter::pNewPhysSmelter
(
    MachLogRace* pRace, uint level, const MexPoint3d& location, const MexRadians& zAngle
)
{
    //get domain and transform to use
    MexTransform3d localTransform;
    W4dDomain* pDomain =
        MachLogPlanetDomains::pDomainPosition( location, zAngle, &localTransform );

    //Construct the smelter
    return _NEW( MachPhysSmelter( pDomain, localTransform, level, pRace->race() ) );
}
//////////////////////////////////////////////////////////////////////////////////////////

//virtual
PhysRelativeTime MachLogSmelter::update( const PhysRelativeTime& alteredMaxCPUTime, MATHEX_SCALAR junk )
{
    bool complete = isComplete();

	PhysRelativeTime interval = 5.0;

    if( not updateCompletionVisualisation() )
		interval = 0.1;
	else if( complete )
	{
		if ( !addedBMUStorageToRace_ )
		{
			MachLogRaces& races = MachLogRaces::instance();

			// this is basically the first time the update has been called after actual completion of the smelter
			addedBMUStorageToRace_ = true;
			races.increaseRaceCapacity( race(), MachPhysData::instance().smelterData( level() ).capacity() );
			// call race method to notify all fristd::endly resource carriers of this smelter's newly-complete state
			// as they may now wish to make use of it.
			races.newSmelterCompleted( race() );
		}
		bool isWorking = false;

		if( droppedOffOreTime_ + 10 > SimManager::instance().currentTime() and droppedOffOreTime_ > 0 )
			isWorking = true;

		if( isWorking != pPhysSmelter()->isWorking() )
		{
			pPhysSmelter()->isWorking( isWorking );
			if( MachLogNetwork::instance().isNetworkGame() )
				MachLogNetwork::instance().messageBroker().sendPlayNormalObjectAnimationMessage( id(), isWorking );
		}
	}

	MachActor::update( alteredMaxCPUTime, junk );
    return interval;
}
//////////////////////////////////////////////////////////////////////////////////////////

MachPhysSmelter* MachLogSmelter::pPhysSmelter()
{
    return (MachPhysSmelter*)( &physObject() );
}

const MachPhysSmelter* MachLogSmelter::pPhysSmelter() const
{
    return (MachPhysSmelter*)( &physObject() );
}
//////////////////////////////////////////////////////////////////////////////////////////
//virtual
const MachPhysConstructionData& MachLogSmelter::constructionData() const
{
	return data();
}

const MachPhysSmelterData& MachLogSmelter::data() const
{
	return pPhysSmelter()->data();
}

MachPhys::BuildingMaterialUnits		MachLogSmelter::inStorage() const
{
	return inStorage_;
}
MachPhys::BuildingMaterialUnits&	MachLogSmelter::inStorage()
{
	return inStorage_;
}

void MachLogSmelter::droppedOffOre()
{
	droppedOffOreTime_ = SimManager::instance().currentTime();
}



//static
bool MachLogSmelter::validSmelterSite( const MexPoint3d& location, const MachLogMine::CheckDiscoveredFlag check )
{
	bool result;

	if (!validForAllMines( location ) )
		result = false;
	else
		result = true;


	if ( check == MachLogMine::IGNORE_DISCOVERED_FLAG)
	{
		//debugging checks for scenario setups

		//first check that we had a valid mineral field in range
		ASSERT (result,"Smelter violated minimum distance from all mines in initial setup");
	}


	return result;

}


//static
bool MachLogSmelter::validForAllMines ( const MexPoint3d& location )
{
	for (MachPhys::Race r = MachPhys::RED; r != MachPhys::N_RACES; ++((int&)r) )
	{
		for (MachLogRaces::Mines::iterator i = MachLogRaces::instance().mines( r ).begin();
			i != MachLogRaces::instance().mines( r ).end(); ++i)
		{
			if (!MachLogMine::outsideMinimumMineSmelterRange ( location , *i )) return false;
		}
	}
	return true;
}

//      virtual
void MachLogSmelter::assignToDifferentRace( MachLogRace& newRace )
{
	// have to deal with the change in race storage capacity, PLUS the actual BMUs stolen from inside this smelter.
	MachPhys::Race physOldRace = race();
	MachPhys::Race physNewRace = newRace.race();

	MachLogRaces& races = MachLogRaces::instance();

	MachPhys::SizeUnits stolenCapacity = MachPhysData::instance().smelterData( level() ).capacity();

	MachPhys::BuildingMaterialUnits oldRaceBMUsBefore = races.nBuildingMaterialUnits( physOldRace );
	races.reduceCapacityWithPenalty( physOldRace, stolenCapacity );
	MachPhys::BuildingMaterialUnits oldRaceBMUsAfter = races.nBuildingMaterialUnits( physOldRace );

	MachPhys::BuildingMaterialUnits stolenBMUs = oldRaceBMUsBefore - oldRaceBMUsAfter;

	// it is possible that the race I'm stealing from actually had more BMUs than capacity (from initial
	// "cheaty" period), so only steal as much as this smelter can store, at most.
	stolenBMUs = std::min( stolenBMUs, stolenCapacity );

	races.increaseRaceCapacity( physNewRace, stolenCapacity );

	races.smartAddBMUs( physNewRace, stolenBMUs );

    MachLogConstruction::assignToDifferentRace( newRace );

	// treat this as though a new smelter had just been built
	races.newSmelterCompleted( physNewRace );
}

void perWrite( PerOstream& ostr, const MachLogSmelter& actor )
{
	const MachLogConstruction& base1 = actor;

	ostr << base1;
	ostr << actor.inStorage_;
	ostr << actor.droppedOffOreTime_;
	ostr << actor.addedBMUStorageToRace_;

	const MexTransform3d& trans = actor.globalTransform();
	PER_WRITE_RAW_OBJECT( ostr, trans );
	const W4dId id = actor.id();
	PER_WRITE_RAW_OBJECT( ostr, id );
	uint level = actor.level();
	PER_WRITE_RAW_OBJECT( ostr, level );
	bool isWorking = actor.pPhysSmelter()->isWorking();
	PER_WRITE_RAW_OBJECT( ostr, isWorking );
	actor.persistenceWriteStrategy( ostr );
}

void perRead( PerIstream& istr, MachLogSmelter& actor )
{
	MachLogConstruction& base1 = actor;

	istr >> base1;
	istr >> actor.inStorage_;
	istr >> actor.droppedOffOreTime_;
	istr >> actor.addedBMUStorageToRace_;

	MexTransform3d trans;
	W4dId id;
	uint level;
	bool isWorking;
	PER_READ_RAW_OBJECT( istr, trans );
	PER_READ_RAW_OBJECT( istr, id );
	PER_READ_RAW_OBJECT( istr, level );
	PER_READ_RAW_OBJECT( istr, isWorking );

	MachPhysSmelter* pPhysSmelter = MachLogSmelter::pNewPhysSmelter( &actor.logRace(), level, trans.position(), trans.rotationAsEulerAngles().azimuth() );
	actor.setObjectPtr( pPhysSmelter, trans );
	actor.id( id );
	actor.constructionCreated( pPhysSmelter );
	MachLogRaces::instance().nConstructions( actor.race() )++;

	pPhysSmelter->isWorking( isWorking );
	actor.persistenceReadStrategy( istr );

}

MachLogSmelter::MachLogSmelter( PerConstructor con )
:	MachLogConstruction( con )
{
}

//virtual
void MachLogSmelter::beHit( const int& damage, MachPhys::WeaponType byType,
						MachActor* pByActor, MexLine3d* pByDirection, MachActor::EchoBeHit echo )
{
	if( not isDead() )
	{
		MachLogConstruction::beHit( damage, byType, pByActor, pByDirection, echo );

		// only knock off BMU capacity from race total if we've actually added it for this smelter (not done until smelter
		// is complete - see update method)
		if( isDead() and addedBMUStorageToRace_ )
		{
			// assume intelligent reassignment of BMUs if we're deconstructing, self-destructing etc.
			// This also allows for smart reallocation if hit by fristd::endly fire, but, hey, what the hell.
			// If we're deliberately aiming at the smelter, one could argue that intelligent reallocation
			// is perfectly valid. If accidentally hit - well, this is so rare, no-one will really be bothered.
			// Errs on the side of the player in any case.
			bool destroyedByFriendly = ( pByActor and pByActor->race() == race() );

			if( destroyedByFriendly )
				MachLogRaces::instance().reduceCapacityNoPenalty( race(), MachPhysData::instance().smelterData( level() ).capacity() );
			else
				MachLogRaces::instance().reduceCapacityWithPenalty( race(), MachPhysData::instance().smelterData( level() ).capacity() );
		}
	}
}

//virtual
void MachLogSmelter::beHitWithoutAnimation( int damage, PhysRelativeTime physicalTimeDelay, MachActor* pByActor, MachActor::EchoBeHit echo )
{
	if( not isDead() )
	{
		MachLogConstruction::beHitWithoutAnimation( damage, physicalTimeDelay, pByActor, echo );

		// only knock off BMU capacity from race total if we've actually added it for this smelter (not done until smelter
		// is complete - see update method)
		if( isDead() and addedBMUStorageToRace_ )
		{
			// assume intelligent reassignment of BMUs if we're deconstructing, self-destructing etc.
			// This also allows for smart reallocation if hit by fristd::endly fire, but, hey, what the hell.
			// If we're deliberately aiming at the smelter, one could argue that intelligent reallocation
			// is perfectly valid. If accidentally hit - well, this is so rare, no-one will really be bothered.
			// Errs on the side of the player in any case.
			bool destroyedByFriendly = ( pByActor and pByActor->race() == race() );

			if( destroyedByFriendly )
				MachLogRaces::instance().reduceCapacityNoPenalty( race(), MachPhysData::instance().smelterData( level() ).capacity() );
			else
				MachLogRaces::instance().reduceCapacityWithPenalty( race(), MachPhysData::instance().smelterData( level() ).capacity() );
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////
/* End SMELTER.CPP **************************************************/
