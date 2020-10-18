/*
 * M E S S B R O K . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

//#include "utility/id.hpp"

#include "mathex/point2d.hpp"

#include "network/message.hpp"
#include "network/session.hpp"
#include "network/nodeuid.hpp"
#include "network/netnet.hpp"
#include "network/node.hpp"
#include "network/sysmess.hpp"

#include "phys/phys.hpp"

#include "sim/manager.hpp"

#include "machlog/messbrok.hpp"
#include "machlog/network.hpp"

long outgoingTotalLength = 0;
long nOutgoingMessages = 0;
long nIncomingMessages = 0;
long incomingTotalLength = 0;

MachLogMessageBroker::MachLogMessageBroker()
{

    TEST_INVARIANT;
}

//virtual
MachLogMessageBroker::~MachLogMessageBroker()
{
	while( cachedOutgoingMessages_.size() )
	{
		MachLogNetMessage* pMessage = cachedOutgoingMessages_.back();
		_DELETE( pMessage );
		cachedOutgoingMessages_.erase( cachedOutgoingMessages_.begin() + cachedOutgoingMessages_.size() - 1 );
	}
    TEST_INVARIANT;

}

//virtual
void MachLogMessageBroker::processMessage( NetMessage* pMessage )
{
	++nIncomingMessages;
	//MachLogNetMessage* pLogMessage = _REINTERPRET_CAST( MachLogNetMessage*, pMessage->body().body() );
	MachLogNetMessage* pLogMessage = _REINTERPRET_CAST( MachLogNetMessage*, _CONST_CAST(uint8*, pMessage->body().body() ));
	MachLogMessageCode code = (MachLogMessageCode)pLogMessage->header_.messageCode_;
	int systemCode = pLogMessage->header_.systemCode_;
	incomingTotalLength += pLogMessage->header_.totalLength_;
	DEBUG_STREAM( DIAG_NETWORK,"MLMessageBroker::processMessage " << code << std::endl );
	NET_ANALYSIS_STREAM("in," << systemCode << "," << code << "," << SimManager::instance().currentTime() << "," << pLogMessage->header_.totalLength_ << std::endl );
	switch( code )
	{
		case CREATE_ACTOR_CODE:
			processCreateActorMessage( pMessage );
		break;
		case CREATE_SPECIAL_ACTOR_CODE:
			processCreateSpecialActorMessage( pMessage );
		break;
		case MACHINE_MOVE_CODE:
			processMachineMoveMessage( pMessage );
		break;
		case READY_CODE:
			processReadyMessage( pMessage );
		break;
		case RESYNC_TIME_CODE:
			processResyncTimeMessage( pMessage );
		break;
		case BE_HIT_CODE:
			processBeHitMessage( pMessage );
		break;
		case MACHINE_SET_DOMAIN_CODE:
			processMachineSetDomainMessage( pMessage );
		break;
		case MACHINE_TURN_CODE:
			processMachineTurnMessage( pMessage );
		break;
		case ADD_RESTING_OBSTACLE_CODE:
			processAddRestingObstacleMessage( pMessage );
		break;
		case ADD_RESTING_OBSTACLE_SHORT_CODE:
			processAddRestingObstacleShortMessage( pMessage );
		break;
		case REQUEST_ADD_RESTING_OBSTACLE_CODE:
			processRequestAddRestingObstacleMessage( pMessage );
		break;
		case CLEAR_RESTING_OBSTACLE_CODE:
			processClearRestingObstacleMessage( pMessage );
		break;
		case STOP_DEAD_CODE:
			processStopDeadMessage( pMessage );
		break;
		case ADJUST_HP_AND_ARMOUR_CODE:
			processAdjustHPAndArmourMessage( pMessage );
		break;
		case ADD_MOTION_CHUNKS_CODE:
			processAddMotionChunksMessage( pMessage );
		break;
		case REQUEST_ADD_MOTION_CHUNKS_CODE:
			processRequestAddMotionChunksMessage( pMessage );
		break;
		case MOTION_CHUNKS_FAILED_CODE:
			processMotionChunksAddFailedMessage( pMessage );
		break;
// 		case ADD_MOVING_OBSTACLE_CODE:
// 			processAddMovingObstacleMessage( pMessage );
// 		break;
// 		case REQUEST_ADD_MOVING_OBSTACLE_CODE:
// 			processRequestAddMovingObstacleMessage( pMessage );
// 		break;
		case CLEAR_MOVING_CHUNKS_CODE:
			processClearMovingChunksMessage( pMessage );
		break;
		case FIRE_WEAPON_ANIMATION_CODE:
			processFireWeaponAnimationMessage( pMessage );
		break;
		case UPDATE_COMPLETION_VISUALISATION_CODE:
			processUpdateCompletionVisualisationMessage( pMessage );
		break;
		case ECHO_LINEAR_PROJECTILE_CODE:
			processEchoLinearProjectileMessage( pMessage );
		break;
		case CREATE_SPECIAL_WEAPON_EFFECT_CODE:
			processCreateSpecialWeaponEffectMessage( pMessage );
		break;
		case APPLY_SPECIAL_WEAPON_ANIMATION_CODE:
			processApplySpecialWeaponAnimationMessage( pMessage );
		break;
		case PLAY_NORMAL_OBJECT_ANIMATION_CODE:
			processPlayNormalObjectAnimationMessage( pMessage );
		break;
		case USE_SPACE_DOMAIN_CODE:
			processUseSpaceDomainMessage( pMessage );
		break;
		case WEAPON_INFORMATION_CODE:
			processWeaponInformationMessage( pMessage );
		break;
		case SET_SITE_DISCOVERED_BY_CODE:
			processSetSiteDiscoveredByMessage( pMessage );
		break;
		case EXTRACT_ORE_CODE:
			processSetSiteOreMessage( pMessage );
		break;
		case NEW_RESEARCH_CODE:
			processNewResearchMessage( pMessage );
		break;
		case HEAL_CODE:
			processHealMessage( pMessage );
		break;
		case REMOVE_CONSTRUCTION_CODE:
			processRemoveConstructionMessage( pMessage );
		break;
		case CHANGE_RACE_CODE:
			processChangeRaceMessage( pMessage );
		break;
		case DEBRIS_EXPIRED_CODE:
			processDebrisExpiredMessage( pMessage );
		break;
		case LOAD_ARTEFACT_MODELS_CODE:
			processLoadArtefactModelsMessage( pMessage );
		break;
		case ADD_NEW_ARTEFACT_SUB_TYPE_CODE:
			processAddNewArtefactSubTypeMessage( pMessage );
		break;
		case ADD_ARTEFACT_DAMAGE_LINK_CODE:
			processAddArtefactDamageLinkMessage( pMessage );
		break;
		case MACHINE_CAMOUFLAGE_CHANGED_CODE:
			processCamouflageMachineMessage( pMessage );
		break;
		case CURRENT_TARGET_CODE:
			processCurrentTargetMessage( pMessage );
		break;
		case FIRST_PERSON_END_CODE:
			processFirstPersonEndMessage( pMessage );
		break;
		case FIRST_PERSON_UPDATE_CODE:
			processFirstPersonUpdateMessage( pMessage );
		break;
		case PLAY_CONSTRUCT_ANIMATION_CODE:
			processPlayConstructAnimationMessage( pMessage );
		break;
		case ENTERED_BUILDING_CODE:
			processEnteredBuildingMessage( pMessage );
		break;
		case START_GAME_CODE:
			processStartGameMessage( pMessage );
		break;
		case LOST_FLAG_SET_CODE:
			processLostFlagSetMessage( pMessage );
		break;
		case ECHO_NEW_SCORE_CODE:
			processEchoNewScoreMessage( pMessage );
		break;
		case ACTOR_THREAT_CODE:
			processActorThreatMessage( pMessage );
		break;
		case CHANGE_OF_DISPOSITION_CODE:
			processChangeOfDispositionMessage( pMessage );
		break;
		case SMELT_MACHINE_CODE:
			processSmeltMachineMessage( pMessage );
		break;
		case ADVANCE_CONSTRUCTION_STATE_CODE:
			processAdvanceConstructionStateMessage( pMessage );
		break;
		case ADD_REPAIR_POINTS_CODE:
			processAddRepairPointsMessage( pMessage );
		break;
		case MAKE_COMPLETE_CONSTRUCTION_CODE:
			processMakeCompleteConstructionMessage( pMessage );
		break;
		case IS_ENTRANCE_OPEN_CODE:
			processIsEntranceOpenMessage( pMessage );
		break;
		default:
			ASSERT_INFO( pLogMessage->header_.messageCode_ );
			ASSERT_INFO( code );
			ASSERT_BAD_CASE;
	}
	_DELETE( pMessage );
	DEBUG_STREAM( DIAG_NETWORK,"MLMessageBroker::processMessage DONE " << code << std::endl );
}

void MachLogMessageBroker::doSend( MachLogNetMessage*& pMessage )
{
	//if the network is not in an ok state - which can happen asynchronously
	//then try to recover - if not then do not actually send the message.
	if( NetNetwork::instance().currentStatus() != NetNetwork::NETNET_OK )
	{
		MachLogNetwork::instance().update();
		if( NetNetwork::instance().currentStatus() != NetNetwork::NETNET_OK )
		{
			return;
		}
	}

	++nOutgoingMessages;
	outgoingTotalLength += pMessage->header_.totalLength_;

/*	NetMessage::NetMessageRecipients to;

	NetAppSession::NodeIds::const_iterator i = NetNetwork::instance().session().nodes().begin();
	NetAppSession::NodeIds::const_iterator j = NetNetwork::instance().session().nodes().end();
	for( ; i != j; ++i )
		to.push_back( (*i) );
*/
	//if we are already caching messages then continue to do so - otherwise messages may go out of sequence.
