/*
 * O P A D A T T A . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include "machlog/opadatta.hpp"

#include "ctl/pvector.hpp"

#include "mathex/point3d.hpp"

#include "phys/motchunk.hpp"

#include "machphys/mcmovinf.hpp"
#include "machphys/machdata.hpp"
#include "machphys/random.hpp"

#include "machlog/attack.hpp"
#include "machlog/administ.hpp"
#include "machlog/machine.hpp"
#include "machlog/minesite.hpp"
#include "machlog/planet.hpp"
#include "machlog/follow.hpp"
#include "machlog/convyoff.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/squad.hpp"
#include "machlog/grpmove.hpp"
#include "machlog/races.hpp"
#include "machlog/strategy.hpp"

/* //////////////////////////////////////////////////////////////// */
PER_DEFINE_PERSISTENT( MachLogAdminAttackOperation );

MachLogAdminAttackOperation::MachLogAdminAttackOperation( MachLogAdministrator * pActor, MachActor* pTarget )
: 	MachLogOperation( "ADMIN_ATTACK_OPERATION", MachLogOperation::ADMIN_ATTACK_OPERATION ),
	pActor_( pActor ),
	pDirectObject_( pTarget ),
	complete_( false ),
	currentlyAttached_( true )
{
	pDirectObject_->attach( this );
}

MachLogAdminAttackOperation::MachLogAdminAttackOperation( MachLogAdministrator * pActor, MachActor* pTarget, PhysPathFindingPriority priority )
: 	MachLogOperation( "ADMIN_ATTACK_OPERATION", MachLogOperation::ADMIN_ATTACK_OPERATION, priority ),
	pActor_( pActor ),
	pDirectObject_( pTarget ),
	complete_( false ),
	currentlyAttached_( true )
{
	pDirectObject_->attach( this );
}


//virtual
MachLogAdminAttackOperation::~MachLogAdminAttackOperation()
{
	if( currentlyAttached_ )
		pDirectObject_->detach( this );
}

void MachLogAdminAttackOperation::doOutputOperator( ostream& o ) const
{
	o << "MachLogAdminAttackOperation" << std::endl;
	
	if( pDirectObject_ )
	{
		o << " pDirectObject_.objecttype() (" << pDirectObject_->id() << ") " << pDirectObject_->objectType() << std::endl;	
	}
	else
	{
		o << " target deleted or changed to our race." << std::endl;
	}

	
}

///////////////////////////////////

bool MachLogAdminAttackOperation::doStart()
{
	// it is possible to restart a suspended op, and the target has been destroyed in the interrim, so have
	// to check the status of complete_ (true if target destroyed)
	if( not complete_ )
	{
		if( pActor_->squadron() )
		{
			//the suitable target is greater than maximum wepaon range of each individual?
			//those in range fire now - the others move closer.
			HAL_STREAM(" issuing group move\n" );
		    MachLogGroupSimpleMove::Points points;
			points.push_back( pDirectObject_->position() );
			string reason;
			ctl_pvector< MachActor > actors;
			for( MachLogSquadron::Machines::iterator i = pActor_->squadron()->machines().begin();
				i!=pActor_->squadron()->machines().end(); ++i )
			{
				MachLogMachine* pSquadronMachine = (*i);
				
				if( pSquadronMachine->id() != pDirectObject_->id() 	// no machine should attack itself or move toward itself
					and pSquadronMachine->id() != pActor_->id() ) 	// don't issue orders to the administrator here
				{
					if( pSquadronMachine->objectIsCanAttack() )
					{
						MachLogCanAttack& machineAsCanAttack = pSquadronMachine->asCanAttack();
						// only attack if in range
						if( machineAsCanAttack.inWeaponRange( *pDirectObject_, MachLogCanAttack::NONE ) )
						{
							pSquadronMachine->newOperation( _NEW( MachLogAttackOperation( pSquadronMachine, pDirectObject_, MachLogAttackOperation::TERMINATE_ON_CHANGE ) ) );				
	   					}
						else
						{
							// not in range yet - just form part of the group move going towards the target destination
							actors.push_back( pSquadronMachine );
						}
					}
					else
					{
						// not an attack-capable machine....just stroll along with the rest of the squad
						actors.push_back( pSquadronMachine );		
					}
				}
			}
			if( actors.size() > 0 )
			    MachLogGroupSimpleMove groupMove( actors, points, MachLogRaces::instance().AICommandId(), &reason, pathFindingPriority() );
		}

		// now actually issue the attack op to the administrator
		if( pActor_->id() != pDirectObject_->id() 		// don't try to make the administrator attack itself
			and not( pActor_->hasCurrentTarget() ) ) //and pActor_->currentTarget().id() == pDirectObject_->id() ) )	// don't duplicate attack ops if we're already targetting this machine
		{
			// note that the administrator will move slightly separately from his subordinate group, relying on his attack op taking him to the target
			// while the others are using a group move
			pActor_->currentTarget( pDirectObject_ );
			pActor_->strategy().newOperation( _NEW( MachLogAttackOperation( pActor_, pDirectObject_, MachLogAttackOperation::TERMINATE_ON_CHANGE ) ) , true );
		}	
	}
	
	return true;
}

