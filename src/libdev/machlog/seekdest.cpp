/*
 * S E E K D E S T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include <iostream>
#include "ctl/pvector.hpp"
#include "sim/manager.hpp"
#include "phys/motchunk.hpp"
#include "machphys/machdata.hpp"
#include "machlog/aggressr.hpp"
#include "machlog/administ.hpp"
#include "machlog/races.hpp"
#include "machlog/seekdest.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/convyoff.hpp"
#include "machlog/follow.hpp"
#include "machlog/attack.hpp"
#include "machlog/inout.hpp"
#include "machlog/squad.hpp"
#include "machlog/constron.hpp"

PER_DEFINE_PERSISTENT( MachLogSeekAndDestroyOperation );
/* //////////////////////////////////////////////////////////////// */

MachLogSeekAndDestroyOperation::MachLogSeekAndDestroyOperation( MachLogMachine * pActor,
							MachLog::TargetSystemType targetSystemType,
							MachLog::ObjectType objectType,
							const MexPoint3d& searchStartingPoint )
:	MachLogOperation( "SEEK_AND_DESTROY_OPERATION" , MachLogOperation::SEEK_AND_DESTROY_OPERATION ),
	pActor_( pActor ),
	currentlyAttached_(false),
	pCurrentTarget_( NULL ),
	targetSystemType_( targetSystemType ),
	objectType_( objectType ),
	searchStartingPoint_( searchStartingPoint ),
	commandId_( MachLogRaces::instance().AINoCommandID() )

{
	/* Intentionally Empty	*/
}

MachLogSeekAndDestroyOperation::MachLogSeekAndDestroyOperation( MachLogMachine * pActor,
							size_t commandId,
							MachLog::TargetSystemType targetSystemType,
							MachLog::ObjectType objectType,
							const MexPoint3d& searchStartingPoint )
:	MachLogOperation( "SEEK_AND_DESTROY_OPERATION" , MachLogOperation::SEEK_AND_DESTROY_OPERATION ),
	pActor_( pActor ),
	currentlyAttached_(false),
	pCurrentTarget_( NULL ),
	targetSystemType_( targetSystemType ),
	objectType_( objectType ),
	searchStartingPoint_( searchStartingPoint ),
	commandId_( commandId )

{
	/* Intentionally Empty	*/
}

MachLogSeekAndDestroyOperation::~MachLogSeekAndDestroyOperation()
{
	if( currentlyAttached_ )
	{
		pCurrentTarget_->detach( this );
	}
}

bool MachLogSeekAndDestroyOperation::doStart()
{
	bool result = not checkNeedAndDoLeaveOperation( pActor_ );
	return result;
}

/* //////////////////////////////////////////////////////////////// */

bool findTargetClosestTo( const MachLogMachine& actor,
											MachActor ** ppResult )
{
	bool found = false;

	found = MachLogRaces::instance().findTargetClosestTo( actor, ppResult );

	return found;
}

PhysRelativeTime MachLogSeekAndDestroyOperation::doUpdate()
{
	MachActor * pTarget = NULL;
	bool found = MachLogRaces::instance().findTargetClosestTo( *pActor_, &pTarget, targetSystemType_, objectType_, searchStartingPoint_);

	if( found and pTarget->objectIsMachine() )
	{
		// our target is a machine. Therefore, it might be inside a building.
		MachLogMachine& targetMachine = pTarget->asMachine();

		found = canAttackEvenIfInsideBuilding( targetMachine );

		if( not found and targetSystemType_ == MachLog::TARGET_NORMAL )
		{
			found = MachLogRaces::instance().findAlternativeTargetClosestTo( *pActor_, &pTarget );

			if( found and  pTarget->objectIsMachine() )
			{
				MachLogMachine& alternativeTargetMachine = pTarget->asMachine();

				found = canAttackEvenIfInsideBuilding( alternativeTargetMachine );
			}
		}

		if( found )
		{
			if( pActor_->objectType() == MachLog::AGGRESSOR )
			{
				aggressorUpdateWithTarget( pTarget );
			}
			else
			{
				ASSERT( pActor_->objectType() == MachLog::ADMINISTRATOR, "Attacking machine was neither aggressor nor administrator!" )

				administratorUpdateWithTarget( pTarget );
			}
		}
	}

	return 2.0;
}

