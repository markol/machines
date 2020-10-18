/*
 * M C M O T S E 2 . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
	Overflow file for mcmotseq.cpp. There is no logical partitioning
	of functions between the two files - the newer functions tend to
	be in here. It's just an effort to try and reduce compile times.
*/

#define _TEST_INVARIANTS

#include "machlog/mcmotseq.hpp"
#include "machlog/cntrl.hpp"
#include "machlog/internal/seqdata.hpp"
#include "machlog/internal/seqdebug.hpp"
#include "machlog/messbrok.hpp"
#include "machlog/races.hpp"
#include "machlog/network.hpp"
#include "machphys/mcmovinf.hpp"
#include "machphys/machine.hpp"
#include "machphys/plansurf.hpp"
#include "machlog/planet.hpp"
#include "mathex/poly2d.hpp"
#include "sim/manager.hpp"
#include "mathex/sausag2d.hpp"
#include "mathex/random.hpp"
#include "mathex/eulerang.hpp"
#include "device/time.hpp"
#include "machlog/internal/collinfo.hpp"

#include "profiler/stktrace.hpp"

//  These are purely to allow testing of alternative strategies.
static size_t nCallsReserveChunk = 0;
static size_t nReducedSpeedSucceed = 0;
static size_t nChunksSwervesOK = 0;
static size_t nStaticObstacleFail = 0;
static size_t nStationaryMachineFail = 0;
static size_t nNormalSpeedSucceed = 0;
static size_t nDelayedStartSucceed = 0;

bool MachLogMachineMotionSequencer::trySwerveMove
(
    const MexTransform3d& startTransform,
	OrientationInfo orientationInfo,
    const MexPoint2d& endPoint,
    PhysAbsoluteTime startTime,
    MATHEX_SCALAR startSpeed,
    ObjectId collisionObjectId,
    const MexPoint2d& collisionPoint,
    MachPhys::FinalState desiredFinalState,
    const PhysConfigSpace2d::ObjectIds& ignoreIds,
    MachPhys::FinalState* pActualFinalState,
    MoveResult* pMoveResult
)
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_ENTER( "trySwerveMove" );

	LOG_INSPECT( startTransform );
    LOG_INSPECT( orientationInfo );
	LOG_INSPECT( endPoint );
	LOG_INSPECT( startTime );
	LOG_INSPECT( startSpeed );
	LOG_INSPECT( collisionPoint );
	LOG_INSPECT( desiredFinalState );
    LOG_STREAM( "Total motion chunks reserved : " << motionChunkIds_.size() << std::endl );

    const size_t originalNChunks = motionChunkIds_.size();

    bool    foundPath = false;

    ++swerveRecursionCount_;

    LOG_INSPECT( swerveRecursionCount_ );

    //  If we are at the limit of the number of motion chunks the network
    //  can handle we can't attempt to swerve. Also don't allow too many
    //  swerves within swerves

    if( motionChunkIds_.size() + 2 < MachLogMessageBroker::maximumMotionChunks() and
        swerveRecursionCount_ < 3 )
    {
        MATHEX_SCALAR obstructingObjectsClearance = 5.0;

        UtlId id = collisionObjectId.asScalar();

        MachLogRaces& races = MachLogRaces::instance();
        if( races.actorExists( id ) and
          races.actor( id ).objectIsMachine() and
          races.actor( id ).race() == logMobile().race() )
        {
            MachLogMachineMotionSequencer& obstructingSequencer =
                races.actor( id ).asMobile().motionSeq();

            obstructingObjectsClearance = obstructingSequencer.pImpl_->useClearance_;
        }

		LOG_INSPECT( obstructingObjectsClearance );

        const MexVec2 travelDirection( startTransform.position(), endPoint );
        MexVec2 travelNormal( travelDirection.normal() );
        travelNormal.makeUnitVector();

		LOG_INSPECT( travelDirection );
		LOG_INSPECT( travelNormal );

        const MATHEX_SCALAR offsetAmount = 1.5 * ( obstructingObjectsClearance + useClearance_ );

        travelNormal *= offsetAmount;

        Point2ds swervePoints( 2, collisionPoint );

        swervePoints[ 0 ] += travelNormal;
        swervePoints[ 1 ] -= travelNormal;

		LOG_INSPECT( swervePoints[ 0 ] );
		LOG_INSPECT( swervePoints[ 1 ] );

        for( size_t i = 0; i < swervePoints.size() and not foundPath; ++i )
        {
            Point2ds path;
            path.reserve( 2 );
            path.push_back( swervePoints[ i ] );
            path.push_back( endPoint );

            MachLogCollisionInfo    dummyCollisionInfo;
            MoveInfoAPtr dummyMoveInfoAPtr;

            size_t pointsReserved = reservePath( startTransform, orientationInfo, path, startTime, startSpeed, desiredFinalState,
              CHECK_STATIC_OBSTACLES,
              ignoreIds,
              pActualFinalState, &dummyCollisionInfo, &dummyMoveInfoAPtr,
              pMoveResult );

            //  For a swerve move to succeed we have to reach all of the points
            if( pointsReserved == path.size() )
            {
                foundPath = true;
            }
            else
            {
                setNChunks( originalNChunks );
            }
        }
    }

    ASSERT( swerveRecursionCount_ != 0, "" );
    --swerveRecursionCount_;

    LOG_STREAM( ( foundPath ? "Swerve worked" : "Swerve failed" ) << std::endl );
    LOG_STREAM( "Total motion chunks reserved : " << motionChunkIds_.size() << std::endl );
    LOG_INSPECT( *pMoveResult );

    POST( implies( not foundPath, originalNChunks == motionChunkIds_.size() ) );
    POST( implies( foundPath, originalNChunks < motionChunkIds_.size() ) );
    POST( motionChunkIds_.size() <= MachLogMessageBroker::maximumMotionChunks() );

    return foundPath;
}

