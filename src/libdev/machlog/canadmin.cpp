 /*
 * C A N A D M I N . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "ctl/list.hpp"
#include "ctl/algorith.hpp"

#include "phys/cspace2.hpp"

#include "machphys/random.hpp"
#include "machphys/machdata.hpp"
#include "machphys/station.hpp"
#include "machphys/stations.hpp"

#include "machlog/canadmin.hpp"
#include "machlog/races.hpp"
#include "machlog/mine.hpp"
#include "machlog/opconstr.hpp"
#include "machlog/seekdest.hpp"
#include "machlog/move.hpp"
#include "machlog/machine.hpp"
#include "machlog/garrison.hpp"
#include "machlog/findspac.hpp"
#include "machlog/inout.hpp"
#include "machlog/planet.hpp"
#include "machlog/rescarr.hpp"
#include "machlog/squad.hpp"
#include "machlog/apc.hpp"
#include "machlog/construc.hpp"
#include "machlog/aggressr.hpp"
#include "machlog/administ.hpp"
#include "machlog/pod.hpp"
#include "machlog/strategy.hpp"
#include "machlog/spacial.hpp"
#include "machlog/mcmotseq.hpp"

PER_DEFINE_PERSISTENT_ABSTRACT( MachLogCanAdminister );

MachLogCanAdminister::MachLogCanAdminister( MachPhys::Race race )
:	canAdminRace_( race )
{

    TEST_INVARIANT;
}

//virtual
MachLogCanAdminister::~MachLogCanAdminister()
{
    TEST_INVARIANT;

}

void MachLogCanAdminister::CLASS_INVARIANT
{
	INVARIANT( this != NULL );
}

//virtual
void MachLogCanAdminister::handleIdleSpyLocator( MachLogCommsId obj )
{
	moveOutOfTheWay( &obj->asMachine() );
	return;
}

//virtual
void MachLogCanAdminister::handleIdleGeoLocator( MachLogCommsId obj )
{
	moveOutOfTheWay(  &obj->asMachine()  );
	return;
/*
	if( obj->administrator() )
		if( MachLogRaces::instance().objectExists( (MachActor*)obj->administrator() ) )
			return;

	bool found  = false;
	// has the Locator discovered a site?
	MachLogMineralSites::iterator i =  MachLogMineralSites::instance().sites( race() ).begin();
	for( ; i != MachLogMineralSites::instance().sites( race() ).end() and not found; ++i )
	{
		found = ( obj->position() == (*i).position() and not (*i).hasBeenDiscoveredBy( race() ) );
		if( found )
			(*i).beDiscoveredBy( race() );
	}

	// Locator moves on to the next site...

	found  = false;
	MachLogMineralSites::const_iterator j =  MachLogMineralSites::instance().sites( race() ).begin();
	for( ; j != MachLogMineralSites::instance().sites( race() ).end() and not found; ++j )
	{
		found = not (*j).hasBeenDiscoveredBy( race() );
		if( found )
			obj->newOperation( _NEW( MachLocateOperation( _STATIC_CAST( MachLogLocator *, obj ) , (*j).position() ) ) );
	}

	if( not found )
		moveOutOfTheWay( obj );
*/
}

/* //////////////////////////////////////////////////////////////// */


//virtual
void MachLogCanAdminister::handleIdleConstructor( MachLogCommsId pObj )
{
	MachLogConstructor* obj = &pObj->asConstructor();
	if( obj->squadron() and obj->squadron()->hasCommander() )
		return;

	bool found = false;

	MachLogRaces::Mines& mines = MachLogRaces::instance().mines( obj->race() );
	MachLogRaces::Mines::iterator i = mines.begin();
	for( ; i != mines.end() and not found; ++i )
	{
		found = not (*i)->isComplete();
		MexPoint2d p( (*i)->position().x(), (*i)->position().y() );
		if( found )
			obj->newOperation( _NEW( MachLogConstructOperation( & obj->asConstructor(), *i ) ) );
	}

	if( not found )
		moveOutOfTheWay( obj );
}

/* //////////////////////////////////////////////////////////////// */

//virtual
void MachLogCanAdminister::handleIdleAggressor( MachLogCommsId pObj )
{
	MachLogAggressor* obj = &pObj->asAggressor();
	if( obj->squadron() )
	{
		if( obj->squadron()->hasCommander() )
			return;
		if( obj->squadron()->strategy().currentOperationType() == MachLogOperation::TASK_ATTACK_OPERATION )
			return;
	}
//	obj->newOperation( _NEW( MachSeekAndDestroyOperation( _STATIC_CAST( MachLogAggressor *, obj ) ) ) );
//	if( MachLogRaces::instance().aggressors( obj->race() ).size() < 5 )
		moveOutOfTheWay( obj );
//	else
//		obj->newOperation( _NEW( MachLogSeekAndDestroyOperation( obj ) ) );
}

