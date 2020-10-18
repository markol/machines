/*
 * S E Q D A T A . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogMachineMotionSequencerData

    Implementation class for the motion sequencer
*/

#ifndef _MACHLOG_SEQDATA_HPP
#define _MACHLOG_SEQDATA_HPP

#include <string>

#include "base/base.hpp"
#include "base/logbuff.hpp"
#include "phys/motchunk.hpp"
#include "ctl/utility.hpp"
#include "ctl/pvector.hpp"
#include "phys/cspace2.hpp"
#include "mathex/point2d.hpp"
#include "world4d/domain.hpp"
#include "machphys/mcmovinf.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/grpmvinf.hpp"
#include "utility/indent.hpp"
#include "system/pathname.hpp"

class MachLogMobile;
class MachLogMachineConvoy;
class MachPhysMachine;

class MachLogMachineMotionSequencerData
// Canonical form revoked
{
public:
    MachLogMachineMotionSequencerData( int id );
    ~MachLogMachineMotionSequencerData( void );

    void CLASS_INVARIANT;

    void commandId( size_t id );
    size_t commandId() const;
    bool commandIdSet() const;
    void clearCommandId();
    // POST( not commandIdSet() );

    //set/get flag indicating whether the motion sequencer should automatically unlock
    //entrances when the machine moves off an entrance pad. This auto-unlock behaviour
    //is not desirable during enter/leave building operations.
    bool keepEntranceLocked() const;
    void keepEntranceLocked( bool keep );

    void indent( int );

    //  Calculate the offset to use as the machine is moving as part of a group
    void calculateOffset( const MexVec2& directionOfTravel );

#ifndef PRODUCTION
    void reasonForStateChange( const string& );
#endif

    void toggleLogging();

private:
    friend ostream& operator <<( ostream& o, const MachLogMachineMotionSequencerData& t );

    friend class MachLogMachineMotionSequencer;
    friend class MachLogMotionSequencerEntryExit;

    //  This is a friend purely to allow template instantiation
    friend void ti1();

    MachLogMachineMotionSequencerData( const MachLogMachineMotionSequencerData& );
    MachLogMachineMotionSequencerData& operator =( const MachLogMachineMotionSequencerData& );

    static  MATHEX_SCALAR defaultDestinationTolerance() { return 0.01; }

    typedef PhysConfigSpace2d::MotionChunkId MotionChunkId;

	// CTL_PAIR( MotionSegment, MotionChunkId, motionChunkId, MachPhysMachineMoveInfo, moveInfo );
	// CTL_PAIR( MotionSegment, MotionChunkId, motionChunkId, MachPhysMachineMoveInfo, moveInfo );

	// ctl_vector< MotionSegment > motionSegments_;

    //  These values used to be combined ib MotionSegment. They have been separated out
    //  because the vector of MachPhysMachineMoveInfo can be used directly,. In fact, it
    //  might not even be necessary to store the vector in here
    //  TBD: See if the vector< MachPhysMachineMoveInfo > can be removed from here
    //  and just have it as a local variable used in initiateMove and setupMotion

    size_t  									commandId_;
    bool 										keepEntranceLocked_; //If true, don't auto unlock entrances when move off pads
    ctl_vector< MotionChunkId > 				motionChunkIds_;
    ctl_vector< MachPhysMachineMoveInfo > 		moveInfos_;
    PhysConfigSpace2d* 							pConfigSpace_; 				//The config space to move around in
    W4dDomain* 									pSpaceDomain_; 				//Domain associated with the assigned space.
    MATHEX_SCALAR 								useClearance_; 				//One of high/lowClearance depending on size of space
    MATHEX_SCALAR 								highClearance_;				//The clearance used for navigation when plenty of free space
    MATHEX_SCALAR 								lowClearance_; 				//The clearance used for navigation when space is tight - ie the minimum to avoid collisions
    MachLogMobile* 								pLogMobile_;   				//The logical mobile
    MachLogMachineConvoy* 						pConvoy_; 	   				//The convoy of which this machine is a member, if any (NULL if not)
    MachLogMachineMotionSequencer* 				pFollowSequencer_; 			//Pointer to sequencer of machine to be followed. Null if none
    MexPoint2d 									followOffset_; 				//Offset to try and maintain wrt to pFollowSequencer_
    PhysAbsoluteTime 							scheduledUpdateTime_; 		//Time at which the update method call is scheduled for
    int 										nFollowers_; 				//Number of machines currently following this machine
    MachPhysMachine* 							pPhysMachine_;				//The physical model machine.
    MexPoint2d 									destinationPoint_; 			//The current desired target location.
    MexPoint2d 									queuedDestination_;			//New destination supplied while not at rest
    MachLogMachineMotionSequencer::InternalState 	internalState_; 		//Current internal state
    bool 										motionSucceeded_; 			//true iff the motion did not reach its destination
    MachLogMachineMotionSequencer::FailureReason   	failureReason_; 		//Reason for movement failure
    PhysConfigSpace2d::DomainFindPathId 		domainFindPathId_;			//id of current domain findpath algorithm. Valid if internalState_ == PLANNING_DOMAIN_PATH
    MachLogMachineMotionSequencer::PortalPoints domainPath_; 				//Sequence of points on inter-domain portals forming the gross path navigation.
    size_t 										nPortalPointsDone_; 		//Number of portals in domainPath_ already reached
    MexPoint2d 									onPortalPoint_; 			//Stores computed on-portal point for next portal
    PhysConfigSpace2d::FindPathId 				findPathId_;				//id of current findpath algorithm. Valid if internalState_ == PLANNING
    MachLogMachineMotionSequencer::Point2ds		path_; 						//Current computed path to portal point or destination
    size_t 										nPathPointsDone_; 			//Number of points on path_ already reached
    MachLogMachineMotionSequencer::PolygonId 	restingObstacleId_;			//Id of polygon added to config space when at rest (including waiting)
    MotionChunkId 								restingChunkId_; 			//Id of short motion chunk covering
    PhysAbsoluteTime 							arrivalTime_; 				//Time of arrival at current point moving to
    MachLogMachineMotionSequencer::PolygonId 	blockingObstacleId_;		//Id of polygon we are waiting to clear
    PhysConfigSpace2d::PolygonIds 				ignoreObstacleIds_; 		//Collection of obstacle polygons to ignore while pathfinding
    bool 										hasQueuedDestination_  	:1; //true iff above contains a new destination
    bool 										stopAsSoonAsPossible_  	:1; //true if should abort current path and stop asap
    bool 										restingObstacleExists_ 	:1;	//true if an obstacle for the resting position has been added to the config space. If so its id is restingObstacleId_, and restingChunkId_ is also valid.
    bool 										restingChunkExists_ 	:1; //true if  restingChunkId_ is a motion chunk added to cover resting position.
    bool 										hasOnPortalPoint_ 		:1; //True iff onPortalPoint_ contains valid value. Implies path_ leads to a portal point.
    bool 										movedOffPath_	 		:1; //True if moved to a point off computed path. This is used to take evasive action when path is blocked.
    bool 										inPlanetSpace_ 			:1; //true when moving around using the external planet space
    bool 										ignoringPolygons_		:1; //true when th eignore polygon list have been disabled
    bool                                        allowMoveOutOfWay_;
    MATHEX_SCALAR                               destinationTolerance_;
    bool                                        logStreamOn_;
    UtlIndentOstream                            logStream_;
    int                                         indentLevel_;
    std::ofstream                                    str_;
    SysPathName                                 logFilePathName_;
	//Extra flags used in networking to remove extraneous traffic
	PhysAbsoluteTime							lastClearMovingChunksTimeMessage_;
	PhysAbsoluteTime							lastSetDomainTimeMessage_;
    MachLogGroupMoveInfo                        groupMoveInfo_;

    //  Store the machine's offset from the group it is moving with.
    //  The offset is stored as angle from the line along which the
    //  group is moving and a distance from the centre of the group.
    //  This make it easier to calculate where the machine will end
    //  up if the group has to turn a corner.
    bool                                        offsetValid_;
    MexDegrees                                  offsetAngle_;
    MATHEX_SCALAR                               offsetDistance_;

    // This is used to stop reservePath getting too recursive
    uint                                        swerveRecursionCount_;

    PhysPathFindingPriority                     pathFindingPriority_;

    PhysAbsoluteTime                            shuffleEndTime_;

#ifndef PRODUCTION
    string          reasonForStateChange_;
#endif
};


