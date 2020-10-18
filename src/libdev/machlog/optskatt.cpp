/*
 * O P T S K L O C . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include "ctl/pvector.hpp"
#include "ctl/list.hpp"

#include "mathex/point2d.hpp"
#include "mathex/random.hpp"

#include "phys/cspace2.hpp"

#include "machphys/machdata.hpp"
#include "machphys/random.hpp"

#include "sim/manager.hpp"

#include "machlog/administ.hpp"
#include "machlog/attack.hpp"
#include "machlog/cntrl_ai.hpp"
#include "machlog/opadatta.hpp"
#include "machlog/opnukeat.hpp"
#include "machlog/oppodatt.hpp"
#include "machlog/optskatt.hpp"
#include "machlog/machine.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/missilem.hpp"
#include "machlog/pod.hpp"
#include "machlog/races.hpp"
#include "machlog/seekdest.hpp"
#include "machlog/squad.hpp"
#include "machlog/strategy.hpp"
#include "machlog/grpmove.hpp"

/* //////////////////////////////////////////////////////////////// */
PER_DEFINE_PERSISTENT( MachLogTaskAttackOperation );

MachLogTaskAttackOperation::MachLogTaskAttackOperation( MachLogSquadron * pActor, PhysRelativeTime initialDelay )
:	MachLogTaskOperation( "TASK_ATTACK_OPERATION" , MachLogTaskOperation::TASK_ATTACK_OPERATION ),
	pActor_( pActor ),
	complete_( false ),
	maxRangeFromPod_( 1000000 ),
	initialising_( true ),
	setStartingPoint_( false ),
	targetSystemType_( MachLog::TARGET_NORMAL ),
	specificObjectType_( false ),
	initialDelay_( initialDelay ),
	initialDelayProcessed_( false ),
	startTime_( 0.0 ),
	objectType_( MachLog::AGGRESSOR ),
	useIonCannon_( false ),
	useNuke_( false ),
	nMinimumMachines_( 0 ),
	assemblyPoint_( 0 ),
	doCount_( -1 ),
	doProximityCheck_( false ),
	state_( ASSEMBLING )
{	
}

MachLogTaskAttackOperation::~MachLogTaskAttackOperation( )
{
}

void MachLogTaskAttackOperation::startingPoint( const MexPoint3d& startingPoint )
{
	ASSERT( initialising_,"Undefined behaviour - setting starting point after initialisation.\n" );
	HAL_STREAM("(" << pActor_->id() << ") MLTAttackOp::startingPoint " << startingPoint << std::endl );
	startingPoint_ = startingPoint;
	setStartingPoint_ = true;
}

void MachLogTaskAttackOperation::targetSystemType( MachLog::TargetSystemType targetSystemType )
{
	ASSERT( initialising_,"Undefined behaviour - setting targetSystemType after initialisation.\n" );
	HAL_STREAM("(" << pActor_->id() << ") MLTAttackOp::targetSystemType " << targetSystemType << std::endl );
	targetSystemType_ = targetSystemType;
}

void MachLogTaskAttackOperation::objectType( MachLog::ObjectType ot, int st)
{
	ASSERT( initialising_,"Undefined behaviour - setting objectType after initialisation.\n" );
	HAL_STREAM("(" << pActor_->id() << ") MLTAttackOp::objectType " << ot << std::endl );
	objectType_ = ot;
	subType_ = st;
	specificObjectType_ = true;
}

void MachLogTaskAttackOperation::useIonCannon()
{
	ASSERT( initialising_,"Undefined behaviour - setting useIonCannon after initialisation.\n" );
	useIonCannon_ = true;
}

void MachLogTaskAttackOperation::useNuke()
{
	ASSERT( initialising_,"Undefined behaviour - setting useNuke after initialisation.\n" );
	useNuke_ = true;
}

void MachLogTaskAttackOperation::nMinimumMachines( int newNMinimumMachines )
{
	nMinimumMachines_ = newNMinimumMachines;

	// change specified 24/2/99
	if( nMinimumMachines_ == 0 )
	{
		doProximityCheck_ = false;
	}
	else
	{
		doProximityCheck_ = true;	
	}
}

