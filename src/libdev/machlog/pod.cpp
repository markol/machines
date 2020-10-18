/*
 * POD . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machlog/pod.hpp"
#include "machlog/internal/podi.hpp"

#include "stdlib/string.hpp"

#include "ctl/pvector.hpp"
#include "ctl/list.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/abox2d.hpp"
#include "mathex/eulerang.hpp"

#include "phys/phys.hpp"

#include "sim/manager.hpp"

#include "world4d/domain.hpp"

#include "machphys/pod.hpp"
#include "machphys/poddata.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/weapon.hpp"

#include "machlog/machlog.hpp"
#include "machlog/planet.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"
#include "machlog/spacial.hpp"
#include "machlog/weapon.hpp"
#include "machlog/armourer.hpp"
#include "machlog/restree.hpp"
#include "machlog/resitem.hpp"
#include "machlog/network.hpp"
#include "machlog/messbrok.hpp"
#include "machlog/vmman.hpp"

#ifndef _INLINE
    #include "machlog/pod.ipp"
#endif


PER_DEFINE_PERSISTENT( MachLogPod );

#define CB_MachLogPod_DEPIMPL()	\
	PRE( pImpl_ )	\
	CB_DEPIMPL( PhysAbsoluteTime, lastUpdateTime_ ) \
	CB_DEPIMPL( bool, hasIonCannon_ ) \
	CB_DEPIMPL( bool, ionCannonCharged_ ) \
	CB_DEPIMPL( bool, addedBMUStorageToRace_ );


/* ////////////////////////////////////////////// constructor /////////////////////////////////////////////////// */

MachLogPod::MachLogPod( MachLogRace* pRace, uint level,
                    const MexPoint3d& location, const MexRadians& angle, MachPhys::WeaponCombo wc )
:   MachLogConstruction( pRace, pNewPhysPod( pRace, level, location, angle ), MachLog::POD,
                         MachPhysData::instance().podData( level ) ),
  	MachLogCanAttack( this, pPhysPod(), wc ),
	pImpl_( _NEW( MachLogPodImpl() ) )
{

    TEST_INVARIANT;
	armour( data().armour() );
	MachLogRaces::instance().pods( pRace->race() ).push_back( this );

}

/* ////////////////////////////////////////////// constructor /////////////////////////////////////////////////// */

MachLogPod::MachLogPod( MachLogRace* pRace, uint level,
                    const MexPoint3d& location, const MexRadians& angle, MachPhys::WeaponCombo wc, UtlId withId )
:   MachLogConstruction( pRace, pNewPhysPod( pRace, level, location, angle ), MachLog::POD, withId,
                         MachPhysData::instance().podData( level ) ),
  	MachLogCanAttack( this, pPhysPod(), wc ),
	pImpl_( _NEW( MachLogPodImpl() ) )
{
    TEST_INVARIANT;
	armour( data().armour() );
	MachLogRaces::instance().pods( pRace->race() ).push_back( this );

}

/* /////////////////////////////////////////////// destructor /////////////////////////////////////////////////// */

