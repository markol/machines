 /*
 * MINE . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "stdlib/string.hpp"
#include "ctl/list.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/abox2d.hpp"
#include "mathex/eulerang.hpp"
#include "phys/phys.hpp"
#include "sim/manager.hpp"
#include "world4d/domain.hpp"

#include "machphys/mine.hpp"
#include "machphys/minedata.hpp"
#include "machphys/smeldata.hpp"
#include "machphys/mphydata.hpp"

//#include "machlog/cntrl_pc.hpp"
#include "machlog/dyingent.hpp"
#include "machlog/machlog.hpp"
#include "machlog/messbrok.hpp"
#include "machlog/mine.hpp"
#include "machlog/minesite.hpp"
#include "machlog/network.hpp"
#include "machlog/planet.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/pod.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"
#include "machlog/smelter.hpp"
#include "machlog/vmdata.hpp"
#include "machlog/vmman.hpp"

#ifndef _INLINE
    #include "machlog/mine.ipp"
#endif

PER_DEFINE_PERSISTENT( MachLogMine );
//////////////////////////////////////////////////////////////////////////////////////////

MachLogMine::MachLogMine( MachLogRace* pRace, uint level,
                    const MexPoint3d& location, const MexRadians& angle,
                    const CheckDiscoveredFlag check )
:   MachLogConstruction( pRace, pNewPhysMine( pRace, level, location, angle ), MachLog::MINE,
                         MachPhysData::instance().mineData( level ) ),
	ore_( 0 ),
	lastUpdateTime_( SimManager::instance().currentTime() ),
	pSite_( NULL )
{
	//hp( data().hitPoints() );
	armour( data().armour() );
	MachLogRaces::instance().mines( pRace->race() ).push_back( this );
	MachLogRaces::instance().nMaxOre( pRace->race() )+= MachPhysData::instance().mineData( level ).capacity();

	if( check == IGNORE_DISCOVERED_FLAG )
	{
		//mark this mineral site as ours if it hasn't previously been discovered
		//this will stop locators trying to "discover" it later.
		discoverAndAssignToNearestMineralSite( pRace );
		ASSERT( hasMineralSite(), "discoverAndAssignToNearestMineralSite failed to find valid site in constructor!" );
	}
	else
	{
		assignToNearestMineralSite();
		ASSERT( hasMineralSite(), "assignToNearestMineralSite failed to find valid site in constructor!" );
		// if( not mineralSite().hasBeenDiscovered() )
		// mineralSite().beDiscoveredBy( pRace->race() );
	}

    TEST_INVARIANT;
}

MachLogMine::MachLogMine( MachLogRace* pRace, uint level,
                    const MexPoint3d& location, const MexRadians& angle,
                    const CheckDiscoveredFlag check, UtlId withId )
:   MachLogConstruction( pRace, pNewPhysMine( pRace, level, location, angle ), MachLog::MINE, withId ,
                         MachPhysData::instance().mineData( level ) ),
	ore_( 0 ),
	lastUpdateTime_( SimManager::instance().currentTime() ),
	pSite_( NULL )
{
	//hp( data().hitPoints() );
	armour( data().armour() );
	MachLogRaces::instance().mines( pRace->race() ).push_back( this );
	MachLogRaces::instance().nMaxOre( pRace->race() )+= MachPhysData::instance().mineData( level ).capacity();

    if( check == IGNORE_DISCOVERED_FLAG )
	{
		//mark this mineral site as ours if it hasn't previously been discovered
		//this will stop locators trying to "discover" it later.
		discoverAndAssignToNearestMineralSite( pRace );
		ASSERT( hasMineralSite(), "discoverAndAssignToNearestMineralSite failed to find valid site in constructor!" );
	}
	else
	{
		assignToNearestMineralSite();
		ASSERT( hasMineralSite(), "assignToNearestMineralSite failed to find valid site in constructor!" );
		//if( not mineralSite().hasBeenDiscovered() )
		//	mineralSite().beDiscoveredBy( pRace->race() );
	}

	// override any scenario specification to rebuild a mine - it's meaningless. The location of mineral sites
	// will determine if and when mines are built
	needRebuild( false );

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

MachLogMine::~MachLogMine()
{
	////HAL_STREAM("MachLogMine::DTOR\n" );
	MachLogRaces::instance().nMaxOre( race() ) -= data().capacity();
	MachLogRaces::instance().nOre( race() ) -= ore_;
    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogMine::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

//////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const MachLogMine& t )
{

	t.doOutputOperator( o );

    return o;
}
//////////////////////////////////////////////////////////////////////////////////////////
//static
MachPhysMine* MachLogMine::pNewPhysMine
(
    MachLogRace* pRace, uint level, const MexPoint3d& location, const MexRadians& zAngle
)
{
    //get domain and transform to use
    MexTransform3d localTransform;
    W4dDomain* pDomain =
        MachLogPlanetDomains::pDomainPosition( location, zAngle, &localTransform );

    //Construct the smelter
    return _NEW( MachPhysMine( pDomain, localTransform, level, pRace->race() ) );
}
//////////////////////////////////////////////////////////////////////////////////////////


//virtual
PhysRelativeTime MachLogMine::update( const PhysRelativeTime& maxCPUTime,
                                      MATHEX_SCALAR clearence )
{
	MachLogConstruction::update( maxCPUTime, clearence );

    PhysRelativeTime interval = 5;
    MachPhysConstruction& physMine = physConstruction();

    if( not physMine.isWorking() )
    {
        bool complete = isComplete();
        bool upToDate = updateCompletionVisualisation();

        if( complete and upToDate and  ore_ < data().capacity() and hasMineralSite() )
		{
            physMine.isWorking( true );
			if( MachLogNetwork::instance().isNetworkGame() )
				MachLogNetwork::instance().messageBroker().sendPlayNormalObjectAnimationMessage( id(), true );

			// no extraction bonuses from when it was being built!
			lastUpdateTime_ = SimManager::instance().currentTime();
		}

        if( not upToDate )
            interval = 0.1;
    }
	else
	{
		// The mine IS currently working.
		ASSERT( hasMineralSite(), "Unexpected absence of mineral site even though mine is flagged as working" );

		if( ore_ < data().capacity() )
		{
			//get some ore from the mineral site that I'm on.

			int tempInt = (int)( (float)data().extractionRate() / 60.0 * ( SimManager::instance().currentTime() - lastUpdateTime_ ) );

			// note that lastUpdateTime_ is not updated if we couldn't quite muster at least 1 BMU's worth of extraction
			// - it'll get added on next time we try

			if( tempInt > 0 )
			{
				MachPhys::BuildingMaterialUnits canAttemptToExtract = tempInt;
				if( ore_ + canAttemptToExtract > data().capacity() )
					canAttemptToExtract = data().capacity() - ore_;

				/*
				if( MachLogRaces::instance().nOre( race() ) + canAttemptToExtract > MachLogRaces::instance().nMaxOre( race() ) )
					canAttemptToExtract = MachLogRaces::instance().nMaxOre( race() ) - MachLogRaces::instance().nOre( race() );
				*/

				MachPhys::BuildingMaterialUnits extracted = mineralSite().extractOre( canAttemptToExtract );
				ore_ += extracted;
				MachLogRaces::instance().nOre( race() ) += extracted;

				lastUpdateTime_ = SimManager::instance().currentTime();
			}
		}
		else
		{
			// I was working, but I've now reached my capacity and should stop extracting
			physMine.isWorking( false );
			if( MachLogNetwork::instance().isNetworkGame() )
				MachLogNetwork::instance().messageBroker().sendPlayNormalObjectAnimationMessage( id(), false );

			lastUpdateTime_ = SimManager::instance().currentTime();
		}
	}

	MachActor::update( maxCPUTime, clearence );

    return interval;
}

