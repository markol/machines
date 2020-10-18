/*
 * A C T O R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include "machlog/actor.hpp"
#include "machlog/internal/actori.hpp"

#include "utility/bndidgen.hpp"

#include "mathex/abox3d.hpp"
//#include "mathex/poly2d.hpp"	//??

#include "network/netnet.hpp"

#include "world4d/manager.hpp"
#include "world4d/soundman.hpp"

#include "sim/manager.hpp"

#include "phys/motchunk.hpp"

#include "machphys/machine.hpp"
#include "machphys/objdata.hpp"
#include "machphys/weapon.hpp"

#include "machlog/animatio.hpp"
#include "machlog/attack.hpp"
#include "machlog/opmeatta.hpp"

#include "machlog/artefact.hpp"
#include "machlog/administ.hpp"
#include "machlog/aggressr.hpp"
#include "machlog/construc.hpp"
#include "machlog/geolocat.hpp"
#include "machlog/spy.hpp"
#include "machlog/rescarr.hpp"
#include "machlog/apc.hpp"
#include "machlog/factory.hpp"
#include "machlog/hwlab.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/missilem.hpp"
#include "machlog/pod.hpp"
#include "machlog/mine.hpp"
#include "machlog/smelter.hpp"
#include "machlog/garrison.hpp"
#include "machlog/beacon.hpp"
#include "machlog/spacial.hpp"
#include "machlog/technici.hpp"
#include "machlog/squad.hpp"
#include "machlog/debris.hpp"
#include "machlog/lmine.hpp"
// TODO fix dependency issiue lmine & oreholo
#undef _MACHLOG_OREHOLO_HPP
#include "machlog/oreholo.hpp"

#include "machlog/canattac.hpp"
#include "machlog/network.hpp"
#include "machlog/messbrok.hpp"
#include "machlog/machine.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"
#include "machlog/strategy.hpp"
#include "machlog/weapon.hpp"

/* //////////////////////////////////////////////////////////////// */
PER_DEFINE_PERSISTENT_ABSTRACT( MachActor );

MachActor::MachActor
(
	MachLogRace * pRace, W4dEntity * pPhysEntity, MachLog::ObjectType ot
)
: SimActor( pRace, pPhysEntity ),
  pImpl_( _NEW( MachActorImpl( this, pRace, ot ) ) )
{
	CB_DEPIMPL( Actors, actorsThreateningMe_ );
	CB_DEPIMPL( MachPhys::Race, displayMapAndIconRace_ );

	MachLogRaces& races = MachLogRaces::instance();

	displayMapAndIconRace_ = pRace->race();
    actorsThreateningMe_.reserve( 10 );

	ASSERT( pRace != NULL, "Race in machActor Ctor was NULL\n" );
	HAL_STREAM("MachActor::CTOR " );
    //Set the id
	if( MachLogNetwork::instance().isNetworkGame() )
	{
        //Ids are unique between races. Set the range depending on race
        UtlId minId, maxId;
        if( ot == MachLog::ARTEFACT )
        {
            minId = 1001;
            maxId = MachLogRaces::ID_UPPER_BOUND - 1;
        }
        else
        {
            minId = pRace->race() * 250 + 1;
            maxId = minId + 250;
        }

		id( MachLogRaces::idGenerator().nextId( minId, maxId ) );
	}
	else
	{
    	id( MachLogRaces::idGenerator().nextId() );
	}
	HAL_STREAM(" id " << id() << " " << ot << " race " << (int)pRace->race() << " (void*)this " << (void*)this << std::endl );
	NETWORK_ERRORS_STREAM("MachActor::MachActor (" << id() << ") local race, " << pRace->race() << " " << ot << std::endl );

    //Add the actor to the races collection
    races.add( this );
/*	if( ot == MachLog::ORE_HOLOGRAPH or
		ot == MachLog::DEBRIS or
		ot == MachLog::SQUADRON or
		ot == MachLog::LAND_MINE )
			races.remove( this );*/
	if( ot == MachLog::SQUADRON )
		races.remove( this );

	// caches basic centroid offset to prevent the need for wasteful future recalculation
	setupBasicTargetOffset();

	HAL_STREAM("MachActor::CTOR done\n" );
}

MachActor::MachActor
(
	MachLogRace * pRace, W4dEntity * pPhysEntity, MachLog::ObjectType ot, UtlId withId
)
: SimActor( pRace, pPhysEntity ),
  pImpl_( _NEW( MachActorImpl( this, pRace, ot ) ) )
{
	CB_DEPIMPL( Actors, actorsThreateningMe_ );
	CB_DEPIMPL( MachPhys::Race, displayMapAndIconRace_ );

	MachLogRaces& races = MachLogRaces::instance();

	displayMapAndIconRace_ = pRace->race();
    actorsThreateningMe_.reserve( 10 );

	ASSERT( pRace != NULL, "Race in machActor Ctor was NULL\n" );
	HAL_STREAM("MachActor::CTOR " << ot << " withId " << withId << std::endl );
    //Set the id
	ASSERT_INFO( withId );
	#ifndef NDEBUG
		if( races.actorExists( withId ) )
			ASSERT_INFO( races.actor( withId ).objectType() );
	#endif
	ASSERT( not races.actorExists( withId ),"Actor already exists\n" );

	if( withId != 0 )
		MachLogRaces::idGenerator().allocateId( withId );

    id( withId );
	HAL_STREAM(" id " << id() << " " << ot << " race " << (int)pRace->race() << " (void*)this " << (void*)this << std::endl );
	NETWORK_ERRORS_STREAM("MachActor::MachActor (" << id() << ") probably remote race, " << pRace->race() << " " << ot << std::endl );

    //Add the actor to the races collection
	HAL_STREAM("MachActor::CTOR add to races" << std::endl );
    races.add( this );
/*	if( ot == MachLog::ORE_HOLOGRAPH or
		ot == MachLog::DEBRIS or
		ot == MachLog::SQUADRON or
		ot == MachLog::LAND_MINE )
	{
			HAL_STREAM("MachActor::CTOR remove to races" << std::endl );
			races.remove( this );
	}*/
	if( ot == MachLog::SQUADRON )
		races.remove( this );

	// caches basic centroid offset to prevent the need for wasteful future recalculation
	setupBasicTargetOffset();

	HAL_STREAM("MachActor::CTOR done" << std::endl );
	HAL_STREAM("MachActor::CTOR done\n" );
}


MachActor::~MachActor()
{
    //Free the id
	HAL_STREAM("(" << id() << ") MachActor::DTOR\n" );
	NETWORK_ERRORS_STREAM("MachActor::~MachActor (" << id() << ")\n" );
	CB_DEPIMPL( bool, isInSpecialUpdateActorsList_ );

	MachLogRaces& races = MachLogRaces::instance();

	if( isInSpecialUpdateActorsList_ )
		races.removeSpecialUpdateActor( this, MachLog::FORCE_REMOVE );

	CB_DEPIMPL( MachLogStrategy*, pStrategy_ );
	_DELETE( pStrategy_ );
	//Holos and debris etc are removed from the collections at startup
/*	if( objectType_ != MachLog::ORE_HOLOGRAPH and
		objectType_ != MachLog::DEBRIS and
		objectType_ != MachLog::SQUADRON and
		objectType_ != MachLog::LAND_MINE )
		races.remove( this ); */
	if( objectType() != MachLog::SQUADRON )
		races.remove( this );
	if( id() != 0 )
	{
		ASSERT_INFO( id() );
		ASSERT_INFO( objectType() );
		ASSERT( MachLogRaces::idGenerator().isAllocated( id() ),"id for actor has not been allocated and trying to free it\n" );
	    MachLogRaces::idGenerator().freeId( id() );
	}

    _DELETE( pImpl_ );
}


MachPhys::Race MachActor::race() const
{
	CB_DEPIMPL( MachLogRace*, pRace_ );
	return pRace_->race();
}

MachLogRace&
MachActor::logRace()
{
	CB_DEPIMPL( MachLogRace*, pRace_ );
	return *pRace_;
}

const MachLogRace&
MachActor::logRace() const
{
	CB_DEPIMPL( MachLogRace*, pRace_ );
	return *pRace_;
}

const MachLogRace& MachActor::logOriginalRace() const
{
	CB_DEPIMPL( MachLogRace*, pOriginalRace_ );
	return *pOriginalRace_;
}

MachLogRace& MachActor::logOriginalRace()
{
	CB_DEPIMPL( MachLogRace*, pOriginalRace_ );
	return *pOriginalRace_;
}

MachLogStrategy& MachActor::strategy()
{
	CB_DEPIMPL( MachLogStrategy*, pStrategy_ );
	return *pStrategy_;
}

const MachLogStrategy& MachActor::strategy() const
{
	CB_DEPIMPL( MachLogStrategy*, pStrategy_ );
	return *pStrategy_;
}

