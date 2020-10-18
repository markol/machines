/*
 * M C M O T S E Q . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogMachineMotionSequencer

    This class controls the motion of a machine around the surface of the planet.
    Each MachLogMachine HAS-A MachLogMachineMotionSequencer.

    Client responsibilities:
    1) Supply the desired destination location for the machine
    2) Call the update() method frequently

    Responsibilities:
    1) Find a collision free path from current position to the destination position;
    2) Invoke atomic movement animations in the physical model;
    3) Register current position and upcoming motions with the planet's configuration
       space as temporary polygon obstacles;
    4) Monitor the path to ensure does not collide with static or dynamic obstacles
       using the configuration space;
    5) Remove any temporary obstacles from the configuration space once finished with.
    6) Cause the physical model to move along the path
*/

#ifndef _MACHLOG_MCMOTSEQ_HPP
#define _MACHLOG_MCMOTSEQ_HPP

#include "base/base.hpp"
#include "mathex/mathex.hpp"
#include "phys/phys.hpp"
#include "mathex/poly2d.hpp"
#include "mathex/point2d.hpp"
#include "phys/motchunk.hpp"
#include "phys/cspace2.hpp"
#include "machlog/machine.hpp"
#include "machphys/machine.hpp"

// Warnings about foward references of template classes
#ifndef CB_NOWARNINGS // Are Charybdis warnings turned on?
#ifndef _MATHEX_POINT2D_HPP
#pragma message( __FILE__ "(40): Warning : ctl_nb_vector< MexPoint2d > foward referenced" )
#endif

#ifndef _PHYS_MOTCHUNK_HPP
#pragma message( __FILE__ "(44): Warning : ctl_vector< PhysMotionChunk > foward referenced" )
#endif

#ifndef _MACHPHYS_MCMOVINF_HPP
#pragma message( __FILE__ "(48): Warning : auto_ptr< MachPhysMachineMoveInfo > foward referenced" )
#endif
#endif

//Forward declarations
class MachPhysMachine;
class MachPhysMachineMoveInfo;
class W4dDomain;
class MachLogMachineConvoy;
class MachLogMobile;
class MexTransform3d;
class MexPoint2d;
class MachLogGroupMoveInfo;
class MachLogCollisionInfo;

class MachLogMachineMotionSequencerData;

//Orthodox canonical (revoked)
class MachLogMachineMotionSequencer
{
public:
    //ctor. Responsible for navigating pPhysMachine around the world, with initial rest position
    //at initialPosition. Clearance for collision avoidance around origin is highClearance
    //for roomy situations, and lowClearance when space is tight.
    //The associated logical mobile is pLogMobile.
    MachLogMachineMotionSequencer( MachLogMobile* pLogMobile, MachPhysMachine* pPhysMachine,
                                   const MexPoint2d& initialPosition,
                                   MATHEX_SCALAR highClearance, MATHEX_SCALAR lowClearance );

    //dtor
    ~MachLogMachineMotionSequencer( void );

    //The associated logical mobile
    MachLogMobile& logMobile();
    const MachLogMobile& logMobile() const;

    //The clearance settings
    MATHEX_SCALAR highClearance() const;
    MATHEX_SCALAR lowClearance() const;

    //Set the new required destination. Supercedes all previous requests.
    //Returns suggested interval to next update() call.
    PhysRelativeTime destination( const MexPoint2d& newDestination );

    //Set the new required destination. Supercedes all previous requests.
    //Uses the supplied group move info to give better motion
    //Returns suggested interval to next update() call.
    PhysRelativeTime destination( const MexPoint2d& newDestination, const MachLogGroupMoveInfo& );

    //true iff has a destination point different from current position
    bool hasDestination() const;

    //The current destination
    const MexPoint2d& destination() const;
    //PRE( hasDestination() )

    //The point to which this sequencer is immediately trying to move (not the
    //ultimate destination).
    MexPoint2d currentTargetPoint() const;

    enum State
    {
        ARRIVED,    //  Exact destination point reached successfully
        FAILED,     //  Did not reach exact destination point
        PLANNING,   //  Movement still being planned
        WAITING,    //  Waiting for an obstacle to move out of the way
        MOVING,      //  Moving towards the destination point
        FIRST_PERSON // Under 1st person control
    };

    State   state() const;

    enum FailureReason
    {
        NO_ROUTE,               //  No route could be found
        DESTINATION_OCCUPIED
    };

