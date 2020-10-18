/*
 * P A T R O L . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include <iostream>

#include "ctl/pvector.hpp"
#include "mathex/point2d.hpp"
#include "mathex/transf3d.hpp"
#include "phys/cspace2.hpp"
#include "phys/motchunk.hpp"
#include "machphys/plansurf.hpp"
#include "machphys/machdata.hpp"
#include "machlog/administ.hpp"
#include "machlog/aggressr.hpp"
#include "machlog/machine.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/patrol.hpp"
#include "machlog/attack.hpp"
#include "machlog/planet.hpp"
#include "machlog/races.hpp"
#include "machlog/follow.hpp"
#include "machlog/convyoff.hpp"
#include "machlog/squad.hpp"
#include "machlog/spacial.hpp"
#include "machlog/move.hpp"

PER_DEFINE_PERSISTENT( MachLogPatrolOperation );

MachLogPatrolOperation::MachLogPatrolOperation( MachLogMachine* pActor, const Path& path, const bool& loop )
:	MachLogOperation( "PATROL_OPERATION" , MachLogOperation::PATROL_OPERATION ),
	pActor_( pActor ),
  	path_( path ),
	pathElement_( 0 ),
  	loop_( loop ),
	finished_( false )
{
	//HAL_STREAM("(" << pActor_->id() << ") MLPatrolOperation::CTOR\n" );
	//HAL_STREAM(" path.size " << path_.size() << std::endl );
	//HAL_STREAM(" looping flag " << loop << std::endl );
}

bool MachLogPatrolOperation::doStart()
{
	if( checkNeedAndDoLeaveOperation( pActor_ ) )
		return false;

	finished_ = false;
	pathElement_ = 0;
	return true;

}

PhysRelativeTime MachLogPatrolOperation::doUpdate( )
{
//	static const SQR_PERSUIT_DISTANCE = 180 * 180;
	//if a target is inside scanner range then go blow the shit out of it.
	//HAL_STREAM("(" << pActor_->id() << ") MLPatrolOp::doUpdate\n" );
	HAL_STREAM("MLPatrolOperation::doUpdate\n" );
	int SQR_PERSUIT_DISTANCE = pActor_->effectiveScannerRange();
	SQR_PERSUIT_DISTANCE *= SQR_PERSUIT_DISTANCE;
	bool persueTarget = false;
	MachActor* pTarget = NULL;
	bool doNextMove = true;

	if( pActor_->objectIsCanAttack() )
	{
		//patrolling machines are ALWAYS on full alert.
		pActor_->asCanAttack().setMinimumAlertness( 125 );

		HAL_STREAM("MLPatrolOperation::doUpdate object is can attack\n" );
		if( MachLogRaces::instance().findTargetClosestTo( *pActor_, &pTarget ) )
			if( pActor_->position().sqrEuclidianDistance( pTarget->position() ) < SQR_PERSUIT_DISTANCE )
				persueTarget = true;

		if( persueTarget )
		{
			HAL_STREAM("MLPatrolOperation::doUpdate persue target " << (void*)pTarget << "\n" );
			doNextMove = false;
			subOperation( pActor_, _NEW( MachLogAttackOperation( pActor_, pTarget, MachLogAttackOperation::TERMINATE_ON_CHANGE ) ) );
			//TBD:: add this falg to line above MachLogAttackOperation::TERMINATE_ON_CHANGE
			HAL_STREAM("MLPatrolOperation::doUpdate check for subordinates " << (void*)pTarget << "\n" );
			if( pActor_->objectType() == MachLog::ADMINISTRATOR )
			{
				//if I have any subordinate Aggressors then task them with blowing up the target.
	//			MachLogAdministrator::Objects oblist = pActor_->objects();
				MachLogSquadron* pSquadron = pActor_->squadron();
				if( pSquadron )
				{
					for( MachLogSquadron::Machines::iterator i = pSquadron->machines().begin(); i!= pSquadron->machines().end(); ++i )
						if( (*i)->id() != pActor_->id() and (*i)->objectIsCanAttack() )
						{
							if( (*i)->isIdle() )
								(*i)->newOperation( _NEW( MachLogAttackOperation( (MachLogMachine*)*i, pTarget, MachLogAttackOperation::TERMINATE_ON_CHANGE ) ) );

						}
				}
	  		}
		}
	}
	//if we are currently doing something then exit out here
	if( pSubOperation() )
		return 1.0;

	//if we still want to do next move and we aren't currently moving then...
	if( doNextMove and not pActor_->motionSeq().hasDestination() )
 	{
		if( pathElement_ < path_.size() )
		{
			MexPoint3d p( path_[ pathElement_ ].x(), path_[ pathElement_ ].y(), 0 );
			MexPoint2d dest;
			bool doActualMove = MachLogSpacialManipulation::getNearestFreeSpacePoint( MexTransform3d( p ), 1, pActor_->highClearence(), &dest );
			//HAL_STREAM( "MLPatrolOp::Update Issuing subOperation MachMoveToOp " << p << std::endl );
			if( doActualMove )
				subOperation( pActor_, _NEW( MachLogMoveToOperation( pActor_, dest, true, 15.0 ) ) );	//15m tolerance
			//HAL_STREAM(" if I am admin then give subordinates relevant commands\n" );
			//HAL_STREAM(" checking objectType " << pActor_->objectType() << std::endl );
			if( pActor_->objectType() == MachLog::ADMINISTRATOR )
			{
				//HAL_STREAM(" (void*)pActor_->squadron() " << (void*)pActor_->squadron() << std::endl );
				if( pActor_->squadron() )
				{
					//HAL_STREAM(" there are " << pActor_->squadron()->machines().size() << " subordinates\n" );
					if( pActor_->squadron()->machines().size() > 0 )
					{
						size_t index = 0;
						for( MachLogSquadron::Machines::iterator i = pActor_->squadron()->machines().begin(); i!=pActor_->squadron()->machines().end(); ++i )
							if( (*i)->objectType() == MachLog::AGGRESSOR )
							{
								//This will get replaced with motionsequencer - follow command.
	//							if( (*i)->isIdle() )
	//								(*i)->newOperation( _NEW( MachMoveToOperation( (MachLogMobile*)*i, p ) ) );
	//							if( (*i)->motionSeq().isWaiting() )
	//								(*i)->motionSeq().stop();
								if( (*i)->isIdle() )
								{
									//HAL_STREAM( " Patrol is adding 'follow' for machine " << (*i)->id() << " type=" << (*i)->objectType() << std::endl );
									(*i)->newOperation( _NEW( MachLogFollowOperation( (MachLogMachine*)*i, pActor_ , MachLogConvoyOffsets::convoyOffset( MachLogConvoyOffsets::PATROL_CONVOY, index, 10 ) ) ) );
								}
								++index;
							}
					}
				}
			}
			//HAL_STREAM(" incrementing pathIter ator\n" );
			++pathElement_;
		}
		else
		{
			if( loop_ )
				pathElement_ = 0;
			else
				finished_ = true;
		}
	}
	//HAL_STREAM(" exit.\n" );
	return 1.0;
}

void MachLogPatrolOperation::doFinish()
{
	/* Intentionally Empty	*/
}

