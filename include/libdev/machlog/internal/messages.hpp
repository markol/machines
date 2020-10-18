#ifndef _MACHLOG_MESSAGES_HPP
#define _MACHLOG_MESSAGES_HPP

#include "mathex/line3d.hpp"
#include "mathex/cabox2d.hpp"
#include "mathex/ctrans3d.hpp"
#include "mathex/cpoint3d.hpp"
#include "mathex/cvec3.hpp"
#include "mathex/cline3d.hpp"
#include "phys/motchunk.hpp"
#include "machphys/p1driver.hpp"
#include "machlog/messbrok.hpp"
#include "machlog/internal/scorei.hpp"

extern const int MACHLOG_MAXLENGTH;

#pragma pack(push,1)
struct MachLogCreateActorMessage
{
	MachLogMessageHeader	header_;
	MachPhys::Race			race_;
	MachLog::ObjectType		objectType_;
	int						subType_;
	int						hwLevel_;
	int						swLevel_;
	MexCompressedPoint3d	position_;
	MexRadians				orientation_;
	MachPhys::WeaponCombo	weaponCombo_;
	UtlId					whichId_;
};

struct MachLogCreateSpecialActorMessage
{
	MachLogMessageHeader	    header_;
	MachPhys::Race			    race_;
	MachLog::ObjectType		    objectType_;
	int						    quantity_;
	MexCompressedPoint3d	    position_;
	MexCompressedAlignedBox2d   boundary_;
	UtlId					    whichId_;
};


struct MachLogSingleMoveInfo
{
	MexCompressedTransform3d 			startTransform_;
    MexCompressedPoint3d				endPosition_;
	float								startSpeed_;
	float								startTime_;
    MachPhys::FinalState    			finalState_;
};

const int MachLogMachineMoveMessageDataSize = MACHLOG_MAXLENGTH - sizeof( UtlId ) - sizeof( int ) - sizeof( float );
struct MachLogMachineMoveMessage
{
	MachLogMessageHeader	header_;
	UtlId					whichId_;
	float					topSpeed_;
	int						nMoveInfos_;
	//the array data will be an array of MLSingleMoveInfo
	char					data_[ MachLogMachineMoveMessageDataSize ];
	//this expression MUST return same as above value
	static const int dataSize() { return MachLogMachineMoveMessageDataSize; }
};

struct MachLogMachineMoveData
{
	MexCompressedTransform3d			position_;
	float								speed_;
};


struct MachLogMachineSetDomainMessage
{
	MachLogMessageHeader				header_;
	UtlId								whichId_;
	MexCompressedTransform3d			transform_;
};

struct MachLogStopDeadMessage
{
	MachLogMessageHeader				header_;
	UtlId								whichId_;
	MexCompressedTransform3d			transform_;
};

struct MachLogRestingObstacleMessage
{
	MachLogMessageHeader	    header_;
	UtlId					    whichId_;
	MexCompressedTransform3d	transform_;
	int						    status_;
};

struct MachLogRestingObstacleShortMessage
{
	MachLogMessageHeader	header_;
	UtlId					whichId_;
};

struct MachLogClearRestingObstacleMessage
{
	MachLogMessageHeader	header_;
	UtlId					whichId_;
};

struct MachLogMovingObstacleMessage
{
	MachLogMessageHeader	header_;
	UtlId					whichId_;
};

struct MachLogMachineTurnMessage
{
	MachLogMessageHeader	header_;
	UtlId					whichId_;
	MexRadians				turnAmount_;
};

struct MachLogReadyMessage
{
	MachLogMessageHeader	header_;
	MachPhys::Race			race_;
};

struct MachLogResyncTimeMessage
{
	MachLogMessageHeader	header_;
	PhysAbsoluteTime		now_;
};

struct MachLogBeHitMessage
{
	MachLogMessageHeader				header_;
	UtlId								whichId_;
	int									damage_;
	MachPhys::WeaponType				weaponType_;
	MachLogMessageBroker::Validity		byWhichIdValid_;
	UtlId								byWhichId_;
	MachLogMessageBroker::Validity		lineValid_;
	MexCompressedLine3d					byDirection_;
	float								physicalDelay_;
	MachLogMessageBroker::BeHitType		beHitType_;
	MachLogMessageBroker::ActorNowDead	actorNowDead_;
};

