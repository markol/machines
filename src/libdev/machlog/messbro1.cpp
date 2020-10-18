#include "mathex/angle.hpp"
#include "mathex/point2d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/poly2d.hpp"

#include "phys/rampacce.hpp"
#include "phys/crampacc.hpp"
#include "phys/cmotchun.hpp"
#include "phys/motchunk.hpp"

#include "network/message.hpp"
#include "network/session.hpp"
#include "network/nodeuid.hpp"
#include "network/netnet.hpp"

#include "sim/manager.hpp"

#include "machlog/network.hpp"
#include "machlog/messbrok.hpp"
#include "machlog/internal/messages.hpp"
#include "machlog/actmaker.hpp"

#include "machlog/races.hpp"

#include "machphys/machine.hpp"
#include "machlog/machine.hpp"

#include "machphys/consdata.hpp"
#include "machlog/constron.hpp"
#include "machphys/constron.hpp"

#include "machlog/mcmotseq.hpp"
#include "machlog/missilem.hpp"
#include "machlog/planet.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/p1remman.hpp"
#include "machlog/stats.hpp"

#include "machphys/mcmovinf.hpp"

void MachLogMessageBroker::sendMachineMoveMessage
(
	UtlId whichId, const MATHEX_SCALAR& topSpeed, const ctl_vector< MachPhysMachineMoveInfo >& infos,
	const ctl_vector< MexTransform3d >& startEndPositions//, const MexPoint3d& endPosition
)
{
	PRE( startEndPositions.size() == ( infos.size() + 1 ) );
	PRE( infos.size() <= maximumMotionChunks() );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendMachineMoveMessage (" << whichId << ")" << std::endl );
	if( infos.size() == 1 and
		startEndPositions[0].position() == startEndPositions[1].position() )
	{
//		NETWORK_STREAM(" move is a shuffle - transmit a turn instead\n" );
//		NETWORK_INDENT( 2 );
//		NETWORK_STREAM("StartEndPositions[0].position " << startEndPositions[0].position() << std::endl );
//		NETWORK_STREAM("StartEndPositions[1].position " << startEndPositions[1].position() << std::endl );
		sendMachineTurnMessage( whichId, infos[0].turnAngle() );
//		NETWORK_INDENT( -2 );
	}
	else
	{
		MachLogNetMessage* pMessage = _NEW( MachLogNetMessage() );
		MachLogMachineMoveMessage* pMoveMessage = _REINTERPRET_CAST( MachLogMachineMoveMessage*, pMessage );
		pMoveMessage->whichId_ = whichId;
		pMoveMessage->header_.systemCode_ = 0;
		pMoveMessage->header_.messageCode_ = MACHINE_MOVE_CODE;
		pMoveMessage->header_.totalLength_ = sizeof( MachLogMachineMoveMessage ) - MachLogMachineMoveMessage::dataSize() + infos.size() * sizeof( MachLogSingleMoveInfo );
		pMoveMessage->topSpeed_ = topSpeed;
		pMoveMessage->nMoveInfos_ = infos.size();
		for( int i = 0; i < infos.size(); ++i )
		{
//DEBUG_STREAM( DIAG_NETWORK, WHERE_STR << std::endl );
//DEBUG_STREAM( DIAG_NETWORK, infos[ i ] );
//DEBUG_STREAM( DIAG_NETWORK, WHERE_STR << std::endl );
			MachLogSingleMoveInfo moveInfo;
			moveInfo.startTransform_ = startEndPositions[ i ];
			moveInfo.endPosition_ = startEndPositions[ i+1 ].position();
			moveInfo.startSpeed_ = infos[i].moveProfiles().front().startSpeed();
			moveInfo.startTime_ = infos[i].startTime();
			moveInfo.finalState_ = infos[i].finalState();
//	        NETWORK_STREAM("MachLogMachineMoveInfo " << moveInfo << std::endl);
			memcpy( (void*)(&pMoveMessage->data_[0] + i * sizeof(MachLogSingleMoveInfo) ), (void*)&moveInfo, sizeof(MachLogSingleMoveInfo) );

		}

//		DEBUG_STREAM( DIAG_NETWORK, "Move Message :\n" << *pMoveMessage << std::endl );
		doSend( pMessage );
	}
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendMachineMoveMessage DONE " << std::endl );
}


