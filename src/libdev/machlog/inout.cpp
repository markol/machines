/*
 * I N O U T . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#include <iostream>
#include "ctl/list.hpp"
#include "mathex/point2d.hpp"

#include "phys/motchunk.hpp"

#include "sim/manager.hpp"

#include "machphys/station.hpp"
#include "machphys/machdata.hpp"
#include "machphys/consdata.hpp"
#include "machphys/stations.hpp"
#include "machphys/random.hpp"
#include "machphys/machine.hpp"

#include "machlog/canattac.hpp"
#include "machlog/constron.hpp"
#include "machlog/inout.hpp"
#include "machlog/machine.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/entrance.hpp"
#include "machlog/planet.hpp"
#include "machlog/spacial.hpp"

#include "machlog/network.hpp"
#include "machlog/messbrok.hpp"
#include "machlog/move.hpp"
#include "machlog/cntrl.hpp"
#include "machlog/races.hpp"

#include "mathex/vec3.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/cvexpgon.hpp"
#include "mathex/circle2d.hpp"

PER_DEFINE_PERSISTENT( MachLogEnterBuildingOperation );
/* //////////////////////////////////////////////////////////////// */

MachLogEnterBuildingOperation::MachLogEnterBuildingOperation( MachLogMachine * pActor,
															MachLogConstruction * pConstr,
															MachPhysStation* pStation )
:	MachLogOperation( "ENTER_OPERATION", MachLogOperation::ENTER_OPERATION ),
	pActor_( pActor ),
  pConstruction_( pConstr ),
  pStation_( pStation ),
  status_( CLEAR ),
  currentlyAttached_( true ),
  buildingGone_( false ),
  ignoreNeedToClearEntrancePolygon_( false ),
  stationType_( 0 )

{
	PRE_INFO( "Actor trying to enter building: " << pActor->id() << std::endl );
    PRE_INFO( "Building they couldn't enter: " << pConstr->id() << std::endl );
    PRE( pActor->canEnterConstruction( *pConstr ) );

	pConstruction_->attach( this );
	HAL_STREAM("(" << pActor_->id() << ") MLEnterOp::CTOR\n" );
}

MachLogEnterBuildingOperation::MachLogEnterBuildingOperation( MachLogMachine * pActor,
															MachLogConstruction * pConstr,
															MachPhysStation* pStation,
                                                            PhysPathFindingPriority priority )
: MachLogOperation( "ENTER_OPERATION", MachLogOperation::ENTER_OPERATION, priority ),
  pActor_( pActor ),
  pConstruction_( pConstr ),
  pStation_( pStation ),
  status_( CLEAR ),
  currentlyAttached_( true ),
  buildingGone_( false ),
  ignoreNeedToClearEntrancePolygon_( false ),
  stationType_( 0 )

{
	PRE_INFO( "Actor trying to enter building: " << pActor->id() << std::endl );
    PRE_INFO( "Building they couldn't enter: " << pConstr->id() << std::endl );
    PRE( pActor->canEnterConstruction( *pConstr ) );

	pConstruction_->attach( this );
	HAL_STREAM("(" << pActor_->id() << ") MLEnterOp::CTOR\n" );
}

MachLogEnterBuildingOperation::~MachLogEnterBuildingOperation()
{
    if( buildingGone_ )
    {
    }
    else if( pActor_->insideBuilding() and &pActor_->insideWhichBuilding() == pConstruction_ )
    {
        //Machine got inside the right building. If not on the internal entrance polygon...
        if( not onInternalEntrancePolygon() )
        {
            //we no longer need to lock the entrance.
            if( pActor_->hasLockedEntrance() )
            {
                //We can't ignore the entrance polygon anymore
                pActor_->ignoreEntrancePolygon( false );

                pActor_->entranceLocked( NULL );
                pConstruction_->entrance( 0 ).unlock();
            }

        }
    }

    //If we didn't make it to the station, unlock it
    if( pStation_ != NULL  and  pActor_->hasStationLocked()  and  &pActor_->stationLocked() == pStation_  and
        MexPoint2d( pActor_->position() ).sqrEuclidianDistance( MexPoint2d( pStation_->position() ) ) > 0.0001 )
    {
        pActor_->stationLocked( NULL );
        pStation_->lock( false );
    }

	//Let the motionsequencer finish itself off. It can now auto-unlock entrances
    pActor_->motionSeq().keepEntranceLocked( false );
	pActor_->motionSeq().stop();
	pActor_->motionSeq().update( 0.001 );

	if( currentlyAttached_ )
		pConstruction_->detach( this );
}