bool MachLogPatrolOperation::doIsFinished() const
{
	// for the milestone 6 deliverable,
	// a patrol cycles indefinitely...
	return finished_;
}

void MachLogPatrolOperation::doOutputOperator( ostream& o ) const
{
	//HAL_STREAM("MLPatrolOp::doOutputOperator\n");
	o << "MachPatrolOperation " << std::endl;
	//HAL_STREAM(" displaying: path_.size\n");
	o << " N_Points " << path_.size() << std::endl;
	if( path_.size() > 0 )
	{
		//HAL_STREAM(" displaying: path element\n");
		for( Path::const_iterator i = path_.begin(); i != path_.end() ; ++i )
			o << *i << " ";
	}
	//HAL_STREAM(" displaying: finished flag\n");
	o << " Finished flag " << finished_ << std::endl;
	//HAL_STREAM(" displaying: (void*)pActor\n");
	o << " pActor_ " << (void*)pActor_ << std::endl;
	//HAL_STREAM(" displaying: END\n");


}

bool MachLogPatrolOperation::doBeInterrupted()
{
	pActor_->motionSeq().stop();
	return not pActor_->motionSeq().hasDestination();
}

void perWrite( PerOstream& ostr, const MachLogPatrolOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pActor_;
	ostr << op.path_;
	ostr << op.pathElement_;
	ostr << op.finished_;
	ostr << op.loop_;
}

void perRead( PerIstream& istr, MachLogPatrolOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pActor_;
	istr >> op.path_;
	istr >> op.pathElement_;
	istr >> op.finished_;
	istr >> op.loop_;
}

MachLogPatrolOperation::MachLogPatrolOperation( PerConstructor con )
:	MachLogOperation( con )
{
}


/* //////////////////////////////////////////////////////////////// */

/* End PATROL.CPP ***************************************************/