//  This function might be recursively called from trySwerveMove
//  Returns the number of path points sucessfully reached

size_t MachLogMachineMotionSequencer::reservePath
(
    const MexTransform3d& startTransform,
	OrientationInfo orientationInfo,
    const Point2ds& path,
    PhysAbsoluteTime startTime,
    MATHEX_SCALAR startSpeed,
    MachPhys::FinalState desiredFinalState,
    ObstacleCheck obstacleCheck,
    const PhysConfigSpace2d::ObjectIds& ignoreIds,
    MachPhys::FinalState* pActualFinalState,
    MachLogCollisionInfo* pCollisionInfo,
    MoveInfoAPtr* pMoveInfoAPtr,
    MoveResult* pMoveResult
)
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_ENTER( "reservePath" );

	LOG_STREAM( "Starting transform: " << std::endl << startTransform );
    LOG_INSPECT( orientationInfo );
	LOG_STREAM( "Path:\n" << path );
	LOG_INSPECT( startTime );
	LOG_INSPECT( startSpeed );
	LOG_INSPECT( desiredFinalState );
    LOG_STREAM( "Total motion chunks reserved : " << motionChunkIds_.size() << std::endl );

    PRE_INFO( path );
    PRE( path.size() > 0 );

    POST_DATA( size_t originalNChunks = motionChunkIds_.size() );

    size_t pathPointIndex = 0;

	//	Because we might reserve more than one motion chunk for each point
	//  on the path we need to keep track of how many motion chunks we have
	//  in the list for each point so that if we have to backtrack we can
	//  backtrack to a point in the path rather than some intermediate point.
	ctl_vector< size_t > nChunks;
    nChunks.reserve( path.size() + 2 );
	nChunks.push_back( motionChunkIds_.size() );

    MexTransform3d fromTransform = startTransform;

    bool finished = false;
    const MATHEX_SCALAR speedLimit = capSpeed();

    *pActualFinalState = MachPhys::IN_MOTION;

    while( not finished )
    {
        ASSERT( pathPointIndex < path.size(), "" );

        const MexPoint3d& toPoint = path[ pathPointIndex ];

		LOG_INSPECT( pathPointIndex );
        LOG_INSPECT( fromTransform );
		LOG_INSPECT( toPoint );
        LOG_INSPECT( orientationInfo );
        LOG_INSPECT( startTime );
        LOG_INSPECT( startSpeed );

        // Find out what we want the state to be for this part of the move
        *pActualFinalState = finalStateToAimFor( startTransform.position(), path, pathPointIndex, desiredFinalState );

        //  Try a simple straight line move
        *pMoveResult = reserveChunk( fromTransform,
          orientationInfo, toPoint, startTime,
          startSpeed, *pActualFinalState, obstacleCheck,
          ignoreIds,
          pCollisionInfo, pMoveInfoAPtr );

		bool motionPossible = false;

		if( *pMoveResult == SUCCESS )
        {
			motionPossible = true;
        }
        else if( *pMoveResult == COLLIDED_WITH_MACHINE )
        {
            //  Try and do a swerve move.

            const MexPoint3d collisionPoint = pCollisionInfo->collisionPoint();

            MoveResult dummyMoveResult;

            motionPossible = trySwerveMove( fromTransform, orientationInfo,
              toPoint, startTime,
              startSpeed, pCollisionInfo->collisionObjectId(), collisionPoint,
              *pActualFinalState,
              ignoreIds,
              pActualFinalState, &dummyMoveResult );

            if( motionPossible )
            {
                *pMoveResult = SUCCESS;
                ++nChunksSwervesOK;
            }
        }

        if( motionPossible )
        {
			LOG_WHERE;

			nChunks.push_back( motionChunkIds_.size() );

            ++pathPointIndex;

            if( *pActualFinalState == MachPhys::AT_REST )
                finished = true;
            else if( pathPointIndex == path.size() )
                finished = true;
            else
            {
				LOG_WHERE;

                //  Set up the data for the next time around the loop

                fromTransform = pPhysMachine_->pParent()->globalTransform();
                fromTransform.transform( moveInfos_.back().endTransform() );

              	orientationInfo = MachPhysCanMove::DONT_USE_ORIENTATION;

                startTime = moveInfos_.back().startTime() + moveInfos_.back().totalTime();
                startSpeed = moveInfos_.back().moveProfiles().back().endSpeed();
            }

			LOG_WHERE;
        }
        else
        {
            //  We failed to make the movement so exit
            finished = true;
        }

		LOG_WHERE;
    }

    ASSERT( implies( pathPointIndex > 0, moveInfos_.size() > 0 ), "" );

    if( pathPointIndex > 0 )
    {
        *pActualFinalState = moveInfos_.back().finalState();

    	//	If we were asked to come to rest at the end of this motion we need to be
        //  sure that we really have. Update pathPointIndex with the number of points
        //  we have actually managed to deal with before coming to rest.

        ASSERT_INFO( nChunks );

    	if( desiredFinalState == MachPhys::AT_REST and *pActualFinalState != MachPhys::AT_REST )
    	{
    		forceMotionToRest( ignoreIds, &nChunks );
            pathPointIndex = nChunks.size();

            if( pathPointIndex == 0 )
                *pMoveResult = COLLIDED_WITH_MACHINE;

    		*pActualFinalState = MachPhys::AT_REST;
    	}
    }

    LOG_INSPECT( pathPointIndex );
    LOG_STREAM( "Total motion chunks reserved : " << motionChunkIds_.size() << std::endl );
    LOG_INSPECT( *pMoveResult );

    POST( implies( pathPointIndex == 0, originalNChunks == motionChunkIds_.size() ) );
    POST( originalNChunks <= motionChunkIds_.size() );

    POST( motionChunkIds_.size() <= MachLogMessageBroker::maximumMotionChunks() );
	POST( implies( desiredFinalState == MachPhys::AT_REST and pathPointIndex > 0, *pActualFinalState == MachPhys::AT_REST ) );
	POST( implies( pathPointIndex > 0, *pActualFinalState == moveInfos_.back().finalState() ) );
    POST( implies( pathPointIndex == path.size(), *pMoveResult == SUCCESS ) );

    return pathPointIndex;
}