void MachActor::newOperation( MachLogOperation * pNewOperation )
{
	CB_DEPIMPL( MachLogStrategy*, pStrategy_ );

	// no machine should EVER be being issued an operation if inside an APC.
   	PRE( not objectIsMachine() or not asMachine().insideAPC() );

	pStrategy_->newOperation( pNewOperation, false );

    //Ensure we get an update as soon as possible
    nextUpdateTime( SimManager::instance().currentTime() );
	//we can do better than ensure a callback soon - we can duplicate a callback ourseleves
	MachLogOperation::OperationType type = pNewOperation->operationType();
	switch( type )
	{
		case MachLogOperation::MOVE_TO_OPERATION:
		case MachLogOperation::ATTACK_OPERATION:
		case MachLogOperation::MISSILE_EMPLACEMENT_ATTACK_OPERATION:
		case MachLogOperation::EVADE_OPERATION:
		{
			MachLogNetwork& network = MachLogNetwork::instance();
			if( network.isNetworkGame() )
			{
				if( network.remoteStatus( race() ) == MachLogNetwork::LOCAL_PROCESS and not NetNetwork::instance().imStuffed() )
					update(0,0);
			}
			else
				update(0,0);
		}
	}
}

bool MachActor::isIdle() const
{
	CB_DEPIMPL( MachLogStrategy*, pStrategy_ );

	// an actor should never have a current target if it is idle
	ASSERT_DATA( bool isIdle = pStrategy_->isFinished() );
	ASSERT( implies( isIdle, not( objectIsCanAttack() ) or not( asCanAttack().hasCurrentTarget() ) ), "An actor should never have a current target if it is idle" );

	return pStrategy_->isFinished();
}

/* //////////////////////////////////////////////////////////////// */

//virtual
void MachActor::beHit( const int& damage, MachPhys::WeaponType wt,
						MachActor* pByActor, MexLine3d* pLine, MachActor::EchoBeHit echo  )
{
	CB_DEPIMPL( bool, selfDestructActive_ );

	if( not isDead() )
	{
		updateArmourAndHps( damage );

		MachLogMessageBroker::ActorNowDead actorNowDead = MachLogMessageBroker::ACTOR_NOT_DEAD;
		if( hp() <= 0 )
			actorNowDead = MachLogMessageBroker::ACTOR_DEAD;
		if( echo == ECHO and MachLogNetwork::instance().isNetworkGame() )
			MachLogNetwork::instance().messageBroker().sendBeHitMessage( id(), damage, wt, pByActor, pLine, 0, MachLogMessageBroker::NORMAL_HIT, actorNowDead );
		//Oops I'm dead!
		if( hp() <= 0 )
		{
			strategy().beInterrupted();
			strategy().newOperation( _NEW( MachLogBeDestroyedAnimation( this ) ) );
			isDead( true );

			if( selfDestructActive_ )
			{
				selfDestructActive_ = false;
				checkAndDoSpecialDeaths();
			}

			bool isMachine = objectIsMachine();

			if( isMachine and asMachine().isTeleporting() )
			{
				asMachine().teleportingInterrupted();
			}

			MATHEX_SCALAR junk = 0.0;
			//by calling into the strategy here we know that the actor will blow up now.
			MachActor::update( PhysRelativeTime( 0.001 ), junk );
			if( isMachine or objectIsConstruction() )
			{
				if( pByActor )
					MachLogRaces::instance().actorDestroyedScoreAdjust( race(), objectData().cost(), *this, pByActor->race() );
				else
					MachLogRaces::instance().actorDestroyedScoreAdjust( race(), objectData().cost(), *this );
			}

            //Ensure marker gets removed
            selectionState( MachLog::NOT_SELECTED );

			// you killed me, you rotters! Anyone nearby is now on full alert.
			dispatchCaveeCall();
		}
		//only bother to send health status changed if not dead.
		else
		{
			doHitButStillAliveStuff( pByActor );
		}
	}
	CB_DEPIMPL( ulong, lastBeHitFrame_ );
	lastBeHitFrame_ = W4dManager::instance().frameNumber();
}

// virtual
void MachActor::beHitWithoutAnimation( int damage, PhysRelativeTime physicalTimeDelay, MachActor* pByActor, MachActor::EchoBeHit echo )
{
	if( not isDead() )
	{
	 	updateArmourAndHps( damage );

		MachLogMessageBroker::ActorNowDead actorNowDead = MachLogMessageBroker::ACTOR_NOT_DEAD;

		if( hp() <= 0 )
		{
			actorNowDead = MachLogMessageBroker::ACTOR_DEAD;

			if( objectIsMachine() and asMachine().isTeleporting() )
			{
				asMachine().teleportingInterrupted();
			}

            //We must stop its strategy now, because the strategy cannot be destroyed as part of the actor,
            //since some operations call eg motionsequencer which will already have disappeared in the destructors
            //of MachLogMobile.
			strategy().beInterrupted();

			isDead( true );

            //Ensure marker gets removed
            selectionState( MachLog::NOT_SELECTED );

			// you killed me, you rotters! Anyone nearby is now on full alert.
			dispatchCaveeCall();
		}
		else
		{
			doHitButStillAliveStuff( pByActor );
		}

		if( echo == ECHO and MachLogNetwork::instance().isNetworkGame() )
			MachLogNetwork::instance().messageBroker().sendBeHitMessage( id(), damage, MachPhys::N_WEAPON_TYPES, pByActor, NULL, physicalTimeDelay, MachLogMessageBroker::NO_ANIMATION_HIT, actorNowDead );

	}
	CB_DEPIMPL( ulong, lastBeHitFrame_ );
	lastBeHitFrame_ = W4dManager::instance().frameNumber();
}

void MachActor::doHitButStillAliveStuff( MachActor* pByActor )
{
	CB_DEPIMPL( PhysAbsoluteTime, nextSpecialUpdateOpportunity_ );

	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();

	if( timeNow > nextSpecialUpdateOpportunity_	)
	{
		MachLogRaces::instance().addSpecialUpdateActor( this, 5 );
		nextSpecialUpdateOpportunity_ = timeNow + 5.0;
	}

	notifyObservers( W4dSubject::CLIENT_SPECIFIC, MachLog::HEALTH_STATUS_CHANGED );
	doVisualiseSelectionState();
	// also, if machine that hit me was a first-person machine (and is still alive), we need to
	// try issuing a first-person SOS
	if( pByActor and pByActor->isIn1stPersonView() )
	{
		// don't do it if it's a non-enemy who's hit us.
		MachLogRaces::DispositionToRace disposition = MachLogRaces::instance().dispositionToRace( race(), pByActor->race() );

		if( disposition == MachLogRaces::NEUTRAL or disposition == MachLogRaces::ENEMY )
			dealWithFirstPersonThreat( pByActor );
	}

	// go on full alert if hit by an enemy
	if( pByActor and pByActor->race() != race() and objectIsCanAttack() )
		asCanAttack().setMinimumAlertness( 125 );
}

void MachActor::lastBeHitFrame( ulong newVal )
{
	CB_DEPIMPL( ulong, lastBeHitFrame_ );
	lastBeHitFrame_ = newVal;
}

/* //////////////////////////////////////////////////////////////// */

PhysRelativeTime MachActor::update( const PhysRelativeTime& maxCPUTime, MATHEX_SCALAR )
{

	HAL_STREAM("(" << id() << ") " << objectType() << " MA::update\n" );
	CB_DEPIMPL( bool, selfDestructActive_ );
	CB_DEPIMPL( MachPhys::HitPointUnits, hp_ );
	CB_DEPIMPL( PhysAbsoluteTime, selfDestructTime_ );

	PhysRelativeTime result = 3.0;
	bool threatened = hasThreats();

	if( selfDestructActive_ and not isDead() )
	{
		if( selfDestructTime_ < SimManager::instance().currentTime() )
		{
			beHit( hp() + armour(), MachPhys::N_WEAPON_TYPES, this );
			selfDestructActive_ = false;
		}
	}
	else if( hp_ > 0 and threatened )
	{
		// deal with any threats we may have currently
		dealWithThreats();
	}

	if( not strategy().isFinished() )
	{
		result = strategy().update( maxCPUTime );
	}

	if( threatened )
	{
		// ensure callback time is never more than 1.0 for a threatened machine.
		result = std::min( 1.0, result );
	}


	HAL_STREAM("(" << id() << ") " << objectType() << " MA::update Exiting\n" );
	return result;

}

/* //////////////////////////////////////////////////////////////// */

PhysRelativeTime MachActor::startExplodingAnimation()
{
    // Initiate the exploding animation
	PhysRelativeTime timeLeft = beDestroyed();
	doStartExplodingAnimation();
	//isDead has moved from endExplodingAnimation()
	//MachLogDyingEntity is created within beDestroyed and maintains pointer
	//to physical object
	isDead( true );

	return timeLeft;
}

void MachActor::endExplodingAnimation()
{
	doEndExplodingAnimation();
}

/* //////////////////////////////////////////////////////////////// */