    FailureReason   failureReason() const;
    //  PRE( state() == FAILED );

    //Come to rest without completing movement to current destination.
    //Returns interval to motion stopped.
    PhysRelativeTime stop();

    // Come to rest instantly but keep the current destination point
    // so the machine will start moving again immediately after the stop.
    //Returns interval to motion stopped.
    PhysRelativeTime stopKeepDestination();

    //Stop any machines about to collide with this one, keeping their destinations.
    //This machine is deemed to be about to move from its current position at startTime to
    //nextPosition at the current time.
    //returns true iff any other machine is actually stopped.
    bool preventImpendingCollisions( const MexTransform3d& nextPosition, const PhysAbsoluteTime& startTime );
    //PRE( is1stPersonControlled() );

    //set/get the 1st person controlled state
    void is1stPersonControlled( bool isIt );
    //PRE( not hasDestination() );

    bool is1stPersonControlled() const;

    //Take destination by following pLeadMobile around, attempting to maintain
    //a position offset relative to the local axis frame of that machine.
    //This instruction is cancelled by stop(), destination(), or the demise of
    //pLeadMobile. (The latter is handled automatically).
    void follow( MachLogMobile* pLeadMobile, const MexPoint2d& offset );

    //Notify any machines following this one that it has changed its current
    //destination.
    void notifyFollowers();

    //Be notified that the leader has changed its destination/position
    void leaderChangedDestination();
    //PRE( isFollowing() );

    //true if currently in motion
    bool isMoving() const;

    //true if not moving because current path is blocked, and is waiting for
    //the path to become clear.
    bool isWaiting() const;

    //True if following something
    bool isFollowing() const;

	//True iff has at least one follower
	bool hasFollowers() const;

	// populates pMachines, returns true if at least one following machine found.
	bool followingMachines( MachLogMachine::Machines* pMachines );

    //Update motion towards current destination, if any.
    //Returns time interval to next required update. Significantly longer delay may result
    //in interruption of motion animation.
    PhysRelativeTime update( const PhysRelativeTime& maxCPUTime );

    //Enable/disable ignoring the polygon in the config space with id id when pathfinding
    void isPolygonIgnored( PhysConfigSpace2d::PolygonId id, bool doIgnore );
    //PRE( id exists in the config space )

    //true if polygon with id id is being ignored for pathfinding
    bool isPolygonIgnored( PhysConfigSpace2d::PolygonId id ) const;

    //Current 2d location of the machine
    MexPoint2d currentLocation() const;

    //Change to moving around in pNewConfigSpace.
    //Also clears the list of polygons to ignore.
    void switchConfigSpace( PhysConfigSpace2d* pNewConfigSpace );
    //PRE( not hasDestination() )

	//note that is non-const as this config space can be used to find intersections etc.
	PhysConfigSpace2d& currentConfigSpace();
	const PhysConfigSpace2d& currentConfigSpace() const;

    //The domain to be used when moving in a non-planet config space.
    //Set when about to enter such a space. Automatically cleared when
    //call switchConfigSpace() back to the planet space.
    void useSpaceDomain( W4dDomain* pDomain );

    //True if a space domain is in use
    bool isUsingSpaceDomain() const;

    //The current space domain
    W4dDomain& spaceDomain();
    //PRE( isUsingSpaceDomain() )

    //set/get flag indicating whether the motion sequencer should automatically unlock
    //entrances when the machine moves off an entrance pad. This auto-unlock behaviour
    //is not desirable during enter/leave building operations.
    bool keepEntranceLocked() const;
    void keepEntranceLocked( bool keep );

    void CLASS_INVARIANT;
    friend ostream& operator <<( ostream& o, const MachLogMachineMotionSequencer& t );
    ostream& traceConvoyData( ostream& o ) const;

    typedef PhysConfigSpace2d::ObstacleFlags ObstacleFlags;

	//return resting/moving obstacle id for motionsequencer.
	//used by guns to determine visibility of target
	PhysConfigSpace2d::PolygonId currentObstacleId() const;

    //True if there is a suitable point to move to from fromPoint in order to
    //avoid some other machine in the way. If so return the point in pToPoint.
    bool findAvoidancePoint( const MexPoint2d& fromPoint, MexPoint2d* pToPoint );

    //Update the current domain for the physical machine and return it
	//overriding current position as you do it.
    W4dDomain* setDomain( const MexTransform3d& );