//  Try and reserve a single motion chunk. If the reserve is successful the config space,
//  movingChunkIds and moveInfos will all be updated. If the reserve is unsuccessful details
//  of the collision which caused things to fail are returned in pCollisionInfo

MachLogMachineMotionSequencer::MoveResult
MachLogMachineMotionSequencer::reserveChunk(
    const MexTransform3d& fromTransform,
	OrientationInfo orientationInfo,
    const MexPoint3d& toPoint,
    const PhysAbsoluteTime& startTime,
    MATHEX_SCALAR startSpeed,
    MachPhys::FinalState finalState,
    ObstacleCheck obstacleCheck,
    const PhysConfigSpace2d::ObjectIds& ignoreIds,
    MachLogCollisionInfo* pCollisionInfo,
    MoveInfoAPtr* pMoveInfoAPtr )
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_INSPECT( (void*)pImpl_ );

    LOG_ENTER( "reserveChunk" );
    LOG_STREAM( "Try to reserve chunk from:\n" << fromTransform << "to " << toPoint << std::endl );
    LOG_STREAM( "Total motion chunks reserved : " << motionChunkIds_.size() << std::endl );
    LOG_INSPECT( orientationInfo );

    LOG_INSPECT( finalState );

    ++nCallsReserveChunk;

    bool    motionPossible = true;

    const MexSausage2d sausage( fromTransform.position(), toPoint, useClearance_ );

  	MoveResult result;

    if( obstacleCheck == CHECK_STATIC_OBSTACLES )
    {
        //  Check for intersections with static obstacles
        PhysConfigSpace2d::PolygonIds  dummyIds;
        dummyIds.reserve( 64 );

        motionPossible = currentConfigSpace().contains( sausage, obstacleFlags(), &dummyIds );

        if( motionPossible )
        {
            LOG_STREAM( "motion chunk did not intersect with any static obstacles" << std::endl );
        }
        else
        {
            LOG_STREAM( "motion chunk intersected with " << dummyIds.size() << " static obstacles" << std::endl );
            LOG_STREAM( "Failed to reserve chunk because of collision with static obstacle" << std::endl );

	    	result = COLLIDED_WITH_STATIC_OBSTACLE;
            ++nStaticObstacleFail;
        }

        for( size_t i = 0; i < dummyIds.size(); ++i )
        {
            LOG_STREAM( "   " << currentConfigSpace().polygon( dummyIds[ i ] ) << std::endl );
        }
    }

    PhysConfigSpace2d::ChunkIntersectionsNoTime chunkIntersections;

    if( motionPossible )
    {
        //  Check for collisions with stationary machines. Also come up with a
        //  list of motion chunks that intersect with this sausage to save time
        //  later.

        currentConfigSpace().findIntersections( sausage, ignoreIds, &chunkIntersections );

        PhysConfigSpace2d::ChunkIntersectionDataNoTime chunkIntersectionData;

        if( anyMotionChunkStationary( chunkIntersections, &chunkIntersectionData ) )
        {
            LOG_STREAM( "Failed to reserve chunk because of collision with stationary machine" << std::endl );

            ++nStationaryMachineFail;
    		result = COLLIDED_WITH_MACHINE;
            motionPossible = false;

            const PhysMotionChunk& chunk =
              currentConfigSpace().motionChunk(
              chunkIntersectionData.collisionChunkId() );

            LOG_INSPECT( chunk );

            const MachLogCollisionInfo info(
              chunk.startPoint(),
              chunkIntersectionData.collisionObjectId() );

            *pCollisionInfo = info;

        }
    }

    const MATHEX_SCALAR speedLimit = capSpeed();

    MotionChunkId   motionChunkId;

    if( motionPossible )
	{
		result = COLLIDED_WITH_MACHINE;

        MachPhysMachine::ProfilePtr profilePtr =
          pPhysMachine_->profile(
            fromTransform, orientationInfo,
            toPoint, *MachLogPlanet::instance().surface() );

        motionPossible = doReserveChunk(
            profilePtr, chunkIntersections, startTime,
            startSpeed, speedLimit, finalState,
            ignoreIds, &motionChunkId,
            pCollisionInfo, pMoveInfoAPtr );

        if( motionPossible )
            ++nNormalSpeedSucceed;

        if( not motionPossible )
        {
            LOG_STREAM( "Failed to reserve chunk at normal speed - " << *pCollisionInfo << std::endl );

            //  Only try other ways of getting this chunk down if the machine
            //  is player controlled - we can put up with slightly dodgy
            //  movement for the AI races but we want the player controlled
            //  race to be as responsive as possible.

            if( isPlayerControlled() )
            {
                //  We failed to reserve a chunk at normal speed - we're going to
                //  try a couple of other things to see if we can get a chunk down.

                //  If we're going from a standing start see if delaying the start time will help

                if( MexEpsilon::isWithinEpsilonOf( startSpeed, 0.0 ) )
                {
                    for( PhysRelativeTime timeOffset = 0.5;
                      timeOffset < 0.7 and ( not motionPossible );
                      timeOffset += 0.5 )
                    {
                        motionPossible = doReserveChunk(
                            profilePtr, chunkIntersections, startTime + timeOffset,
                            startSpeed, speedLimit, finalState,
                            ignoreIds, &motionChunkId,
                            pCollisionInfo, pMoveInfoAPtr );

                        if( motionPossible )
                        {
                            ++nDelayedStartSucceed;
                        }
                    }

                    if( motionPossible )
                    {
                        LOG_STREAM( "Reserved chunk with delayed start time" << std::endl );
                    }
                    else
                    {
                        LOG_STREAM( "Failed to reserve chunk with delayed start time" << std::endl );
                    }
                }

                if( not motionPossible )
                {
                    //  See if going at a slower speed helps

                    motionPossible = doReserveChunk(
                        profilePtr, chunkIntersections, startTime,
                        startSpeed, speedLimit * speedProportion(), finalState,
                        ignoreIds, &motionChunkId,
                        pCollisionInfo, pMoveInfoAPtr );

                    if( motionPossible )
                        ++nReducedSpeedSucceed;
                    else
                    {
                        LOG_STREAM( "Failed to reserve chunk at reduced speed - " << *pCollisionInfo << std::endl );
                    }
                }
            }
        }
	}

    if( motionPossible )
    {
        const MachPhysMachineMoveInfo* pInfo = &(**pMoveInfoAPtr);

        ASSERT( pInfo != NULL, "" );

        LOG_INSPECT( *pInfo );

        moveInfos_.push_back( *pInfo );
        motionChunkIds_.push_back( motionChunkId );

		NETWORK_STREAM("(" << pLogMobile_->id() << ") !! Added motion chunk with id " << motionChunkId << "callstack\n" << ProStackTracer() << std::endl << "!! add end\n" );
		LOG_STREAM( "Motion is possible" << std::endl );

		result = SUCCESS;
    }
    else
    {
        LOG_STREAM( "Could not reserve chunk - " << *pCollisionInfo << std::endl );
    }

    LOG_STREAM( "Total motion chunks reserved : " << motionChunkIds_.size() << std::endl );
    LOG_INSPECT( result );

    POST( moveInfos_.size() == motionChunkIds_.size() );
    POST( implies( result == SUCCESS and finalState == MachPhys::AT_REST,
      moveInfos_.back().finalState() == MachPhys::AT_REST ) );

    return result;
}