struct MachLogAdjustHPAndArmourMessage
{
	MachLogMessageHeader	header_;
	UtlId					whichId_;
	int						hp_;
	int						armour_;
};
const int MachLogMotionChunksMessageDataSize = MACHLOG_MAXLENGTH - sizeof( UtlId ) - 2 * sizeof( int );
struct MachLogMotionChunksMessage
{
	MachLogMessageHeader	header_;
	UtlId					whichId_;
	int						status_;
	int						nChunks_;
	char					data_[ MachLogMotionChunksMessageDataSize ];
	static const int dataSize() { return MachLogMotionChunksMessageDataSize; }
};

struct MachLogMotionChunksAddFailedMessage
{
	MachLogMessageHeader	    header_;
	UtlId					    whichId_;
	int						    status_;
	MexCompressedTransform3d    transform_;
};

struct MachLogFireWeaponAnimationMessage
{
	MachLogMessageHeader	header_;
	UtlId					whichId_;
	MachPhys::Mounting		mounting_;
	int						numberInBurst_;
};

struct MachLogEchoLinearProjectileMessage
{
	MachLogMessageHeader	header_;
	UtlId					whichId_;
	MachPhys::Mounting		mounting_;
	UtlId					targetId_;
	PhysAbsoluteTime		launchTime_;
	MexCompressedPoint3d				targetOffset_;	//nb - targetOffset_ is dual purpose - inaccuracy-laced target offset
											// when firing at actors, local tile offset when firing at terrain
	int						nRounds_;
	char					arrayData_[ 512 ];

};

struct MachLogProjectileDestructionData
{
	float					destroyAtTime_;
	MachPhys::StrikeType	strikeType_;
};


struct MachLogUpdateCompletionVisualisationMessage
{
	MachLogMessageHeader	header_;
	UtlId					whichId_;
	double					percentage_;
};

struct MachLogCreateSpecialWeaponEffectMessage
{
	MachLogMessageHeader	header_;
	MexCompressedPoint3d	position_;
	MachPhys::WeaponType	weaponType_;
	UtlId					whichId_;
};

struct MachLogApplySpecialWeaponAnimationMessage
{
	MachLogMessageHeader	header_;
	UtlId					whichId_;
	MachPhys::WeaponType	weaponType_;
};

struct MachLogPlayNormalObjectAnimationMessage
{
	MachLogMessageHeader	header_;
	UtlId					whichId_;
	bool					onOrOff_;
};

struct MachLogPlayConstructionAnimationMessage
{
	MachLogMessageHeader	header_;
	UtlId					constructorId_;
	UtlId					constructionId_;
	bool					onOrOff_;
};


struct MachLogUseSpaceDomainMessage
{
	MachLogMessageHeader	header_;
	UtlId					whichId_;
	UtlId					constructionId_;
};

struct MachLogSetSiteDiscoveredByMessage
{
	MachLogMessageHeader	header_;
	MexCompressedPoint3d	position_;
	MachPhys::Race			race_;
};

struct MachLogNewResearchMessage
{
	MachLogMessageHeader	header_;
	MachLog::ObjectType		objectType_;
	int						subType_;
	int						hwLevel_;
	MachPhys::WeaponCombo	weaponCombo_;
	MachPhys::Race			race_;
};

struct MachLogHealMessage
{
	MachLogMessageHeader	header_;
	bool					garrisonHealing_;
	UtlId					whichId_;
	UtlId					targetId_;
	MachLogMessageBroker::HealingFlag	healingFlag_;
};

struct MachLogRemoveConstructionMessage
{
	MachLogMessageHeader	header_;
	UtlId					whichId_;
};

struct MachLogChangeRaceMessage
{
	MachLogMessageHeader	header_;
	UtlId					whichId_;
	MachPhys::Race			newRace_;
};

struct MachLogLoadArtefactModelsMessage
{
	MachLogMessageHeader	header_;
	char					arrayData_[ 512 ];
};