/* //////////////////////////////////////////////////////////////// */

// config space is subtractive
// containsLine will do for firing...

//virtual
void MachLogCanAdminister::handleIdleAdministrator( MachLogCommsId pObj )
{
	MachLogAdministrator* obj = &pObj->asAdministrator();
	if( obj->squadron() )
	{
		if( obj->squadron()->hasCommander() )
			return;
		if( obj->squadron()->strategy().currentOperationType() == MachLogOperation::TASK_ATTACK_OPERATION )
			return;
	}
	moveOutOfTheWay( &obj->asMachine() );
/*	ctl_list< MexPoint2d > path;

	if( race() == MachPhys::RED )
	{
		ctl_append( &path, MexPoint2d( 540, 150 ) );
		ctl_append( &path, MexPoint2d( 640, 200 ) );
		ctl_append( &path, MexPoint2d( 540, 300 ) );
	}
	else if( race() == MachPhys::BLUE )
	{
		ctl_append( &path, MexPoint2d( 900, 800 ) );
		ctl_append( &path, MexPoint2d( 930, 700 ) );
		ctl_append( &path, MexPoint2d( 350, 900 ) );
	}

	obj->newOperation( _NEW( MachPatrolOperation( _STATIC_CAST( MachLogAdministrator *, obj ), path ) ) );
	*/
}

/* //////////////////////////////////////////////////////////////// */

//virtual
void MachLogCanAdminister::handleIdleResourceCarrier( MachLogCommsId pObj )
{
	MachLogMachine* obj = &pObj->asMachine();
	HAL_STREAM("(" << obj->id() << ") MachLogCanAdminister::handleIdleResourceCarrier" << std::endl );
	if( obj->squadron() and obj->squadron()->hasCommander() )
		return;

	MachLogResourceCarrier::assignResourceCarrierTask( &obj->asResourceCarrier() );
}

//virtual
void MachLogCanAdminister::handleIdleAPC( MachLogCommsId pObj )
{
	MachLogMachine* obj = &pObj->asMachine();
	if( obj->squadron() and obj->squadron()->hasCommander() )
		return;

	MachLogAPC::assignAPCTask( &obj->asAPC() );
}