void MachActor::doOutputOperator( ostream& o ) const
{
	o << "(" << id() << ") [" << objectType() << "]   position" << position() << std::endl;
	o << " hp " << hp() << " armour " << armour() << std::endl;
	o << "My military strength: " << militaryStrength() << " Local strength estimate: " << localStrengthValue() << std::endl;
	if( hasThreats() )
		o << "-- I have threats --" << std::endl;

	if( objectIsCanAttack() )
	{
		asCanAttack().doOutputOperator( o );
	}
	o << strategy() << std::endl;

}

/* //////////////////////////////////////////////////////////////// */

bool MachActor::isIn1stPersonView() const
{
	CB_DEPIMPL( bool, isIn1stPersonView_ );
	return isIn1stPersonView_;
}

void MachActor::isIn1stPersonView( bool isIn1st )
{
	CB_DEPIMPL( bool, isIn1stPersonView_ );
	isIn1stPersonView_ = isIn1st;
}

ostream& operator<<(ostream& o, const MachActor& t)
{
	t.doOutputOperator( o );
	return o;
}

void MachActor::objectType( const MachLog::ObjectType& ot )
{
	HAL_STREAM("(" << id() << ") Type " << ot << std::endl );
	CB_DEPIMPL( MachLog::ObjectType, objectType_ );
	objectType_ = ot;
}

MachPhys::ArmourUnits MachActor::armour( void ) const
{
	CB_DEPIMPL( MachPhys::ArmourUnits, armour_ );
	return armour_;
}

void MachActor::armour( const MachPhys::ArmourUnits& newArmour )
{
	CB_DEPIMPL( MachPhys::ArmourUnits, armour_ );
	armour_ = newArmour;
}

MachPhys::HitPointUnits MachActor::hp( void ) const
{
	CB_DEPIMPL( MachPhys::HitPointUnits, hp_ );
	return hp_;
}

void MachActor::hp( const MachPhys::HitPointUnits& newHp )
{
	CB_DEPIMPL( MachPhys::HitPointUnits, hp_ );
	if( newHp < 0 )
		hp_ = 0;
	else
		hp_ = newHp;
}

MATHEX_SCALAR MachActor::hpRatio() const
{
	return (MATHEX_SCALAR)hp()/(MATHEX_SCALAR)maximumhp();
}

void MachActor::selfDestruct( const PhysRelativeTime& whenRelative )
{
	CB_DEPIMPL( bool, selfDestructActive_ );
	CB_DEPIMPL( PhysAbsoluteTime, selfDestructTime_ );
	if( whenRelative == 0 )
		selfDestructActive_ = false;
	else
	{
		selfDestructTime_ = SimManager::instance().currentTime() + whenRelative;
		selfDestructActive_ = true;
		W4dEntity& physObj = physObject();
		if(objectIsMachine())
			W4dSoundManager::instance().play(&physObj, SID_MACHINE_SELF_DESTRUCT, selfDestructTime_, 1);
		else if(objectIsConstruction())
			W4dSoundManager::instance().play(&physObj, SID_CONSTRUCTION_SELF_DESTRUCT, selfDestructTime_, 1);
	}
}

PhysRelativeTime MachActor::selfDestructTime() const
{
	CB_DEPIMPL( PhysAbsoluteTime, selfDestructTime_ );
	return selfDestructTime_ - SimManager::instance().currentTime();
}

bool MachActor::selfDestruct() const
{
	CB_DEPIMPL( bool, selfDestructActive_ );
	return selfDestructActive_;
}


MachLogAdministrator& MachActor::asAdministrator()
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::ADMINISTRATOR,"Bad _STATIC_CAST type in MachActor::asAdministrator\n" );
	return *(_STATIC_CAST( MachLogAdministrator*,this ) );
}

const MachLogAdministrator& MachActor::asAdministrator() const
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::ADMINISTRATOR,"Bad _STATIC_CAST type in MachActor::asAdministrator\n" );
	return *(_STATIC_CAST( const MachLogAdministrator*,this ) );
}

MachLogAggressor& MachActor::asAggressor()
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::AGGRESSOR ,"Bad _STATIC_CAST type in MachActor::asAggressor\n" );
	return *(_STATIC_CAST( MachLogAggressor*,this ) );
}

const MachLogAggressor& MachActor::asAggressor() const
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::AGGRESSOR ,"Bad _STATIC_CAST type in MachActor::asAggressor\n" );
	return *(_STATIC_CAST( const MachLogAggressor*,this ) );
}

MachLogConstructor& MachActor::asConstructor()
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::CONSTRUCTOR ,"Bad _STATIC_CAST type in MachActor::asConstructor\n" );
	return *(_STATIC_CAST( MachLogConstructor*,this ) );
}

const MachLogConstructor& MachActor::asConstructor() const
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::CONSTRUCTOR ,"Bad _STATIC_CAST type in MachActor::asConstructor\n" );
	return *(_STATIC_CAST( const MachLogConstructor*,this ) );
}

MachLogGeoLocator& MachActor::asGeoLocator()
{
	ASSERT_INFO(objectType() );
	ASSERT(objectType() == MachLog::GEO_LOCATOR ,"Bad _STATIC_CAST type in MachActor::asGeoLocator\n" );
	return *(_STATIC_CAST( MachLogGeoLocator*,this ) );
}

const MachLogGeoLocator& MachActor::asGeoLocator() const
{
	ASSERT_INFO(objectType() );
	ASSERT(objectType() == MachLog::GEO_LOCATOR ,"Bad _STATIC_CAST type in MachActor::asGeoLocator\n" );
	return *(_STATIC_CAST( const MachLogGeoLocator*,this ) );
}

MachLogSpyLocator& MachActor::asSpyLocator()
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::SPY_LOCATOR ,"Bad _STATIC_CAST type in MachActor::asSpyLocator\n" );
	return *(_STATIC_CAST( MachLogSpyLocator*,this ) );
}

const MachLogSpyLocator& MachActor::asSpyLocator() const
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::SPY_LOCATOR ,"Bad _STATIC_CAST type in MachActor::asSpyLocator\n" );
	return *(_STATIC_CAST( const MachLogSpyLocator*,this ) );
}

MachLogTechnician& MachActor::asTechnician()
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::TECHNICIAN ,"Bad _STATIC_CAST type in MachActor::asTechnician\n" );
	return *(_STATIC_CAST( MachLogTechnician*,this ) );
}

const MachLogTechnician& MachActor::asTechnician() const
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::TECHNICIAN ,"Bad _STATIC_CAST type in MachActor::asTechnician\n" );
	return *(_STATIC_CAST( const MachLogTechnician*,this ) );
}

MachLogResourceCarrier& MachActor::asResourceCarrier()
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::RESOURCE_CARRIER ,"Bad _STATIC_CAST type in MachActor::asResourceCarrier\n" );
	return *(_STATIC_CAST( MachLogResourceCarrier*,this ) );
}

const MachLogResourceCarrier& MachActor::asResourceCarrier() const
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::RESOURCE_CARRIER ,"Bad _STATIC_CAST type in MachActor::asResourceCarrier\n" );
	return *(_STATIC_CAST( const MachLogResourceCarrier*,this ) );
}

MachLogAPC& MachActor::asAPC()
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::APC ,"Bad _STATIC_CAST type in MachActor::asAPC\n" );
	return *(_STATIC_CAST( MachLogAPC*,this ) );
}

const MachLogAPC& MachActor::asAPC() const
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::APC ,"Bad _STATIC_CAST type in MachActor::asAPC\n" );
	return *(_STATIC_CAST( const MachLogAPC*,this ) );
}

MachLogMobile& MachActor::asMobile()
{
	ASSERT_INFO( objectType() );
	ASSERT( objectIsMachine() ,"Bad _STATIC_CAST type in MachActor::asMobile\n" );
	return *(_STATIC_CAST( MachLogMobile*,this ) );
}

const MachLogMobile& MachActor::asMobile() const
{
	ASSERT_INFO( objectType() );
	ASSERT( objectIsMachine() ,"Bad _STATIC_CAST type in MachActor::asMobile\n" );
	return *(_STATIC_CAST( const MachLogMobile*,this ) );
}

MachLogCanAttack& MachActor::asCanAttack()
{
	ASSERT_INFO( objectType() );
	ASSERT( objectIsCanAttack()
			,"Bad _STATIC_CAST type in MachActor::asCanAttack\n" );
	switch( objectType() )
	{
		case MachLog::AGGRESSOR:
			return *(MachLogCanAttack*)(_STATIC_CAST( MachLogAggressor*,this ) );
		case MachLog::ADMINISTRATOR:
			return *(MachLogCanAttack*)(_STATIC_CAST( MachLogAdministrator*,this ) );
		case MachLog::MISSILE_EMPLACEMENT:
			return *(MachLogCanAttack*)(_STATIC_CAST( MachLogMissileEmplacement*,this ) );
		default:
			return *(MachLogCanAttack*)(_STATIC_CAST( MachLogPod*,this ) );
	}


}