void MachLogTaskAttackOperation::assemblyPoint( int newAssemblyPoint )
{
	//no way to verify if this is an ok position.
	assemblyPoint_ = newAssemblyPoint;
}

void MachLogTaskAttackOperation::initialDelay( const PhysRelativeTime& newInitialDelay )
{
	initialDelay_ = newInitialDelay;
	if( startTime_ + initialDelay_ < SimManager::instance().currentTime() )
		initialDelayProcessed_ = true;
}

void MachLogTaskAttackOperation::doOutputOperator( ostream& o ) const
{
	o << "MachLogTaskAttackOperation" << std::endl; 
	o << " initialDelay_ " << initialDelay_ << " processed " << initialDelayProcessed_ << " use: Ion C " << useIonCannon_ << " Nuke " << useNuke_ << std::endl;
//	MATHEX_SCALAR		maxRangeFromPod_;
	o << " setStartPoint " << setStartingPoint_ << " starting point " << startingPoint_ << std::endl;
	o << " targetSystemType_ " << targetSystemType_ << " specificObjectType_ " << specificObjectType_;
	if( specificObjectType_ )
		o << objectType_ << " " << (int)subType_ << std::endl;
	else
		o << std::endl;
	o << " assembly " << assemblyPoint_ << " minimium " << nMinimumMachines_ << " do proximity check " << doProximityCheck_ << std::endl;
	o << "Squadron state: "<< state_ << std::endl;
}

///////////////////////////////////

bool MachLogTaskAttackOperation::doStart()
{
	initialising_ = false;
	return true;
}

PhysRelativeTime MachLogTaskAttackOperation::doUpdate( )
{
	PhysRelativeTime interval = 5.0;

	bool done = false;

	MachLogRaces& races = MachLogRaces::instance(); 

	if( complete_ )
	{
		interval = 60.0;	
		done = true;
	}
	
	if( not done and not initialDelayProcessed_ )
	{
		if( startTime_ + initialDelay_ < SimManager::instance().currentTime() )
		{
			initialDelayProcessed_ = true;	
		}			
		else
		{
			for( MachLogSquadron::Machines::iterator i = pActor_->machines().begin();
				i != pActor_->machines().end(); ++i )
			{
				if( (*i)->isIdle() )
					races.controller( pActor_->race() ).moveOutOfTheWay( *i, assemblyPoint_ );
			}
			interval = 30.0;
			done = true;
		}
	}

	if( not done )
	{
		if( useIonCannon_ )
		{
			tryIonAttack();		
		}

		if( useNuke_ )
		{
			tryNukeAttack();					
		}
	}

	if( not done and pActor_->machines().size() < nMinimumMachines_ )
	{
		for( MachLogSquadron::Machines::iterator i = pActor_->machines().begin();
			i != pActor_->machines().end(); ++i )
		{
			if( (*i)->isIdle() )
			{
				races.controller( pActor_->race() ).moveOutOfTheWay( *i, assemblyPoint_ );
			}
		}

		if( state_ != RAMPAGING )
		{
			interval = 30.0;	
			done = true;
		}		
	}
	

	if( not done )
	{
		//will this attack operation occur a fixed number of times only?
		if( doCount_ > -1 )
		{
			if( --doCount_ <= 0 )
			{
				setCompleteState();
				pActor_->removeAllDesiredMachines();
			}
		}

		if( pActor_->hasCommander() )
		{
			interval = doUpdateWithCommander();
		}
		else
		{
			interval = doUpdateWithoutCommander();	
		}
	}

	return interval;	
}

void MachLogTaskAttackOperation::doFinish()
{
}

bool MachLogTaskAttackOperation::doIsFinished() const
{	
	return false;
}

bool MachLogTaskAttackOperation::setCompleteState()
{
	complete_ = false;
	return complete_;
}

bool MachLogTaskAttackOperation::doBeInterrupted()
{
	return true;
}

