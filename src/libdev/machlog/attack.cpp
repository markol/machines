/*
 * A T T A C K . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include <iostream>
#include "ctl/pvector.hpp"

#include "mathex/cvexpgon.hpp"
#include "mathex/line3d.hpp"
#include "mathex/point2d.hpp"

#include "phys/cspace2.hpp"

#include "sim/manager.hpp"

#include "world4d/subject.hpp"

#include "phys/motchunk.hpp"

#include "machphys/mcmovinf.hpp"
#include "machphys/random.hpp"
#include "machphys/weapon.hpp"

#include "machlog/attack.hpp"
#include "machlog/internal/attacki.hpp"
#include "machlog/internal/firedata.hpp"

#include "machlog/aggressr.hpp"
#include "machlog/administ.hpp"
#include "machlog/animatio.hpp"
#include "machlog/artefact.hpp"
#include "machlog/canattac.hpp"
#include "machlog/cntrl.hpp"
#include "machlog/constron.hpp"
#include "machlog/entrance.hpp"
#include "machlog/inout.hpp"
#include "machlog/machine.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/planet.hpp"
#include "machlog/races.hpp"
#include "machlog/spacial.hpp"
#include "machlog/stats.hpp"
#include "machlog/weapon.hpp"
#include "machlog/grpmvinf.hpp"


PER_DEFINE_PERSISTENT( MachLogAttackOperation );

/* //////////////////////////////////////////////////////////////// */
MachLogAttackOperation::MachLogAttackOperation( MachLogMachine * pActor, MachActor * pDirectObject, MachLogAttackOperation::RaceChangeTerminate rct )
:	MachLogOperation( "ATTACK_OPERATION", MachLogOperation::ATTACK_OPERATION ),
	pImpl_( _NEW( MachLogAttackOperationImpl( pActor, pDirectObject, rct ) ) )
{
	CB_DEPIMPL( MachLogMachine*, pActor_ );
	CB_DEPIMPL( MachActor*, pDirectObject_ );
	CB_DEPIMPL( MachLogAggressor*, pAgg_ );
	CB_DEPIMPL( MachLogAdministrator*, pAdm_ );

	ASSERT( pActor_->objectIsCanAttack() ,"Object passed to Attack operation is not a CanAttack\n" );
	if( pActor_->objectType() == MachLog::AGGRESSOR )
	{
		pAgg_ = &pActor_->asAggressor();
	}
	else
	{
		pAdm_ = &pActor_->asAdministrator();
	}

	pDirectObject_->attach( this );
	currentlyAttached( true );

	// if I'm the only one currently initiating an attack against this target, it's quite probable I'm the
	// first one to spot it. Dispatch a cavee call to set all nearby chums on alert.
	if( not pDirectObject_->hasThreats() )
		pActor_->dispatchCaveeCall();

	// now signal to the target that the subject machine is threatening it,
    pDirectObject_->addThreat( pActor_->id() );
}

MachLogAttackOperation::MachLogAttackOperation( MachLogMachine * pActor, MachActor * pDirectObject, size_t commandId, MachLogAttackOperation::RaceChangeTerminate rct )
:	MachLogOperation( "ATTACK_OPERATION", MachLogOperation::ATTACK_OPERATION ),
  	pImpl_( _NEW( MachLogAttackOperationImpl( pActor, pDirectObject, commandId, rct ) ) )
{
	CB_DEPIMPL( MachLogMachine*, pActor_ );
	CB_DEPIMPL( MachActor*, pDirectObject_ );
	CB_DEPIMPL( MachLogAggressor*, pAgg_ );
	CB_DEPIMPL( MachLogAdministrator*, pAdm_ );

	ASSERT( pActor_->objectIsCanAttack() ,"Object passed to Attack operation is not a CanAttack\n" );
	if( pActor_->objectType() == MachLog::AGGRESSOR )
	{
		pAgg_ = &pActor_->asAggressor();
	}
	else
	{
		pAdm_ = &pActor_->asAdministrator();
	}

	pDirectObject_->attach( this );
	currentlyAttached( true );

	// if I'm the only one currently initiating an attack against this target, it's quite probable I'm the
	// first one to spot it. Dispatch a cavee call to set all nearby chums on alert.
	if( not pDirectObject_->hasThreats() )
		pActor_->dispatchCaveeCall();

	// now signal to the target that the subject machine is threatening it,
    pDirectObject_->addThreat( pActor_->id() );
}

MachLogAttackOperation::~MachLogAttackOperation()
{
	CB_DEPIMPL( MachActor*, pDirectObject_ );
	CB_DEPIMPL( MachLogMachine*, pActor_ );

	if( currentlyAttached() )
    {
		pDirectObject_->detach( this );

		// now signal to the target that the subject machine is no longer threatening it,
		// and if the current target is the direct object, cancel the target
    	stopTargetting();
    }
	MachLogRaces::instance().removeSpecialUpdateActor( pActor_, MachLog::DO_NOT_FORCE_REMOVE );

	_DELETE( pImpl_ );
}