#ifndef PRODUCTION
#define	CB_MachLogMachineMotionSequencerData_DEPIMPL() \
    CB_DEPIMPL( size_t, commandId_ ); \
	CB_DEPIMPL( bool, keepEntranceLocked_ ); \
	CB_DEPIMPL( ctl_vector< MotionChunkId >, motionChunkIds_ ); \
	CB_DEPIMPL( ctl_vector< MachPhysMachineMoveInfo >, moveInfos_ ); \
	CB_DEPIMPL( PhysConfigSpace2d*, pConfigSpace_ ); \
	CB_DEPIMPL( W4dDomain*, pSpaceDomain_ ); \
	CB_DEPIMPL( MATHEX_SCALAR, useClearance_ ); \
	CB_DEPIMPL( MATHEX_SCALAR, highClearance_ ); \
	CB_DEPIMPL( MATHEX_SCALAR, lowClearance_ ); \
	CB_DEPIMPL( MachLogMobile*, pLogMobile_ ); \
	CB_DEPIMPL( MachLogMachineConvoy*, pConvoy_ ); \
	CB_DEPIMPL( MachLogMachineMotionSequencer*, pFollowSequencer_ ); \
	CB_DEPIMPL( MexPoint2d, followOffset_ ); \
	CB_DEPIMPL( PhysAbsoluteTime, scheduledUpdateTime_ ); \
	CB_DEPIMPL( int, nFollowers_ ); \
	CB_DEPIMPL( MachPhysMachine*, pPhysMachine_ ); \
	CB_DEPIMPL( MexPoint2d, destinationPoint_ ); \
	CB_DEPIMPL( MexPoint2d, queuedDestination_ ); \
	CB_DEPIMPL( InternalState, internalState_ ); \
	CB_DEPIMPL( bool, motionSucceeded_ ); \
	CB_DEPIMPL( FailureReason, failureReason_ ); \
	CB_DEPIMPL( PhysConfigSpace2d::DomainFindPathId, domainFindPathId_ ); \
	CB_DEPIMPL( PortalPoints, domainPath_ ); \
	CB_DEPIMPL( size_t, nPortalPointsDone_ ); \
	CB_DEPIMPL( MexPoint2d, onPortalPoint_ ); \
	CB_DEPIMPL( PhysConfigSpace2d::FindPathId, findPathId_ ); \
	CB_DEPIMPL( ctl_nb_vector< MexPoint2d >, path_ ); \
	CB_DEPIMPL( size_t, nPathPointsDone_ ); \
	CB_DEPIMPL( PolygonId, restingObstacleId_ ); \
	CB_DEPIMPL( MotionChunkId, restingChunkId_ ); \
	CB_DEPIMPL( PhysAbsoluteTime, arrivalTime_ ); \
	CB_DEPIMPL( PolygonId, blockingObstacleId_ ); \
	CB_DEPIMPL( PhysConfigSpace2d::PolygonIds, ignoreObstacleIds_ ); \
    CB_DEPIMPL( string, reasonForStateChange_ ); \
    CB_DEPIMPL( bool, allowMoveOutOfWay_ ); \
    CB_DEPIMPL( MATHEX_SCALAR, destinationTolerance_ ); \
	CB_DEPIMPL( PhysAbsoluteTime, lastClearMovingChunksTimeMessage_ ); \
	CB_DEPIMPL( PhysAbsoluteTime, lastSetDomainTimeMessage_); \
    CB_DEPIMPL( uint, swerveRecursionCount_ ); \
    CB_DEPIMPL( PhysAbsoluteTime, shuffleEndTime_ );