void MachLogEnterBuildingOperation::doOutputOperator( ostream& o ) const
{
	o << "MachLogEnterBuildingOperation " << pConstruction_->id() << "[" << pConstruction_->objectType() << "]" << std::endl;
	o << " entrance( 0 ).locked() " << pConstruction_->entrance( 0 ).locked() << std::endl;
	o << " entrance( 0 ).open() " << pConstruction_->isEntranceOpen( 0 ) << std::endl;
	o << " external( 0 ).position() " << pConstruction_->entranceExternalPoint( 0 ) << std::endl;
	o << " internal( 0 ).position() " << pConstruction_->entranceInternalPoint( 0 ) << std::endl;
	o << " current position() " << pActor_->position() << std::endl;
	o << " motionSeq() is in RESTING state " << not pActor_->motionSeq().hasDestination() << std::endl;
	o << " pStation_ " << (void*)pStation_ << std::endl;
	if( pStation_ != NULL )
	{
		o << " pStation_->locked() " << pStation_->locked() << std::endl;
		o << " station position " << pStation_->position() << std::endl;
	}
	switch( status_ )
	{
		case INTERNAL_POINT:
			o << "INTERNAL_POINT\n";
			break;
		case STATION:
			o << "STATION\n";
			break;
		case GOING_TO_INTERNAL_POINT:
			o << "GOING_TO_INTERNAL_POINT\n";
			break;
		case EXTERNAL_POINT:
			o << "EXTERNAL_POINT\n";
			break;
		case CLEAR:
			o << "CLEAR\n";
			break;
		case GOING_TO_STATION:
			o << "GOING_TO_STATION\n";
			break;
		case GOING_TO_INTERIOR:
			o << "GOING_TO_INTERIOR\n";
			break;
		case GOING_TO_EXTERNAL_POINT:
			o << "GOING_TO_EXTERNAL_POINT\n";
			break;
		case TERMINATE_EARLY:
			o << "TERMINATE_EARLY\n";
			break;
		default:
			ASSERT_INFO( (int)status_ );
			ASSERT_BAD_CASE;
	}

}

///////////////////////////////////

bool MachLogEnterBuildingOperation::doStart()
{
    //If already in a different construction, we can't start until we have left it
	MachLogConstruction* pConstruction = NULL;
	bool insideABuilding = checkNeedLeaveOperation( pActor_, &pConstruction );
	if( insideABuilding and pConstruction->id() != pConstruction_->id() )
		return not checkNeedAndDoLeaveOperation( pActor_ );

    HAL_STREAM(" (" << pActor_->id() << ") MEnterBOp::doStart no leave necesary " << pConstruction_->objectType() << "\n" );

    //Ensure motion sequencer doesn't auto-unlock entrances while in this op
	MachLogMachineMotionSequencer& motseq = pActor_->motionSeq();
    motseq.keepEntranceLocked( true );

    //Determine what state to start in from current location
    if( insideABuilding )
    {
        //we're already inside the building. If on the entrance polygon, set state as if
        //at internal entrance point. Otherwise must be going to station.
        if( onInternalEntrancePolygon() )
            status_ = INTERNAL_POINT;
        else
            status_ = GOING_TO_STATION;
    }
    else if( pActor_->hasLockedEntrance()  and  &pActor_->lockedEntrance() == &pConstruction_->entrance( 0 ) )
    {
        //Already have the entrance locked. So act as if going to external point.
        status_ = GOING_TO_EXTERNAL_POINT;
    }
    else
        status_ = CLEAR;

	return true;
}

