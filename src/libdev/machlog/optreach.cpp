/*
 * A T T A C K . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include <iostream>

#include "ctl/pvector.hpp"

//#include "mathex/cvexpgon.hpp"
#include "mathex/line3d.hpp"
#include "mathex/point2d.hpp"

#include "sim/manager.hpp"

#include "world4d/subject.hpp"

#include "phys/cspace2.hpp"
#include "phys/motchunk.hpp"

//#include "machphys/random.hpp"
#include "machphys/weapon.hpp"

#include "machlog/aggressr.hpp"
#include "machlog/administ.hpp"
#include "machlog/canattac.hpp"
#include "machlog/optreach.hpp"

#include "machlog/machine.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/planet.hpp"
#include "machlog/races.hpp"
#include "machlog/spacial.hpp"
#include "machlog/constron.hpp"
#include "machlog/entrance.hpp"
#include "machlog/inout.hpp"
#include "machlog/weapon.hpp"
#include "machlog/grpmvinf.hpp"

PER_DEFINE_PERSISTENT( MachLogTreacheryOperation );


/* //////////////////////////////////////////////////////////////// */
MachLogTreacheryOperation::MachLogTreacheryOperation( MachLogMachine * pActor, MachActor * pDirectObject )
:	MachLogOperation( "TREACHERY_OPERATION" , MachLogOperation::TREACHERY_OPERATION ),
	pActor_( pActor ),
  	pDirectObject_( pDirectObject ),
  	targetBehindCover_( false ),
	pAgg_( NULL ),
	pAdm_( NULL ),
	lastAction_( WAIT_FOR_INTERVAL )
{
	ASSERT( pActor_->objectIsCanAttack() ,"Object passed to Treachery operation is not a CanAttack\n" );
	//HAL_STREAM("(" << pActor_->id() << ") MLTreacheryOperation::CTOR\n" );
	//HAL_STREAM(" " << pActor_->objectType() << std::endl );
	//HAL_STREAM(" forming a pointer to relevant class\n" );
	if( pActor_->objectType() == MachLog::AGGRESSOR )
	{
		pAgg_ = &pActor_->asAggressor();
	}
	else
	{
		pAdm_ = &pActor_->asAdministrator();
	}

	pDirectObject_->attach( this );
	pDirectObject_->addThreat( pActor_->id() );
	currentlyAttached_ = true;
	//HAL_STREAM(" exit\n" );

	// prevents machine from doing its normal checkAndAttackStuff for a little while.
	pActor_->asCanAttack().suppressAttackingUrges();
}

MachLogTreacheryOperation::~MachLogTreacheryOperation()
{
	//HAL_STREAM("(" << pActor_->id() << ") MLTreacheryOp::DTOR\n" );
	if( currentlyAttached_ )
	{
		pDirectObject_->detach( this );
		stopTargetting();
	}
}

void MachLogTreacheryOperation::doOutputOperator( ostream& o ) const
{
	o << "MachLogTreacheryOperation\n";
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
			if( pAgg_->hasCurrentTarget() and not pAgg_->currentTarget().isDead() )
				o << " (" << pAgg_->currentTarget().id() << ") [" << pAgg_->currentTarget().objectType() << "] " << std::endl;
			else
			{
				if( pAgg_->hasCurrentTarget() )
				{
					o << "Warning pAgg_ thinks it has a target but the pointer does not exist\n";
					o << " pAgg_ " << (void*)pAgg_ << std::endl;
					o << "pactor_->id() " << pActor_->id() << " type " << pActor_->objectType() << std::endl;
				}
				o << ": None Selected.\n";
			}
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
	o << "Motion Sequencer\n================\n" << pActor_->motionSeq() << std::endl;

}

///////////////////////////////////

MachActor& MachLogTreacheryOperation::directObject()
{
	return *pDirectObject_;
}

const MachActor& MachLogTreacheryOperation::directObject() const
{
	return *pDirectObject_;
}

///////////////////////////////////