void MachLogMessageBroker::processMachineMoveMessage( NetMessage* pMessage )
{
	//MachLogMachineMoveMessage* pMoveMessage = _REINTERPRET_CAST( MachLogMachineMoveMessage*, &pMessage->body().body()[0] );
	MachLogMachineMoveMessage* pMoveMessage = _REINTERPRET_CAST( MachLogMachineMoveMessage*, _CONST_CAST(uint8*, &pMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processMachineMoveMessage for " << pMoveMessage->whichId_ << std::endl );
	DEBUG_STREAM( DIAG_NETWORK, *pMoveMessage << std::endl );
//	DEBUG_STREAM( DIAG_NETWORK, "Current Time " << SimManager::instance().currentTime() << std::endl );
	ASSERT( MachLogRaces::instance().actorExists( pMoveMessage->whichId_ ), "Actor does not exists" );
	if( MachLogRaces::instance().actorExists( pMoveMessage->whichId_ ) )
	{
		MachActor& actor = MachLogRaces::instance().actor( pMoveMessage->whichId_ );
		if( actor.objectIsMachine() )
		{
			MachPhysMachine* pPhysMachine = &actor.asMachine().physMachine();
			ctl_vector< MachPhysMachineMoveInfo > infos;
			for( int i = 0; i < pMoveMessage->nMoveInfos_; ++i )
			{
				MachLogSingleMoveInfo* pMoveInfo = _REINTERPRET_CAST( MachLogSingleMoveInfo*, (pMoveMessage->data_ + i * sizeof( MachLogSingleMoveInfo ) ) );
//NETWORK_STREAM( "MachLogSingleMoveInfo " << i << std::endl << *pMoveInfo << std::endl);

		        MachPhysMachine::ProfilePtr profilePtr =
		          pPhysMachine->profile(
		            pMoveInfo->startTransform_, MachPhysMachine::USE_ORIENTATION,
		            pMoveInfo->endPosition_, *MachLogPlanet::instance().surface() );

	            std::auto_ptr< MachPhysMachineMoveInfo > infoAPtr = pPhysMachine->moveInfo(
								                 profilePtr,
	    	                                     pMoveInfo->startTime_,
	                                             pMoveInfo->startSpeed_,
	                                             pMoveMessage->topSpeed_,
	    	                                     pMoveInfo->finalState_ );
//DEBUG_STREAM( DIAG_NETWORK, WHERE_STR << std::endl );
//DEBUG_STREAM( DIAG_NETWORK, *infoAPtr );
//DEBUG_STREAM( DIAG_NETWORK, WHERE_STR << std::endl );

	            const MachPhysMachineMoveInfo& info = *infoAPtr;
				infos.push_back( info );
			}
		    //  Combine the multiple move infos into one.

		    MachPhysMachineMoveInfo::TransformsPtr transformsPtr( _NEW( MachPhysMachineMoveInfo::Transforms() ) );
		    MachPhysMachineMoveInfo::RampAccelerationsPtr rampAccelerationsPtr( _NEW( MachPhysMachineMoveInfo::RampAccelerations() ) );

		    for( size_t i = 0; i < infos.size(); ++i )
		    {

	            const MachPhysMachineMoveInfo& info = infos[i];
		        const MachPhysMachineMoveInfo::Transforms& transforms = info.transforms();
		        const MachPhysMachineMoveInfo::RampAccelerations& rampAccelerations = info.moveProfiles();

		        MachPhysMachineMoveInfo::Transforms::const_iterator beginTx = transforms.begin();
		        MachPhysMachineMoveInfo::Transforms::const_iterator endTx = transforms.end();

		        MachPhysMachineMoveInfo::RampAccelerations::const_iterator beginRA = rampAccelerations.begin();
		        MachPhysMachineMoveInfo::RampAccelerations::const_iterator endRA = rampAccelerations.end();

		        const bool firstInfo = ( i == 0 );
		        const bool lastInfo = ( i == infos.size() - 1 );

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

		    const PhysAbsoluteTime startTime = infos[ 0 ].startTime();


		    MachPhysMachineMoveInfo info( transformsPtr, rampAccelerationsPtr, startTime );

//DEBUG_STREAM( DIAG_NETWORK, WHERE_STR << std::endl );
//DEBUG_STREAM( DIAG_NETWORK, info );
//DEBUG_STREAM( DIAG_NETWORK, WHERE_STR << std::endl );

    		MachLogMachine& mlm = actor.asMachine();
//    		mlm.motionSeq().setIntersectingDomains();
    		mlm.physMachine().move( info );
		}
	}
	if( pMoveMessage->nMoveInfos_ > maximumMotionChunks() )
		NetNetwork::instance().doAbort( string( ". move infos outside maximum allowed range\n" ) );
}

void MachLogMessageBroker::sendMachineSetDomainMessage( UtlId whichId, const MexTransform3d& transform )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendMachineSetDomainMessage (" << whichId << ")" << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogMachineSetDomainMessage* pMessage = _REINTERPRET_CAST( MachLogMachineSetDomainMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = MACHINE_SET_DOMAIN_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogMachineSetDomainMessage );
	pMessage->whichId_ = whichId;
	pMessage->transform_ = transform;
	doSend( pLogNetMessage );
//DEBUG_STREAM( DIAG_NETWORK, " transform " << transform << std::endl );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendMachineSetDomainMessage DONE " << std::endl );
}

void MachLogMessageBroker::processMachineSetDomainMessage( NetMessage* pNetMessage )
{
	//MachLogMachineSetDomainMessage* pMessage = _REINTERPRET_CAST( MachLogMachineSetDomainMessage*, &pNetMessage->body().body()[0] );
	MachLogMachineSetDomainMessage* pMessage = _REINTERPRET_CAST( MachLogMachineSetDomainMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processMachineSetDomainMessage " << pMessage->whichId_ << std::endl );
	ASSERT( MachLogRaces::instance().actorExists( pMessage->whichId_ ), "Actor does not exists" );
	if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
	{
//DEBUG_STREAM( DIAG_NETWORK, " transform " << pMessage->transform_ << std::endl );
		MachActor& actor = MachLogRaces::instance().actor( pMessage->whichId_ );
		if( actor.objectIsMachine() )
			actor.asMachine().motionSeq().setDomain( pMessage->transform_ );
	}
}

void MachLogMessageBroker::sendMachineTurnMessage( UtlId whichId, const MexRadians& turnAmount )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendMachineTurnMessage (" << whichId << ")" << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogMachineTurnMessage* pMessage = _REINTERPRET_CAST( MachLogMachineTurnMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = MACHINE_TURN_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogMachineTurnMessage );
	pMessage->whichId_ = whichId;
	pMessage->turnAmount_ = turnAmount;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendMachineTurnMessage DONE " << std::endl );
}

void MachLogMessageBroker::processMachineTurnMessage( NetMessage* pNetMessage )
{
	//MachLogMachineTurnMessage* pMessage = _REINTERPRET_CAST( MachLogMachineTurnMessage*, &pNetMessage->body().body()[0] );
	MachLogMachineTurnMessage* pMessage = _REINTERPRET_CAST( MachLogMachineTurnMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0]) );
	DEBUG_STREAM( DIAG_NETWORK,"processMachineTurnMessage " << pMessage->whichId_ << std::endl );
	ASSERT( MachLogRaces::instance().actorExists( pMessage->whichId_ ), "Actor does not exists" );
	if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
	{
		MachActor& actor = MachLogRaces::instance().actor( pMessage->whichId_ );
		if( actor.objectIsMachine() )
			actor.asMachine().turnWithoutEcho( pMessage->turnAmount_ );
		else if( actor.objectType() == MachLog::MISSILE_EMPLACEMENT )
		{
			actor.asMissileEmplacement().turnWithoutEcho( pMessage->turnAmount_ );
		}
	}
}

void MachLogMessageBroker::sendAddRestingObstacleMessage( UtlId whichId, const MexTransform3d& transform, int /*status*/ )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendAddRestingObstacleMessage (" << whichId << ")" << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogRestingObstacleMessage* pMessage = _REINTERPRET_CAST( MachLogRestingObstacleMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = ADD_RESTING_OBSTACLE_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogRestingObstacleMessage );
	pMessage->whichId_ = whichId;
	pMessage->transform_ = transform;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendAddRestingObstacleMessage DONE " << std::endl );
}

void MachLogMessageBroker::processAddRestingObstacleMessage( NetMessage* pNetMessage )
{
	//MachLogRestingObstacleMessage* pMessage = _REINTERPRET_CAST( MachLogRestingObstacleMessage*, &pNetMessage->body().body()[0] );
	MachLogRestingObstacleMessage* pMessage = _REINTERPRET_CAST( MachLogRestingObstacleMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processAddRestingObstacleMessage " << pMessage->whichId_ << std::endl );
	//machines are constructed in a funny order this assertion isn't always valid
	//ASSERT( MachLogRaces::instance().actorExists( pMessage->whichId_ ), "Actor does not exists" );
	if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
	{
		MachActor& actor = MachLogRaces::instance().actor( pMessage->whichId_ );
		if( actor.objectIsMachine() )
        {
            //It is incorrect to set the state of a remote motion sequencer to the same state as the owning node.
            //For example, if we set the state to INTERNAL_PLANNING, the remote motion sequencer will assume
            //it owns a findpath, and try to delete it on construction.
            //Hence we no longer use the transmitted status.
			actor.asMachine().motionSeq().addRestingObstacleWithoutEcho( pMessage->transform_ );
        }
	}
}

void MachLogMessageBroker::sendAddRestingObstacleShortMessage( UtlId whichId )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendAddRestingObstacleShortMessage (" << whichId << ")" << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogRestingObstacleShortMessage* pMessage = _REINTERPRET_CAST( MachLogRestingObstacleShortMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = ADD_RESTING_OBSTACLE_SHORT_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogRestingObstacleShortMessage );
	pMessage->whichId_ = whichId;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendAddRestingObstacleShortMessage DONE " << std::endl );
}