    //Clears any intersect relations with other domains
    void clearIntersectingDomains();

    //Add a temporary obstacle to the config space surrounding current position
    void addRestingObstacle();

	//if the network is active and the message comes back ok to add resting obstacle
	//then do so.
	void addRestingObstacleWithoutEcho( const MexTransform3d& );

    //Add a resting chunk at current position, without echoing round network.
    //Clears any existing resting chunk.
    void addRestingObstacleWithoutEcho();

    //Remove the temporary resting obstacle and resting chunk
    void clearRestingObstacleWithoutEcho();

    //Remove the temporary moving obstacle and moving chunk
	//will echo to network if it is for a local machine.
    void clearMovingObstacle();

	//function only really to be used by Network stuff - possibly move this private and add friendship - hmm.
	void changeStatus( int newStatus );

    //Cancel any existing motion
    void stopDeadWithoutEcho();

	bool addMotionChunks( const ctl_vector< PhysMotionChunk >& );
	void removeMotionChunksWithoutEcho();

    //Update the intersect relations a machine should have with neighbouring domains
	//promoted to public interface for networking calls.
    void setIntersectingDomains();

    void commandId( size_t newId );

    //True if this machine can sit at targetPoint, using current clearance without
    //intersecting any permanent obstacles, except those flagged to be ignored.
    bool targetPositionContainedInSpace( const MexPoint2d& targetPoint ) const;

    // True if this machine can sit at targetPoint in the supplied config space
    // using the clearance that is appropriate to the config space - i.e. high
    // clearance for the planet, low clearance for everything else - without
    // intersecting any permanent obstacles, except those flagged to be ignored.
    bool positionContainedInSpace( const MexPoint2d& targetPoint, const PhysConfigSpace2d& configSpace ) const;

    //  Set the flag which states whether this machine will move out of the
    //  way when asked.
    void allowMoveOutOfWay( bool allowed );
	bool allowMoveOutOfWay() const;

    //  Set the distance at which the path finding can decide it is close
    //  enough to the destination. The destination() method will reset this
    //  value to the default.
    void destinationTolerance( MATHEX_SCALAR tolerance );

    //  Set the priority to use for pathfinding. The destination method
    //  will reset this value to the default.
    void pathFindingPriority( PhysPathFindingPriority );

    //  Toggle the logging to a separate file
    void toggleLogging();

	MATHEX_SCALAR scalarSpeedNow() const;
	// PRE( not is1stPersonControlled() );

private:
    friend class MachLogMachineConvoy;
    friend class MachLogMotionSequencerEntryExit;
    friend class MachLog1stPersonMachineHandler;
    friend class MachLogMachineMotionSequencerData;

    // Operations deliberately revoked
    MachLogMachineMotionSequencer( const MachLogMachineMotionSequencer& );
    MachLogMachineMotionSequencer& operator =( const MachLogMachineMotionSequencer& );
    bool operator ==( const MachLogMachineMotionSequencer& );

    //Useful typedefs
    typedef PhysConfigSpace2d::PortalPoint PortalPoint;
    typedef PhysConfigSpace2d::PortalPoints PortalPoints;
    typedef PhysConfigSpace2d::PolygonId PolygonId;
    typedef PhysConfigSpace2d::MotionChunkId MotionChunkId;
    typedef PhysConfigSpace2d::ObjectId ObjectId;

    typedef ctl_nb_vector< MexPoint2d > Point2ds;
    typedef MachPhysMachine::OrientationInfo OrientationInfo;

    //Enumeration indicating current activity
    enum InternalState
    {
        INTERNAL_RESTING, //At current destination point
        INTERNAL_WANT_DOMAIN_PATH, //Have a destination, but no domain path
        INTERNAL_PLANNING_DOMAIN_PATH, //Executing a domain findPath in the config space
        INTERNAL_WANT_PATH, //Have a (possibly empty) domain path to current destination,
                   //but need a path from current point to next protal point or destination
        INTERNAL_PLANNING, //Executing a findPath in the planet config space to find a route
        INTERNAL_MOVING, //Moving towards current point
        INTERNAL_WAITING, //Waiting for an obstruction to clear
		INTERNAL_WAITING_FOR_NETWORK_CONFIRM,	// can't do anything if in this status as we are waiting for the
										//host to respond to our request.
        INTERNAL_1ST_PERSON //Under 1st person control
    };
    friend ostream& operator <<( ostream& o, const InternalState& t );

