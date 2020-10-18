/*
 * M E S S B R O K . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogMessageBroker

    A brief description of the class should go in here
*/

#ifndef _MACHLOG_MESSBROK_HPP
#define _MACHLOG_MESSBROK_HPP
#include "base/base.hpp"
#include "ctl/vector.hpp"
#include "ctl/pvector.hpp"
#include "utility/id.hpp"
#include "phys/phys.hpp"
#include "phys/motchunk.hpp"
#include "network/message.hpp"
//#include "network/nodeuid.hpp"
//#include "network/node.hpp"

#include "machphys/machphys.hpp"
#include "machphys/mcmovinf.hpp"
#include "machlog/machlog.hpp"

#include "mathex/point2d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/transf3d.hpp"

class MexRadians;
class MexLine3d;
class MexAlignedBox2d;
class SysPathName;
class MachPhysFirstPersonStateVector;
class NetNode;
class NetMessage;
class MachLogScoreImpl;

const int	MACHLOG_MAXLENGTH = 1024;

#pragma pack(push,1)
struct MachLogMessageHeader
{
	char 	systemCode_;
	int32	messageCode_;
	int32	totalLength_;
};

struct MachLogNetMessage
{
	MachLogMessageHeader	header_;
	char					data_[ MACHLOG_MAXLENGTH ];
};
#pragma pack(pop)