void MachLogMessageBroker::processAddRestingObstacleShortMessage( NetMessage* pNetMessage )
{
	//MachLogRestingObstacleShortMessage* pMessage = _REINTERPRET_CAST( MachLogRestingObstacleShortMessage*, &pNetMessage->body().body()[0] );
	MachLogRestingObstacleShortMessage* pMessage = _REINTERPRET_CAST( MachLogRestingObstacleShortMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processAddRestingObstacleShortMessage " << pMessage->whichId_ << std::endl );
	if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
	{
		MachActor& actor = MachLogRaces::instance().actor( pMessage->whichId_ );
		if( actor.objectIsMachine() )
        {
			actor.asMachine().motionSeq().addRestingObstacleWithoutEcho();
        }
	}
}


void MachLogMessageBroker::sendRequestAddRestingObstacleMessage( UtlId whichId, const MexTransform3d& transform, int status )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendRequestAddRestingObstacleMessage (" << whichId << ")" << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogRestingObstacleMessage* pMessage = _REINTERPRET_CAST( MachLogRestingObstacleMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = REQUEST_ADD_RESTING_OBSTACLE_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogRestingObstacleMessage );
	pMessage->whichId_ = whichId;
	pMessage->transform_ = transform;
	pMessage->status_ = status;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendRequestAddRestingObstacleMessage DONE " << std::endl );
}

void MachLogMessageBroker::processRequestAddRestingObstacleMessage( NetMessage* pNetMessage )
{
	//MachLogRestingObstacleMessage* pMessage = _REINTERPRET_CAST( MachLogRestingObstacleMessage*, &pNetMessage->body().body()[0] );
	MachLogRestingObstacleMessage* pMessage = _REINTERPRET_CAST( MachLogRestingObstacleMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processRequestAddRestingObstacleMessage " << pMessage->whichId_ << std::endl );
	if( MachLogNetwork::instance().isNodeLogicalHost() )
	{
		ASSERT( MachLogRaces::instance().actorExists( pMessage->whichId_ ), "Actor does not exists" );
		if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
		{
			MachActor& actor = MachLogRaces::instance().actor( pMessage->whichId_ );
			if( actor.objectIsMachine() )
			{
				actor.asMachine().motionSeq().setDomain( pMessage->transform_ );
				actor.asMachine().motionSeq().changeStatus( pMessage->status_ );
				actor.asMachine().motionSeq().addRestingObstacle();
			}
		}
	}
}

//oldvoid MachLogMessageBroker::sendAddMovingObstacleMessage( UtlId whichId, const MexPoint3d& start, const MexPoint3d& end, int status )
//old{
//old	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendAddMovingObstacleMessage (" << whichId << ")" << std::endl );
//old	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
//old	MachLogMovingObstacleMessage* pMessage = _REINTERPRET_CAST( MachLogMovingObstacleMessage*, pLogNetMessage );
//old	pMessage->header_.systemCode_ = 0;
//old	pMessage->header_.messageCode_ = ADD_MOVING_OBSTACLE_CODE;
//old	pMessage->header_.totalLength_ = sizeof( MachLogMovingObstacleMessage );
//old	pMessage->whichId_ = whichId;
//old	pMessage->start_ = start;
//old	pMessage->end_ = end;
//old	pMessage->status_ = status;
//old	doSend( pLogNetMessage );
//old	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendAddMovingObstacleMessage DONE " << std::endl );
//old}
//old
//oldvoid MachLogMessageBroker::processAddMovingObstacleMessage( NetMessage* pNetMessage )
//old{
//old	MachLogMovingObstacleMessage* pMessage = _REINTERPRET_CAST( MachLogMovingObstacleMessage*, &pNetMessage->body().body()[0] );
//old	DEBUG_STREAM( DIAG_NETWORK,"processAddMovingObstacleMessage " << pMessage->whichId_ << std::endl );
//old	ASSERT( MachLogRaces::instance().actorExists( pMessage->whichId_ ), "Actor does not exists" );
//old	if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
//old	{
//old		MachActor& actor = MachLogRaces::instance().actor( pMessage->whichId_ );
//old		ASSERT( actor.objectIsMachine(), "actor is not a machine - oops.\n" );
//old		if( actor.objectIsMachine() )
//old		{
//old			MachLogMachineMotionSequencer& motionSeq = actor.asMachine().motionSeq();
//old			motionSeq.changeStatus( pMessage->status_ );
//old//  TBD: Remove this entire function
//old//			motionSeq.addMovingObstacle( pMessage->start_, pMessage->end_ );
//old			//by this time the original intersecting relationship has been removed by adding a temporary resting obstacle
//old			//while the node was waiting for a confirm. (Or other nodes other than host).
//old			motionSeq.setIntersectingDomains();
//old		}
//old	}
//old}
//old

//oldvoid MachLogMessageBroker::sendRequestAddMovingObstacleMessage( UtlId whichId, const MexPoint3d& start, const MexPoint3d& end, int status )
//old{
//old	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendRequestAddMovingObstacleMessage (" << whichId << ")" << std::endl );
//old	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
//old	MachLogMovingObstacleMessage* pMessage = _REINTERPRET_CAST( MachLogMovingObstacleMessage*, pLogNetMessage );
//old	pMessage->header_.systemCode_ = 0;
//old	pMessage->header_.messageCode_ = REQUEST_ADD_MOVING_OBSTACLE_CODE;
//old	pMessage->header_.totalLength_ = sizeof( MachLogMovingObstacleMessage );
//old	pMessage->whichId_ = whichId;
//old	pMessage->start_ = start;
//old	pMessage->end_ = end;
//old	pMessage->status_ = status;
//old	doSend( pLogNetMessage );
//old	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendRequestAddMovingObstacleMessage DONE " << std::endl );
//old}
//old
//oldvoid MachLogMessageBroker::processRequestAddMovingObstacleMessage( NetMessage* pNetMessage )
//old{
//old	MachLogMovingObstacleMessage* pMessage = _REINTERPRET_CAST( MachLogMovingObstacleMessage*, &pNetMessage->body().body()[0] );
//old	DEBUG_STREAM( DIAG_NETWORK,"processRequestAddMovingObstacleMessage " << pMessage->whichId_ << std::endl );
//old	if( MachLogNetwork::instance().isNodeLogicalHost() )
//old	{
//old		ASSERT( MachLogRaces::instance().actorExists( pMessage->whichId_ ), "Actor does not exists" );
//old		if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
//old		{
//old			MachActor& actor = MachLogRaces::instance().actor( pMessage->whichId_ );
//old			ASSERT( actor.objectIsMachine(), "actor is not a machine - oops.\n" );
//old			if( actor.objectIsMachine() )
//old			{
//old				actor.asMachine().motionSeq().changeStatus( pMessage->status_ );
//old//  TBD: Remove this entire function
//old//				actor.asMachine().motionSeq().addMovingObstacle( pMessage->start_, pMessage->end_ );
//old				sendAddMovingObstacleMessage( pMessage->whichId_, pMessage->start_, pMessage->end_, pMessage->status_ );
//old			}
//old		}
//old	}
//old}
//old
void MachLogMessageBroker::sendAddMotionChunksMessage( UtlId whichId, const ctl_vector< PhysMotionChunk >& chunks, int status )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendAddMotionChunksMessage (" << whichId << ")" << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogMotionChunksMessage* pMessage = _REINTERPRET_CAST( MachLogMotionChunksMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = ADD_MOTION_CHUNKS_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogMotionChunksMessage ) - MachLogMotionChunksMessage::dataSize() + chunks.size() * sizeof( PhysCompressedMotionChunk );
	pMessage->whichId_ = whichId;
	pMessage->nChunks_ = chunks.size();
	pMessage->status_ = status;
	for( int i = 0; i < chunks.size(); ++i )
	{
		PhysCompressedMotionChunk chunk = chunks[i];
		memcpy( (void*)(&pMessage->data_[0] + i * sizeof( PhysCompressedMotionChunk ) ),(void*)&chunk, sizeof( PhysCompressedMotionChunk ) );
	}
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendAddMotionChunksMessage DONE " << std::endl );
}