/* //////////////////////////////////////////////////////////////// */


//virtual
void MachLogMine::doStartExplodingAnimation()
{
	// override any scenario specification to rebuild a mine - it's meaningless. The location of mineral sites
	// will determine if and when mines are built
	needRebuild( false );
}

//////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachLogMine::doEndExplodingAnimation()
{
	// Intentionally Empty
}

//////////////////////////////////////////////////////////////////////////////////////////

MachPhysMine* MachLogMine::pPhysMine()
{
    return (MachPhysMine*)( &physObject() );
}

const MachPhysMine* MachLogMine::pPhysMine() const
{
    return (MachPhysMine*)( &physObject() );
}
//////////////////////////////////////////////////////////////////////////////////////////

//virtual
const MachPhysConstructionData& MachLogMine::constructionData() const
{
	return data();
}

const MachPhysMineData& MachLogMine::data() const
{
	return pPhysMine()->data();
}

MachPhys::BuildingMaterialUnits	MachLogMine::ore() const
{
	return ore_;
}

bool MachLogMine::worthVisiting()
{
	HAL_STREAM("(" << id() << ") MachLogMine::worthVisiting ( ore_ > 0 ) " << (ore_ > 0 ) << " or hasMineralSite " << hasMineralSite() << std::endl );
	return ore_ > 0 or hasMineralSite();
}