//	if( to.size() == 0 or NetNetwork::instance().imStuffed() or hasCachedOutgoingMessages() )
	if( NetNetwork::instance().imStuffed() or hasCachedOutgoingMessages() )
	{
		addCachedOutgoingMessage( pMessage );
	}
	else
	{
		NetPriority priority(1);
		NetMessageBody body( _REINTERPRET_CAST( const unsigned char*, pMessage ), pMessage->header_.totalLength_ );
//		NetNode& node = MachLogNetwork::instance().node();
//		node.sendMessage( priority, to, body);
		NetNetwork::instance().sendMessage( priority, body);
		MachLogMessageCode code = (MachLogMessageCode)pMessage->header_.messageCode_;
		int systemCode = (int)pMessage->header_.systemCode_;
		NET_ANALYSIS_STREAM("out," << systemCode << "," << code << "," << SimManager::instance().currentTime() << "," << pMessage->header_.totalLength_ << std::endl );
/*		switch( node.currentStatus() )
		{
			case NetNode::NETNODE_OK:
			break;
			case NetNode::NETNODE_CONNECTIONLOST:
			{
				nodeConnectionLost();
			}
			break;
		}*/
		_DELETE( pMessage );
	}
}




//virtual
void MachLogMessageBroker::sendMessage( int , NetNode* , NetMessage::NetMessageRecipients )
{
}