void MachLogAttackOperation::doOutputOperator( ostream& o ) const
{
	CB_DEPIMPL( MachLogMachine*, pActor_ );
	CB_DEPIMPL( MachActor*, pDirectObject_ );
	CB_DEPIMPL( bool, targetBehindCover_ );

	o << "MachLogAttackOperation\n";
	if( pDirectObject_ )
	{
		o << " (" << directObject().id() << ") [" << directObject().objectType() << "] ";
		o << directObject().position() << std::endl;
		o << " hp " << directObject().hp() << " armour " << directObject().armour() << std::endl;
		if( pActor_->motionSeq().hasDestination() )
			o << "MotionSeq Dest " << pActor_->motionSeq().destination() << std::endl;
		o << " SQR range to target " << pActor_->position().sqrEuclidianDistance( directObject().position() ) << std::endl;
		if( targetBehindCover_ )
		{
			PhysConfigSpace2d::PolygonId junk;
			PhysConfigSpace2d& cSpace = MachLogPlanet::instance().configSpace();
			bool result = not cSpace.contains( pActor_->position(), pDirectObject_->position(), pActor_->obstacleFlags(), &junk, PhysConfigSpace2d::USE_PERMANENT_ONLY );
			o << " target behind cover Polygon " << junk << std::endl;
			if( pDirectObject_->objectIsConstruction() )
			{
				o << " object is cons. obstacle polygon " << pDirectObject_->asConstruction().obstaclePolygonId() << std::endl;
			}

		}

		/*
		o << " machine current target ";
		if( pAgg_ )
		{
			if( pAgg_->hasCurrentTarget() )
			{
				if( not pAgg_->currentTarget().isDead() )
					o << " (" << pAgg_->currentTarget().id() << ") [" << pAgg_->currentTarget().objectType() << "] " << std::endl;
				else
				{
					o << "Warning pAgg_ thinks it has a target but the pointer does not exist\n";
					o << " pAgg_ " << (void*)pAgg_ << std::endl;
					o << "pactor_->id() " << pActor_->id() << " type " << pActor_->objectType() << std::endl;
				}
			}
			else
				o << ": None Selected.\n";
		}
		if( pAdm_ )
		{
			if( pAdm_->hasCurrentTarget() and MachLogRaces::instance().objectExists( &pAdm_->currentTarget() ) )
				o << " (" << pAdm_->currentTarget().id() << ") [" << pAdm_->currentTarget().objectType() << "] " << std::endl;
			else
			{
				if( pAdm_->hasCurrentTarget() )
				{
					o << "Warning pAdm_ thinks it has a target but the pointer does not exist\n";
					o << " pAdm_ " << (void*)pAdm_ << std::endl;
					o << "pactor_->id() " << pActor_->id() << " type " << pActor_->objectType() << std::endl;
				}
				o << ": None Selected.\n";
			}
		}
		*/
	}
	else
		o << "target not there anymore" << std::endl;

}

///////////////////////////////////

MachActor& MachLogAttackOperation::directObject()
{
	CB_DEPIMPL( MachActor*, pDirectObject_ );

	return *pDirectObject_;
}

const MachActor& MachLogAttackOperation::directObject() const
{
	CB_DEPIMPL( MachActor*, pDirectObject_ );

	return *pDirectObject_;
}

///////////////////////////////////

bool MachLogAttackOperation::doStart()
{
	CB_DEPIMPL( MachLogMachine*, pActor_ );
	CB_DEPIMPL( MachActor*, pDirectObject_ );
	CB_DEPIMPL( MachLogAggressor*, pAgg_ );
	CB_DEPIMPL( MachLogAdministrator*, pAdm_ );
	CB_DEPIMPL( size_t, commandId_ );

	bool result = true;

	// safety check as OCCASIONALLY a doStart is called without a prior doIsFinished() check.
	if( pDirectObject_ )
	{
		// try to leave if our target's outside, we're inside, and our target's not a machine who's already on his way in
		// (If he is, best to wait for him.)

		MachLogConstruction* pConstruction;
		bool insideABuilding = checkNeedLeaveOperation( pActor_, &pConstruction );

		if( insideABuilding
			and not pConstruction->globalBoundary().contains( pDirectObject_->position() )
			and not( pDirectObject_->objectIsMachine() and pDirectObject_->asMachine().isEnteringBuilding( *pConstruction ) ) )
		{
			result = not checkNeedAndDoLeaveOperation( pActor_ );
		}

		if( pAgg_ )
			pAgg_->currentTarget( pDirectObject_ );
		else if( pAdm_ )
			pAdm_->currentTarget( pDirectObject_ );

	    if( commandId_ != MachLogRaces::instance().AINoCommandID() )
	        pActor_->motionSeq().commandId( commandId_ );
	}

	return result;
}

MexRadians
angleToTurnToFace( const MachLogMachine& actor, const MexPoint3d& pos );

/* //////////////////////////////////////////////////////////////// */


/* //////////////////////////////////////////////////////////////// */