// static
MATHEX_SCALAR MachLogMine::sqrMaxMineralDistance( uint hwLevel )
{
	//maximum distance a mine is allowed to be away from the nearest (discovered) mineral site
	return MachPhysData::instance().mineData( hwLevel ).sqrMineralDistance();
}

//////////////////////////////////////////////////////////////////////////////////////////

//static
bool MachLogMine::validMineSite( const MexPoint3d& location, uint hwLevel, const MachLogMine::CheckDiscoveredFlag check )
{
	MATHEX_SCALAR maxMineralDistance = sqrMaxMineralDistance( hwLevel );

    //Find a mineral deposit within (root maxMineralDistance) metres
	//HAL_STREAM("MLMine::validMineSite...number of sites to check " << MachLogPlanet::instance().sites().size() << std::endl );
    bool result = false;
	for( MachLogPlanet::Sites::const_iterator i = MachLogPlanet::instance().sites().begin();
	     i != MachLogPlanet::instance().sites().end(); ++i )
    {
        MachLogMineralSite* pSite = *i;
		if( check == CHECK_DISCOVERED_FLAG )
		{
			if( pSite->hasBeenDiscovered()
				and pSite->amountOfOre() > 0
			    and location.sqrEuclidianDistance( pSite->position() ) < maxMineralDistance )
			{
				result = true;
				break;

			}
		}
		else
		{
			//this is for locations set up at the beginning of the scenario - don't need to test for having been discovered
			if( location.sqrEuclidianDistance( pSite->position() ) < maxMineralDistance )
			{
	       	 	result = true;
				break;
			}
		}
    }

	if ( check != CHECK_DISCOVERED_FLAG)
	{
		//debugging checks for scenario setups

		//first check that we had a valid mineral field in range
		ASSERT (result,"Mine too far from mineral site in initial setup");
		//and if so, check that it was outside the minumum distance allowed for all mine-smelter relationships
		ASSERT( validForAllSmelters( location ),"Mine violated minimum distance from all smelters in initial setup");
	}

	if (result == false) return result;

	//We've got here - it's certainly within range of a discovered mineral site.

	//However, we now need to check for whether or not placing the mine here would violate the
	//rule stipulating a minimum distance from ALL smelter sites (this includes those of other races).

	if (!validForAllSmelters( location ) )
		result = false;
	else
		result = true;


	return result;

}


//static
bool MachLogMine::validForAllSmelters ( const MexPoint3d& location )
{
	for (MachPhys::Race r = MachPhys::RED; r != MachPhys::N_RACES; ++((int&)r))
	{
		for (MachLogRaces::Smelters::const_iterator i = MachLogRaces::instance().smelters( r ).begin();
			i != MachLogRaces::instance().smelters( r ).end(); ++i )
		{
			if (not outsideMinimumMineSmelterRange ( location , (*i) ) )
				return false;
		}
		for (MachLogRaces::Pods::const_iterator i = MachLogRaces::instance().pods( r ).begin();
			i != MachLogRaces::instance().pods( r ).end(); ++i)
		{
			const MachLogPod* pPod = _CONST_CAST( const MachLogPod*, (*i) );
			if (not outsideMinimumMineSmelterRange ( location , pPod ) )
				return false;
		}
	}
	return true;
}