PhysRelativeTime MachLogEnterBuildingOperation::doUpdate()
{
    //Do nothing if delegated to a subop that's still going
	if( pSubOperation() )
		return 1.0;

    //get 2d versions of the current actor and station positions
	MexPoint2d actorPos( pActor_->position() );
	MexPoint2d stationPos;
	if( pStation_ )
        stationPos = pStation_->position();

    //Get other useful bits and pieces
    MachLogMachineMotionSequencer& mSeq = pActor_->motionSeq();
    bool moving = mSeq.hasDestination();

	HAL_STREAM(" (" << pActor_->id() << ") MLEnterBOp::doUpdate\n" );

    //This enumeration defines the actions we might want to take
    enum Action { NONE, MOVE_TO_STATION, MOVE_TO_INTERNAL_POINT, MOVE_TO_EXTERNAL_POINT };
    Action action = NONE;

    //Decide on the action
    PhysRelativeTime callBackInterval = 1.0;
    switch( status_ )
    {
        case STATION:
        {
            //We've arrived
            callBackInterval = 0.0;
            break;
        }

        case GOING_TO_STATION:
        {
            ASSERT( pStation_ != NULL, "" );
            //Check to see if we've arrived
            if( actorPos.sqrEuclidianDistance( stationPos ) < 0.0001 )
            {
                status_ = STATION;
                callBackInterval = 0.0;
                if( moving )
                    mSeq.stop();
            }
            else if( not moving )
                action = MOVE_TO_STATION;
            else
                callBackInterval = 1.0;

            break;
        }

        case GOING_TO_INTERIOR:
        {
            ASSERT( pStation_ == NULL, "" );
            if( not moving  and  onInternalEntrancePolygon() )
                action = MOVE_TO_STATION;

            break;
        }

        case INTERNAL_POINT:
        {
            //Stop if moving.
            if( moving )
                mSeq.stop();

            //Move to next bit
            action = MOVE_TO_STATION;
            break;
        }

        case GOING_TO_INTERNAL_POINT:
        {
            if( not moving )
            {
                //Have we arrived?
                if( actorPos.euclidianDistance( pConstruction_->entranceInternalPoint( 0 ) ) < 0.001 )
                {
                    status_ = INTERNAL_POINT;
                    action = MOVE_TO_STATION;
                }
                else
                    action = MOVE_TO_INTERNAL_POINT; //try again
            }

            break;
        }

        case EXTERNAL_POINT:
        {
            //Stop if moving.
            if( moving )
                mSeq.stop();

            //At the external point.
            action = MOVE_TO_INTERNAL_POINT;

            break;
        }

        case GOING_TO_EXTERNAL_POINT:
        {
            if( not moving )
            {
                //Have we arrived?
                if( actorPos.euclidianDistance( pConstruction_->entranceExternalPoint( 0 ) ) < 0.001 )
                {
                    status_ = EXTERNAL_POINT;
                    action = MOVE_TO_INTERNAL_POINT;
                }
                else
                    action = MOVE_TO_EXTERNAL_POINT; //try again
            }

            break;
        }

        case CLEAR:
        {
            if( not moving )
                action = MOVE_TO_EXTERNAL_POINT;

            break;
        }

        DEFAULT_ASSERT_BAD_CASE( int( status_ ) );
    }

    switch( action )
    {
        case NONE: break;

        case MOVE_TO_STATION:
        {
    		//shut the door
            if( pConstruction_->isEntranceOpen( 0 ) )
    	    	pConstruction_->isEntranceOpen( 0, false );

            //Ensure not moving
            if( moving )
        		mSeq.stop();

            //Switch config space, domain etc
            pActor_->insideWhichBuilding( pConstruction_ );

            //Ignore the internal entrance polygon for pathfinding, if we have the entrance locked.
            //If we have already got inside and off the entrance polygon, we don't need to.
            if( pActor_->hasLockedEntrance() )
                pActor_->ignoreEntrancePolygon( true );

            if( pStation_ != NULL )
            {
    			//Make an L-shaped move forwards until level with station, then onto station
    			MachLogMoveToOperation::Path path;
    			MexPoint2d dest2 = pConstruction_->entranceExternalPoint( 0 );
    			MexPoint2d dest3 = pConstruction_->entranceInternalPoint( 0 );
    			MexPoint2d dest4( stationPos );
    			MexPoint2d dest5( stationPos );

    			//check for the orientation of the building. Remember that buildings are axis aligned!
    			if( Mathex::abs( dest2.x() - dest3.x() ) < 0.01 )
    				dest4.x( dest2.x() );
    			else
    				dest4.y( dest2.y() );

    			path.push_back( dest4 );
    			path.push_back( dest5 );

    			subOperation( pActor_, _NEW( MachLogMoveToOperation( pActor_, path, false ) ) );
                callBackInterval = 5.0;
                status_ = GOING_TO_STATION;
            }
            else
            {
                //Try a move off the polygon a short way
				MexPoint2d point2mForwardInDirectionFaced( 2, 0 );
				pActor_->physObject().globalTransform().transform( &point2mForwardInDirectionFaced );
	    		if( pActor_->motionSeq().findAvoidancePoint( pActor_->position(), &point2mForwardInDirectionFaced ) )
                {
			    	subOperation( pActor_, _NEW( MachLogMoveToOperation( pActor_, point2mForwardInDirectionFaced, false ) ) );
                    callBackInterval = 2.0;
                    status_ = GOING_TO_INTERIOR;
                }
                else
                {
					// if we're on a mission from god to kill infidels inside this building, we don't care
					// about upsetting people by not getting out of the way.
					// also, if it's an enemy building, full stop, don't get out of the way
                	if( ( 	pActor_->race() != pConstruction_->race() )
						or
                		( 	pActor_->objectIsCanAttack()
                		and	pActor_->asCanAttack().hasCurrentTarget()
						and pActor_->asCanAttack().currentTarget().objectIsMachine()
						and pActor_->asCanAttack().currentTarget().asMachine().insideBuilding()
						and &(pActor_->asCanAttack().currentTarget().asMachine().insideWhichBuilding() ) == pConstruction_ ) )
					{
						// I'm glad you could follow that.
						// Don't bother stepping off the internal entrance polygon in order to terminate this op
						ignoreNeedToClearEntrancePolygon_ = true;
					}

					else
						callBackInterval = 3.0;
                }
            }

            break;
        }

        case MOVE_TO_INTERNAL_POINT:
        {
            ASSERT( pActor_->inBuildingOrOnPad(), "" );

    		//Ensure the door is opened
		    if( !pConstruction_->isEntranceOpen( 0 ) )
            {
			    pConstruction_->isEntranceOpen( 0, true );
                callBackInterval = 1.0;
            }
            else
    		{
                ASSERT( pActor_->hasLockedEntrance(), "Machine on pad but not locked entrance" );
                ASSERT( pStation_ == NULL  or  pStation_->locked(), "Moved inside without having locked station" );

                //Ignore the building obstacle polygon while we make the move to inside
    			pActor_->ignoreConstructionPolygon( true );

    			//Tell motion sequencer that we are entering another domain
                W4dDomain* pInteriorDomain = &pConstruction_->interiorDomain();
    			pActor_->motionSeq().useSpaceDomain( pInteriorDomain );
    			if( MachLogNetwork::instance().isNetworkGame() )
    			{
    				MachLogNetwork::instance().messageBroker().sendUseSpaceDomainMessage( pActor_->id(), pConstruction_->id() );
    			}

                //Post move op to internal point
    			MexPoint2d dest2 = pConstruction_->entranceInternalPoint( 0 );
    			MexPoint3d dest3( dest2.x(), dest2.y(), 0 );
    			subOperation( pActor_, _NEW( MachLogMoveToOperation( pActor_, dest3, false ) ) );
                status_ = GOING_TO_INTERNAL_POINT;
                callBackInterval = 3.0;
    		}

            break;
        }

        case MOVE_TO_EXTERNAL_POINT:
        {
            ASSERT( not pActor_->insideBuilding(), "" );

            // Are we close enough to move directly to the entrance point

        	const MexPoint2d externalLoc = pConstruction_->entranceExternalPoint( 0 );
        	const MATHEX_SCALAR sqrRangeToEntrance = actorPos.sqrEuclidianDistance( externalLoc );
            const MATHEX_SCALAR close = 35.0;

            if( sqrRangeToEntrance <= close * close )
            {
                //Close enough to attempt a move to the external entrance point.
                //Try to lock the entrance, if not already done so
        		if( pActor_->hasLockedEntrance()  or  pConstruction_->entrance( 0 ).lock() )
        		{
                    //Ok. If our station is locked, see if there is another free one of the same type.
                    //Otherwise we can't start the move.
        			bool doMove = true;
        			if( pStation_ )
        			{
        				if( pStation_->locked() and not pActor_->hasStationLocked() )
        				{
        					doMove = false;
        					MachPhysStation::Type type = pStation_->type();
        					MachPhysConstructionData& conData =
        					    _CONST_CAST( MachPhysConstructionData&, pConstruction_->constructionData() );

        					if( conData.stations().freeStation( type, &pStation_ ) )
        						doMove = true;
        				}
        			}

        			if( doMove )
        			{
        				//Lock the station, informing, machine as well
        				if( pStation_ )
                        {
        					pStation_->lock( true );
            				pActor_->stationLocked( pStation_ );
                        }

                        //Tell machine we have entrance locked
                        pActor_->inOrOnPadBuilding( pConstruction_ );
            			pActor_->entranceLocked( &pConstruction_->entrance( 0 ) );

                        //Get machine to ignore the external entrance polygon
            			pActor_->ignoreEntrancePolygon( true );

                        //Post move subop to external point
        				MexPoint2d dest2 = pConstruction_->entranceExternalPoint( 0 );
        				MexPoint3d dest3( dest2.x(), dest2.y(), 0 );
        				subOperation( pActor_, _NEW( MachLogMoveToOperation( pActor_, dest3, false ) ) );

        				status_ = GOING_TO_EXTERNAL_POINT;
                        callBackInterval = 5.0;
        			}
        			else
                    {
            			//we couldn't move due to station being busy and we locked entrance
            			//so unlock it again.
        				pConstruction_->entrance( 0 ).unlock();
                        status_ = CLEAR;
						if( MachLogRaces::instance().controller( pActor_->race() ).type() == MachLogController::AI_CONTROLLER )
							status_ = TERMINATE_EARLY;
                    }
        		}
                else
                {
                    //Entrance is locked. Try again in a bit.
                    callBackInterval = 5.0;
                    status_ = CLEAR;
                }
            }
            else
            {
                //Need to move closer to the entrance if not moving already
                if( not moving )
                {
            		MexPoint2d newDest;

            		if( MachLogPlanet::instance().configSpace().findSpace( actorPos, externalLoc, 8, 15, pActor_->obstacleFlags(), &newDest ) )
            			subOperation( pActor_, _NEW( MachLogMoveToOperation( pActor_, newDest ) ) );
            		else if( MachLogPlanet::instance().configSpace().findSpace( actorPos, externalLoc, 8, 25, pActor_->obstacleFlags(), &newDest ) )
            			subOperation( pActor_, _NEW( MachLogMoveToOperation( pActor_, newDest ) ) );
            		else if( MachLogPlanet::instance().configSpace().findSpace( actorPos, externalLoc, 8, 35, pActor_->obstacleFlags(), &newDest ) )
            			subOperation( pActor_, _NEW( MachLogMoveToOperation( pActor_, newDest ) ) );

                    //  Try the random findSpace variant
            		else if( MachLogPlanet::instance().configSpace().findSpace( externalLoc, 8, 15, pActor_->obstacleFlags(), &newDest ) )
            			subOperation( pActor_, _NEW( MachLogMoveToOperation( pActor_, newDest ) ) );
            		else if( MachLogPlanet::instance().configSpace().findSpace( externalLoc, 8, 25, pActor_->obstacleFlags(), &newDest ) )
            			subOperation( pActor_, _NEW( MachLogMoveToOperation( pActor_, newDest ) ) );
            		else if( MachLogPlanet::instance().configSpace().findSpace( externalLoc, 8, 35, pActor_->obstacleFlags(), &newDest ) )
            			subOperation( pActor_, _NEW( MachLogMoveToOperation( pActor_, newDest ) ) );
                }

                callBackInterval = 5.0;
                status_ = CLEAR;
            }
            break;
        }

        DEFAULT_ASSERT_BAD_CASE( action );
    }

    return callBackInterval;
}