PhysRelativeTime MachLogAttackOperation::doUpdate()
{
	CB_DEPIMPL( MachLogMachine*, pActor_ );
	CB_DEPIMPL( MachActor*, pDirectObject_ );
	CB_DEPIMPL( bool, targetBehindCover_ );
	CB_DEPIMPL( Action, lastAction_ );
	CB_DEPIMPL( size_t, commandId_ );
	CB_DEPIMPL( bool, hasGorillaPunch_ );


HAL_STREAM( "(" << pActor_->id() << ") MachLogAttackOperation::doUpdate entry at " << SimManager::instance().currentTime() << std::endl );
	PRE( not isFinished() );
	PRE( pActor_ != NULL );

    //Need a view of the subject actor as a canAttack
    MachLogCanAttack& attacker = pActor_->asCanAttack();
    MachActor& target = directObject();

    //Check each weapon separately
    const MachLogCanAttack::Weapons& weapons = _CONST_CAST( const MachLogCanAttack&, attacker).weapons();
	if( pActor_->isInSpecialUpdateActorsList() and MachLogRaces::instance().inSpecialActorUpdate() and not weapons.front()->recharged() )
		return 0;

    //Do nothing if delegated to a subop
	if( pSubOperation() )
		return 0.2;

	//If the attacker is busy shooting something else right now, try again in a bit.
    //Otherwise ensure the direct object is the current target.
	if( attacker.hasCurrentTarget() )
	{
	    if( &attacker.currentTarget() != &target )
    		return 0.5;
    }
    else
	{
        attacker.currentTarget( &target );
	    if( commandId_ != MachLogRaces::instance().AINoCommandID() )
	        pActor_->motionSeq().commandId( commandId_ );
	}

    //If we were moving to the side, and haven't arrived, try again later
    MachLogMachineMotionSequencer& motionSequencer = pActor_->motionSeq();
    if( lastAction_ == MOVE_TO_SIDE  and  motionSequencer.hasDestination() )
        return 0.1;

    //Set up useful local variables
   	PhysRelativeTime interval = 0;		//if this can fire then there will be a call back time of zero
										//otherwise will be overridden.
    const W4dEntity& targetObject = _CONST_CAST( const MachActor&, target).physObject();
    MATHEX_SCALAR heightModifier = 1.0;
    bool canMove = not pActor_->isStandingGround();
    MachLogCanAttack::WeaponDisposition worstDisposition = MachLogCanAttack::IN_RANGE;

    //Check for target being inside a building
    MachLogConstruction* pTargetBuilding = NULL;

	if( target.objectIsMachine() )
	{
		MachLogMachine& targetMachine = target.asMachine();
        if( targetMachine.insideBuilding() )
            pTargetBuilding = &(targetMachine.insideWhichBuilding());

		    //Check for attacker inside a building
	    MachLogConstruction* pAttackerBuilding = NULL;

		if( pActor_->objectIsMachine() )
		{
			MachLogMachine& attackingMachine = pActor_->asMachine();
	        if( attackingMachine.insideBuilding() )
	            pAttackerBuilding = &(attackingMachine.insideWhichBuilding() );
	    }

	    if( pTargetBuilding != pAttackerBuilding )
	    {
	        //If the target is inside a building, enter it
	    	pActor_->motionSeq().stop();
	        if( pTargetBuilding != NULL )
	        {
	            //Check attacker can legally enter the building
	            if( pActor_->canEnterConstructionNow( *pTargetBuilding )
					and not pActor_->isStandingGround()
	            	and not targetMachine.isLeavingBuilding() )		// don't bother chasing inside if the target is already coming out.
	    			subOperation( pActor_, _NEW( MachLogEnterBuildingOperation( pActor_, pTargetBuilding, NULL ) ) );
	            else
	                targetBehindCover_ = true;
	        }
	        else
	        {
	            //Target is outside, and attacker is inside, so leave the building, UNLESS that attacker is currently coming
				// into this very building, in which case, wait for him. Also wait if I'm standing ground
				if( targetMachine.isEnteringBuilding( *pAttackerBuilding ) or pActor_->isStandingGround() )
				{
					// should really make sure I'm facing towards the bounder, I suppose.
					turnToFaceTarget();
				}
				else
					subOperation( pActor_, _NEW( MachLogLeaveBuildingOperation( pActor_, pAttackerBuilding, NULL ) ) );
	        }
			return 1.0;
	    }
    }


	MachLogFireData fireData = pActor_->asCanAttack().createFireData();

	HAL_STREAM("firing weapons\n" );
	bool doRemoveSpecialUpdateActor = true;
	bool doAddSpecialUpdateActor = false;

	bool doingSpecialAttack = false;
	bool inPunchRange = false;

	bool targetIsFlyingMachine = pDirectObject_->objectIsMachine()
        						 and pDirectObject_->asMachine().machineIsGlider();

    for( MachLogCanAttack::Weapons::const_iterator it = weapons.begin(); it != weapons.end(); ++it )
    {
        //certain weapon types use special attack ops, and will not be fired here
        MachLogWeapon& weapon = *(*it);
        const MachPhysWeapon& physWeapon = _CONST_CAST( const MachLogWeapon&, weapon).physWeapon();
        MachPhys::WeaponType type = physWeapon.type();
		if( type == MachPhys::SUPERCHARGE_ADVANCED
			or type == MachPhys::SUPERCHARGE_SUPER
			or ( type == MachPhys::ORB_OF_TREACHERY and not pDirectObject_->objectIsMachine() ) )
            continue;

        //Gorilla punch handled differently. Don't attack flying machines with ground punch, btw.
        if( type == MachPhys::GORILLA_PUNCH )
        {
			hasGorillaPunch_ = true;
			if( not targetIsFlyingMachine )		// don't try to punch aircraft - that's plain daft
			{
				ASSERT( pActor_->objectIsCanAttack(), "Expected canAttack actor!" );

				MachLogCanAttack& canAttackWithPunch = pActor_->asCanAttack();
				if( weapon.recharged() and canAttackWithPunch.canPunch( directObject() ) )
	            {
					inPunchRange = true;

					if( weapon.recharged() )
					{
						canAttackWithPunch.attackWithPunch( pDirectObject_ );
					}
	            }
			}
        }
        else
        {
			HAL_STREAM(" weapon is a normal type firing pDirectObject is at this point " << (void*)pDirectObject_ << std::endl );
            //Get the disposition of this weapon with respect to position (not cover)
            MexPoint3d globalAimPoint;

            const MexTransform3d& globalWeaponTransform = physWeapon.globalTransform() ;
            MachLogCanAttack::WeaponDisposition disposition =
                MachLogCanAttack::rangeData( globalWeaponTransform, targetObject, physWeapon.weaponData(),
                                             heightModifier, &globalAimPoint );
        HAL_STREAM( "  rangeData disposition " << disposition << std::endl );

            //If in range, need to check for cover. Set up the global line from the weapon to the aim point
            if( disposition == MachLogCanAttack::IN_RANGE )
            {
                MexLine3d projectilePath( globalWeaponTransform.position(), globalAimPoint );
                disposition = attacker.behindCover( projectilePath, MachLogCanAttack::SAME_RACE_OK );
            }

            //If this weapon can fire, do so
            if( disposition == MachLogCanAttack::IN_RANGE )
            {
            	weapon.fire( &target, fireData );
				pActor_->asCanAttack().improveAim();
				doAddSpecialUpdateActor = true;
            }


            //Keep track of the most serious reason for not being able to fire
            if( disposition < worstDisposition )
                worstDisposition = disposition;

			// if gorilla is busy, it'll be because he's doing his punch
			// also, AI race eradicators will set a brief busy period after firing their vortex
			// to help stop them wandering into their own fire.
			doingSpecialAttack = ( pActor_->busy() );
        }
    }

	// special clause for George - if in punch range, don't care about the disposition returned from
	// any other weapon
	if( inPunchRange )
		worstDisposition = MachLogCanAttack::IN_RANGE;;

	HAL_STREAM("MLAttackOp::deciding what to do next\n" );
    //We need to decide what action to take.
    Action action = WAIT_FOR_INTERVAL;

    //Set action depending on the worst disposition
    if( worstDisposition != MachLogCanAttack::IN_RANGE )
    {
		action = actionForDisposition( worstDisposition, attacker );

		if( doingSpecialAttack and action == MOVE_CLOSER )
		{
			// override urge to move closer - just continue to track the target
			action = TURN_TO_FACE;
		}

		if( action == TURN_TO_FACE or action == MOVE_TO_SIDE )
			doAddSpecialUpdateActor = true;
	}
	else if( hasGorillaPunch_
			 and not targetIsFlyingMachine
			 and not doingSpecialAttack
			 and not inPunchRange
			 and canMove )
	{
		//Special clause for George....he ALWAYS wants to get into punch range, providing he's not standing ground.
		action = MOVE_CLOSER;
	}

	// We might want to stop if we're in weapon range.
	// HOWEVER, we don't necessarily want to if the target is currently moving away from us.
    if( canMove
        and worstDisposition != MachLogCanAttack::NOT_IN_DISTANCE_RANGE
        and action != MOVE_CLOSER
        and motionSequencer.hasDestination()
        and not ( pDirectObject_->objectIsMachine() and pDirectObject_->asMachine().evading() ) )
    {
        motionSequencer.stop();

        //TBD: This is necessary because the stop() cancels all plans for the machine, including the
        //tracking. This should not be the case, it should only cancel move animations.
        //This calls reinvokes the tracking.
        attacker.currentTarget( pDirectObject_ );
    }

    //take the action decided on
HAL_STREAM( "    action " << action << std::endl );
    switch( action )
    {
        case MOVE_CLOSER:
        {
            interval = moveCloserToTarget();
            break;
        }

        case TURN_TO_FACE:
        {
            interval = turnToFaceTarget();
            break;
        }

        case MOVE_TO_SIDE:
        {
            interval = moveToSide();
            break;
        }
    }

    //store the action
    lastAction_ = action;

	if( doAddSpecialUpdateActor )
		MachLogRaces::instance().addSpecialUpdateActor( pActor_, 0 );
	else
		MachLogRaces::instance().removeSpecialUpdateActor( pActor_, MachLog::DO_NOT_FORCE_REMOVE );

HAL_STREAM( "MachLogAttackOperation::doUpdate exit: interval " << interval << std::endl );
	return interval;
}

