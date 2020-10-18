/*
 * L O C A T O R . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include <iostream>
#include "stdlib/string.hpp"

#include "ctl/algorith.hpp"
#include "ctl/list.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/point2d.hpp"
#include "world4d/domain.hpp"

#include "machphys/geolocat.hpp"
#include "machphys/glocdata.hpp"

#include "machlog/geolocat.hpp"
#include "machlog/planet.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/race.hpp"
#include "machlog/races.hpp"

#include "machlog/network.hpp"
#include "machlog/messbrok.hpp"

PER_DEFINE_PERSISTENT( MachLogGeoLocator );
/* //////////////////////////////////////////////////////////////// */

MachLogGeoLocator::MachLogGeoLocator( MachLogMachine::Level hwLevel, MachLogMachine::Level swLevel,
    								MachLogRace * pRace, const MexPoint3d& location )
: MachLogMachine( MachPhys::GEO_LOCATOR, hwLevel, swLevel, pRace,
                  pNewPhysGeoLocator( hwLevel, swLevel, pRace, location ), MachLog::GEO_LOCATOR )
{
//	objectType( MachLog::GEO_LOCATOR);
	hp( data().hitPoints() );
	armour( data().armour() );
	ctl_append( &MachLogRaces::instance().geoLocators( pRace->race() ), this );
}

MachLogGeoLocator::MachLogGeoLocator( MachLogMachine::Level hwLevel, MachLogMachine::Level swLevel,
    								MachLogRace * pRace, const MexPoint3d& location, UtlId withId )
: MachLogMachine( MachPhys::GEO_LOCATOR, hwLevel, swLevel, pRace,
                  pNewPhysGeoLocator( hwLevel, swLevel, pRace, location ), MachLog::GEO_LOCATOR, withId )
{
	objectType( MachLog::GEO_LOCATOR);
	hp( data().hitPoints() );
	armour( data().armour() );
	ctl_append( &MachLogRaces::instance().geoLocators( pRace->race() ), this );
}

MachLogGeoLocator::~MachLogGeoLocator()
{
	/* Intentionally Empty	*/
}

PhysRelativeTime MachLogGeoLocator::locate()
{
	return 1.0;
}

/* //////////////////////////////////////////////////////////////// */

void MachLogGeoLocator::doOutputOperator( ostream& o ) const
{
	MachLogMachine::doOutputOperator( o );
}

/* //////////////////////////////////////////////////////////////// */
//static
MachPhysGeoLocator* MachLogGeoLocator::pNewPhysGeoLocator
(
    Level hwLevel, Level swLevel, MachLogRace * pRace, const MexPoint3d& location
)
{
    //get domain and transform to use
    MexTransform3d localTransform;
    W4dDomain* pDomain =
        MachLogPlanetDomains::pDomainPosition( location, 0, &localTransform );

    //Construct the physical machine
    return _NEW( MachPhysGeoLocator( pDomain, localTransform, hwLevel, swLevel, pRace->race() ) );
}
/* //////////////////////////////////////////////////////////////// */

MachPhysGeoLocator& MachLogGeoLocator::physGeoLocator()
{
    return _STATIC_CAST( MachPhysGeoLocator&, physObject() );
}
//////////////////////////////////////////////////////////////////////////////////////////

const MachPhysGeoLocator& MachLogGeoLocator::physGeoLocator() const
{
    return _STATIC_CAST( const MachPhysGeoLocator&, physObject() );
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogGeoLocator::isLocating( bool doLocate )
{
	if( doLocate != isLocating() )
	{
		MachLogNetwork& network = MachLogNetwork::instance();
		if( network.isNetworkGame() and network.remoteStatus( race() ) == MachLogNetwork::LOCAL_PROCESS )
			network.messageBroker().sendPlayNormalObjectAnimationMessage( id(), doLocate );
	    physGeoLocator().isLocating( doLocate );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////

bool MachLogGeoLocator::isLocating() const
{
    return physGeoLocator().isLocating();
}
//virtual
const MachPhysMachineData& MachLogGeoLocator::machineData() const
{
	return data();
}

const MachPhysGeoLocatorData& MachLogGeoLocator::data() const
{
	//return  _STATIC_CAST( MachPhysGeoLocatorData&, physMachine().machineData() );
	return  _STATIC_CAST( const MachPhysGeoLocatorData&, physMachine().machineData() );
}


void perWrite( PerOstream& ostr, const MachLogGeoLocator& actor )
{
	const MachLogMachine& base1 = actor;

	ostr << base1;

	MexTransform3d trans = actor.globalTransform();

    //If we don't do this, the large coords of the transform cause are used when read back in to find
    //a planet domain, which of course falls over big time.
    if( actor.insideAPC() )
        trans.position( MexPoint3d(10,10,0) );

	PER_WRITE_RAW_OBJECT( ostr, trans );
	const W4dId id = actor.id();
	PER_WRITE_RAW_OBJECT( ostr, id );
	actor.persistenceWriteStrategy( ostr );
}

void perRead( PerIstream& istr, MachLogGeoLocator& actor )
{
	MachLogMachine& base1 = actor;

	istr >> base1;

	MexTransform3d trans;
	W4dId id;
	PER_READ_RAW_OBJECT( istr, trans );
	PER_READ_RAW_OBJECT( istr, id );

	MachPhysGeoLocator* pPhysGeoLocator = MachLogGeoLocator::pNewPhysGeoLocator( base1.hwLevel(), base1.swLevel(), &actor.logRace(), trans.position() );
	actor.setObjectPtr( pPhysGeoLocator, trans );
	actor.id( id );
	actor.machineCreated();
	actor.persistenceReadStrategy( istr );
}

MachLogGeoLocator::MachLogGeoLocator( PerConstructor con )
:	MachLogMachine( con )
{
}


//////////////////////////////////////////////////////////////////////////////////////////
/* End LOCATOR.CPP **************************************************/