MachLogPod::~MachLogPod()
{
	TEST_INVARIANT;

	_DELETE( pImpl_ );

}
/* ////////////////////////////////////////////////////////////////////////////////////////////////////////////// */

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogPod::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const MachLogPod& t )
{

    o << "MachLogPod " << (void*)&t << " start" << std::endl;
    o << "MachLogPod " << (void*)&t << " end" << std::endl;

    return o;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//static
MachPhysPod* MachLogPod::pNewPhysPod
(
    MachLogRace* pRace, uint level, const MexPoint3d& location, const MexRadians& zAngle
)
{
    //get domain and transform to use
    MexTransform3d localTransform;
    W4dDomain* pDomain =
        MachLogPlanetDomains::pDomainPosition( location, zAngle, &localTransform );

    return _NEW( MachPhysPod( pDomain, localTransform, level, pRace->race() ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
PhysRelativeTime MachLogPod::update( const PhysRelativeTime& alteredMaxCPUTime, MATHEX_SCALAR junk  )
{
	CB_MachLogPod_DEPIMPL();

    bool complete = isComplete();

	PhysRelativeTime interval = 5.0;

    if( not updateCompletionVisualisation() )
		interval = 0.1;
	else if( complete )
	{
		MachLogRaces& races = MachLogRaces::instance();
		MachPhys::Race myRace = race();

		if ( !addedBMUStorageToRace_ )
		{
			// this is basically the first time the update has been called after actual completion of the pod
			// and we must add its BMU storage capacity to the race, as we would a pod.
			addedBMUStorageToRace_ = true;
			races.increaseRaceCapacity( myRace, MachPhysData::instance().podData( level() ).capacity() );
			// call race method to notify all fristd::endly resource carriers of this pod's newly-complete state
			// as they may now wish to make use of it in its capacity as a pod.
			races.newSmelterCompleted( myRace );
		}

		if( not pPhysPod()->isWorking() )
		{
	     	pPhysPod()->isWorking( true );
			if( MachLogNetwork::instance().isNetworkGame() )
				MachLogNetwork::instance().messageBroker().sendPlayNormalObjectAnimationMessage( id(), true );
		}

		if( not hasIonCannon() )
		{
			MachLogResearchItem& mlri = races.researchTree().researchItem( MachLog::POD, 0, 1, MachPhys::T_ION_ORBITAL_CANNON );
			if( mlri.researched( myRace ) )
			{
				ionCannonAcquiredForFirstTime();
			}
		}
		else
		{
			if( ionCannonReady() )
			{
				if( not ionCannonCharged_ )	// i.e. it wasn't charged last update time
					// aha! Ion cannon has come online!
					MachLogVoiceMailManager::instance().postNewMail( VID_POD_ION_CANNON_CHARGED, /*id(),*/ myRace );

				ionCannonCharged_ = true;
			}
			else
				ionCannonCharged_ = false;

			// do we want this only when the status changes? If so, take account of charged->uncharged switch as well.
			// the thing is, this way, we guarantee having incremental charge bars updating, but it's pretty expensive stuff.
			notifyObservers( W4dSubject::CLIENT_SPECIFIC, MachLog::WEAPON_AVAILABILITY_CHANGED );
		}

		PhysAbsoluteTime timeNow = SimManager::instance().currentTime();
		int potentialBMUs = (int)( (float)data().extractionRate() / 60.0 * ( timeNow - lastUpdateTime_ ) );

		if (potentialBMUs > 0)

		{
			//		Clip BMUs added if global race BMU total capacity would be exceeded by full addition.

			MachPhys::BuildingMaterialUnits currentRaceBMUs = races.nBuildingMaterialUnits( myRace );
			MachPhys::BuildingMaterialUnits maxRaceBMUs = races.nMaxBuildingMaterialUnits( myRace );

		  	if( currentRaceBMUs + potentialBMUs > maxRaceBMUs )
				potentialBMUs = std::max( 0, maxRaceBMUs - currentRaceBMUs );

			races.nBuildingMaterialUnits( myRace, currentRaceBMUs + potentialBMUs );

			lastUpdateTime_ = timeNow;
		}
		//						(otherwise the difference between "now" and "the last time I was updated" is allowed
		//							to accumulate.)

	}

	MachActor::update( alteredMaxCPUTime, junk );

    return interval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

MachPhysPod* MachLogPod::pPhysPod()
{
    return (MachPhysPod*)( &physObject() );
}

const MachPhysPod* MachLogPod::pPhysPod() const
{
    return (MachPhysPod*)( &physObject() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
const MachPhysConstructionData& MachLogPod::constructionData() const
{
	return data();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

const MachPhysPodData& MachLogPod::data() const
{
	return pPhysPod()->data();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// static
MexRadians MachLogPod::angleRange()
{
	static MexRadians result_( 2 * Mathex::PI );
	return result_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// static
MATHEX_SCALAR MachLogPod::sinAngleRange()
{
	static MATHEX_SCALAR result_ = sin( MachLogPod::angleRange().asScalar() );
	return result_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogPod::canAttack( const MachActor& other ) const
{
	if( position().sqrEuclidianDistance( other.position() ) == 0 )
		return true;

	return inWeaponRange( other );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogPod::canTurnToAttack( const MachActor& other ) const
{
	return inWeaponRange( other );
//				and orthogonal( coord(), other.coord() )
//					and lineOfSightIsClear( coord(), other.coord() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogPod::inWeaponRange( const MachActor& other ) const
{
	static const MATHEX_SCALAR WEAPON_RANGE = getMaximumWeaponRange() * 0.95;

	if( other.objectIsConstruction() )
		return MachLogSpacialManipulation::withinRangeOfMexAlignedBox2d( other.asConstruction().globalBoundary(), position(), WEAPON_RANGE );

	MexPoint3d myPos = position();
	MexPoint3d targetPos = other.position();
	return myPos.sqrEuclidianDistance( targetPos ) <= WEAPON_RANGE * WEAPON_RANGE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogPod::inAngleRange( const W4dEntity& ) const
{
	//Ion cannons do not have to rotate.
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogPod::hasIonCannon() const
{
	CB_MachLogPod_DEPIMPL();

	return hasIonCannon_;

	/*
	bool result = false;

	if( hasIonCannon_ )
		result = true;
	else
	{
		for( MachLogCanAttack::Weapons::const_iterator i = weapons().begin(); i != weapons().end() ; ++i )
		{
			MachPhys::WeaponType type = ( ( const MachLogWeapon&)(**i) ).physWeapon().type();
			if( type == MachPhys::ION_ORBITAL_CANNON )
			{
				result = true;
				break;
			}
		}

		// CONST_CAST naughtiness to simulate hasIonCannon_ being mutable - spirit of const-correctness is not violated
		// from client's point of view.
		bool& mutableHasIonCannon = _CONST_CAST( bool&, hasIonCannon_ );
		mutableHasIonCannon = result;
	}

	return result;
	*/
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogPod::ionCannonReady() const
{
	return hasIonCannon() and weapons().front()->recharged();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogPod::ionCannonAcquiredForFirstTime()
{
	CB_MachLogPod_DEPIMPL();

	MachPhys::Race podRace = race();
	MachLogVoiceMailManager::instance().postNewMail( VID_POD_ION_CANNON_ACQUIRED, /*id(),*/ podRace );

	MachLogRaces::instance().builtIonCannon( podRace );

	MachLogNetwork& network = MachLogNetwork::instance();

	// ........and whizz the warning round the network if it's a network game.
	if( network.isNetworkGame() )
	{
		if( network.remoteStatus( podRace ) != MachLogNetwork::REMOTE_PROCESS )
		{
			network.messageBroker().sendWeaponInformationMessage( MachLogMessageBroker::ION_CANNON_ONLINE, podRace );
		}
	}
}

void MachLogPod::activateIonCannon()
{
	// To be honsest, this method might more properly be called "addIonCannon" as in fact we are adding an ion
	// cannon to our weapons collection, not somehow "switchiing on" an already-present ion cannon.

	CB_MachLogPod_DEPIMPL();

	MachLogArmourer::arm( this );
	hasIonCannon_ = true;
	pPhysPod()->ionCannonResearched( true );
	notifyObservers( W4dSubject::CLIENT_SPECIFIC, MachLog::WEAPON_AVAILABILITY_CHANGED );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PhysRelativeTime MachLogPod::attack( MachActor* pTarget )
{
	PhysRelativeTime rt = 0;
	for( MachLogCanAttack::Weapons::iterator i = weapons().begin(); i != weapons().end() ; ++i )
	{
		PhysRelativeTime wft = (*i)->fire( pTarget );
		if(wft > rt) rt = wft;
	}
	return rt;
}

PhysRelativeTime MachLogPod::attack( const MexPoint3d& position )
{
	PhysRelativeTime rt = 0;
	for( MachLogCanAttack::Weapons::iterator i = weapons().begin(); i != weapons().end() ; ++i )
	{
		PhysRelativeTime wft = (*i)->fire( position );
		if(wft > rt) rt = wft;
	}
	return rt;
}

//virtual
int MachLogPod::militaryStrength() const
{
	if( not isComplete() )
		return 0;

	int hwLevel = (int)( asConstruction().level() );


	// arbitrariness!
	return ( hp()*hwLevel ) / 30;
}


//	virtual
void MachLogPod::assignToDifferentRace( MachLogRace& newRace )
{
	// have to deal with the change in race storage capacity, PLUS the actual BMUs stolen from inside this pod.
	MachPhys::Race physOldRace = race();
	MachPhys::Race physNewRace = newRace.race();

	MachLogRaces& races = MachLogRaces::instance();

	MachPhys::SizeUnits stolenCapacity = MachPhysData::instance().podData( level() ).capacity();

	MachPhys::BuildingMaterialUnits oldRaceBMUsBefore = races.nBuildingMaterialUnits( physOldRace );
	races.reduceCapacityWithPenalty( physOldRace, stolenCapacity );
	MachPhys::BuildingMaterialUnits oldRaceBMUsAfter = races.nBuildingMaterialUnits( physOldRace );

	MachPhys::BuildingMaterialUnits stolenBMUs = oldRaceBMUsBefore - oldRaceBMUsAfter;

	// it is possible that the race I'm stealing from actually had more BMUs than capacity (from initial
	// "cheaty" period), so only steal as much as this pod can store, at most.
	stolenBMUs = std::min( stolenBMUs, stolenCapacity );

	races.increaseRaceCapacity( physNewRace, stolenCapacity );

	races.smartAddBMUs( physNewRace, stolenBMUs );

	MachLogConstruction::assignToDifferentRace( newRace );

	// treat this as though a new smelter had just been built
	races.newSmelterCompleted( physNewRace );

	// now, if this pod doesn't have an ion cannon but the capturing race has the technology, arm it up.

	if( not hasIonCannon() )
	{
		MachLogResearchItem& mlri = races.researchTree().researchItem( MachLog::POD, 0, 1, MachPhys::T_ION_ORBITAL_CANNON );
		if( mlri.researched( race() ) )
		{
			activateIonCannon();
		}
	}
}



/////////////////////////////////////////////////// persistence /////////////////////////////////////////////////////

void perWrite( PerOstream& ostr, const MachLogPod& actor )
{
	const MachLogConstruction& base1 = actor;
	const MachLogCanAttack& base2 = actor;

	ostr << base1;
	ostr << base2;
	ostr << actor.pImpl_;

	const MexTransform3d& trans = actor.globalTransform();
	PER_WRITE_RAW_OBJECT( ostr, trans );
	const W4dId id = actor.id();
	PER_WRITE_RAW_OBJECT( ostr, id );
	uint level = actor.level();
	PER_WRITE_RAW_OBJECT( ostr, level );
	bool armedWithIonCannon = actor.hasIonCannon();
	PER_WRITE_RAW_OBJECT( ostr, armedWithIonCannon );
	bool isWorking = actor.pPhysPod()->isWorking();
	PER_WRITE_RAW_OBJECT( ostr, isWorking );

	actor.persistenceWriteStrategy( ostr );
}

void perRead( PerIstream& istr, MachLogPod& actor )
{
	MachLogConstruction& base1 = actor;
	MachLogCanAttack& base2 = actor;

	istr >> base1;
	istr >> base2;
	istr >> actor.pImpl_;

	MexTransform3d trans;
	W4dId id;
	uint level;
	bool armedWithIonCannon;
	bool isWorking;
	PER_READ_RAW_OBJECT( istr, trans );
	PER_READ_RAW_OBJECT( istr, id );
	PER_READ_RAW_OBJECT( istr, level );
	PER_READ_RAW_OBJECT( istr, armedWithIonCannon );
	PER_READ_RAW_OBJECT( istr, isWorking );

	MachPhysPod* pPhysPod = MachLogPod::pNewPhysPod( &actor.logRace(), level, trans.position(), trans.rotationAsEulerAngles().azimuth() );
	actor.setObjectPtr( pPhysPod, trans );
	actor.setPhysCanAttack( pPhysPod );
	actor.id( id );

	if( armedWithIonCannon )
	{
		MachLogArmourer::rearm( &actor );
		pPhysPod->ionCannonResearched( true );
	}

	actor.constructionCreated( pPhysPod );
	MachLogRaces::instance().nConstructions( actor.race() )++;
	pPhysPod->isWorking( isWorking );
	actor.persistenceReadStrategy( istr );
}

MachLogPod::MachLogPod( PerConstructor con )
:	MachLogConstruction( con ),
	MachLogCanAttack( con )
{
}

//virtual
void MachLogPod::beHit( const int& damage, MachPhys::WeaponType byType,
						MachActor* pByActor, MexLine3d* pByDirection, MachActor::EchoBeHit echo )
{
	CB_MachLogPod_DEPIMPL();

	if( not isDead() )
	{
		MachLogConstruction::beHit( damage, byType, pByActor, pByDirection, echo );

		// only knock off BMU capacity from race total if we've actually added it for this pod (not done until pod
		// is complete - see update method)
		if( isDead() and addedBMUStorageToRace_ )
		{
			// assume intelligent reassignment of BMUs if we're deconstructing, self-destructing etc.
			// This also allows for smart reallocation if hit by fristd::endly fire, but, hey, what the hell.
			// If we're deliberately aiming at the pod, one could argue that intelligent reallocation
			// is perfectly valid. If accidentally hit - well, this is so rare, no-one will really be bothered.
			// Errs on the side of the player in any case.
			bool destroyedByFriendly = ( pByActor and pByActor->race() == race() );

			if( destroyedByFriendly )
				MachLogRaces::instance().reduceCapacityNoPenalty( race(), MachPhysData::instance().podData( level() ).capacity() );
			else
				MachLogRaces::instance().reduceCapacityWithPenalty( race(), MachPhysData::instance().podData( level() ).capacity() );
		}
	}
}

//virtual
void MachLogPod::beHitWithoutAnimation( int damage, PhysRelativeTime physicalTimeDelay, MachActor* pByActor, MachActor::EchoBeHit echo )
{
	CB_MachLogPod_DEPIMPL();

	if( not isDead() )
	{
		MachLogConstruction::beHitWithoutAnimation( damage, physicalTimeDelay, pByActor, echo );

		// only knock off BMU capacity from race total if we've actually added it for this pod (not done until pod
		// is complete - see update method)
		if( isDead() and addedBMUStorageToRace_ )
		{
			// assume intelligent reassignment of BMUs if we're deconstructing, self-destructing etc.
			// This also allows for smart reallocation if hit by fristd::endly fire, but, hey, what the hell.
			// If we're deliberately aiming at the pod, one could argue that intelligent reallocation
			// is perfectly valid. If accidentally hit - well, this is so rare, no-one will really be bothered.
			// Errs on the side of the player in any case.
			bool destroyedByFriendly = ( pByActor and pByActor->race() == race() );

			if( destroyedByFriendly )
				MachLogRaces::instance().reduceCapacityNoPenalty( race(), MachPhysData::instance().podData( level() ).capacity() );
			else
				MachLogRaces::instance().reduceCapacityWithPenalty( race(), MachPhysData::instance().podData( level() ).capacity() );
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* End POD.CPP **************************************************/
