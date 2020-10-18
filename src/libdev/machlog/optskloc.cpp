/*
 * O P T S K L O C . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include "ctl/pvector.hpp"

#include "mathex/point2d.hpp"

#include "phys/cspace2.hpp"

#include "machphys/machdata.hpp"
#include "machphys/random.hpp"

#include "machlog/opadloc.hpp"
#include "machlog/oplocate.hpp"
#include "machlog/optskloc.hpp"
#include "machlog/squad.hpp"
#include "machlog/races.hpp"
#include "machlog/cntrl_ai.hpp"
#include "machlog/administ.hpp"
#include "machlog/planet.hpp"
#include "machlog/geolocat.hpp"
#include "machlog/minesite.hpp"
#include "machlog/strategy.hpp"
#include "machlog/pod.hpp"

/* //////////////////////////////////////////////////////////////// */
PER_DEFINE_PERSISTENT( MachLogTaskLocateOperation );

MachLogTaskLocateOperation::MachLogTaskLocateOperation( MachLogSquadron * pActor, int nDesiredSites, MATHEX_SCALAR maxRangeFromPod )
:	MachLogTaskOperation( "TASK_LOCATE_OPERATION" , MachLogOperation::TASK_LOCATE_OPERATION ),
	pActor_( pActor ),
	complete_( false ),
	nDesiredSites_( nDesiredSites ),
	maxRangeFromPod_( maxRangeFromPod )
{
}

MachLogTaskLocateOperation::~MachLogTaskLocateOperation( )
{
}


void MachLogTaskLocateOperation::doOutputOperator( ostream& o ) const
{
	o << "MachLogTaskLocateOperation" << std::endl;
    o << "complete_ " << complete_ << " max range from pod " << maxRangeFromPod_ << std::endl;
	o << "nDesiredSites_ " << nDesiredSites_ << " totalWeDiscovered " << totalWeDiscovered_ << " totalDiscovered " << totalDiscovered_ << " totalPossibleToDiscover_ " << totalPossibleToDiscover_ << std::endl;
}

///////////////////////////////////

bool MachLogTaskLocateOperation::doStart()
{
	return true;
}

PhysRelativeTime MachLogTaskLocateOperation::doUpdate( )
{
	if( complete_ )
		return 60;

    //Check we have a locator
	MachLogGeoLocator *pLoc = NULL;
	for( MachLogSquadron::Machines::iterator i = pActor_->machines().begin();
	     i!=pActor_->machines().end(); ++i )
			if( (*i)->objectType() == MachLog::GEO_LOCATOR )
				pLoc = &(*i )->asGeoLocator();

	HAL_STREAM(" (void*)pLoc " << (void*)pLoc << std::endl );
    //If not, set longer call back as owing squad will be trying to create a locator.
    if( pLoc == NULL )
    {
        return 10.0;
    }

	if( not pActor_->hasCommander() )
	{
		//oops no commander for squadron. This leaves the locator without a brain.
		//if it is currently locating them leave it alone.
		if( pLoc->strategy().currentOperationType() == MachLogOperation::LOCATE_OPERATION )
			return 10.0;
		//locator is not currently locating...issue moveOutOfWay operation -
		//this will hopefully bring squadron back to safty
//		MachLogRaces::instance().controller( pActor_->race() ).moveOutOfTheWay( pLoc );
//		return 50.0;
	}

	//right we know we have a locator (at the minimum) (possibly administrator as well).

	//check composition of squadron and what they are doing.
	//if any machines are doing anything then we can wait.
	//if all machines are idling or FOLLOWING then we can assign task

	int index = 0;
	bool ok = true;
	for( MachLogSquadron::Machines::iterator i = pActor_->machines().begin();
	     i != pActor_->machines().end(); ++i )
		if( (*i)->id() != pActor_->id() and ( (*i)->isIdle() or (*i)->strategy().currentOperationType() == MachLogOperation::FOLLOW_OPERATION ) )
		{
		}
		else
		{
			ok = false;
		}

	if( not ok )
	{
		//at least one machine is doing something.
		//return in a few seconds and try again.
		return 15.0;
	}

	//Everybody is idle so now we have to select a suitable location.
	//we know we are not complete so there must be at least one mineral site within range
	//of the Pod. (As defined by maxRangeToPod ).
	//All we do here is find it and give it as a parameter to the commander of the squadron.
	bool found  = false;
	bool foundAGoodOne = false;
	MATHEX_SCALAR range = 1000000000;
	MATHEX_SCALAR rangeClosest = 1000000000;
	MachLogPlanet::Sites::const_iterator i =  MachLogPlanet::instance().sites().begin();
	MachLogPlanet::Sites::const_iterator j =  MachLogPlanet::instance().sites().begin();
	MexPoint2d checkFromPos = pActor_->hasCommander() ? pActor_->commander().position() : pActor_->machines().front()->position();
	for( ; j != MachLogPlanet::instance().sites().end(); ++j )
	{
		found = not (*j)->hasBeenDiscovered();
		range = checkFromPos.euclidianDistance( (*j)->position() );
		HAL_STREAM(" checking mineral sites range " << range << " amount of ore " << (*j)->amountOfOre() << std::endl );
		if( found and range < rangeClosest and (*j)->amountOfOre() > 0 and range < maxRangeFromPod_ )
		{
			HAL_STREAM("  saving this one for later...\n" );
			rangeClosest = range;
			i = j;
			foundAGoodOne = true;
		}
	}
	if( foundAGoodOne )
	{
		HAL_STREAM(" found a good one...assigning admin locate op pos " << (*i)->position() << std::endl );
		MexPoint3d dest( (*i)->position() );

		if( pActor_->hasCommander() )
			pActor_->commander().newOperation( _NEW( MachLogAdminLocateOperation( &pActor_->commander() , dest ) ) );
		else
		{
		    //Check we have a locator
			MachLogGeoLocator *pLoc = NULL;
			for( MachLogSquadron::Machines::iterator i = pActor_->machines().begin();
			     i!=pActor_->machines().end(); ++i )
					if( (*i)->objectType() == MachLog::GEO_LOCATOR )
					{
						pLoc = &(*i )->asGeoLocator();
						pLoc->newOperation( _NEW( MachLogLocateOperation( pLoc, dest ) ) );
					}
		}
	}

	return 25;
}