PhysRelativeTime MachLogTaskAttackOperation::doUpdateWithCommander()
{
	MachLogOperation::OperationType commanderOp = pActor_->commander().strategy().currentOperationType();

	if( commanderOp == MachLogOperation::ATTACK_OPERATION or
		commanderOp == MachLogOperation::SEEK_AND_DESTROY_OPERATION or
		commanderOp == MachLogOperation::ADMIN_ATTACK_OPERATION )
		return 30.0;
	//The commander is not doing an aggressive operation...so we can reassign as necessary.
	pActor_->commander().newOperation( _NEW( MachLogSeekAndDestroyOperation( &pActor_->commander(), MachLogRaces::instance().AICommandId(), targetSystemType_, objectType_, pActor_->commander().position() ) ) );
	return 30;
}

PhysRelativeTime MachLogTaskAttackOperation::doUpdateWithoutCommander()
{
	PhysRelativeTime interval = 10.0;

	HAL_STREAM("MachLogTaskAttackOperation::doUpdateWithoutCommander\n" );
	if( pActor_->machines().size() == 0 )
	{
		switchToAssembleMode();	
		return 10.0;
	}

	if( doProximityCheck_ )
	{
		if( state_ == RAMPAGING
			and doProximityCheck_ 
			and noRampagingMachines() )	
		{
			switchToAssembleMode();	
		}

		if( state_ == ASSEMBLING )
		{
			if( checkAllCloseEnoughAndDoAssemble() )
			{
				switchToRampageMode();	
			}	
		}		
	}
	else
	{
		// squadrons without proximity check requirements are constantly on the rampage.
		switchToRampageMode();	
	}	
		
	if( state_ == RAMPAGING )
	{
		//if they are all in range of the assmebly point then choose a new target ourselves and use a group move if the
		//target is greater than maximumweapon range of group away - other wise use seek and destroy operations.
		HAL_STREAM(" checking maximum weapon range\n" );
//		MATHEX_SCALAR squadronMaxWeaponRange = 0;
		MachActor* pCanAttackActor = NULL;
		for( MachLogSquadron::Machines::iterator i = pActor_->machines().begin();
			 not pCanAttackActor and i != pActor_->machines().end(); 
			 ++i )
		{
			MachLogMachine* pSquadronMachine = (*i);

			if( pSquadronMachine->objectIsCanAttack()
				and pSquadronMachine->rampaging() )
			{
				pCanAttackActor = pSquadronMachine;

				/*
				MATHEX_SCALAR thisMachineMaxWeaponRange = squadronMachine.asCanAttack().getMaximumWeaponRange();

				if( thisMachineMaxWeaponRange > squadronMaxWeaponRange )
				{
					squadronMaxWeaponRange = thisMachineMaxWeaponRange;
				}
				*/					
			}
		}
			
		//choose a suitable target
		MachActor* pTarget;

		if( pCanAttackActor )		
		{
			// okay, we have at least one attack-capable machine in the squad.

			if( MachLogRaces::instance().findTargetClosestTo( *pCanAttackActor, &pTarget, 
								targetSystemType_, objectType_, pCanAttackActor->position() ) )
			{
				interval = attackSpecificTarget( *pTarget );			  
			}
			else
			{
				// no viable targets to attack anywhere! Better just sit and wait.				
				interval = 30.0;
			} 
		} 
		else
		{
			// no attack-capable machines in the squadron! Better just sit and wait until some are built.				
			interval = 30.0; 
		}		
	}

	return interval;
}

MachPhys::Race MachLogTaskAttackOperation::enemyRace()
{
	ASSERT(false,"Call of MLTaskAttackOperation::enemyRace()\n" );
	if( pActor_->race() == MachPhys::RED )
		return MachPhys::BLUE;
	return MachPhys::RED;
}

void MachLogTaskAttackOperation::doCount( int newDo )
{
	doCount_ = newDo;
}