PhysRelativeTime MachLogAdminAttackOperation::doUpdate( )
{
	MexPoint2d targetPositionNow( pDirectObject_->position() );

	if( pActor_->squadron() )
	{
		ctl_pvector< MachActor > actors;
		MachLogGroupSimpleMove::Points points;
		points.push_back( targetPositionNow );
		string dummyReasonString;	

		//the suitable target is greater than maximum wepaon range of each individual?
		//those in range fire now - leave the rest alone
		for( MachLogSquadron::Machines::iterator i = pActor_->squadron()->machines().begin();
			i!=pActor_->squadron()->machines().end(); ++i )
		{
			MachLogMachine& squadronMachine = (**i);
			
			if( squadronMachine.id() != pDirectObject_->id() 	// no machine should attack itself or move toward itself
				and squadronMachine.id() != pActor_->id()	    // don't issue orders to the administrator here
				and not squadronMachine.isStandingGround() ) 	// machines standing ground are exempt from squad orders
			{
				if( squadronMachine.objectIsCanAttack() )
				{
					MachLogCanAttack& machineAsCanAttack = squadronMachine.asCanAttack();
					// only attack if in range and doesn't already have a target
					if( not machineAsCanAttack.hasCurrentTarget() )
					{
						MATHEX_SCALAR distanceToTargetNow = squadronMachine.position().euclidianDistance( targetPositionNow );
						// if we're within 100m of the target OR in weapon range, initiate an attack op
						if( distanceToTargetNow < 100.0 
							or machineAsCanAttack.inWeaponRange( *pDirectObject_, MachLogCanAttack::NONE ) )
						{
							squadronMachine.newOperation( _NEW( MachLogAttackOperation( &squadronMachine, pDirectObject_, MachLogAttackOperation::TERMINATE_ON_CHANGE ) ) );					
						}
						else
						{
							// if it's moved a reasonable distance since the last time we looked, we may need to
							// update our own movement to track the git.

							MATHEX_SCALAR distanceTargetMovedSinceAttackerChoseDestination = lastTargetPosition_.euclidianDistance( targetPositionNow );		
								
							MATHEX_SCALAR recalculateRatio = 0.45;

							if( distanceTargetMovedSinceAttackerChoseDestination / distanceToTargetNow > recalculateRatio ) 
							{
								actors.push_back( &squadronMachine );

								// yes, this is a bit shoddy - ideally, we'd want a lastTargetPosition_ for each machine.
								// This is a bit of a hack and the 0.45 ratio heuristic will break down when large numbers
								// of squad machines attacking this target are spread out over the map
								lastTargetPosition_ = targetPositionNow;	
							}
						}
					}
					// else if DOES have current target, just leave it alone.
				}
			}
		}
		if( actors.size() > 0 )
			    MachLogGroupSimpleMove groupMove( actors, points, MachLogRaces::instance().AICommandId(), &dummyReasonString, pathFindingPriority() );

	}	

	// now actually issue the attack op to the administrator
	if( pActor_->id() != pDirectObject_->id() 		// don't try to make the administrator attack itself
		and not( pActor_->hasCurrentTarget() ) ) //and pActor_->currentTarget().id() == pDirectObject_->id() ) )	// don't duplicate attack ops if we're already targetting this machine
	{
		// note that the administrator will move slightly separately from his subordinate group, relying on his attack op taking him to the target
		// while the others are using a group move
		pActor_->strategy().newOperation( _NEW( MachLogAttackOperation( pActor_, pDirectObject_, MachLogAttackOperation::TERMINATE_ON_CHANGE ) ) , true );
	}	
		
	return 5;
}

void MachLogAdminAttackOperation::doFinish()
{

}
	
bool MachLogAdminAttackOperation::doIsFinished() const
{
	return complete_;
}

bool MachLogAdminAttackOperation::doBeInterrupted()
{
	return true;
}

//virtual
bool MachLogAdminAttackOperation::beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData )
{
	ASSERT( pSubject == (W4dSubject*)pDirectObject_, "Notified by unexpected subject." );

	bool stayAttached = true;

	switch( event )
	{
	case W4dSubject::DELETED:
			//target has been deleted
			pDirectObject_ = NULL;
			currentlyAttached_ = false;
			complete_ = true;
			stayAttached = false;
	
			break;
	case W4dSubject::CLIENT_SPECIFIC:
		switch( clientData )
		{
			case MachLog::RACE_CHANGED:
			{
				MachLogRaces::DispositionToRace disposition = MachLogRaces::instance().dispositionToRace( pDirectObject_->race(), pActor_->race() );

				if( disposition == MachLogRaces::OUR_RACE or disposition == MachLogRaces::ALLY )
				{
					//target has changed to a fristd::endly race - no longer a viable target
					pDirectObject_ = NULL;
					currentlyAttached_ = false;
					complete_ = true;
					stayAttached = false;
				}
				break;

			}
			default:
				;
		}
		break;
	default:
		;
	}

	return stayAttached;
}

void perWrite( PerOstream& ostr, const MachLogAdminAttackOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pActor_;
	ostr << op.pDirectObject_;
    ostr << op.complete_;
	ostr << op.currentlyAttached_;
	ostr << op.lastTargetPosition_;	
}

void perRead( PerIstream& istr, MachLogAdminAttackOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pActor_;
	istr >> op.pDirectObject_;
    istr >> op.complete_;
	istr >> op.currentlyAttached_;
	istr >> op.lastTargetPosition_;	
	if( op.currentlyAttached_ )
		op.pDirectObject_->attach( &op );	
}

MachLogAdminAttackOperation::MachLogAdminAttackOperation( PerConstructor con )
:	MachLogOperation( con )
{
}

// Forced recompile 16/2/99 CPS
/* //////////////////////////////////////////////////////////////// */