void MachLogEnterBuildingOperation::doFinish()
{
    //It all happens in the dtor
}

bool MachLogEnterBuildingOperation::doBeInterrupted()
{
	return isInterruptable();
}

bool MachLogEnterBuildingOperation::doIsFinished() const
{
    //Must hav efinished if the building has been destroyed
	bool result = buildingGone_ or ( status_ == TERMINATE_EARLY );

    if( not result )
    {
        //If moving to a station, we must have got there and stopped moving
        if( pStation_ )
        {
        	result = not pActor_->motionSeq().hasDestination()  and
                     MexPoint2d( pActor_->position() ).sqrEuclidianDistance( MexPoint2d( pStation_->position() ) ) < 0.0001;
        }
        else
        {
            //No station, so ok if inside the building, not moving, and not on the entrance polygon
            result = not pActor_->motionSeq().hasDestination()  and
                     pActor_->insideBuilding()  and
                     ( not onInternalEntrancePolygon()
                       or  ignoreNeedToClearEntrancePolygon_ );
        }
    }

	return result;

//	MexPoint2d actorPos( pActor_->position().x(), pActor_->position().y() );
//	return actorPos == pConstruction_->entranceInternalPoint( 0 );
}

//virtual
bool MachLogEnterBuildingOperation::beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int )
{
	//HAL_STREAM("(" << pActor_->id() << ") MLEnterBuilding::beNotified\n" );
	switch( event )
	{
	case W4dSubject::DELETED:
	{
		if( (W4dSubject*)pConstruction_ == pSubject )
		{
			currentlyAttached_ = false;
			pActor_->stationLocked( NULL );
			pActor_->entranceLocked( NULL );
			buildingGone_ = true;
			nextCallBackTime( SimManager::instance().currentTime() );
			//HAL_STREAM(" target deleted returning false\n" );
			return false;
		}
	}
	default:
		;
	}
	//HAL_STREAM(" returning true\n" );
	return true;
}

