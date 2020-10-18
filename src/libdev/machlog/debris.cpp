/*
 * O R E H O L O . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "stdlib/string.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/abox2d.hpp"
//#include "mathex/abox2d.hpp"
#include "world4d/domain.hpp"
#include "sim/manager.hpp"
#include "machphys/debris.hpp"
#include "machphys/consdata.hpp"
#include "machphys/mphydata.hpp"

#include "machlog/debris.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/planet.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"
#include "machlog/dyingent.hpp"

#include "machlog/network.hpp"
#include "machlog/messbrok.hpp"

const MATHEX_SCALAR BURN_TIME = 60;

PER_DEFINE_PERSISTENT( MachLogDebris );
//////////////////////////////////////////////////////////////////////////////////////////

MachLogDebris::MachLogDebris
(
    MachLogRace* pRace, MachPhys::BuildingMaterialUnits quantity, const MexPoint3d& location, const MexAlignedBox2d& boundary
)
:   MachActor( pRace, pNewDebris( pRace, quantity, location, boundary ), MachLog::DEBRIS ),
	quantity_( quantity ),
	lastUpdateTime_( SimManager::instance().currentTime() ),
	pBoundary_( _NEW( MexAlignedBox2d( boundary ) ) )
{
	setLifeTime( quantity );

	setupChildrenDebrisIDs();

	HAL_STREAM("(" << id() << ") MLDebris::CTOR location " << location << " boundary " << boundary << " quant " << quantity << std::endl );
	pPhysDebris()->startBurning();

	MachLogPlanet::instance().newDebrisSite( this );

	TEST_INVARIANT;
}

MachLogDebris::MachLogDebris
(
    MachLogRace* pRace, MachPhys::BuildingMaterialUnits quantity, const MexPoint3d& location, const MexAlignedBox2d& boundary, UtlId withId
)
:   MachActor( pRace, pNewDebris( pRace, quantity, location, boundary ), MachLog::DEBRIS, withId ),
	quantity_( quantity ),
	lastUpdateTime_( SimManager::instance().currentTime() ),
	pBoundary_( _NEW( MexAlignedBox2d( boundary ) ) )
{
	setLifeTime( quantity );

	setupChildrenDebrisIDs();

	HAL_STREAM("(" << id() << ") MLDebris::CTOR location " << location << " boundary " << boundary << " quant " << quantity << std::endl );
	pPhysDebris()->startBurning();

	MachLogPlanet::instance().newDebrisSite( this );

	TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

//static
MachPhysDebris* MachLogDebris::pNewDebris
(
    MachLogRace* pRace, MachPhys::BuildingMaterialUnits quantity, const MexPoint3d& location, const MexAlignedBox2d& boundary
)
{
    //get domain and transform to use
    MexTransform3d localTransform;
    W4dDomain* pDomain =
        MachLogPlanetDomains::pDomainPosition( location, 0, &localTransform );

    //Construct the debris
	MachPhysDebris::MachPhysDebrisType whichType = MachPhysDebris::SMALL;
	MATHEX_SCALAR area = boundary.area();
	HAL_STREAM("MLDebris::pNewDebris computed area " << area << std::endl );
	if( area > 2500 )
		whichType = MachPhysDebris::LARGE;
	else if( area > 1600 )
		whichType = MachPhysDebris::LARGISH;
	else if( area > 900 )
		whichType = MachPhysDebris::MEDIUM;
	else if( area > 400 )
		whichType = MachPhysDebris::SMALLISH;

//Not needed	MexPoint2d minCorner( boundary.minCorner() );
//Not needed	MexPoint2d maxCorner( boundary.maxCorner() );
//Not needed	MexTransform3d inverseDomainGlobal;
//Not needed	pDomain->globalTransform().invert( &inverseDomainGlobal );
//Not needed	inverseDomainGlobal.transform( &minCorner );
//Not needed	inverseDomainGlobal.transform( &maxCorner );
//Not needed	if( minCorner.x() < 0 )
//Not needed		minCorner.x( minCorner.x() * .75 );
//Not needed	else
//Not needed		minCorner.x( minCorner.x() * 1.25 );
//Not needed	if( minCorner.y() < 0 )
//Not needed		minCorner.y( minCorner.y() * .75 );
//Not needed	else
//Not needed		minCorner.y( minCorner.y() * 1.25 );
//Not needed	maxCorner.x( maxCorner.x() * 0.75 );
//Not needed	maxCorner.y( maxCorner.y() * 0.75 );
//Not needed	MexAlignedBox2d localBoundary( minCorner , maxCorner );
//Not needed
//Not needed	HAL_STREAM(" Local Transform is\n" << localTransform << std::endl << " Boundary is\n" << boundary << std::endl << " local Boundary is " << localBoundary << std::endl );

	return _NEW( MachPhysDebris( pDomain, localTransform, boundary,
                                       BURN_TIME, whichType ) );
}
//////////////////////////////////////////////////////////////////////////////////////////

MachLogDebris::~MachLogDebris()
{
	//HAL_STREAM("(" << id() << ") MLDebris::DTOR\n" );
	_DELETE( pBoundary_ );

	MachLogPlanet::instance().removeDebrisSite( this );

    TEST_INVARIANT;

}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogDebris::CLASS_INVARIANT
{
	INVARIANT( this );
}
//////////////////////////////////////////////////////////////////////////////////////////

//virtual
PhysRelativeTime MachLogDebris::beDestroyed()
{
    return 0;
}
//////////////////////////////////////////////////////////////////////////////////////////

//virtual
bool MachLogDebris::exists() const
{
    return true;
}
//////////////////////////////////////////////////////////////////////////////////////////
//virtual
void MachLogDebris::doStartExplodingAnimation()
{
}

//virtual
void MachLogDebris::doEndExplodingAnimation()
{
}

//virtual
const MachPhysObjectData& MachLogDebris::objectData() const
{
	return data();
}

const MachPhysConstructionData& MachLogDebris::constructionData() const
{
	return data();
}

//TBD: Woah would you look at that...someone returning the oreholograph data for buring debris....
const MachPhysConstructionData& MachLogDebris::data() const
{
	//return _REINTERPRET_CAST( MachPhysConstructionData&, MachPhysData::instance().oreHolographData() );
	return _REINTERPRET_CAST( const MachPhysConstructionData&, MachPhysData::instance().oreHolographData() );
}

//virtual
void MachLogDebris::doVisualiseSelectionState()
{
	//Do nothing - they can't be selected
}

const MachPhys::BuildingMaterialUnits MachLogDebris::quantity() const
{
	return quantity_;
}

MachPhysDebris* MachLogDebris::pPhysDebris()
{
    return (MachPhysDebris*)( &physObject() );
}

const MachPhysDebris* MachLogDebris::pPhysDebris() const
{
    return (MachPhysDebris*)( &physObject() );
}

void MachLogDebris::hasBeenPickedUp()
{
	MachLogDyingEntityEvent* pEvent = _NEW( MachLogDyingEntityEvent( physObjectPtr(), NULL, 0, MachLogDyingEntityEvent::NOT_INSIDE_BUILDING, NULL ) );
	SimManager::instance().add( pEvent );
	isDead( true );
}

void MachLogDebris::setupChildrenDebrisIDs()
{
	MachPhysDebris* pNewDebris = pPhysDebris();

	MachPhysDebris::DebrisUnits& debrisUnits = pNewDebris->debrisUnits();

	UtlId debrisParentID = pNewDebris->id();

	for ( MachPhysDebris::DebrisUnits::iterator i = debrisUnits.begin(); i != debrisUnits.end(); ++i )
   	{
   		(*i)->id( debrisParentID );
   	}
}


//virtual
PhysRelativeTime MachLogDebris::update( const PhysRelativeTime& maxCPUTime,
                                              MATHEX_SCALAR clearence )
{
	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();
	lifetimeRemaining_ -= ( timeNow - lastUpdateTime_ );
	lastUpdateTime_ = timeNow;

	if (lifetimeRemaining_ <= 0)
	{
		MachLogDyingEntityEvent* pEvent = _NEW( MachLogDyingEntityEvent( physObjectPtr(), NULL, 0, MachLogDyingEntityEvent::NOT_INSIDE_BUILDING, NULL ) );
		SimManager::instance().add( pEvent );
		isDead( true );
		if( MachLogNetwork::instance().isNetworkGame() )
			MachLogNetwork::instance().messageBroker().sendDebrisExpiredMessage( id() );
	}


	return MachActor::update( maxCPUTime, clearence );
}

void perWrite( PerOstream& ostr, const MachLogDebris& actor )
{
	const MachActor& base1 = actor;

	ostr << base1;
	ostr << actor.quantity_;
	ostr << actor.lifetimeRemaining_;
	ostr << actor.lastUpdateTime_;
	ostr << actor.pBoundary_;
	const MexTransform3d& trans = actor.globalTransform();
	PER_WRITE_RAW_OBJECT( ostr, trans );
	const W4dId id = actor.id();
	PER_WRITE_RAW_OBJECT( ostr, id );
	actor.persistenceWriteStrategy( ostr );

}

void perRead( PerIstream& istr, MachLogDebris& actor )
{
	MachActor& base1 = actor;

	istr >> base1;
	istr >> actor.quantity_;
	istr >> actor.lifetimeRemaining_;
	istr >> actor.lastUpdateTime_;
	istr >> actor.pBoundary_;

	MexTransform3d trans;
	W4dId id;
	PER_READ_RAW_OBJECT( istr, trans );
	PER_READ_RAW_OBJECT( istr, id );

	MachPhysDebris* pPhysDebris = MachLogDebris::pNewDebris( &actor.logRace(), actor.quantity_, trans.position(), *actor.pBoundary_ );
	actor.setObjectPtr( pPhysDebris, trans );
	actor.id( id );
	actor.actorCreated();
	actor.persistenceReadStrategy( istr );

    //Add this debris to the planet's list
	MachLogPlanet::instance().newDebrisSite( &actor );
}

MachLogDebris::MachLogDebris( PerConstructor con )
:	MachActor( con )
{
}

void MachLogDebris::setLifeTime( MachPhys::BuildingMaterialUnits quantity )
{
	// Little trick here....if no race currently has a scavenger built, the debris is a bit meaningless.
	// In these cases, just set the burnm time to something small to keep the number of actors down.

	bool scavengersExist = false;

	for( MachPhys::Race r = MachPhys::RED; not( scavengersExist ) and r < MachPhys::N_RACES; ++( (int&)r ) )
	{
		if( MachLogRaces::instance().nScavengers( r ) )
			scavengersExist = true;
	}

	if( scavengersExist )
		lifetimeRemaining_ = BURN_TIME + ( quantity * 2 );		// debris from bigger (well, more costly...) things sticks around longer
	else
		lifetimeRemaining_ = BURN_TIME;
}


/* End OREHOLO.CPP **************************************************/