void MachLogAttackOperation::doFinish()
{
}

bool MachLogAttackOperation::doIsFinished() const
{
	CB_DEPIMPL( MachActor*, pDirectObject_ );
	CB_DEPIMPL( bool, targetBehindCover_ );
	CB_DEPIMPL( MachLogMachine*, pActor_ );

	bool result = false;
	if(	pDirectObject_ == NULL )
	{
		result = true;
	}
	else
	{
		MachLogRaces& races = MachLogRaces::instance();
		MachPhys::Race attackerRace = pActor_->race();

		if( races.controller( attackerRace ).type() == MachLogController::AI_CONTROLLER )
		{
			// If we're now allies of this race, better halt the attack.
			if( races.dispositionToRace( attackerRace, pDirectObject_->race() ) == MachLogRaces::ALLY )
			{
				result = true;
			}
		}
		else if( targetBehindCover_ or pDirectObject_->isDead() )
		{
			result = true;
		}
	}

	return result;
}

bool MachLogAttackOperation::doBeInterrupted()
{
	CB_DEPIMPL( MachLogMachine*, pActor_ );

	pActor_->motionSeq().stop();
	return not pActor_->motionSeq().hasDestination();
}

//virtual
bool MachLogAttackOperation::beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData )
{
	CB_DEPIMPL( MachLogMachine*, pActor_ );
	CB_DEPIMPL( MachActor*, pDirectObject_ );
	CB_DEPIMPL( RaceChangeTerminate, raceChangeTerminate_ );

	ASSERT( pSubject == (W4dSubject*)pDirectObject_, "Notified by unexpected subject." );
	ASSERT( currentlyAttached(), "Notified when object believed itself to be unattached." );

	switch( event )
	{
	case W4dSubject::DELETED:
		// if the current target is the direct object, cancel the target
		stopTargetting();
		pDirectObject_ = NULL;
		nextCallBackTime( SimManager::instance().currentTime() );
		currentlyAttached( false );

		// ensure that we're instantly ready to acquire new targets
		ASSERT( pActor_->objectIsCanAttack(), "Attack op actor was not a canAttack!" );
		pActor_->asCanAttack().cancelAnyAttackingUrgeSuppression();

		break;
	case W4dSubject::CLIENT_SPECIFIC:
		switch( clientData )
		{
			case MachLog::RACE_CHANGED:

				if( raceChangeTerminate_ == TERMINATE_ON_CHANGE )
				{
					MachLogRaces::DispositionToRace disposition = MachLogRaces::instance().dispositionToRace( pDirectObject_->race(), pActor_->race() );

					if( disposition == MachLogRaces::OUR_RACE or disposition == MachLogRaces::ALLY )
					{
						// signal to the target that the subject machine is no longer threatening it,
						// and if the current target is the direct object, cancel the target
						stopTargetting();
						pDirectObject_ = NULL;
						nextCallBackTime( SimManager::instance().currentTime() );
						currentlyAttached( false );
					}
				}
				break;
			case MachLog::TELEPORTED_OUT_OF_WORLD:
				// signal to the target that the subject machine is no longer threatening it,
				// and if the current target is the direct object, cancel the target
				stopTargetting();
				pDirectObject_ = NULL;
				nextCallBackTime( SimManager::instance().currentTime() );
				currentlyAttached( false );
				break;
			default:
			;
		}
	default:
		;
	}
	return currentlyAttached();
}