bool MachLogMessageBroker::hasCachedOutgoingMessages() const
{
	return cachedOutgoingMessages_.size() > 0 ;
}

void MachLogMessageBroker::sendCachedOutgoingMessages()
{
	NetMessage::NetMessageRecipients to;
/*	NetAppSession::NodeIds::const_iterator i = NetNetwork::instance().session().nodes().begin();
	NetAppSession::NodeIds::const_iterator j = NetNetwork::instance().session().nodes().end();
	for( ; i != j; ++i )
		to.push_back( (*i) );
*/

	NetPriority priority(1);
	bool connectionLost = false;

//	while( cachedOutgoingMessages_.size() and to.size() > 0 and not NetNetwork::instance().imStuffed() and not connectionLost )
	while( cachedOutgoingMessages_.size() and not NetNetwork::instance().imStuffed() and not connectionLost )
	{
		DEBUG_STREAM( DIAG_NETWORK, " sending a cached message. cachedMessages.size() " << cachedOutgoingMessages_.size() << std::endl );
		MachLogNetMessage* pMessage = cachedOutgoingMessages_.front();
		DEBUG_STREAM( DIAG_NETWORK, " messageCode " << _STATIC_CAST( const MachLogMessageCode, pMessage->header_.messageCode_ ) << std::endl );
		NetMessageBody body( _REINTERPRET_CAST( const unsigned char*, pMessage ), pMessage->header_.totalLength_ );
        NetNetwork::instance().sendMessage( priority, body);
/*		NetNode& node = MachLogNetwork::instance().node();
		node.sendMessage( priority, to, body);
		switch( node.currentStatus() )
		{
			case NetNode::NETNODE_OK:
			break;
			case NetNode::NETNODE_CONNECTIONLOST:
			{
                NETWORK_STREAM("MLMessageBroker::sendCachedOutgoingMessages NETNODE_CONNECTIONLOST returned.\n" );
				nodeConnectionLost();
				connectionLost = true;
			}
			break;
		}*/
		MachLogMessageCode code = (MachLogMessageCode)pMessage->header_.messageCode_;
		int systemCode = (int)pMessage->header_.systemCode_;
		NET_ANALYSIS_STREAM("out," << systemCode << "," << code << "," << SimManager::instance().currentTime() << "," << pMessage->header_.totalLength_ << std::endl );
		_DELETE( pMessage );
		cachedOutgoingMessages_.erase( cachedOutgoingMessages_.begin() );
		DEBUG_STREAM( DIAG_NETWORK, " sent a cached message size() " << cachedOutgoingMessages_.size() << std::endl );
	}
}

