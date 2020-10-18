/*
 * O P E V A D E . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include "machlog/opevade.hpp"

#include "machlog/internal/opevadei.hpp"

#include "mathex/vec3.hpp"
#include "mathex/poly2d.hpp"

#include "sim/manager.hpp"

#include "phys/motchunk.hpp"

#include "machphys/constron.hpp"
#include "machphys/epp.hpp"
#include "machphys/epps.hpp"
#include "machphys/machdata.hpp"
#include "machphys/machine.hpp"
#include "machphys/mcmovinf.hpp"
#include "machphys/random.hpp"

#include "machlog/administ.hpp"
#include "machlog/aggressr.hpp"
#include "machlog/constron.hpp"
#include "machlog/entrance.hpp"
#include "machlog/follow.hpp"
#include "machlog/garrison.hpp"
#include "machlog/inout.hpp"
#include "machlog/machine.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/missilem.hpp"
#include "machlog/move.hpp"
#include "machlog/pod.hpp"
#include "machlog/races.hpp"
#include "machlog/spacial.hpp"
#include "machlog/strategy.hpp"


PER_DEFINE_PERSISTENT( MachLogEvadeOperation );

//dereferencing all these will cause a hit in the debug version - in the release version the compiler will remove
//any unreferenced variables.
#define CB_MachLogEvadeOperation_DEPIMPL()	\
		PRE( pImpl_ );	\
		CB_DEPIMPL( MachLogMachine*, pActor_ ); \
    	CB_DEPIMPL( bool, finished_ ); \
		CB_DEPIMPL( MachLogOperation*, pCachedOperation_ ); \
		CB_DEPIMPL( EvadeOpType,	currentEvadeOpType_ ); \
		CB_DEPIMPL( PhysAbsoluteTime,	lastTimeIHadStrongThreat_ );

/* //////////////////////////////////////////////////////////////// */

MachLogEvadeOperation::MachLogEvadeOperation( MachLogMachine * pActor )
:	MachLogOperation( "EVADE_OPERATION", MachLogOperation::EVADE_OPERATION ),
	pImpl_( _NEW( MachLogEvadeOperationImpl( pActor ) ) )
{
	CB_MachLogEvadeOperation_DEPIMPL();

	pActor_->evading( true );

	pActor_->motionSeq().allowMoveOutOfWay( true );
}
/* //////////////////////////////////////////////////////////////// */

MachLogEvadeOperation::~MachLogEvadeOperation()
{
	CB_MachLogEvadeOperation_DEPIMPL();

	pActor_->evading( false );

	// if we still have a pointer to the cached op, we must be terminating through circumstances where that
	// that op has NOT been restored to the strategy. We must delete it ourselves to prevent a memory leak.
	if( pCachedOperation_ )
		_DELETE( pCachedOperation_ );

	_DELETE( pImpl_ );
}

/* //////////////////////////////////////////////////////////////// */