void MachLogAttackOperation::stopTargetting()
{
	CB_DEPIMPL( MachLogMachine*, pActor_ );
	CB_DEPIMPL( MachActor*, pDirectObject_ );

	PRE( pDirectObject_ );

	pDirectObject_->removeThreat( pActor_->id() );

	MachLogCanAttack& attacker = pActor_->asCanAttack();
    if( &(attacker.currentTarget()) == pDirectObject_ )
    	attacker.currentTarget( NULL );
}



void MachLogAttackOperation::dt( const char* text, bool start )
{
	CB_DEPIMPL( MachLogMachine*, pActor_ );

	static PhysAbsoluteTime startTime;
	static PhysAbsoluteTime endTime;
	if( start )
		startTime = Phys::time();
	else
	{
		endTime = Phys::time();
		DEBUG_STREAM( DIAG_MISC, pActor_->id() << ",," << text << "," << endTime - startTime << std::endl );
	}
}

PhysRelativeTime MachLogAttackOperation::moveCloserToTarget()
{
	CB_DEPIMPL( MachLogMachine*, pActor_ );
	CB_DEPIMPL( MachActor*, pDirectObject_ );
	CB_DEPIMPL( MexPoint2d, lastTargetPosition_ );
	CB_DEPIMPL( bool, hasGorillaPunch_ );

	ASSERT( not pActor_->isStandingGround(), "Should never move closer if actor is standing ground." );

	HAL_STREAM("MachLogAttackOperation::moveCloserToTarget\n" );
	if( not pDirectObject_ )
		return 0;

    PhysRelativeTime interval = 0.25;

	//If not moving, or the enemy has moved more than a certain distance (rules of latter described below),
	// set a new destination near the enemy
    MachActor& target = directObject();
	MexPoint2d targetPositionNow( target.position() );
	MexPoint2d attackerPositionNow( pActor_->position() );
	MATHEX_SCALAR distanceToTargetNow = attackerPositionNow.euclidianDistance( targetPositionNow );

	/*
	MexPoint2d targetDestinationNow;

	if( target.motionSeq().hasDestination() );
		targetDestinationNow = target.motionSeq().destination();
	*/

    HAL_INSPECT( targetPositionNow );
    HAL_INSPECT( lastTargetPosition_ );

	MachLogMachineMotionSequencer& motSeq = pActor_->motionSeq();

	bool recalculateDestination = false;
	bool chasingEvader = pDirectObject_->objectIsMachine() and pDirectObject_->asMachine().evading();

	if( not motSeq.hasDestination() )
			recalculateDestination = true;
	else
	{
		// only bother recalculating path if our current course will NOT take us within weapon range anyway.

		MATHEX_SCALAR sqrDistanceBetweenDestinationAndTarget = motSeq.destination().sqrEuclidianDistance( targetPositionNow );
		if( not pActor_->asCanAttack().withinSqrMaximumWeaponRange( sqrDistanceBetweenDestinationAndTarget ) )
		{
			// okay, what this system does is to decide whether or not to recalculate the path based on
			// a ratio of the (non-squared) distance the target has moved since we last set up a path
			// to the distance that now lies between us and the target. This gives greater resistance
			// to threshing the further the target is away - if he's on the other side of the map, it'll take
			// a logn time before his movement causes us to recalculate our move. Note also that even if he
			// slows to a standstill, the closer WE get, the closer we'll get to the point at which
			// recalculation is necessary.

			MATHEX_SCALAR distanceTargetMovedSinceAttackerChoseDestination = lastTargetPosition_.euclidianDistance( targetPositionNow );

			MATHEX_SCALAR recalculateRatio = 0.45;
			if( distanceToTargetNow < 50 and chasingEvader )
				recalculateRatio = 0.90;

			if( distanceTargetMovedSinceAttackerChoseDestination / distanceToTargetNow > recalculateRatio )
				recalculateDestination = true;
		}
	}

    if( recalculateDestination )
    	interval = calculateNewApproachPoint( targetPositionNow, attackerPositionNow, chasingEvader, distanceToTargetNow );

	HAL_STREAM("MachLogAttackOperation::moveCloserToTarget DONE: interval " << interval << std::endl);
    return interval;
}