struct MachLogAddNewArtefactSubTypeMessage
{
	MachLogMessageHeader			header_;
	int 							subType_;
	MachPhys::BuildingMaterialUnits cost_;
	MachPhys::ArmourUnits 			armour_;
	MachPhys::HitPointUnits 		hitPoints_;
	uint 							stringId_;
	MATHEX_SCALAR 					height_;
	int								boundaryPoints_;
	//the boundary data will then be encoded inside the arrayData and followed by the name.
	char							arrayData_[ 512 ];
};

struct MachLogAddArtefactDamageLinkMessage
{
	MachLogMessageHeader			header_;
	UtlId							destroyId_;
	UtlId							damageId_;
	int								hpModifier_;
	int								armourModifier_;
};

struct MachLogCamouflageMachineMessage
{
	MachLogMessageHeader	header_;
	UtlId					whichId_;
	MachLogMessageBroker::CamouflageFlag	camouflageFlag_;
};

struct MachLogCurrentTargetMessage
{
	MachLogMessageHeader	header_;
	UtlId					whichId_;
	UtlId					targetId_;
	MachLogMessageBroker::Validity	targetIdValidFlag_;
};

struct MachLogFirstPersonEndMessage
{
	MachLogMessageHeader	header_;
	UtlId					whichId_;
    MachPhys::Race          race_;
    bool                    open_;
};

struct MachLogFirstPersonUpdateMessage
{
	MachLogMessageHeader	header_;
	UtlId					whichId_;
    MachPhys::Race          race_;
    MachPhysFirstPersonStateVector state_;
};

struct MachLogEnteredBuildingMessage
{
	MachLogMessageHeader	header_;
	UtlId					whichId_;
	UtlId					constructionId_;
};

struct MachLogSetSiteOreMessage
{
	MachLogMessageHeader				header_;
	int									siteId_;
	MachPhys::BuildingMaterialUnits		newOreAmount_;
};

struct MachLogWeaponInformationMessage
{
	MachLogMessageHeader					header_;
	MachLogMessageBroker::WeaponInfoFlag	weaponInfoFlag_;
	MachPhys::Race							race_;
};

struct MachLogEchoNewScoreMessage
{
	MachLogMessageHeader					header_;
	MachLogScoreImpl						scoreImpl_;
};

struct MachLogActorThreatMessage
{
	MachLogMessageHeader					header_;
	UtlId									whichId_;
	UtlId									threatId_;
	MachLogMessageBroker::ThreatFlag		threatFlag_;
};

struct MachLogChangeOfDispositionMessage
{
	MachLogMessageHeader					header_;
	MachPhys::Race							checkRace_;
	MachPhys::Race							targetRace_;
	int										disposition_;
};

struct MachLogSmeltMachineMessage
{
	MachLogMessageHeader					header_;
	UtlId									whichId_;
};

struct MachLogAdvanceConstructionStateMessage
{
	MachLogMessageHeader					header_;
	UtlId									whichId_;
	MachPhys::BuildingMaterialUnits			addAmount_;
};

struct MachLogAddRepairPointsMessage
{
	MachLogMessageHeader					header_;
	UtlId									whichId_;
	MachPhys::HitPointUnits					addAmount_;
};

struct MachLogMakeCompleteConstructionMessage
{
	MachLogMessageHeader					header_;
	UtlId									whichId_;
};

struct MachLogIsEntranceOpenMessage
{
	MachLogMessageHeader					header_;
	UtlId									whichId_;
	uint8									door_;
	uint8									doOpen_;
};
#pragma pack(pop)

ostream& operator <<( ostream& o, const MachLogCreateActorMessage& t );
ostream& operator <<( ostream& o, const MachLogCreateSpecialActorMessage& t );
ostream& operator <<( ostream& o, const MachLogMachineMoveMessage& t );
ostream& operator <<( ostream& o, const MachLogBeHitMessage& t );
ostream& operator <<( ostream& o, const MachLogMessageHeader& t );
ostream& operator <<( ostream& o, const MachLogSingleMoveInfo& t );



#endif