class MachLogMessageBroker
// Canonical form revoked
{
public:
	enum MachLogMessageCode : uint8 {
			CREATE_ACTOR_CODE, MACHINE_MOVE_CODE, READY_CODE, RESYNC_TIME_CODE, BE_HIT_CODE, MACHINE_SET_DOMAIN_CODE,
			MACHINE_TURN_CODE, ADD_RESTING_OBSTACLE_CODE, REQUEST_ADD_RESTING_OBSTACLE_CODE, ADJUST_HP_AND_ARMOUR_CODE,
			CLEAR_RESTING_OBSTACLE_CODE, STOP_DEAD_CODE, ADD_MOTION_CHUNKS_CODE, REQUEST_ADD_MOTION_CHUNKS_CODE,
			MOTION_CHUNKS_FAILED_CODE, ADD_MOVING_OBSTACLE_CODE, REQUEST_ADD_MOVING_OBSTACLE_CODE,
			CLEAR_MOVING_CHUNKS_CODE, FIRE_WEAPON_ANIMATION_CODE, UPDATE_COMPLETION_VISUALISATION_CODE,
			ECHO_LINEAR_PROJECTILE_CODE, CREATE_SPECIAL_ACTOR_CODE, CREATE_SPECIAL_WEAPON_EFFECT_CODE,
			APPLY_SPECIAL_WEAPON_ANIMATION_CODE, PLAY_NORMAL_OBJECT_ANIMATION_CODE, USE_SPACE_DOMAIN_CODE,
			SET_SITE_DISCOVERED_BY_CODE, NEW_RESEARCH_CODE, HEAL_CODE, REMOVE_CONSTRUCTION_CODE,
			CHANGE_RACE_CODE, DEBRIS_EXPIRED_CODE, LOAD_ARTEFACT_MODELS_CODE, ADD_NEW_ARTEFACT_SUB_TYPE_CODE,
			ADD_ARTEFACT_DAMAGE_LINK_CODE, MACHINE_CAMOUFLAGE_CHANGED_CODE, CURRENT_TARGET_CODE,
            FIRST_PERSON_END_CODE, FIRST_PERSON_UPDATE_CODE, PLAY_CONSTRUCT_ANIMATION_CODE, ENTERED_BUILDING_CODE,
			EXTRACT_ORE_CODE, WEAPON_INFORMATION_CODE, ADD_RESTING_OBSTACLE_SHORT_CODE, START_GAME_CODE, LOST_FLAG_SET_CODE,
			ECHO_NEW_SCORE_CODE, ACTOR_THREAT_CODE, CHANGE_OF_DISPOSITION_CODE, SMELT_MACHINE_CODE,
			ADVANCE_CONSTRUCTION_STATE_CODE, ADD_REPAIR_POINTS_CODE, MAKE_COMPLETE_CONSTRUCTION_CODE,
			IS_ENTRANCE_OPEN_CODE,
			N_CODES
	};

	enum BeHitType :        uint8 { NORMAL_HIT, NO_ANIMATION_HIT };
	enum ActorNowDead :     uint8 { ACTOR_NOT_DEAD, ACTOR_DEAD };
	enum Validity :         uint8 { VALID, INVALID };
	enum HealingFlag :      uint8 { BEGIN_HEALING, STOP_HEALING };
	enum CamouflageFlag :   uint8 { BEGIN_CAMOUFLAGE, STOP_CAMOUFLAGE };
	enum WeaponInfoFlag :   uint8 { ION_CANNON_FIRED, NUKE_FIRED, ION_CANNON_ONLINE, NUKE_ONLINE };
	enum ThreatFlag :       uint8 { ADD_THREAT, REMOVE_THREAT };


	typedef	ctl_pvector< MachLogNetMessage > CachedMessages;
	#pragma pack(push,1)
	struct SingleProjectileDestroyData
	{
		PhysAbsoluteTime		destroyAtTime_;
		MachPhys::StrikeType	strikeType_;
	};
	#pragma pack(pop)

	typedef	ctl_pvector< SingleProjectileDestroyData > ProjectileDestroyData;

    MachLogMessageBroker( void );
    virtual ~MachLogMessageBroker( void );

	virtual void processMessage( NetMessage* );
	//note that message code has to be integer. This allows untype safe but useful different
	//enums to be used at the different library levels.
	virtual void sendMessage( int messageCode, NetNode* from, NetMessage::NetMessageRecipients to );

	bool hasCachedOutgoingMessages() const;
	void sendCachedOutgoingMessages();
	void addCachedOutgoingMessage( MachLogNetMessage* );

	//this is a new helper function so that bits of code that generate motion chunks
	//can know how many the network library can cope with.
	static int maximumMotionChunks();

	//individual message types will be formatted and sent.
	void sendCreateActorMessage( MachPhys::Race race, MachLog::ObjectType ot, int subType, int hwLevel, int swLevel, const MexPoint3d& position, const MexRadians& orientation, MachPhys::WeaponCombo	weaponCombo, UtlId	whichId );
	void sendMachineMoveMessage( UtlId whichId, const MATHEX_SCALAR& topSpeed, const ctl_vector< MachPhysMachineMoveInfo >&, const ctl_vector< MexTransform3d >& );
	void sendMachineSetDomainMessage( UtlId whichId, const MexTransform3d& transform );
	void sendReadyMessage();
	void sendResyncTimeMessage();
	void sendBeHitMessage( UtlId whichId, int damage, MachPhys::WeaponType, MachActor* pByActor, MexLine3d*, PhysRelativeTime, BeHitType, ActorNowDead );
	void sendMachineTurnMessage( UtlId whichId, const MexRadians& amount );
	void sendAddRestingObstacleMessage( UtlId whichId, const MexTransform3d& transform, int status );
	void sendAddRestingObstacleShortMessage( UtlId whichId );
	void sendRequestAddRestingObstacleMessage( UtlId whichId, const MexTransform3d& transform, int status );
	void sendAdjustHPAndArmourMessage( UtlId whichId, int hp, int armour );
	void sendClearRestingObstacleMessage( UtlId whichId );
	void sendClearMovingChunksMessage( UtlId whichId );
	void sendStopDeadMessage( UtlId whichId, const MexTransform3d& transform );
	void sendAddMotionChunksMessage( UtlId whichId, const ctl_vector< PhysMotionChunk >& , int status );
	void sendRequestAddMotionChunksMessage( UtlId whichId, const ctl_vector< PhysMotionChunk >& , int status );
	void sendMotionChunksAddFailedMessage( UtlId whichId, int status, const MexTransform3d& );
	void sendAddMovingObstaclesMessage( UtlId whichId, const MexPoint3d& start, const MexPoint3d& end, int status );
	void sendRequestAddMovingObstaclesMessage( UtlId whichId, const MexPoint3d& start, const MexPoint3d& end, int status );
	void sendFireWeaponAnimationMessage( UtlId whichId, MachPhys::Mounting whichGun, int numberInBurst );
	void sendUpdateCompletionVisualisationMessage( UtlId whichId, double percentage );
	void sendEchoLinearProjectileMessage( UtlId whichId, MachPhys::Mounting whichGun, UtlId targetId, const PhysAbsoluteTime& launchTime, int nRounds, const ProjectileDestroyData&, const MexPoint3d& terrainPosition );
	void sendCreateSpecialActorMessage( MachPhys::Race race, MachLog::ObjectType ot, int quantity, const MexPoint3d& position, const MexAlignedBox2d& boundary, UtlId whichId );
	void sendCreateSpecialWeaponEffectMessage( const MexPoint3d& position, MachPhys::WeaponType, UtlId firerId = 0 );
	void sendApplySpecialWeaponAnimationMessage( UtlId whichId, MachPhys::WeaponType );
	void sendPlayNormalObjectAnimationMessage( UtlId whichId, bool onOrOff );
	void sendPlayConstructAnimationMessage( UtlId constructorId, UtlId constructionId, bool onOrOff );
	void sendUseSpaceDomainMessage( UtlId whichId, UtlId constructionId  );
	void sendWeaponInformationMessage( WeaponInfoFlag weaponInfoFlag, MachPhys::Race );
	void sendSetSiteDiscoveredByMessage( const MexPoint3d& position, MachPhys::Race race  );
	void sendNewResearchMessage( MachLog::ObjectType, int subType, int hwLevel, MachPhys::WeaponCombo, MachPhys::Race );
	void sendHealMessage( UtlId whichId, UtlId targetId, HealingFlag, bool garrisonHealing );
	void sendRemoveConstructionMessage( UtlId whichId );
	void sendChangeRaceMessage( UtlId whichId, MachPhys::Race );
	void sendDebrisExpiredMessage( UtlId whichId );
	void sendLoadArtefactModelsMessage( const SysPathName& );
	//pLocalBoundary is allowed to be NULL as DUMB artefacts do not have any data associated with them.
	void sendAddNewArtefactSubTypeMessage( int subType, const string& name, MachPhys::BuildingMaterialUnits cost,
                     MachPhys::ArmourUnits armour, MachPhys::HitPointUnits hitPoints,
                     uint stringId, MATHEX_SCALAR height, const ctl_vector< MexPoint2d >* pLocalBoundary );
	void sendAddArtefactDamageLinkMessage( UtlId destroyId, UtlId damageId, int hpModifier, int armourModifier );
	void sendCamouflageMachineMessage( UtlId whichId, CamouflageFlag );
	void sendCurrentTargetMessage( UtlId whichId, UtlId targetId );	//special case of targetId == 0 is stop targetting.
    void sendFirstPersonEndMessage( UtlId whichId, MachPhys::Race race, bool open );
    void sendFirstPersonUpdateMessage( UtlId whichId, MachPhys::Race race, const MachPhysFirstPersonStateVector& state );
    void sendEnteredBuildingMessage( UtlId whichId, UtlId constructionId );
	void sendSetSiteOreMessage( int siteId, MachPhys::BuildingMaterialUnits newOreAmount );
	void sendStartGameMessage();
	void sendLostFlagSetMessage( MachPhys::Race );
	void sendEchoNewScoreMessage( const MachLogScoreImpl& );
	void sendActorThreatMessage( UtlId whichId, UtlId threatId, ThreatFlag );
	void sendChangeOfDispositionMessage( MachPhys::Race, MachPhys::Race, int ); //note last paramter is actually a MLRaces::DispositionToRace
	void sendSmeltMachineMessage( UtlId whichId );
	void sendAdvanceConstructionStateMessage( UtlId whichId, MachPhys::BuildingMaterialUnits addAmount );
	void sendAddRepairPointsMessage( UtlId whichId, MachPhys::HitPointUnits );
	void sendMakeCompleteConstructionMessage( UtlId whichId );
	void sendIsEntranceOpenMessage( UtlId whichId, size_t door, bool doOpen );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogMessageBroker& t );
    friend ostream& operator <<( ostream& o, const MachLogMessageCode& t );
    friend ostream& operator <<( ostream& o, const ActorNowDead& t );