bool MachLogTreacheryOperation::doStart()
{
	////HAL_STREAM("(" << pActor_->id() << ") MLTreacheryOp::doStart checking for inside a building\n" );
	bool result = true;

	if( pDirectObject_ )
	{
		MachLogConstruction* pConstruction;
		bool insideABuilding = checkNeedLeaveOperation( pActor_, &pConstruction );
		//#ifndef NDEBUG
		//if( insideABuilding )
		//{
			//HAL_STREAM(" inside a building " << pConstruction->id() << std::endl );
			//HAL_STREAM(" Target contained in boundary " << pConstruction->globalBoundary().contains( pDirectObject_->position() ) << std::endl);
			//HAL_STREAM(" Target insideBuilding " << MachActor::asMachine( pDirectObject_ ).insideBuilding() << std::endl );
			//if( MachActor::asMachine( pDirectObject_ ).insideBuilding() )
				//HAL_STREAM(" Target insideWhichBuilding().id() " << MachActor::asMachine( pDirectObject_ ).insideWhichBuilding().id() << std::endl );
		//}
		//#endif
		if( insideABuilding and not pConstruction->globalBoundary().contains( pDirectObject_->position() ) )
		{
			//HAL_STREAM(" issuing leave Buolding operation.\n" );
			result = not checkNeedAndDoLeaveOperation( pActor_ );
		}

		//HAL_STREAM(" Setting current target for object\n" );
		if( pAgg_ )
			pAgg_->currentTarget( pDirectObject_ );
		else if( pAdm_ )
			pAdm_->currentTarget( pDirectObject_ );

	}

	return result;
}

MexRadians
angleToTurnToFace( const MachLogMachine& actor, const MexPoint3d& pos );

/* //////////////////////////////////////////////////////////////// */

PhysRelativeTime MachLogTreacheryOperation::doUpdate()
{
	PRE( not isFinished() );
	PRE( pActor_ != NULL );

    //Do nothing if delegated to a subop
	if( pSubOperation() )
		return 0.5;

    //Need a view of the subject actor as a canAttack
    MachLogCanAttack& attacker = pActor_->asCanAttack();

	//If the attacker is busy shooting something else right now, try again in a bit.
    //Otherwise ensure the direct object is the current target
    MachActor& target = directObject();
	if( attacker.hasCurrentTarget() )
    {
    	if( &attacker.currentTarget() != &target )
    		return 0.5;
    }
    else
        attacker.currentTarget( &target );

    //If we were moving to the side, and haven't arrived, try again later
    MachLogMachineMotionSequencer& motionSequencer = pActor_->motionSeq();
    if( lastAction_ == MOVE_TO_SIDE  and  motionSequencer.hasDestination() )
        return 0.5;

    //Set up useful local variables
   	PhysRelativeTime interval = 0.25;
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
    }

    //Check for attacker inside a building
    MachLogConstruction* pAttackerBuilding = NULL;

	if( pActor_->objectIsMachine() )
	{
		MachLogMachine& attackerMachine = pActor_->asMachine();
        if( attackerMachine.insideBuilding() )
            pAttackerBuilding = &(attackerMachine.insideWhichBuilding());
    }

    if( pTargetBuilding != pAttackerBuilding )
    {
        //If the target is inside a building, enter it
    	pActor_->motionSeq().stop();
        if( pTargetBuilding != NULL )
        {
            //Check attacker can legally enter the building
            if( pActor_->canEnterConstruction( *pTargetBuilding ) )
    			subOperation( pActor_, _NEW( MachLogEnterBuildingOperation( pActor_, pTargetBuilding, NULL ) ) );
            else
                targetBehindCover_ = true;
        }
        else
        {
            //Target is outside, and attacker is inside, so leave it
			subOperation( pActor_, _NEW( MachLogLeaveBuildingOperation( pActor_, pAttackerBuilding, NULL ) ) );
        }
		return 1.0;
    }

    //Check each weapon separately
    const MachLogCanAttack::Weapons& weapons = _CONST_CAST( const MachLogCanAttack&, attacker).weapons();

    for( MachLogCanAttack::Weapons::const_iterator it = weapons.begin(); it != weapons.end(); ++it )
    {
        //only process supercharger weapons
        MachLogWeapon& weapon = *(*it);
        const MachPhysWeapon& physWeapon = _CONST_CAST( const MachLogWeapon&, weapon).physWeapon();
        MachPhys::WeaponType type = weapon.type();
		if( type != MachPhys::ORB_OF_TREACHERY )
            continue;

        //Get the disposition of this weapon with respect to position (not cover)
        MexPoint3d globalAimPoint;

        const MexTransform3d& globalWeaponTransform = physWeapon.globalTransform();
        MachLogCanAttack::WeaponDisposition disposition =
            MachLogCanAttack::rangeData( globalWeaponTransform, targetObject, physWeapon.weaponData(),
                                         heightModifier, &globalAimPoint );

        //If in range, need to check for cover. Set up the global line from the weapon to the aim point
        if( disposition == MachLogCanAttack::IN_RANGE )
        {
            MexLine3d projectilePath( globalWeaponTransform.position(), globalAimPoint );
            disposition = attacker.behindCover( projectilePath, MachLogCanAttack::SAME_RACE_OK );
        }

        //If this weapon can fire, do so.
        if( disposition == MachLogCanAttack::IN_RANGE )
            interval = attacker.attackWithTreachery( &target );

        //Keep track of the most serious reason for not being able to fire
        if( disposition < worstDisposition )
            worstDisposition = disposition;
    }

    //We need to decide what action to take.
    Action action = WAIT_FOR_INTERVAL;

    //Set action depending on the worst disposition
    if( worstDisposition != MachLogCanAttack::IN_RANGE )
    {
        if( worstDisposition == MachLogCanAttack::NOT_IN_HORIZONTAL_ANGLE_RANGE )
        {
            //Weapons aren't facing in the right direction. If tracking, will be soon, otherwise
            //Need to turn the whole machine to face the target.
        	if( not attacker.canTurnHead() )
                action = TURN_TO_FACE;
        }
		else if( canMove )
		{
			if( worstDisposition == MachLogCanAttack::NOT_IN_DISTANCE_RANGE )
	        {
	            //Can't reach and could be closer, so move there
	        	action = MOVE_CLOSER;
	        }
	        else if( worstDisposition == MachLogCanAttack::NOT_IN_VERTICAL_ANGLE_RANGE )
	        {
	            //If weapons can tilt, but aren't at limits yet, we'll probably be in vertical range soon.
	            //Otherwise try to move in closer.
	        	if( attacker.atTiltLimits() )
	            	action = MOVE_CLOSER;
	        }
	        else if( worstDisposition == MachLogCanAttack::BEHIND_COVER )
	        {
	            //Hard cover. Get nearer.
	        	action = MOVE_CLOSER;
	        }
	        else if( worstDisposition == MachLogCanAttack::BEHIND_FRIENDLY_COVER )
	        {
	            //Soft cover. Move to one side.
	            action = MOVE_TO_SIDE;
	        }

		}
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

	return interval;
}

