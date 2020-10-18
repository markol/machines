/*
 * GARRISON . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "stdlib/string.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/abox2d.hpp"
#include "mathex/eulerang.hpp"
#include "phys/phys.hpp"
#include "world4d/domain.hpp"
#include "machphys/garrison.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/garrdata.hpp"

#include "machlog/garrison.hpp"
#include "machlog/internal/garrisoi.hpp"
#include "machlog/machlog.hpp"
#include "machlog/planet.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"
#include "machlog/machine.hpp"
#include "machlog/spy.hpp"

#include "machlog/network.hpp"
#include "machlog/messbrok.hpp"

#include "sim/manager.hpp"

#include <algorithm>

#ifndef _INLINE
    #include "machlog/garrison.ipp"
#endif

#define CB_MachLogGarrison_DEPIMPL()	\
	PRE( pImpl_ )	\
	CB_DEPIMPL( MachLogConstruction::Machines, machinesBeingHealed_ ); \
	CB_DEPIMPL( PhysAbsoluteTime , lastUpdateTime_) ;

//////////////////////////////////////////////////////////////////////////////////////////
PER_DEFINE_PERSISTENT( MachLogGarrison );

MachLogGarrison::MachLogGarrison( MachLogRace* pRace, uint level,
                    const MexPoint3d& location, const MexRadians& angle )
:   MachLogConstruction( pRace, pNewPhysGarrison( pRace, level, location, angle ),
						 MachLog::GARRISON,
                         MachPhysData::instance().garrisonData( level ) ),
	pImpl_( _NEW( MachLogGarrisonImpl ) )
{

	CB_MachLogGarrison_DEPIMPL();
    TEST_INVARIANT;

	//do not set hp here
	//hp( data().hitPoints() );
	armour( data().armour() );
	MachLogRaces::instance().garrisons( pRace->race() ).push_back( this );
}

MachLogGarrison::MachLogGarrison( MachLogRace* pRace, uint level,
                    const MexPoint3d& location, const MexRadians& angle, UtlId withId )
:   MachLogConstruction( pRace, pNewPhysGarrison( pRace, level, location, angle ),
						 MachLog::GARRISON,
						 withId,
                         MachPhysData::instance().garrisonData( level ) ),
	pImpl_( _NEW( MachLogGarrisonImpl ) )
{

	CB_MachLogGarrison_DEPIMPL();
    TEST_INVARIANT;

	//do not set hp here
	//hp( data().hitPoints() );
	armour( data().armour() );
	MachLogRaces::instance().garrisons( pRace->race() ).push_back( this );
}
//////////////////////////////////////////////////////////////////////////////////////////

MachLogGarrison::~MachLogGarrison()
{
    TEST_INVARIANT;
	_DELETE( pImpl_ );
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogGarrison::CLASS_INVARIANT
{
	INVARIANT( this );
}
//////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const MachLogGarrison& t )
{

    o << "MachLogGarrison " << (void*)&t << " start" << std::endl;
    o << "MachLogGarrison " << (void*)&t << " end" << std::endl;

    return o;
}

//////////////////////////////////////////////////////////////////////////////////////////

//virtual
void MachLogGarrison::doOutputOperator( ostream& o ) const
{
	o << *this;
	o << "Repair rate: " << data().repairRate() << std::endl;
	MachLogConstruction::doOutputOperator( o );
}

//////////////////////////////////////////////////////////////////////////////////////////

MachPhysGarrison* MachLogGarrison::pNewPhysGarrison
(
    MachLogRace* pRace, uint level, const MexPoint3d& location, const MexRadians& zAngle
)
{
    //get domain and transform to use
    MexTransform3d localTransform;
    W4dDomain* pDomain =
        MachLogPlanetDomains::pDomainPosition( location, zAngle, &localTransform );

    //Construct the smelter
    return _NEW( MachPhysGarrison( pDomain, localTransform, level, pRace->race() ) );
}
//////////////////////////////////////////////////////////////////////////////////////////

//virtual
PhysRelativeTime MachLogGarrison::update( const PhysRelativeTime& alteredMaxCPUTime, MATHEX_SCALAR junk )
{
    bool complete = isComplete();

	PhysRelativeTime interval = 8.0;

	if( not machines().empty() )
	{
		// faster callback time if have machines inside me
		interval = 2.0;
	}

    if( not updateCompletionVisualisation() )
		interval = 0.1;
	else
	{
		if( not pPhysGarrison()->isWorking() )
		{
			pPhysGarrison()->isWorking( true );
			if( MachLogNetwork::instance().isNetworkGame() )
				MachLogNetwork::instance().messageBroker().sendPlayNormalObjectAnimationMessage( id(), true );
		}
	}

	// restock fristd::endly spies' landmines
	for (MachLogConstruction::Machines::iterator i = machines().begin(); i != machines().end(); ++i )
	{
		MachLogMachine* pMachine=(*i);

		if ( (*i)->objectType() == MachLog::SPY_LOCATOR )
		{
			MachLogRaces::DispositionToRace disposition = MachLogRaces::instance().dispositionToRace( race(), pMachine->race() );

			if ( disposition == MachLogRaces::OUR_RACE )
			{
				pMachine->asSpyLocator().restockMines();
			}
		 }
	}

	MachActor::update( alteredMaxCPUTime, junk );

	// heal all fristd::endly machines residing within...
	healAll();

    return interval;
}

//////////////////////////////////////////////////////////////////////////////////////////

MachPhysGarrison* MachLogGarrison::pPhysGarrison()
{
    return (MachPhysGarrison*)( &physObject() );
}

const MachPhysGarrison* MachLogGarrison::pPhysGarrison() const
{
    return (MachPhysGarrison*)( &physObject() );
}
//////////////////////////////////////////////////////////////////////////////////////////

//virtual
const MachPhysConstructionData& MachLogGarrison::constructionData() const
{
	return data();
}

const MachPhysGarrisonData& MachLogGarrison::data() const
{
	return pPhysGarrison()->data();
}

//////////////////////////////////////////////////////////////////////////////////////////

void MachLogGarrison::healAll()
{
	// firstly, set up new healing relationships

	CB_MachLogGarrison_DEPIMPL();

	// code to add newly-viable machines to our healing list
	for (MachLogConstruction::Machines::iterator i = machines().begin(); i != machines().end(); ++i )
	{
		MachLogMachine* pMachine=(*i);

		MachLogRaces::DispositionToRace disposition = MachLogRaces::instance().dispositionToRace( race(), pMachine->race() );

		if ( disposition == MachLogRaces::OUR_RACE or disposition == MachLogRaces::ALLY )
		{
			// can only heal machines standing still that have less than maximum hp, are idle and which
			// we're not already healing
			if( pMachine->hpRatio() < 1.0
				/*and pMachine->isIdle()*/
				and not currentlyHealing( pMachine ) )
			{
				startHealing ( pMachine );
			}
		}
	}

	// next, iterate through all those machines in our list of those currently being healed.
	// If any of them are no longer valid candidates for healing, stop healing them.

	bool foundNoMachinesThatNeedHealingStopped = false;

	while( not foundNoMachinesThatNeedHealingStopped )
	{
		foundNoMachinesThatNeedHealingStopped = true;	// provisional assignment

		for (MachLogConstruction::Machines::iterator i = machinesBeingHealed_.begin(); i != machinesBeingHealed_.end(); ++i )
		{
			MachLogMachine* pMachineOnHealingList = (*i);

			if( pMachineOnHealingList->hpRatio() == 1.0
				/*or not pMachineOnHealingList->isIdle()*/
				or not containsMachine( pMachineOnHealingList ) )	 // can't continue to heal if outside building
			{
				stopHealing ( pMachineOnHealingList );
				foundNoMachinesThatNeedHealingStopped = false;
				break;
			}
		}
	}

	// then iterate through all those on list, and add hps.

	MachPhys::HitPointUnits addAmount = (int) ( (float)data().repairRate() *
			   ( ( SimManager::instance().currentTime() - lastUpdateTime_ ) / 60.0 ) );


	if ( addAmount > 0 )
	{
		lastUpdateTime_ = SimManager::instance().currentTime();

		for (MachLogConstruction::Machines::iterator i = machinesBeingHealed_.begin();
											i != machinesBeingHealed_.end(); ++i )
		{
			(*i)->addHitPoints( addAmount );
		}
	}
	// otherwise, lastUpdateTime_ stays as it was, and no machines are healed this turn.

}