PhysRelativeTime MachLogAttackOperation::turnToFaceTarget()
{
	CB_DEPIMPL( MachActor*, pDirectObject_ );
	CB_DEPIMPL( MachLogMachine*, pActor_ );

	HAL_STREAM("MachLogAttackOperation::turnToFaceTarget\n" );
	if( not pDirectObject_ )
		return 0;
    //If we are currently moving we must stop first.
    PhysRelativeTime interval = 0.25;
    bool canTurn = true;

	if( pActor_->motionSeq().hasDestination() )
    {
		interval = pActor_->motionSeq().stop();
        canTurn = interval == 0.0;
    }

    //If not stuck waiting to stop, do the turn
    if( canTurn )
    {
		MexRadians turnBy = angleToTurnToFace( *pActor_, directObject().position() );
		subOperation( pActor_, _NEW( MachLogTurnAnimation( pActor_, turnBy ) ) );
    }

	HAL_STREAM("MachLogAttackOperation::turnToFaceTarget DONE\n" );
    return interval;
}

PhysRelativeTime MachLogAttackOperation::moveToSide()
{
	CB_DEPIMPL( MachActor*, pDirectObject_ );
	CB_DEPIMPL( MachLogMachine*, pActor_ );

	if( not pDirectObject_ )
		return 0;
HAL_STREAM( "  MachLogAttackOperation::moveToSide() entry" << std::endl );
    PhysRelativeTime interval = 0.5;

	//move sideways with respect to the target
	MexPoint2d toPoint( directObject().position() );
HAL_STREAM( "    from point " << pActor_->position() << " to " << toPoint << std::endl );
	if( pActor_->motionSeq().findAvoidancePoint( pActor_->position(), &toPoint ) )
    {
        if( groupMoveInfo().valid() )
            interval = pActor_->motionSeq().destination( toPoint, groupMoveInfo() );
        else
            interval = pActor_->motionSeq().destination( toPoint );

		pActor_->motionSeq().pathFindingPriority( pathFindingPriority() );

HAL_STREAM( "    moving to " << toPoint << std::endl );
    }

HAL_STREAM( "  MachLogAttackOperation::moveToSide() exit" << std::endl );
    return interval;
}

void perWrite( PerOstream& ostr, const MachLogAttackOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pImpl_;
}

void perRead( PerIstream& istr, MachLogAttackOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pImpl_;
	if( op.currentlyAttached() )
		op.pDirectObject()->attach( &op );
}