void MachLogTreacheryOperation::doFinish()
{

}

bool MachLogTreacheryOperation::doIsFinished() const
{
	//HAL_STREAM("(" << pActor_->id() << ") MLTreacheryOp::doIsFinsihed " );
	bool result = pDirectObject_ == NULL or targetBehindCover_ or pDirectObject_->race() == pActor_->race();
	//HAL_STREAM( result << std::endl );
	//if( result )
		//HAL_STREAM(" pDirectObject == NULL is " << ( pDirectObject_ == NULL ) << " targetBehindCover_ " << targetBehindCover_ << std::endl );
	return result;
}

bool MachLogTreacheryOperation::doBeInterrupted()
{
	//HAL_STREAM("(" << pActor_->id() << ") MLTreacheryOp::doBeInterrupted\n" );
	pActor_->motionSeq().stop();
	return not pActor_->motionSeq().hasDestination();
}

//virtual
bool MachLogTreacheryOperation::beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData )
{

	bool stayAttached = true;

	ASSERT( pSubject == (W4dSubject*)pDirectObject_, "Notified by unexpected subject." );
	ASSERT( currentlyAttached_, "Notified when object believed itself to be unattached." );

	switch( event )
	{
	case W4dSubject::DELETED:
		// if the current target is the direct object, cancel the target
		stopTargetting();
		pDirectObject_ = NULL;
		nextCallBackTime( SimManager::instance().currentTime() );
		currentlyAttached_ = false;
		break;
	case W4dSubject::CLIENT_SPECIFIC:
		switch( clientData )
		{
		case MachLog::TELEPORTED_OUT_OF_WORLD:
			stopTargetting();
			pDirectObject_ = NULL;
			nextCallBackTime( SimManager::instance().currentTime() );
			currentlyAttached_ = false;
			break;
		default:
			;
		}
		break;
	default:
		;
	}

	return currentlyAttached_;
}