//static
bool MachLogMine::outsideMinimumMineSmelterRange ( const MexPoint3d& location, const MachLogConstruction* pTarget)
{

	//This method is also called from MachLogSmelter::validForAllSmelters(). pTarget is intended to be either a smelter,
	//a pod or a mine, the complement of whichever type passed its location to this function as const MexPoint3d& location.

	PRE( pTarget->objectIsCanSmelt() or pTarget->objectType() == MachLog::MINE );

	static MATHEX_SCALAR sqrMinSmelterDistance = MachPhysData::instance().smelterData(1).sqrMineDistance();		//minimum distance a mine must be away from the nearest
																												//smelter
    ASSERT_INFO( location );
    ASSERT_INFO( pTarget->position() );
    ASSERT_INFO( sqrMinSmelterDistance );

	return location.sqrEuclidianDistance( pTarget->position() ) > sqrMinSmelterDistance;
}

void perWrite( PerOstream& ostr, const MachLogMine& actor )
{
	const MachLogConstruction& base1 = actor;

	ostr << base1;
	ostr << actor.ore_;
	ostr << actor.pSite_;
	ostr << actor.lastUpdateTime_;

	const MexTransform3d& trans = actor.globalTransform();
	PER_WRITE_RAW_OBJECT( ostr, trans );
	const W4dId id = actor.id();
	PER_WRITE_RAW_OBJECT( ostr, id );
	uint level = actor.level();
	PER_WRITE_RAW_OBJECT( ostr, level );
	bool isWorking = actor.pPhysMine()->isWorking();
	PER_WRITE_RAW_OBJECT( ostr, isWorking );
	actor.persistenceWriteStrategy( ostr );
}

void perRead( PerIstream& istr, MachLogMine& actor )
{
	MachLogConstruction& base1 = actor;

	istr >> base1;
	istr >> actor.ore_;
	istr >> actor.pSite_;
	istr >> actor.lastUpdateTime_;

	MexTransform3d trans;
	W4dId id;
	uint level;
	bool isWorking;
	PER_READ_RAW_OBJECT( istr, trans );
	PER_READ_RAW_OBJECT( istr, id );
	PER_READ_RAW_OBJECT( istr, level );
	PER_READ_RAW_OBJECT( istr, isWorking );

	MachPhysMine* pPhysMine = MachLogMine::pNewPhysMine( &actor.logRace(), level, trans.position(), trans.rotationAsEulerAngles().azimuth() );
	actor.setObjectPtr( pPhysMine, trans );
	actor.id( id );
	actor.constructionCreated( pPhysMine );
	MachLogRaces::instance().nConstructions( actor.race() )++;
	pPhysMine->isWorking( isWorking );
	actor.persistenceReadStrategy( istr );
}

MachLogMine::MachLogMine( PerConstructor con )
:	MachLogConstruction( con )
{
}

//virtual
void MachLogMine::doOutputOperator( ostream& o ) const
{
    o << "MachLogMine " << (void*)this << " start" << std::endl;
	o << " Capacity " << data().capacity() << std::endl;
	o << " Ore " << ore_ << std::endl;
	o << " Extraction rate sec-1 " << data().extractionRate() / 60 << " min-1 " << data().extractionRate() << std::endl;
	if( hasMineralSite() )
		o << " Mineral Site " << mineralSite() << std::endl;
	else
		o << " Mineral Site::NULL\n";
    o << "MachLogMine " << (void*)this << " end" << std::endl;

	MachLogConstruction::doOutputOperator( o );
}

bool MachLogMine::hasMineralSite() const
{
	return ( pSite_ != NULL );
}

const MachLogMineralSite& MachLogMine::mineralSite() const
{
	PRE( hasMineralSite() );

	return *pSite_;
}

// private non-const get method
MachLogMineralSite& MachLogMine::mineralSite()
{
	PRE( hasMineralSite() );

	return *pSite_;
}