bool MachLogAttackOperation::currentlyAttached() const
{
	CB_DEPIMPL( bool, currentlyAttached_ );

	return currentlyAttached_;
}

void MachLogAttackOperation::currentlyAttached( bool status )
{
	CB_DEPIMPL( bool, currentlyAttached_ );

	currentlyAttached_ = status;
}

MachActor* MachLogAttackOperation::pDirectObject()
{
	CB_DEPIMPL( MachActor*, pDirectObject_ );

	return pDirectObject_;
}

MachLogAttackOperation::MachLogAttackOperation( PerConstructor con )
:	MachLogOperation( con )
{
}
ostream& operator <<( ostream& o, MachLogAttackOperation::Action action )
{

    switch( action )
    {
        case MachLogAttackOperation:: WAIT_FOR_INTERVAL:
            o << "WAIT_FOR_INTERVAL";
            break;
        case MachLogAttackOperation:: MOVE_CLOSER:
            o << "MOVE_CLOSER";
            break;
        case MachLogAttackOperation:: TURN_TO_FACE:
            o << "TURN_TO_FACE";
            break;
        case MachLogAttackOperation:: MOVE_TO_SIDE:
            o << "MOVE_TO_SIDE";
			break;
		case MachLogAttackOperation:: FIRE:
            o << "FIRE";
            break;

  		DEFAULT_ASSERT_BAD_CASE( action );
    }

    return o;
}

MachLogAttackOperation::Action MachLogAttackOperation::actionForDisposition( MachLogCanAttack::WeaponDisposition worstDisposition, const MachLogCanAttack& attacker ) const
{
	CB_DEPIMPL( MachLogMachine*, pActor_ );
	CB_DEPIMPL( MachActor*, pDirectObject_ );
	CB_DEPIMPL( PhysAbsoluteTime, nextHardCoverCheckTime_ );

	Action action = WAIT_FOR_INTERVAL;
	bool canMove = not pActor_->isStandingGround();

	if( worstDisposition == MachLogCanAttack::NOT_IN_HORIZONTAL_ANGLE_RANGE
		and not attacker.canTurnHead() )
	{
		// if we're a non torso-twisty machine, we need to be a bit clever. The problem is that initiating
		// a move-closer often forces us to move out of horizontal angle range. So we need to check if there
		// is actually a clear line to the target we're wanting to turn and face. If there isn't, we
		// select MOVE_CLOSER in preference to TURN_TO_FACE and keep checking every update. If there is, we
		// TURN_TO_FACE and we don't need to check again for another (3?) seconds.

		// Oh, BTW, never do the MOVE_CLOSER thing if we're standing ground.

		PhysAbsoluteTime timeNow = SimManager::instance().currentTime();
		if( timeNow < nextHardCoverCheckTime_ or pActor_->isStandingGround() )
			action = TURN_TO_FACE;
		else if( pActor_->asCanAttack().IHaveClearWeaponPathToHim( pDirectObject_ ) )
		{
			nextHardCoverCheckTime_ = timeNow + behindCoverMoveGracePeriod();
			action = TURN_TO_FACE;
		}
		else
			action = MOVE_CLOSER;
	}
	else if( canMove )
	{
		switch( worstDisposition )
		{
			case MachLogCanAttack::NOT_IN_DISTANCE_RANGE:
			{
				//Can't reach and could be closer, so move there
            	action = MOVE_CLOSER;
			}
			break;

			case MachLogCanAttack::NOT_IN_VERTICAL_ANGLE_RANGE:
			{
				//If weapons can tilt, but aren't at limits yet, we'll probably be in vertical range soon.
	            //Otherwise try to move in closer.
				if( attacker.atTiltLimits() )
	            	action = MOVE_CLOSER;
			}
			break;

			case MachLogCanAttack::BEHIND_COVER:
			{
				//Hard cover. Get nearer.
	           	action = MOVE_CLOSER;
			}
			break;

			case MachLogCanAttack::BEHIND_FRIENDLY_COVER:
			{
	   			//Can't reach and could be closer, so move there
	           	action = MOVE_TO_SIDE;
			}
			break;

			default:
				;
		}
	}

	return action;
}