void MachLogEvadeOperation::storeOldFirstOperation( MachLogOperation* pOldOp )
{
	CB_MachLogEvadeOperation_DEPIMPL();

	pCachedOperation_ = pOldOp;
	POST( pCachedOperation_ != NULL );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogEvadeOperation::doOutputOperator( ostream& o ) const
{
	CB_MachLogEvadeOperation_DEPIMPL();

	o << "MachLogEvadeOperation\n";

	switch( currentEvadeOpType_ )
	{
		case SEEK_GARRISON: 	o << "[ Moving to garrison. ]" << std::endl; 	break;
		case SEEK_AGGRESSIVE:   o << "[ Moving to aggressive: ]" << std::endl ; break;
	   	case SEEK_TURRET:		o << "[ Moving to turret. ]" << std::endl ; break;
		case SEEK_POD:			o << "[ Moving to pod. ]" << std::endl; 		break;
	   	default:
		{
			o << "Seeking random runaway point." << std::endl;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogEvadeOperation::doStart()
{
   	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

PhysRelativeTime MachLogEvadeOperation::doUpdate()
{
	CB_MachLogEvadeOperation_DEPIMPL();

	PRE( not isFinished() );
	PRE( pActor_ != NULL );

	ASSERT( pCachedOperation_ != NULL, "MachLogEvadeOperation::doUpdate : Hey, we shouldn't have a null pointer for the cached op by now!" );
	// get hold of our current strong threats

	MachLogMachine::Actors strongThreats;

	strongThreats.reserve( 6 );

	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();

	pActor_->populateStrongThreats( &strongThreats );

	if( not strongThreats.empty() )
		lastTimeIHadStrongThreat_ = timeNow;

	// If we are still doing a subop, only terminate evade after 16 seconds clear of strong threats.
	// If we DON'T have a subop, and there's no strong threats, we'll have to cancel as subsequent code
	// to initiate an evade subop relies on there being at least one strong threat.

	bool naturallyTerminate = ( pSubOperation() and ( timeNow - lastTimeIHadStrongThreat_ >= 16.0 ) ) or
							  ( not( pSubOperation() ) and strongThreats.empty() );

	// have we now met any of the conditions for terminating this operation?
	if ( naturallyTerminate			// stop evading if no strong threats for 10 seconds or more
		 or pActor_->virtualDefCon() == MachLog::DEFCON_HIGH
		 or ( not( strongThreats.empty() ) and pActor_->notAfraidOfStrongThreats( strongThreats ) ) )
	{
		// restore previous first operation as first (and only) operation on the strategy queue.
		pActor_->strategy().newOperation( pCachedOperation_, false );
		pCachedOperation_ = NULL;	// so we don't try and delete it in the destructor
		finished_ = true;
		return 0.0;
	}

	// gadzooks! We're still under threat and must do something!

	// well, perhaps we're already in the process of doing something...
	if( pSubOperation() )
		return 2.0;

	// otherwise, it's time to START doing something.

	PhysRelativeTime interval = 0.0;

	// let's get a hold of our EPP object from via our associated physmachine object.

	const MachPhysEvasionPriorityPlan& EPP = pActor_->machineData().EPP();

	// iterate through priorities 0 to MachPhysEvasionPriorityPlans::N_PRIORITY_LEVELS.
	//	At each stage, get closest candidate target of
	//	an applicable evasion operations. At the end of each priority level sweep, see if
	//  we now have a target. If we do, initiate a relevent operation. If we do not, go onto
	//	the next priority level.

	// default op type - will change as soon as any viable alternative is found
   	currentEvadeOpType_ = SEEK_SAFER_GROUND;

	// pointer keeping track of most viable destination actor
	MachActor* pEvadeDestinationActor = NULL;
	int strongestMilitaryStrength = 0;

   	// to be used only in case of entering a garrison
   	MachLogGarrison* pGarrison = NULL;
	MachPhysStation* pStation = NULL;

	// to be used only in case of moving near to a pod
	MachActor* pPodActor = NULL;

   	for( int priority = 0;
		currentEvadeOpType_ == SEEK_SAFER_GROUND and priority < MachPhysEvasionPriorityPlans::N_PRIORITY_LEVELS;
		++priority )
	{
		if( EPP.garrisonPriority() == priority )
		{
			bool foundGarrison = MachLogRaces::instance().findFriendlyGarrisonClosestTo( *pActor_, &pGarrison, &pStation, MachLogRaces::FREE_BAYS_PLUS_NO_ENEMY_CANATTACKS );

			if( foundGarrison )
			{
				pEvadeDestinationActor = pGarrison;
				currentEvadeOpType_ = SEEK_GARRISON;
				strongestMilitaryStrength = pGarrison->localStrengthFromPosition( pActor_->position() );
			}
		}

		if( EPP.aggressivesPriority() == priority )
		{
			MachLogMachine* pFriendlyMachine=NULL;
			if( pActor_->findSafestNearbyAggressiveMachine( &pFriendlyMachine ) )
			{
				ASSERT( pFriendlyMachine, "Unexpected NULL pointer for pFriendlyMachine." );

				int aggressiveMilitaryStrength = pFriendlyMachine->localStrengthFromPosition( pActor_->position() );

				if( not pEvadeDestinationActor
					or
					( pEvadeDestinationActor and
					  aggressiveMilitaryStrength > strongestMilitaryStrength ) )
				{
					pEvadeDestinationActor = pFriendlyMachine;
					currentEvadeOpType_ = SEEK_AGGRESSIVE;
					strongestMilitaryStrength = aggressiveMilitaryStrength;
				}
			}
		}

		if( EPP.turretsPriority() == priority )
		{
			MachLogMissileEmplacement* pFriendlyMissileEmplacement=NULL;
			if( pActor_->findSafestNearbyMissileEmplacement( &pFriendlyMissileEmplacement ) )
			{
				ASSERT( pFriendlyMissileEmplacement, "Unexpected NULL pointer for pFriendlyMissileEmplacement." );
				int missileEmplacementMilitaryStrength = pFriendlyMissileEmplacement->localStrengthFromPosition( pActor_->position() );
				if( not pEvadeDestinationActor
					or
					( pEvadeDestinationActor and
					  missileEmplacementMilitaryStrength > strongestMilitaryStrength ) )
				{
					pEvadeDestinationActor = pFriendlyMissileEmplacement;
					currentEvadeOpType_ = SEEK_TURRET;
					strongestMilitaryStrength = missileEmplacementMilitaryStrength;
				}
			}
		}

		if( EPP.podPriority() == priority )
		{
			int podMilitaryStrength = MachLogRaces::instance().findSafestFriendly( *pActor_, &pPodActor,
							MachLogRaces::PODS );

			if ( podMilitaryStrength > 0 ) 		// (0 = nothing found)
			{
				if( ( not pEvadeDestinationActor
					  or
					  ( pEvadeDestinationActor and
					    podMilitaryStrength > strongestMilitaryStrength ) )
					  and
					  // don't count this as a valid option if we're already very close to the pod
					  pActor_->position().sqrEuclidianDistance( pPodActor->position() ) > 3000 )
				{
					pEvadeDestinationActor = pPodActor;
					currentEvadeOpType_ = SEEK_POD;
					strongestMilitaryStrength = podMilitaryStrength;
				}
			}
		}
	}

	// now create a subop dependent on the results of our priority search.

	MachLogOperation* pNewSubOp;

	switch( currentEvadeOpType_ )
	{
		case SEEK_GARRISON:
		{
			ASSERT( pGarrison and pStation, "MachLogEvadeOperation::doUpdate : We have an unexpected NULL pointer for pGarrison and/or pStation." );

			pNewSubOp = _NEW( MachLogEnterBuildingOperation( pActor_, pGarrison, pStation ) );
			break;
		}
		case SEEK_AGGRESSIVE:
		{
			ASSERT( pEvadeDestinationActor, "MachLogEvadeOperation::doUpdate : We have an unexpected NULL pointer for the destination actor." );
			ASSERT( pEvadeDestinationActor->objectIsMachine(), "MachLogEvadeOperation::doUpdate : cannot follow a non-machine" );

			// set up a special follow op which will terminate when the machine is sufficiently close.

			pNewSubOp = _NEW( MachLogFollowOperation(
						pActor_,
						&( pEvadeDestinationActor->asMachine() ),
						MexPoint2d( MachPhysRandom::randomDouble( -20, 20 ), MachPhysRandom::randomDouble( -20, 20 ) ),
						MachLogFollowOperation::NOT_CAMOUFLAGED,
						MachLogFollowOperation::TERMINATE_WHEN_CLOSE ) );
			break;
		}
		case SEEK_TURRET:
		{
			// essentially just move to a free space within the protective umbrella of the selected turret

			ASSERT( pEvadeDestinationActor, "MachLogEvadeOperation::doUpdate : We have an unexpected NULL pointer for the destination actor." );

			MexPoint2d dest;

			// okay, find the nearest strong threat. We'll use his global transform to help define the space search.

	 		// !! We should really use the direction vector of the aggressor from the turret, then pop on the far side of it.

			MATHEX_SCALAR sqrDistance = 0;
			MATHEX_SCALAR range = 1000000000;

			const MachActor* pNearestEnemy = NULL;

			ASSERT( not strongThreats.empty(), "Our strongThreats list is unexpectedly empty." );

			MachLogRaces& races = MachLogRaces::instance();

 			for( MachLogMachine::Actors::iterator i = strongThreats.begin(); i < strongThreats.end(); ++i )
			{
				sqrDistance = pActor_->position().sqrEuclidianDistance( races.actor( (*i) ).position() );

				ASSERT_INFO( sqrDistance );
				ASSERT( sqrDistance < 1000000000, "Unfeasably large distance between two actors." );

				if( sqrDistance < range )
				{
					range = sqrDistance;
					pNearestEnemy = &races.actor( *i );
				}
			}

			ASSERT( pNearestEnemy, "Unexpected NULL pointer for pNearestEnemy!" );

			// now calculate a vector between the aggressor and the destination turret, and use that vector's
			// direction to find a spot that places the turret between the evader and the aggressor

			MexVec3 aggressorToTurretVec( pNearestEnemy->position(), pEvadeDestinationActor->position() );
    		aggressorToTurretVec.makeUnitVector();
			aggressorToTurretVec *= 8;	// distance the evader will try and sit behind the turret
			MexPoint2d pointBehindTurret = pEvadeDestinationActor->position();
			pointBehindTurret += aggressorToTurretVec;

			const MexTransform3d& enemyAsTransform = pNearestEnemy->physObject().globalTransform();
			MexTransform3d destinationPointAsTransform( enemyAsTransform.xBasis(),enemyAsTransform.yBasis(),enemyAsTransform.zBasis(), pointBehindTurret );

			bool finished = false;
			MATHEX_SCALAR radius = 1;
			while( not finished )
			{
				if( not MachLogSpacialManipulation::getNearestFreeSpacePoint( destinationPointAsTransform, radius , pActor_->highClearence(), &dest ) )
					radius += 20;
				else
					finished = true;
			}

			pNewSubOp = _NEW( MachLogMoveToOperation( pActor_, dest ) );

			break;
		}
		case SEEK_POD:
		{
			MexPoint2d dest;

			ASSERT( pPodActor, "MachLogEvadeOperation::doUpdate : We have an unexpected NULL pointer for pPodActor." );

			const MexTransform3d& podAsTransform = pPodActor->asPod().physConstruction().globalTransform();
			MexTransform3d destinationPointAsTransform( podAsTransform.xBasis(),podAsTransform.yBasis(),podAsTransform.zBasis(), pPodActor->position() );

			bool finished = false;
			MATHEX_SCALAR radius = 25;
			while( not finished )
			{
				if( not MachLogSpacialManipulation::getNearestFreeSpacePoint( destinationPointAsTransform, radius , pActor_->highClearence(), &dest ) )
					radius += 20;
				else
					finished = true;
			}

			pNewSubOp = _NEW( MachLogMoveToOperation( pActor_, dest, true, 10.0 ) );	//10m tolerance

			break;
		}
		default:
		{
			// in this case, we simply try and find a random point 40m from our current position. If we
			// can't find space, we keep checking outwards in 20m radius increment concentric circles

			MexPoint2d dest;

			// okay, find the strongest threat (this takes into account both the military strength of the actor, and its
			// distance). We'll use its global transform to help define the space search.
			// This means if we draw a circle around ourselves, our preferred evasion point is in a direction from
			// ourselves aligned with the facing of the aggressing machine. As that machine is most likely pointing
			// toward us, this should take us away from said aggressor.

			int strongestThreatMilitaryStrength = -1;
			int candidateThreatMilitaryStrength;

			const MachActor* pNearestEnemy = NULL;

			ASSERT( not strongThreats.empty(), "Our strongThreats list is unexpectedly empty." );
			MachLogRaces& races = MachLogRaces::instance();
			for( MachLogMachine::Actors::iterator i = strongThreats.begin(); i < strongThreats.end(); ++i )
			{
				MachActor& threatActor = races.actor( *i );
				candidateThreatMilitaryStrength = threatActor.localStrengthFromPosition( pActor_->position() );
				if( candidateThreatMilitaryStrength > strongestThreatMilitaryStrength )
				{
					strongestThreatMilitaryStrength = candidateThreatMilitaryStrength;
					pNearestEnemy = &threatActor;
				}
			}

			ASSERT( pNearestEnemy, "MachLogEvadeOperation::doUpdate : We have an unexpected NULL pointer for pNearestEnemy." );

			MexTransform3d destinationPointAsTransform = MachLogSpacialManipulation::sourceToDestination3dTransform( pNearestEnemy->position(), pActor_->position(), MachLogSpacialManipulation::DESTINATION_END_IS_CENTRE );

			bool finished = false;
			MATHEX_SCALAR radius = 60;
			while( not finished )
			{
				if( not MachLogSpacialManipulation::getNearestFreeSpacePoint( destinationPointAsTransform, radius , pActor_->highClearence(), &dest ) )
					radius += 20;
				else
					finished = true;
			}

			pNewSubOp = _NEW( MachLogMoveToOperation( pActor_, dest, true, 10.0 ) );	//10m tolerance
		}
	}

	subOperation( pActor_, pNewSubOp );


   	return interval;			//!! Have to alter interval if we have a subop. Probably.

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void MachLogEvadeOperation::doFinish()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogEvadeOperation::doIsFinished() const
{
	CB_MachLogEvadeOperation_DEPIMPL();

	// !! Code in here to check if strong threats are detected.
	return finished_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogEvadeOperation::doBeInterrupted()
{
	CB_MachLogEvadeOperation_DEPIMPL();

	//HAL_STREAM("(" << pActor_->id() << ") MLHealOp::doBeInterrupted\n" );
	pActor_->motionSeq().stop();
	return not pActor_->motionSeq().hasDestination();
}


/////////////////////////////////////////////////// persistence /////////////////////////////////////////////////////

void perWrite( PerOstream& ostr, const MachLogEvadeOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pImpl_;
}

void perRead( PerIstream& istr, MachLogEvadeOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pImpl_;
}

MachLogEvadeOperation::MachLogEvadeOperation( PerConstructor con )
:	MachLogOperation( con )
{
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////


// End of code for MachLogEvadeOperation


/* End OPEVADE.CPP *************************************************/