#else
#define	CB_MachLogMachineMotionSequencerData_DEPIMPL() \
    CB_DEPIMPL( size_t, commandId_ ); \
	CB_DEPIMPL( bool, keepEntranceLocked_ ); \
	CB_DEPIMPL( ctl_vector< MotionChunkId >, motionChunkIds_ ); \
	CB_DEPIMPL( ctl_vector< MachPhysMachineMoveInfo >, moveInfos_ ); \
	CB_DEPIMPL( PhysConfigSpace2d*, pConfigSpace_ ); \
	CB_DEPIMPL( W4dDomain*, pSpaceDomain_ ); \
	CB_DEPIMPL( MATHEX_SCALAR, useClearance_ ); \
	CB_DEPIMPL( MATHEX_SCALAR, highClearance_ ); \
	CB_DEPIMPL( MATHEX_SCALAR, lowClearance_ ); \
	CB_DEPIMPL( MachLogMobile*, pLogMobile_ ); \
	CB_DEPIMPL( MachLogMachineConvoy*, pConvoy_ ); \
	CB_DEPIMPL( MachLogMachineMotionSequencer*, pFollowSequencer_ ); \
	CB_DEPIMPL( MexPoint2d, followOffset_ ); \
	CB_DEPIMPL( PhysAbsoluteTime, scheduledUpdateTime_ ); \
	CB_DEPIMPL( int, nFollowers_ ); \
	CB_DEPIMPL( MachPhysMachine*, pPhysMachine_ ); \
	CB_DEPIMPL( MexPoint2d, destinationPoint_ ); \
	CB_DEPIMPL( MexPoint2d, queuedDestination_ ); \
	CB_DEPIMPL( InternalState, internalState_ ); \
	CB_DEPIMPL( bool, motionSucceeded_ ); \
	CB_DEPIMPL( FailureReason, failureReason_ ); \
	CB_DEPIMPL( PhysConfigSpace2d::DomainFindPathId, domainFindPathId_ ); \
	CB_DEPIMPL( PortalPoints, domainPath_ ); \
	CB_DEPIMPL( size_t, nPortalPointsDone_ ); \
	CB_DEPIMPL( MexPoint2d, onPortalPoint_ ); \
	CB_DEPIMPL( PhysConfigSpace2d::FindPathId, findPathId_ ); \
	CB_DEPIMPL( ctl_nb_vector< MexPoint2d >, path_ ); \
	CB_DEPIMPL( size_t, nPathPointsDone_ ); \
	CB_DEPIMPL( PolygonId, restingObstacleId_ ); \
	CB_DEPIMPL( MotionChunkId, restingChunkId_ ); \
	CB_DEPIMPL( PhysAbsoluteTime, arrivalTime_ ); \
	CB_DEPIMPL( PolygonId, blockingObstacleId_ ); \
	CB_DEPIMPL( PhysConfigSpace2d::PolygonIds, ignoreObstacleIds_ ); \
    CB_DEPIMPL( bool, allowMoveOutOfWay_ ); \
    CB_DEPIMPL( MATHEX_SCALAR, destinationTolerance_ ); \
	CB_DEPIMPL( PhysAbsoluteTime, lastClearMovingChunksTimeMessage_ ); \
	CB_DEPIMPL( PhysAbsoluteTime, lastSetDomainTimeMessage_); \
    CB_DEPIMPL( uint, swerveRecursionCount_ ); \
    CB_DEPIMPL( PhysAbsoluteTime, shuffleEndTime_ );
#endif




#endif

/* End SEQDATA.HPP **************************************************/