protected:
	void doSend( MachLogNetMessage*& pMessage );

private:
	void processCreateActorMessage( NetMessage* pMessage );
	void processResyncTimeMessage( NetMessage* pMessage );
	void processReadyMessage( NetMessage* pMessage );
	void processMachineMoveMessage( NetMessage* pMessage );
	void processMachineSetDomainMessage( NetMessage* pMessage );
	void processBeHitMessage( NetMessage* pMessage );
	void processMachineTurnMessage( NetMessage* pMessage );
	void processAddRestingObstacleMessage( NetMessage* pMessage );
	void processAddRestingObstacleShortMessage( NetMessage* pMessage );
	void processRequestAddRestingObstacleMessage( NetMessage* pMessage );
	void processAdjustHPAndArmourMessage( NetMessage* pMessage );
	void processClearRestingObstacleMessage( NetMessage* pMessage );
	void processClearMovingChunksMessage( NetMessage* pMessage );
	void processStopDeadMessage( NetMessage* pMessage );
	void processAddMotionChunksMessage( NetMessage* pMessage );
	void processRequestAddMotionChunksMessage( NetMessage* pMessage );
	void processMotionChunksAddFailedMessage( NetMessage* pMessage );
	void processAddMovingObstacleMessage( NetMessage* pMessage );
	void processRequestAddMovingObstacleMessage( NetMessage* pMessage );
	void processFireWeaponAnimationMessage( NetMessage* pMessage );
	void processUpdateCompletionVisualisationMessage( NetMessage* pMessage );
	void processEchoLinearProjectileMessage( NetMessage* pMessage );
	void processCreateSpecialActorMessage( NetMessage* pMessage );
	void processCreateSpecialWeaponEffectMessage( NetMessage* pMessage );
	void processApplySpecialWeaponAnimationMessage( NetMessage* pMessage );
	void processPlayNormalObjectAnimationMessage( NetMessage* pMessage );
	void processPlayConstructAnimationMessage( NetMessage* pMessage );
	void processUseSpaceDomainMessage( NetMessage* pMessage );
	void processWeaponInformationMessage( NetMessage* pNetMessage );
	void processSetSiteDiscoveredByMessage( NetMessage* pMessage );
	void processNewResearchMessage( NetMessage* pMessage );
	void processHealMessage( NetMessage* pMessage );
	void processRemoveConstructionMessage( NetMessage* pMessage );
	void processChangeRaceMessage( NetMessage* pMessage );
	void processDebrisExpiredMessage( NetMessage* pMessage );
	void processLoadArtefactModelsMessage( NetMessage* pMessage );
	void processAddNewArtefactSubTypeMessage( NetMessage* pMessage );
	void processAddArtefactDamageLinkMessage( NetMessage* pMessage );
	void processCamouflageMachineMessage( NetMessage* pMessage );
	void processCurrentTargetMessage( NetMessage* pMessage );
    void processFirstPersonEndMessage( NetMessage* pMessage );
    void processFirstPersonUpdateMessage( NetMessage* pMessage );
    void processEnteredBuildingMessage( NetMessage* pMessage );
	void processSetSiteOreMessage( NetMessage* pNetMessage );
	void processStartGameMessage( NetMessage* pNetMessage );
	void processLostFlagSetMessage( NetMessage* pNetMessage );
	void processEchoNewScoreMessage( NetMessage* pNetMessage );
	void processActorThreatMessage( NetMessage* pNetMessage );
	void processChangeOfDispositionMessage(  NetMessage* pNetMessage );
	void processSmeltMachineMessage( NetMessage* pNetMessage );
	void processAdvanceConstructionStateMessage( NetMessage* pNetMessage );
	void processAddRepairPointsMessage( NetMessage* pNetMessage );
	void processMakeCompleteConstructionMessage( NetMessage* pNetMessage );
	void processIsEntranceOpenMessage( NetMessage* pNetMessage );

	void nodeConnectionLost();

    MachLogMessageBroker( const MachLogMessageBroker& );
    MachLogMessageBroker& operator =( const MachLogMessageBroker& );
    bool operator ==( const MachLogMessageBroker& );

	CachedMessages		cachedOutgoingMessages_;
};

#endif

/* End MESSBROK.HPP *************************************************/