//
//
//THIS FUNCTION HAS MULTIPLE EXIT POINTS
//
void MachLogCanAdminister::moveOutOfTheWay( MachLogMachine * pObj )
{
	//find closest garrison that isn't full and that I can fit inside.

	MachLogGarrison* pGarrison = NULL;
	MachPhysStation* pStation = NULL;
	MachLogRaces& races = MachLogRaces::instance();
	bool foundGarrison = races.findFriendlyGarrisonClosestTo( *pObj, &pGarrison, &pStation, MachLogRaces::FREE_BAYS_ONLY );

//	ASSERT( not garrisons.empty(), logic_error() );

	//check for already in a garrison.
	if( pObj->insideBuilding() and pObj->insideWhichBuilding().objectType() == MachLog::GARRISON )
		return;

	//if I am an aggressor then I will go to an assembly point of my race.
	if( pObj->objectType() == MachLog::AGGRESSOR )
	{
		int index = MachPhysRandom::randomInt(0, (int)races.aggressorAssemblyPoints( pObj->race() ).size() );
		ASSERT( index < races.aggressorAssemblyPoints( pObj->race() ).size(),"trying to move aggressor to outside n_points\n" );
		MexPoint2d dest = races.aggressorAssemblyPoints( pObj->race() )[ index ];
		//more than 20 metres from target?
		if( pObj->position().sqrEuclidianDistance( dest ) > 400 )
		{
			//do a quick find sapce using the functionality exported by MLSpacial::getNearestFreeSpacePoint
			MexTransform3d trans( dest );
			if( MachLogSpacialManipulation::getNearestFreeSpacePoint( trans, 1, pObj->highClearence(), &dest ) )
			{
				// less than 30m from an assembly point is good enough.
				pObj->newOperation( _NEW( MachLogMoveToOperation( pObj, dest, true, 30.0 ) ) );
			}

		}
		return;
	}

	//if I am an administrator then I will go to the assembly point for my race.
	if( pObj->objectType() == MachLog::ADMINISTRATOR )
	{
		int index = MachPhysRandom::randomInt(0, (int)races.administratorAssemblyPoints( pObj->race() ).size() );
		ASSERT( index < races.administratorAssemblyPoints( pObj->race() ).size(),"trying to move administrator to outside n_points\n" );
		MexPoint2d dest = races.administratorAssemblyPoints( pObj->race() )[ index ];
		//more than 20 metres from target?

		if( pObj->position().sqrEuclidianDistance( dest ) > 400 )
		{
			//do a quick find sapce using the functionality exported by MLSpacial::getNearestFreeSpacePoint
			MexTransform3d trans( dest );
			if( MachLogSpacialManipulation::getNearestFreeSpacePoint( trans, 1, pObj->highClearence(), &dest ) )
			{
				// less than 30m from an assembly point is good enough.
				pObj->newOperation( _NEW( MachLogMoveToOperation( pObj, dest, true, 30.0 ) ) );
			}
		}
		return;
	}
	int randomBaseChance = ( pObj->objectType() == MachLog::AGGRESSOR or pObj->objectType() == MachLog::ADMINISTRATOR ) ? 5 : 1;
	if( pObj->objectType() == MachLog::GEO_LOCATOR )
		randomBaseChance = 10;
	//If no garrisons try to move to the pod
	//or 30% chance of moving to the pod
	if( not foundGarrison or MachPhysRandom::randomInt( 0, 10 ) < randomBaseChance )
	{
		if( not races.pods( pObj->race() ).empty() )
		{
			MexPoint2d entrance = (*races.pods( pObj->race() ).begin() )->entranceExternalPoint( 0 );

			MexPoint2d dest;
			if( pObj->position().sqrEuclidianDistance( entrance ) > 3600 )
			{
				MexPoint2d actorPos( pObj->position().x(), pObj->position().y() );
				MexTransform3d trans( entrance );
				if( MachLogSpacialManipulation::getNearestFreeSpacePoint( trans, 30, pObj->highClearence(), &dest, 30 ) )
				{
					// less than 50m from an assembly point is good enough.
					pObj->newOperation( _NEW( MachLogMoveToOperation( pObj, dest, true, 60.0 ) ) );
				}
			}
		}
		else
		//No pods! Shouldn't happen but you never know...
		{
			MexPoint2d dest = races.administratorAssemblyPoints( pObj->race() )[ 0 ];
			//do a quick find sapce using the functionality exported by MLSpacial::getNearestFreeSpacePoint
			MexTransform3d trans( dest );
			if( MachLogSpacialManipulation::getNearestFreeSpacePoint( trans, 1, pObj->highClearence(), &dest ) )
			{
				// less than 30m from an assembly point is good enough.
				pObj->newOperation( _NEW( MachLogMoveToOperation( pObj, dest, true, 30.0 ) ) );
			}
		}

		return;
	}

	// otherwise, let's go to that garrison we found earlier.

	pObj->newOperation( _NEW( MachLogEnterBuildingOperation( pObj, pGarrison, pStation ) ) );
}


void MachLogCanAdminister::moveOutOfTheWay( MachLogMachine * pObj, int assemblyPoint )
{
	//if the assembly point exists then ALWAYS use it. if not then use normal move out of the way function.
	if( assemblyPoint < MachLogRaces::instance().aggressorAssemblyPoints( pObj->race() ).size() )
	{
		MexPoint2d dest = MachLogRaces::instance().aggressorAssemblyPoints( pObj->race() )[ assemblyPoint ];
		//more than 20 metres from target?
		if( pObj->position().sqrEuclidianDistance( dest ) > 400 )
		{
			MATHEX_SCALAR radius = 0;
			MexTransform3d trans( dest );
			while( not MachLogSpacialManipulation::getNearestFreeSpacePoint( trans, radius, pObj->highClearence(),
				&dest ) )
				radius += 20;

			// less than 30m from an assembly point is good enough.
			pObj->newOperation( _NEW( MachLogMoveToOperation( pObj, dest, true, 30.0 ) ) );
		}
	}
	else
		moveOutOfTheWay( pObj );
}

ostream& operator <<( ostream& o, const MachLogCanAdminister& t )
{

    o << "MachLogCanAdminister " << (void*)&t << " start" << std::endl;
    o << "MachLogCanAdminister " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachLogCanAdminister& canAdmin )
{
	ostr << canAdmin.canAdminRace_;
}

void perRead( PerIstream& istr, MachLogCanAdminister& canAdmin )
{
	istr >> canAdmin.canAdminRace_;
}

MachLogCanAdminister::MachLogCanAdminister( PerConstructor )
{
}

/* End CANADMIN.CPP *************************************************/
