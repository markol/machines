/*
 * M C M O T S E Q . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#define _TEST_INVARIANTS

#include "machlog/mcmotseq.hpp"
#include "machlog/internal/seqdata.hpp"
#include "machlog/internal/seqdebug.hpp"
#include "base/base.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/point2d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/cvexpgon.hpp"
#include "mathex/abox2d.hpp"
#include "mathex/vec2.hpp"
#include "mathex/line2d.hpp"
#include "mathex/circle2d.hpp"
#include "mathex/sausag2d.hpp"
#include "mathex/abox3d.hpp"
#include "mathex/random.hpp"
#include "phys/cspace2.hpp"
#include "phys/cs2qwrit.hpp"
#include "phys/phys.hpp"
#include "phys/rampacce.hpp"
#include "world4d/domain.hpp"
#include "world4d/entity.hpp"
#include "sim/manager.hpp"
#include "machphys/machdata.hpp"
#include "machphys/machine.hpp"
#include "machphys/mcmovinf.hpp"
#include "machphys/plansurf.hpp"
#include "ctl/list.hpp"

#include "machlog/planet.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/mcconvoy.hpp"
#include "machlog/machine.hpp"
#include "machlog/races.hpp"
#include "machlog/constron.hpp"
#include "machlog/entrance.hpp"

#include "machlog/messbrok.hpp"
#include "machlog/network.hpp"

#include "profiler/profiler.hpp"
#include "profiler/stktrace.hpp"

#include "machlog/internal/collinfo.hpp"

#include <string>

#ifndef _INLINE
    #include "machlog/mcmotseq.ipp"
#endif

#pragma GCC diagnostic ignored "-Wunused-variable"

//////////////////////////////////////////////////////////////////////////////////////////

MachLogMachineMotionSequencer::MachLogMachineMotionSequencer(	MachLogMobile* pLogMobile, MachPhysMachine* pPhysMachine,
															    const MexPoint2d& /*initialPosition*/,
															    MATHEX_SCALAR highClearance, MATHEX_SCALAR lowClearance )