const MachLogCanAttack& MachActor::asCanAttack() const
{
	ASSERT_INFO( objectType() );
	ASSERT( objectIsCanAttack()
			,"Bad _STATIC_CAST type in MachActor::asCanAttack\n" );
	switch( objectType() )
	{
		case MachLog::AGGRESSOR:
			//return *(MachLogCanAttack*)(_STATIC_CAST( MachLogAggressor*,this ) );
			return *(MachLogCanAttack*)(_STATIC_CAST( const MachLogAggressor*,this ) );
		case MachLog::ADMINISTRATOR:
			//return *(MachLogCanAttack*)(_STATIC_CAST( MachLogAdministrator*,this ) );
			return *(MachLogCanAttack*)(_STATIC_CAST( const MachLogAdministrator*,this ) );
		case MachLog::MISSILE_EMPLACEMENT:
			//return *(MachLogCanAttack*)(_STATIC_CAST( MachLogMissileEmplacement*,this ) );
			return *(MachLogCanAttack*)(_STATIC_CAST( const MachLogMissileEmplacement*,this ) );
		default:
			//return *(MachLogCanAttack*)(_STATIC_CAST( MachLogPod*,this ) );
			return *(MachLogCanAttack*)(_STATIC_CAST( const MachLogPod*,this ) );
	}
}

MachLogMachine& MachActor::asMachine()
{
	ASSERT_INFO( objectType() );
	ASSERT( objectIsMachine(), "Bad _STATIC_CAST type in MachActor::asMachine\n" );
	return *(_STATIC_CAST( MachLogMachine*,this ) );
}

const MachLogMachine& MachActor::asMachine() const
{
	ASSERT_INFO( objectType() );
	ASSERT( objectIsMachine(), "Bad _STATIC_CAST type in MachActor::asMachine\n" );
	return *(_STATIC_CAST( const MachLogMachine*,this ) );
}

MachLogConstruction& MachActor::asConstruction()
{
	ASSERT_INFO( objectType() );
	ASSERT( objectIsConstruction(), "Bad _STATIC_CAST type in MachActor::asConstruction\n" );
	return *(_STATIC_CAST( MachLogConstruction*,this ) );
}

const MachLogConstruction& MachActor::asConstruction() const
{
	ASSERT_INFO( objectType() );
	ASSERT( objectIsConstruction(), "Bad _STATIC_CAST type in MachActor::asConstruction\n" );
	return *(_STATIC_CAST( const MachLogConstruction*,this ) );
}

MachLogMine& MachActor::asMine()
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::MINE, "Bad _STATIC_CAST type in MachActor::asMine\n" );
	return *(_STATIC_CAST( MachLogMine*,this ) );
}

const MachLogMine& MachActor::asMine() const
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::MINE, "Bad _STATIC_CAST type in MachActor::asMine\n" );
	return *(_STATIC_CAST( const MachLogMine*,this ) );
}

MachLogSmelter& MachActor::asSmelter()
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::SMELTER, "Bad _STATIC_CAST type in MachActor::asSmelter\n" );
	return *(_STATIC_CAST( MachLogSmelter*,this ) );
}

const MachLogSmelter& MachActor::asSmelter() const
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::SMELTER, "Bad _STATIC_CAST type in MachActor::asSmelter\n" );
	return *(_STATIC_CAST( const MachLogSmelter*,this ) );
}

MachLogFactory& MachActor::asFactory()
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::FACTORY, "Bad _STATIC_CAST type in MachActor::asFactory\n" );
	return *(_STATIC_CAST( MachLogFactory*,this ) );
}

const MachLogFactory& MachActor::asFactory() const
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::FACTORY, "Bad _STATIC_CAST type in MachActor::asFactory\n" );
	return *(_STATIC_CAST( const MachLogFactory*,this ) );
}

MachLogGarrison& MachActor::asGarrison()
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::GARRISON, "Bad _STATIC_CAST type in MachActor::asGarrison\n" );
	return *(_STATIC_CAST( MachLogGarrison*,this ) );
}

const MachLogGarrison& MachActor::asGarrison() const
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::GARRISON, "Bad _STATIC_CAST type in MachActor::asGarrison\n" );
	return *(_STATIC_CAST( const MachLogGarrison*,this ) );
}

bool MachActor::objectIsMachine() const
{
//	//HAL_STREAM("MActor::object is machine...(void*)this " << (void*)this << std::endl );
//	//HAL_STREAM("(" << id() << ") MActor::objectIsMachine " << objectType() << "\n" );
	MachLog::ObjectType type = objectType();
	if( type == MachLog::ADMINISTRATOR or
		type == MachLog::AGGRESSOR or
		type == MachLog::APC or
		type == MachLog::CONSTRUCTOR or
		type == MachLog::GEO_LOCATOR or
		type == MachLog::RESOURCE_CARRIER or
		type == MachLog::SPY_LOCATOR or
		type == MachLog::TECHNICIAN )
			return true;

	return false;
}

bool MachActor::objectIsConstruction() const
{
	MachLog::ObjectType type = objectType();
	if( type == MachLog::BEACON or
		type == MachLog::FACTORY or
		type == MachLog::GARRISON or
		type == MachLog::HARDWARE_LAB or
		type == MachLog::MINE or
		type == MachLog::MISSILE_EMPLACEMENT or
		type == MachLog::POD or
		type == MachLog::SMELTER or
		type == MachLog::SOFTWARE_LAB )
			return true;

	return false;
}

bool MachActor::objectIsILF() const
{
	ASSERT( false, "Ooops, call to objectIsILF\n" );
	return false;
}

bool MachActor::objectIsCanAttack() const
{
//	//HAL_STREAM("MActor::object is Can Attack...(void*)this " << (void*)this << std::endl );
//	//HAL_STREAM("(" << id() << ") MActor::objectIsCanAttack " << objectType() << "\n" );
	MachLog::ObjectType type = objectType();
	if( type == MachLog::ADMINISTRATOR or
		type == MachLog::AGGRESSOR or
		type == MachLog::MISSILE_EMPLACEMENT or
		type == MachLog::POD )
			return true;

	return false;
}

bool MachActor::objectIsDebris() const
{
	return objectType() == MachLog::DEBRIS;
}
bool MachActor::objectIsOreHolograph() const
{
	return objectType() == MachLog::ORE_HOLOGRAPH;
}
bool MachActor::objectIsLandMine() const
{
	return objectType() == MachLog::LAND_MINE;
}

bool MachActor::objectIsMissileEmplacement() const
{
	return objectType() == MachLog::MISSILE_EMPLACEMENT;
}

bool MachActor::objectIsCanSmelt() const
{
	return objectType() == MachLog::SMELTER
		or objectType() == MachLog::POD;
}

MachLogArtefact& MachActor::asArtefact()
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::ARTEFACT, "Bad _STATIC_CAST type in MachActor::asArtefact" );
	return *(_STATIC_CAST( MachLogArtefact*,this ) );
}

const MachLogArtefact& MachActor::asArtefact() const
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::ARTEFACT, "Bad _STATIC_CAST type in MachActor::asArtefact" );
	return *(_STATIC_CAST( const MachLogArtefact*,this ) );
}

MachLogSquadron& MachActor::asSquadron()
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::SQUADRON, "Bad _STATIC_CAST type in MachActor::asSquadron" );
	return *(_STATIC_CAST( MachLogSquadron*,this ) );
}

const MachLogSquadron& MachActor::asSquadron() const
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::SQUADRON, "Bad _STATIC_CAST type in MachActor::asSquadron" );
	return *(_STATIC_CAST( const MachLogSquadron*,this ) );
}

bool MachActor::objectIsArtefact() const
{
	return objectType() == MachLog::ARTEFACT;
}

void MachActor::selectionState( MachLog::SelectionState nextState )
{
	CB_DEPIMPL( MachLog::SelectionState, selectionState_ );
    //No need to do anything unless changing
    if( nextState != selectionState_ )
    {
        //cache the new state
        selectionState_ = nextState;
        //Implement the visualisation
        doVisualiseSelectionState();
    }
}
MachLog::SelectionState MachActor::selectionState() const
{
	CB_DEPIMPL( MachLog::SelectionState, selectionState_ );
    return selectionState_;
}

MachLogHardwareLab& MachActor::asHardwareLab()
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::HARDWARE_LAB, "Bad _STATIC_CAST type in MachActor::asHardwareLab\n" );
	return *(_STATIC_CAST( MachLogHardwareLab*,this ) );
}

const MachLogHardwareLab& MachActor::asHardwareLab() const
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::HARDWARE_LAB, "Bad _STATIC_CAST type in MachActor::asHardwareLab\n" );
	return *(_STATIC_CAST( const MachLogHardwareLab*,this ) );
}

MachLogSoftwareLab& MachActor::asSoftwareLab()
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::SOFTWARE_LAB, "Bad _STATIC_CAST type in MachActor::asSoftwareLab\n" );
	//return *(_STATIC_CAST( MachLogSoftwareLab*,this ) );
	return *(_REINTERPRET_CAST( MachLogSoftwareLab*,this ) );
}