bool MachLogMine::assignToNearestMineralSite()
{
	HAL_STREAM("(" << id() << ") MachLogMine::assignToNearestMineralSite\n" );
	HAL_INDENT( 2 );
	bool found = false;
	pSite_ = NULL;

	MATHEX_SCALAR maxMineralDistance = sqrMaxMineralDistance( level() );
	HAL_STREAM("maxMineralDistance " << maxMineralDistance << std::endl );

	MATHEX_SCALAR closestSiteDistance = 999999999;
	MATHEX_SCALAR testDistance = 0;

	for( MachLogPlanet::Sites::const_iterator i = MachLogPlanet::instance().sites().begin();
	     i != MachLogPlanet::instance().sites().end(); ++i )
    {
        MachLogMineralSite* pTestSite = *i;
		HAL_STREAM("checking against mineral site " << pTestSite->position() << " position.sqrEuclid " << position().sqrEuclidianDistance( pTestSite->position() ) << " " );
		HAL_STREAM("closestSiteDistance " << closestSiteDistance << std::endl );
		if( pTestSite->hasBeenDiscovered()
			and pTestSite->amountOfOre() > 0
		    and( testDistance = position().sqrEuclidianDistance( pTestSite->position() ) ) < closestSiteDistance
		    and testDistance < maxMineralDistance )
		{
			HAL_STREAM("this one is ok so setting\n" );
			closestSiteDistance = testDistance;
			pSite_ = pTestSite;
			found = true;
			break;
		}
	}

	HAL_INDENT( -2 );
	HAL_STREAM("(" << id() << ") MachLogMine::assignToNearestMineralSite\n" );
	return found;
}

bool MachLogMine::discoverAndAssignToNearestMineralSite( MachLogRace* pRace )
{
	bool found = false;
	pSite_ = NULL;

	MATHEX_SCALAR maxMineralDistance = sqrMaxMineralDistance( level() );

	MATHEX_SCALAR closestSiteDistance = 999999999;
	MATHEX_SCALAR testDistance = 0;

	for( MachLogPlanet::Sites::const_iterator i = MachLogPlanet::instance().sites().begin();
	     i != MachLogPlanet::instance().sites().end(); ++i )
    {
        MachLogMineralSite* pTestSite = *i;
		if( pTestSite->amountOfOre() > 0
		    and( testDistance = position().sqrEuclidianDistance( pTestSite->position() ) ) < closestSiteDistance
		    and testDistance < maxMineralDistance )
		{
			closestSiteDistance = testDistance;
			pSite_ = pTestSite;
			found = true;
			break;
		}
	}

	if( found )
		if( not pSite_->hasBeenDiscovered() )
			pSite_->beDiscoveredBy( pRace->race() );

	return found;
}

void MachLogMine::mineralSiteIsExhausted()
{
	ASSERT( hasMineralSite(), "Was told my mineral site was exhausted when I don't even HAVE a mineral site." );

	// Try to reassign to an alternative mineral site.
	if( assignToNearestMineralSite() )
	{
		// Hoorah! We may wish to put in a nice new voicemail here to inform of the successful reassignment.
		MachLogVoiceMailManager::instance().postNewMail( VID_POD_MINE_SWITCHED_SOURCE, position(), race() );
	}
	else
	{
		// Nope, that failed. Well, looks like we don't have a mineral site any more.
		pSite_ = NULL;

		// Halt working animations
		physConstruction().isWorking( false );
		if( MachLogNetwork::instance().isNetworkGame() )
			MachLogNetwork::instance().messageBroker().sendPlayNormalObjectAnimationMessage( id(), false );

		lastUpdateTime_ = SimManager::instance().currentTime();

		// issue voicemail
		MachLogVoiceMailManager::instance().postNewMail( VID_POD_MINE_INOPERABLE, position() , race() );
	}
}

void MachLogMine::newMineralSiteAvailable()
{
	if( not hasMineralSite() )
	{
		// we may want to see if we could use this newly-discovered site.
		if( assignToNearestMineralSite() )
		{
			// Hoorah! We may wish to put in a nice new voicemail here to inform of the successful reassignment.
			MachLogVoiceMailManager::instance().postNewMail( VID_POD_MINE_SWITCHED_SOURCE, position(), race() );
		}
	}
}

MachPhys::BuildingMaterialUnits MachLogMine::removeOre( MachPhys::BuildingMaterialUnits oreRemoved )
{
	MachPhys::BuildingMaterialUnits actualOreRemoved = std::min( ore_, oreRemoved );

	ore_ -= actualOreRemoved;

	// if I have no ore and no mineral site, I'm as useful as a Southampton FC trophy cabinet.
	// Inform anyone who cares to know that I'm pretty worthless.

	if( not worthVisiting() )
		notifyObservers( W4dSubject::CLIENT_SPECIFIC, MachLog::MINE_CHANGED_OPERATIONAL_STATUS );

	return actualOreRemoved;
}


/* End MINE.CPP **************************************************/
