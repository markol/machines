/*
 * O P T S K P A T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include "ctl/pvector.hpp"

#include "mathex/point2d.hpp"

#include "phys/cspace2.hpp"

#include "machphys/machdata.hpp"
#include "machphys/random.hpp"

#include "sim/manager.hpp"

#include "machlog/administ.hpp"
#include "machlog/patrol.hpp"
#include "machlog/cntrl_ai.hpp"
#include "machlog/optskpat.hpp"
#include "machlog/squad.hpp"
#include "machlog/strategy.hpp"
#include "machlog/races.hpp"

/* //////////////////////////////////////////////////////////////// */
PER_DEFINE_PERSISTENT( MachLogTaskPatrolOperation );

MachLogTaskPatrolOperation::MachLogTaskPatrolOperation( MachLogSquadron * pActor, const MachLogTaskPatrolOperation::Path& path, const PhysRelativeTime& initialDelay )
:	MachLogTaskOperation( "TASK_PATROL_OPERATION" , MachLogTaskOperation::TASK_PATROL_OPERATION ),
	pActor_( pActor ),
	path_( path ),
	complete_( false ),
	initialDelay_( initialDelay ),
	initialDelayProcessed_( false ),
	startTime_( 0.0 )
{
}

MachLogTaskPatrolOperation::~MachLogTaskPatrolOperation( )
{
}

void MachLogTaskPatrolOperation::doOutputOperator( ostream& o ) const
{
	o << "MachLogTaskPatrolOperation" << std::endl;
	o << " path " << path_ << std::endl;
	o << " initial delay " << initialDelay_ << " processed ? " << initialDelayProcessed_ << std::endl;
}

///////////////////////////////////

bool MachLogTaskPatrolOperation::doStart()
{
	return true;
}

PhysRelativeTime MachLogTaskPatrolOperation::doUpdate( )
{
	if( complete_ )
		return 60.0;

	if( not initialDelayProcessed_ )
	{
		if( startTime_ + initialDelay_ < SimManager::instance().currentTime() )
			initialDelayProcessed_ = true;
		else
		{
			for( MachLogSquadron::Machines::iterator i = pActor_->machines().begin();
				i != pActor_->machines().end(); ++i )
				{
					if( (*i)->isIdle() )
						MachLogRaces::instance().controller( pActor_->race() ).moveOutOfTheWay( *i );
				}
			return 30.0;
		}
	}

	if( pActor_->hasCommander() )
		return doUpdateWithCommander();
	return doUpdateWithoutCommander();

}

void MachLogTaskPatrolOperation::doFinish()
{
}

bool MachLogTaskPatrolOperation::doIsFinished() const
{
	return false;
}

bool MachLogTaskPatrolOperation::doBeInterrupted()
{
	return true;
}

PhysRelativeTime MachLogTaskPatrolOperation::doUpdateWithCommander()
{
	MachLogOperation::OperationType commanderOp = pActor_->commander().strategy().currentOperationType();

	if( commanderOp == MachLogOperation::ATTACK_OPERATION or
		commanderOp == MachLogOperation::SEEK_AND_DESTROY_OPERATION or
		commanderOp == MachLogOperation::ADMIN_ATTACK_OPERATION or
		commanderOp == MachLogOperation::PATROL_OPERATION )
		return 22.0;
	//The commander is not doing an aggressive operation...so we can reassign as necessary.
	pActor_->commander().newOperation( _NEW( MachLogPatrolOperation( &pActor_->commander(), path_, true  ) ) );
	return 30;
}

PhysRelativeTime MachLogTaskPatrolOperation::doUpdateWithoutCommander()
{
	for( MachLogSquadron::Machines::iterator i = pActor_->machines().begin();
		i != pActor_->machines().end(); ++i )
		{
			MachLogOperation::OperationType op = (*i)->strategy().currentOperationType();
			if( op != MachLogOperation::ATTACK_OPERATION and
				op != MachLogOperation::PATROL_OPERATION and
				op != MachLogOperation::SEEK_AND_DESTROY_OPERATION )
				(*i)->newOperation( _NEW( MachLogPatrolOperation( (*i), path_, true  ) ) );
		}


	return 30;
}

void perWrite( PerOstream& ostr, const MachLogTaskPatrolOperation& op )
{
	const MachLogTaskOperation& base1 = op;

	ostr << base1;
	ostr << op.pActor_;
    ostr << op.complete_;
	ostr << op.path_;
	ostr << op.initialDelay_;
	ostr << op.initialDelayProcessed_;
	ostr << op.startTime_;
}

void perRead( PerIstream& istr, MachLogTaskPatrolOperation& op )
{
	MachLogTaskOperation& base1 = op;

	istr >> base1;
	istr >> op.pActor_;
    istr >> op.complete_;
	istr >> op.path_;
	istr >> op.initialDelay_;
	istr >> op.initialDelayProcessed_;
	istr >> op.startTime_;
}

MachLogTaskPatrolOperation::MachLogTaskPatrolOperation( PerConstructor con )
:	MachLogTaskOperation( con )
{
}

/* //////////////////////////////////////////////////////////////// */