const MachLogSoftwareLab& MachActor::asSoftwareLab() const
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::SOFTWARE_LAB, "Bad _STATIC_CAST type in MachActor::asSoftwareLab\n" );
	//return *(_STATIC_CAST( const MachLogSoftwareLab*,this ) );
	return *(_REINTERPRET_CAST( const MachLogSoftwareLab*,this ) );
}

MachLogMissileEmplacement& MachActor::asMissileEmplacement()
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::MISSILE_EMPLACEMENT, "Bad _STATIC_CAST type in MachActor::asMissileEmplacement\n" );
	return *(_STATIC_CAST( MachLogMissileEmplacement*,this ) );
}

const MachLogMissileEmplacement& MachActor::asMissileEmplacement() const
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::MISSILE_EMPLACEMENT, "Bad _STATIC_CAST type in MachActor::asMissileEmplacement\n" );
	return *(_STATIC_CAST( const MachLogMissileEmplacement*,this ) );
}

MachLogPod& MachActor::asPod()
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::POD, "Bad _STATIC_CAST type in MachActor::asPod\n" );
	return *(_STATIC_CAST( MachLogPod*,this ) );
}

const MachLogPod& MachActor::asPod() const
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::POD, "Bad _STATIC_CAST type in MachActor::asPod\n" );
	return *(_STATIC_CAST( const MachLogPod*,this ) );
}

MachLogBeacon& MachActor::asBeacon()
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::BEACON, "Bad _STATIC_CAST type in MachActor::asBeacon\n" );
	return *(_STATIC_CAST( MachLogBeacon*,this ) );
}

const MachLogBeacon& MachActor::asBeacon() const
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::BEACON, "Bad _STATIC_CAST type in MachActor::asBeacon\n" );
	return *(_STATIC_CAST( const MachLogBeacon*,this ) );
}

MachLogDebris& MachActor::asDebris()
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::DEBRIS, "Bad _STATIC_CAST type in MachActor::asDebris\n" );
	return *(_STATIC_CAST( MachLogDebris*,this ) );
}
const MachLogDebris& MachActor::asDebris() const
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::DEBRIS, "Bad _STATIC_CAST type in MachActor::asDebris\n" );
	return *(_STATIC_CAST( const MachLogDebris*,this ) );
}

MachLogOreHolograph& MachActor::asOreHolograph()
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::ORE_HOLOGRAPH, "Bad _STATIC_CAST type in MachActor::asOreHolograph\n" );
	return *(_STATIC_CAST( MachLogOreHolograph*,this ) );
}
const MachLogOreHolograph& MachActor::asOreHolograph() const
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::ORE_HOLOGRAPH, "Bad _STATIC_CAST type in MachActor::asOreHolograph\n" );
	return *(_STATIC_CAST( const MachLogOreHolograph*,this ) );
}

MachLogLandMine& MachActor::asLandMine()
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::LAND_MINE, "Bad _STATIC_CAST type in MachActor::asLandMine\n" );
	return *(_STATIC_CAST( MachLogLandMine*,this ) );
}

const MachLogLandMine& MachActor::asLandMine() const
{
	ASSERT_INFO( objectType() );
	ASSERT( objectType() == MachLog::LAND_MINE, "Bad _STATIC_CAST type in MachActor::asLandMine\n" );
	return *(_STATIC_CAST( const MachLogLandMine*,this ) );
}

int	MachActor::subType() const
{
	MachLog::ObjectType ot = objectType();
	switch( ot )
	{
		case MachLog::ADMINISTRATOR:
			return this->asAdministrator().subType();
		case MachLog::AGGRESSOR:
			return this->asAggressor().subType();
		case MachLog::CONSTRUCTOR:
			return this->asConstructor().subType();
		case MachLog::TECHNICIAN:
			return this->asTechnician().subType();
		case MachLog::FACTORY:
			return this->asFactory().subType();
		case MachLog::HARDWARE_LAB:
			return this->asHardwareLab().subType();
		case MachLog::MISSILE_EMPLACEMENT:
			return this->asMissileEmplacement().subType();
		case MachLog::ARTEFACT:
			return this->asArtefact().subType();
	}
	return 0;
}

void MachActor::assignToDifferentRace( MachLogRace& newRace )
{
	HAL_STREAM("(" << id() << ") MachActor::AssignToDifferentRace " << newRace.race() << "\n" );

	MachLogRaces& races = MachLogRaces::instance();

	races.remove( this );
	assignToDifferentProcess( &newRace );

	CB_DEPIMPL( MachLogRace*, pRace_ );
	CB_DEPIMPL( MachLogRace*, pOriginalRace_ );
	CB_DEPIMPL( PhysAbsoluteTime, changeRaceStartTime_ );

	pRace_ = &newRace;
	displayMapAndIconRace( newRace.race() );

	races.addToAllCollections( this );

	// machines collection is separate, special case.......(due to slightly iffy design!)
	if( objectIsMachine() )
	{
		races.addToMachinesCollection( &asMachine() );
	}

	notifyObservers( W4dSubject::CLIENT_SPECIFIC, MachLog::RACE_CHANGED );

	if( newRace.race() != pOriginalRace_->race() )
		changeRaceStartTime_ = SimManager::instance().currentTime();
	else
		changeRaceStartTime_ = 0;

	MachLogNetwork& network = MachLogNetwork::instance();
	if( network.isNetworkGame() )
	{
		HAL_STREAM(" it is a network game so determining if need message\n" );
		if( network.remoteStatus( race() ) == MachLogNetwork::LOCAL_PROCESS )
			network.messageBroker().sendChangeRaceMessage( id(), newRace.race() );
		else if( newRace.race() == pOriginalRace_->race() )
			network.messageBroker().sendChangeRaceMessage( id(), newRace.race() );
	}

	strategy().beInterrupted();

	HAL_STREAM(" done\n" );
}

void MachActor::addHitPoints( MachPhys::HitPointUnits inc)
{
	hp( hp() + inc );
	if( hp() > objectData().hitPoints() )
		hp( objectData().hitPoints() );
	doVisualiseSelectionState();
	notifyObservers( W4dSubject::CLIENT_SPECIFIC, MachLog::HEALTH_STATUS_CHANGED );
}

const PhysAbsoluteTime& MachActor::changeRaceStartTime() const
{
	CB_DEPIMPL( PhysAbsoluteTime, changeRaceStartTime_ );
	return changeRaceStartTime_;
}

void MachActor::changeRaceStartTime( const PhysAbsoluteTime& newChangeRaceStartTime )
{
	CB_DEPIMPL( PhysAbsoluteTime, changeRaceStartTime_ );
	changeRaceStartTime_ = newChangeRaceStartTime;
}

ulong	MachActor::lastBeHitFrame() const
{
	CB_DEPIMPL( ulong, lastBeHitFrame_ );
	return lastBeHitFrame_;
}

void MachActor::setHPAndArmour( MachPhys::HitPointUnits newHp, MachPhys::ArmourUnits newArmour )
{
	hp( newHp );
	armour( newArmour );
	notifyObservers( W4dSubject::CLIENT_SPECIFIC, MachLog::HEALTH_STATUS_CHANGED );
	doVisualiseSelectionState();

	MachLogNetwork& network = MachLogNetwork::instance();
	if( network.isNetworkGame() and network.remoteStatus( race() ) == MachLogNetwork::LOCAL_PROCESS )
	{
		network.messageBroker().sendAdjustHPAndArmourMessage( id(), newHp, newArmour );
	}
}

MachPhys::HitPointUnits MachActor::maximumhp( void ) const
{
	return objectData().hitPoints();
}

bool MachActor::willHitCauseDeath( int damage )
{
	MachPhys::ArmourUnits armourLeft = armour();

	MachPhys::ArmourUnits absorb = damage - 1;
	if( damage > armourLeft )
		absorb = armourLeft;

	MachPhys::HitPointUnits newHp = hp() - damage + absorb;
	if( newHp <= 0 )
		return true;

	return false;
}