//virtual
bool MachLogEnterBuildingOperation::isInterruptable()
{
	return status_ == CLEAR;
}

void perWrite( PerOstream& ostr, const MachLogEnterBuildingOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pActor_;
	ostr << op.pConstruction_;

	bool needToGetAStation = ( op.pStation_ != NULL );
	PER_WRITE_RAW_OBJECT( ostr, needToGetAStation );
	if( needToGetAStation )
	{
        MexPoint2d coords = op.pStation_->position();
		PER_WRITE_RAW_OBJECT( ostr, coords );
	}

    ostr << op.stationType_;
	ostr << op.status_;
	ostr << op.buildingGone_;
	ostr << op.currentlyAttached_;
	ostr << op.ignoreNeedToClearEntrancePolygon_;
}

void perRead( PerIstream& istr, MachLogEnterBuildingOperation& op )
{
	HAL_STREAM("MLEnterBuildingOp::perRead\n" );
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pActor_;
	istr >> op.pConstruction_;

	op.pStation_ = NULL;
	bool needToGetAStation;
	PER_READ_RAW_OBJECT( istr, needToGetAStation );
    MexPoint2d stationCoords;
    if( needToGetAStation )
    {
        PER_READ_RAW_OBJECT( istr, stationCoords );
    }

    istr >> op.stationType_;
	istr >> op.status_;
	istr >> op.buildingGone_;
	istr >> op.currentlyAttached_;
	istr >> op.ignoreNeedToClearEntrancePolygon_;

	if( op.currentlyAttached_ )
		op.pConstruction_->attach( &op );

    //Get the station if required
    if( needToGetAStation )
    {
        MachPhysConstructionData& data =
            _CONST_CAST( MachPhysConstructionData&, op.pConstruction_->constructionData() );
        bool found = data.stations().nearStation( stationCoords, &op.pStation_ );
        ASSERT( found, "Invalid station coords" );
    }
}

MachLogEnterBuildingOperation::MachLogEnterBuildingOperation( PerConstructor con )
:	MachLogOperation( con )
{
}

bool MachLogEnterBuildingOperation::onInternalEntrancePolygon() const
{
    //Construct a circle at actor's current position
    MexCircle2d testCircle( MexPoint2d( pActor_->position() ), pActor_->motionSeq().lowClearance() + 0.01 );

    //Get the interior entrance polygon
    const MexPolygon2d& interiorPolygon =
        pConstruction_->interiorConfigSpace().polygon( pConstruction_->entrance(0).interiorPolygonId() );

    //See if intersects the interior entrance polygon
    return testCircle.intersects( interiorPolygon );
}

const MachLogConstruction& MachLogEnterBuildingOperation::destination() const
{
	return *pConstruction_;
}

/* //////////////////////////////////////////////////////////////// */
PER_DEFINE_PERSISTENT( MachLogLeaveBuildingOperation );

MachLogLeaveBuildingOperation::MachLogLeaveBuildingOperation( MachLogMachine * pActor,
															MachLogConstruction * pConstr,
															MachPhysStation* pStation )
:	MachLogOperation( "LEAVE_OPERATION", MachLogOperation::LEAVE_OPERATION ),
	pActor_( pActor ),
  pConstruction_( pConstr ),
  pStation_( pStation ),
  currentlyAttached_( true ),
  buildingGone_( false ),
	stationType_( 0 ),
	status_( STATION )
{
	pConstruction_->attach( this );
}