    //Timing constant values
    enum
    {
        ADVANCE_UPDATE_INTERVAL = 2, //Number of seconds before end of current motion
                                    //at which we want to try and queue motion for next chunk
        WAITING_TIMEOUT = 5 //How long we sit in wait state before trying to move round the
                            //obstacle
    };

    //true if in a convoy
    bool isInConvoy() const;

    //The number of machines following this machine in convoy
    size_t nFollowers() const;

    //true iff this sequencer is directly or indiretcly following the machine with
    //sequencer pRhsSequencer
    bool isFollowing( MachLogMachineMotionSequencer* pRhsSequencer ) const;

    //The convoy of which this machine is a member
    MachLogMachineConvoy& convoy();
    //PRE( isInConvoy() )

    //Stop following any current convoy leader
    void stopFollowing();

    //Stop any followers from following this machine
    void stopLeading();

    //  Stop any machines that are due to collide with
    //  this machine's current position
    void stopMachinesOnCollisionCourse();

    //Add this to pConvoy
    void addToConvoy( MachLogMachineConvoy* pConvoy );

    //Remove this from any convoy of which it is a member, deleting the convoy if
    //this is the last member
    void removeFromConvoy();

    //Move all the machines in pConvoy into teh convoy of this machine
    void mergeConvoys( MachLogMachineConvoy* pConvoy );
    //PRE( pConvoy != NULL );
    //PRE( pConvoy_ != NULL );

    //Create new convoys for all the machines in pOldConvoy, splitting them into disjoint
    //convoys as necessary. Then deletes the pOldConvoy.
    static void updateConvoy( MachLogMachineConvoy* pOldConvoy );
    //PRE( pOldConvoy != NULL );

    //Update destinationPoint_ according to the item being followed
    void updateFollowDestination();

    //Request which can be issued by another machine currently at askerStartPoint, and
    //trying to move to askerEndPoint. That machine has clearance askerClearance.
    //Returns zero if can't find a place to move to, or the time required to complete
    //the move otherwise.
    PhysRelativeTime moveOutOfWay
    (
        const MexPoint2d& askerStartPoint, const MexPoint2d& askerEndPoint,
        MATHEX_SCALAR askerClearance
    );

    //True if the clearance surrounding the line joining fromPoint to toPoint is
    //clear in the config space. Client must disable any resting/moving polygon
    //before calling to avoid a clash.
    bool canMoveTo( const MexPoint2d fromPoint, const MexPoint2d toPoint );

    MachPhysMachine& physMachine();
	const MachPhysMachine& physMachine() const;

    //Establish a domain findPath algorithm in the config space from pathStart
    //to pathDestination
    void createDomainFindPath( const MexPoint2d& pathStart, const MexPoint2d& pathDestination );

    //Establish a findPath algorithm in the config space from current location
    //to next portal or destination point.
    void createFindPath();

    //Compute a point on the next domain portal as defined by domainPath_
    void getOnPortalPoint();

    //  Calculate the portal point to use if we are doing a group move
    void calculateGroupMovePortalPoint();

    //Queue animation to move to next path point, returning the next update time.
    //Enters moving or waiting state, depending on whether the motion can be queued.
    PhysRelativeTime initiateMove();

    //Does the work of actually queuing the motion from fromPoint toPoint which has
    //previously had profiles etc computed in pData_->motionSegments_.
    PhysRelativeTime setupMotion();

    //Try and find a closer point to which we can move after failed to register motion chunk
    //moving from fromPoint to toPoint using motion data info, due to a potential collision at time
    //collideTime.
    //True if successful, in which case return the new point in pNewPoint
    bool tryNearerPoint
    (
        const MexPoint2d& fromPoint, const MexPoint2d& toPoint,
        const MexPoint2d& collisionPoint, MexPoint2d* pNewPoint
    );

    //A maximum appropriate speed, of use when in a convoy to limit the speed to an
    //appropriate value to keep the convoy together
    MATHEX_SCALAR capSpeed() const;

    //true if the object identified by collisionObjectId has a motion sequencer in RESTING state
    bool objectIsResting( const ObjectId& collisionObjectId );

    //True iff the object identified by collisionObjectId is in the same convoy as this
    bool objectInSameConvoy( const ObjectId& collisionObjectId );

