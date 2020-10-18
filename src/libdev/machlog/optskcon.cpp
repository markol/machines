/*
 * O P T S K L O C . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include "ctl/list.hpp"
#include "ctl/pvector.hpp"

#include "mathex/eulerang.hpp"
#include "mathex/abox3d.hpp"
#include "mathex/cvexpgon.hpp"

#include "phys/cspace2.hpp"

#include "machphys/machdata.hpp"
#include "machphys/random.hpp"
#include "machphys/constron.hpp"
#include "machphys/consdata.hpp"

#include "machlog/actmaker.hpp"
#include "machlog/administ.hpp"
#include "machlog/cntrl_ai.hpp"
#include "machlog/constron.hpp"
#include "machlog/construc.hpp"
#include "machlog/machine.hpp"
#include "machlog/opadcon.hpp"
#include "machlog/opconstr.hpp"
#include "machlog/optskcon.hpp"
#include "machlog/planet.hpp"
#include "machlog/produnit.hpp"
#include "machlog/races.hpp"
#include "machlog/squad.hpp"
#include "machlog/strategy.hpp"
#include "machlog/spacial.hpp"
#include "machlog/mine.hpp"

PER_DEFINE_PERSISTENT( MachLogTaskConstructOperation );
/* //////////////////////////////////////////////////////////////// */

MachLogTaskConstructOperation::MachLogTaskConstructOperation( MachLogSquadron * pActor )
:	MachLogTaskOperation( "TASK_CONSTRUCT_OPERATION" , MachLogTaskOperation::TASK_CONSTRUCT_OPERATION ),
	pActor_( pActor ),
	complete_( false )
{
}

MachLogTaskConstructOperation::~MachLogTaskConstructOperation( )
{
}


void MachLogTaskConstructOperation::doOutputOperator( ostream& o ) const
{
	o << "MachLogTaskConstructOperation" << std::endl; 
	o << "AIController:\n";
	MachLogRaces::instance().AIController( pActor_->race() ).doOutputOperator( o );
}

///////////////////////////////////

bool MachLogTaskConstructOperation::doStart()
{
	return true;
}