void MachLogSeekAndDestroyOperation::doFinish()
{
	//HAL_STREAM("(" << pActor_->id() << ") MLSeekAndDestroyOp::doFinish checking for currently attached\n" );
	if( currentlyAttached_ )
	{
		//HAL_STREAM(" am attached calling detach\n" );
		pCurrentTarget_->detach( this );
	}
}

bool MachLogSeekAndDestroyOperation::doIsFinished() const
{
	// for the milestone 6 deliverable,
	// this operation cycles indefinitely...
	return false;
;
}

void MachLogSeekAndDestroyOperation::doOutputOperator( ostream& o ) const
{
	o << "MachLogSeekAndDestroyOperation" << std::endl;
	if( pCurrentTarget_ )
	{
		o << " CurrentTarget " << pCurrentTarget_->id() << std::endl;
	 	if(  pCurrentTarget_->objectIsMachine() )
		{
			MachLogMachine& mlm =  pCurrentTarget_->asMachine();
			if( mlm.insideBuilding() )
				o << " inside building " << mlm.insideWhichBuilding().id() << std::endl;
		}
	}
	else
		o << " No current Target.\n";

	if( pActor_->objectType() == MachLog::AGGRESSOR )
	{
		MachLogAggressor* pAgg = & pActor_->asAggressor();
		o << " machine current target ";
		if( pAgg->hasCurrentTarget() )
		{
			o << " (" << pAgg->currentTarget().id() << ") [" << pAgg->currentTarget().objectType() << "] " << std::endl;
		}
		else
			o << ": None Selected.\n";
	}
	if( pSubOperation() )
		o << " there is a sub operation active\n";
	else
		o << " no current sub operation.\n";

}

bool MachLogSeekAndDestroyOperation::doBeInterrupted()
{
	pActor_->motionSeq().stop();
	return true;
}

//virtual
bool MachLogSeekAndDestroyOperation::beNotified( W4dSubject* pSubject,
						                         W4dSubject::NotificationEvent event, int /*clientData*/ )
{
	ASSERT( pSubject == (W4dSubject*)pCurrentTarget_, "Notified by unexpected subject!" );

	bool stayAttached = true;

	switch( event )
	{
	case W4dSubject::DELETED:
		currentlyAttached_ = false;
		pCurrentTarget_ = NULL;
		nextCallBackTime( SimManager::instance().currentTime() );
		stayAttached = false;
		break;
	default:
		;
	}

	return stayAttached;
}

void perWrite( PerOstream& ostr, const MachLogSeekAndDestroyOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pActor_;
	ostr << op.currentlyAttached_;
	ostr << op.pCurrentTarget_;
	ostr << op.searchStartingPoint_;
	ostr << op.targetSystemType_;
	ostr << op.objectType_;
}

void perRead( PerIstream& istr, MachLogSeekAndDestroyOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pActor_;
	istr >> op.currentlyAttached_;
	istr >> op.pCurrentTarget_;
	istr >> op.searchStartingPoint_;
	istr >> op.targetSystemType_;
	istr >> op.objectType_;
	if( op.currentlyAttached_ )
		op.pCurrentTarget_->attach( &op );
}

MachLogSeekAndDestroyOperation::MachLogSeekAndDestroyOperation( PerConstructor con )
:	MachLogOperation( con )
{
}

void MachLogSeekAndDestroyOperation::aggressorUpdateWithTarget( MachActor* pTarget )
{
	MachLogAggressor& aggressor = pActor_->asAggressor();

	if( not pSubOperation() or not aggressor.hasCurrentTarget() or ( aggressor.hasCurrentTarget() and not aggressor.currentTarget().isDead() and aggressor.currentTarget().id() != pTarget->id() ) )
	{
		if( currentlyAttached_ )
		{
			pCurrentTarget_->detach( this );
		}
		pTarget->attach( this );
		currentlyAttached_ = true;
		pCurrentTarget_ = pTarget;

		if( pTarget->objectIsMachine() )
		{
			//check to see if the target is inside a different building to us.
			//we of course may not be in a building at all.
			MachLogMachine& targetMachine =  pTarget->asMachine();

			if( targetMachine.insideBuilding()
				and ( not aggressor.insideBuilding() or aggressor.insideWhichBuilding().id() != targetMachine.insideWhichBuilding().id() ) )
			{
				MachLogConstruction& construction =  targetMachine.insideWhichBuilding();
				// our target is inside a building which we're not inside. Move to chase it if possible.
				if( aggressor.canEnterConstructionNow( targetMachine.insideWhichBuilding() ) )
				{
					pActor_->motionSeq().stop();
					subOperation( &aggressor, _NEW( MachLogEnterBuildingOperation( &aggressor, &construction, NULL ) ) );
				}
				else
				{
					// I can't enter the building. So, if it's an enemy building, let's blow the building up instead.
					if( construction.race() != aggressor.race() )
					{
						pActor_->motionSeq().stop();
						subOperation(  &aggressor, _NEW( MachLogAttackOperation( &aggressor, &construction, commandId_ ) ) );
					}
				}
			}
			else
				subOperation( &aggressor, _NEW( MachLogAttackOperation( &aggressor, pTarget, commandId_ ) ) );
		}
		else
			subOperation( &aggressor, _NEW( MachLogAttackOperation( &aggressor, pTarget, commandId_ ) ) );
	}
}