void MachLogMessageBroker::addCachedOutgoingMessage( MachLogNetMessage* pMessage )
{
	DEBUG_STREAM( DIAG_NETWORK,"MLMessageBroker::addcachedOutgoingMessages " << std::endl );
	cachedOutgoingMessages_.push_back( pMessage );
	DEBUG_STREAM( DIAG_NETWORK,"MLMessageBroker::addcachedOutgoingMessages size() " << cachedOutgoingMessages_.size() << std::endl );
}

void MachLogMessageBroker::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachLogMessageBroker& t )
{

    o << "MachLogMessageBroker " << (void*)&t << " start" << std::endl;
	o << " nOutgoing " << nOutgoingMessages << std::endl;
	o << " outgoing total Length " << outgoingTotalLength << std::endl;
	o << " nIncoming " << nIncomingMessages << std::endl;
	o << " incoming total Length " << incomingTotalLength << std::endl;
	if( nOutgoingMessages > 0 )
		o << " average size " << outgoingTotalLength / nOutgoingMessages << std::endl;
	if( SimManager::instance().currentTime() > 0 )
		o << " average load per second " << outgoingTotalLength / SimManager::instance().currentTime() << std::endl;
	o << " cachedOutgoingMessages_.size " << t.cachedOutgoingMessages_.size() << std::endl;
    o << "MachLogMessageBroker " << (void*)&t << " end" << std::endl;

    return o;
}