MachLogLeaveBuildingOperation::~MachLogLeaveBuildingOperation( )
{
    if( buildingGone_ )
    {
    }
    else if( not pActor_->insideBuilding() )
    {
        //Machine got outside the building.
        //Check to see if not on external entrance polygon.
        if( not onExternalEntrancePolygon() )
        {
            //we no longer need to lock the entrance.
            if( pActor_->hasLockedEntrance() )
            {
                //We can't ignore the entrance polygon anymore
                pActor_->ignoreEntrancePolygon( false );

                //Free the entrance
                pActor_->entranceLocked( NULL );
                pConstruction_->entrance( 0 ).unlock();

                //Machine no longer needs to observe the building
                pActor_->inOrOnPadBuilding( NULL );
            }

        }

        //Clear any intersect relation we had with the building interior
        W4dDomain* pInteriorDomain = &pConstruction_->interiorDomain();
        W4dEntity* pPhysMachine = &pActor_->physMachine();
        if( pPhysMachine->intersects( *pInteriorDomain ) )
            pPhysMachine->intersects( pInteriorDomain, false );
    }

	//Let the motionsequencer finish itself off. Can now auto-unlock entrances.
    pActor_->motionSeq().useSpaceDomain( NULL );
    pActor_->motionSeq().keepEntranceLocked( false );
	pActor_->motionSeq().stop();
	pActor_->motionSeq().update( 0.001 );

	if( currentlyAttached_ )
		pConstruction_->detach( this );
}

void MachLogLeaveBuildingOperation::doOutputOperator( ostream& o ) const
{
	o << "MachLogLeaveBuildingOperation\n";
	o << " (void*)pSubOperation_ " << (void*)pSubOperation() << std::endl;
	o << " entrance( 0 ).locked() " << pConstruction_->entrance( 0 ).locked() << std::endl;
	o << " external( 0 ).position() " << pConstruction_->entranceExternalPoint( 0 ) << std::endl;
	o << " internal( 0 ).position() " << pConstruction_->entranceInternalPoint( 0 ) << std::endl;
	o << " entrance( 0 ).open() " << pConstruction_->isEntranceOpen( 0 ) << std::endl;
	o << " pStation_ " << (void*)pStation_ << std::endl;
	if( pStation_ != NULL )
		o << " pStation_->locked() " << pStation_->locked() << std::endl;

	o << "Status ";
	switch( status_ )
	{
		case STATION:
			o << "STATION\n";
			break;
		case INTERNAL_POINT:
			o << "INTERNAL_POINT\n";
			break;
		case GOING_TO_INTERNAL_POINT:
			o << "GOING_TO_INTERNAL_POINT\n";
			break;
		case EXTERNAL_POINT:
			o << "EXTERNAL_POINT\n";
			break;
		case GOING_TO_EXTERNAL_POINT:
			o << "GOING_TO_EXTERNAL_POINT\n";
			break;
		case GOING_CLEAR:
			o << "GOING_CLEAR\n";
			break;
		default:
			o << "UNKNOWN STATUS\n";
	}
}

///////////////////////////////////

bool MachLogLeaveBuildingOperation::doStart()
{
	////HAL_STREAM(" (" << pActor_->id() << ") MLeaveBOp::doStart()\n" );
    HAL_STREAM(" (" << pActor_->id() << ") MLLeaveBOp::doStart \n" );

    //Ensure motion sequencer doesn't auto-unlock entrances while in this op
	MachLogMachineMotionSequencer& motseq = pActor_->motionSeq();
    motseq.keepEntranceLocked( true );

    //Determine what state to start in from current location
    if( pActor_->insideBuilding() )
    {
        if( pActor_->hasLockedEntrance() )
            status_ = GOING_TO_INTERNAL_POINT;
        else
            status_ = STATION;
    }
    else if( onExternalEntrancePolygon() )
    {
        status_ = GOING_TO_EXTERNAL_POINT;
    }
    else
        status_ = CLEAR;

	return true;
}