//////////////////////////////////////////////////////////////////////////////////////////

void MachLogGarrison::startHealing( MachLogMachine* pMachine )
{
	CB_MachLogGarrison_DEPIMPL();
   	PRE( not currentlyHealing( pMachine ) );

	machinesBeingHealed_.push_back( pMachine );
	pMachine->attach( this );
	pMachine->addHealingAuraReference()	;
	MachLogNetwork& network = MachLogNetwork::instance();
	if( network.isNetworkGame() )
		network.messageBroker().sendHealMessage( id(), pMachine->id(), MachLogMessageBroker::BEGIN_HEALING, true );

	POST( currentlyHealing( pMachine ) );
}

//////////////////////////////////////////////////////////////////////////////////////////

void MachLogGarrison::stopHealing( MachLogMachine* pMachine )
{
	CB_MachLogGarrison_DEPIMPL();

	PRE( currentlyHealing( pMachine ) );

	MachLogConstruction::Machines::iterator iToBeRemoved = find( machinesBeingHealed_.begin(),
								machinesBeingHealed_.end(), pMachine );

	ASSERT( iToBeRemoved != machinesBeingHealed_.end(), "pMachine not found in machinesBeingHealed_ vector!" );

	machinesBeingHealed_.erase( iToBeRemoved );
	pMachine->detach( this );
	pMachine->releaseHealingAuraReference()	;
	MachLogNetwork& network = MachLogNetwork::instance();
	if( network.isNetworkGame() )
		network.messageBroker().sendHealMessage( id(), pMachine->id(), MachLogMessageBroker::STOP_HEALING, true );

	POST( not currentlyHealing( pMachine ) );
}