PhysRelativeTime MachLogAttackOperation::calculateNewApproachPoint( const MexPoint2d& targetPositionNow, const MexPoint2d& attackerPositionNow, bool chasingEvader, MATHEX_SCALAR distanceToTargetNow )
{
	CB_DEPIMPL( MachLogMachine*, pActor_ );
	CB_DEPIMPL( MexPoint2d, lastTargetPosition_ );
	CB_DEPIMPL( bool, hasGorillaPunch_ );
	CB_DEPIMPL( MachActor*, pDirectObject_ );

	PRE( not pActor_->isStandingGround() );

	PhysRelativeTime interval = 0;
	MachLogMachineMotionSequencer& motSeq = pActor_->motionSeq();

    //cache the target's position as we change our destination
    lastTargetPosition_ = targetPositionNow;

	// now we have the question of how far from the target we wish to start the free space point search from.
	// It should be no greater than maximum weapon range, but not necessarily really close. If we have a 100m range
	// weapon, for example, we might not want to start looking at 5m out, because that could actually be behind an
	// obstacle lying between us and the target (e.g. if the target is on top of a hill, the base of the hill will
	// be an obstacle), necessitating a ridiculous-looking attempt to find a route to the top of the hill rather
	// than simply approaching nearer to its base.

	MATHEX_SCALAR desiredRange = 10.0;

	if( chasingEvader )
	{
		desiredRange = 3.0;
	}
	else if( hasGorillaPunch_ )
	{
		desiredRange = 8.0;
	}
	else
	{
		MATHEX_SCALAR maxWeaponRange = pActor_->asCanAttack().getMaximumWeaponRange() * 0.95;

		if( distanceToTargetNow > maxWeaponRange )
		{
			if( pDirectObject_->objectIsConstruction() )
			{
				// 97% of weapon range seems a reasonable range at which to start looking for free space points
				// when attacking constructions
				desiredRange = maxWeaponRange * 0.97;
			}
			else
			{
				// We must be attacking a machine.

				// Okay, we use a little bit of nous here. If the guy is definitely moving away from us, we'll attempt
				// to keep closing to well within our weapons range. Otherwise, move to a position on the very edge of
				// weapons range to maximise any range advantage we may have.

				MexPoint3d estimated1SecondTargetOffset = pDirectObject_->predictedTargetOffset( 1.0 );
				MexTransform3d trans = pDirectObject_->globalTransform();
				trans.translate( estimated1SecondTargetOffset );

				MexPoint2d attackerPositionInOneSecond = trans.position();

				MATHEX_SCALAR sqrDistanceToTargetIn1Second = attackerPositionNow.sqrEuclidianDistance( attackerPositionInOneSecond );

				if( sqrDistanceToTargetIn1Second > sqr( distanceToTargetNow ) )
				{
					// target is moving away from us.
					// So 80% of weapon range seems a reasonable range at which to start looking for free space points.
					desiredRange = maxWeaponRange * 0.80;

				}
				else
				{
					// Target is not moving away from us, therefore do not attempt to close to any significant degree within
					// our maximum weapon range
					desiredRange = maxWeaponRange * 0.97;
				}
			}
		}

		else
			// already IN weapon range...set desired range to current distance, the "guaranteed distance to close by"
			// will do the rest.
			desiredRange = distanceToTargetNow;
	}

	// impose minimum and maximum boundings	---------------

	// the minimum we'll go towards the target is a further 7m (except if that takes us within 5m - see below).
	MATHEX_SCALAR minApproachRange = distanceToTargetNow - 7.0;

	desiredRange = std::min( desiredRange, minApproachRange );

	// never try and go closer than 5m
	desiredRange = std::max( desiredRange, 5.0 );
	// -----------------------------------------------------

	MATHEX_SCALAR extraSeparation = distanceToTargetNow - desiredRange;

	// need to have a guaranteed search ring thickness of 10m.
	extraSeparation = std::max( extraSeparation, 10.0 );


	MachLogSpacialManipulation::UseWhichPattern pattern = MachLogSpacialManipulation::USE_CLOCK_PATTERN;
	// now.....approach head-on, or try a flanking approach?
	if( pActor_->asCanAttack().canTurnHead()
		and distanceToTargetNow > 40.0
		and MachPhysRandom::randomInt( 0, 3 ) != 0 )
		pattern = MachLogSpacialManipulation::USE_FLANKING_PATTERN;

    //Construct a transform in which the z axis is vertical, the x axis is pointing from the target to
    //the attacker, and the origin is at the target's origin.
    MexTransform3d testTransform = MachLogSpacialManipulation::sourceToDestination3dTransform( targetPositionNow, attackerPositionNow, MachLogSpacialManipulation::SOURCE_END_IS_CENTRE );

    //get the clearance to use while searching.
    //TBD: There is still a problem with the pathfinding, in that the expansion space expands
    //polygons by 1.5, which can mean we on't be able to find a path to points legitimately contained in
    //the space. Hence use a scaled clearance. Once pathfinding is sorted, this can be unscaled.
    MATHEX_SCALAR useClearance = motSeq.highClearance() * 1.5;

    //Find a space we can move to on the way to the target. Note we can't move to the target itself,
    //because if it's a construction the destination would be inside the construction's obstacle polygon.
	MexPoint2d attackPoint;

	bool foundSpace = MachLogSpacialManipulation::getNearestFreeSpacePoint( testTransform, desiredRange, useClearance, &attackPoint, extraSeparation, pattern );

    if( foundSpace )
    {
        if( groupMoveInfo().valid() )
            interval = motSeq.destination( attackPoint, groupMoveInfo() );
        else
            interval = motSeq.destination( attackPoint );

        //We might be in range before arriving, so limit the time we wait
        if( interval > 0.5 )
            interval = 0.5;
    }
    else
    {
    	interval = 1.0;
    }


	return interval;
}

//static
PhysRelativeTime MachLogAttackOperation::behindCoverMoveGracePeriod()
{
	static PhysRelativeTime period = 3.0;
	return period;
}

// Forced recompile 16/2/99 CPS
/* End OPERATIO.CPP *************************************************/