void MachLogMessageBroker::processAddMotionChunksMessage( NetMessage* pNetMessage )
{
	//MachLogMotionChunksMessage* pMessage = _REINTERPRET_CAST( MachLogMotionChunksMessage*, &pNetMessage->body().body()[0] );
	MachLogMotionChunksMessage* pMessage = _REINTERPRET_CAST( MachLogMotionChunksMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processAddMotionChunksMessage " << pMessage->whichId_ << std::endl );
	ASSERT( MachLogRaces::instance().actorExists( pMessage->whichId_ ), "Actor does not exists" );
	if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
	{
		MachActor& actor = MachLogRaces::instance().actor( pMessage->whichId_ );
		ASSERT( actor.objectIsMachine(), "actor is not a machine - oops.\n" );
		if( actor.objectIsMachine() )
		{
			actor.asMachine().motionSeq().changeStatus( pMessage->status_ );
			ctl_vector< PhysMotionChunk > chunks;
			for( int i = 0; i < pMessage->nChunks_; ++i )
			{
				PhysCompressedMotionChunk* pChunk = _REINTERPRET_CAST( PhysCompressedMotionChunk*, (pMessage->data_ + i * sizeof( PhysCompressedMotionChunk ) ) );
				chunks.push_back( *pChunk );
			}
			MachLogMachineMotionSequencer& motSeq = actor.asMachine().motionSeq();
			motSeq.addMotionChunks( chunks );
			motSeq.setIntersectingDomains();
		}
	}
}


void MachLogMessageBroker::sendRequestAddMotionChunksMessage( UtlId whichId, const ctl_vector< PhysMotionChunk >& chunks, int status )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendRequestAddMotionChunksMessage (" << whichId << ")" << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogMotionChunksMessage* pMessage = _REINTERPRET_CAST( MachLogMotionChunksMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = REQUEST_ADD_MOTION_CHUNKS_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogMotionChunksMessage ) - MachLogMotionChunksMessage::dataSize() + chunks.size() * sizeof( PhysCompressedMotionChunk );
	pMessage->whichId_ = whichId;
	pMessage->nChunks_ = chunks.size();
	pMessage->status_ = status;
	for( int i = 0; i < chunks.size(); ++i )
	{
		PhysCompressedMotionChunk chunk = chunks[i];
		memcpy( (void*)(&pMessage->data_[0] + i * sizeof( PhysCompressedMotionChunk ) ),(void*)&chunk, sizeof( PhysCompressedMotionChunk ) );
	}
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendRequestAddMotionChunksMessage DONE " << std::endl );
}

void MachLogMessageBroker::processRequestAddMotionChunksMessage( NetMessage* pNetMessage )
{
	//MachLogMotionChunksMessage* pMessage = _REINTERPRET_CAST( MachLogMotionChunksMessage*, &pNetMessage->body().body()[0] );
	MachLogMotionChunksMessage* pMessage = _REINTERPRET_CAST( MachLogMotionChunksMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processRequestAddMotionChunksMessage " << pMessage->whichId_ << std::endl );
	//if we are the logical host then verify addition as normal and send back failure if it didn't work
	//if not host then treat this as an AddMotionChunksMessage
	if( MachLogNetwork::instance().isNodeLogicalHost() )
	{
//		ASSERT( MachLogRaces::instance().actorExists( pMessage->whichId_ ), "Actor does not exists" );
		if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
		{
			MachActor& actor = MachLogRaces::instance().actor( pMessage->whichId_ );
//			ASSERT( actor.objectIsMachine(), "actor is not a machine - oops.\n" );
			if( actor.objectIsMachine() )
			{
    			MachLogMachineMotionSequencer& motSeq = actor.asMachine().motionSeq();
				ctl_vector< PhysMotionChunk > chunks;
                chunks.reserve( pMessage->nChunks_ );

				for( int i = 0; i < pMessage->nChunks_; ++i )
				{
					PhysCompressedMotionChunk* pChunk = _REINTERPRET_CAST( PhysCompressedMotionChunk*, (pMessage->data_ + i * sizeof( PhysCompressedMotionChunk ) ) );
					chunks.push_back( *pChunk );
				}

				if( motSeq.addMotionChunks( chunks ) )
                {
					motSeq.changeStatus( pMessage->status_ );
        			motSeq.setIntersectingDomains();
                }
				else
				{
					sendMotionChunksAddFailedMessage( pMessage->whichId_, pMessage->status_, actor.globalTransform() );
				}
			}
		}
	}
	else
		processAddMotionChunksMessage( pNetMessage );
}

void MachLogMessageBroker::sendMotionChunksAddFailedMessage( UtlId whichId, int status, const MexTransform3d& transform )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendMotionChunksAddFailedMessage (" << whichId << ")" << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogMotionChunksAddFailedMessage* pMessage = _REINTERPRET_CAST( MachLogMotionChunksAddFailedMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = MOTION_CHUNKS_FAILED_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogMotionChunksAddFailedMessage );
	pMessage->whichId_ = whichId;
	pMessage->status_ = status;
	pMessage->transform_ = transform;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendMotionChunksAddFailedMessage DONE " << std::endl );
}

void MachLogMessageBroker::processMotionChunksAddFailedMessage( NetMessage* pNetMessage )
{
	//MachLogMotionChunksAddFailedMessage* pMessage = _REINTERPRET_CAST( MachLogMotionChunksAddFailedMessage*, &pNetMessage->body().body()[0] );
	MachLogMotionChunksAddFailedMessage* pMessage = _REINTERPRET_CAST( MachLogMotionChunksAddFailedMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processMotionChunksAddFailedMessage " << pMessage->whichId_ << std::endl );
	ASSERT( MachLogRaces::instance().actorExists( pMessage->whichId_ ), "Actor does not exists" );
	if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
	{
		MachActor& actor = MachLogRaces::instance().actor( pMessage->whichId_ );
		ASSERT( actor.objectIsMachine(), "actor is not a machine - oops.\n" );
		if( actor.objectIsMachine() )
		{
			//if( MachLogNetwork::instance().remoteStatus( actor.race() ) == MachLogNetwork::LOCAL_PROCESS )
			{
				MachLogMachineMotionSequencer& motionSeq = actor.asMachine().motionSeq();
				motionSeq.removeMotionChunksWithoutEcho();
				motionSeq.changeStatus( pMessage->status_ );
				motionSeq.setDomain( pMessage->transform_ );
				motionSeq.stop();
			}
		}
	}
}

void MachLogMessageBroker::sendClearRestingObstacleMessage( UtlId whichId )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendClearRestingObstacleMessage (" << whichId << ")" << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogClearRestingObstacleMessage* pMessage = _REINTERPRET_CAST( MachLogClearRestingObstacleMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = CLEAR_RESTING_OBSTACLE_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogClearRestingObstacleMessage );
	pMessage->whichId_ = whichId;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendClearRestingObstacleMessage DONE " << std::endl );
}