PhysRelativeTime MachLogLeaveBuildingOperation::doUpdate()
{
    //Do nothing if waiting for a subop
	if( pSubOperation() )
    {
		return 1.0;
    }

    //Set up useful variables
	MexPoint2d actorPos( pActor_->position() );
    MachLogMachineMotionSequencer& mSeq = pActor_->motionSeq();
    bool moving = mSeq.hasDestination();

    //This enumeration defines the actions we might want to take
    enum Action { NONE, MOVE_TO_INTERNAL_POINT, MOVE_TO_EXTERNAL_POINT, MOVE_CLEAR };
    Action action = NONE;

    //Decide on the action
    PhysRelativeTime callBackInterval = 1.0;
    switch( status_ )
    {
        case STATION:
        {
            action = MOVE_TO_INTERNAL_POINT;
            break;
        }

	    case GOING_TO_INTERNAL_POINT:
	    {
            if( not moving )
            {
                //Have we arrived?
                if( actorPos.sqrEuclidianDistance( pConstruction_->entranceInternalPoint( 0 ) ) < 0.0001 )
                {
                    status_ = INTERNAL_POINT;
                    action = MOVE_TO_EXTERNAL_POINT;
                }
                else
                    action = MOVE_TO_INTERNAL_POINT; //try again
            }

	        break;
	    }

	    case INTERNAL_POINT:
        {
            //Stop if moving.
            if( moving )
                mSeq.stop();

            //Move to next bit
            action = MOVE_TO_EXTERNAL_POINT;
            break;
        }

	    case GOING_TO_EXTERNAL_POINT:
	    {
            if( not moving )
            {
                //Have we arrived?
                if( actorPos.sqrEuclidianDistance( pConstruction_->entranceExternalPoint( 0 ) ) < 0.0001 )
                {
                    status_ = EXTERNAL_POINT;
                    action = MOVE_CLEAR;
                }
                else
                    action = MOVE_TO_EXTERNAL_POINT; //try again
            }

	        break;
	    }

	    case EXTERNAL_POINT:
        {
            //Stop if moving.
            if( moving )
                mSeq.stop();

            //Move to next bit
            action = MOVE_CLEAR;
	        break;
        }


	    case GOING_CLEAR:
	    {
            if( not moving )
            {
                //See if clear of the external entrance polygon
                MexCircle2d testCircle( actorPos, mSeq.highClearance() + 1.0 );
                const MexPolygon2d& exteriorPolygon =
                    MachLogPlanet::instance().configSpace().polygon( pConstruction_->entrance( 0 ).exteriorPolygonId() );

                //If not clear try again to move clear. Otherwise, done.
                if( testCircle.intersects( exteriorPolygon ) )
                    action = MOVE_CLEAR;
                else
                {
                    status_ = CLEAR;
                    callBackInterval = 0.0;
                }
            }
	        break;
	    }

	    case CLEAR:
        {
            //We've arrived
            callBackInterval = 0.0;
	        break;
        }

        DEFAULT_ASSERT_BAD_CASE( int( status_ ) );
    }

    //Now take any specified action
    switch( action )
    {
        case MOVE_TO_INTERNAL_POINT:
        {
            //Ensure we get the entrance locked
    		if( pActor_->hasLockedEntrance()  or  pConstruction_->entrance( 0 ).lock() )
    		{
    			status_ = GOING_TO_INTERNAL_POINT;

                //Ensure machine knows entrance locked
    			pActor_->entranceLocked( &pConstruction_->entrance( 0 ) );

                //Ignore the internal entrance polygon
    			pActor_->ignoreEntrancePolygon( true );

                //Make absolutely sure not already moving
                if( mSeq.hasDestination() )
                    mSeq.stop();

                //Set up a 2-leg move to the point
    			MachLogMoveToOperation::Path path;
    			MexPoint2d dest2 = pConstruction_->entranceInternalPoint( 0 );
    			MexPoint2d dest3 = pConstruction_->entranceExternalPoint( 0 );
    			MexPoint2d dest4 = pActor_->position();

    			if( Mathex::abs( dest2.x() - dest3.x() ) < 0.01 )
    				dest4.x( dest2.x() );
    			else
    				dest4.y( dest2.y() );

                //Check the intermediate point is legal first
                if( mSeq.targetPositionContainedInSpace( dest4 ) )
        			path.push_back( dest4 );

    			path.push_back( dest2 );

    		    subOperation( pActor_, _NEW( MachLogMoveToOperation( pActor_, path, false ) ) );

                //If the machine has a station locked, unlock it
                if( pActor_->hasStationLocked() )
                {
                    pActor_->stationLocked().lock( false );
                    pActor_->stationLocked( NULL );

                }
    		}

            callBackInterval = 2.0;
            break;
        }

        case MOVE_TO_EXTERNAL_POINT:
        {
            ASSERT( pActor_->hasLockedEntrance(), "" );

            //Ensure the door is open
        	if( !pConstruction_->isEntranceOpen( 0 ) )
            {
		        pConstruction_->isEntranceOpen( 0, true );
                callBackInterval = 1.0;
            }
            else
            {
                //Ensure not moving
                if( mSeq.hasDestination() )
                    mSeq.stop();

                //Switch to outside space, and therefore become logically outside the building
                pActor_->insideWhichBuilding( NULL );

                //Ignore the exterior entrance polygon and the building obstacle
                pActor_->ignoreEntrancePolygon( true );
                pActor_->ignoreConstructionPolygon( true );

                //Set up intersect relation with the interior domain
                pActor_->physMachine().intersects( &pConstruction_->interiorDomain(), true );
                mSeq.useSpaceDomain( &pConstruction_->interiorDomain() );

        		//post subop to move to external point
        		MexPoint2d dest2 = pConstruction_->entranceExternalPoint( 0 );
        		MexPoint3d dest3( dest2.x(), dest2.y(), 0 );
			    subOperation( pActor_, _NEW( MachLogMoveToOperation( pActor_, dest3, false ) ) );

                status_ = GOING_TO_EXTERNAL_POINT;
                callBackInterval = 2.0;
            }

            break;
        }

        case MOVE_CLEAR:
        {
            //Inform the motion sequencer the interior domain is longer required
            mSeq.useSpaceDomain( NULL );

            //Close the door
			if( pConstruction_->isEntranceOpen( 0 ) )
				pConstruction_->isEntranceOpen( 0, false );

            //Stop ignoring the building obstacle
            pActor_->ignoreConstructionPolygon( false );

            //Move away from the outside to a point about 20m past the external point
	    	MexPoint2d internalLoc = pConstruction_->entranceInternalPoint( 0 );
	    	MexPoint2d externalLoc = pConstruction_->entranceExternalPoint( 0 );
	        MATHEX_SCALAR separation = internalLoc.euclidianDistance( externalLoc );
	        MexPoint2d destination( internalLoc, externalLoc, (separation + 20.0) / separation );

            if( MachLogSpacialManipulation::getNearestFreeSpacePoint( MexTransform3d( MexPoint3d( destination ) ),
                                                                      10.0, mSeq.highClearance(), &destination ) )
            {
    			subOperation( pActor_, _NEW( MachLogMoveToOperation( pActor_, destination, false ) ) );

                status_ = GOING_CLEAR;
                callBackInterval = 2.0;
            }
            else
                callBackInterval = 15.0; //oh dear. Will probably stay stuck

            break;
        }

        DEFAULT_ASSERT_BAD_CASE( action );
    }

    return callBackInterval;
}