    //Clear any existing stored paths
    void clearOldPaths();

    //Checks current position and advances points/portals done accordingly
    void advancePathProgress();

    //Updates to try and queue motion to finish current section from rest
    PhysRelativeTime updateWhileStopped();

    //Updates to try and queue motion to finish current section whilst moving
    PhysRelativeTime updateWhileMoving();

    //Updates while performing a path find
    PhysRelativeTime updateWhilePlanning( const PhysRelativeTime& maxCPUTime );

    //Tidy up current queued motion plans and reserved chunks so we stop as soon
    //as we can. Return time til we stop.
    PhysRelativeTime stopRequestWhileMoving();

    //Cancel any existing motion
    void stopDead();
    //PRE( internalState_ == MOVING );
    //POST( internalState_ == WAITING );

    //Cancel any existing motion
    void doStopDead();
    //PRE( internalState_ == MOVING );
    //POST( internalState_ == WAITING );

    //Remove the temporary resting obstacle and resting chunk
    void clearRestingObstacle();

    //Remove the temporary resting obstacle and resting chunk
    void doClearRestingObstacle();

    //Update the current domain for the physical machine and return it
    W4dDomain* setDomain();

    //Update the current domain for the physical machine and return it
	//this will not be echoed to the network
    W4dDomain* setDomainWithoutEcho();

    //Export various states
    InternalState internalState() const;

    size_t commandId() const;
    bool commandIdSet() const;

    //  Return true iff the given object id has the same command id as we do
    bool objectHasSameCommandId( ObjectId collisionObjectId );

    //If not keeping locked entrances, unlock any entrance we have moved off
    void autoUnlockAnyLockedEntrance();

    //  Return the obstacle flags for the mobile object this machine corresponds to
    ObstacleFlags obstacleFlags() const;

	// ignore polygons for path finding
	void ignorePolygons( bool setIgnore ) const;

    //  Try moving the machine sideways. return the number of motion
    //  chunks that the function managed to reserve.
    size_t tryMoveSideways();

    MATHEX_SCALAR sqrDestinationTolerance() const;

    //  Reserve as many chunks as possible between our current path points
    //  Returns the number of motion chunks that could be reserved. If the
    //  number of chunks that could be reserved is zero, the id of the
    //  object that caused the collision is returned in pCollisionObjectId.
    size_t reserveMotionChunks( ObjectId* pCollisionObjectId );

    typedef std::auto_ptr< MachPhysMachineMoveInfo > MoveInfoAPtr;

    enum ObstacleCheck
    {
        CHECK_STATIC_OBSTACLES,
        DONT_CHECK_STATIC_OBSTACLES
    };

    enum MoveResult
    {
        COLLIDED_WITH_MACHINE,
        COLLIDED_WITH_STATIC_OBSTACLE,
	    SUCCESS
    };

    friend ostream& operator <<( ostream& o, const MoveResult& t );

    //  Try and reserve a single motion chunk. If the reserve is successful the config space,
    //  movingChunkIds and moveInfos will all be updated. If the reserve is unsuccessful details
    //  of the collision which caused things to fail are returned in pCollisionObjectId,
    //  pCollideTime and pInfo.
    MoveResult reserveChunk(
        const MexTransform3d& fromTransform,
        OrientationInfo,
        const MexPoint3d& toPoint,
        const PhysAbsoluteTime& startTime,
        MATHEX_SCALAR startSpeed,
        MachPhys::FinalState finalState,
        ObstacleCheck,
        const PhysConfigSpace2d::ObjectIds& ignoreIds,
        MachLogCollisionInfo* pCollisionInfo,
        MoveInfoAPtr* pMoveInfoAPtr );

    //  Try and swerve around an object that is blocking our way.
    //  Update the movingChunkIds and moveInfos if successful.
    bool trySwerveMove
    (
        const MexTransform3d& startTransform,
        OrientationInfo,
        const MexPoint2d& endPoint,
        PhysAbsoluteTime startTime,
        MATHEX_SCALAR startSpeed,
        ObjectId collisionObjectId,
        const MexPoint2d& collisionPoint,
        MachPhys::FinalState desiredFinalState,
        const PhysConfigSpace2d::ObjectIds& ignoreIds,
        MachPhys::FinalState* pActualFinalState,
        MoveResult* pMoveResult
    );