void MachLogSeekAndDestroyOperation::administratorUpdateWithTarget( MachActor* pTarget )
{
	MachLogAdministrator& administrator  = pActor_->asAdministrator();

	if( administrator.squadron() )
	{
		MachLogSquadron::Machines oblist = administrator.squadron()->machines();

		// only look to issue attack ops to underlings if I myself (the squadron commander) am now in scanner range
		// of the target
		bool issueAttack = pActor_->position().euclidianDistance( pTarget->position() ) > pActor_->effectiveScannerRange();

		int convoyIndex = 0;
		for( MachLogSquadron::Machines::iterator i = administrator.squadron()->machines().begin(); i!= administrator.squadron()->machines().end(); ++i )
		{
			MachLogMachine& squadronMachine = (**i);
			MachLog::ObjectType obType = squadronMachine.objectType();
			if( obType == MachLog::AGGRESSOR
				or( obType == MachLog::ADMINISTRATOR and squadronMachine.id() != pActor_->id() ) )   // administrator shouldn't issue orders to itself
			{
				// convoyIndex may or may not be incremented depending on whether the issuing of the order
				// resulted in squadronMachine joining the convoy
				convoyIndex = issueOrderToSquadronMachine( &squadronMachine, pTarget, issueAttack, convoyIndex );
			}
		}
	}

	// administrator who leads the gang should be given a standard attack op
	if( not pSubOperation() )
		subOperation( &administrator, _NEW( MachLogAttackOperation( pActor_, pTarget, commandId_ ) ) );
}

int MachLogSeekAndDestroyOperation::issueOrderToSquadronMachine( MachLogMachine* pSquadronMachine,
																 MachActor* pTarget,
																 bool issueAttack,
																 int convoyIndex )
{
	if( pSquadronMachine->motionSeq().isFollowing() or ( not pSquadronMachine->motionSeq().hasDestination() ) )
	{
		if( issueAttack )
		{
			pSquadronMachine->motionSeq().stop();
			pSquadronMachine->newOperation( _NEW( MachLogAttackOperation( pSquadronMachine, pTarget, commandId_ ) ) );
		}
		else if( not pSquadronMachine->motionSeq().isFollowing() )
		{
			pSquadronMachine->newOperation( _NEW( MachLogFollowOperation( pSquadronMachine, pActor_ , MachLogConvoyOffsets::convoyOffset( MachLogConvoyOffsets::KILLER_CONVOY, convoyIndex, 20 ) ) ) );
			++convoyIndex;
		}
	}

	return convoyIndex;
}

bool MachLogSeekAndDestroyOperation::canAttackEvenIfInsideBuilding( const MachLogMachine& targetMachine ) const
{
	bool okayToAttack = true;

	if( targetMachine.insideBuilding() )
	{
		// yes, it IS inside a building. If we're able to
		// enter it, or are currently inside it, no problems. Otherwise, sod this as a target.

		const MachLogConstruction& buildingTargetIsInside = targetMachine.insideWhichBuilding();

		okayToAttack = ( pActor_->insideBuilding() and pActor_->insideWhichBuilding().id() == buildingTargetIsInside.id() )
							or pActor_->canEnterConstructionNow( buildingTargetIsInside );
	}

	return okayToAttack;
}

/* //////////////////////////////////////////////////////////////// */

/* End SEEKDEST.CPP ***************************************************/