PhysRelativeTime MachLogTaskConstructOperation::doUpdate( )
{
	setCompleteState();
	//lets just check there are no unfinished buildings that need finishing off first.
	for( MachLogRaces::Objects::iterator i = MachLogRaces::instance().raceObjects( pActor_->race() ).begin();
		i != MachLogRaces::instance().raceObjects( pActor_->race() ).end(); ++i )
	{
		if( (*i)->objectIsConstruction() and not (*i)->asConstruction().isComplete() and (*i)->asConstruction().nConstructors() == 0 )
		{
			if( not pActor_->hasCommander() )
				issueConstructOperation( &(*i)->asConstruction() );
			else
				issueAdminConstructOperation( &(*i)->asConstruction() );
			//we have found something to do so exit now.
			return 15.0;

		}

	}

	//there are no constructions on the list and there are no extra incomplete buildings waiting so may as well get out of the way and exit.
	if( complete_ )
	{
		for( MachLogSquadron::Machines::iterator i = pActor_->machines().begin();
			i != pActor_->machines().end(); ++i )
			{
				if( (*i)->isIdle() )
					MachLogRaces::instance().controller( pActor_->race() ).moveOutOfTheWay( *i );
			}
		return 16;
	}

    //Check we have a constructor
	int idleConstructors = 0;
	MachLogConstructor *pConstructor = NULL;
	for( MachLogSquadron::Machines::iterator i = pActor_->machines().begin();
	     i!=pActor_->machines().end(); ++i )
			if( (*i)->objectType() == MachLog::CONSTRUCTOR )
			{
				pConstructor = &(*i )->asConstructor();
				if( pConstructor->strategy().currentOperationType() != MachLogOperation::CONSTRUCT_OPERATION )
					idleConstructors++;
			}

    //If not, set longer call back as owning squad will be trying to create a constructor.
    if( pConstructor == NULL or idleConstructors == 0 )
    {
        return 10.0;
    }

	MachLogProductionUnit prod;
	bool prodUnitFound = MachLogRaces::instance().AIController( pActor_->race() ).getHighestPriorityConstruction( &prod );
	
	if( not prodUnitFound )
		return 15.0;
		
	if( not pActor_->hasCommander() )
	{
		//oops no commander for squadron. This leaves the constructor without a brain.
		//if it is currently constructing then leave it alone.
//		if( pConstructor->strategy().currentOperationType() == MachLogOperation::CONSTRUCT_OPERATION )
//		{
//			MachLogProductionUnit* newProd = _NEW( MachLogProductionUnit( prod ) );
//			newProd->globalTransform( prod.globalTransform() );
//			MachLogRaces::instance().AIController( pActor_->race() ).addConstructionProductionUnit( newProd );
//			return 15.0;
//		}
		if( prod.type() == MachLog::MINE )
		{
			//if the type is a mine then we need to check that the mine can be placed in a valid site
			//if not then throw THIS PRODUCTION UNIT AWAY;
			if( not MachLogMine::validMineSite( prod.globalTransform().position(), 1, MachLogMine::CHECK_DISCOVERED_FLAG ) )
				return 0;
		}
		MachLogConstruction* pConstruction;
		if( placeConstruction( prod, &pConstruction ) )
		{
			if( prod.needRebuild() )
				pConstruction->needRebuild( true );
			//if the pConstructor actor is currently displayed as then nobble the new construction to be as well.
			if( pConstructor->displayMapAndIconRace() != pConstructor->race() )							
				pConstruction->displayAsRace( pConstructor->displayMapAndIconRace() );

			issueConstructOperation( pConstruction );
		}
		else
		{
			MachLogProductionUnit* newProd = _NEW( MachLogProductionUnit( prod ) );
			newProd->globalTransform( prod.globalTransform() );
			MachLogRaces::instance().AIController( pActor_->race() ).addConstructionProductionUnit( newProd );
		}

		//otherwise issue moveOutOfTheWay operation
//		MachLogRaces::instance().controller( pActor_->race() ).moveOutOfTheWay( pConstructor );
		return 10.0;
	}

	//right we know we have an administrator and a constructor


	//Everybody is idle so now we have to issue admin construct operation at let the commanders AI
	//run the show
	MachLogConstruction* pConstruction;
	HAL_STREAM("Attempt to place construction\n" );
	if( placeConstruction( prod, &pConstruction ) )
	{
		HAL_STREAM(" placeConstruction worked so issuing adminConstructOperation\n" );
		if( prod.needRebuild() )
			pConstruction->needRebuild( true );

		//if the pConstructor actor is currently displayed as then nobble the new construction to be as well.
		if( pConstructor->displayMapAndIconRace() != pConstructor->race() )							
			pConstruction->displayAsRace( pConstructor->displayMapAndIconRace() );
		
		issueAdminConstructOperation( pConstruction );
	}
	else
	{
		HAL_STREAM(" placeConstruction failed so re-issue production unit\n" );
		MachLogProductionUnit* newProd = _NEW( MachLogProductionUnit( prod ) );
		newProd->globalTransform( prod.globalTransform() );
		
		MachLogRaces::instance().AIController( pActor_->race() ).addConstructionProductionUnit( newProd );
	}

	return 7;
}

void MachLogTaskConstructOperation::doFinish()
{
}

bool MachLogTaskConstructOperation::doIsFinished() const
{
	return false;
}

bool MachLogTaskConstructOperation::setCompleteState()
{
	if( MachLogRaces::instance().AIController( pActor_->race() ).hasWaitingConstruction() )
		complete_ = false;
	else
		complete_ = true;

	return complete_;
}

bool MachLogTaskConstructOperation::doBeInterrupted()
{
	return true;
}