MachLog::SelectableType	MachActor::selectableType() const
{
	switch( objectType() )
	{
		case MachLog::ORE_HOLOGRAPH:
		case MachLog::DEBRIS:
		case MachLog::SQUADRON:
		case MachLog::LAND_MINE:
		case MachLog::ARTEFACT:
			return MachLog::NOT_SELECTABLE;
		default:
			return MachLog::FULLY_SELECTABLE;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

// virtual
int MachActor::militaryStrength() const
{
	//default is 0;
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachActor::thisActorIsStrongerThanMe( const MachActor& otherActor ) const
{
	return otherActor.militaryStrength() > militaryStrength();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachActor::isOutGunned() const
{
	return militaryValueOfIncomingThreats() > ( militaryStrength() * 1.3 );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

int MachActor::localStrengthValue() const
{
	CB_DEPIMPL( int, localStrength_ );
	return localStrength_;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
int MachActor::localStrength() const
{
	//default is to just return my strength - no surrounding allies are taken into account.
   return militaryStrength();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

int MachActor::localStrengthMaximumRadius() const
{
	static int radius = 100;
	return radius;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

int MachActor::militaryValueFromHere( MachActor& otherActor ) const
{
	// this method has multiple exit points for efficiency

	static const int radiusSquared = ( localStrengthMaximumRadius() * localStrengthMaximumRadius() );
	static const int halfRadiusSquared = ( localStrengthMaximumRadius() / 2 ) * ( localStrengthMaximumRadius() / 2 );

	float multiplier;
	if( position().sqrEuclidianDistance( otherActor.position() ) > radiusSquared )
			return 0;
	else if( position().sqrEuclidianDistance( otherActor.position() ) > halfRadiusSquared )
			multiplier = 0.5;
	else
			multiplier = 1.0;

	return otherActor.militaryStrength() * multiplier;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachActor::updateLocalStrengthEstimate()
{
	// reset local strength to 0 before adding on values from everything in vicinity (including myself)
	CB_DEPIMPL( int, localStrength_ );
	localStrength_ = 0;

	ctl_pvector< MachActor >	actors;
    actors.reserve( 64 );
	MachLogRaces::instance().allFriendlyAggressiveMachinesWithinRange( this, localStrengthMaximumRadius(), &actors );

	for( ctl_pvector< MachActor >::iterator i = actors.begin(); i != actors.end() ; ++i )
	{
		localStrength_ += militaryValueFromHere( (**i) );
   	}

	// and let's not forget my own strength
	localStrength_ += militaryStrength();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

int MachActor::localStrengthFromPosition( const MexPoint3d& pos ) const
{
	MATHEX_SCALAR sqrDistance = pos.sqrEuclidianDistance( position() );
	float distanceMultiplier = 1.0;

	// cumulative fractional ( x0.7) multiplications at distance bands 40m, 80m, 120m, 160m, 250m, 500m
	if( sqrDistance > 1600 )
		distanceMultiplier *= 0.7;
	if( sqrDistance > 6400 )
		distanceMultiplier *= 0.7;
	if( sqrDistance > 14400 )
		distanceMultiplier *= 0.7;
	if( sqrDistance > 25600 )
		distanceMultiplier *= 0.7;
	if( sqrDistance > 62500 )
		distanceMultiplier *= 0.7;
	if( sqrDistance > 250000 )
		distanceMultiplier *= 0.7;

	return (int)( (float)localStrength()  * distanceMultiplier );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachActor::dealWithThreats()
{

	CB_DEPIMPL( PhysAbsoluteTime, nextSOSOpportunity_ );

	// if we have no threats at all, no need to do any threat processing
	if ( isIn1stPersonView()  or
	     ( objectIsMachine()  and  asMachine().insideAPC() ) )
	{
		return;
	}


	// go on medium alert immediately for having any threats at all
	if( objectIsCanAttack() )
		asCanAttack().setMinimumAlertness( 60 );

	// okay, we have to check to see whether or not we have any of these threats that I can detect
	// and which are deemed to be a serious, immediate threat.

	Actors strongThreats;
	strongThreats.reserve( 10 );

	populateStrongThreats( &strongThreats );

	// do we have any strong threats at all?
	if ( strongThreats.size() == 0 )
	{
		return;
	}

	// go on full alert
	if( objectIsCanAttack() )
		asCanAttack().setMinimumAlertness( 125 );

	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();

	// alert other nearby machines of our plight?
	if( timeNow > nextSOSOpportunity_ )
	{
		dispatchSOS( strongThreats );
		nextSOSOpportunity_ = timeNow + 6.0;		// "suck it and see" value
	}


	// initiate behaviours appropriate to our situation and defcon if applicable
	checkAndDoOnDefCon( strongThreats );

}

void MachActor::dealWithFirstPersonThreat( MachActor* pFirstPersonThreatActor )
{
	CB_DEPIMPL( PhysAbsoluteTime, nextFirstPersonSOSOpportunity_ );

	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();

	// alert other nearby machines of our plight?
	if( timeNow > nextFirstPersonSOSOpportunity_ )
	{
		Actors strongThreat;
		strongThreat.reserve( 1 );
		strongThreat.push_back( pFirstPersonThreatActor->id() );
		dispatchSOS( strongThreat );
		nextFirstPersonSOSOpportunity_ = timeNow + 4.0;		// "suck it and see" value
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachActor::checkAndDoOnDefCon( const Actors& /*strongThreats*/ )
{
	// default is to do nothing
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachActor::addThreat( UtlId threatId /*MachActor* pThreat*/ )
{
	CB_DEPIMPL( Actors, actorsThreateningMe_ );
	//Has to be defensive due to network
	if( not hasThisActorAsAThreat( threatId ) )
	{
		actorsThreateningMe_.push_back( threatId );
		MachLogNetwork& network = MachLogNetwork::instance();
		MachLogRaces& races = MachLogRaces::instance();
		if( network.isNetworkGame() and races.actorExists( threatId ) and network.remoteStatus( races.actor( threatId ).race() ) == MachLogNetwork::LOCAL_PROCESS )
		{
			network.messageBroker().sendActorThreatMessage( id(), threatId, MachLogMessageBroker::ADD_THREAT );
		}
	}

	POST( hasThreats() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachActor::removeThreat( UtlId threatId /*const MachActor* pThreat*/ )
{
	//This has to be defensive due to network asynchronicity.
	if( hasThisActorAsAThreat( threatId ) )
	{
		CB_DEPIMPL( Actors, actorsThreateningMe_ );
	   	Actors::iterator i = find( actorsThreateningMe_.begin(),
									actorsThreateningMe_.end(), threatId );

		ASSERT( i != actorsThreateningMe_.end(), "Threat was not found!" );

		actorsThreateningMe_.erase( i );

		MachLogNetwork& network = MachLogNetwork::instance();
		MachLogRaces& races = MachLogRaces::instance();
		if( network.isNetworkGame() and races.actorExists( threatId ) and network.remoteStatus( races.actor( threatId ).race() ) == MachLogNetwork::LOCAL_PROCESS )
		{
			network.messageBroker().sendActorThreatMessage( id(), threatId, MachLogMessageBroker::REMOVE_THREAT );
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachActor::hasThisActorAsAThreat( UtlId threatId /*const MachActor* pThreat*/ ) const
{
	CB_DEPIMPL( const Actors, actorsThreateningMe_ );
	Actors::const_iterator i = find( actorsThreateningMe_.begin(),
								actorsThreateningMe_.end(), threatId );

	if( i != actorsThreateningMe_.end() )
		return true;
	else
		return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachActor::hasThreats() const
{
	CB_DEPIMPL( Actors, actorsThreateningMe_ );
	return actorsThreateningMe_.size() > 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachActor::populateStrongThreats( Actors* pActors )
{
	// builds up a vector of threats within an arbitrary range of this actor

	PRE( pActors->size() == 0 )

	validateActorsThreateningMe();

	CB_DEPIMPL( Actors, actorsThreateningMe_ );

	// default for non-missile-emplacement constructions
	MATHEX_SCALAR sqrStrongThreatRange = 10000;

	Actors& strongThreats = *pActors;
	MachLogRaces& races = MachLogRaces::instance();

	// if we were a machine, this method would be being overridden. As we're not a machine,
	// we must be outside - only count as strong threats those threats which are not inside a building.
	for( Actors::iterator i = actorsThreateningMe_.begin(); i != actorsThreateningMe_.end(); ++i )
	{
		MachActor& actor = races.actor( *i );
		MachLogRaces::DispositionToRace disposition = races.dispositionToRace( race(), actor.race() );

		// only add this as a strong threat if we don't think it's a friend (same race or ally),
		if( ( disposition == MachLogRaces::NEUTRAL or disposition == MachLogRaces::ENEMY )
			and
			( position().sqrEuclidianDistance( actor.position() ) < sqrStrongThreatRange ) )
		{
			// can only check building location status of machines, not other actor types
			if ( actor.objectIsMachine() )
			{
				const MachLogMachine& threatMachine = actor.asMachine();

				if ( ( not threatMachine.insideBuilding() ) )
					strongThreats.push_back( *i );
			}
			else
			{
				// in the case of missile emplacements, only count them as a strong threat if they're capable
				// of firing at us right now

				ASSERT_INFO( actor.objectType() );
				ASSERT( actor.objectType() == MachLog::MISSILE_EMPLACEMENT, "Non-machine threat actor should only be a missile emplacement!" );


				const MachLogMissileEmplacement& missileEmp = actor.asMissileEmplacement();

					if ( missileEmp.canDetect( *this ) )
						strongThreats.push_back( *i );
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////

int MachActor::militaryValueOfIncomingThreats() const
{
	MachActor* pMe = _CONST_CAST( MachActor*, this );
	pMe->validateActorsThreateningMe();

	CB_DEPIMPL( const Actors, actorsThreateningMe_ );
	int totalValue = 0;

	MachLogRaces& races = MachLogRaces::instance();
	for( Actors::const_iterator i = actorsThreateningMe_.begin(); i != actorsThreateningMe_.end(); ++i )
	{
		MachActor& threatActor = races.actor( (*i ) );
		if( threatActor.objectIsMissileEmplacement() )
		{
			// only add its value to the pool if I'm in its weapon range
			if( threatActor.asMissileEmplacement().canTurnToAttack( *this ) )
			{
				totalValue += threatActor.militaryStrength();
			}
		}
		else
		{
			// it'll be a machine - its military strength gets added regardless
			totalValue += threatActor.militaryStrength();
		}
	}
	return totalValue;
}

//virtual
void MachActor::dispatchSOS( Actors& strongThreats )
{
	// Attempt to recruit fristd::endly thugs to help me tonk the rascals threatening me.
	// Said thugs must be idle and have a non-HIGH DefCon.

	Actors::iterator iThreats = strongThreats.begin();

   	// first try and recruit nearby idle aggressives to the cause;

	ctl_pvector< MachActor > friends;
	friends.reserve( 30 );

	MachLogRaces& races = MachLogRaces::instance();

	MATHEX_SCALAR firstWaveRadius = _STATIC_CAST( MATHEX_SCALAR, localStrengthMaximumRadius() ) + 50.0;
	MATHEX_SCALAR secondWaveRadius = _STATIC_CAST( MATHEX_SCALAR, localStrengthMaximumRadius() * 2 ) + 50.0;

	if( objectType() == MachLog::POD )
	{
		// special case logic for when the pod's being duffed in. Make the SOS radius really bloody big.
		firstWaveRadius *= 3;
		secondWaveRadius *= 3;
	}

	races.allFriendlyAggressiveMachinesWithinRange( this, firstWaveRadius, &friends );

	ctl_pvector< MachActor >::iterator iFriends = friends.begin();

	while( iThreats != strongThreats.end() and iFriends != friends.end() )
	{
		MachActor& threatActor = races.actor( *iThreats );
		if( threatActor.isOutGunned()
			or not MachLogCanAttack::IHaveClearWeaponPathToHim( this, &threatActor ) )
		{
			++iThreats;
		}
		else
		{
			MachActor& friendlyActor = (**iFriends);

			ASSERT( friendlyActor.objectIsCanAttack(), "Shouldn't be recruiting non-attack-capable actors." );
			ASSERT( friendlyActor.objectIsMachine(), "We should only be recruiting fristd::endly machines in an SOS, not missile emplacements." );

			MachLogRaces::DispositionToRace disposition = races.dispositionToRace( friendlyActor.race(), threatActor.race() );

			// This machine will only listen to the SOS request if it also considers the threat actor an enemy
			if( disposition == MachLogRaces::NEUTRAL or disposition == MachLogRaces::ENEMY )
			{
				// yep, he'll listen to the request

				MachLogMachine& friendlyMachine = friendlyActor.asMachine();
				MachLogCanAttack& friendlyCanAttack = friendlyActor.asCanAttack();

				// even if this actor doesn't respond to the SOS call, bump its alertness up to full
				friendlyCanAttack.setMinimumAlertness( 125 );


				if( friendlyMachine.virtualDefCon() != MachLog::DEFCON_HIGH
					and ( not friendlyCanAttack.hasCurrentTarget() )
					and friendlyCanAttack.willingToRespondToSOS()		// wounded machines may give this a miss
					and ( not friendlyMachine.isStandingGround() )
					and ( not friendlyMachine.evading() )
					and ( not friendlyMachine.insideBuilding() )
					and ( not friendlyMachine.isIn1stPersonView() )
					and ( friendlyCanAttack.canFireAt( threatActor ) )
					and ( not friendlyMachine.motionSeq().isFollowing() or friendlyMachine.virtualDefCon() == MachLog::DEFCON_LOW ) )
				{

					if( not friendlyMachine.strategy().isUninterruptable() )
					{
						friendlyMachine.motionSeq().stop();

						friendlyMachine.strategy().changeToCounterattackMode( &threatActor );
					}
				}
			}
			++iFriends;
		}
	}

	// then try and recruit those idle aggressives further away if the nearer machines were not sufficient
	// (do NOT bother attempting to recruit missile emplacements at this further range)
	// This uses firstWaveRadius as a minimum radius.

	ctl_pvector< MachActor > furtherFriends;
	furtherFriends.reserve( 40 );

	races.allFriendlyAggressiveMachinesWithinRange( this, secondWaveRadius, &furtherFriends, firstWaveRadius );
	iFriends = furtherFriends.begin();

	while( iThreats != strongThreats.end() and iFriends != furtherFriends.end() )
	{
		MachActor& threatActor = races.actor( *iThreats );
		if( threatActor.isOutGunned()
			or not MachLogCanAttack::IHaveClearWeaponPathToHim( this, &threatActor ) )
		{
			++iThreats;
		}
		else
		{
			MachActor& friendlyActor = (**iFriends);

			ASSERT( friendlyActor.objectIsCanAttack(), "Shouldn't be recruiting non-attack-capable actors." );
			ASSERT( friendlyActor.objectIsMachine(), "We should only be recruiting fristd::endly machines in an SOS, not missile emplacements." );

			MachLogRaces::DispositionToRace disposition = races.dispositionToRace( friendlyActor.race(), threatActor.race() );

			// This machine will only listen to the SOS request if it also considers the threat actor an enemy
			if( disposition == MachLogRaces::NEUTRAL or disposition == MachLogRaces::ENEMY )
			{
				// yep, he'll listen to the request

				MachLogMachine& friendlyMachine = friendlyActor.asMachine();
				MachLogCanAttack& friendlyCanAttack = friendlyActor.asCanAttack();

				// even if this actor doesn't respond to the SOS call, bump its alertness up a reasonable amount.
				friendlyCanAttack.setMinimumAlertness( 70 );

				if( friendlyMachine.virtualDefCon() != MachLog::DEFCON_HIGH
					and ( not friendlyCanAttack.hasCurrentTarget() )
					and friendlyCanAttack.willingToRespondToSOS()		// wounded machines may give this a miss
					and ( not friendlyMachine.isStandingGround() )
					and ( not friendlyMachine.evading() )
					and ( not friendlyMachine.insideBuilding() )
					and ( not friendlyMachine.isIn1stPersonView() )
					and ( friendlyCanAttack.canFireAt( threatActor ) )
					and ( not friendlyMachine.motionSeq().isFollowing() or friendlyMachine.virtualDefCon() == MachLog::DEFCON_LOW ) )
				{

					if( not friendlyMachine.strategy().isUninterruptable() )
					{
						friendlyMachine.motionSeq().stop();

						friendlyMachine.strategy().changeToCounterattackMode( &threatActor );
					}
				}
			}

			++iFriends;
		}
	}
}

MachActor::Actors& MachActor::actorsThreateningMe()
{
	CB_DEPIMPL( Actors, actorsThreateningMe_ );
	return actorsThreateningMe_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void perWrite( PerOstream& ostr, const MachActor& actor )
{
	const SimActor& base1 = actor;

	ostr << base1;
	ostr << actor.pImpl_;

// do not write out the selection state as all actors come back in unselected.
//	ostr << actor.selectionState_;
}

void perRead( PerIstream& istr, MachActor& actor )
{
	SimActor& base1 = actor;

	istr >> base1;
	istr >> actor.pImpl_;
	actor.assignToDifferentProcess( &actor.logRace() );
}

void MachActor::persistenceWriteStrategy( PerOstream& ostr ) const
{
	CB_DEPIMPL( MachLogStrategy*, pStrategy_ );
	ostr << pStrategy_;
}

void MachActor::persistenceReadStrategy( PerIstream& istr )
{
	CB_DEPIMPL( MachLogStrategy*, pStrategy_ );
	istr >> pStrategy_;
}

MachActor::MachActor( PerConstructor con )
:	SimActor( con )
{
}

void MachActor::actorCreated()
{
	CB_DEPIMPL( Actors, actorsThreateningMe_ );
    actorsThreateningMe_.reserve( 10 );

	//actors can already appear in the map depending on how they were created
	//so we can't assert out if already exists - only if they aren't the same.
	#if !defined( NDEBUG ) && !defined( PRODUCTION ) && !defined( DEMO )
	ASSERT_INFO( id() );
	if( MachLogRaces::instance().actorExists( id() ) )
	{
		ASSERT_INFO( MachLogRaces::instance().actor( id() ).objectType() );
		ASSERT_INFO( objectType() );
		ASSERT( &MachLogRaces::instance().actor( id() ) == this,"ID already allocated and pointers are not the same" );
	}
	#endif
	if( not MachLogRaces::instance().actorExists( id() ) )
		MachLogRaces::idGenerator().allocateId( id() );
}

bool MachActor::isInSpecialUpdateActorsList() const
{
	CB_DEPIMPL( bool, isInSpecialUpdateActorsList_ );
	return isInSpecialUpdateActorsList_;
}
void MachActor::isInSpecialUpdateActorsList( bool newValue )
{
	CB_DEPIMPL( bool, isInSpecialUpdateActorsList_ );
	isInSpecialUpdateActorsList_ = newValue;
}

const PhysRelativeTime&	MachActor::maximumTimeInSpecialUpdateActors() const
{
	CB_DEPIMPL( PhysRelativeTime, maximumTimeInSpecialUpdateActors_ );
	return maximumTimeInSpecialUpdateActors_;
}
void MachActor::maximumTimeInSpecialUpdateActors( const PhysRelativeTime& maxTime )
{
	CB_DEPIMPL( PhysRelativeTime, maximumTimeInSpecialUpdateActors_ );
	maximumTimeInSpecialUpdateActors_ = maxTime;
}

const PhysAbsoluteTime&	MachActor::startTimeInSpecialUpdateActors() const
{
	CB_DEPIMPL( PhysAbsoluteTime, startTimeInSpecialUpdateActors_ );
	return startTimeInSpecialUpdateActors_;
}

void MachActor::startTimeInSpecialUpdateActors( const PhysAbsoluteTime&	startTime )
{
	CB_DEPIMPL( PhysAbsoluteTime, startTimeInSpecialUpdateActors_ );
	startTimeInSpecialUpdateActors_ = startTime;
}

MachLog::ObjectType MachActor::objectType() const
{
	CB_DEPIMPL( MachLog::ObjectType, objectType_ );
	return objectType_;
}

PhysAbsoluteTime MachActor::nextSOSOpportunity() const
{
	CB_DEPIMPL( PhysAbsoluteTime, nextSOSOpportunity_ );
	return nextSOSOpportunity_;
}

void MachActor::setupBasicTargetOffset()
{
	CB_DEPIMPL( MexPoint3d, basicTargetOffset_ );

	//Use the target centroid as the aim point
    const W4dEntity& targetEntity = physObject();
    MexAlignedBox3d targetBoundary =
        (targetEntity.isComposite() ? targetEntity.asComposite().compositeBoundingVolume()
                                    : targetEntity.boundingVolume());

    if( targetBoundary.minCorner().z() < 0.0 )
    {
        MexPoint3d newCorner( targetBoundary.minCorner() );
        newCorner.z( 0.0 );
        targetBoundary.minCorner( newCorner );
    }

    basicTargetOffset_ = targetBoundary.centroid();
}

const MexPoint3d& MachActor::basicTargetOffset() const
{
	CB_DEPIMPL( MexPoint3d, basicTargetOffset_ );

	return basicTargetOffset_;
}

const MexPoint3d& MachActor::predictedTargetOffset( PhysRelativeTime t )
{
	// NOTE this method has multiple exit points for maximum efficiency, avoidance of creating
	// local data where possible etc.

	CB_DEPIMPL( MexPoint3d, cachedTargetOffset_ );
	CB_DEPIMPL( PhysRelativeTime, nextPredictedPositionRecalculateTime_ );


	// note! Ignores possibility of ILFs.
	if( objectIsMachine() )
	{
		PhysAbsoluteTime timeNow = SimManager::instance().currentTime();

		if( timeNow >= nextPredictedPositionRecalculateTime_ )
		{
			const MachLogMachine& machine = asMachine();
			// we'll have to add on some stuff in the x and z planes that takes into account the direction of travel
			MATHEX_SCALAR xShift = machine.scalarSpeedNow() * t;

			MexPoint3d predictedDestinationPoint( xShift, 0, 0 );
			physObject().globalTransform().transform( &predictedDestinationPoint );
			MexPoint3d heightAdjustedDestinationPoint = MachLogSpacialManipulation::heightAdjustedLocation( predictedDestinationPoint );

			// what we should actually do here is to take the height-adjusted global location, turn it back into a local
			// thing, and add to it the basicTargetOffset. Simple.

			MexPoint3d destinationRelativeToMachine( heightAdjustedDestinationPoint );
			physObject().globalTransform().transformInverse( &heightAdjustedDestinationPoint );
			MexPoint3d result = heightAdjustedDestinationPoint;
			result += basicTargetOffset();
			cachedTargetOffset_ = result;
			// const-cast naughtiness, but this is okay as it doesn't violate the spirit of const
			// from the client's point of view - this is for cached lazy evaluation.
			MachActor* pThis = _CONST_CAST( MachActor*, this ) ;
			pThis->pImpl_->nextPredictedPositionRecalculateTime_ = timeNow + 0.05;
		}
		return cachedTargetOffset_;
	}
	else
		return basicTargetOffset();
}

void MachActor::dispatchCaveeCall()
{
	// this sets the alertness of all local chums to high.

	ctl_pvector< MachActor > friends;
	friends.reserve( 30 );

	MachLogRaces::instance().allFriendlyAggressiveMachinesWithinRange( this, localStrengthMaximumRadius(), &friends );

	for( ctl_pvector< MachActor >::iterator iFriends = friends.begin(); iFriends != friends.end(); ++iFriends )
	{
		ASSERT( (*iFriends)->objectIsCanAttack(), "One of the 'local aggressives' is not a canAttack!" );

		(*iFriends)->asCanAttack().setMinimumAlertness( 125 );
	}
}

void MachActor::setBusyPeriod( PhysRelativeTime busyPeriod )
{
	CB_DEPIMPL( PhysAbsoluteTime, busyUntil_ );

	busyUntil_ = SimManager::instance().currentTime() + busyPeriod;
}

bool MachActor::busy() const
{
	CB_DEPIMPL( PhysAbsoluteTime, busyUntil_ );

	return( not isDead() and SimManager::instance().currentTime() <  busyUntil_ );
}

void MachActor::updateArmourAndHps( int damageTaken )
{
	MachPhys::ArmourUnits armourLeft = armour();

	// default is for armour to absorb all but 1 point of the damage
	MachPhys::ArmourUnits absorb = damageTaken - 1;

	// if not enough armour to absorb this much, just absorb as much as possible.
	if( absorb > armourLeft )
		absorb = armourLeft;

	armour( armourLeft - absorb );
	MachPhys::HitPointUnits hpLeft = hp();
	hp( hpLeft - ( damageTaken - absorb ) );

	POST( armour() >= 0 );
}

void MachActor::displayMapAndIconRace( MachPhys::Race race )
{
	CB_DEPIMPL( MachPhys::Race, displayMapAndIconRace_ );
	displayMapAndIconRace_ = race;
}

MachPhys::Race MachActor::displayMapAndIconRace() const
{
	CB_DEPIMPL( MachPhys::Race, displayMapAndIconRace_ );
	return displayMapAndIconRace_;
}

void MachActor::validateActorsThreateningMe()
{
	CB_DEPIMPL( Actors, actorsThreateningMe_ );

	Actors removeTheseActors;
	MachLogRaces& races = MachLogRaces::instance();

	for( Actors::iterator i = actorsThreateningMe_.begin(); i != actorsThreateningMe_.end(); ++i )
	{
		if( not races.actorExists( *i ) )
			removeTheseActors.push_back( *i );
	}
	//Remove redundant actors (remember that there are two seperate mechanisms for removal (i) by removeThreat
	//and (ii) by bit in loop above which verifies actor still exists - this was written soley to enable
	//network threats to work correctly.
	for( Actors::iterator i = removeTheseActors.begin(); i != removeTheseActors.end(); ++i )
		actorsThreateningMe_.erase( find( actorsThreateningMe_.begin(), actorsThreateningMe_.end(), *i ) );
}

void MachActor::checkAndDoSpecialDeaths()
{
	if( objectType() == MachLog::AGGRESSOR )
	{
		MachLogAggressor& aggressor = asAggressor();

		if( aggressor.isEradicator() )
		{
			// okay, this is a self-destructing eradicator! Let's make it kick out one final vortex.
			// From hell's heart, I stab at thee........for hate's sake, I spit my last breath at thee........

			MachLogWeapon& vortexWeapon = *( aggressor.weapons().front() );
			ASSERT( _CONST_CAST( const MachLogWeapon&, vortexWeapon ).physWeapon().type() == MachPhys::VORTEX, "Eradicator's first weapon was NOT a vortex!" );

			vortexWeapon.fire( position() );
		}
	}
	else if( objectType() == MachLog::MISSILE_EMPLACEMENT )
	{
		MachLogMissileEmplacement& me = asMissileEmplacement();
		if( me.subType() == MachPhys::ICBM and me.isComplete() )
		{
			// okay, this is a self-destructing ICBM! Let's make it kick out one final nuke.
			// From hell's heart, I eat beans at thee........for Keith's sake, I wheeze my last breath at thee........

			MachLogWeapon& nukeWeapon = *( me.weapons().front() );
			ASSERT( _CONST_CAST( const MachLogWeapon&, nukeWeapon ).physWeapon().type() == MachPhys::NUCLEAR_MISSILE, "ICBM's first weapon was NOT a nuke!" );

			nukeWeapon.fire( position() );
		}
	}
}

/* //////////////////////////////////////////////////////////////// */

/* End ACTOR.CPP ****************************************************/