ostream& operator <<( ostream& o, const MachLogMessageBroker::MachLogMessageCode& t )
{
	switch( t )
	{
		case MachLogMessageBroker::CREATE_ACTOR_CODE: o << "CREATE_ACTOR_CODE"; break;
		case MachLogMessageBroker::CREATE_SPECIAL_ACTOR_CODE: o << "CREATE_SPECIAL_ACTOR_CODE"; break;
		case MachLogMessageBroker::MACHINE_MOVE_CODE: o << "MACHINE_MOVE_CODE";	break;
		case MachLogMessageBroker::READY_CODE: o << "READY_CODE"; break;
		case MachLogMessageBroker::RESYNC_TIME_CODE: o << "RESYNC_TIME_CODE:"; break;
		case MachLogMessageBroker::BE_HIT_CODE:o << "BE_HIT_CODE:";	break;
		case MachLogMessageBroker::MACHINE_SET_DOMAIN_CODE: o << "MACHINE_SET_DOMAIN_CODE:";	break;
		case MachLogMessageBroker::MACHINE_TURN_CODE: o << "MACHINE_TURN_CODE:";	break;
		case MachLogMessageBroker::ADD_RESTING_OBSTACLE_CODE: o << "ADD_RESTING_OBSTACLE_CODE:";	break;
		case MachLogMessageBroker::ADD_RESTING_OBSTACLE_SHORT_CODE: o << "ADD_RESTING_OBSTACLE_SHORT_CODE:";	break;
		case MachLogMessageBroker::REQUEST_ADD_RESTING_OBSTACLE_CODE: o << "REQUEST_ADD_RESTING_OBSTACLE_CODE:";	break;
		case MachLogMessageBroker::ADJUST_HP_AND_ARMOUR_CODE: o << "ADJUST_HP_AND_ARMOUR_CODE:";	break;
		case MachLogMessageBroker::CLEAR_RESTING_OBSTACLE_CODE: o << "CLEAR_RESTING_OBSTACLE_CODE:";	break;
		case MachLogMessageBroker::STOP_DEAD_CODE: o << "STOP_DEAD_CODE:";	break;
		case MachLogMessageBroker::ADD_MOTION_CHUNKS_CODE: o << "ADD_MOTION_CHUNKS_CODE:";	break;
		case MachLogMessageBroker::REQUEST_ADD_MOTION_CHUNKS_CODE: o << "REQUEST_ADD_MOTION_CHUNKS_CODE:";	break;
		case MachLogMessageBroker::MOTION_CHUNKS_FAILED_CODE: o << "MOTION_CHUNKS_FAILED_CODE:";	break;
		case MachLogMessageBroker::ADD_MOVING_OBSTACLE_CODE: o << "ADD_MOVING_OBSTACLE_CODE:";	break;
		case MachLogMessageBroker::REQUEST_ADD_MOVING_OBSTACLE_CODE: o << "REQUEST_ADD_MOVING_OBSTACLE_CODE:";	break;
		case MachLogMessageBroker::CLEAR_MOVING_CHUNKS_CODE: o << "CLEAR_MOVING_CHUNKS_CODE:";	break;
		case MachLogMessageBroker::FIRE_WEAPON_ANIMATION_CODE: o << "FIRE_WEAPON_ANIMATION_CODE:";	break;
		case MachLogMessageBroker::UPDATE_COMPLETION_VISUALISATION_CODE: o << "UPDATE_COMPLETION_VISUALISATION_CODE:";	break;
		case MachLogMessageBroker::CREATE_SPECIAL_WEAPON_EFFECT_CODE: o << "CREATE_SPECIAL_WEAPON_EFFECT_CODE:";	break;
		case MachLogMessageBroker::APPLY_SPECIAL_WEAPON_ANIMATION_CODE: o << "APPLY_SPECIAL_WEAPON_ANIMATION_CODE:";	break;
		case MachLogMessageBroker::ECHO_LINEAR_PROJECTILE_CODE: o << "ECHO_LINEAR_PROJECTILE_CODE"; break;
		case MachLogMessageBroker::PLAY_NORMAL_OBJECT_ANIMATION_CODE: o << "PLAY_NORMAL_OBJECT_ANIMATION_CODE"; break;
		case MachLogMessageBroker::USE_SPACE_DOMAIN_CODE: o << "USE_SPACE_DOMAIN_CODE"; break;
		case MachLogMessageBroker::WEAPON_INFORMATION_CODE: o << "WEAPON_INFORMATION_CODE"; break;
		case MachLogMessageBroker::SET_SITE_DISCOVERED_BY_CODE: o << "SET_SITE_DISCOVERED_BY_CODE"; break;
		case MachLogMessageBroker::EXTRACT_ORE_CODE: o << "EXTRACT_ORE_CODE"; break;
		case MachLogMessageBroker::NEW_RESEARCH_CODE: o << "NEW_RESEARCH_CODE"; break;
		case MachLogMessageBroker::HEAL_CODE: o << "HEAL_CODE"; break;
		case MachLogMessageBroker::REMOVE_CONSTRUCTION_CODE: o << "REMOVE_CONSTRUCTION_CODE"; break;
		case MachLogMessageBroker::CHANGE_RACE_CODE: o << "CHANGE_RACE_CODE"; break;
		case MachLogMessageBroker::DEBRIS_EXPIRED_CODE: o << "DEBRIS_EXPIRED_CODE"; break;
		case MachLogMessageBroker::LOAD_ARTEFACT_MODELS_CODE: o << "LOAD_ARTEFACT_MODELS_CODE"; break;
		case MachLogMessageBroker::ADD_NEW_ARTEFACT_SUB_TYPE_CODE: o << "ADD_NEW_ARTEFACT_SUB_TYPE_CODE"; break;
		case MachLogMessageBroker::ADD_ARTEFACT_DAMAGE_LINK_CODE: o << "ADD_ARTEFACT_DAMAGE_LINK_CODE"; break;
		case MachLogMessageBroker::MACHINE_CAMOUFLAGE_CHANGED_CODE: o << "MACHINE_CAMOUFLAGE_CHANGED_CODE"; break;
		case MachLogMessageBroker::CURRENT_TARGET_CODE: o << "CURRENT_TARGET_CODE"; break;
		case MachLogMessageBroker::FIRST_PERSON_END_CODE: o << "FIRST_PERSON_END_CODE"; break;
		case MachLogMessageBroker::FIRST_PERSON_UPDATE_CODE: o << "FIRST_PERSON_UPDATE_CODE"; break;
		case MachLogMessageBroker::PLAY_CONSTRUCT_ANIMATION_CODE: o << "PLAY_CONSTRUCT_ANIMATION_CODE"; break;
		case MachLogMessageBroker::ENTERED_BUILDING_CODE: o << "ENTERED_BUILDING_CODE"; break;
		case MachLogMessageBroker::START_GAME_CODE: o << "START_GAME_CODE"; break;
		case MachLogMessageBroker::LOST_FLAG_SET_CODE: o << "LOST_FLAG_SET_CODE"; break;
		case MachLogMessageBroker::ECHO_NEW_SCORE_CODE: o << "ECHO_NEW_SCORE_CODE"; break;
		case MachLogMessageBroker::ACTOR_THREAT_CODE: o << "ACTOR_THREAT_CODE"; break;
		case MachLogMessageBroker::CHANGE_OF_DISPOSITION_CODE: o << "CHANGE_OF_DISPOSITION_CODE"; break;
		case MachLogMessageBroker::SMELT_MACHINE_CODE: o << "SMELT_MACHINE_CODE"; break;
		case MachLogMessageBroker::ADVANCE_CONSTRUCTION_STATE_CODE: o << "ADVANCE_CONSTRUCTION_STATE_CODE"; break;
		case MachLogMessageBroker::ADD_REPAIR_POINTS_CODE: o << "ADD_REPAIR_POINTS_CODE"; break;
		case MachLogMessageBroker::MAKE_COMPLETE_CONSTRUCTION_CODE: o << "MAKE_COMPLETE_CONSTRUCTION_CODE"; break;
		case MachLogMessageBroker::IS_ENTRANCE_OPEN_CODE: o << "IS_ENTRANCE_OPEN_CODE"; break;
		default:
			o << "Unknown message code " << (int)t;
			break;
	}
	return o;
}