//  Reserve as many chunks as possible between our current path points
//  Returns the number of motion chunks that could be reserved. If the
//  number of chunks that could be reserved is zero, the id of the
//  object that caused the collision is returned in pCollisionObjectId.

size_t MachLogMachineMotionSequencer::reserveMotionChunks(
	ObjectId* pCollisionObjectId )
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_ENTER( "reserveMotionChunks" );
    LOG_STREAM( "Total motion chunks reserved : " << motionChunkIds_.size() << std::endl );

    PRE_INFO( motionChunkIds_.size() );
    PRE_INFO( moveInfos_.size() );

    PRE( motionChunkIds_.size() == 0 );
    PRE( moveInfos_.size() == 0 );

    MexTransform3d fromTransform = pPhysMachine_->globalTransform();
    PhysAbsoluteTime startTime = SimManager::instance().currentTime();
    bool finished = false;
    MATHEX_SCALAR startSpeed = 0.0;
    const MATHEX_SCALAR speedLimit = capSpeed();

    PhysConfigSpace2d::ObjectIds    ignoreIds;
    getIntersectingIds( &ignoreIds );

	//	TBD: It should be possible to do this with a constructor, unfortunately
	//  the conversion between nb_vector iterators and nb_vector const_iterators
	//  doesn't work properly.

	Point2ds path;
    path.reserve( path_.size() );
	for( Point2ds::iterator i = path_.begin() + nPathPointsDone_; i != path_.end(); ++i )
		path.push_back( *i );

    MachPhys::FinalState actualFinalState;
    MachLogCollisionInfo    collisionInfo;
    MoveInfoAPtr dummyMoveInfoAPtr;

    MoveResult moveResult;

    //  The journeys between the path points have already been checked
    //  against static obstacles
	size_t nPointsReached = reservePath( fromTransform,
	    MachPhysMachine::USE_ORIENTATION, path, startTime,
	    startSpeed, MachPhys::AT_REST, DONT_CHECK_STATIC_OBSTACLES,
        ignoreIds,
        &actualFinalState,
        &collisionInfo,
	    &dummyMoveInfoAPtr,
        &moveResult );

    if( nPointsReached == 0 )
    {
        *pCollisionObjectId = collisionInfo.collisionObjectId();

        ASSERT( collisionInfo.valid(), "" );

        const MexPoint2d fromPoint2d = pPhysMachine_->globalTransform().position();
        MexPoint2d toPoint( path_[ nPathPointsDone_ ] );

        //  See if we can get at least some way along our first path segment
        if( tryNearerPoint( fromPoint2d, toPoint, collisionInfo.collisionPoint(), &toPoint ) )
        {
            reserveChunk( fromTransform, MachPhysMachine::USE_ORIENTATION, toPoint, startTime, startSpeed,
              MachPhys::AT_REST, DONT_CHECK_STATIC_OBSTACLES,
              ignoreIds, &collisionInfo, &dummyMoveInfoAPtr );
        }
    }

    POST_INFO( motionChunkIds_.size() );
    POST_INFO( moveInfos_.size() );

    POST_INFO( MachLogMessageBroker::maximumMotionChunks() );

    POST( implies( moveInfos_.size() != 0, moveInfos_.back().finalState() == MachPhys::AT_REST ) );

    LOG_STREAM( "Total motion chunks reserved : " << motionChunkIds_.size() << std::endl );

    LOG_INSPECT( nCallsReserveChunk );
    LOG_INSPECT( nReducedSpeedSucceed );
    LOG_INSPECT( nChunksSwervesOK );
    LOG_INSPECT( nStaticObstacleFail );
    LOG_INSPECT( nStationaryMachineFail );
    LOG_INSPECT( nNormalSpeedSucceed );
    LOG_INSPECT( nDelayedStartSucceed );

    return motionChunkIds_.size();
}