void MachLogTreacheryOperation::stopTargetting()
{
	PRE( pDirectObject_ );

	pDirectObject_->removeThreat( pActor_->id() );

	MachLogCanAttack& attacker = pActor_->asCanAttack();
    if( &(attacker.currentTarget()) == pDirectObject_ )
    	attacker.currentTarget( NULL );
}

PhysRelativeTime MachLogTreacheryOperation::moveCloserToTarget()
{
	ASSERT( not pActor_->isStandingGround(), "Should never move closer if actor is standing ground." );

	if( not pDirectObject_ )
		return 0;

    PhysRelativeTime interval = 0.25;

	//If not moving, or the enemy has moved more than 40m set a new destination near the enemy
    MachActor& target = directObject();
	MexPoint2d targetPositionNow( target.position() );
	MexPoint2d attackerPositionNow( pActor_->position() );

	MATHEX_SCALAR distanceToTargetNow = attackerPositionNow.euclidianDistance( targetPositionNow );

	MachLogMachineMotionSequencer& motSeq = pActor_->motionSeq();

	bool recalculateDestination = false;

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
			if( distanceToTargetNow < 50
					and pDirectObject_->objectIsMachine() and pDirectObject_->asMachine().evading() )
				recalculateRatio = 0.90;

			if( distanceTargetMovedSinceAttackerChoseDestination / distanceToTargetNow > recalculateRatio )
				recalculateDestination = true;
		}
	}

    if( recalculateDestination )
    {
        //cache the target's position as we change our destination
        lastTargetPosition_ = targetPositionNow;

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

        bool foundSpace = false;
		MATHEX_SCALAR innerRadius = std::min( distanceToTargetNow, 30.0 );
		MATHEX_SCALAR extraSeparation = std::max( 10.0, distanceToTargetNow - innerRadius );

		foundSpace = MachLogSpacialManipulation::getNearestFreeSpacePoint( testTransform, innerRadius, useClearance, &attackPoint, extraSeparation );

        if( foundSpace )
        {
            if( groupMoveInfo().valid() )
                interval = motSeq.destination( attackPoint, groupMoveInfo() );
            else
                interval = motSeq.destination( attackPoint );

			pActor_->motionSeq().pathFindingPriority( pathFindingPriority() );

            //We might be in range before arriving, so limit the time we wait
            if( interval > 0.5 )
                interval = 0.5;
        }
        else
            interval = 1.0;
    }

    return interval;
}

PhysRelativeTime MachLogTreacheryOperation::turnToFaceTarget()
{
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

    return interval;
}

PhysRelativeTime MachLogTreacheryOperation::moveToSide()
{
    PhysRelativeTime interval = 0.5;

	//move sideways with respect to the target
	MexPoint2d toPoint( directObject().position() );
	if( pActor_->motionSeq().findAvoidancePoint( pActor_->position(), &toPoint ) )
    {
        if( groupMoveInfo().valid() )
            interval = pActor_->motionSeq().destination( toPoint, groupMoveInfo() );
        else
            interval = pActor_->motionSeq().destination( toPoint );

		pActor_->motionSeq().pathFindingPriority( pathFindingPriority() );
    }

    return interval;
}

void perWrite( PerOstream& ostr, const MachLogTreacheryOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pActor_;
	ostr << op.currentlyAttached_;
	ostr << op.pAgg_;
	ostr << op.pAdm_;
	ostr << op.pDirectObject_;
    ostr << op.lastTargetPosition_;
	ostr << op.targetBehindCover_;
}

void perRead( PerIstream& istr, MachLogTreacheryOperation& op )
{
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pActor_;
	istr >> op.currentlyAttached_;
	istr >> op.pAgg_;
	istr >> op.pAdm_;
	istr >> op.pDirectObject_;
    istr >> op.lastTargetPosition_;
	istr >> op.targetBehindCover_;
	if( op.currentlyAttached_ )
		op.pDirectObject_->attach( &op );

    op.lastAction_ = MachLogTreacheryOperation::WAIT_FOR_INTERVAL;
}

MachLogTreacheryOperation::MachLogTreacheryOperation( PerConstructor con )
:	MachLogOperation( con )
{
}

/* End OPERATIO.CPP *************************************************/