//////////////////////////////////////////////////////////////////////////////////////////

bool MachLogGarrison::currentlyHealing( MachLogMachine* pMachine )
{
	CB_MachLogGarrison_DEPIMPL();
	MachLogConstruction::Machines::iterator i = find( machinesBeingHealed_.begin(),
								machinesBeingHealed_.end(), pMachine );

	return i != machinesBeingHealed_.end();
}

//////////////////////////////////////////////////////////////////////////////////////////

//virtual
bool MachLogGarrison::beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int  )
{
	CB_MachLogGarrison_DEPIMPL();
  	ASSERT_DATA( MachActor* pActor = _STATIC_CAST( MachActor*, pSubject ); );
	ASSERT( pActor->objectIsMachine(), "MachLogGarrison::beNotified : have been notified by a subject that is NOT a machine." );

	bool stayAttached = true;

	switch( event )
	{
	case W4dSubject::DELETED:
	case W4dSubject::LEFT_DOMAIN:
		{
			MachLogConstruction::Machines::iterator i = find( machinesBeingHealed_.begin(),
								machinesBeingHealed_.end(), pSubject );

			if( i != machinesBeingHealed_.end() )
			{
				stopHealing( (*i) );
				stayAttached = false;
			}
		}
	default:
		;
	}

	return stayAttached;
}

//virtual
int MachLogGarrison::militaryStrength() const
{
	if( not isComplete() )
		return 0;

	int hwLevel = (int)( asConstruction().level() );
	// arbitrariness!
	return ( hp()*hwLevel ) / 20;
}


/////////////////////////////////////////////////// persistence /////////////////////////////////////////////////////

void perWrite( PerOstream& ostr, const MachLogGarrison& actor )
{
	const MachLogConstruction& base1 = actor;

	ostr << base1;
	ostr << actor.pImpl_;

	const MexTransform3d& trans = actor.globalTransform();
	PER_WRITE_RAW_OBJECT( ostr, trans );
	const W4dId id = actor.id();
	PER_WRITE_RAW_OBJECT( ostr, id );
	uint level = actor.level();
	PER_WRITE_RAW_OBJECT( ostr, level );
	bool isWorking = actor.pPhysGarrison()->isWorking();
	PER_WRITE_RAW_OBJECT( ostr, isWorking );

	actor.persistenceWriteStrategy( ostr );
}

void perRead( PerIstream& istr, MachLogGarrison& actor )
{
	MachLogConstruction& base1 = actor;

	istr >> base1;
	istr >> actor.pImpl_;

	MexTransform3d trans;
	W4dId id;
	uint level;
	bool isWorking;
	PER_READ_RAW_OBJECT( istr, trans );
	PER_READ_RAW_OBJECT( istr, id );
	PER_READ_RAW_OBJECT( istr, level );
	PER_READ_RAW_OBJECT( istr, isWorking );

	MachPhysGarrison* pPhysGarrison = MachLogGarrison::pNewPhysGarrison( &actor.logRace(), level, trans.position(), trans.rotationAsEulerAngles().azimuth() );
	actor.setObjectPtr( pPhysGarrison, trans );
	actor.id( id );
	actor.constructionCreated( pPhysGarrison );
	MachLogRaces::instance().nConstructions( actor.race() )++;
	pPhysGarrison->isWorking( isWorking );
	actor.persistenceReadStrategy( istr );
}

MachLogGarrison::MachLogGarrison( PerConstructor con )
:	MachLogConstruction( con )
{
}

// Forced recompile 19/2/99 CPS
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
/* End GARRISON.CPP **************************************************/