void MachLogMachineMotionSequencer::forceMotionToRest(
    const PhysConfigSpace2d::ObjectIds& ignoreIds,
    ctl_vector< size_t >* pNChunks )
{
    TEST_INVARIANT;

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_ENTER( "forceMotionToRest" );
    LOG_STREAM( "Total motion chunks reserved : " << motionChunkIds_.size() << std::endl );

    PRE( pNChunks->size() > 0 );
    PRE( motionChunkIds_.size() > 0 );
	PRE( pNChunks->back() == motionChunkIds_.size() );

	//	Drop the last element - we already know how many chunks we have
	//  for the current point
	pNChunks->pop_back();

	while( pNChunks->size() > 0 and moveInfos_.back().finalState() != MachPhys::AT_REST )
	{
        ASSERT_INFO( *pNChunks );
        ASSERT_INFO( motionChunkIds_.size() );

        //  See if we can make the last segment come to rest

        MotionChunkId motionChunkId = motionChunkIds_.back();
        const MachPhysMachineMoveInfo& info = moveInfos_.back();

        pConfigSpace_->removeMotionChunk( motionChunkId );

        MexTransform3d fromTransform = pPhysMachine_->pParent()->globalTransform();
        fromTransform.transform( info.startTransform() );

        MexTransform3d  toTransform( pPhysMachine_->pParent()->globalTransform() );
        toTransform.transform( info.endTransform() );

        const MexPoint3d& toPoint = toTransform.position();
        MATHEX_SCALAR startTime = info.startTime();
        MATHEX_SCALAR startSpeed = info.moveProfiles().front().startSpeed();

        motionChunkIds_.pop_back();
        moveInfos_.pop_back();

    	MoveInfoAPtr            dummyCollideMoveInfoAPtr;
        MachLogCollisionInfo    dummyCollisionInfo;

        LOG_STREAM( "Attempt to make segment come to rest" << std::endl );
        LOG_STREAM( "From:\n" << fromTransform );
        LOG_STREAM( "To: " << toPoint << std::endl );

        MoveResult moveResult = reserveChunk( fromTransform,
          MachPhysMachine::USE_ORIENTATION, toPoint, startTime,
          startSpeed, MachPhys::AT_REST, DONT_CHECK_STATIC_OBSTACLES,
          ignoreIds, &dummyCollisionInfo,
          &dummyCollideMoveInfoAPtr );

		if( moveResult != SUCCESS )
        {
            ASSERT_INFO( *pNChunks );
            ASSERT_INFO( motionChunkIds_ );
            ASSERT( pNChunks->size() != 0, "" );

			//	Remove motion chunks back to the previous point

			setNChunks( pNChunks->back() );
			pNChunks->pop_back();
        }
    }

    POST( implies( pNChunks->size() > 0, moveInfos_.back().finalState() == MachPhys::AT_REST ) );

    LOG_STREAM( "Total motion chunks reserved : " << motionChunkIds_.size() << std::endl );

    TEST_INVARIANT;
}

void MachLogMachineMotionSequencer::setNChunks( size_t nChunks )
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_ENTER( "setNChunks" );
    LOG_INSPECT( nChunks );
    LOG_STREAM( "Total motion chunks reserved : " << motionChunkIds_.size() << std::endl );

	TEST_INVARIANT;
	PRE_INFO( nChunks );
	PRE_INFO( motionChunkIds_.size() );

	PRE( nChunks <= motionChunkIds_.size() );

	while( motionChunkIds_.size() > nChunks )
	{
        MotionChunkId motionChunkId = motionChunkIds_.back();
        const MachPhysMachineMoveInfo& info = moveInfos_.back();

        pConfigSpace_->removeMotionChunk( motionChunkId );

        motionChunkIds_.pop_back();
        moveInfos_.pop_back();
	}

	POST_INFO( nChunks );
	POST_INFO( motionChunkIds_.size() );
	POST( nChunks == motionChunkIds_.size() );
	TEST_INVARIANT;

    LOG_STREAM( "Total motion chunks reserved : " << motionChunkIds_.size() << std::endl );
}