    //  This function might be recursively called from trySwerveMove
    //  Returns the number of path points sucessfully reached
    size_t reservePath
    (
        const MexTransform3d& startTransform,
        OrientationInfo,
        const Point2ds& path,
        PhysAbsoluteTime startTime,
        MATHEX_SCALAR startSpeed,
        MachPhys::FinalState desiredFinalState,
        ObstacleCheck,
        const PhysConfigSpace2d::ObjectIds& ignoreIds,
        MachPhys::FinalState* pActualFinalState,
        MachLogCollisionInfo* pCollisionInfo,
        MoveInfoAPtr* pMoveInfoAPtr,
        MoveResult* pMoveResult
    );

    //  Attempt to reserve a chunk. Return true iff we succeed. Do not
    //  update any the the motion sequencer's data structures though,
    //  this job is left to reserveChunk.
    //  *pMotionChunkId is the id of any chunk that was reserved.
    bool doReserveChunk(
        MachPhysMachine::ProfilePtr profilePtr,
        const PhysConfigSpace2d::ChunkIntersectionsNoTime& chunkIntersections,
        const PhysAbsoluteTime& startTime,
        MATHEX_SCALAR startSpeed,
        MATHEX_SCALAR maxSpeed,
        MachPhys::FinalState finalState,
        const PhysConfigSpace2d::ObjectIds& ignoreIds,
        MotionChunkId* pMotionChunkId,
        MachLogCollisionInfo* pCollisionInfo,
        MoveInfoAPtr* pMoveInfoAPtr ) const;

    void forceMotionToRest(
        const PhysConfigSpace2d::ObjectIds& ignoreIds,
        ctl_vector< size_t >* pNChunks );

    //  Clear out motion chunks until we have as many
    //  motion chunks as specified.
    void setNChunks( size_t nChunks );
	// PRE( nChunks <= motionChunkIds_.size() );
	// POST( nChunks == motionChunkIds_.size() );

    //  Clear all of the data associated with the current path (note
    //  that this is _not_ the domain path)
    void clearPath();

    //  Clear all of the data associated with the current motion segments
    void removeMotionChunks();

    MachPhysMachine* pPhysMachine() const;
	MachLogMachineConvoy*& pConvoy() const;
	int& nFollowers();
	MachLogMachineMotionSequencer* pFollowSequencer() const;
	ostream& streamOut( ostream& o ) const;

    MATHEX_SCALAR minHeight() const;
    MATHEX_SCALAR maxHeight() const;

    // Try for a simple straight line move directly to a target. If we
    // can do it, start the move immediately.
    bool straightLineMove( const MexPoint3d& target, PhysRelativeTime* pInterval );
    PhysRelativeTime initiatePhysicalMotion();

    MachLogMachineMotionSequencerData* pImpl_;

    bool shuffling() const;
    void stopShuffling();
    void shuffle();
    MoveInfoAPtr getShuffleMoveInfo(
        const MexTransform3d& sourceTransform,
        const MexTransform3d& targetTransform );

    //  Return the time taken by the motion that has been sent
    PhysRelativeTime sendMotionToPhysicalMachine();

    void writePortalPoints() const;
    void writePathPoints() const;

    bool anyMotionChunkStationary(
        const PhysConfigSpace2d::ChunkIntersectionsNoTime& chunkIntersections,
        PhysConfigSpace2d::ChunkIntersectionDataNoTime* pCollisionData ) const;

    bool isPlayerControlled() const;

    //  Returns the proportion of full speed we should try if the full
    //  speed movement doesn't work
    static MATHEX_SCALAR speedProportion();
    // POST( result > 0.0 );
    // POST( result < 1.0 );

    //  Calculate whether we want to finish this part of the movement
    //  in motion or at rest
    MachPhys::FinalState finalStateToAimFor(
        const MexPoint3d& startPosition,
        const Point2ds& path,
        size_t pathPointIndex,
        MachPhys::FinalState desiredFinalState );

    //  Get the ids of those motion chunks that this machine is currently
    //  intersecting with.
    void getIntersectingIds( PhysConfigSpace2d::ObjectIds* pIntersectingIds ) const;

    //  Does a machine have a motion chunk that it is currently intersecting?
    bool machineHasMotionChunk() const;
};

#ifdef _INLINE
    #include "machlog/mcmotseq.ipp"
#endif


#endif

/* End MCMOTSEQ.HPP *************************************************/