void MachLogMessageBroker::processClearRestingObstacleMessage( NetMessage* pNetMessage )
{
	//MachLogClearRestingObstacleMessage* pMessage = _REINTERPRET_CAST( MachLogClearRestingObstacleMessage*, &pNetMessage->body().body()[0] );
	MachLogClearRestingObstacleMessage* pMessage = _REINTERPRET_CAST( MachLogClearRestingObstacleMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processClearRestingObstacleMessage " << pMessage->whichId_ << std::endl );
	//assertion removed as this is actually a valid message before a machine is constructed.
	//ASSERT( MachLogRaces::instance().actorExists( pMessage->whichId_ ), "Actor does not exists" );
	if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
	{
		MachActor& actor = MachLogRaces::instance().actor( pMessage->whichId_ );
		if( actor.objectIsMachine() )
		{
			actor.asMachine().motionSeq().clearRestingObstacleWithoutEcho();
		}
	}
}

void MachLogMessageBroker::sendClearMovingChunksMessage( UtlId whichId )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendClearMovingObstacleMessage (" << whichId << ")" << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogMovingObstacleMessage* pMessage = _REINTERPRET_CAST( MachLogMovingObstacleMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = CLEAR_MOVING_CHUNKS_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogMovingObstacleMessage );
	pMessage->whichId_ = whichId;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendClearMovingObstacleMessage DONE " << std::endl );
}

void MachLogMessageBroker::processClearMovingChunksMessage( NetMessage* pNetMessage )
{
	//MachLogMovingObstacleMessage* pMessage = _REINTERPRET_CAST( MachLogMovingObstacleMessage*, &pNetMessage->body().body()[0] );
	MachLogMovingObstacleMessage* pMessage = _REINTERPRET_CAST( MachLogMovingObstacleMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processClearMovingObstacleMessage " << pMessage->whichId_ << std::endl );
	//ASSERT( MachLogRaces::instance().actorExists( pMessage->whichId_ ), "Actor does not exists" );
	if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
	{
		MachActor& actor = MachLogRaces::instance().actor( pMessage->whichId_ );
		ASSERT( actor.objectIsMachine(), "actor is not a machine - oops.\n" );
		if( actor.objectIsMachine() )
		{
			actor.asMachine().motionSeq().removeMotionChunksWithoutEcho();
		}
	}
}

void MachLogMessageBroker::sendStopDeadMessage( UtlId whichId, const MexTransform3d& transform )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendStopDeadMessage (" << whichId << ")" << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogStopDeadMessage* pMessage = _REINTERPRET_CAST( MachLogStopDeadMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = STOP_DEAD_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogStopDeadMessage );
	pMessage->whichId_ = whichId;
	pMessage->transform_ = transform;
	DEBUG_STREAM( DIAG_NETWORK, " transform " << transform << " domain at info " << MachLogPlanetDomains::pDomainAt( transform.position() ) << std::endl );
	DEBUG_STREAM( DIAG_NETWORK, " local transfrom here is " << _CONST_CAST( const MachActor&, MachLogRaces::instance().actor( whichId ) ).physObject().localTransform() << std::endl );
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendStopDeadMessage DONE " << std::endl );
}

void MachLogMessageBroker::processStopDeadMessage( NetMessage* pNetMessage )
{
	//MachLogStopDeadMessage* pMessage = _REINTERPRET_CAST( MachLogStopDeadMessage*, &pNetMessage->body().body()[0] );
	MachLogStopDeadMessage* pMessage = _REINTERPRET_CAST( MachLogStopDeadMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processStopDeadMessage " << pMessage->whichId_ << std::endl );
// TODO?: fails\/
//	ASSERT( MachLogRaces::instance().actorExists( pMessage->whichId_ ), "Actor does not exists" );
	if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
	{
		MachActor& actor = MachLogRaces::instance().actor( pMessage->whichId_ );
		ASSERT( actor.objectIsMachine(), "actor is not a machine - oops.\n" );
		if( actor.objectIsMachine() )
		{
			MachLogMachineMotionSequencer& motionSeq = actor.asMachine().motionSeq();
			motionSeq.stopDeadWithoutEcho();
			//set global transform but do NOT recompute domain parentage etc - this will be performed
			//by a subsequent message either a move or resting obstacle.
			actor.asMachine().physMachine().globalTransform( pMessage->transform_ );
			//do not recompute the domain!!
//			motionSeq.setDomain( pMessage->transform_ );
			motionSeq.setIntersectingDomains();
		}
	}
}

void MachLogMessageBroker::sendAdjustHPAndArmourMessage( UtlId whichId, int hp, int armour )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendAdjustHPAndArmourMessage (" << whichId << ")" << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogAdjustHPAndArmourMessage* pMessage = _REINTERPRET_CAST( MachLogAdjustHPAndArmourMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = ADJUST_HP_AND_ARMOUR_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogAdjustHPAndArmourMessage );
	pMessage->whichId_ = whichId;
	pMessage->hp_ = hp;
	pMessage->armour_ = armour;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendAdjustHPAndArmourMessage DONE " << std::endl );
}