void MachLogTaskLocateOperation::doFinish()
{
}

bool MachLogTaskLocateOperation::doIsFinished() const
{
	//MachLogTaskLocateOperation* pNonConstThis = _STATIC_CAST( MachLogTaskLocateOperation*, this );
	MachLogTaskLocateOperation* pNonConstThis = _CONST_CAST( MachLogTaskLocateOperation*, this );
	return pNonConstThis->setCompleteState();
}

bool MachLogTaskLocateOperation::setCompleteState()
{
	totalWeDiscovered_ = 0;
	totalDiscovered_ = 0;
	totalPossibleToDiscover_ = 0;
	for( MachLogPlanet::Sites::const_iterator i = MachLogPlanet::instance().sites().begin();
		i != MachLogPlanet::instance().sites().end(); ++i )
	{
		if( (*i)->hasBeenDiscovered() )
		{
			++totalDiscovered_;
			if( (*i)->hasBeenDiscoveredBy( pActor_->race() ) )
				++totalWeDiscovered_;
		}
		else
		{
			if( MachLogRaces::instance().pods( pActor_->race() ).size() == 0 )
				++totalPossibleToDiscover_;
			else
			{
				MexPoint3d podPosition( MachLogRaces::instance().pods( pActor_->race() ).front()->position() );
				if( podPosition.euclidianDistance( (*i)->position() ) < maxRangeFromPod_ )
					++totalPossibleToDiscover_;
			}
		}
	}

	if( totalWeDiscovered_ >= nDesiredSites_ or
		totalPossibleToDiscover_ == 0 )
		complete_ = true;
	if( complete_ )
	{
		for( MachLogSquadron::Machines::iterator i = pActor_->machines().begin();
		     i!=pActor_->machines().end(); ++i )
				(*i)->strategy().tryToRemoveAllOperations();

		pActor_->releaseAllMachines();
		pActor_->removeAllDesiredMachines();
	}

	return complete_;
}

bool MachLogTaskLocateOperation::doBeInterrupted()
{
	return true;
}

void perWrite( PerOstream& ostr, const MachLogTaskLocateOperation& op )
{
	const MachLogTaskOperation& base1 = op;

	ostr << base1;
	ostr << op.pActor_;
    ostr << op.complete_;
	ostr << op.maxRangeFromPod_;
	ostr << op.nDesiredSites_;
	ostr << op.totalWeDiscovered_;
	ostr << op.totalDiscovered_;
	ostr << op.totalPossibleToDiscover_;
}

void perRead( PerIstream& istr, MachLogTaskLocateOperation& op )
{
	MachLogTaskOperation& base1 = op;

	istr >> base1;
	istr >> op.pActor_;
    istr >> op.complete_;
	istr >> op.maxRangeFromPod_;
	istr >> op.nDesiredSites_;
	istr >> op.totalWeDiscovered_;
	istr >> op.totalDiscovered_;
	istr >> op.totalPossibleToDiscover_;
}

MachLogTaskLocateOperation::MachLogTaskLocateOperation( PerConstructor con )
:	MachLogTaskOperation( con )
{
}

/* //////////////////////////////////////////////////////////////// */