void perWrite( PerOstream& ostr, const MachLogTaskAttackOperation& op )
{
	const MachLogTaskOperation& base1 = op;

	ostr << base1;
	ostr << op.pActor_;
    ostr << op.complete_;
	ostr << op.maxRangeFromPod_;
	ostr << op.setStartingPoint_;
	ostr << op.startingPoint_;
	ostr << op.targetSystemType_;
	ostr << op.specificObjectType_;
	ostr << op.objectType_;
	ostr << op.subType_;
	ostr << op.initialising_;
	ostr << op.initialDelay_;
	ostr << op.initialDelayProcessed_;
	ostr << op.startTime_;
	ostr << op.useNuke_;
	ostr << op.useIonCannon_;
	ostr << op.nMinimumMachines_;
	ostr << op.assemblyPoint_;
	ostr << op.doCount_;
	ostr << op.doProximityCheck_;
	ostr << op.state_;
}

void perRead( PerIstream& istr, MachLogTaskAttackOperation& op )
{
	MachLogTaskOperation& base1 = op;

	istr >> base1;
	istr >> op.pActor_;
    istr >> op.complete_;
	istr >> op.maxRangeFromPod_;
	istr >> op.setStartingPoint_;
	istr >> op.startingPoint_;
	istr >> op.targetSystemType_;
	istr >> op.specificObjectType_;
	istr >> op.objectType_;
	istr >> op.subType_;
	istr >> op.initialising_;
	istr >> op.initialDelay_;
	istr >> op.initialDelayProcessed_;
	istr >> op.startTime_;
	istr >> op.useNuke_;
	istr >> op.useIonCannon_;
	istr >> op.nMinimumMachines_;
	istr >> op.assemblyPoint_;
	istr >> op.doCount_;
	istr >> op.doProximityCheck_;
	istr >> op.state_;
}

MachLogTaskAttackOperation::MachLogTaskAttackOperation( PerConstructor con )
:	MachLogTaskOperation( con )
{
}

void MachLogTaskAttackOperation::supressProximityCheck()
{
	doProximityCheck_ = false;
}

void MachLogTaskAttackOperation::tryIonAttack()
{
	MachLogRaces& races = MachLogRaces::instance();

	for( MachLogRaces::Pods::iterator i = races.pods( pActor_->race() ).begin(); i != races.pods( pActor_->race() ).end();
			++i )
	{
		MachLogPod& pod = (**i);
		if( pod.ionCannonReady() )    // obviously returns false if the pod has no ion cannon at all.....
		{
			MachActor* pTarget = NULL;
			if( races.findMostValuableTarget( pod, &pTarget, targetSystemType_, objectType_, pod.position(), 50 ) )
			{
				// let's toast it!
				pod.newOperation( _NEW( MachLogPodAttackOperation( &pod, pTarget ) ) );
			}
		}	
	}
}

void MachLogTaskAttackOperation::tryNukeAttack()
{
	MachLogRaces& races = MachLogRaces::instance();

	for( MachLogRaces::MissileEmplacements::iterator i = races.missileEmplacements( pActor_->race() ).begin(); i != races.missileEmplacements( pActor_->race() ).end();
			++i )
	{
		MachLogMissileEmplacement& emplacement = (**i);

		if( emplacement.subType() == MachPhys::ICBM )
		{
			if( emplacement.domeOpen() )
			{
				MachActor* pTarget = NULL;

				MachLog::TargetSystemType nukeTargetType = targetSystemType_;
				
				if( nukeTargetType == MachLog::TARGET_NORMAL )
					nukeTargetType = MachLog::FAVOUR_STATIC_TARGETS;
				
				if( races.findMostValuableTarget( emplacement, &pTarget, nukeTargetType, objectType_, emplacement.position(), 100 ) ) // 100 = minimum distance required
				{
					// I'm pressin' the button, Charlie.......
					emplacement.newOperation( _NEW( MachLogNukeAttackOperation( &emplacement, pTarget ) ) );
				}					
			}	
		}
	}	
}