ostream& operator <<( ostream& o, const MachLogMessageBroker::ActorNowDead& t )
{
	switch( t )
	{
		case MachLogMessageBroker::ACTOR_NOT_DEAD: o << "ACTOR_NOT_DEAD"; break;
		case MachLogMessageBroker::ACTOR_DEAD: o << "ACTOR_DEAD"; break;
		default: ASSERT_BAD_CASE;
	}
	return o;
}

//	static
int MachLogMessageBroker::maximumMotionChunks()
{
    //  TBD: Make this calculate the result properly

    return 6;
}


void MachLogMessageBroker::nodeConnectionLost()
{
	NETWORK_STREAM("MachLogMessageBroker::nodeConnectionLost node has lost connection\n" );
//				string reason;
//				reason = ". The connection has been lost in the message broker.\n";
//				NetNetwork::instance().doAbort( reason );
	ctl_vector< string >		nodeNames;
/*	NetAppSession::NodeIds::const_iterator i = NetNetwork::instance().session().nodes().begin();
	NetAppSession::NodeIds::const_iterator j = NetNetwork::instance().session().nodes().end();
	for( ; i != j; ++i )
	{
		nodeNames.push_back( (*i)->nodeName() );
	}*/
	NETWORK_STREAM(" nodes before loss size " << nodeNames.size() << std::endl );
	NETWORK_INDENT( 2 );
	NETWORK_STREAM( nodeNames );
	NETWORK_INDENT( -2 );

//	NetNetwork::instance().session().updateNodes();
//	NETWORK_STREAM(" nodes after loss size " << NetNetwork::instance().session().nodes().size() << std::endl );
	ctl_vector< string >		nodeNamesAfterUpdate;
/*	i = NetNetwork::instance().session().nodes().begin();
	j = NetNetwork::instance().session().nodes().end();
	for( ; i != j; ++i )
	{
		nodeNamesAfterUpdate.push_back( (*i)->nodeName() );
	}*/
	NETWORK_INDENT( 2 );
	NETWORK_STREAM( nodeNamesAfterUpdate );
	NETWORK_INDENT( -2 );
//	if( nodeNamesAfterUpdate.size() == nodeNames.size() )
//	{
//		NETWORK_STREAM("lists are the same size so exiting without doing any work\n" );
//	}
//	else
	{
		for( ctl_vector< string >::const_iterator i = nodeNames.begin(); i != nodeNames.end(); ++i )
		{
			if( find( nodeNamesAfterUpdate.begin(), nodeNamesAfterUpdate.end(), (*i) ) == nodeNamesAfterUpdate.end() )
			{
				if( NetNetwork::instance().hasSystemMessageHandler() )
					NetNetwork::instance().systemMessageHandler().handleDestroyPlayerMessage( (*i) );
			}
		}
	}
    NETWORK_STREAM("MachLogMessageBroker::nodeConnectionLost DONE\n" );
}

/* End MESSBROK.CPP *************************************************/