bool MachLogTaskConstructOperation::placeConstruction( const MachLogProductionUnit& prod, MachLogConstruction** ppConstruction )
{
	bool isValid = couldPlaceConstruction( prod );
	if( isValid )
	{
		HAL_STREAM(" assigning to ppConstruction:: (void*)(*ppConstruction) before " << (void**)(ppConstruction) << std::endl );
		*ppConstruction = MachLogActorMaker::newLogConstruction( prod.type(), 
								prod.subType(), 
								prod.hwLevel(), 
								prod.globalTransform().position(),
								prod.globalTransform().rotationAsEulerAngles().azimuth(),
								pActor_->race() );
						
		// ensure the new production has the same construction ID value as the produnit that specified its production.
		// this will ensure that alternative site production units for this construction will never be considered
		// while this construction remains in the world.
		(*ppConstruction)->constructionId( prod.constructionId() );
		// as the building race, we need to store our original construction id in case of capture
		// and subsequent recapture
		(*ppConstruction)->originalRaceConstructionId( prod.constructionId() );
		HAL_STREAM(" assigning to ppConstruction:: (void*)(*ppConstruction) After " << (void**)(ppConstruction) << std::endl );
	}

	HAL_STREAM("MLTaskConstructOperation::placeConstruction exit\n" );
	return isValid;
//					MachLogMine * pConstruction_ = _NEW( MachLogMine( pRace, 1, (*i)->position(), MexRadians( 0 ) ) );
}

// static
bool MachLogTaskConstructOperation::couldPlaceConstruction( const MachLogProductionUnit& prod )
{
	HAL_STREAM("MLTaskConstructOperation::placeConstruction\n" );
	MachPhysConstruction* pPhysConstruction = MachLogActorMaker::newPhysConstruction( prod.type(), prod.subType(), prod.hwLevel(),
                                                &MachLogPlanet::instance().hiddenRoot(),
                                                prod.globalTransform(), MachPhys::RED );



    const MexAlignedBox2d globalBorder( pPhysConstruction->largestBoundary() );
	
	HAL_STREAM( "Our global border to check is:" << std::endl << globalBorder << std::endl );

    //Construct a polygon using the global position of the boundary given the
    //current global origin.
	HAL_STREAM(" constructed globalBorder " << globalBorder << std::endl );
    MexConvexPolygon2d polygon( globalBorder );

    //Check contained in the planet's config space
    PhysConfigSpace2d::PolygonId id;
	bool isValid = MachLogPlanet::instance().configSpace().contains( polygon, MachLog::OBSTACLE_NORMAL, &id );
	HAL_STREAM(" isValid " << isValid << std::endl << "calling delete for pointer " << (void*)pPhysConstruction << std::endl );
	_DELETE( pPhysConstruction );
	HAL_STREAM(" delete worked\n" );

	#ifndef PRODUCTION
	if( not isValid )
    {
		HAL_STREAM(" Clashed with poygon id " << id << " " << MachLogPlanet::instance().configSpace().polygon( id ) << std::endl );
    }
	#endif

	if( isValid )
        isValid = not MachLogSpacialManipulation::intersectsWithActor( globalBorder );

	if( isValid )
        isValid = not MachLogSpacialManipulation::intersectsWithMotionChunks( globalBorder );

	return isValid;
//					MachLogMine * pConstruction_ = _NEW( MachLogMine( pRace, 1, (*i)->position(), MexRadians( 0 ) ) );
}


void MachLogTaskConstructOperation::issueConstructOperation( MachLogConstruction* pConstruction )
{
	for( MachLogSquadron::Machines::iterator i = pActor_->machines().begin();
	     i!=pActor_->machines().end(); ++i )
			if( (*i)->objectType() == MachLog::CONSTRUCTOR )
			{
				MachLogConstructor* pConstructor = &(*i )->asConstructor();
				if( pConstructor->strategy().currentOperationType() != MachLogOperation::CONSTRUCT_OPERATION )
				{
					pConstructor->newOperation( _NEW( MachLogConstructOperation( pConstructor, pConstruction ) ) );
				}
			}
}

void MachLogTaskConstructOperation::issueAdminConstructOperation( MachLogConstruction* pConstruction )
{
	pActor_->commander().newOperation( _NEW( MachLogAdminConstructOperation( &pActor_->commander() , pConstruction ) ) );
}

void perWrite( PerOstream& ostr, const MachLogTaskConstructOperation& op )
{
	const MachLogTaskOperation& base1 = op;

	ostr << base1;
	ostr << op.pActor_;
    ostr << op.complete_;
}

void perRead( PerIstream& istr, MachLogTaskConstructOperation& op )
{
	MachLogTaskOperation& base1 = op;

	istr >> base1;
	istr >> op.pActor_;
    istr >> op.complete_;
}

MachLogTaskConstructOperation::MachLogTaskConstructOperation( PerConstructor con )
:	MachLogTaskOperation( con )
{
}

/* //////////////////////////////////////////////////////////////// */