bool MachLogTaskAttackOperation::checkAllCloseEnoughAndDoAssemble()
{
	const MachLogRaces::AssemblyPoints& points = MachLogRaces::instance().aggressorAssemblyPoints( pActor_->race() );

	if( assemblyPoint_ >= points.size() )
	{
		if( points.size() == 1 )
			assemblyPoint_ = 0;
		else
		{
			MexBasicRandom random = MexBasicRandom::constructSeededFromTime();
			assemblyPoint_ = mexRandomInt( &random, 0, (int)points.size() );
		}
	}

	bool closeEnough = true;

	for( MachLogSquadron::Machines::iterator i = pActor_->machines().begin();
		 i != pActor_->machines().end(); ++i )
	{
		MachLogMachine& squadronMachine = (**i);
		MexPoint2d machine2dPosition = squadronMachine.position();
		
		// if any machine is too far away (>50m), then we must return false 
		if( machine2dPosition.sqrEuclidianDistance( points[ assemblyPoint_ ] ) > 2500 )
		{
			closeEnough = false;

			// issue the move order to this machine if it's not idle
			if( squadronMachine.isIdle() )
			{
				MachLogRaces::instance().controller( pActor_->race() ).moveOutOfTheWay( &squadronMachine, assemblyPoint_ );
			}				
		}					
	}
	
	return closeEnough;
}	

void MachLogTaskAttackOperation::issueSeekAndDestroysToAllMachinesWithoutTargets()
{
	HAL_STREAM(" issuing seek and destroys\n" );
		
	for( MachLogSquadron::Machines::iterator i = pActor_->machines().begin();
		i != pActor_->machines().end(); ++i )
	{
		MachLogMachine& squadronMachine = (**i);
		if( squadronMachine.objectIsCanAttack() and not squadronMachine.asCanAttack().hasCurrentTarget() )
		{
			squadronMachine.newOperation( _NEW( MachLogSeekAndDestroyOperation( &squadronMachine, MachLogRaces::instance().AICommandId(), targetSystemType_, objectType_, squadronMachine.position() ) ) );	
		}			
	}
}

PhysRelativeTime MachLogTaskAttackOperation::attackSpecificTarget( const MachActor& targetActor )
{
	PRE( pActor_->machines().size() > 0 );
	PRE( not noRampagingMachines() );

	HAL_STREAM(" found closer target..looking at range against first machine compared against sqr weapon range\n" );
//	HAL_STREAM(" range " << targetActor.position().sqrEuclidianDistance( pCanAttackActor->position() ) << " sqr weaponRange " << ( maxWeaponRange * maxWeaponRange ) << std::endl );

	HAL_STREAM(" issuing group move\n" );

	PhysRelativeTime interval = 25.0;
    MexPoint2d destinationPoint;

	bool orderedAssemblyAroundClosestMachine = false;

	// may have to put the attack on hold until any stragglers have caught up....order them to move to the
	// position of the machine currently nearest the target if anyone is more than 50m away from him
	if( doProximityCheck_ and not pActor_->machines().empty() )
	{
		MexPoint2d positionOfMachineNearestTarget = positionOfRampagingMachineNearestTargetPoint( targetActor.position() );
		if( not allRampagingMachinesCloseToPoint( positionOfMachineNearestTarget ) )
		{	
			destinationPoint = positionOfMachineNearestTarget;	
			orderedAssemblyAroundClosestMachine = true;
			interval = 7.0;
		}
	}

 	if( not orderedAssemblyAroundClosestMachine )
 	{
		// just go towards the target
 		destinationPoint = targetActor.position();
 	}

	MachLogGroupSimpleMove::Points points;
    points.reserve( 1 );
	points.push_back( destinationPoint ); 

	string dummyReason;
	ctl_pvector< MachActor > actors;
    actors.reserve( pActor_->machines().size() + 1 );
	for( MachLogSquadron::Machines::iterator i = pActor_->machines().begin();
		i != pActor_->machines().end(); ++i )
	{
		MachLogMachine& squadronMachine = (**i);
		if( squadronMachine.objectIsCanAttack()
			and squadronMachine.rampaging() )
		{
			if( not squadronMachine.asCanAttack().hasCurrentTarget() )
			{
				MachLogCanAttack& canAttack = squadronMachine.asCanAttack();
				MATHEX_SCALAR sqrWeaponRange = sqr( canAttack.getMaximumWeaponRange() );

				//Any individual machine that can start shooting will commence a direct attack NOW.
				if( targetActor.position().sqrEuclidianDistance( squadronMachine.position() ) > sqrWeaponRange )
				{
					// don't add myself to the group move if my position is the destination point,
					// otherwise I might turn around and look stupid
					if( MexPoint2d( squadronMachine.position() ) != destinationPoint )
					{
						actors.push_back( &squadronMachine );	
					}		
					else
					{
						//I'm the leader - stop in my tracks.
						squadronMachine.motionSeq().stop();
					}
				}					
				else
				{
					squadronMachine.newOperation( _NEW( MachLogSeekAndDestroyOperation( &squadronMachine, MachLogRaces::instance().AICommandId(), targetSystemType_, objectType_, squadronMachine.position() ) ) );
				}
			}
		}
		else
		{
			actors.push_back( &squadronMachine );	
		}			
	}
	
	if( not actors.empty() )
	{
		MachLogGroupSimpleMove groupMove( actors, points, MachLogRaces::instance().AICommandId(), &dummyReason, pathFindingPriority() );
	}	

	return interval;
}