bool MachLogMachineMotionSequencer::doReserveChunk(
    MachPhysMachine::ProfilePtr profilePtr,
    const PhysConfigSpace2d::ChunkIntersectionsNoTime& chunkIntersections,
    const PhysAbsoluteTime& startTime,
    MATHEX_SCALAR startSpeed,
    MATHEX_SCALAR maxSpeed,
    MachPhys::FinalState finalState,
    const PhysConfigSpace2d::ObjectIds& ignoreIds,
    MotionChunkId* pMotionChunkId,
    MachLogCollisionInfo* pCollisionInfo,
    MoveInfoAPtr* pInfoAPtr ) const
{
    LOG_ENTER( "doReserveChunk" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    *pInfoAPtr = pPhysMachine_->moveInfo(
        profilePtr,
        startTime, startSpeed, maxSpeed, finalState );

    const MachPhysMachineMoveInfo& info = (**pInfoAPtr);

    const PhysRampAcceleration& motionProfile = info.averageProfile();

    const MexTransform3d& parentTransform = pPhysMachine_->pParent()->globalTransform();

    MexTransform3d fromTransform = profilePtr->front();
    fromTransform.preTransform( parentTransform );

    MexPoint3d toPoint = profilePtr->back().position();
    parentTransform.transform( &toPoint );

    const PhysMotionChunk motionChunk( fromTransform.position(), toPoint, useClearance_, motionProfile,
                                 startTime, info.turnTime(), minHeight(), maxHeight() );

    ObjectId            collisionObjectId = ObjectId::invalidId();
    PhysAbsoluteTime    collideTime = 0;

    bool motionPossible = pConfigSpace_->add( motionChunk, pLogMobile_->id(),
      ignoreIds, chunkIntersections, pMotionChunkId, &collisionObjectId, &collideTime );

    if( not motionPossible )
    {
        *pCollisionInfo = MachLogCollisionInfo( info, parentTransform, collisionObjectId, collideTime );
    }

    LOG_STREAM( ( motionPossible ? "managed to reserve chunk" : "could not reserve chunk" ) << std::endl );

    return motionPossible;
}

ostream& operator <<( ostream& o, const MachLogMachineMotionSequencer::MoveResult& t )
{
    switch( t )
    {
        case MachLogMachineMotionSequencer::COLLIDED_WITH_MACHINE:
            o << "COLLIDED_WITH_MACHINE";
            break;
        case MachLogMachineMotionSequencer::COLLIDED_WITH_STATIC_OBSTACLE:
            o << "COLLIDED_WITH_STATIC_OBSTACLE";
            break;
        case MachLogMachineMotionSequencer::SUCCESS:
            o << "SUCCESS";
            break;
    }

    return o;
}

bool MachLogMachineMotionSequencer::shuffling() const
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    return SimManager::instance().currentTime() < shuffleEndTime_;
}