void MachLogMessageBroker::processAdjustHPAndArmourMessage( NetMessage* pNetMessage )
{
	//MachLogAdjustHPAndArmourMessage* pMessage = _REINTERPRET_CAST( MachLogAdjustHPAndArmourMessage*, &pNetMessage->body().body()[0] );
	MachLogAdjustHPAndArmourMessage* pMessage = _REINTERPRET_CAST( MachLogAdjustHPAndArmourMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processAdjustHPAndArmourMessageMessage " << pMessage->whichId_ << std::endl );
	ASSERT( MachLogRaces::instance().actorExists( pMessage->whichId_ ), "Actor does not exists" );
	if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
	{
		MachActor& actor = MachLogRaces::instance().actor( pMessage->whichId_ );
		actor.setHPAndArmour( pMessage->hp_, pMessage->armour_ );
	}
}



void MachLogMessageBroker::sendCreateActorMessage
(
	MachPhys::Race race,
	MachLog::ObjectType ot,
	int	subType,
	int hwLevel,
	int swLevel,
	const MexPoint3d& position,
	const MexRadians& orientation,
	MachPhys::WeaponCombo	weaponCombo,
	UtlId					whichId
)
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendCreateActorMessage " << ot << std::endl );
	MachLogNetMessage* pMessage = _NEW( MachLogNetMessage() );
	MachLogCreateActorMessage* createMessage = _REINTERPRET_CAST( MachLogCreateActorMessage*, pMessage );
	createMessage->race_ = race;
	createMessage->objectType_ = ot;
	createMessage->subType_ = subType;
	createMessage->hwLevel_ = hwLevel;
	createMessage->swLevel_ = swLevel;
	createMessage->position_ = position;
	createMessage->orientation_ = orientation;
	createMessage->weaponCombo_ = weaponCombo;
	createMessage->whichId_ = whichId;

	createMessage->header_.systemCode_ = 0;
	createMessage->header_.messageCode_ = CREATE_ACTOR_CODE;
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendCreateActorMessage set sizeof " << std::endl );
	DEBUG_STREAM( DIAG_NETWORK, " message: " << *createMessage << std::endl );
	createMessage->header_.totalLength_ = sizeof( MachLogCreateActorMessage );

	doSend( pMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendCreateActorMessage DONE " << std::endl );
}

void MachLogMessageBroker::processCreateActorMessage( NetMessage* pMessage )
{
	//MachLogCreateActorMessage* pCreateMessage = _REINTERPRET_CAST( MachLogCreateActorMessage*, &pMessage->body().body()[0] );
	MachLogCreateActorMessage* pCreateMessage = _REINTERPRET_CAST( MachLogCreateActorMessage*, _CONST_CAST(uint8*, &pMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processCreateActorMessage " << pCreateMessage->objectType_ << std::endl );
	DEBUG_STREAM( DIAG_NETWORK, *pCreateMessage << std::endl );

	#ifndef PRODUCTION
	if( MachLogRaces::instance().actorExists( pCreateMessage->whichId_ ) )
	{
		NETWORK_ERRORS_STREAM("NE01: Actor with id " << pCreateMessage->whichId_ << " ALREADY Exists\nActor:\n" );
		NETWORK_ERRORS_INDENT( 2 );
		NETWORK_ERRORS_STREAM( MachLogRaces::instance().actor( pCreateMessage->whichId_ ) << std::endl );
		NETWORK_ERRORS_INDENT( -2 );
	}
	#endif

	switch( pCreateMessage->objectType_ )
	{
		case MachLog::BEACON:
		case MachLog::FACTORY:
		case MachLog::GARRISON:
		case MachLog::HARDWARE_LAB:
		case MachLog::MINE:
		case MachLog::MISSILE_EMPLACEMENT:
		case MachLog::POD:
		case MachLog::SMELTER:
		{
			MachLogConstruction* pConstruction = MachLogActorMaker::newLogConstruction( pCreateMessage->objectType_,
													pCreateMessage->subType_,
													pCreateMessage->hwLevel_,
													pCreateMessage->position_,
													pCreateMessage->orientation_,
													pCreateMessage->race_,
													pCreateMessage->whichId_ );
//Oops - this line of code below will fully build the construction in question...not good.
//			pConstruction->advanceConstructionState( pConstruction->constructionData().cost() );

		}
			break;
		case MachLog::AGGRESSOR:
		case MachLog::ADMINISTRATOR:
		case MachLog::APC:
		case MachLog::CONSTRUCTOR:
		case MachLog::GEO_LOCATOR:
		case MachLog::RESOURCE_CARRIER:
		case MachLog::SPY_LOCATOR:
		case MachLog::TECHNICIAN:
		{
			MachLogMachine* pMachine = MachLogActorMaker::newLogMachine( pCreateMessage->objectType_,
													pCreateMessage->subType_,
													pCreateMessage->hwLevel_,
													pCreateMessage->swLevel_,
													pCreateMessage->race_,
													pCreateMessage->position_,
													pCreateMessage->weaponCombo_,
													pCreateMessage->whichId_ );
			pMachine->teleportIntoWorld();
			break;
		}
		case MachLog::ORE_HOLOGRAPH:
			MachLogActorMaker::newLogOreHolograph( pCreateMessage->race_,
													pCreateMessage->hwLevel_,
													pCreateMessage->swLevel_,
													pCreateMessage->position_,
													pCreateMessage->whichId_ );
			break;
		case MachLog::ARTEFACT:
			if( pCreateMessage->hwLevel_ == 0 )
				MachLogActorMaker::newLogArtefact( pCreateMessage->subType_,
													pCreateMessage->position_,
													pCreateMessage->orientation_,
													pCreateMessage->whichId_ );
			else
				MachLogActorMaker::newDumbArtefact( pCreateMessage->subType_,
													pCreateMessage->position_,
													pCreateMessage->orientation_ );
			break;
		default:
			ASSERT_INFO( pCreateMessage->objectType_ );
			ASSERT_BAD_CASE;
	}
}

void MachLogMessageBroker::sendCreateSpecialActorMessage
(
	MachPhys::Race race,
	MachLog::ObjectType ot,
	int quantity,
	const MexPoint3d& position,
	const MexAlignedBox2d& boundary,
	UtlId					whichId
)
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendCreateSpecialActorMessage " << ot << std::endl );
	MachLogNetMessage* pLogMessage = _NEW( MachLogNetMessage() );
	MachLogCreateSpecialActorMessage* pMessage = _REINTERPRET_CAST( MachLogCreateSpecialActorMessage*, pLogMessage );
	pMessage->race_ = race;
	pMessage->objectType_ = ot;
	pMessage->quantity_ = quantity;
	pMessage->position_ = position;
	pMessage->boundary_ = boundary;
	pMessage->whichId_ = whichId;

	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = CREATE_SPECIAL_ACTOR_CODE;
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendCreateSpecialActorMessage " << std::endl );
	DEBUG_STREAM( DIAG_NETWORK, " message:\n" << *pMessage << std::endl );
	pMessage->header_.totalLength_ = sizeof( MachLogCreateSpecialActorMessage );

	doSend( pLogMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendCreateSpecialActorMessage DONE " << std::endl );
}

void MachLogMessageBroker::processCreateSpecialActorMessage( NetMessage* pLogMessage )
{
	//MachLogCreateSpecialActorMessage* pMessage = _REINTERPRET_CAST( MachLogCreateSpecialActorMessage*, &pLogMessage->body().body()[0] );
	MachLogCreateSpecialActorMessage* pMessage = _REINTERPRET_CAST( MachLogCreateSpecialActorMessage*, _CONST_CAST(uint8*, &pLogMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processCreateSpecialActorMessage " << pMessage->objectType_ << std::endl );
	DEBUG_STREAM( DIAG_NETWORK, *pMessage << std::endl );

	#ifndef PRODUCTION
	if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
	{
		NETWORK_ERRORS_STREAM("NE02: Actor with id " << pMessage->whichId_ << " ALREADY Exists\nActor:\n" );
		NETWORK_ERRORS_INDENT( 2 );
		NETWORK_ERRORS_STREAM( MachLogRaces::instance().actor( pMessage->whichId_ ) << std::endl );
		NETWORK_ERRORS_INDENT( -2 );
	}
	#endif

	switch( pMessage->objectType_ )
	{
		case MachLog::DEBRIS:
			MachLogActorMaker::newLogDebris( pMessage->race_,
													pMessage->quantity_,
													pMessage->position_,
													pMessage->boundary_,
													pMessage->whichId_ );
			break;
		default:
			ASSERT_INFO( pMessage->objectType_ );
			ASSERT_BAD_CASE;
	}
}

void MachLogMessageBroker::sendReadyMessage()
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendReadyMessage " << std::endl );
	MachLogNetMessage* pMessage = _NEW( MachLogNetMessage() );
	MachLogReadyMessage* pReadyMessage = _REINTERPRET_CAST( MachLogReadyMessage*, pMessage );
	pReadyMessage->header_.systemCode_ = 0;
	pReadyMessage->header_.messageCode_ = READY_CODE;
	pReadyMessage->header_.totalLength_ = sizeof( MachLogReadyMessage );
	pReadyMessage->race_ = MachLogRaces::instance().pcController().race();
	doSend( pMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendReadyMessage DONE " << std::endl );
}

void MachLogMessageBroker::processReadyMessage( NetMessage* pMessage )
{
	//MachLogReadyMessage* pReadyMessage = _REINTERPRET_CAST( MachLogReadyMessage*, &pMessage->body().body()[0] );
	MachLogReadyMessage* pReadyMessage = _REINTERPRET_CAST( MachLogReadyMessage*, _CONST_CAST(uint8*, &pMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processReadyMessage " << std::endl );
	MachLogNetwork::instance().ready( pReadyMessage->race_, true );
}

void MachLogMessageBroker::sendResyncTimeMessage()
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendResyncTimeMessage " << std::endl );
	MachLogNetMessage* pMessage = _NEW( MachLogNetMessage() );
	MachLogResyncTimeMessage* pResyncMessage = _REINTERPRET_CAST( MachLogResyncTimeMessage*, pMessage );
	pResyncMessage->header_.systemCode_ = 0;
	pResyncMessage->header_.messageCode_ = RESYNC_TIME_CODE;
	pResyncMessage->header_.totalLength_ = sizeof( MachLogResyncTimeMessage );

	bool resetSuspended = false;
	if( SimManager::instance().isSuspended() )
	{
		resetSuspended = true;
		SimManager::instance().resume();
	}

	pResyncMessage->now_ = SimManager::instance().currentTime();

	if( resetSuspended )
		SimManager::instance().suspend();

	doSend( pMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendResyncTimeMessage DONE " << std::endl );
}

void MachLogMessageBroker::processResyncTimeMessage( NetMessage* pMessage )
{
	//MachLogResyncTimeMessage* pResyncMessage = _REINTERPRET_CAST( MachLogResyncTimeMessage*, &pMessage->body().body()[0] );
	MachLogResyncTimeMessage* pResyncMessage = _REINTERPRET_CAST( MachLogResyncTimeMessage*, _CONST_CAST(uint8*, &pMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processResyncTimeMessage " << std::endl );
	DEBUG_STREAM( DIAG_NETWORK, "Current Time " << SimManager::instance().currentTime() << std::endl );
	DEBUG_STREAM( DIAG_NETWORK, "resync time " << pResyncMessage->now_ << std::endl );

    //Ensure not paused
    SimManager& simManager = SimManager::instance();
	bool resetSuspended = false;
	if( simManager.isSuspended() )
	{
		resetSuspended = true;
		simManager.resume();
	}

    //Check for time difference less than the threshold. If so, don't bother doing the resync, as it will
    //cause unnecessary jumping of machines in motion.
    PhysRelativeTime threshold = MachLogRaces::instance().stats().resyncThreshold();
    PhysAbsoluteTime syncTime = pResyncMessage->now_;
    if( Mathex::abs( simManager.currentTime() - syncTime ) > threshold )
    	simManager.setSimulationTime( syncTime );
	if( resetSuspended )
		simManager.suspend();
}

void MachLogMessageBroker::sendUseSpaceDomainMessage( UtlId whichId, UtlId constructionId )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendUseSpaceDomainMessage\n" );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendUseSpaceDomainMessage " << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogUseSpaceDomainMessage* pMessage = _REINTERPRET_CAST( MachLogUseSpaceDomainMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = USE_SPACE_DOMAIN_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogUseSpaceDomainMessage );
	pMessage->whichId_ = whichId;
	pMessage->constructionId_ = constructionId;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendUseSpaceDomainMessage DONE " << std::endl );
}

void MachLogMessageBroker::processUseSpaceDomainMessage( NetMessage* pNetMessage )
{
	//MachLogUseSpaceDomainMessage* pMessage = _REINTERPRET_CAST( MachLogUseSpaceDomainMessage*, &pNetMessage->body().body()[0] );
	MachLogUseSpaceDomainMessage* pMessage = _REINTERPRET_CAST( MachLogUseSpaceDomainMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processUseSpaceDomainMessage " << pMessage->whichId_ << std::endl );
	//ASSERT( MachLogRaces::instance().actorExists( pMessage->whichId_ ), "Actor does not exists" );
	//ASSERT( MachLogRaces::instance().actorExists( pMessage->constructionId_ ), "Actor [the construction] does not exists" );

    //TBD: We really must sort out remote machines entering buildings properly
	if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
	{
        //The actor must be a machine
        MachActor& actor = MachLogRaces::instance().actor( pMessage->whichId_ );
        if( actor.objectIsMachine() )
        {
            MachLogMachine& mlm = actor.asMachine();

            //The other actor must be a construction
            if( MachLogRaces::instance().actorExists( pMessage->constructionId_ ) )
            {
                MachActor& cActor = MachLogRaces::instance().actor( pMessage->constructionId_ );
                if( cActor.objectIsConstruction() )
                {
                    MachLogConstruction& construction = cActor.asConstruction();

                    //Tell the machine's motion sequencer to use the construction's interior domain
                    mlm.motionSeq().useSpaceDomain( &construction.interiorDomain() );

                    //Make the machine an observer of the construction in case it gets destroyed
                    construction.attach( &mlm );
                }
            }
        }
	}
}

ostream& operator <<( ostream& o, const MachLogCreateActorMessage& t )
{
	o << "Race: " << t.race_ << std::endl;
	o << "Object: " << t.objectType_ << std::endl;
	o << "subType: " << t.subType_ << " hwLevel: " << t.hwLevel_ << " swLevel: " << t.swLevel_ << std::endl;
	o << "position:" << t.position_ << std::endl;
	o << "orientation:" << t.orientation_ << std::endl;
	o << "WC " << t.weaponCombo_ << std::endl;
	o << "WhichId " << t.whichId_ << std::endl;
	return o;
}

ostream& operator <<( ostream& o, const MachLogCreateSpecialActorMessage& t )
{
	o << "Race: " << t.race_ << std::endl;
	o << "Object: " << t.objectType_ << std::endl;
	o << "Quantity: " << t.quantity_ << std::endl;
	o << "Position: " << t.position_ << std::endl;
	o << "Boundary: " << t.boundary_ << std::endl;
	o << "WhichId " << t.whichId_ << std::endl;
	return o;
}


ostream& operator <<( ostream& o, const MachLogMachineMoveMessage& t )
{
    o << "Header: " << t.header_ << std::endl;
	o << "whichId " << t.whichId_ << std::endl;
	o << "topSpeed_ " << t.topSpeed_ << std::endl;
	o << "nMoveInfos_ " << t.nMoveInfos_ << std::endl;
	return o;
};

ostream& operator <<( ostream& o, const MachLogBeHitMessage& t )
{
	o << "whichId_ " << t.whichId_ << std::endl;
	o << "damage_ " << t.damage_ << std::endl;
	o << "weaponType_ " << t.weaponType_ << std::endl;
	if( t.byWhichIdValid_ == MachLogMessageBroker::VALID )
	{
		o << "byWhichIdValid_ is VALID " << t.byWhichId_ << std::endl;
	}
	else
		o << "byWhichIdValid_ is INVALID " << std::endl;
	if( t.lineValid_ == MachLogMessageBroker::VALID )
	{
		o << "lineValid_ is VALID " << std::endl;
	}
	else
		o << "lineValid_ is INVALID " << std::endl;

	o << "physicalDelay_ " << t.physicalDelay_ << std::endl;
	o << "beHitType_ " << t.beHitType_ << std::endl;
	o << "actor now dead? " << t.actorNowDead_ << std::endl;

	return o;
}

void MachLogMessageBroker::sendFirstPersonEndMessage( UtlId whichId, MachPhys::Race race, bool open )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendFirstPersonEndMessage\n" );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogFirstPersonEndMessage* pMessage = _REINTERPRET_CAST( MachLogFirstPersonEndMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = FIRST_PERSON_END_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogFirstPersonEndMessage );
	pMessage->whichId_ = whichId;
	pMessage->race_ = race;
	pMessage->open_ = open;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendFirstPersonEndMessage DONE " << std::endl );
}

void MachLogMessageBroker::processFirstPersonEndMessage( NetMessage* pNetMessage )
{
	//MachLogFirstPersonEndMessage* pMessage = _REINTERPRET_CAST( MachLogFirstPersonEndMessage*, &pNetMessage->body().body()[0] );
	MachLogFirstPersonEndMessage* pMessage = _REINTERPRET_CAST( MachLogFirstPersonEndMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processFirstPersonEndMessage " << pMessage->whichId_ << std::endl );
	//ASSERT( MachLogRaces::instance().actorExists( pMessage->whichId_ ), "Actor does not exist" );
	if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
	{
        MachLogRemoteFirstPersonManager& remoteFirstPersonManager = MachLogRaces::instance().remoteFirstPersonManager();
        if( pMessage->open_ )
            remoteFirstPersonManager.actorOpen( pMessage->whichId_, pMessage->race_ );
        else
            remoteFirstPersonManager.actorClose( pMessage->whichId_, pMessage->race_ );
	}
}

void MachLogMessageBroker::sendFirstPersonUpdateMessage
(
    UtlId whichId, MachPhys::Race race, const MachPhysFirstPersonStateVector& state
)
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendFirstPersonUpdateMessage\n" );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogFirstPersonUpdateMessage* pMessage = _REINTERPRET_CAST( MachLogFirstPersonUpdateMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = FIRST_PERSON_UPDATE_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogFirstPersonUpdateMessage );
	pMessage->whichId_ = whichId;
	pMessage->race_ = race;
	pMessage->state_ = state;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendFirstPersonUpdateMessage DONE " << std::endl );
}

void MachLogMessageBroker::processFirstPersonUpdateMessage( NetMessage* pNetMessage )
{
	//MachLogFirstPersonUpdateMessage* pMessage = _REINTERPRET_CAST( MachLogFirstPersonUpdateMessage*, &pNetMessage->body().body()[0] );
	MachLogFirstPersonUpdateMessage* pMessage = _REINTERPRET_CAST( MachLogFirstPersonUpdateMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processFirstPersonUpdateMessage " << pMessage->whichId_ << std::endl );
	//ASSERT( MachLogRaces::instance().actorExists( pMessage->whichId_ ), "Actor does not exist" );
	if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
	{
        MachLogRemoteFirstPersonManager& remoteFirstPersonManager = MachLogRaces::instance().remoteFirstPersonManager();
        remoteFirstPersonManager.actorState( pMessage->whichId_, pMessage->race_, pMessage->state_ );
	}
}

void MachLogMessageBroker::sendEnteredBuildingMessage
(
    UtlId whichId, UtlId constructionId
)
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendEnteredBuildingMessage\n" );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogEnteredBuildingMessage* pMessage = _REINTERPRET_CAST( MachLogEnteredBuildingMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = ENTERED_BUILDING_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogEnteredBuildingMessage );
	pMessage->whichId_ = whichId;
	pMessage->constructionId_ = constructionId;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendEnteredBuildingMessage DONE " << std::endl );
}

void MachLogMessageBroker::processEnteredBuildingMessage( NetMessage* pNetMessage )
{
	//MachLogEnteredBuildingMessage* pMessage = _REINTERPRET_CAST( MachLogEnteredBuildingMessage*, &pNetMessage->body().body()[0] );
	MachLogEnteredBuildingMessage* pMessage = _REINTERPRET_CAST( MachLogEnteredBuildingMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processEnteredBuildingMessage " << pMessage->whichId_ << " constructionId " << pMessage->constructionId_ << std::endl );
	MachLogRaces& races = MachLogRaces::instance();
	if( races.actorExists( pMessage->whichId_ ) )
	{
		MachActor& actor = races.actor( pMessage->whichId_ );
		if( not actor.isIn1stPersonView() )
		{
			if( pMessage->constructionId_ )
			{
				if( races.actorExists( pMessage->constructionId_ ) )
				{
					MachActor& construction = races.actor( pMessage->constructionId_ );
					if( actor.objectIsMachine() and construction.objectIsConstruction() )
						actor.asMachine().insideWhichBuilding( &construction.asConstruction() );
				}
			}
			else
			{
				if( actor.objectIsMachine() )
					actor.asMachine().insideWhichBuilding( NULL );
			}
		}
 	}
}

ostream& operator <<( ostream& o, const MachLogMessageHeader& t )
{
	o << " systemCode_ " << (int)t.systemCode_ << " messageCode_ " << t.messageCode_ << " " << (MachLogMessageBroker::MachLogMessageCode)t.messageCode_ << " totalLength_ " << t.totalLength_ << std::endl;
	return o;
};

ostream& operator <<( ostream& o, const MachLogSingleMoveInfo& t )
{
	o << "startTransform_ " << std::endl << t.startTransform_ << std::endl << "endPosition_ " << t.endPosition_ << std::endl;
    o << "startSpeed_ " << t.startSpeed_ << " startTime_ " << t.startTime_ << " finalState_ " << int(t.finalState_) << std::endl;
	return o;
};

void MachLogMessageBroker::sendStartGameMessage()
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendStartGameMessage " << std::endl );
	MachLogNetMessage* pMessage = _NEW( MachLogNetMessage() );
	MachLogReadyMessage* pReadyMessage = _REINTERPRET_CAST( MachLogReadyMessage*, pMessage );
	pReadyMessage->header_.systemCode_ = 0;
	pReadyMessage->header_.messageCode_ = START_GAME_CODE;
	pReadyMessage->header_.totalLength_ = sizeof( MachLogReadyMessage );
	pReadyMessage->race_ = MachLogRaces::instance().pcController().race();
	doSend( pMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendReadyMessage DONE " << std::endl );
}

void MachLogMessageBroker::processStartGameMessage( NetMessage* pMessage )
{
	//MachLogReadyMessage* pReadyMessage = _REINTERPRET_CAST( MachLogReadyMessage*, &pMessage->body().body()[0] );
	MachLogReadyMessage* pReadyMessage = _REINTERPRET_CAST( MachLogReadyMessage*, _CONST_CAST(uint8*, &pMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processReadyMessage " << std::endl );
	SimManager::instance().resume();
}

void MachLogMessageBroker::sendLostFlagSetMessage( MachPhys::Race race )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sstd::endlostFlagSetMessage " << std::endl );
	MachLogNetMessage* pMessage = _NEW( MachLogNetMessage() );
	MachLogReadyMessage* pReadyMessage = _REINTERPRET_CAST( MachLogReadyMessage*, pMessage );
	pReadyMessage->header_.systemCode_ = 0;
	pReadyMessage->header_.messageCode_ = LOST_FLAG_SET_CODE;
	pReadyMessage->header_.totalLength_ = sizeof( MachLogReadyMessage );
	pReadyMessage->race_ = race;
	doSend( pMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sstd::endlostFlagSetMessage DONE " << std::endl );
}

void MachLogMessageBroker::processLostFlagSetMessage( NetMessage* pMessage )
{
	//MachLogReadyMessage* pReadyMessage = _REINTERPRET_CAST( MachLogReadyMessage*, &pMessage->body().body()[0] );
	MachLogReadyMessage* pReadyMessage = _REINTERPRET_CAST( MachLogReadyMessage*, _CONST_CAST(uint8*, &pMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processReadyMessage " << std::endl );
	MachLogRaces::instance().hasLost( pReadyMessage->race_ , true );
}