bool MachLogTaskAttackOperation::allRampagingMachinesCloseToPoint( const MexPoint2d& point ) const
{
	bool allCloseEnough = true;

	for( MachLogSquadron::Machines::iterator i = pActor_->machines().begin();
		 allCloseEnough and i != pActor_->machines().end(); ++i )
	{
		MachLogMachine& squadronMachine = (**i);
		MexPoint2d machine2dPosition = squadronMachine.position();

		// if any machine is too far away (>70m), then we must return false 
		if( machine2dPosition.sqrEuclidianDistance( point ) > 4900 )
		{
			allCloseEnough = false;
		}					
	}

	return allCloseEnough;
}

MexPoint2d MachLogTaskAttackOperation::positionOfRampagingMachineNearestTargetPoint( const MexPoint2d& targetPoint ) const
{
	PRE( not pActor_->machines().empty() );
	PRE( not noRampagingMachines() );

	MATHEX_SCALAR sqrClosestDistance = 1000000000;
	MachLogMachine* pClosestMachine = NULL;

	for( MachLogSquadron::Machines::iterator i = pActor_->machines().begin();
		 i != pActor_->machines().end(); ++i )
	{
		MachLogMachine& squadronMachine = (**i);
		
		if( squadronMachine.rampaging() )
		{
			MexPoint2d machine2dPosition = squadronMachine.position();
			MATHEX_SCALAR testDistance = machine2dPosition.sqrEuclidianDistance( targetPoint );

			// if any machine is too far away (>40m), then we must return false 
			if( testDistance < sqrClosestDistance )
			{
				sqrClosestDistance = testDistance;
				pClosestMachine = &squadronMachine;
			}					
		}
   	}

	ASSERT( pClosestMachine != NULL, "No machine was found!" );

	return pClosestMachine->position();		
}

void MachLogTaskAttackOperation::switchToAssembleMode()
{
	state_ = ASSEMBLING;		
}
	
void MachLogTaskAttackOperation::switchToRampageMode()
{

	for( MachLogSquadron::Machines::iterator i = pActor_->machines().begin();
		 i != pActor_->machines().end(); ++i )
	{
		(*i)->rampaging( true );
	}

	state_ = RAMPAGING;	
}

bool MachLogTaskAttackOperation::noRampagingMachines() const
{
	bool result = true;

	for( MachLogSquadron::Machines::iterator i = pActor_->machines().begin();
		 result and i != pActor_->machines().end(); ++i )
	{
		result = not( (*i)->rampaging() );
	}

	return result;
}

ostream& operator <<( ostream& o, const MachLogTaskAttackOperation::State& state )
{
    switch( state )
    {
        case MachLogTaskAttackOperation::ASSEMBLING:
            o << "ASSEMBLING";
            break;
        case MachLogTaskAttackOperation::RAMPAGING:
            o << "RAMPAGING";
            break;
    }
    
    return o;
}

/* //////////////////////////////////////////////////////////////// */