void MachLogLeaveBuildingOperation::doFinish()
{
    //All happens in the dtor
}

bool MachLogLeaveBuildingOperation::doIsFinished() const
{
    //Must be finsihed if building no longer exists
    bool result = buildingGone_;

    if( not result )
    {
        //Check not contained in the building
    	MexPoint2d actorPos( pActor_->position() );

        if( not pConstruction_->globalBoundary().contains( actorPos ) )
        {
            //Not in global building boundary. Check not on external polygon.
            result = not onExternalEntrancePolygon();
        }
    }

    return result;
}

bool MachLogLeaveBuildingOperation::doBeInterrupted()
{
    return isInterruptable();
}

//virtual
bool MachLogLeaveBuildingOperation::beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int )
{
	//HAL_STREAM("(" << pActor_->id() << ") MLEnterBuilding::beNotified\n" );
	switch( event )
	{
	case W4dSubject::DELETED:
		if( (W4dSubject*)pConstruction_ == pSubject )
		{
			currentlyAttached_ = false;
			pActor_->stationLocked( NULL );
			pActor_->entranceLocked( NULL );
			buildingGone_ = true;
			nextCallBackTime( SimManager::instance().currentTime() );
		}
		//HAL_STREAM(" target deleted returning false\n" );
		return false;
	default:
		;
	}
	//HAL_STREAM(" returning true\n" );
	return true;
}

//virtual
bool MachLogLeaveBuildingOperation::isInterruptable()
{
	return status_ == CLEAR;
}

void perWrite( PerOstream& ostr, const MachLogLeaveBuildingOperation& op )
{
	const MachLogOperation& base1 = op;

	ostr << base1;
	ostr << op.pActor_;
	ostr << op.pConstruction_;

	bool needToGetAStation = ( op.pStation_ != NULL );
	PER_WRITE_RAW_OBJECT( ostr, needToGetAStation );
	if( needToGetAStation )
	{
        MexPoint2d coords = op.pStation_->position();
		PER_WRITE_RAW_OBJECT( ostr, coords );
	}

    ostr << op.stationType_;
	ostr << op.status_;
	ostr << op.buildingGone_;
	ostr << op.currentlyAttached_;
}

void perRead( PerIstream& istr, MachLogLeaveBuildingOperation& op )
{
	HAL_STREAM("MLLeaveBuildingOp::perRead\n" );
	MachLogOperation& base1 = op;

	istr >> base1;
	istr >> op.pActor_;
	istr >> op.pConstruction_;

	op.pStation_ = NULL;
	bool needToGetAStation;
	PER_READ_RAW_OBJECT( istr, needToGetAStation );
    MexPoint2d stationCoords;
    if( needToGetAStation )
    {
        PER_READ_RAW_OBJECT( istr, stationCoords );
    }

    istr >> op.stationType_;
	istr >> op.status_;
	istr >> op.buildingGone_;
	istr >> op.currentlyAttached_;

	if( op.currentlyAttached_ )
		op.pConstruction_->attach( &op );

    //Get the station if required
    if( needToGetAStation )
    {
        MachPhysConstructionData& data =
            _CONST_CAST( MachPhysConstructionData&, op.pConstruction_->constructionData() );
        bool found = data.stations().nearStation( stationCoords, &op.pStation_ );
        ASSERT( found, "Invalid station coords" );
    }
}


MachLogLeaveBuildingOperation::MachLogLeaveBuildingOperation( PerConstructor con )
:	MachLogOperation( con )
{
}

bool MachLogLeaveBuildingOperation::onExternalEntrancePolygon() const
{
    //Construct a circle at actor's current position
    MexCircle2d testCircle( MexPoint2d( pActor_->position() ), pActor_->motionSeq().highClearance() + 0.01 );

    //Get the exterior entrance polygon
    const MexPolygon2d& exteriorPolygon =
        MachLogPlanet::instance().configSpace().polygon( pConstruction_->entrance(0).exteriorPolygonId() );

    //See if intersects the interior entrance polygon
    return testCircle.intersects( exteriorPolygon );
}

ostream& operator <<( ostream& o, MachLogEnterBuildingOperation::Status s )
{
    switch( s )
    {
        case MachLogEnterBuildingOperation::STATION:
            o << "STATION";
            break;
        case MachLogEnterBuildingOperation::GOING_TO_STATION:
            o << "GOING_TO_STATION";
            break;
        case MachLogEnterBuildingOperation::GOING_TO_INTERIOR:
            o << "GOING_TO_INTERIOR";
            break;
        case MachLogEnterBuildingOperation::INTERNAL_POINT:
            o << "INTERNAL_POINT";
            break;
        case MachLogEnterBuildingOperation::GOING_TO_INTERNAL_POINT:
            o << "GOING_TO_INTERNAL_POINT";
            break;
        case MachLogEnterBuildingOperation::EXTERNAL_POINT:
            o << "EXTERNAL_POINT";
            break;
        case MachLogEnterBuildingOperation::GOING_TO_EXTERNAL_POINT:
            o << "GOING_TO_EXTERNAL_POINT";
            break;
        case MachLogEnterBuildingOperation::CLEAR:
            o << "CLEAR";
            break;
        default:
            o << "UNKNOWN";
            break;
    }

    return o;
}

/* //////////////////////////////////////////////////////////////// */
/* End INOUT.CPP ****************************************************/