:   pImpl_( _NEW( MachLogMachineMotionSequencerData( pLogMobile->id() ) ) )
{
    pImpl_->pLogMobile_ = pLogMobile;

    LOG_ENTER( "MachLogMotionSequencer" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

	// Initialise data
	pConfigSpace_ = &( MachLogPlanet::instance().configSpace() );
    pSpaceDomain_ = NULL;
	highClearance_ = highClearance;
    lowClearance_ = lowClearance;
    useClearance_ = highClearance;
    pPhysMachine_ = pPhysMachine;
    pFollowSequencer_ = NULL ;
    nFollowers_ = 0 ;
    pConvoy_ = NULL ;
    internalState_ = INTERNAL_RESTING ;
    nPortalPointsDone_ = 0 ;
    nPathPointsDone_ = 0 ;
    pImpl_->restingObstacleExists_ = false;
    pImpl_->restingChunkExists_ = false ;
    pImpl_->hasQueuedDestination_ = false ;
    pImpl_->stopAsSoonAsPossible_ = false ;
    pImpl_->hasOnPortalPoint_  = false ;
    pImpl_->movedOffPath_ = false ;
    pImpl_->inPlanetSpace_ = true ;
    scheduledUpdateTime_ = 0.0 ;
    motionSucceeded_ = true ;
    allowMoveOutOfWay_ = true;
	lastClearMovingChunksTimeMessage_ = -1.0;

    PRE( pLogMobile != NULL );
    PRE( pPhysMachine != NULL );

    //Set reasonable vector allocation size
    path_.reserve( 8 );
    domainPath_.reserve( 8 );
	ignoreObstacleIds_.reserve( 4 );

    destinationTolerance( MachLogMachineMotionSequencerData::defaultDestinationTolerance() );

    //Construct a temporary polygon obstacle for the initial position
    addRestingObstacle();

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

MachLogMachineMotionSequencer::~MachLogMachineMotionSequencer()
{
    {
        //  The reason for this apparantly unnecessary set of braces
        //  is to ensure that the MachLogMotionSequencerEntryExit
        //  object (created by the LOG_ENTER macro) is destroyed
        //  before the _DELETE( pImpl_ ) call.
        //  If it isn't destroyed, it will try and write to the stream
        //  that has just been destroyed because the pimpl has been
        //  destroyed.

        LOG_ENTER( "~MachLogMotionSequencer" );

    	CB_MachLogMachineMotionSequencerData_DEPIMPL();

        TEST_INVARIANT;

        //If executing a domain pathFind, kill it off
        if( internalState_ == INTERNAL_PLANNING_DOMAIN_PATH )
            pConfigSpace_->removeDomainFindPath( domainFindPathId_ );

        //If executing a pathFind, kill it off
        if( internalState_ == INTERNAL_PLANNING )
            pConfigSpace_->removeFindPath( findPathId_ );

        //Remove any polygon obstacles registered in the config space
        clearRestingObstacle();
        removeMotionChunksWithoutEcho();

        //Ensure no one following this, and not following anything
        stopLeading();
        stopFollowing();

        LOG_STREAM( "pLogMobile = " << (void*)pLogMobile_ << std::endl );
        LOG_STREAM( "pPhysMachine = " << (void*)pPhysMachine_ << std::endl );

        //Ensure all followers have been removed, and not in a convoy
        ASSERT( nFollowers_ == 0, "" );
        ASSERT( pConvoy_ == NULL, "" );
    }

    _DELETE( pImpl_ );
}

//////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachineMotionSequencer::CLASS_INVARIANT
{
    LOG_ENTER( "CLASS_INVARIANT" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    INVARIANT( highClearance_ >= lowClearance_ );
    INVARIANT( pPhysMachine_ != NULL );
    INVARIANT( not pImpl_->hasQueuedDestination_ or ( internalState_ == INTERNAL_MOVING or internalState_ == INTERNAL_WANT_DOMAIN_PATH ) );
    INVARIANT( not pImpl_->stopAsSoonAsPossible_ or internalState_ == INTERNAL_MOVING );
    INVARIANT( pImpl_ != NULL );

    if( internalState_ == INTERNAL_MOVING )
    {
        INVARIANT( not pImpl_->restingChunkExists_ );
    }

	INVARIANT( motionChunkIds_.size() == moveInfos_.size() );

    INVARIANT( internalState_ != INTERNAL_PLANNING  or  pConfigSpace_->findPathExists( findPathId_ ) );
    INVARIANT( internalState_ != INTERNAL_PLANNING_DOMAIN_PATH  or
               pConfigSpace_->domainFindPathExists( domainFindPathId_ ) );
}
//////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const MachLogMachineMotionSequencer::InternalState& t )
{
    switch( t )
    {
        case MachLogMachineMotionSequencer::INTERNAL_RESTING: o << "RESTING"; break;
        case MachLogMachineMotionSequencer::INTERNAL_WANT_DOMAIN_PATH: o << "WANT_DOMAIN_PATH"; break;
        case MachLogMachineMotionSequencer::INTERNAL_PLANNING_DOMAIN_PATH: o << "PLANNING_DOMAIN_PATH"; break;
        case MachLogMachineMotionSequencer::INTERNAL_WANT_PATH: o << "WANT_PATH"; break;
        case MachLogMachineMotionSequencer::INTERNAL_PLANNING: o << "PLANNING"; break;
        case MachLogMachineMotionSequencer::INTERNAL_MOVING: o << "MOVING"; break;
        case MachLogMachineMotionSequencer::INTERNAL_WAITING: o << "WAITING"; break;
        case MachLogMachineMotionSequencer::INTERNAL_WAITING_FOR_NETWORK_CONFIRM: o << "WAITING_FOR_NETWORK_CONFIRM"; break;
        case MachLogMachineMotionSequencer::INTERNAL_1ST_PERSON: o << "1ST PERSON"; break;
    }
	return o;
}

ostream& operator <<( ostream& o, const MachLogMachineMotionSequencer::State& t )
{
    switch( t )
    {
        case MachLogMachineMotionSequencer::ARRIVED: o << "ARRIVED"; break;
        case MachLogMachineMotionSequencer::FAILED: o << "FAILED"; break;
        case MachLogMachineMotionSequencer::PLANNING: o << "PLANNING"; break;
        case MachLogMachineMotionSequencer::WAITING: o << "WAITING"; break;
        case MachLogMachineMotionSequencer::MOVING: o << "MOVING"; break;
        case MachLogMachineMotionSequencer::FIRST_PERSON: o << "FIRST PERSON"; break;
    }
	return o;
}

ostream& operator <<( ostream& o, const MachLogMachineMotionSequencer::FailureReason& t )
{
    switch( t )
    {
        case MachLogMachineMotionSequencer::NO_ROUTE: o << "NO_ROUTE"; break;
        case MachLogMachineMotionSequencer::DESTINATION_OCCUPIED: o << "DESTINATION_OCCUPIED"; break;
    }
	return o;
}

ostream& MachLogMachineMotionSequencer::streamOut( ostream& o ) const
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

	PhysConfigSpace2d::ObjectIds intersectingIds;
	getIntersectingIds( &intersectingIds );

    o << "MachLogMachineMotionSequencer " << (void*)this << " start" << std::endl;
    o << "  pPhysMachine " << (void*)pPhysMachine_;
    o << "  pLogMachine " << (void*)(MachLogMachine*)pLogMobile_;
    o << "  object id " << pLogMobile_->id() << std::endl;
    o << "  time " << SimManager::instance().currentTime() << std::endl;
    o << "  internal state " << internalState_;
    o << "  state " << state() << std::endl;

    if( pImpl_->logStreamOn_ )
    {
        o << "  Logging to file " << pImpl_->logFilePathName_ << std::endl;
    }

    o << "  failure reason ";
    if( state() == MachLogMachineMotionSequencer::FAILED )
        o << failureReason_ << std::endl;
    else
        o << std::endl;

    o << "  location  " << currentLocation();
    o << "  destination " << destinationPoint_ << std::endl;
    o << "  progress: domains " << nPortalPointsDone_ << "/" << domainPath_.size()
      << "  path " << nPathPointsDone_ << "/" << path_.size() << std::endl;
    o << "  hasQueuedDestination " << pImpl_->hasQueuedDestination_;
    o << "  stopAsSoonAsPossible " << pImpl_->stopAsSoonAsPossible_;
    o << "  moved off path " << pImpl_->movedOffPath_ << std::endl;
    if( pFollowSequencer_ != NULL )
    {
        o << "  following " << (void*)pFollowSequencer_ << " at " << followOffset_ << std::endl;
    }
    if( nFollowers_ != 0 )
        o << "  has " << nFollowers_ << " followers" << std::endl;
    if( pConvoy_ != NULL )
        o << "  convoy " << (void*)pConvoy_ << std::endl;

    o << "  scheduled update time " << scheduledUpdateTime_ << std::endl;
	o << "  motionChunkIds_.size " << pImpl_->motionChunkIds_.size() << std::endl;
    o << "  min height " << minHeight() << " max height " << maxHeight() << std::endl;
    o << "  resting obstacle exists " << pImpl_->restingObstacleExists_;
    o << "  resting chunk exists " << pImpl_->restingChunkExists_ << std::endl;
	o << "  intersecting ids";
	for( size_t i = 0; i < intersectingIds.size(); ++i )
	{
		o << "  " << intersectingIds[ i ];
	}
	o << std::endl;

    o << *(pImpl_);

    o << "MachLogMachineMotionSequencer " << (void*)this << " end" << std::endl;

	return o;
}

ostream& operator <<( ostream& o, const MachLogMachineMotionSequencer& t )
{
    return t.streamOut( o );
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysRelativeTime MachLogMachineMotionSequencer::destination( const MexPoint2d& newDestination )
{
    LOG_ENTER( "destination( newDestination )" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();
	PRE( implies( pLogMobile_->objectIsMachine(), not pLogMobile_->asMachine().isStandingGround() ) );

    return destination( newDestination, MachLogGroupMoveInfo() );
}

PhysRelativeTime MachLogMachineMotionSequencer::destination( const MexPoint2d& newDestination, const MachLogGroupMoveInfo& groupMoveInfo )
{
    LOG_ENTER( "destination( newDestination, groupMoveInfo )" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

	PRE( implies( pLogMobile_->objectIsMachine(), not pLogMobile_->asMachine().isStandingGround() ) );

    TEST_INVARIANT;

	LOG_STREAM( "Set destination to " << newDestination << std::endl );
    LOG_STREAM( "Group move info " << groupMoveInfo << std::endl );
	LOG_STREAM( *this );

    LOG_INSPECT( useClearance_ );
    LOG_INSPECT( lowClearance_ );
    LOG_INSPECT( highClearance_ );

    PRE_DATA( PhysConfigSpace2d::DomainId domainId );
	PRE_INFO( "pLogMobile id " << pLogMobile_->id() << " type " << pLogMobile_->objectType() << std::endl );
    if( pFollowSequencer_ != NULL )
    {
        PRE_INFO( "  following " << (void*)pFollowSequencer_ << " at " << followOffset_ << std::endl );
    }
    PRE_INFO( newDestination );
    PRE( pConfigSpace_->domain( newDestination, &domainId ) );
    PRE( pConfigSpace_->domain( currentLocation(), &domainId ) );

    #ifndef PRODUCTION
        if( not targetPositionContainedInSpace( newDestination ) )
        {
            BOB_STREAM( "About to fail precondition" << std::endl );
            BOB_INSPECT( newDestination );
            if( BOB_STREAM_ENABLED )
            {
                pConfigSpace_->traceObstacles( Diag::instance().bobStream() );
            }
        }
    #endif

    PRE( targetPositionContainedInSpace( newDestination ) );
    PRE( not is1stPersonControlled() );
	PRE( implies( pLogMobile_->objectIsMachine(), not pLogMobile_->asMachine().isStandingGround() ) );

    PhysRelativeTime interval = 0;

    //  This is a bit of defensive programming just in case a dodgy destination slips through.
    if( targetPositionContainedInSpace( newDestination ) )
    {
        pImpl_->groupMoveInfo_ = groupMoveInfo;
        pImpl_->offsetValid_ = false;


        //  Always reset the destination tolerance and pathfinding priority
        destinationTolerance( MachLogMachineMotionSequencerData::defaultDestinationTolerance() );
        pathFindingPriority( Phys::defaultPathFindingPriority() );

        //Before we can go somewhere else, we must stop current journey
        interval = stop();

        //Now see what we are doing
        if( internalState_ == INTERNAL_RESTING )
        {
            //  If we are really close to the destination don't bother moving at all
            MexPoint2d location( currentLocation() );
            if( location.sqrEuclidianDistance( newDestination ) > 0.0001 )
            {
                //Set the new destination
                destinationPoint_ = newDestination;

                //  Try a quick check to see if we can move directly to our
                //  destination - this might save us messing around with path
                //  finding.

                if( pFollowSequencer_ == NULL and straightLineMove( destinationPoint_, &interval ) )
                {
                    //  Don't have to do anything further - the straight
                    //  line move is good enough
                }
                else
                {
                    CHANGE_STATE( INTERNAL_WANT_DOMAIN_PATH, "new destination set" );
                    if( ( pFollowSequencer_ == NULL ) and isPlayerControlled() )
                        shuffle();
                }
            }
        }
        else if( internalState_ == INTERNAL_MOVING )
        {
            //We are still moving, so queue the new destination
            pImpl_->hasQueuedDestination_ = true;
            queuedDestination_ = newDestination;
        }
        else
        {
            //No other states are valid after stop()
            ASSERT_FAIL("");
        }
    }
    else
    {
        CHANGE_STATE( INTERNAL_RESTING, "destination was not in config space" );

        motionSucceeded_ = false;
        failureReason_ = DESTINATION_OCCUPIED;

        interval = 3.0;
    }

    LOG_STREAM( *this );

    //Reset the scheduled update time
    scheduledUpdateTime_ = SimManager::instance().currentTime() + interval;

    TEST_INVARIANT;

    return interval;
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachineMotionSequencer::createFindPath()
{
    LOG_ENTER( "createFindPath" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    //Cancel any current findPath if we have one
    if( internalState_ == INTERNAL_PLANNING ) {
        pConfigSpace_->removeFindPath( findPathId_ );
        CHANGE_STATE( INTERNAL_RESTING, "Current move canceled."); //Added bugfix
    }
    //get the current location
    MexPoint2d nowPoint = currentLocation();

    //Check to see if there is a straight line from current position to ultimate
    //destination which is contained in the config space. If so, scrap any intervening
    //domain path.
    size_t nDomainPathPoints = domainPath_.size();
    if( nDomainPathPoints != 0 )
    {
        PhysConfigSpace2d::PolygonIds badIds;
		ignorePolygons( true );

        if( pConfigSpace_->contains(
          MexSausage2d( nowPoint, destinationPoint_, useClearance_ ),
          obstacleFlags(), &badIds ) )
            nPortalPointsDone_ = nDomainPathPoints;

		ignorePolygons( false );
    }

    //Decide which point to find a path to. If still some more domain portals to
    //visit, get one from there. Otherwise use the destination point.
    MexPoint2d targetPoint;
    bool haveTargetPoint = false;

    while( nPortalPointsDone_ < nDomainPathPoints )
    {
        //Compute the coordinates of the next on-portal point
        if( not pImpl_->hasOnPortalPoint_ )
            getOnPortalPoint();

        //Check for already there
        if( onPortalPoint_.sqrEuclidianDistance( nowPoint ) < 0.0002 )
        {
            pImpl_->hasOnPortalPoint_ = false;
            ++nPortalPointsDone_;
        }
        else
        {
            targetPoint = onPortalPoint_;
            haveTargetPoint = true;
            break;
        }
    }

    //Use the final destination point if no more portal points
    if( not haveTargetPoint )
    {
        targetPoint = destinationPoint_;
        haveTargetPoint = targetPoint.sqrEuclidianDistance( nowPoint ) >= 0.0002;
		LOG_STREAM("(" << pLogMobile_->id() << ") McMotSeq::createFindPath have target point " << haveTargetPoint << " sqrEuclidain " << targetPoint.sqrEuclidianDistance( nowPoint ) << std::endl );
    }

    //Only do the path find if we have a target point
    if( haveTargetPoint )
    {
        //Check that the target point is attainable using the config space
        if( not targetPositionContainedInSpace( targetPoint ) )
        {
            LOG_STREAM("  Target in findpath not contained in config space (permanent) " << std::endl);
            //No, so use findspace to get a point we can reach
            MexPoint2d newTarget;
            haveTargetPoint = pConfigSpace_->findSpace( nowPoint, targetPoint,
                                                     useClearance_, 15.0,
                                                     obstacleFlags(),
                                                     &newTarget );
            //Check findspace succeeded
            if( haveTargetPoint )
            {
                //Store new target
                targetPoint = newTarget;

                //Check haven't generated the start point
                haveTargetPoint = targetPoint.sqrEuclidianDistance( nowPoint ) >= 0.0002;
            }
        }
    }

    //Make sure we still have somewhere to go
    if( haveTargetPoint )
    {
        PhysRelativeTime dummyInterval;

        if( pConvoy_ == NULL and
            pFollowSequencer_ == NULL and
            straightLineMove( targetPoint, &dummyInterval ) )
        {
            LOG_STREAM( "Managed to get a straight line path to " << targetPoint << std::endl );
        }
        else
        {
            //Set up the pathfind
            findPathId_ = pConfigSpace_->addFindPath( nowPoint, targetPoint, useClearance_, logMobile().obstacleFlags(), pImpl_->pathFindingPriority_ );
            pConfigSpace_->findPathIgnorePolygons( findPathId_, ignoreObstacleIds_ );

            //Enter planning state
            CHANGE_STATE( INTERNAL_PLANNING, "we have a target point" );

            LOG_STREAM("  Creating path find from " << currentLocation() << " to " << targetPoint << std::endl );
        }
    }
    else
    {
        //We've arrived. If following enter wait state, otherwise rest state
        CHANGE_STATE( (pFollowSequencer_ == NULL ? INTERNAL_RESTING : INTERNAL_WAITING),
          "arrived at target point" );
        clearOldPaths();
        pImpl_->clearCommandId();
		LOG_STREAM(" McMotSeq::createFindPath thinks we have arrived\n" );
    }
}
//////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachineMotionSequencer::hasDestination() const
{
	CB_DEPIMPL( InternalState, internalState_ );

    return internalState_ != INTERNAL_RESTING and internalState_ != INTERNAL_1ST_PERSON;
}
//////////////////////////////////////////////////////////////////////////////////////////

const MexPoint2d& MachLogMachineMotionSequencer::destination() const
{
    LOG_ENTER( "destination()" );

	CB_DEPIMPL( MexPoint2d, queuedDestination_ );
	CB_DEPIMPL( MexPoint2d, destinationPoint_ );

    PRE( hasDestination() );
    return ( pImpl_->hasQueuedDestination_ ? queuedDestination_ : destinationPoint_);
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysRelativeTime MachLogMachineMotionSequencer::stop()
{
    LOG_ENTER( "stop" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

	if( internalState_ == INTERNAL_WAITING_FOR_NETWORK_CONFIRM )
		return 1.0;

    TEST_INVARIANT;

    PhysRelativeTime interval = 0;

    //Remove any following relation
    stopFollowing();

    switch( internalState_ )
    {
        case INTERNAL_RESTING:
        {
            //Already stopped
            break;
        }

        case INTERNAL_WANT_DOMAIN_PATH:
        {
            //Clear any existing paths, and enter INTERNAL_RESTING state
            clearOldPaths();
            CHANGE_STATE( INTERNAL_RESTING, "MachLogMachineMotionSequencer::stop called" );
            motionSucceeded_ = true;
            break;
        }

        case INTERNAL_PLANNING_DOMAIN_PATH:
        {
            //Abort the path planning, and enter rest state
            pConfigSpace_->removeDomainFindPath( domainFindPathId_ );

            clearOldPaths();
            CHANGE_STATE( INTERNAL_RESTING, "MachLogMachineMotionSequencer::stop called" );
            motionSucceeded_ = true;
            break;
        }

        case INTERNAL_WANT_PATH:
        {
            //Just enter INTERNAL_RESTING state
            clearOldPaths();
            CHANGE_STATE( INTERNAL_RESTING, "MachLogMachineMotionSequencer::stop called" );
            motionSucceeded_ = true;
            break;
        }

        case INTERNAL_PLANNING:
        {
            //Abort the path planning, and enter rest state
            pConfigSpace_->removeFindPath( findPathId_ );
            clearOldPaths();
            CHANGE_STATE( INTERNAL_RESTING, "MachLogMachineMotionSequencer::stop called" );
            motionSucceeded_ = true;
            break;
        }

        case INTERNAL_MOVING:
        {
            //Try and stop dead
            interval = stopRequestWhileMoving();

            //If successful, enter resting state
            if( internalState_ != INTERNAL_MOVING )
            {
                clearOldPaths();
                CHANGE_STATE( INTERNAL_RESTING, "MachLogMachineMotionSequencer::stop called" );
                motionSucceeded_ = true;
            }
            break;
        }

        case INTERNAL_WAITING:
        {
            //Clear the current path and enter resting state
            clearOldPaths();
            CHANGE_STATE( INTERNAL_RESTING, "MachLogMachineMotionSequencer::stop called" );
            motionSucceeded_ = true;
            break;
        }

        case INTERNAL_1ST_PERSON:
        {
            //Nothing to do
            break;
        }

        default: ASSERT_BAD_CASE;
    }

    //Ensure we don't try and move off to a queued destination
    pImpl_->hasQueuedDestination_ = false;

    //Ensure we have a resting obstacle registered if not still moving
    if( internalState_ != INTERNAL_MOVING and
      not ( pImpl_->restingObstacleExists_ and pImpl_->restingChunkExists_ ) )
        addRestingObstacle();

    //Reset the scheduled update time
    scheduledUpdateTime_ = SimManager::instance().currentTime() + interval;

    pImpl_->clearCommandId();

    TEST_INVARIANT;

    return interval;
}
//////////////////////////////////////////////////////////////////////////////////////////

//  Perform a stop but keep the destination
PhysRelativeTime MachLogMachineMotionSequencer::stopKeepDestination()
{
    LOG_ENTER( "stopKeepDestination" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    TEST_INVARIANT;

	if( internalState_ == INTERNAL_WAITING_FOR_NETWORK_CONFIRM )
		return 1.0;

    PhysRelativeTime interval = 0;

    switch( internalState_ )
    {
        case INTERNAL_RESTING:
        {
            //Already stopped
            break;
        }

        case INTERNAL_WANT_DOMAIN_PATH:
        {
            //  Leave it wanting as it isn't currently moving
            break;
        }

        case INTERNAL_PLANNING_DOMAIN_PATH:
        {
            //  Leave it planning as it isn't currently moving
            break;
        }

        case INTERNAL_WANT_PATH:
        {
            //  Leave it wanting as it isn't currently moving
            break;
        }

        case INTERNAL_PLANNING:
        {
            //  Leave it planning as it isn't currently moving
            break;
        }

        case INTERNAL_MOVING:
        {
            //Try and stop dead
            interval = stopRequestWhileMoving();

            ASSERT( internalState_ != INTERNAL_MOVING, "" );

            if( internalState_ != INTERNAL_MOVING )
            {
                clearPath();
                //  We already have a valid domain path so get ourselves
                //  into the "looking for path" state.
                CHANGE_STATE( INTERNAL_WANT_PATH, "MachLogMachineMotionSequencer::stopKeepDestination called" );
            }

            break;
        }

        case INTERNAL_WAITING:
        {
            //  Leave it waiting as it isn't currently moving
            break;
        }

        case INTERNAL_1ST_PERSON:
        {
            // Nothing to do
            break;
        }

        default: ASSERT_BAD_CASE;
    }

    //Ensure we have a resting obstacle registered if not still moving
    if( internalState_ != INTERNAL_MOVING and
      not ( pImpl_->restingObstacleExists_ and pImpl_->restingChunkExists_ ) )
        addRestingObstacle();

    //Reset the scheduled update time
    scheduledUpdateTime_ = SimManager::instance().currentTime() + interval;

    pImpl_->stopAsSoonAsPossible_ = false;

    TEST_INVARIANT;

    return interval;
}

bool MachLogMachineMotionSequencer::isMoving() const
{
    LOG_ENTER( "isMoving" );

	CB_DEPIMPL( InternalState, internalState_ );

    return internalState_ == INTERNAL_MOVING;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachineMotionSequencer::isWaiting() const
{
    LOG_ENTER( "isWaiting" );

	CB_DEPIMPL( InternalState, internalState_ );

    return internalState_ == INTERNAL_WAITING;
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachineMotionSequencer::addRestingObstacle()
{
    LOG_ENTER( "addRestingObstacle" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

	clearRestingObstacleWithoutEcho();
    PRE( not pImpl_->restingObstacleExists_ );
    PRE( not pImpl_->restingChunkExists_ );

	//set the parent correctly otherwise the network can get inconsistencies.
	//use the none network version as we shall send through the transform in a few lines time...
	setDomainWithoutEcho();

    //Construct 2 points at this location
    MexPoint2d location2d( currentLocation() );
    MexPoint2d nearPoint( location2d.x() + 0.01, location2d.y() );

    //Construct a new polygon surrounding this position
    MexConvexPolygon2d* pPolygon =
        _NEW( MexConvexPolygon2d( location2d, nearPoint, useClearance_ ) );
    std::auto_ptr< MexPolygon2d > polygonAPtr( pPolygon );

    PhysAbsoluteTime timeNow = SimManager::instance().currentTime();

	MachLogNetwork& network = MachLogNetwork::instance();
	if( network.isNetworkGame() )
	{
		if( network.remoteStatus( pLogMobile_->race() ) == MachLogNetwork::LOCAL_PROCESS )
		{
			if( lastSetDomainTimeMessage_ == timeNow )
				network.messageBroker().sendAddRestingObstacleShortMessage( pLogMobile_->id() );
			else
				network.messageBroker().sendAddRestingObstacleMessage( pLogMobile_->id(), pLogMobile_->globalTransform(), internalState_ );
		}
	}
    //Add this to the config space
    restingObstacleId_ = pConfigSpace_->add( polygonAPtr, 3, MachLog::OBSTACLE_NORMAL, PhysConfigSpace2d::TEMPORARY );
    pImpl_->restingObstacleExists_ = true;

    LOG_INSPECT( location2d );
    LOG_INSPECT( useClearance_ );

    //Now construct a motion chunk as if we had moved from nearPoint to location2d,
    //and add this
    PhysMotionChunk motionChunk
    (
        nearPoint,
        location2d,
        useClearance_,
        PhysRampAcceleration( 0.0, 400.0, -400.0, 0.01, 0.005, 0.005 ),
        PhysAbsoluteTime( timeNow - 0.01 ),
        PhysRelativeTime( 0.0 ),
        minHeight(),
        maxHeight()
    );

    //Add the motion chunk to the space
    restingChunkId_ = pConfigSpace_->add( motionChunk, pLogMobile_->id() );
    pImpl_->restingChunkExists_ = true;

    //Update the intersect relations the machine has with neighbouring domains
    setIntersectingDomains();

    LOG_INSPECT( pImpl_->restingChunkExists_ );

    POST( pImpl_->restingChunkExists_ );
    //POST( machineHasMotionChunk() ); //TODO this seems to be incorrect, actors are added to the map id->ptr collection later
}
//////////////////////////////////////////////////////////////////////////////////////////
void MachLogMachineMotionSequencer::addRestingObstacleWithoutEcho( const MexTransform3d& newTransform )
{
    LOG_ENTER( "addRestingObstacleWithoutEcho" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

	clearRestingObstacleWithoutEcho();
	PRE_INFO( pLogMobile_->id() );
    PRE( not pImpl_->restingObstacleExists_ );
    PRE( not pImpl_->restingChunkExists_ );
	DEBUG_STREAM( DIAG_NETWORK, "(" << pLogMobile_->id() << ") addRestingObstacleWithoutEcho Transform to " << newTransform << std::endl );

	setDomain( newTransform );

    //Construct 2 points at this location
    MexPoint2d location2d( currentLocation() );
	DEBUG_STREAM( DIAG_NETWORK, "(" << pLogMobile_->id() << ") addRestingObstacleWithoutEcho currentLocation " << currentLocation() << std::endl );
    MexPoint2d nearPoint( location2d.x() + 0.01, location2d.y() );

    //Construct a new polygon surrounding this position
    MexConvexPolygon2d* pPolygon =
        _NEW( MexConvexPolygon2d( location2d, nearPoint, useClearance_ ) );
    std::auto_ptr< MexPolygon2d > polygonAPtr( pPolygon );

    //Add this to the config space
    restingObstacleId_ = pConfigSpace_->add( polygonAPtr, 3, MachLog::OBSTACLE_NORMAL, PhysConfigSpace2d::TEMPORARY );
    pImpl_->restingObstacleExists_ = true;

    //Now construct a motion chunk as if we had moved from nearPoint to location2d,
    //and add this
    PhysAbsoluteTime timeNow = SimManager::instance().currentTime();
    PhysMotionChunk motionChunk
    (
        nearPoint,
        location2d,
        useClearance_,
        PhysRampAcceleration( 0.0, 400.0, -400.0, 0.01, 0.005, 0.005 ),
        PhysAbsoluteTime( timeNow - 0.01 ),
        PhysRelativeTime( 0.0 ),
        minHeight(),
        maxHeight()
    );

    //Add the motion chunk to the space
    restingChunkId_ = pConfigSpace_->add( motionChunk, pLogMobile_->id() );
    pImpl_->restingChunkExists_ = true;

    //Update the intersect relations the machine has with neighbouring domains
    setIntersectingDomains();

    POST( pImpl_->restingChunkExists_ );
}

//////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachineMotionSequencer::changeStatus( int newStatus )
{
    LOG_ENTER( "changeStatus" );

	CB_DEPIMPL( InternalState, internalState_ );

    CHANGE_STATE( _STATIC_CAST( InternalState, newStatus ), "MachLogMachineMotionSequencer::change status called. Forced change of state" );
}

PhysRelativeTime MachLogMachineMotionSequencer::updateWhileStopped()
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_ENTER( "updateWhileStopped" );

    //This function must only be called when stopped, and the current path segment is
    //not complete
    PRE( internalState_ != INTERNAL_RESTING );

    PhysRelativeTime interval;
    MATHEX_SCALAR limit = 2.0 * useClearance_;

    //If following, we may want to change the current destination, etc
    if( pFollowSequencer_ != NULL )
    {
        //Only update the destination if we haven't just completed a path search
        if( internalState_ != INTERNAL_PLANNING )
            updateFollowDestination();
    }


    LOG_INSPECT( pImpl_->hasQueuedDestination_ );
    LOG_INSPECT( pImpl_->stopAsSoonAsPossible_ );
    LOG_INSPECT( pImpl_->movedOffPath_ );
    LOG_INSPECT( nPathPointsDone_ );
    LOG_INSPECT( path_.size() );
    LOG_INSPECT( nPortalPointsDone_ );
    LOG_INSPECT( domainPath_.size() );
    LOG_INSPECT( currentLocation() );
    LOG_INSPECT( destinationPoint_ );

    advancePathProgress();

    //Act on the various alternatives
    if( pImpl_->hasQueuedDestination_ )
    {
        //Scrap current plans and switch to new path
        clearOldPaths();
        destinationPoint_ = queuedDestination_;
        pImpl_->hasQueuedDestination_ = false;
        pImpl_->stopAsSoonAsPossible_ = false;
        CHANGE_STATE( INTERNAL_WANT_DOMAIN_PATH, "has queued destination" );
        interval = 0;
    }
    else if( pImpl_->stopAsSoonAsPossible_ )
    {
        //Just go to rest state
        clearOldPaths();
        CHANGE_STATE( INTERNAL_RESTING, "must stop ASAP" );
        motionSucceeded_ = true;
        interval = 0;
        pImpl_->stopAsSoonAsPossible_ = false;
    }
    else if( pImpl_->movedOffPath_ )
    {
        //Last move took us off the computed path. Clear the existing one, and
        //set up to recompute a new path to the on-portal point
        pImpl_->movedOffPath_ = false;
        clearPath();
        CHANGE_STATE( INTERNAL_WANT_PATH, "moved off path - trying to get back" );
        interval = PhysRelativeTime( 2 + (((int)(Phys::time() * 10000.0)) & 3) );
    }
    else if( nPathPointsDone_ < path_.size() )
    {
        LOG_WHERE;
        //Kick off move along next path segment
        interval = initiateMove();
        LOG_WHERE;
    }
    else if( nPortalPointsDone_ < domainPath_.size() )
    {
        //Need a new path to next portal
        CHANGE_STATE( INTERNAL_WANT_PATH, "getting path to next portal point" );
        interval = 0;
    }
    else if( currentLocation().sqrEuclidianDistance( destinationPoint_ ) > 0.0001 )
    {
        //Need a path direct to destination
        CHANGE_STATE( INTERNAL_WANT_DOMAIN_PATH, "have stopped, need to recalculate domain path" );
        interval = 0;
    }
    else
    {
        //We have arrived! If following, wait for a couple of seconds.
        clearOldPaths();
        if( pFollowSequencer_ != NULL )
        {
            CHANGE_STATE( INTERNAL_WAITING, "have arrived but we're following" );
            interval = 2;
        }
        else
        {
            CHANGE_STATE( INTERNAL_RESTING, "have arrived" );
            motionSucceeded_ = true;
            interval = 0;
        }

        pImpl_->clearCommandId();
    }

    return interval;
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysRelativeTime MachLogMachineMotionSequencer::updateWhileMoving()
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_ENTER( "updateWhileMoving" );
  	LOG_STREAM( "(" << pLogMobile_->id() << ") Enter MachLogMachineMotionSequencer::updateWhileMoving" << std::endl );
    PRE( internalState_ == INTERNAL_MOVING );
    PhysRelativeTime interval = 0.1;

    //Check to see if have reached arrival time
    PhysAbsoluteTime timeNow = SimManager::instance().currentTime();
    PhysRelativeTime timeLeft = arrivalTime_ - timeNow;
  	LOG_STREAM( " timeLeft " << timeLeft << std::endl );

    if( timeLeft <= 0.0 )
    {
        removeMotionChunks();

        //Update current domain
	    LOG_STREAM( " setDomain " );
        setDomain();

        //Update count of path and portal points done

	    LOG_STREAM( " advancePathProgress " );
        advancePathProgress();

        //Now decide what to do next
	    LOG_STREAM( " updateWhileStopped " << std::endl );
        interval = updateWhileStopped();
    }
    else if( pImpl_->stopAsSoonAsPossible_ )
    {
        //Try and stop dead. If successful we won't be in moving state any more,
        //and can do stop style update.
        interval = stopRequestWhileMoving();
        if( internalState_ != INTERNAL_MOVING )
        {
            clearOldPaths();
            pImpl_->stopAsSoonAsPossible_ = false;
            interval = updateWhileStopped();
            pImpl_->clearCommandId();
        }
    }
    else
    {
        //We haven't got there yet
        interval = timeLeft;
    }

    LOG_STREAM( "(" << pLogMobile_->id() << ") Exit MachLogMachineMotionSequencer::updateWhileMoving " << interval << std::endl );

    return interval;
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysRelativeTime MachLogMachineMotionSequencer::update( const PhysRelativeTime& maxCPUTime )
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_ENTER( "update" );

    LOG_WHERE;

    LOG_STREAM( PhysPathFindingQueueWriter( currentConfigSpace() ) );

    TEST_INVARIANT;
    PRE( not is1stPersonControlled() );
    PRE( machineHasMotionChunk() );

    LOG_STREAM( "Global transform for phys machine " << (void*)pPhysMachine_ << std::endl );
    LOG_STREAM( pPhysMachine_->globalTransform() );

    LOG_STREAM( "Local transform for phys machine " << (void*)pPhysMachine_ << std::endl );
    LOG_STREAM( pPhysMachine_->localTransform() );

	//check for update before the network confirm came back.
	if( internalState_ == INTERNAL_WAITING_FOR_NETWORK_CONFIRM )
		return 1.0;

    //Check we haven't been called too early, and return if so
    PhysAbsoluteTime timeNow = SimManager::instance().currentTime();
    PhysRelativeTime interval = scheduledUpdateTime_ - timeNow;

    LOG_STREAM( "Time now " << timeNow << std::endl );
    LOG_STREAM( "Interval " << interval << std::endl );
    LOG_STREAM( "maxCPUTime " << maxCPUTime << std::endl );

    if( interval <= 0.0 )
    {
        // Used to be 1.0. But that was stupid.

        //Set default callback time
        interval = 3.2;

        //Take action depending on state, until finished or run out of time
        PhysAbsoluteTime entryTime = Phys::time();
        PhysRelativeTime timeLeft = maxCPUTime;
        bool canDoMore = true;

        LOG_WHERE;
        do
        {
            LOG_STREAM( *this );
            LOG_WHERE;

            //Action depends on state
            switch( internalState_ )
            {
                case INTERNAL_RESTING:
                {
                    LOG_WHERE;
                    canDoMore = false;
                    break; //Nothing to do
                }

                case INTERNAL_WANT_DOMAIN_PATH:
                {
                    LOG_WHERE;
                    if( not targetPositionContainedInSpace( currentLocation() ) )
                    {
                        //  We are currently in an obstacle. Try and find a close
                        //  point that isn't inside an obstacle and move to that
                        //  before doing our proper move.

                        MexPoint2d newTarget;
                        if( pConfigSpace_->findSpace( currentLocation(), currentLocation(),
                          useClearance_, 15.0, obstacleFlags(), &newTarget ) )
                        {
                            //  Force the machine to the new position. It will teleport, however
                            //  this is the easiest way of ensuring that we get out of the obstacle
                            //  correctly.
                    		MexTransform3d newGlobalTransform( pPhysMachine_->globalTransform() );
                            const MATHEX_SCALAR newZ = MachLogPlanet::instance().surface()->terrainHeight( newTarget.x(), newTarget.y() );
                            MexPoint3d newPosition( newTarget.x(), newTarget.y(), newZ );

                            newGlobalTransform.position( newPosition );
                    		pPhysMachine_->globalTransform( newGlobalTransform );

                            LOG_STREAM( "In obstacle but have found close point to teleport to " << destinationPoint_ << std::endl );

							//	Make sure that the resting obstacle is correctly updated
							clearRestingObstacle();
							addRestingObstacle();

                            if( shuffling() )
                                stopShuffling();
                        }
                        else
                        {
                            CHANGE_STATE( INTERNAL_RESTING, "in obstacle and can't find a close point to escape to" );
                            motionSucceeded_ = false;
                        }
                    }
                    else
                    {
                        //If not already at the destination, set up a domain findpath algorithm
                        MexPoint2d location( currentLocation() );
                        if( location.sqrEuclidianDistance( destinationPoint_ ) > 0.0001 )
                        {
                            createDomainFindPath( location, destinationPoint_ );
                            CHANGE_STATE( INTERNAL_PLANNING_DOMAIN_PATH, "trying to get path to destination" );
                            interval = 0;
                        }
                        else
                        {
                            canDoMore = false;
                            clearOldPaths();
                            CHANGE_STATE( INTERNAL_RESTING, "reached destination" );
                            motionSucceeded_ = true;
                            pImpl_->clearCommandId();
                        }
                    }
                    break;
                }

                case INTERNAL_PLANNING_DOMAIN_PATH:
                {
                    //Update the domain findpath algorithm, and see if done
                    bool finished = pConfigSpace_->updateDomainFindPath( domainFindPathId_, timeLeft );

                    if( finished )
                    {
                        //Finished! Get the portal path.
                        bool finished = pConfigSpace_->domainFindPath( domainFindPathId_, &domainPath_ );
                        if( finished )
                        {
    						PhysConfigSpace2d::DomainId startDomain;
    						PhysConfigSpace2d::DomainId endDomain;
    						bool foundStart = pConfigSpace_->domain( currentLocation(), &startDomain );
    						bool foundEnd = pConfigSpace_->domain( destinationPoint_, &endDomain );

    						ASSERT_INFO( pLogMobile_->id() );
    						ASSERT_INFO( pLogMobile_->objectType() );
    						ASSERT_INFO( currentLocation() );
    						ASSERT_INFO( destinationPoint_ );
    						ASSERT_INFO( foundStart );
    						ASSERT_INFO( startDomain );
    						ASSERT_INFO( foundEnd );
    						ASSERT_INFO( endDomain );

                            LOG_STREAM("Found domain path:" << std::endl );

                            #ifndef PRODUCTION
                                writePortalPoints();
                            #endif

                            nPortalPointsDone_ = 0;
                            pImpl_->hasOnPortalPoint_ = false;
                            CHANGE_STATE( INTERNAL_WANT_PATH, "found domain path" );

                            //Advance the portal point count if already there etc
                            advancePathProgress();
                        }
                        else
                        {
                            CHANGE_STATE( INTERNAL_RESTING, "failed to find domain path" );

                            motionSucceeded_ = false;
                            failureReason_ = NO_ROUTE;
                        }
                    }

                    interval = 0; //Carry on ASAP

                    break;
                }

                case INTERNAL_WANT_PATH:
                {
                    //Check not already at the destination
                    MexPoint3d location;
                    pPhysMachine_->globalTransform().position( & location );
					LOG_STREAM("(" << pLogMobile_->id() << ") McMotSeq::Update on INTERNAL_WANT_PATH looking at distance to target\n loc " << location << " dest " << destinationPoint_ << std::endl );
					LOG_STREAM("fabs( location.x() - destinationPoint_.x() ) " << fabs( location.x() - destinationPoint_.x() ) << std::endl );
					LOG_STREAM("fabs( location.y() - destinationPoint_.y() ) " << fabs( location.y() - destinationPoint_.y() ) << std::endl );
                    if( fabs( location.x() - destinationPoint_.x() ) < 0.01 and
                        fabs( location.y() - destinationPoint_.y() ) < 0.01 )
                    {
                        clearOldPaths();
                        CHANGE_STATE( INTERNAL_RESTING, "reached destination" );
                        motionSucceeded_ = true;
                        canDoMore = false;
                        pImpl_->clearCommandId();
                    }
                    else
                    {
						LOG_STREAM(" creating a findPath as every thing is fine\n" );
                        //Create the findPath algorithm, and enter PLANNING state
                        createFindPath();
                    }
                    interval = 0;
                    break;
                }

                case INTERNAL_PLANNING:
                {
                    interval = updateWhilePlanning( timeLeft );
                    break;
                }

                case INTERNAL_MOVING:
                {
                    interval = updateWhileMoving();
                    break;
                }

                case INTERNAL_WAITING:
                {
                    interval = updateWhileStopped();
                    break;
                }

                default: ASSERT_BAD_CASE;
            }

            //How much time do we have left?
            timeLeft = maxCPUTime - (Phys::time() - entryTime);
        }
        while( canDoMore  and  interval == 0.0  and  timeLeft > 0 );

        LOG_WHERE;
        //If not moving, we must have a resting obstacle (if we are waiting for network messages then wait as well)
        if( internalState_ != INTERNAL_WAITING_FOR_NETWORK_CONFIRM and internalState_ != INTERNAL_MOVING )
        {
            //Add resting obstacle if one doesn't exist
            if( not pImpl_->restingObstacleExists_ )
                addRestingObstacle();

            //Unlock any locked entrances we might have moved off
            autoUnlockAnyLockedEntrance();
        }

        LOG_WHERE;
        //Store the scheduled update time
        scheduledUpdateTime_ = timeNow + interval;

        LOG_WHERE;
        LOG_STREAM( "timeLeft " << timeLeft << std::endl );
    }
    else
    {
        LOG_STREAM( "Not yet time for this object to be updated" << std::endl );
    }

    //  Comented out by Bob on 8 Feb 1999. It has been decided that we only
    //  want one shuffle per move. This is done when the destination is set.

//     if( internalState_ != INTERNAL_RESTING and
//         internalState_ != INTERNAL_MOVING and
//         internalState_ != INTERNAL_1ST_PERSON and
//         pFollowSequencer_ == NULL and
//         not shuffling() )
//     {
//         shuffle();
//     }

    //Ensure we have a resting obstacle registered if not still moving
    if( internalState_ != INTERNAL_MOVING and
      not ( pImpl_->restingObstacleExists_ and pImpl_->restingChunkExists_ ) )
        addRestingObstacle();

    LOG_WHERE;

    LOG_STREAM( *this );

    TEST_INVARIANT;

    POST_INFO( currentLocation() );
    POST_DATA( PhysConfigSpace2d::DomainId domainId );
    POST( pConfigSpace_->domain( currentLocation(), &domainId ) );

    POST( machineHasMotionChunk() );

    return interval;
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachineMotionSequencer::clearOldPaths()
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_ENTER( "clearOldPaths" );

    //Erase the paths
    domainPath_.erase( domainPath_.begin(), domainPath_.end() );
    clearPath();

    //reset related data
    nPathPointsDone_ = 0;
    nPortalPointsDone_ = 0;
    pImpl_->hasOnPortalPoint_ = false;
    pImpl_->movedOffPath_ = false;
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysRelativeTime MachLogMachineMotionSequencer::stopRequestWhileMoving()
{
    LOG_ENTER( "stopRequestWhileMoving" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();
    PRE( internalState_ == INTERNAL_MOVING );

    PhysRelativeTime interval = 0;

    stopDead();

    //Will stop when we arrive at current queued position
    return interval;
}
//////////////////////////////////////////////////////////////////////////////////////////

W4dDomain* MachLogMachineMotionSequencer::setDomain()
{
    LOG_ENTER( "setDomain" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

	DEBUG_STREAM( DIAG_NETWORK, "(" << pLogMobile_->id() << ") setDomain normal method\n" );
    //Get current domain
    W4dDomain* pDomain = pPhysMachine_->containingDomain();

 	DEBUG_STREAM( DIAG_NETWORK, " pDomain " << (void*)pDomain << std::endl );
    //Check we are in the outside world space
    if( pImpl_->inPlanetSpace_ )
    {
		DEBUG_STREAM( DIAG_NETWORK, " pImpl_->inPlanetSpace_ is TRUE\n" );
        //Get the machine's current global 2d position, and hence the domain it should be in
        W4dDomain* pCorrectDomain = MachLogPlanetDomains::pDomainAt( currentLocation() );

		DEBUG_STREAM( DIAG_NETWORK, " pCorrectDomain " << (void*)pCorrectDomain << std::endl );
        //If not attached to the right domain, attach it now
        if( pDomain != pCorrectDomain )
        {
            pPhysMachine_->attachTo( pCorrectDomain );
            pDomain = pCorrectDomain;
        }
		if( MachLogNetwork::instance().isNetworkGame() )
		{
			MachLogNetwork::instance().messageBroker().sendMachineSetDomainMessage( pLogMobile_->id(), pLogMobile_->globalTransform() );
			lastSetDomainTimeMessage_ = SimManager::instance().currentTime();
		}
    }

	//LOG_STREAM(" Exit setDomain\n" );

    return pDomain;
}

W4dDomain* MachLogMachineMotionSequencer::setDomainWithoutEcho()
{
    LOG_ENTER( "setDomainWithoutEcho" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

	DEBUG_STREAM( DIAG_NETWORK, "(" << pLogMobile_->id() << ") setDomain normal method\n" );
    //Get current domain
    W4dDomain* pDomain = pPhysMachine_->containingDomain();

 	DEBUG_STREAM( DIAG_NETWORK, " pDomain " << (void*)pDomain << std::endl );
    //Check we are in the outside world space
    if( pImpl_->inPlanetSpace_ )
    {
		DEBUG_STREAM( DIAG_NETWORK, " pImpl_->inPlanetSpace_ is TRUE\n" );
        //Get the machine's current global 2d position, and hence the domain it should be in
        W4dDomain* pCorrectDomain = MachLogPlanetDomains::pDomainAt( currentLocation() );

		DEBUG_STREAM( DIAG_NETWORK, " pCorrectDomain " << (void*)pCorrectDomain << std::endl );
        //If not attached to the right domain, attach it now
        if( pDomain != pCorrectDomain )
        {
            pPhysMachine_->attachTo( pCorrectDomain );
            pDomain = pCorrectDomain;
        }
    }

	//LOG_STREAM(" Exit setDomain\n" );

    return pDomain;
}

W4dDomain* MachLogMachineMotionSequencer::setDomain( const MexTransform3d& newGlobalTransform )
{
    LOG_ENTER( "setDomain" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

	//LOG_STREAM("(" << pLogMobile_->id() << ") setDomain get current domain\n" );
	DEBUG_STREAM( DIAG_NETWORK, "(" << pLogMobile_->id() << ") setDomain assign newGlobal\n" );
	DEBUG_STREAM( DIAG_NETWORK, "(" << pLogMobile_->id() << ") setDomain local before " << pPhysMachine_->localTransform() << std::endl );
    //Get current domain
    W4dDomain* pDomain = pPhysMachine_->containingDomain();
	DEBUG_STREAM( DIAG_NETWORK, " current domain " << (void*)pDomain << std::endl );

    //Check we are in the outside world space
    if( pImpl_->inPlanetSpace_ )
    {
		//LOG_STREAM(" pImpl_->inPlanetSpace_ is TRUE\n" );
        //Get the machine's current global 2d position, and hence the domain it should be in
        W4dDomain* pCorrectDomain = MachLogPlanetDomains::pDomainAt( newGlobalTransform.position() );
		DEBUG_STREAM( DIAG_NETWORK, " correct domain " << (void*)pCorrectDomain << std::endl );

        //If not attached to the right domain, attach it now
        if( pDomain != pCorrectDomain )
        {
          //LOG_STREAM( "  Changing domain seq = " << (void*)this << std::endl);
			//LOG_STREAM( " pCorrectDomain " << (void*)pCorrectDomain << std::endl );
            pPhysMachine_->attachTo( pCorrectDomain );
            pDomain = pCorrectDomain;
        }
		pPhysMachine_->globalTransform( newGlobalTransform );
    }

	//DEBUG_STREAM( DIAG_NETWORK, "(" << pLogMobile_->id() << ") setDomain global is now " << pLogMobile_->globalTransform() << std::endl );
	//DEBUG_STREAM( DIAG_NETWORK, "(" << pLogMobile_->id() << ") setDomain local is now " << pPhysMachine_->localTransform() << std::endl );
	//LOG_STREAM(" Exit setDomain\n" );

    return pDomain;
}

//////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachineMotionSequencer::setIntersectingDomains()
{
    LOG_ENTER( "setIntersectingDomains" );
	NETWORK_STREAM("MachLogMachineMotionSequencer::setIntersectingDomains\n" );
	NETWORK_INDENT( 2 );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    //Check we are in the outside world space
    if( pImpl_->inPlanetSpace_ )
    {
        //Clear all existing intersect relations
        clearIntersectingDomains();

        //Get current domain
        W4dDomain* pCurrentDomain = pPhysMachine_->containingDomain();

        //Set up collection for intersecting domains
        MachLogPlanetDomains::Domains domains;
        domains.reserve( 16 );

        //Add the domains intersecting the appropriate polygon
        if( pImpl_->restingObstacleExists_ )
        {
            //Get the resting obstacle boundary
            MexAlignedBox2d boundary;
            pConfigSpace_->polygon( restingObstacleId_ ).boundary( &boundary );

            //Get the list of domains it intersects with
            MachLogPlanetDomains::intersecting( boundary, &domains );
        }

		NETWORK_STREAM("motionChunkIds_.size " << motionChunkIds_.size() << std::endl );
        for( size_t i = 0; i < motionChunkIds_.size(); ++i )
        {
			NETWORK_STREAM("deref motionChunk id is " << motionChunkIds_[ i ] << std::endl );
			NETWORK_STREAM("motionChunkExists " << pConfigSpace_->motionChunkExists( motionChunkIds_[ i ] ) << std::endl );

            MexAlignedBox2d boundary;
            pConfigSpace_->motionChunk( motionChunkIds_[ i ] ).boundary( &boundary );

            //Get the list of domains it intersects with
            MachLogPlanetDomains::intersecting( boundary, &domains );
        }

        //If we have a specific space domain, we also need an intersect relation with it
        if( pSpaceDomain_ != NULL )
            domains.push_back( pSpaceDomain_ );

        //Set an intersect relation with each new domain
        for( MachLogPlanetDomains::Domains::iterator it = domains.begin();
             it != domains.end(); ++it )
        {
            W4dDomain* pExtraDomain = *it;
            if( pExtraDomain != pCurrentDomain  and
                not pPhysMachine_->intersects( *pExtraDomain ) )
            {
                pPhysMachine_->intersects( pExtraDomain, true );
            }
        }
    }

	NETWORK_INDENT( -2 );
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachineMotionSequencer::isPolygonIgnored( PhysConfigSpace2d::PolygonId id, bool doIgnore )
{
    LOG_ENTER( "isPolygonIgnored" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

	PRE( pConfigSpace_->exists( id ) );

	//Action depends on enabling/disabling
	if( doIgnore )
		ignoreObstacleIds_.push_back( id );
	else
	{
		//remove it from the list
		PhysConfigSpace2d::PolygonIds::iterator position;
		position = find( ignoreObstacleIds_.begin(), ignoreObstacleIds_.end(), id );
		ASSERT( position != ignoreObstacleIds_.end(), "");
		ignoreObstacleIds_.erase( position );
	}
}
//////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachineMotionSequencer::isPolygonIgnored( PhysConfigSpace2d::PolygonId id ) const
{
    LOG_ENTER( "isPolygonIgnored" );

	CB_DEPIMPL( const PhysConfigSpace2d::PolygonIds, ignoreObstacleIds_ );

	//See if in the collection
	PhysConfigSpace2d::PolygonIds::const_iterator position;
	position = find( ignoreObstacleIds_.begin(), ignoreObstacleIds_.end(), id );

	return position != ignoreObstacleIds_.end();
}
//////////////////////////////////////////////////////////////////////////////////////////

MexPoint2d MachLogMachineMotionSequencer::currentLocation() const
{
	CB_DEPIMPL( MachPhysMachine*, pPhysMachine_ );

    //Get the current global transform, and hence compute 2d location
    const MexTransform3d& currentPosition = pPhysMachine_->globalTransform();
    MexPoint3d currentLoc( currentPosition.position() );

    return MexPoint2d( currentLoc.x(), currentLoc.y() );
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachineMotionSequencer::createDomainFindPath
(
    const MexPoint2d& pathStart, const MexPoint2d& pathDestination
)
{
    LOG_ENTER( "createDomainFindPath" );

    CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_STREAM("  Creating domain findpath from " << pathStart << " to " << pathDestination << std::endl );

    //Cancel any current domain findPath if we have one
    if( internalState_ == INTERNAL_PLANNING_DOMAIN_PATH )
        pConfigSpace_->removeDomainFindPath( domainFindPathId_ );

    //Set up the pathfind

    domainFindPathId_ = pConfigSpace_->addDomainFindPath( pathStart, pathDestination, useClearance_, obstacleFlags(), pImpl_->pathFindingPriority_ );

    //Enter planning state
    CHANGE_STATE( INTERNAL_PLANNING_DOMAIN_PATH, "creating domain find path" );
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachineMotionSequencer::getOnPortalPoint()
{
    LOG_ENTER( "getOnPortalPoint" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    PRE( nPortalPointsDone_ < domainPath_.size() );
    PRE( not pImpl_->hasOnPortalPoint_ );

    //Extract the portal id and distance from the domain path
    const PortalPoint& portalPoint = domainPath_[ nPortalPointsDone_ ];
    PhysConfigSpace2d::PortalId portalId = portalPoint.first;
    MATHEX_SCALAR nominalDistance = portalPoint.second;
    MATHEX_SCALAR useDistance = nominalDistance;

    //Get the point
    onPortalPoint_ = pConfigSpace_->portalPoint( portalId, useDistance );

    LOG_STREAM( "Actual on portal point " << onPortalPoint_ << std::endl );

    if( pImpl_->groupMoveInfo_.valid() and ( not pImpl_->offsetValid_ ) )
    {
        pImpl_->calculateOffset( MexVec2(
          pPhysMachine_->globalTransform().position(),
          onPortalPoint_ ) );
    }

    if( pImpl_->offsetValid_ )
    {
        calculateGroupMovePortalPoint();
    }
    else
    {
        //  See if we can use a nearby random point instead to stop the
        //  portal points getting clogged
        MexPoint2d nearbyPoint;

        MexBasicRandom r;
        r.seedFromTime();
        const MATHEX_SCALAR radius = mexRandomScalar( &r, 0.0, useClearance_ * 10.0 );

        if( pConfigSpace_->findSpace( onPortalPoint_, useClearance_,
          radius, obstacleFlags(), &nearbyPoint ) )
        {
            LOG_STREAM( "Use portal point " << nearbyPoint << " rather than " << onPortalPoint_ << std::endl );

            onPortalPoint_ = nearbyPoint;
        }
    }

    LOG_STREAM( "Portal point used " << onPortalPoint_ << std::endl );

    pImpl_->hasOnPortalPoint_ = true;
}

//  Calculate the portal point to use if we are doing a group move
void MachLogMachineMotionSequencer::calculateGroupMovePortalPoint()
{
    LOG_ENTER( "calculateGroupMovePortalPoint" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    PRE( pImpl_->offsetValid_ );
    PRE( pImpl_->groupMoveInfo_.valid() );

    const MachLogGroupMoveInfo& groupMoveInfo = pImpl_->groupMoveInfo_;

    //  Try and place a circle with the same radius as the group onto the portal.

    const PortalPoint& portalPoint = domainPath_[ nPortalPointsDone_ ];
    const PhysConfigSpace2d::PortalId portalId = portalPoint.first;
    const MATHEX_SCALAR distance = portalPoint.second;
    const MATHEX_SCALAR portalLength = pConfigSpace_->portalLength( portalId );
    const MATHEX_SCALAR circleRadius = groupMoveInfo.groupRadius();

    MexPoint2d usePoint = currentConfigSpace().portalPoint( portalId, distance );

    if( portalLength > circleRadius )
    {
        MATHEX_SCALAR tryDistance;
        //  Take account of which end of the portal we're at
        if( distance > portalLength / 2 )
            tryDistance = portalLength - circleRadius;
        else
            tryDistance = circleRadius;

        MexPoint2d centre = pConfigSpace_->portalPoint( portalId, tryDistance );
        MexCircle2d testCircle( centre, circleRadius );
        PolygonId badId;
        if( currentConfigSpace().contains( testCircle, obstacleFlags(), &badId ) )
        {
            LOG_STREAM( "Found circle big enough for whole group" << std::endl );
            usePoint = centre;
        }
    }

    LOG_INSPECT( usePoint );

    //  usePoint now contains the point we are going to use as the centre of the group.

    //  We need to take some account of the rotation of the group of machines as it rounds
    //  corners. To do this we'll come up with an "average" direction of travel vector which is half
    //  way between our direction of travel arriving at this point and our direction of
    //  travel as we leave this point.

    MexPoint2d preceedingPoint = pPhysMachine_->globalTransform().position();
    MexPoint2d succeedingPoint;

    if( nPortalPointsDone_ + 1 < domainPath_.size() )
    {
        const PortalPoint& portalPoint = domainPath_[ nPortalPointsDone_ + 1 ];
        const PhysConfigSpace2d::PortalId portalId = portalPoint.first;
        const MATHEX_SCALAR distance = portalPoint.second;

        succeedingPoint = currentConfigSpace().portalPoint( portalId, distance );
    }
    else
        succeedingPoint = destination();

    LOG_INSPECT( preceedingPoint );
    LOG_INSPECT( succeedingPoint );

    MexVec2 inBoundVector( preceedingPoint, usePoint );
    MexVec2 outBoundVector( usePoint, succeedingPoint );

    inBoundVector.makeUnitVector();
    outBoundVector.makeUnitVector();

    MexVec2 averageVector = inBoundVector;
    averageVector += outBoundVector;
    averageVector.makeUnitVector();

    // We have our average vector, now rotate and scale it according to the
    // offsets for this machine and get out the final point we want the
    // machine to be aiming at

    averageVector.rotate( pImpl_->offsetAngle_ );
    averageVector *= pImpl_->offsetDistance_;

    usePoint += averageVector;

    //  Only use our new point if it doesn't intersect with any obstacles
    MexCircle2d testCircle( usePoint, useClearance_ );
    PolygonId badId;
    if( currentConfigSpace().contains( testCircle, obstacleFlags(), &badId ) )
        onPortalPoint_ = usePoint;
}

//////////////////////////////////////////////////////////////////////////////////////////

size_t MachLogMachineMotionSequencer::tryMoveSideways()
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_ENTER( "tryMoveSideways" );

    size_t  nChunksReserved = 0;

    const MexPoint2d  fromPoint2d( pPhysMachine_->globalTransform().position() );
    MexPoint2d  toPoint2d;

    if( path_.size() > 0 )
        toPoint2d = path_[ nPathPointsDone_ ];
    else
        toPoint2d = destinationPoint_;

    if( findAvoidancePoint( fromPoint2d, &toPoint2d ) )
    {
        const MATHEX_SCALAR startSpeed = 0.0;

        MoveInfoAPtr            dummyMoveInfoAPtr;
        MachLogCollisionInfo    dummyCollisionInfo;

        PhysConfigSpace2d::ObjectIds ignoreIds;
        ignoreIds.push_back( pLogMobile_->id() );

        if( reserveChunk(
          pPhysMachine_->globalTransform(),
          MachPhysMachine::USE_ORIENTATION,
          toPoint2d,
          SimManager::instance().currentTime(),
          startSpeed,
          MachPhys::AT_REST,
          DONT_CHECK_STATIC_OBSTACLES,
          ignoreIds,
          &dummyCollisionInfo,
          &dummyMoveInfoAPtr ) == SUCCESS )
        {
            nChunksReserved = 1;
        }
    }

    LOG_INSPECT( nChunksReserved );

    return nChunksReserved;
}

PhysRelativeTime MachLogMachineMotionSequencer::initiateMove()
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_ENTER( "initiateMove" );

    DEBUG_STREAM( DIAG_NETWORK, "(" << pLogMobile_->id() << ") MachLogMachineMotionSequencer::initiateMove " << std::endl );

    LOG_INSPECT( nPathPointsDone_ );
    LOG_STREAM( "Path:" << std::endl << path_ );

    PRE( nPathPointsDone_ < path_.size() );
    // PRE( not currently in motion );

    //Initialise the motion data
    PhysRelativeTime interval = 0;
    PhysAbsoluteTime timeNow = SimManager::instance().currentTime();
    const MexTransform3d& fromPosition = pPhysMachine_->globalTransform();
    MexPoint3d fromPoint3d = fromPosition.position();
    MexPoint2d fromPoint2d( fromPoint3d.x(), fromPoint3d.y() );

//    MexPoint2d targetPoint = path_[ nPathPointsDone_ ];
    MATHEX_SCALAR startSpeed = 0.0;
    MATHEX_SCALAR speedLimit = capSpeed();

//    LOG_STREAM("Initiate move from " << fromPoint2d << " to " << targetPoint << std::endl );
//    DEBUG_STREAM( DIAG_NETWORK, "  Initially from " << fromPoint2d << " to " << targetPoint << std::endl );

    // Try to add one or more motion chunks to the config space for proposed motion
    bool motionInitiated = false;

    ObjectId collisionObjectId;
    size_t nChunksReserved = reserveMotionChunks( &collisionObjectId );

    LOG_STREAM( "Reserved " << nChunksReserved << " chunks" << std::endl );

    motionInitiated = nChunksReserved > 0;

    LOG_INSPECT( motionInitiated );

    if( not motionInitiated )
    {
        bool done = false;
//            LOG_STREAM("Blocked by object " << collisionObjectId.asScalar() << " at " << collideTime << std::endl );

        const MexPoint2d& targetPoint = path_[ nPathPointsDone_ ];

        LOG_INSPECT( targetPoint );
        LOG_INSPECT( (void*)pConvoy_ );
        LOG_INSPECT( nPathPointsDone_ );
        LOG_INSPECT( path_.size() );
        LOG_INSPECT( objectIsResting( collisionObjectId ) );
        LOG_INSPECT( objectHasSameCommandId( collisionObjectId ) );

        //Check for trying to move to last point on current path, and the
        //obstacle being in resting state.
        if( pConvoy_ == NULL and
            nPathPointsDone_ >= (path_.size() - 1)  and
            objectIsResting( collisionObjectId ) and
            not objectHasSameCommandId( collisionObjectId ) )
        {
            LOG_INSPECT( nPortalPointsDone_ );
            LOG_INSPECT( domainPath_.size() );

            //Can take action if destination is final point
            if( nPortalPointsDone_ == domainPath_.size() )
            {
                //Check to see if we're only a short distance from this last point
                MATHEX_SCALAR sqrDistanceToTarget =
                    fromPoint2d.sqrEuclidianDistance( targetPoint );

                bool almostThere = sqrDistanceToTarget < sqrDestinationTolerance();
                LOG_STREAM("Almost there " << almostThere << std::endl );

                if( almostThere )
                {
                    CHANGE_STATE( INTERNAL_RESTING, "obstacle in way but we're almost there" );
                    motionSucceeded_ = true;
                }
                else
                {
                    LOG_STREAM( "Can we move blocking machine out of way?" << std::endl );

                    //  If we're being blocked by a machine of the same race
                    //  as us, try and move it out of the way

                    bool obstacleBeingMoved = false;

                    UtlId id = collisionObjectId.asScalar();
                    MachLogRaces& races = MachLogRaces::instance();
                    if( races.actorExists( id ) and
                      races.actor( id ).objectIsMachine() and
                      races.actor( id ).race() == logMobile().race() )
                    {
                        MachLogMachineMotionSequencer& obstructingSequencer =
                            races.actor( id ).asMobile().motionSeq();

                        LOG_STREAM( "Try move out of way" << std::endl );

                        //  Don't allow a recursive move out of way
                        allowMoveOutOfWay( false );

                        interval = obstructingSequencer.moveOutOfWay( currentLocation(),
                          targetPoint, useClearance_ );

                        allowMoveOutOfWay( true );

                        LOG_INSPECT( interval );

                        if( interval > 0 )
                            obstacleBeingMoved = true;
                    }

                    if( not obstacleBeingMoved )
                    {
                        //Try an avoidance move

                        nChunksReserved = tryMoveSideways();

                        LOG_INSPECT( nChunksReserved );

                        if( nChunksReserved > 0 )
                        {
                            pImpl_->movedOffPath_ = true;
                            motionInitiated = true;
                        }
                        else
                        {
                            //If we can't find an alternative destination,
                            //just stop.
                            MexPoint2d newDestination;
                            if( not pConfigSpace_->findSpace( fromPoint2d, targetPoint, useClearance_,
                                                              destinationTolerance_, obstacleFlags(), &newDestination ) )
                            {
                                CHANGE_STATE( INTERNAL_RESTING, "obstacle in way that won't move" );

                                motionSucceeded_ = false;
                                failureReason_ = DESTINATION_OCCUPIED;
                            }
                            else
                            {
                                //Change the destination
                                destinationPoint_ = newDestination;
                                CHANGE_STATE( INTERNAL_WANT_PATH, "immovable obstacle in way, trying an alternative destination" );
                                LOG_STREAM("    Re-entered INTERNAL_WANT_PATH with new destination " << newDestination << std::endl );
                            }
                        }
                    }
                }
                if( not motionInitiated )
                    clearOldPaths();

                interval = 0;
                done = true;
            }
        }

        if( not done )
        {
            //See if the obstructing object is a machine
            MachLogRaces& races = MachLogRaces::instance();
            UtlId actorId = collisionObjectId.asScalar();

            if( races.actorExists( actorId )  and
                races.actor( actorId ).objectIsMachine() )
            {
                MachLogMachineMotionSequencer* pObstructingSequencer
                    = &(races.actor( actorId ).asMobile().motionSeq());

                bool tryMove = true;

                // If we are in a convoy, only move other machines in the same
                // convoy out of the way if I am the leader

                if( pConvoy_ and pConvoy_ == pObstructingSequencer->pConvoy() )
                {
                    if( nFollowers_ != 0 )
                        tryMove = true;
                    else
                        tryMove = false;
                }

                if( tryMove )
                {
                    //  Don't allow a recursive move out of way
                    allowMoveOutOfWay( false );

                    interval = pObstructingSequencer->moveOutOfWay( fromPoint2d,
                                                                    targetPoint,
                                                                    useClearance_ );

                    allowMoveOutOfWay( true );

                    LOG_STREAM("  Move out of way returns " << interval << std::endl );

                    if( interval > 0 )
                        done = true;
                }
            }
        }

        if( not done )
        {
            //  If we're being blocked by a machine that has the same
            //  command id as we do don't try and move out of the way
            if( objectHasSameCommandId( collisionObjectId ) )
            {
                LOG_STREAM( "Obstructing obstacle has same command id" << std::endl );
                done = true;
            }
        }

        if( not done )
        {
            //Try an avoidance move

            LOG_STREAM( "Trying an avoidance move" << std::endl );

            nChunksReserved = tryMoveSideways();

            if( nChunksReserved > 0 )
            {
                pImpl_->movedOffPath_ = true;
                motionInitiated = true;
            }
        }
    }

    //Check for success
    if( motionInitiated )
    {
        LOG_STREAM( "  Success!" << std::endl );

        ASSERT( nPathPointsDone_ > 0, "" );

        interval = initiatePhysicalMotion();
    }

    //If we didn't move, and we haven't chosen a new destination, just wait for a bit
    if( internalState_ != INTERNAL_RESTING and
        not ( motionInitiated  or  internalState_ == INTERNAL_WANT_PATH) )
    {
        //Enter wait state
        CHANGE_STATE( INTERNAL_WAITING, "didn't move and didn't chose a new destination so waiting" );

        LOG_STREAM( "  Enter INTERNAL_WAITING state" << std::endl );

        //Ensure we have a resting obstacle
        if( not pImpl_->restingObstacleExists_ )
            addRestingObstacle();

        //Try again in a bit
        if( interval == 0.0 )
            interval = 2;
    }

    LOG_STREAM( "Finish initiate move " << (void*)this << std::endl );

    LOG_STREAM("MachLogMachineMotionSequencer::initiateMove exit" << std::endl );

    return interval;
}
//////////////////////////////////////////////////////////////////////////////////////////

//  Return true iff the given object id has the same command id as we do
bool MachLogMachineMotionSequencer::objectHasSameCommandId( ObjectId collisionObjectId )
{
    LOG_ENTER( "objectHasSameCommandId" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    bool    result = false;

    UtlId id = collisionObjectId.asScalar();
    MachLogRaces& races = MachLogRaces::instance();
    if( races.actorExists( id ) and
      races.actor( id ).objectIsMachine() )
    {
        MachLogMachineMotionSequencer& obstructingSequencer =
            races.actor( id ).asMobile().motionSeq();

        if( commandIdSet() and obstructingSequencer.commandIdSet() and
          commandId() == obstructingSequencer.commandId() )
            result = true;
    }

    return result;
}

PhysRelativeTime MachLogMachineMotionSequencer::setupMotion()
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_ENTER( "setupMotion" );

    //remove any resting obstacle/chunk
    clearRestingObstacle();

    //Update the intersecting domains which we may cross while moving
    setIntersectingDomains();

    const PhysAbsoluteTime startTime = moveInfos_[ 0 ].startTime();
    const PhysRelativeTime interval = sendMotionToPhysicalMachine();

    //Store the time of arrival
    arrivalTime_ = startTime + interval;

	if( internalState_ == INTERNAL_WAITING_FOR_NETWORK_CONFIRM )
		return 1.0;

    //Enter appropriate state
    CHANGE_STATE( INTERNAL_MOVING, "setting up the motion" );

    //Inform any machines following this one
    notifyFollowers();

    return interval;
}

bool MachLogMachineMotionSequencer::objectIsResting( const ObjectId& collisionObjectId )
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_ENTER( "objectIsResting" );

    bool answer = false;

    //Get the motion sequencer for the object in the way
    UtlId id = collisionObjectId.asScalar();
    MachLogRaces& races = MachLogRaces::instance();
    if( races.actorExists( id )  and  races.actor( id ).objectIsMachine() )
    {
        MachLogMachineMotionSequencer& obstructingSequencer =
            races.actor( id ).asMobile().motionSeq();

        LOG_INSPECT( obstructingSequencer.internalState() );

        //Check for resting state
        answer = obstructingSequencer.internalState() == INTERNAL_RESTING or
                 obstructingSequencer.internalState() == INTERNAL_WAITING;
    }

    LOG_INSPECT( answer );

    return answer;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachineMotionSequencer::objectInSameConvoy( const ObjectId& collisionObjectId )
{
    LOG_ENTER( "objectInSameConvoy" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    bool answer = false;

    //Get the motion sequencer for the object in the way
    if( pConvoy_ != NULL )
    {
        UtlId id = collisionObjectId.asScalar();
        MachLogRaces& races = MachLogRaces::instance();

        if( races.actorExists( id )  and  races.actor( id ).objectIsMachine() )
        {
            MachLogMachineMotionSequencer& obstructingSequencer =
                races.actor( id ).asMobile().motionSeq();

            //Check for same convoy
            answer = obstructingSequencer.pConvoy() == pConvoy_;
        }
    }

    return answer;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachineMotionSequencer::tryNearerPoint
(
    const MexPoint2d& fromPoint, const MexPoint2d& toPoint,
    const MexPoint2d& collisionPoint, MexPoint2d* pNewPoint
)
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_ENTER( "tryNearerPoint" );
    LOG_INSPECT( fromPoint );
    LOG_INSPECT( toPoint );
    LOG_INSPECT( collisionPoint );

    //We can move closer if the distance to the collide point is at least 3 times the clearance
    MATHEX_SCALAR sqrDistanceToCollision = collisionPoint.sqrEuclidianDistance( fromPoint );
    MATHEX_SCALAR sqrLimit = 3.0 * useClearance_;
    sqrLimit *= sqrLimit;
    bool canMoveCloser = sqrDistanceToCollision > sqrLimit;

    if( canMoveCloser )
    {
        //try a point 2 * clearance nearer than the collision
        MATHEX_SCALAR distanceToCollision = sqrt( sqrDistanceToCollision );
        MATHEX_SCALAR f = (distanceToCollision - 2.0 * useClearance_) / distanceToCollision;
        *pNewPoint = MexPoint2d( fromPoint, collisionPoint, f );
    }

    LOG_INSPECT( canMoveCloser );
    LOG_INSPECT( *pNewPoint );

    return canMoveCloser;
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachineMotionSequencer::addToConvoy( MachLogMachineConvoy* pConvoy )
{
    LOG_ENTER( "addToConvoy" );

	CB_DEPIMPL( MachLogMachineConvoy*, pConvoy_ );

    PRE( pConvoy != NULL );
    PRE( pConvoy_ == NULL );

    pConvoy->add( this );
    pConvoy_ = pConvoy;
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachineMotionSequencer::removeFromConvoy()
{
    LOG_ENTER( "removeFromConvoy" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    PRE( nFollowers_ == 0 );
    PRE( pFollowSequencer_ == NULL );

    //Check in one
    if( pConvoy_ != NULL )
    {
        //Remove this one
        pConvoy_->remove( this );

        //Delete the convoy if it no longer has any members
        if( pConvoy_->sequencers().size() == 0 )
            _DELETE( pConvoy_ );

        pConvoy_ = NULL;
    }
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachineMotionSequencer::follow
(
    MachLogMobile* pLeadMobile, const MexPoint2d& offset
)
{
    LOG_ENTER( "follow" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    PRE( pLeadMobile != NULL );

    //Remove any existing follow relationship
    if( pFollowSequencer_ != NULL )
        stopFollowing();

    //Get the new leader's motion sequencer, and any existing convoy pointer
    MachLogMachineMotionSequencer& leadSequencer = pLeadMobile->motionSeq();
    MachLogMachineConvoy* pLeadConvoy = leadSequencer.pConvoy();

    //This machine may still be in a convoy if it has followers
    if( pConvoy_ == NULL )
    {
        //Ensure the lead machine has a convoy
        if( pLeadConvoy == NULL )
            leadSequencer.pConvoy() = pLeadConvoy = _NEW( MachLogMachineConvoy( &leadSequencer ) );

        //Add this machine to the convoy
        addToConvoy( pLeadConvoy );
    }
    else if( pLeadConvoy == NULL )
    {
        //Add the new leader into the existing convoy for this machine
        leadSequencer.addToConvoy( pConvoy_ );
    }
    else
    {
        //We need to merge the two convoys
        mergeConvoys( pLeadConvoy );
    }

    //Set up the follow relationship
    pFollowSequencer_ = &leadSequencer;
    ++( leadSequencer.nFollowers() );

    followOffset_ = offset;

    //Clear these flags
    pImpl_->hasQueuedDestination_ = false;
    pImpl_->stopAsSoonAsPossible_ = false;

    //If currently in resting state, compute the new destination, and enter appropriate
    //state.
    if( internalState_ == INTERNAL_RESTING )
        updateFollowDestination();
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachineMotionSequencer::stopFollowing()
{
    LOG_ENTER( "stopFollowing" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    //Check has a follow relation
    if( pFollowSequencer_ != NULL )
    {
        //If the leader has no other followers and is not itself following it should
        //be removed from its convoy. Decrement follower count in leader.
        bool leaderRemovedFromConvoy =
            --( pFollowSequencer_->nFollowers() ) == 0  and
            pFollowSequencer_->pFollowSequencer() == NULL;

        if( leaderRemovedFromConvoy )
            pFollowSequencer_->removeFromConvoy();

        //Stop following
        pFollowSequencer_ = NULL;

        //If this has no followers, remove it from its convoy
        MachLogMachineConvoy* pConvoy = pConvoy_;

        if( nFollowers_ == 0 )
            removeFromConvoy();
        else if( not leaderRemovedFromConvoy )
        {
            //This machine is still leading, and the old leader is still in a convoy,
            //so we need to update the convoy, possibly splitting into 2 or more.
            updateConvoy( pConvoy );
        }
    }
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachineMotionSequencer::stopLeading()
{
    LOG_ENTER( "stopLeading" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    //Check have some followers
    if( nFollowers_ != 0 )
    {
        //Create a list of the followers of this machine.
        //We do this before making any stopFollowing calls, in case those calls modify
        //the convoy's vector - or even separate the convoy.
        const MachLogMachineConvoy::Sequencers& convoySequencers = pConvoy_->sequencers();
        size_t nInConvoy = convoySequencers.size();

        MachLogMachineConvoy::Sequencers followingSequencers;
        followingSequencers.reserve( nInConvoy );

        for( size_t i = 0; i != nInConvoy; ++i )
        {
            MachLogMachineMotionSequencer* pConvoySequencer = convoySequencers[ i ];
            if( pConvoySequencer->pFollowSequencer() == this )
                followingSequencers.push_back( pConvoySequencer );
        }

        //Now simply make each follower stop following
        size_t n = followingSequencers.size();
        for( size_t i = 0; i != n; ++i )
            followingSequencers[i]->stopFollowing();

        ASSERT( nFollowers_ == 0, "" );
    }
}
//////////////////////////////////////////////////////////////////////////////////////////

MexPoint2d MachLogMachineMotionSequencer::currentTargetPoint() const
{
    LOG_ENTER( "currentTargetPoint" );

	CB_DEPIMPL( InternalState, internalState_ );
	CB_DEPIMPL( ctl_nb_vector< MexPoint2d >, path_ );
	CB_DEPIMPL( size_t, nPathPointsDone_ );

    //Use the current location if resting or no existing path computed.
    //Otherwise the next path point
    bool useCurrentPosition = internalState_ == INTERNAL_RESTING  or  nPathPointsDone_ == path_.size();
    return (useCurrentPosition ? currentLocation() : path_[ nPathPointsDone_ ]);
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachineMotionSequencer::updateFollowDestination()
{
    LOG_ENTER( "updateFollowDestination" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    PRE( pFollowSequencer_ != NULL );

    //Ensure we don't try and stop
    pImpl_->stopAsSoonAsPossible_ = false;

    //Get the target's destination, and current position
    MexPoint2d leadDestination( pFollowSequencer_->currentTargetPoint() );
    MexPoint2d leadCurrentLocation( pFollowSequencer_->currentLocation() );

    //Compute a direction vector representing the direction the leader will be facing on
    //arrival at its target point. If current location and target are coincident,
    //use current direction
    MexVec2 leadDirection;
    if( leadDestination.sqrEuclidianDistance( leadCurrentLocation ) < 0.0001 )
    {
        leadDirection = pFollowSequencer_->pPhysMachine()->globalTransform().xBasis();
    }
    else
    {
        leadDirection = MexVec2( leadCurrentLocation, leadDestination );
        leadDirection.makeUnitVector();
    }

    MexPoint2d tryPoint;

    LOG_INSPECT( followOffset_ );

    if( MexVec2( followOffset_ ).isZeroVector() )
    {
        //  Generate a point that is on the line between this machine and the leader

        const MATHEX_SCALAR totalClearance = highClearance() + pFollowSequencer_->highClearance();
        const MATHEX_SCALAR distance = leadCurrentLocation.euclidianDistance( currentLocation() );

        LOG_INSPECT( totalClearance );
        LOG_INSPECT( distance );

        tryPoint = MexPoint2d( leadCurrentLocation, currentLocation(), 2.0 * totalClearance / distance );
    }
    else
    {
        //Now try a destination at the offset from the target, relative to the local axis
        //system of the target
        tryPoint = MexPoint2d( leadDestination.x() + followOffset_.x() * leadDirection.x()
                                                 - followOffset_.y() * leadDirection.y(),
                             leadDestination.y() + followOffset_.x() * leadDirection.y()
                                                 + followOffset_.y() * leadDirection.x() );
    }

    LOG_INSPECT( tryPoint );

    //We must check that the destination is not inside an obstacle.
    MexPoint2d newDestination;
    bool haveNewDestination = false;
    if( targetPositionContainedInSpace( tryPoint ) )
    {
        newDestination = tryPoint;
        haveNewDestination = true;
    }
    else
    {
        //Try the leader's current position
        if( targetPositionContainedInSpace( leadDestination ) )
        {
            newDestination = leadDestination;
            haveNewDestination = true;
        }
    }

    //If the destination is different, clear any existing paths, and enter
    //state to plan a new path
    if( haveNewDestination  and
        (internalState_ == INTERNAL_RESTING  or
         newDestination.sqrEuclidianDistance( destinationPoint_ ) > 1.0 ) )
    {
        clearOldPaths();
        CHANGE_STATE( INTERNAL_WANT_DOMAIN_PATH, "follow destination has been updated" );
        destinationPoint_ = newDestination;
		HAL_STREAM("(" << pLogMobile_->id() << ") (following)has just changed destinationPoint_ to " << destinationPoint_ << std::endl );
		PhysConfigSpace2d::DomainId domainId;
		bool d = pConfigSpace_->domain( destinationPoint_, &domainId );
		HAL_STREAM(" in domain " << d << " " << domainId << std::endl );
    }
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysRelativeTime MachLogMachineMotionSequencer::moveOutOfWay
(
    const MexPoint2d& askerStartPoint, const MexPoint2d& askerEndPoint,
    MATHEX_SCALAR askerClearance
)
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_ENTER( "moveOutOfWay" );

    bool canMove = false;
    PhysRelativeTime interval = 0.0;
    MexPoint2d targetPoint;

    LOG_STREAM( SimManager::instance().currentTime() <<
      " asked to move out of the way askerStartPoint " << askerStartPoint <<
      " askerEndPoint " << askerEndPoint << " askerClearance " << askerClearance << std::endl );

    LOG_STREAM(" Asked to move out of way " << (void*)this << std::endl );

    if( allowMoveOutOfWay() )
    {
        //We must not be moving or planning a path
        if( internalState_ != INTERNAL_MOVING  and
            internalState_ != INTERNAL_PLANNING  and
            internalState_ != INTERNAL_PLANNING_DOMAIN_PATH  and
            internalState_ != INTERNAL_1ST_PERSON )
        {
            //Compute a unit vector in the direction the asker is trying to go
            MexVec2 askerDir( askerStartPoint, askerEndPoint );

            LOG_INSPECT( askerDir );

            if( fabs( askerDir.modulus() ) < MexEpsilon::instance() )
                askerDir = MexVec2( askerStartPoint, currentLocation() );

            askerDir.makeUnitVector();

            LOG_INSPECT( askerDir );

            //Hence get normal vector to this direction
            MexVec2 askerNormal( askerDir.normal() );

            LOG_INSPECT( askerNormal );

            //Set a scalar depending on which side of the move line we're on
            MexPoint2d pos = currentLocation();
            MexVec2 offsetFromAsker( askerStartPoint, pos );
            MATHEX_SCALAR side = (offsetFromAsker.dotProduct( askerNormal ) >= 0.0 ? 1.0 : -1.0);

            LOG_INSPECT( pos );
            LOG_INSPECT( side );

            //Set the distance to be moved to be twice the larger of the 2 clearances,
            //and scale our 2 vectors by this amount
            MATHEX_SCALAR distance = 2.0 * std::max( useClearance_, askerClearance );
            askerDir.setVector( askerDir.x() * distance, askerDir.y() * distance );
            askerNormal.setVector( askerNormal.x() * distance * side,
                                   askerNormal.y() * distance * side );

            LOG_INSPECT( distance );

            //Now try various moves for containment in the config space
            PolygonId nastyPolygonId;

            //Disable the resting polygon so we don't get conflicts
            if( pImpl_->restingObstacleExists_ )
                pConfigSpace_->isPolygonEnabled( restingObstacleId_, false );

            LOG_INSPECT( pos );
            LOG_INSPECT( askerNormal );
            LOG_INSPECT( askerDir );

            //try moves in various directions
            canMove =
                canMoveTo( pos,
                           targetPoint = MexPoint2d( pos.x() + askerNormal.x(),
                                                     pos.y() + askerNormal.y() ) )  or

                canMoveTo( pos,
                           targetPoint = MexPoint2d( pos.x() - askerNormal.x(),
                                                     pos.y() - askerNormal.y() ) )  or

                canMoveTo( pos,
                           targetPoint = MexPoint2d( pos.x() + askerNormal.x() + askerDir.x(),
                                                     pos.y() + askerNormal.y() + askerDir.y() ) )  or

                canMoveTo( pos,
                           targetPoint = MexPoint2d( pos.x() - askerNormal.x() + askerDir.x(),
                                                     pos.y() - askerNormal.y() + askerDir.y() ) )  or

                canMoveTo( pos,
                           targetPoint = MexPoint2d( pos.x() + askerNormal.x() - askerDir.x(),
                                                     pos.y() + askerNormal.y() - askerDir.y() ) )  or

                canMoveTo( pos,
                           targetPoint = MexPoint2d( pos.x() - askerNormal.x() - askerDir.x(),
                                                     pos.y() - askerNormal.y() - askerDir.y() ) )  or

                canMoveTo( pos,
                           targetPoint = MexPoint2d( pos.x() + askerDir.x(),
                                                     pos.y() + askerDir.y() ) );

            //Reenable the resting polygon
            if( pImpl_->restingObstacleExists_ )
                pConfigSpace_->isPolygonEnabled( restingObstacleId_, true );
        }

        //See if we succeeded
        if( canMove )
        {
            LOG_STREAM("  Complied with move to " << targetPoint << std::endl );
            //Set up a new path
            clearPath();

            ASSERT( path_.size() == 0, "" );
            //  path_ must always start with the current position
            path_.push_back( currentLocation() );
            path_.push_back( targetPoint );
            nPathPointsDone_ = 1;

            // If we're currently resting then treat this as a simple move to a
            // new destination. If we're not resting then we're being moved off
            // our current path and will want to return to our current path later.

            if( internalState_ == INTERNAL_RESTING )
                destinationPoint_ = targetPoint;
            else
                pImpl_->movedOffPath_ = true;

            LOG_WHERE;
            //Set up the move to the new point
            interval = initiateMove();
            LOG_WHERE;

        }
    }

    return interval;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachineMotionSequencer::isFollowing
(
    MachLogMachineMotionSequencer* pRhsSequencer
) const
{
    LOG_ENTER( "isFollowing" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    PRE( pRhsSequencer != NULL );

    return pFollowSequencer_ == pRhsSequencer;
}
//////////////////////////////////////////////////////////////////////////////////////////

PhysRelativeTime MachLogMachineMotionSequencer::updateWhilePlanning
(
    const PhysRelativeTime& maxCPUTime
)
{
    LOG_ENTER( "updateWhilePlanning" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    PhysRelativeTime interval = 0;

    //Update the findpath algorithm
    bool finished = pConfigSpace_->updateFindPath( findPathId_, maxCPUTime );

    if( finished )
    {
        //Finished! Get the path.
        if( pConfigSpace_->findPath( findPathId_, &path_ ) )
        {
            LOG_STREAM("Found path:" << std::endl );

            #ifndef PRODUCTION
                writePathPoints();
            #endif
            CHANGE_STATE(INTERNAL_WAITING, "Found path."); // Added bugfix
            ASSERT( path_.size() > 1, "Bad machine path" );

            //The path includes the start and end points. Hence we regard
            //the start point as done.
            nPathPointsDone_ = 1;
            advancePathProgress();
            interval = updateWhileStopped();
        }
        else
        {
            //No path found.
            LOG_STREAM( "Failed to find path" << std::endl );

            //Try moving randomly. Maybe then we can find a path.
            MexPoint2d targetPoint = destinationPoint_;
            if( findAvoidancePoint( currentLocation(), &targetPoint ) )
            {
                //We have a point to move to. Make this the current path.
                clearPath();
                ASSERT( path_.size() == 0, "" );
                //  path_ must always start with the current position
                path_.push_back( currentLocation() );
                path_.push_back( targetPoint );
                nPathPointsDone_ = 1;
                pImpl_->hasOnPortalPoint_ = false;

                LOG_WHERE;
                //Set up the move to the new point
                interval = initiateMove();
                LOG_WHERE;

                //Remember we've gone off a previously computed path, so we'll recompute
                //the path to destination.
                pImpl_->movedOffPath_ = true;
            }
            else
            {
                //If we are following something, use updateWhileStopped to update our
                //destination
                CHANGE_STATE( INTERNAL_WANT_PATH, "planning finished, path not found but we're following something" );
                interval = updateWhileStopped();
            }
        }
    }

    //Carry on as soon as possible
    return interval;
}
//////////////////////////////////////////////////////////////////////////////////////////

MATHEX_SCALAR MachLogMachineMotionSequencer::capSpeed() const
{
    LOG_ENTER( "capSpeed" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    MATHEX_SCALAR speedLimit = physMachine().maxTranslationSpeed();

    //Check for in a convoy
    if( pConvoy_ != NULL )
    {
        //Get the convoy top speed
        MATHEX_SCALAR convoySpeed = pConvoy_->minTopSpeed();

        //If a leader only, move at convoy speed
        if( pFollowSequencer_ == NULL )
            speedLimit = convoySpeed;
        else
        {
            //Get the current location and destination of the leader
            MexPoint2d leaderCurrentPosition = pFollowSequencer_->currentLocation();
            //MexPoint2d leaderTargetPosition = pFollowSequencer_->currentTargetPoint();
            const MexPoint2d& targetPosition = path_.back();

            //Hence estimate the time it will take the leader to arrive
            PhysRelativeTime leaderTravelTime
                = leaderCurrentPosition.euclidianDistance( targetPosition ) / convoySpeed;

            if( leaderTravelTime > 0.5 )
            {
                //Get this machine's current position and hence distance to travel
                MATHEX_SCALAR distance = targetPosition.euclidianDistance( currentLocation() );

                //Hence compute the speed needed to arrive at approximately the same time
                speedLimit = (distance + followOffset_.x() + followOffset_.y()) / leaderTravelTime;

                //Don't go slower than convoy speed
                if( speedLimit < convoySpeed )
                    speedLimit = convoySpeed;
            }
        }
    }

    LOG_INSPECT( speedLimit );

    return speedLimit;
}

//////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachineMotionSequencer::targetPositionContainedInSpace( const MexPoint2d& targetPoint ) const
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_ENTER( "targetPositionContainedInSpace" );

    //Construct a circle around the test point using the clearance
    // TBD: The circle used here should match the circle used in
    // PhysConfigSpace2d::findSpace. They should really use the same
    // bit of code to calculate the circle. Bob.
    MexCircle2d testCircle( targetPoint, useClearance_ + 0.01 );

    LOG_INSPECT( targetPoint );

    //Disable any selected polygons in the config space
	ignorePolygons( true );

    //Check for containment
    PhysConfigSpace2d::PolygonId inTheWay;
    PhysConfigSpace2d::DomainId domainId;
    bool result = pConfigSpace_->contains( testCircle, obstacleFlags(), &inTheWay ) and pConfigSpace_->domain( targetPoint, &domainId );

    //Re-enable the polygons
	ignorePolygons( false );

    LOG_INSPECT( result );

    return result;
}

//////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachineMotionSequencer::positionContainedInSpace( const MexPoint2d& targetPoint, const PhysConfigSpace2d& configSpace ) const
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_ENTER( "positionContainedInSpace" );

    //Construct a circle around the test point using the clearance
    // TBD: The circle used here should match the circle used in
    // PhysConfigSpace2d::findSpace. They should really use the same
    // bit of code to calculate the circle. Bob.

    MATHEX_SCALAR clearance;

    if( &configSpace == &(MachLogPlanet::instance().configSpace()) )
        clearance = highClearance();
    else
        clearance = lowClearance();

    MexCircle2d testCircle( targetPoint, clearance + 0.01 );

    LOG_INSPECT( targetPoint );

    //Check for containment
    PhysConfigSpace2d::PolygonId inTheWay;
    PhysConfigSpace2d::DomainId domainId;
    bool result = configSpace.contains( testCircle, obstacleFlags(), &inTheWay ) and configSpace.domain( targetPoint, &domainId );

    LOG_INSPECT( result );

    return result;
}

//////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachineMotionSequencer::switchConfigSpace( PhysConfigSpace2d* pNewConfigSpace )
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();
	NETWORK_STREAM("MachLogMachineMotionSequencer::switchConfigSpace\n" );

    LOG_ENTER( "switchConfigSpace" );

    LOG_STREAM( "Old config space " << (void*)pConfigSpace_ << std::endl );
    LOG_STREAM( "Old clearance " << useClearance_ << std::endl );
	NETWORK_STREAM(" old cspace " << (void*)pConfigSpace_ << " new config space " << (void*)pNewConfigSpace << " pSpaceDomain " << (void*)pSpaceDomain_ << "\n" );

    PRE_INFO( pLogMobile_->id() );

    //  This is commented out because it causes problems with networking.
//    PRE( not hasDestination() );

    //Clear any list of polygons to ignore
    ignoreObstacleIds_.erase( ignoreObstacleIds_.begin(), ignoreObstacleIds_.end() );

    //Clear the resting obstacle/chunk from the old space
    clearRestingObstacle();

    //Store the new space pointer
    pConfigSpace_ = pNewConfigSpace;

    //Decide if the planet space
    pImpl_->inPlanetSpace_ = pConfigSpace_ == &(MachLogPlanet::instance().configSpace());

    //Attach to any space domain specified if not on the planet space
    if( not pImpl_->inPlanetSpace_  and  pSpaceDomain_ != NULL )
    {
		NETWORK_STREAM(" not in planet space and space domain != NULL so clear intersecting relationships and attacTo\n" );
        //Clear all existing intersect relations
        clearIntersectingDomains();

        pPhysMachine_->attachTo( pSpaceDomain_ );
    }

    //If re-entering the planet space, we no longer need the space domain
    if( pImpl_->inPlanetSpace_ )
        pSpaceDomain_ = NULL;

    //Use low clearance if not the planet space
    useClearance_ = ( pImpl_->inPlanetSpace_ ? highClearance_ : lowClearance_ );

    //Drop a resting obstacle
    addRestingObstacle();

    LOG_STREAM( "New config space " << (void*)pConfigSpace_ << std::endl );
    LOG_STREAM( "New clearance " << useClearance_ << std::endl );
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachineMotionSequencer::removeMotionChunks()
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_ENTER( "removeMotionChunks" );

    removeMotionChunksWithoutEcho();

    MachLogNetwork& network = MachLogNetwork::instance();
	if( network.isNetworkGame() )
	{
		PhysAbsoluteTime now = SimManager::instance().currentTime();
		if( now != lastClearMovingChunksTimeMessage_ )
		{
	    	network.messageBroker().sendClearMovingChunksMessage( pLogMobile_->id() );
			lastClearMovingChunksTimeMessage_ = now;
		}
	}
}

void MachLogMachineMotionSequencer::clearRestingObstacle()
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_ENTER( "clearRestingObstacle" );

	doClearRestingObstacle();
	if( MachLogNetwork::instance().isNetworkGame() )
	{
		MachLogNetwork::instance().messageBroker().sendClearRestingObstacleMessage( pLogMobile_->id() );
	}
}

void MachLogMachineMotionSequencer::clearRestingObstacleWithoutEcho()
{
    LOG_ENTER( "clearRestingObstacleWithoutEcho" );

	doClearRestingObstacle();
}

void MachLogMachineMotionSequencer::doClearRestingObstacle()
{
    LOG_ENTER( "doClearRestingObstacle" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    //Clear any polygon
    if( pImpl_->restingObstacleExists_ )
    {
        pConfigSpace_->remove( restingObstacleId_ );
        pImpl_->restingObstacleExists_ = false;
    }

    //Clear any chunk
    if( pImpl_->restingChunkExists_ )
    {
        pConfigSpace_->removeMotionChunk( restingChunkId_ );
        pImpl_->restingChunkExists_ = false;
    }
}
//////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachineMotionSequencer::canMoveTo
(
    const MexPoint2d fromPoint, const MexPoint2d toPoint
)
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_ENTER( "canMoveTo" );

	PhysConfigSpace2d::DomainId domainId;
	bool rc = pConfigSpace_->domain( toPoint, &domainId );
	if( not rc )
	{
		HAL_STREAM("canMoveTo toPoint " << toPoint << " does not lie in a domain\n" );
		return rc;
	}
    //Construct a motion chunk between the 2 points. This does not represent the actual
    //motion, but will serve to check for intersection with nearby stationary actors.
    PhysAbsoluteTime timeNow = SimManager::instance().currentTime();
    MATHEX_SCALAR distance = fromPoint.euclidianDistance( toPoint );
    MATHEX_SCALAR speed = 5.0;
    PhysMotionChunk motionChunk
    (
        fromPoint,
        toPoint,
        useClearance_,
        PhysRampAcceleration( speed, 1.0, 1.0, distance / speed, 0.0, 0.0 ),
        PhysAbsoluteTime( timeNow ),
        PhysRelativeTime( 0.0 ),
        minHeight(),
        maxHeight()
    );

    //Try to add it to the config space. Success implies the move does not collide
    //with nearby stationary actors at least.

    PhysConfigSpace2d::ObjectIds ignoreIds;
    ignoreIds.push_back( pLogMobile_->id() );

    PhysRelativeTime collideTime;
    ObjectId collisionObjectId;
    PhysConfigSpace2d::MotionChunkId tempId;
    bool result = pConfigSpace_->add( motionChunk, pLogMobile_->id(),
      ignoreIds, &tempId,
      &collisionObjectId, &collideTime );


    if( result )
    {
        //Remove the test chunk from the space
        pConfigSpace_->removeMotionChunk( tempId );

        //Now we have to check this move doesn't collide with permanent obstacles.
        //Construct a test polygon using the current clearance around the line joining
        //the two points
        MexConvexPolygon2d testPolygon( fromPoint, toPoint, useClearance_ );

        //Can move if the polygon is contained in the config space
        PhysConfigSpace2d::PolygonId nastyPolygonId;
        result = pConfigSpace_->contains( testPolygon, obstacleFlags(), &nastyPolygonId,
                                          PhysConfigSpace2d::USE_PERMANENT_ONLY );

        if( not result )
        {
            LOG_STREAM( "    Location " << toPoint << " clashes with polygon id " << nastyPolygonId.asScalar() << std::endl );
            LOG_STREAM( pConfigSpace_->polygon( nastyPolygonId ) );
        }
        else
        {
            //Secondly we must check that the end position is safe with a 1.5 safety factor
            //on the clearance
            MexConvexPolygon2d testEndPolygon( MexAlignedBox2d( toPoint, useClearance_ * 1.5 ) );
            result = pConfigSpace_->contains( testEndPolygon, obstacleFlags(), &nastyPolygonId,
                                              PhysConfigSpace2d::USE_PERMANENT_ONLY );
            if( not result )
            {
                LOG_STREAM( "    Location " << toPoint << " clashes with polygon id " << nastyPolygonId.asScalar() << std::endl );
                LOG_STREAM( pConfigSpace_->polygon( nastyPolygonId ) );
            }
			else
			{
				//Finally check that the toPoint is contained in a valid domain
				PhysConfigSpace2d::DomainId domainId;
				result = pConfigSpace_->domain( toPoint, &domainId );
				if( not result )
				{
					HAL_STREAM(" Location " << toPoint << " is not in any domain\n " << std::endl );
				}
			}
        }
    }

    LOG_INSPECT( fromPoint );
    LOG_INSPECT( toPoint );
    LOG_INSPECT( result );

    return result;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachineMotionSequencer::findAvoidancePoint
(
    const MexPoint2d& fromPoint, MexPoint2d* pToPoint
)
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_ENTER( "findAvoidancePoint" );

    //Construct a vector for the direction we're trying to move in, and make its
    //length twice the current clearance
    MexVec2 moveDirection( fromPoint, *pToPoint );

    do
    {
        // A small random element should stop us
        // getting stuck in the same pattern forever.
        MexBasicRandom r = MexBasicRandom::constructSeededFromTime();

        moveDirection += MexVec2(
          mexRandomScalar( &r, -1.0, 1.0 ), mexRandomScalar( &r, -1.0, 1.0 ) );
    } while( moveDirection.isZeroVector() );

    moveDirection.makeUnitVector();
    moveDirection.setVector( moveDirection.x() * useClearance_ * 2.0,
                             moveDirection.y() * useClearance_ * 2.0 );

    //Construct a normal
    MexVec2 norm( moveDirection.normal() );

    //Disable the resting polygon if present
    if( pImpl_->restingObstacleExists_ )
        pConfigSpace_->isPolygonEnabled( restingObstacleId_, false );

    //Try various directions
    const MexPoint2d pos = fromPoint;
    MexPoint2d targetPoint;
    bool canAvoid =
        canMoveTo( pos,
                   targetPoint = MexPoint2d( pos.x() + moveDirection.x() + norm.x(),
                                             pos.y() + moveDirection.y() + norm.y() ) )  or

        canMoveTo( pos,
                   targetPoint = MexPoint2d( pos.x() + moveDirection.x() - norm.x(),
                                             pos.y() + moveDirection.y() - norm.y() ) )  or

        canMoveTo( pos,
                   targetPoint = MexPoint2d( pos.x() + norm.x(),
                                             pos.y() + norm.y() ) )  or

        canMoveTo( pos,
                   targetPoint = MexPoint2d( pos.x() - norm.x(),
                                             pos.y() - norm.y() ) );

    //Return the point if found
    if( canAvoid )
        *pToPoint = targetPoint;

    //Enable the resting polygon if present
    if( pImpl_->restingObstacleExists_ )
        pConfigSpace_->isPolygonEnabled( restingObstacleId_, true );

    LOG_INSPECT( canAvoid );

    return canAvoid;
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachineMotionSequencer::advancePathProgress()
{
    LOG_ENTER( "advancePathProgress" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_STREAM( "Path:" << std::endl << path_ );
    LOG_INSPECT( currentLocation() );

    //Increment the count of path points reached as far as we can
    MexPoint2d nowPoint = currentLocation();
    for( size_t i = nPathPointsDone_; i < path_.size(); ++i )
    {
        LOG_STREAM( i << " " << path_[ i ] << "  " );
        LOG_STREAM( "distance " << nowPoint.sqrEuclidianDistance( path_[ i ] ) << std::endl );

        if( nowPoint.sqrEuclidianDistance( path_[ i ] ) <= 0.0002 )
            nPathPointsDone_ = i + 1;
    }
    LOG_INSPECT( nPathPointsDone_ );

    //Check for path complete
    if( nPathPointsDone_ == path_.size() )
    {
        //Clear this path
        clearPath();

        //Check for having arrived at portal points, if any
        while( nPortalPointsDone_ < domainPath_.size() )
        {
            //Check for being within 1 metre of the portal line
            const PortalPoint& portalPoint = domainPath_[ nPortalPointsDone_ ];
            PhysConfigSpace2d::PortalId portalId = portalPoint.first;

            //Get the portal length and endpoints
            MATHEX_SCALAR portalLength = pConfigSpace_->portalLength( portalId );
            MexPoint2d portalStart( pConfigSpace_->portalPoint( portalId, 0.0 ) );
            MexPoint2d portalEnd( pConfigSpace_->portalPoint( portalId, portalLength ) );

            //Get square of distance from the portal
            MATHEX_SCALAR sqrDistanceFromPortal =
                MexLine2d::sqrEuclidianDistance( portalStart, portalEnd, portalLength, nowPoint );

            //If within 0.25 metres, treat as arrived
            if( sqrDistanceFromPortal < 0.0625 )
            {
                ++nPortalPointsDone_;
                pImpl_->hasOnPortalPoint_ = false;
            }
            else
                break;
        }
    }
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachineMotionSequencer::useSpaceDomain( W4dDomain* pDomain )
{
    LOG_ENTER( "useSpaceDomain" );

	CB_DEPIMPL( W4dDomain*, pSpaceDomain_ );

    pSpaceDomain_ = pDomain;
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachineMotionSequencer::clearIntersectingDomains()
{
    LOG_ENTER( "clearIntersectingDomains" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    const W4dEntity::W4dDomains& oldDomains = pPhysMachine_->intersectingDomains();
    while( oldDomains.size() != 0 )
    {
        PRE_DATA( size_t oldSize = oldDomains.size() );
        pPhysMachine_->intersects( oldDomains.back(), false );
        POST( oldSize == 1 + oldDomains.size() );
    }
}
//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachineMotionSequencer::stopDead()
{
    LOG_ENTER( "stopDead" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

	doStopDead();
	if( MachLogNetwork::instance().isNetworkGame() )
	{
		MachLogNetwork::instance().messageBroker().sendStopDeadMessage( pLogMobile_->id(), pLogMobile_->globalTransform() );
	}
}

void MachLogMachineMotionSequencer::stopDeadWithoutEcho()
{
    LOG_ENTER( "stopDeadWithoutEcho" );

	doStopDead();
}

void MachLogMachineMotionSequencer::doStopDead()
{
    LOG_ENTER( "doStopDead" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

//This assertion isn't valid in the current network implementation as an this node
//I may well have stopped moving by the time this arrives from a remote node.
//	PRE_INFO( internalState_ );
//    PRE( internalState_ == INTERNAL_MOVING or internalState_ == INTERNAL_WAITING_FOR_NETWORK_CONFIRM );

    //Cancel any current motion animation

    pPhysMachine_->stopDead();

    //  Get the motion chunks correct for our new, stopped state
    removeMotionChunks();
    addRestingObstacle();

    //Enter waiting state (as long as we aren't in 1st person control - otherwise everything will shag
	//as the motion sequencer will not match the 1st person controller state).
	if( not is1stPersonControlled() )
	{
	    CHANGE_STATE( INTERNAL_WAITING, "stop dead called" );
	}
    pImpl_->stopAsSoonAsPossible_ = false;

    //Inform any machines following this one
    notifyFollowers();

    stopMachinesOnCollisionCourse();
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachineMotionSequencer::stopMachinesOnCollisionCourse()
{
    LOG_ENTER( "stopMachinesOnCollisionCourse" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    //  Stop any machines that are about to run into this one

    //Construct 2 points at and near the current location
    MexPoint2d location2d( currentLocation() );
    MexPoint2d nearPoint( location2d.x() + 0.01, location2d.y() );

    //Now construct a motion chunk as if we had moved from nearPoint to location2d
    PhysAbsoluteTime timeNow = SimManager::instance().currentTime();
    PhysMotionChunk motionChunk
    (
        nearPoint,
        location2d,
        useClearance_,
        PhysRampAcceleration( 0.0, 400.0, -400.0, 0.01, 0.005, 0.005 ),
        PhysAbsoluteTime( timeNow - 0.01 ),
        PhysRelativeTime( 0.0 ),
        minHeight(),
        maxHeight()
    );

    //Now see if there are any intersections with any motion chunks and this
    //resting chunk, other than with the machine's current moving chunk
    PhysConfigSpace2d::Intersections intersections;
    intersections.reserve( 16 );

    pConfigSpace_->findIntersections( motionChunk, pLogMobile_->id(), &intersections );

    for( PhysConfigSpace2d::Intersections::iterator i = intersections.begin(); i != intersections.end(); ++i )
    {
		if( MachLogRaces::instance().actorExists( (*i).collisionObjectId_.asScalar() ) )
		{
			MachActor& actor = MachLogRaces::instance().actor( (*i).collisionObjectId_.asScalar() );

            if( actor.objectIsMachine() )
            {
                MachLogMachine& machine = actor.asMachine();
                machine.motionSeq().stopKeepDestination();
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachineMotionSequencer::leaderChangedDestination()
{
    LOG_ENTER( "leaderChangedDestination" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    PRE( isFollowing() );

    //If we are moving and won't arrive for more than 2 seconds, interrupt motion
    if( internalState_ == INTERNAL_MOVING  and  (arrivalTime_ + 2.0) > SimManager::instance().currentTime() )
    {
        pImpl_->stopAsSoonAsPossible_ = true;
        scheduledUpdateTime_ = 0.0;
    }
}
//////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachineMotionSequencer::notifyFollowers()
{
    LOG_ENTER( "notifyFollowers" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    //Notify all other machines in this convoy that are following this one of a change
    //of destination.
    if( nFollowers_ != 0 )
    {
        const MachLogMachineConvoy::Sequencers& convoySequencers = pConvoy_->sequencers();
        for( MachLogMachineConvoy::Sequencers::const_iterator it = convoySequencers.begin();
             it != convoySequencers.end(); ++it )
        {
            MachLogMachineMotionSequencer* pConvoySequencer = *it;
            if( pConvoySequencer != this  and  pConvoySequencer->pFollowSequencer() == this )
                pConvoySequencer->logMobile().checkLeaderDestination();
        }
    }
}

PhysConfigSpace2d& MachLogMachineMotionSequencer::currentConfigSpace()
{
	CB_DEPIMPL( PhysConfigSpace2d*, pConfigSpace_ );

	return *pConfigSpace_;
}

bool MachLogMachineMotionSequencer::addMotionChunks( const ctl_vector< PhysMotionChunk >& chunks )
{
    LOG_ENTER( "addMotionChunks" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    ObjectId collisionObjectId;
    PhysAbsoluteTime collideTime;

    bool    worked = true;

    for( size_t i = 0; worked  and  i < chunks.size(); ++i )
    {
        PhysConfigSpace2d::MotionChunkId    chunkId;

        PhysConfigSpace2d::ObjectIds ignoreIds;
        ignoreIds.push_back( pLogMobile_->id() );

        if( pConfigSpace_->add( chunks[ i ], pLogMobile_->id(),
            ignoreIds, &chunkId, &collisionObjectId, &collideTime ) )
        {
            motionChunkIds_.push_back( chunkId );
			NETWORK_STREAM("(" << pLogMobile_->id() << ") !! Added motion chunk with id " << chunkId << "callstack\n" << ProStackTracer() << std::endl << "!! add end\n" );
        }
        else
        {
            worked = false;

            //Remove the motion chunks already added
            removeMotionChunksWithoutEcho();
        }

    }

	return worked;
}

void MachLogMachineMotionSequencer::removeMotionChunksWithoutEcho()
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_ENTER( "removeMotionChunksWithoutEcho" );

    for( size_t i = 0; i < motionChunkIds_.size(); ++i )
    {
        PhysConfigSpace2d::MotionChunkId    chunkId = motionChunkIds_[ i ];

    	if( pConfigSpace_->motionChunkExists( chunkId ) )
	    	pConfigSpace_->removeMotionChunk( chunkId );
    }

    motionChunkIds_.erase( motionChunkIds_.begin(), motionChunkIds_.end() );
    moveInfos_.erase( moveInfos_.begin(), moveInfos_.end() );
}

MachLogMachineMotionSequencer::State   MachLogMachineMotionSequencer::state() const
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    State   result = ARRIVED;

    switch( internalState_ )
    {
        case MachLogMachineMotionSequencer::INTERNAL_RESTING:
            if( motionSucceeded_ )
                result = ARRIVED;
            else
                result = FAILED;
            break;
        case MachLogMachineMotionSequencer::INTERNAL_WANT_DOMAIN_PATH:
            result = PLANNING;
            break;
        case MachLogMachineMotionSequencer::INTERNAL_PLANNING_DOMAIN_PATH:
            result = PLANNING;
            break;
        case MachLogMachineMotionSequencer::INTERNAL_WANT_PATH:
            result = PLANNING;
            break;
        case MachLogMachineMotionSequencer::INTERNAL_PLANNING:
            result = PLANNING;
            break;
        case MachLogMachineMotionSequencer::INTERNAL_MOVING:
            result = MOVING;
            break;
        case MachLogMachineMotionSequencer::INTERNAL_WAITING:
            result = WAITING;
            break;
        case MachLogMachineMotionSequencer::INTERNAL_WAITING_FOR_NETWORK_CONFIRM:
            result = PLANNING;
            break;
        case MachLogMachineMotionSequencer::INTERNAL_1ST_PERSON:
            result = FIRST_PERSON;
            break;

        DEFAULT_ASSERT_BAD_CASE( internalState_ );
    }

    return result;
}

void MachLogMachineMotionSequencer::commandId( size_t newId )
{
    pImpl_->commandId( newId );
}

size_t MachLogMachineMotionSequencer::commandId( ) const
{
    return pImpl_->commandId();
}

bool MachLogMachineMotionSequencer::commandIdSet( ) const
{
    return pImpl_->commandIdSet();
}

//////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachineMotionSequencer::hasFollowers() const
{
	CB_DEPIMPL( int, nFollowers_ );

	return nFollowers_ > 0;
}

//////////////////////////////////////////////////////////////////////////////////////////

bool MachLogMachineMotionSequencer::followingMachines( MachLogMachine::Machines* pMachines )
{
    LOG_ENTER( "followingMachines" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

	if( not hasFollowers() )
		return false;

	bool found = false;

	MachLogMachine::Machines& followingMachines = *pMachines;


	const MachLogMachineConvoy::Sequencers& convoySequencers = pConvoy_->sequencers();
	for( MachLogMachineConvoy::Sequencers::const_iterator it = convoySequencers.begin();
	     it != convoySequencers.end(); ++it )
	{
	    MachLogMachineMotionSequencer* pConvoySequencer = *it;
	    if( pConvoySequencer != this  and  pConvoySequencer->pFollowSequencer() == this )
	    {
			ASSERT( pConvoySequencer->logMobile().objectIsMachine(), "Following MLMobile was not machine." );
	    	followingMachines.push_back( &pConvoySequencer->logMobile().asMachine() );
	    }
	}

	return found;
}

//  Return the obstacle flags for the mobile object this machine corresponds to
MachLogMachineMotionSequencer::ObstacleFlags MachLogMachineMotionSequencer::obstacleFlags() const
{
    return logMobile().obstacleFlags();
}

void MachLogMachineMotionSequencer::clearPath()
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_ENTER( "clearPath" );

    removeMotionChunks();
    path_.erase( path_.begin(), path_.end() );
    nPathPointsDone_ = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////

void MachLogMachineMotionSequencer::is1stPersonControlled( bool isIt )
{
	CB_DEPIMPL( InternalState, internalState_ );

    PRE( not isIt  or  internalState_ == INTERNAL_RESTING );

    if( isIt )
    {
        CHANGE_STATE( INTERNAL_1ST_PERSON, "Entering 1st person" );
    }
    else
    {
        CHANGE_STATE( INTERNAL_RESTING, "Leaving 1st person" );
    }
}

bool MachLogMachineMotionSequencer::is1stPersonControlled() const
{
	CB_DEPIMPL( InternalState, internalState_ );

    return internalState_ == INTERNAL_1ST_PERSON;
}

void MachLogMachineMotionSequencer::addRestingObstacleWithoutEcho()
{
	CB_DEPIMPL( MachPhysMachine*, pPhysMachine_ );

    addRestingObstacleWithoutEcho( pPhysMachine_->globalTransform() );
}

bool MachLogMachineMotionSequencer::preventImpendingCollisions
(
    const MexTransform3d& nextPosition, const PhysAbsoluteTime& startTime
)
{
    LOG_ENTER( "preventImpendingCollisions" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    PRE( is1stPersonControlled() );

    //Construct a motion chunk from current position to next position
    MexPoint2d startPoint( pPhysMachine_->globalTransform().position() );
    MexPoint2d endPoint( nextPosition.position() );
    PhysAbsoluteTime now = SimManager::instance().currentTime();
    PhysRelativeTime moveDuration = now - startTime;
    ASSERT( moveDuration >= 0.0, "Invalid time" );
    MATHEX_SCALAR speed = ( moveDuration == 0.0 ? 0.0 : startPoint.euclidianDistance( endPoint ) / moveDuration );
    MATHEX_SCALAR dummyAcceleration = 400.0;
    PhysRampAcceleration motionProfile( speed, dummyAcceleration, dummyAcceleration, moveDuration, 0.0, 0.0 );

    PhysMotionChunk testChunk( startPoint, endPoint, useClearance_, motionProfile, startTime, 0.0, minHeight(), maxHeight() );

    //Identify all the potential collisions in the next half second
    PhysAbsoluteTime limitTime = SimManager::instance().currentTime() + 0.5;
    PhysConfigSpace2d::ChunkIntersections intersections;
    intersections.reserve( 32 );
    pConfigSpace_->findIntersections( testChunk, pLogMobile_->id(), &intersections );

    bool stoppedAnotherMachine = false;

    for( PhysConfigSpace2d::ChunkIntersections::iterator it = intersections.begin(); it != intersections.end(); ++it )
    {
        PhysConfigSpace2d::ChunkIntersectionData& data = (*it);
        if( data.collisionTime_ < limitTime )
        {
            //get the colliding machine and stop it
            MachLogMachine& otherMachine = MachLogRaces::instance().actor( data.collisionObjectId_.asScalar() ).asMachine();
            otherMachine.motionSeq().stopKeepDestination();
            stoppedAnotherMachine = true;
        }
    }

    return stoppedAnotherMachine;
}

const PhysConfigSpace2d& MachLogMachineMotionSequencer::currentConfigSpace() const
{
	CB_DEPIMPL( PhysConfigSpace2d*, pConfigSpace_ );

	return *pConfigSpace_;
}

bool MachLogMachineMotionSequencer::keepEntranceLocked() const
{
    return pImpl_->keepEntranceLocked();
}

void MachLogMachineMotionSequencer::keepEntranceLocked( bool keep )
{
    pImpl_->keepEntranceLocked( keep );
}

void MachLogMachineMotionSequencer::autoUnlockAnyLockedEntrance()
{
    LOG_ENTER( "autoUnlockAnyLockedEntrance" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    //check have permission
    if( not keepEntranceLocked() )
    {
        //Get the machine and see if it has a locked entrance
        MachLogMachine* pMachine = &pLogMobile_->asMachine();
        if( pMachine->hasLockedEntrance()  and  pMachine->inBuildingOrOnPad() )
        {
            //Need to see if still on the internal or external entrance polygon.
            //Make a clearance circle for the machine
            MexCircle2d testCircle( MexPoint2d( pMachine->position() ), useClearance_ + 0.3 );

            //Check against the external polygon
            MachLogConstruction& construction = pMachine->inOrOnPadWhichBuilding();
            PhysConfigSpace2d::PolygonId externalId = construction.entrance( 0 ).exteriorPolygonId();
            PhysConfigSpace2d& planetConfigSpace = MachLogPlanet::instance().configSpace();

            const MexPolygon2d& externalEntrancePolygon = planetConfigSpace.polygon( externalId );
            bool onPolygons = testCircle.intersects( externalEntrancePolygon );

            if( not onPolygons )
            {
                //Not on the external polygon, so test the internal one
                PhysConfigSpace2d::PolygonId internalId = construction.entrance( 0 ).interiorPolygonId();
                PhysConfigSpace2d& interiorConfigSpace = construction.interiorConfigSpace();
                const MexPolygon2d& internalEntrancePolygon = interiorConfigSpace.polygon( internalId );
                onPolygons = testCircle.intersects( internalEntrancePolygon );
            }

            //Check for circle not intersecting the polygon
            if( not onPolygons )
            {
                //Close the door
                if( construction.isEntranceOpen( 0 ) )
        	    	construction.isEntranceOpen( 0, false );

                //Stop ignoring any polygons
                bool outside = pConfigSpace_ == &planetConfigSpace;
                pMachine->ignoreEntrancePolygon( false );
                if( outside )
                    pMachine->ignoreConstructionPolygon( false );

                //Unlock the entrance
                pMachine->lockedEntrance().unlock();
                pMachine->entranceLocked( NULL );

                //If outside, relinquish association with the building
                if( outside )
                    pMachine->inOrOnPadBuilding( NULL );
            }
        }
    }
}

void MachLogMachineMotionSequencer::ignorePolygons( bool setIgnore ) const
{
    LOG_ENTER( "ignorePolygons" );

	CB_DEPIMPL( const PhysConfigSpace2d::PolygonIds, ignoreObstacleIds_ );
	CB_DEPIMPL( PhysConfigSpace2d*, pConfigSpace_ );

    for( PhysConfigSpace2d::PolygonIds::const_iterator it = ignoreObstacleIds_.begin();
         it != ignoreObstacleIds_.end(); ++it )
        pConfigSpace_->isPolygonEnabled( *it, not setIgnore );
}

//////////////////////////////////////////////////////////////////////////////////////////
MATHEX_SCALAR MachLogMachineMotionSequencer::highClearance() const
{
	CB_DEPIMPL( MATHEX_SCALAR, highClearance_ );

    return highClearance_;
}
//////////////////////////////////////////////////////////////////////////////////////////
MATHEX_SCALAR MachLogMachineMotionSequencer::lowClearance() const
{
	CB_DEPIMPL( MATHEX_SCALAR, lowClearance_ );

    return lowClearance_;
}
//////////////////////////////////////////////////////////////////////////////////////////
MachLogMachineMotionSequencer::InternalState MachLogMachineMotionSequencer::internalState() const
{
	CB_DEPIMPL( InternalState, internalState_ );

    return internalState_;
}
//////////////////////////////////////////////////////////////////////////////////////////
MachLogMachineMotionSequencer::FailureReason   MachLogMachineMotionSequencer::failureReason() const
{
	CB_DEPIMPL( FailureReason, failureReason_ );

    PRE( state() == FAILED );
    return failureReason_;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool MachLogMachineMotionSequencer::isInConvoy() const
{
	CB_DEPIMPL( MachLogMachineConvoy*, pConvoy_ );

    return pConvoy_ != NULL;
}
//////////////////////////////////////////////////////////////////////////////////////////
MachLogMachineConvoy& MachLogMachineMotionSequencer::convoy()
{
	CB_DEPIMPL( MachLogMachineConvoy*, pConvoy_ );

    PRE( isInConvoy() );

    return *pConvoy_;
}
//////////////////////////////////////////////////////////////////////////////////////////
MachPhysMachine& MachLogMachineMotionSequencer::physMachine()
{
	CB_DEPIMPL( MachPhysMachine*, pPhysMachine_ );

    return *pPhysMachine_;
}
//////////////////////////////////////////////////////////////////////////////////////////
const MachPhysMachine& MachLogMachineMotionSequencer::physMachine() const
{
	CB_DEPIMPL( MachPhysMachine*, pPhysMachine_ );

    return *pPhysMachine_;
}
//////////////////////////////////////////////////////////////////////////////////////////
MachLogMobile& MachLogMachineMotionSequencer::logMobile()
{
	CB_DEPIMPL( MachLogMobile*, pLogMobile_ );

    return *pLogMobile_;
}
//////////////////////////////////////////////////////////////////////////////////////////
const MachLogMobile& MachLogMachineMotionSequencer::logMobile() const
{
	CB_DEPIMPL( MachLogMobile*, pLogMobile_ );

    return *pLogMobile_;
}
//////////////////////////////////////////////////////////////////////////////////////////
size_t MachLogMachineMotionSequencer::nFollowers() const
{
	CB_DEPIMPL( int, nFollowers_ );

    return nFollowers_;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool MachLogMachineMotionSequencer::isFollowing() const
{
	CB_DEPIMPL( MachLogMachineMotionSequencer*, pFollowSequencer_ );

    return pFollowSequencer_ != NULL;
}
//////////////////////////////////////////////////////////////////////////////////////////
bool MachLogMachineMotionSequencer::isUsingSpaceDomain() const
{
	CB_DEPIMPL( W4dDomain*, pSpaceDomain_ );

    return pSpaceDomain_ != NULL;
}
//////////////////////////////////////////////////////////////////////////////////////////
W4dDomain& MachLogMachineMotionSequencer::spaceDomain()
{
	CB_DEPIMPL( W4dDomain*, pSpaceDomain_ );

    return *pSpaceDomain_;
}

MachPhysMachine* MachLogMachineMotionSequencer::pPhysMachine() const
{
	CB_DEPIMPL( MachPhysMachine*, pPhysMachine_ );

	return pPhysMachine_;
}

MachLogMachineConvoy*& MachLogMachineMotionSequencer::pConvoy() const
{
	CB_DEPIMPL( MachLogMachineConvoy*, pConvoy_ );

	return pConvoy_;
}

int& MachLogMachineMotionSequencer::nFollowers()
{
	CB_DEPIMPL( int, nFollowers_ );

	return nFollowers_;
}

MachLogMachineMotionSequencer* MachLogMachineMotionSequencer::pFollowSequencer() const
{
	CB_DEPIMPL( MachLogMachineMotionSequencer*, pFollowSequencer_ );

	return pFollowSequencer_;
}

void MachLogMachineMotionSequencer::allowMoveOutOfWay( bool allowed )
{
	CB_DEPIMPL( bool, allowMoveOutOfWay_ );

    allowMoveOutOfWay_ = allowed;
}

bool MachLogMachineMotionSequencer::allowMoveOutOfWay() const
{
	CB_DEPIMPL( bool, allowMoveOutOfWay_ );

	return allowMoveOutOfWay_;
}

void MachLogMachineMotionSequencer::destinationTolerance( MATHEX_SCALAR tolerance )
{
	CB_DEPIMPL( MATHEX_SCALAR, destinationTolerance_ );

    destinationTolerance_ = tolerance;
}

MATHEX_SCALAR MachLogMachineMotionSequencer::sqrDestinationTolerance() const
{
	CB_DEPIMPL( MATHEX_SCALAR, destinationTolerance_ );

    return destinationTolerance_ * destinationTolerance_;
}

MATHEX_SCALAR MachLogMachineMotionSequencer::minHeight() const
{
    return pPhysMachine()->compositeBoundingVolume().minCorner().z();
}

MATHEX_SCALAR MachLogMachineMotionSequencer::maxHeight() const
{
    return pPhysMachine()->compositeBoundingVolume().maxCorner().z();
}

bool MachLogMachineMotionSequencer::straightLineMove(
    const MexPoint3d& toPoint,
    PhysRelativeTime* pInterval )
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    PRE_INFO( motionChunkIds_.size() );
    PRE_INFO( moveInfos_.size() );

    PRE( motionChunkIds_.size() == 0 );
    PRE( moveInfos_.size() == 0 );

    PRE( pFollowSequencer_ == NULL );

    LOG_ENTER( "straightLineMove" );

    //Disable the ignore polygon set
    ignorePolygons( true );

    PhysConfigSpace2d::ObjectIds    ignoreIds;
    getIntersectingIds( &ignoreIds );

    const MexTransform3d fromTransform = pPhysMachine_->globalTransform();

    MachPhys::FinalState actualFinalState;

    Point2ds path;
    path.reserve( 32 );
    path.push_back( toPoint );

    PhysAbsoluteTime startTime = SimManager::instance().currentTime();
    MATHEX_SCALAR startSpeed = 0.0;
    MoveInfoAPtr collideMoveInfoAPtr;
    MachLogCollisionInfo collisionInfo;

    MoveResult moveResult;

    size_t pointsReached = reservePath( fromTransform, MachPhysMachine::USE_ORIENTATION,
        path, startTime, startSpeed,
        MachPhys::AT_REST, CHECK_STATIC_OBSTACLES,
        ignoreIds,
        &actualFinalState,
        &collisionInfo,
        &collideMoveInfoAPtr, &moveResult );

    bool motionPossible = false;

    if( pointsReached == 1 )
    {
        LOG_STREAM( "Straight line move succeeded" << std::endl );

        *pInterval = initiatePhysicalMotion();
        motionPossible = true;
    }
    else if( moveResult == COLLIDED_WITH_MACHINE )
    {
        LOG_STREAM( "Straight line move was blocked by a machine" << std::endl );

        //  This code commented out by Bob 16 Feb 1999. The idea behind the code is
        //  valid - if we are only being blocked by a machine then skip the whole
        //  pathfinding stage. Unfortunately we run into problems if the destination
        //  is occupied by a machine, the only place the "close enough" and
        //  "find near point" and "move the blocking machine out of the way" code
        //  exists is in the pathfinding code so, as the quickest solution to the
        //  problem, this code has been commented out.
        //  TBD: Set up the "avoid a blocking machine" code in here.

//         //  We are only being blocked by machines - this means we can
//         //  skip the pathfinding steps. We pretend that we're just
//         //  finishing the internal planning state.
//
//         CHANGE_STATE( INTERNAL_WAITING, "straight line move blocked by a machine" );
//
//         path_.erase( path_.begin(), path_.end() );
//
//         ASSERT( path_.size() == 0, "" );
//
//         path_.push_back( fromTransform.position() );
//         path_.push_back( toPoint );
//
//         //The path includes the start and end points. Hence we regard
//         //the start point as done.
//         nPathPointsDone_ = 1;
//         advancePathProgress();
//         *pInterval = 1.0;
//
//         motionPossible = true;
    }

    //Re-enable the ignore polygon set
    ignorePolygons( false );

    return motionPossible;
}

PhysRelativeTime MachLogMachineMotionSequencer::initiatePhysicalMotion()
{
    LOG_ENTER( "initiatePhysicalMotion" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_STREAM( "  Success!" << std::endl );

    //Queue the actual motion
    PhysRelativeTime interval = setupMotion();

	MachLogNetwork& network = MachLogNetwork::instance();
	if( network.isNetworkGame() )
	{
        ctl_vector< PhysMotionChunk >   motionChunks;

        for( size_t i = 0; i < motionChunkIds_.size(); ++i )
        {
            const PhysConfigSpace2d::MotionChunkId& id = motionChunkIds_[ i ];
            const PhysMotionChunk& motionChunk = pConfigSpace_->motionChunk( id );

            motionChunks.push_back( motionChunk );
        }

		if( network.isNodeLogicalHost() )
			network.messageBroker().sendAddMotionChunksMessage( pLogMobile_->id(), motionChunks, INTERNAL_MOVING );
		else
		{
			network.messageBroker().sendRequestAddMotionChunksMessage( pLogMobile_->id(), motionChunks, INTERNAL_MOVING );
		}
	}

    return interval;
}

void MachLogMachineMotionSequencer::toggleLogging()
{
    pImpl_->toggleLogging();
}

ostream& MachLogMachineMotionSequencer::traceConvoyData( ostream& o ) const
{
	CB_DEPIMPL( MachLogMobile*, pLogMobile_ );
	CB_DEPIMPL( MachLogMachineConvoy*, pConvoy_ );
	CB_DEPIMPL( MachLogMachineMotionSequencer*, pFollowSequencer_ );
	CB_DEPIMPL( int, nFollowers_ );

    o << "--------Convoy data for " << (void*)this << std::endl;
    ProProfiler::instance().traceStack( o, true, 0, "" );
    o << "  id " << pLogMobile_->id() << "  pConvoy_ " << (void*)pConvoy_ << "  pFollowSequencer_ " <<(void*)pFollowSequencer_ << std::endl;
    o << "  nFollowers_ " << nFollowers_ << std::endl;
    o << "------------------------" << std::endl;
    return o;
}

void MachLogMachineMotionSequencer::mergeConvoys( MachLogMachineConvoy* pConvoy )
{
    LOG_ENTER( "mergeConvoys" );

	CB_DEPIMPL( MachLogMachineConvoy*, pConvoy_ );
    PRE( pConvoy != NULL );
    PRE( pConvoy_ != NULL );

    //Iterate thru the argument convoy, moving all its members to this object's convoy
    if( pConvoy != pConvoy_ )
    {
        MachLogMachineConvoy* pBiggerConvoy = pConvoy_;

        const MachLogMachineConvoy::Sequencers& sequencers = pConvoy->sequencers();
        size_t nToMove = sequencers.size();
        for( size_t i = 0; i != nToMove; ++i )
        {
            MachLogMachineMotionSequencer* pSequencer = sequencers[i];
            pSequencer->pImpl_->pConvoy_ = NULL; //Needed for precondition
            pBiggerConvoy->add( pSequencer );
            pSequencer->pImpl_->pConvoy_ = pBiggerConvoy;
        }

        //Delete the old convoy
        _DELETE( pConvoy );
    }
}

//static
void MachLogMachineMotionSequencer::updateConvoy( MachLogMachineConvoy* pOldConvoy )
{
    PRE( pOldConvoy != NULL );

    //Iterate thru all the machines in the convoy, setting their convoy pointers to null
    const MachLogMachineConvoy::Sequencers& sequencers = pOldConvoy->sequencers();
    size_t nSequencers = sequencers.size();

    for( size_t i = 0; i != nSequencers; ++i )
        sequencers[i]->pImpl_->pConvoy_ = NULL;

    for( size_t i = 0; i != nSequencers; ++i )
    {
        //Only process this machine if it is a follower, and hasn't yet been assigned to a convoy
        MachLogMachineMotionSequencer* pStartSequencer = sequencers[i];
        if( pStartSequencer->pImpl_->pFollowSequencer_ != NULL and
            pStartSequencer->pImpl_->pConvoy_ == NULL )
        {
            //Assign the old convoy pointer to this and any sequence of machines being followed.
            //This prevents infinite recursion with cyclic following relations, marking each sequencer
            //in the run starting at pStartSequencer.
            MachLogMachineMotionSequencer* pCurrentSequencer;
            for( pCurrentSequencer = pStartSequencer;
                 pCurrentSequencer != NULL and pCurrentSequencer->pImpl_->pConvoy_ == NULL;
                 pCurrentSequencer = pCurrentSequencer->pImpl_->pFollowSequencer_ )
            {
                pCurrentSequencer->pImpl_->pConvoy_ = pOldConvoy;
            }

            //If we have stopped on a sequencer which has already been assigned a new convoy,
            //then add all the machines in our run to this convoy.
            //Otherwise create a new convoy, and add them all to that.
            MachLogMachineConvoy* pNewConvoy;
            if( pCurrentSequencer != NULL and pCurrentSequencer->pImpl_->pConvoy_ != pOldConvoy )
            {
                pNewConvoy = pCurrentSequencer->pImpl_->pConvoy_;
                pStartSequencer->pImpl_->pConvoy_ = NULL;
                pNewConvoy->add( pStartSequencer );
            }
            else
            {
                pNewConvoy = _NEW( MachLogMachineConvoy( pStartSequencer ) );
            }

            //Ensure start machine has convoy set
            pStartSequencer->pImpl_->pConvoy_ = pNewConvoy;

            //Now add remaining machines in run to same convoy
            for( pCurrentSequencer = pStartSequencer->pImpl_->pFollowSequencer_;
                 pCurrentSequencer != NULL and pCurrentSequencer->pImpl_->pConvoy_ != pNewConvoy;
                 pCurrentSequencer = pCurrentSequencer->pImpl_->pFollowSequencer_ )
            {
                pCurrentSequencer->pImpl_->pConvoy_ = NULL;
                pNewConvoy->add( pCurrentSequencer );
                pCurrentSequencer->pImpl_->pConvoy_ = pNewConvoy;
            }
        }
    }

    //Finally delete the old convoy. None of the machines will be a member of it any longer.
    _DELETE( pOldConvoy );
}

MATHEX_SCALAR MachLogMachineMotionSequencer::scalarSpeedNow() const
{
    LOG_ENTER( "scalarSpeedNow" );

	CB_DEPIMPL( ctl_vector< MotionChunkId >, motionChunkIds_ );
	CB_DEPIMPL( PhysConfigSpace2d*, pConfigSpace_);

	PRE( not is1stPersonControlled() );

	MATHEX_SCALAR speedNow = 0.0;	// unless proven otherwise

	if( motionChunkIds_.size() != 0 )
	{
		PhysAbsoluteTime timeNow = SimManager::instance().currentTime();
		for( ctl_vector< MotionChunkId >::reverse_iterator rIter = motionChunkIds_.rbegin();
													  	   rIter != motionChunkIds_.rend();
														 ++rIter )
		{
			const PhysMotionChunk& candidateChunk = pConfigSpace_->motionChunk( (*rIter ) );
			const PhysAbsoluteTime& chunkCreateTime = candidateChunk.createTime();
			if( timeNow > chunkCreateTime )
			{
				// this is our baby.
				//Now, is this chunk still in its turning phase?
				PhysAbsoluteTime endOfTurnPhase = chunkCreateTime +  candidateChunk.motionTimeOffset();
				if( timeNow > endOfTurnPhase )
				{
					// it MAY have actually finished its move (in which case, speed remains at 0).
					if( timeNow < endOfTurnPhase + candidateChunk.motionProfile().totalTime() )
						speedNow = physMachine().machineData().speed();
				}
				break;
			}
		}
	}

	return speedNow;


}

void MachLogMachineMotionSequencer::pathFindingPriority( PhysPathFindingPriority priority )
{
    pImpl_->pathFindingPriority_ = priority;
}

void MachLogMachineMotionSequencer::writePortalPoints() const
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    if( LOG_ENABLED )
    {
        LOG_STREAM( "Start portal points" << std::endl );
        LOG_INDENT( 2 );

        for( size_t i = 0; i < domainPath_.size(); ++i )
        {
            const PortalPoint& portalPoint = domainPath_[ i ];
            const PhysConfigSpace2d::PortalId portalId = portalPoint.first;
            const MATHEX_SCALAR distance = portalPoint.second;

            //Get the point
            const MexPoint2d point = pConfigSpace_->portalPoint( portalId, distance );

            LOG_STREAM( i << "  " << point << std::endl );
        }

        LOG_INDENT( -2 );
        LOG_STREAM( "End portal points" << std::endl );
    }
}

void MachLogMachineMotionSequencer::writePathPoints() const
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    if( LOG_ENABLED )
    {
        LOG_STREAM( "Start path points" << std::endl );
        LOG_INDENT( 2 );

        for( size_t i = 0; i < path_.size(); ++i )
        {
            LOG_STREAM( i << "  " << path_[ i ] << std::endl );
        }

        LOG_INDENT( -2 );
        LOG_STREAM( "End path points" << std::endl );
    }
}

#pragma GCC diagnostic pop
/* End MCMOTSEQ.CPP *************************************************/