void MachLogMachineMotionSequencer::stopShuffling()
{
    LOG_ENTER( "stopShuffling" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    PRE( shuffling() );

    pPhysMachine_->stopDead();
    shuffleEndTime_ = SimManager::instance().currentTime();

    POST( not shuffling() );
}

void MachLogMachineMotionSequencer::shuffle()
{
    LOG_ENTER( "shuffle" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    PRE_INFO( internalState_ );
    PRE( internalState_ != INTERNAL_RESTING );
    PRE( internalState_ != INTERNAL_MOVING );
    PRE( internalState_ != INTERNAL_1ST_PERSON );

    if( not shuffling() )
    {
        //Get the machine's current position
        const MexTransform3d& currentTransform = pPhysMachine_->globalTransform();
        const MexPoint2d currentLocation = currentTransform.position();

        //Find a point to aim at - nearest of next path point or destination
        MexPoint2d target( destinationPoint_ );
        bool targetPointFound = false;
        const MATHEX_SCALAR sqr1cm = 0.0001;

        LOG_WHERE;
        LOG_INSPECT( target );

        for( size_t index = nPathPointsDone_; index < path_.size()  and  not targetPointFound; ++index )
        {
            LOG_INSPECT( path_[ index ] );

            if( path_[ index ].sqrEuclidianDistance( currentLocation ) > sqr1cm )
            {
                targetPointFound = true;
                target = path_[ index ];

                LOG_WHERE;
                LOG_INSPECT( target );
            }
        }

        if( not targetPointFound  and  target.sqrEuclidianDistance( currentLocation ) < sqr1cm )
        {
            target.x( target.x() + 0.01 );

            LOG_WHERE;
            LOG_INSPECT( target );
        }

        //  Create a transform pointing at the target

        const MexVec3 targetVector( currentTransform.position(), target );

        LOG_INSPECT( currentTransform );
        LOG_INSPECT( currentTransform.position() );
        LOG_INSPECT( target );
        LOG_INSPECT( currentTransform.zBasis() );
        LOG_INSPECT( targetVector );

        MexTransform3d targetTransform(
            MexTransform3d::Z_ZX,
            currentTransform.zBasis(),
            targetVector,
            currentTransform.position() );

        // Now rotate the transform about Z so that it isn't necessarily
        // pointing immediately at the target. Make sure that we do actually
        // come up with a different transform

        MexRadians turnAngle;

        do
        {
            MexBasicRandom r;
            r.seedFromTime();

            const MexDegrees rotationAngle = mexRandomScalar( &r, -30.0, 30.0 );

            targetTransform.rotate( MexEulerAngles( rotationAngle, 0.0, 0.0 ) );

            turnAngle = currentTransform.xBasis().angleBetween( targetTransform.xBasis() );
        } while( MexEpsilon::isWithinEpsilonOf( turnAngle.asScalar(), 0.0 ) );

        //  Get the move info corresponding to this

        MoveInfoAPtr moveInfoAPtr = getShuffleMoveInfo( currentTransform, targetTransform );

        moveInfos_.erase( moveInfos_.begin(), moveInfos_.end() );
        moveInfos_.push_back( *moveInfoAPtr );

        sendMotionToPhysicalMachine();

        // By clearing move infos now we avoid any possibility that a
        // later move might interfere or be interfered with.
        moveInfos_.erase( moveInfos_.begin(), moveInfos_.end() );

        shuffleEndTime_ = moveInfoAPtr->startTime() + moveInfoAPtr->totalTime();
    }

    POST( shuffling() );
    POST( moveInfos_.size() == 0 );
}

MachLogMachineMotionSequencer::MoveInfoAPtr
MachLogMachineMotionSequencer::getShuffleMoveInfo(
    const MexTransform3d& sourceTransform,
    const MexTransform3d& targetTransform )
{
    LOG_ENTER( "getShuffleMoveInfo" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    MachPhysMachine::ProfilePtr profilePtr(
      _NEW( MachPhysPlanetSurface::Profile ) );

    profilePtr->push_back( sourceTransform );
    profilePtr->push_back( targetTransform );

    //  Convert into the machine's parent's reference frame

    MexTransform3d inverseParentGlobal;
    pPhysMachine_->pParent()->globalTransform().invert( &inverseParentGlobal );

    for( MachPhysPlanetSurface::Profile::iterator i = profilePtr->begin();
      i != profilePtr->end(); ++i )
    {
        (*i).preTransform( inverseParentGlobal );
    }

    MoveInfoAPtr moveInfoAPtr;

    //  This is a cheat because MachPhysMachine does not have a "turn info" method.
    //  TBD: Fix this

    const MATHEX_SCALAR maxSpeed = 10.0;
    const MATHEX_SCALAR startSpeed = 0.0;

    moveInfoAPtr = pPhysMachine_->moveInfo(
        profilePtr,
        SimManager::instance().currentTime(),
        startSpeed, maxSpeed, MachPhys::AT_REST );

    ASSERT( moveInfoAPtr->needsToTurn(), "" );

    return moveInfoAPtr;
}

PhysRelativeTime MachLogMachineMotionSequencer::sendMotionToPhysicalMachine()
{
    LOG_ENTER( "sendMotionToPhysicalMachine" );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    //  Combine the multiple move infos into one.

    const uint nToReserve = 64;
    MachPhysMachineMoveInfo::TransformsPtr transformsPtr( _NEW( MachPhysMachineMoveInfo::Transforms() ) );
    transformsPtr->reserve( nToReserve );
    MachPhysMachineMoveInfo::RampAccelerationsPtr rampAccelerationsPtr( _NEW( MachPhysMachineMoveInfo::RampAccelerations() ) );
    rampAccelerationsPtr->reserve( nToReserve );

    LOG_STREAM( "Combining " << moveInfos_.size() << " move infos into one" << std::endl );

    for( size_t i = 0; i < moveInfos_.size(); ++i )
    {
        const MachPhysMachineMoveInfo& info( moveInfos_[ i ] );

        LOG_STREAM( info );

        const MachPhysMachineMoveInfo::Transforms& transforms = info.transforms();
        const MachPhysMachineMoveInfo::RampAccelerations& rampAccelerations = info.moveProfiles();

        MachPhysMachineMoveInfo::Transforms::const_iterator beginTx = transforms.begin();
        MachPhysMachineMoveInfo::Transforms::const_iterator endTx = transforms.end();

        MachPhysMachineMoveInfo::RampAccelerations::const_iterator beginRA = rampAccelerations.begin();
        MachPhysMachineMoveInfo::RampAccelerations::const_iterator endRA = rampAccelerations.end();

        const bool firstInfo = ( i == 0 );
        const bool lastInfo = ( i == moveInfos_.size() - 1 );

        if( not firstInfo and info.needsToTurn() )
        {
            ++beginTx;
            ++beginRA;
        }

        if( not lastInfo )
            --endTx;

        transformsPtr->insert( transformsPtr->end(), beginTx, endTx );
        rampAccelerationsPtr->insert( rampAccelerationsPtr->end(), beginRA, endRA );
    }

    const PhysAbsoluteTime startTime = moveInfos_[ 0 ].startTime();

    MachPhysMachineMoveInfo info( transformsPtr, rampAccelerationsPtr, startTime );

    LOG_INSPECT( startTime );

    LOG_STREAM( "Resultant move info:" << std::endl << info );

    if( shuffling() )
        stopShuffling();

    //Queue the motion.
    pPhysMachine_->move( info );

	if( MachLogNetwork::instance().isNetworkGame() )
    {
        MachPhysMachineMoveInfo::Transforms transforms;

        for( size_t i = 0; i < moveInfos_.size(); ++i )
        {
            transforms.push_back( pPhysMachine_->pParent()->globalTransform() );
            transforms.back().transform( moveInfos_[ i ].transforms().front() );
        }

        transforms.push_back( pPhysMachine_->pParent()->globalTransform() );
        transforms.back().transform( moveInfos_.back().transforms().back() );

    	MachLogNetwork::instance().messageBroker().sendMachineMoveMessage( pLogMobile_->id(), capSpeed(), moveInfos_, transforms );
    }

    return info.totalTime();
}

bool MachLogMachineMotionSequencer::anyMotionChunkStationary(
    const PhysConfigSpace2d::ChunkIntersectionsNoTime& chunkIntersections,
    PhysConfigSpace2d::ChunkIntersectionDataNoTime* pCollisionData ) const
{
    LOG_ENTER( "anyMotionChunkStationary" );

    bool result = false;

    for( size_t i = 0; i != chunkIntersections.size() and not result; ++i )
    {
        const PhysMotionChunk& chunk =
          currentConfigSpace().motionChunk(
          chunkIntersections[ i ].collisionChunkId() );

        //  Check that the chunk intersects the height of this machine

        if( chunk.minHeight() < MexDouble( maxHeight() ) and chunk.maxHeight() > MexDouble( minHeight() ) )
        {
            //  We define a stationary chunk as one that isn't moving very far
            const MATHEX_SCALAR sqrStationaryDistance = 0.1 * 0.1;

            if( chunk.startPoint().sqrEuclidianDistance( chunk.endPoint() ) < sqrStationaryDistance )
            {
                result = true;
                *pCollisionData = chunkIntersections[ i ];
            }
        }
    }

    LOG_INSPECT( result );

    return result;
}

bool MachLogMachineMotionSequencer::isPlayerControlled() const
{
    MachPhys::Race race = logMobile().race();

    const MachLogController& controller = MachLogRaces::instance().controller( race );

    return controller.type() == MachLogController::PLAYER_CONTROLLER;
}

// static
MATHEX_SCALAR MachLogMachineMotionSequencer::speedProportion()
{
    MATHEX_SCALAR result = 0.8;

    static bool first = true;

    if( first )
    {
        const char* pSpeed = getenv( "CB_SPEED_PROPORTION" );

        if( pSpeed )
            result = atof( pSpeed );

        first = false;
    }

    POST( result > 0.0 );
    POST( result < 1.0 );

    return result;
}

MachPhys::FinalState MachLogMachineMotionSequencer::finalStateToAimFor(
    const MexPoint3d& startPosition,
    const Point2ds& path,
    size_t pathPointIndex,
    MachPhys::FinalState desiredFinalState )
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_ENTER( "finalStateToAimFor" );

    //  By default we want to try and keep moving as much as possible
    MachPhys::FinalState finalState = MachPhys::IN_MOTION;

    //  If we are at the limit of motion chunks that the network can handle
    //  in a single go we have to come to rest.

    if( motionChunkIds_.size() + 1 == MachLogMessageBroker::maximumMotionChunks() )
        finalState = MachPhys::AT_REST;

	//	If we are dealing with the last point on the path we must try and
	//  honour the caller's desired final state.

	if( pathPointIndex + 1 == path.size() )
        finalState = desiredFinalState;
    else
    {
        //  Come to rest if the angle between this movement and the next one is too great

        MexVec2 thisMovement;

        if( pathPointIndex == 0 )
            thisMovement = MexVec2( startPosition, path[ pathPointIndex ] );
        else
            thisMovement = MexVec2( path[ pathPointIndex - 1 ], path[ pathPointIndex ] );

        const MexVec2 nextMovement( path[ pathPointIndex ], path[ pathPointIndex + 1 ] );

        const MexDegrees angle = thisMovement.angleBetween( nextMovement );

        const MexDegrees handbrakeTurnLimit = 45.0;
        if( fabs( angle.asScalar() ) > handbrakeTurnLimit.asScalar() )
            finalState = MachPhys::AT_REST;

        LOG_INSPECT( angle.asScalar() );
    }

    LOG_INSPECT( finalState );

    return finalState;
}

void MachLogMachineMotionSequencer::getIntersectingIds(
    PhysConfigSpace2d::ObjectIds* pIntersectingIds ) const
{
    //  This function is partly for defensive purposes and partly
    //  to provide extra debug checking.
    //  A machine should always be intersecting with its own motion
    //  chunk and never be intersecting with any other motion chunks.
    //  This is not always true in the network game, but should be
    //  true for the single player game. If a machine does end up
    //  intersecting with another motion chunk due to a bug, it can
    //  lock up that machine's movement - any attempted movement fails
    //  because of an intersection with a motion chunk. This function
    //  allows this top be overcome by making a list of the motion
    //  chunks a machine is intersecting with and allowing them to
    //  be ignored.

	PRE( pIntersectingIds->size() == 0 );

	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    LOG_ENTER( "getIntersectingIds" );

	//	Construct a dummy motion chunk corresponding to our current position
    const PhysAbsoluteTime timeNow = SimManager::instance().currentTime();
    const PhysMotionChunk motionChunk
    (
        currentLocation(),
        currentLocation(),
        useClearance_,
        PhysRampAcceleration( 0.0, 400.0, -400.0, 0.01, 0.005, 0.005 ),
        PhysAbsoluteTime( timeNow - 0.01 ),
        PhysRelativeTime( 0.0 ),
        minHeight(),
        maxHeight()
    );

	LOG_INSPECT( currentLocation() );
	LOG_INSPECT( timeNow );

	PhysConfigSpace2d::ChunkIntersections	chunkIntersections;
	chunkIntersections.reserve( 3 );

    pConfigSpace_->findIntersections( motionChunk, ObjectId::invalidId(),
      &chunkIntersections );

	for( size_t i = 0; i < chunkIntersections.size(); ++i )
	{
		pIntersectingIds->push_back( chunkIntersections[ i ].collisionObjectId_ );
	}

	//	This is a bit of a hack, however it is here in case the findIntersections method does not manage to
	pIntersectingIds->push_back( pLogMobile_->id() ); // TODO hack

    ASSERT_INFO( pLogMobile_->id() );

    LOG_INSPECT( *pIntersectingIds );

	//	This is purely useful for debugging
	#ifndef PRODUCTION

		for( size_t i = 0; i < pIntersectingIds->size(); ++i )
		{
			const MachActor& actor = MachLogRaces::instance().actor( (*pIntersectingIds)[ i ].asScalar() );

			LOG_STREAM( (*pIntersectingIds)[ i ] << "  " );
			LOG_STREAM( actor.globalTransform().position() );
			LOG_STREAM( "   " );
			const MexPoint2d p1 = actor.globalTransform().position();
			const MexPoint2d p2 = currentLocation();
			LOG_STREAM( p1.euclidianDistance( p2 ) );
			LOG_STREAM( std::endl );
		}

		if( pIntersectingIds->size() > 1 )
		{
			LOG_STREAM( "Machine is intersecting with other machines" << std::endl );
		}

	#endif
}

bool MachLogMachineMotionSequencer::machineHasMotionChunk() const
{
	CB_MachLogMachineMotionSequencerData_DEPIMPL();

    //  A machine should always have a motion chunk associated with it.
    //  This is effectively an invariant.

    PhysConfigSpace2d::ObjectIds intersectingIds;

    getIntersectingIds( &intersectingIds );

    const bool hasChunk =
      find( intersectingIds.begin(), intersectingIds.end(), pLogMobile_->id() ) != intersectingIds.end();

    return hasChunk;
}

/* End MCMOTSE2.CPP *************************************************/

