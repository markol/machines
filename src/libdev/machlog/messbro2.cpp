#include "machlog/messbrok.hpp"

#include "machlog/internal/messages.hpp"

#include "ctl/vector.hpp"

#include "mathex/angle.hpp"
#include "mathex/point2d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/poly2d.hpp"

#include "phys/rampacce.hpp"

#include "network/message.hpp"
#include "network/session.hpp"
#include "network/nodeuid.hpp"

#include "world4d/garbcoll.hpp"

#include "system/pathname.hpp"

#include "sim/manager.hpp"


#include "machphys/beebombe.hpp"
#include "machphys/consdata.hpp"
#include "machphys/constron.hpp"
#include "machphys/ionbeam.hpp"
#include "machphys/ionweap.hpp"
#include "machphys/machine.hpp"
#include "machphys/mcmovinf.hpp"
#include "machphys/nuclbomb.hpp"
#include "machphys/treachry.hpp"
#include "machphys/vortbomb.hpp"
#include "machphys/vortweap.hpp"

#include "machlog/actmaker.hpp"
#include "machlog/artfacts.hpp"
#include "machlog/beebomex.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/canattac.hpp"
#include "machlog/constron.hpp"
#include "machlog/construc.hpp"
#include "machlog/debris.hpp"
#include "machlog/geolocat.hpp"
#include "machlog/ionbeam.hpp"
#include "machlog/machine.hpp"
#include "machlog/mcmotseq.hpp"
#include "machlog/missilem.hpp"
#include "machlog/minesite.hpp"
#include "machlog/network.hpp"
#include "machlog/nuclbomb.hpp"
#include "machlog/plandoms.hpp"
#include "machlog/planet.hpp"
#include "machlog/punweap.hpp"
#include "machlog/races.hpp"
#include "machlog/rescarr.hpp"
#include "machlog/restree.hpp"
#include "machlog/resitem.hpp"
#include "machlog/technici.hpp"
#include "machlog/vmman.hpp"
#include "machlog/vortbomb.hpp"
#include "machlog/weapon.hpp"
#include "machlog/wepline.hpp"
#include "machlog/score.hpp"
#include "machlog/internal/scorei.hpp"

void MachLogMessageBroker::sendBeHitMessage( UtlId whichId, int damage, MachPhys::WeaponType wt, MachActor* pByActor, MexLine3d* pLine, PhysRelativeTime timeOffset, MachLogMessageBroker::BeHitType type, MachLogMessageBroker::ActorNowDead actorNowDead )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendBeHitMessage " << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogBeHitMessage* pMessage = _REINTERPRET_CAST( MachLogBeHitMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = BE_HIT_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogBeHitMessage );
	pMessage->whichId_ = whichId;
	pMessage->damage_ = damage;
	pMessage->weaponType_ = wt;
	if( pByActor == NULL )
		pMessage->byWhichIdValid_ = INVALID;
	else
	{
		pMessage->byWhichIdValid_ = VALID;
		pMessage->byWhichId_ = pByActor->id();
	}

	if( pLine == NULL )
		pMessage->lineValid_ = INVALID;
	else
	{
		DEBUG_STREAM( DIAG_NETWORK,"pointer to line != NULL so dereferencing\n" );
		pMessage->lineValid_ = VALID;
		pMessage->byDirection_ = *pLine;
	}
	pMessage->physicalDelay_ = timeOffset;
	pMessage->beHitType_ = type;
	pMessage->actorNowDead_ = actorNowDead;
	DEBUG_STREAM( DIAG_NETWORK, *pMessage << std::endl );
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendBeHitMessage DONE " << std::endl );
}

void MachLogMessageBroker::processBeHitMessage( NetMessage* pNetMessage )
{
	//MachLogBeHitMessage* pMessage = _REINTERPRET_CAST( MachLogBeHitMessage*, &pNetMessage->body().body()[0] );
	MachLogBeHitMessage* pMessage = _REINTERPRET_CAST( MachLogBeHitMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	NETWORK_ERRORS_STREAM("MachLogMessageBroker::processBeHitMessage " << pMessage->whichId_ << std::endl );
	DEBUG_STREAM( DIAG_NETWORK,"processBeHitMessage " << pMessage->whichId_ << std::endl );
	DEBUG_STREAM( DIAG_NETWORK, *pMessage << std::endl );
	if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
	{
		MachActor& actor = MachLogRaces::instance().actor( pMessage->whichId_ );
		//MexLine3d* pLine = NULL;
		MexLine3d line;
		if( pMessage->lineValid_ == VALID )
		{
			//pLine = &MexLine3d( pMessage->byDirection_ );
			line = MexLine3d( pMessage->byDirection_ );
		}
		MachActor* pByActor = NULL;
		if( pMessage->byWhichIdValid_ == VALID )
		{
			//the following assertion is NOT true! due to sending order on the originating machine.
			//ASSERT( MachLogRaces::instance().actorExists( pMessage->byWhichId_ ), "Actor does not exist" );
			if( MachLogRaces::instance().actorExists( pMessage->byWhichId_ ) )
				pByActor = &MachLogRaces::instance().actor( pMessage->byWhichId_ );
		}
		DEBUG_STREAM( DIAG_NETWORK,"call whichever beHit Method\n" );
		//if the actor is supposed to die then make sure it does otherwise we get funnies occuring.
		if( pMessage->actorNowDead_ == ACTOR_DEAD and not actor.willHitCauseDeath( pMessage->damage_ ) )
			pMessage->damage_ = 10000;
		//of course - if the actor isn't supposed to die then we also have to trap for that as well.
		else if( pMessage->actorNowDead_ == ACTOR_NOT_DEAD and actor.willHitCauseDeath( pMessage->damage_ ) )
		{
			pMessage->damage_ = actor.hp() + actor.armour() - 1;
			//check that we have done enough
			//e.g. if actor has 1hp and 1ap and damage =1 then it would die on this hit - so make damage reduce to zero
			//in limiting case
			while( actor.willHitCauseDeath( pMessage->damage_ ) and pMessage->damage_ > 0 )
				pMessage->damage_ -= 1;
		}
		if( pMessage->damage_ > 0 )
		{
			if( pMessage->beHitType_ == NORMAL_HIT )
				//actor.beHit( pMessage->damage_, pMessage->weaponType_, pByActor, pLine, MachActor::NO_ECHO );
				actor.beHit( pMessage->damage_, pMessage->weaponType_, pByActor, &line, MachActor::NO_ECHO );
			else
				actor.beHitWithoutAnimation( pMessage->damage_, pMessage->physicalDelay_, pByActor, MachActor::NO_ECHO );
		}
		if( pMessage->actorNowDead_ == ACTOR_DEAD and not actor.isDead() )
		{
			NETWORK_STREAM("############\nFAILURE TO KILL ACTOR WHEN ASKED::\n" );
			NETWORK_INDENT( 2 );
			NETWORK_STREAM( actor );
			NETWORK_INDENT( -2 );
			NETWORK_STREAM("################" << std::endl );
			NETWORK_ERRORS_STREAM("############\nFAILURE TO KILL ACTOR WHEN ASKED::\n" );
			NETWORK_ERRORS_INDENT( 2 );
			NETWORK_ERRORS_STREAM( actor );
			NETWORK_ERRORS_INDENT( -2 );
			NETWORK_ERRORS_STREAM("################" << std::endl );
		}
	}
	else
	{
		NETWORK_ERRORS_STREAM("NW01: that actor is not present in the map\n" );
	}
}


void MachLogMessageBroker::sendFireWeaponAnimationMessage( UtlId whichId, MachPhys::Mounting mounting, int numberInBurst )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendFireWeaponAnimationMessage " << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogFireWeaponAnimationMessage* pMessage = _REINTERPRET_CAST( MachLogFireWeaponAnimationMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = FIRE_WEAPON_ANIMATION_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogFireWeaponAnimationMessage );
	pMessage->whichId_ = whichId;
	pMessage->mounting_ = mounting;
	pMessage->numberInBurst_ = numberInBurst;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendFireWeaponAnimationMessage DONE " << std::endl );
}

void MachLogMessageBroker::processFireWeaponAnimationMessage( NetMessage* pNetMessage )
{
	//MachLogFireWeaponAnimationMessage* pMessage = _REINTERPRET_CAST( MachLogFireWeaponAnimationMessage*, &pNetMessage->body().body()[0] );
	MachLogFireWeaponAnimationMessage* pMessage = _REINTERPRET_CAST( MachLogFireWeaponAnimationMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processFireWeaponAnimationMessage " << pMessage->whichId_ << std::endl );
//	DEBUG_STREAM( DIAG_NETWORK, *pMessage << std::endl );
	ASSERT( MachLogRaces::instance().actorExists( pMessage->whichId_ ), "Actor does not exist" );
	if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
	{
		DEBUG_STREAM( DIAG_NETWORK,"processFireWeaponAnimationMessage " << pMessage->whichId_ << std::endl );
		MachActor& actor = MachLogRaces::instance().actor( pMessage->whichId_ );
		if( actor.objectIsCanAttack() )
		{
			MachLogCanAttack& canAttack = actor.asCanAttack();
			MachLogCanAttack::Weapons& weapons = _CONST_CAST( MachLogCanAttack::Weapons&, _CONST_CAST( const MachLogCanAttack&, canAttack).weapons() );
//			MachLogCanAttack::Weapons& weapons = _CONST_CAST( MachLogCanAttack::Weapons&, cweapons );
			for( MachLogCanAttack::Weapons::iterator i = weapons.begin(); i != weapons.end(); ++i )
				if( (*i)->mounting() == pMessage->mounting_ )
				{
					if( (*i)->type() == MachPhys::GORILLA_PUNCH )
						_STATIC_CAST( MachLogPunchWeapon*, (*i) )->doEchoPunch();
					else
						(*i)->fireWithoutEcho( pMessage->numberInBurst_ );
					return;
				}
		}
	}
}

void MachLogMessageBroker::sendUpdateCompletionVisualisationMessage( UtlId whichId, double percentage )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendUpdateCompletionVisualisationMessage\n" );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendUpdateCompletionVisualisationMessage " << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogUpdateCompletionVisualisationMessage* pMessage = _REINTERPRET_CAST( MachLogUpdateCompletionVisualisationMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = UPDATE_COMPLETION_VISUALISATION_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogUpdateCompletionVisualisationMessage );
	pMessage->whichId_ = whichId;
	pMessage->percentage_ = percentage;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendUpdateCompletionVisualisationMessage DONE " << std::endl );
}

void MachLogMessageBroker::processUpdateCompletionVisualisationMessage( NetMessage* pNetMessage )
{
	//MachLogUpdateCompletionVisualisationMessage* pMessage = _REINTERPRET_CAST( MachLogUpdateCompletionVisualisationMessage*, &pNetMessage->body().body()[0] );
	MachLogUpdateCompletionVisualisationMessage* pMessage = _REINTERPRET_CAST( MachLogUpdateCompletionVisualisationMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processUpdateCompletionVisualisationMessage " << pMessage->whichId_ << std::endl );
//	DEBUG_STREAM( DIAG_NETWORK, *pMessage << std::endl );
	ASSERT_INFO( pMessage->whichId_ );
	ASSERT( MachLogRaces::instance().actorExists( pMessage->whichId_ ), "Actor does not exist" );
	if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
	{
		DEBUG_STREAM( DIAG_NETWORK,"processUpdateCompletionVisualisationMessage " << pMessage->whichId_ << std::endl );
		MachActor& actor = MachLogRaces::instance().actor( pMessage->whichId_ );
		if( actor.objectIsConstruction() )
		{
			MachLogConstruction& construction = actor.asConstruction();
			construction.updateCompletionVisualisation( pMessage->percentage_ );
		}
	}
}

void MachLogMessageBroker::sendEchoLinearProjectileMessage( UtlId whichId, MachPhys::Mounting mounting, UtlId targetId, const PhysAbsoluteTime& launchTime, int nRounds, const MachLogMessageBroker::ProjectileDestroyData& extraData, const MexPoint3d& targetOffset )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendEchoLinearProjectileMessage\n" );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendEchoLinearProjectileMessage " << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogEchoLinearProjectileMessage* pMessage = _REINTERPRET_CAST( MachLogEchoLinearProjectileMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = ECHO_LINEAR_PROJECTILE_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogEchoLinearProjectileMessage ) - 512 + nRounds * sizeof(MachLogProjectileDestructionData);
	pMessage->whichId_ = whichId;
	pMessage->mounting_ = mounting;
	pMessage->targetId_ = targetId;
	pMessage->launchTime_ = launchTime;
	pMessage->nRounds_ = nRounds;
	pMessage->targetOffset_ = targetOffset;
	DEBUG_STREAM( DIAG_NETWORK, "Number of rounds " << nRounds << " extraData.size " << extraData.size() << std::endl );
	//add on the more complex extra data information.
	for( int i = 0; i < nRounds; ++i )
	{
		DEBUG_STREAM( DIAG_NETWORK,"processing round " << i << std::endl );
		MachLogProjectileDestructionData spdd;
		spdd.destroyAtTime_ = extraData[i]->destroyAtTime_;
		spdd.strikeType_ = extraData[i]->strikeType_;
		DEBUG_STREAM( DIAG_NETWORK, "spdd.destroyAtTime_ " << spdd.destroyAtTime_ << std::endl );
		DEBUG_STREAM( DIAG_NETWORK, "spdd.strikeType_ " << (int)spdd.strikeType_ << std::endl );
		DEBUG_STREAM( DIAG_NETWORK, "going to copy to address " << (void*)(pMessage->arrayData_[0] + i * sizeof(MachLogProjectileDestructionData) ) << std::endl );
		DEBUG_STREAM( DIAG_NETWORK, "arrayData_[0] is at " << &pMessage->arrayData_[0] << std::endl );
		memcpy( (void*)(&pMessage->arrayData_[0] + i * sizeof(MachLogProjectileDestructionData) ), (void*)&spdd, sizeof(MachLogProjectileDestructionData) );
	}
	DEBUG_STREAM( DIAG_NETWORK," doSend\n" );
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendEchoLinearProjectileMessage DONE " << std::endl );
}

void MachLogMessageBroker::processEchoLinearProjectileMessage( NetMessage* pNetMessage )
{
	//MachLogEchoLinearProjectileMessage* pMessage = _REINTERPRET_CAST( MachLogEchoLinearProjectileMessage*, &pNetMessage->body().body()[0] );
	MachLogEchoLinearProjectileMessage* pMessage = _REINTERPRET_CAST( MachLogEchoLinearProjectileMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processEchoLinearProjectileMessage " << pMessage->whichId_ << std::endl );
//	DEBUG_STREAM( DIAG_NETWORK, *pMessage << std::endl );
	ASSERT( MachLogRaces::instance().actorExists( pMessage->whichId_ ), "Actor does not exist" );

	UtlId targetID = ( pMessage->targetId_ );
	MachActor* pTarget = NULL;

	int nRounds = pMessage->nRounds_;

	if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) and
		( targetID == 0  or  MachLogRaces::instance().actorExists( targetID ) ) )
	{
		DEBUG_STREAM( DIAG_NETWORK,"processEchoLinearProjectileMessage " << pMessage->whichId_ << std::endl );
		MachActor& actor = MachLogRaces::instance().actor( pMessage->whichId_ );

        if( targetID )
    		pTarget = &( MachLogRaces::instance().actor( targetID ) );

        PhysAbsoluteTime launchTime = pMessage->launchTime_;

		if( actor.objectIsCanAttack() )
		{
			MachLogMessageBroker::ProjectileDestroyData extraData;
			for( int i = 0; i < nRounds; ++i )
			{
				MachLogProjectileDestructionData spdd;
				SingleProjectileDestroyData* pSpdd = _NEW( SingleProjectileDestroyData );
				memcpy( (void*)&spdd, (void*)(&pMessage->arrayData_[0] + i * sizeof(MachLogProjectileDestructionData) ), sizeof(MachLogProjectileDestructionData) );
				pSpdd->destroyAtTime_ = spdd.destroyAtTime_;
				pSpdd->strikeType_ = spdd.strikeType_;
				extraData.push_back( pSpdd );
			}
			MachLogCanAttack& canAttack = actor.asCanAttack();
			MachLogCanAttack::Weapons& weapons = _CONST_CAST( MachLogCanAttack::Weapons&, _CONST_CAST( const MachLogCanAttack&, canAttack).weapons() );
//			MachLogCanAttack::Weapons& weapons = _CONST_CAST( MachLogCanAttack::Weapons&, cweapons );
			for( MachLogCanAttack::Weapons::iterator i = weapons.begin(); i != weapons.end(); ++i )
				if( (*i)->mounting() == pMessage->mounting_ )
				{
					MachLogLinearWeapon* pLineWeap = _REINTERPRET_CAST( MachLogLinearWeapon*, *i );
					switch( (*i)->type() )
					{
						case MachPhys::NUCLEAR_MISSILE:
						{
							// special event voicemail may be needed. Note that this drops through into following cases.
							//MachPhys::Race myRace = MachLogRaces::instance().pcController().race();
							//if( MachLogRaces::instance().beaconType( myRace ) == MachLog::LEVEL_3_BEACON )
								MachLogVoiceMailManager::instance().postNewMail( VID_POD_ENEMY_NUKE_LAUNCH, MachLogRaces::instance().pcController().race() );
						}
						case MachPhys::HOMING_MISSILE:
				        case MachPhys::LARGE_MISSILE:
				        case MachPhys::MULTI_LAUNCHER1:
				        case MachPhys::MULTI_LAUNCHER2:
				        case MachPhys::MULTI_LAUNCHER3:
				        case MachPhys::MULTI_LAUNCHER4:
				        case MachPhys::MULTI_LAUNCHER5:
				        case MachPhys::MULTI_LAUNCHER6:
				        case MachPhys::MULTI_LAUNCHER7:

							if( targetID == 0 )
							{
								// target is in fact just a terrain tile - call special doEchoFireForTerrain case
								pLineWeap->doEchoFireForTerrain( pMessage->targetOffset_, launchTime, nRounds, extraData, MachLogLinearWeapon::CREATE_HOLDER );
							}
							else
							{
								ASSERT( pTarget, "pTarget should not be NULL here." );
								pLineWeap->doEchoFire( pTarget, pMessage->targetOffset_, launchTime, nRounds, extraData, MachLogLinearWeapon::CREATE_HOLDER );
							}
							break;
						default:
							if( targetID == 0 )
							{
								// target is in fact just a terrain tile - call special doEchoFireForTerrain case
								pLineWeap->doEchoFireForTerrain( pMessage->targetOffset_, launchTime, nRounds, extraData, MachLogLinearWeapon::DO_NOT_CREATE_HOLDER );
							}
							else
							{
								ASSERT( pTarget, "pTarget should not be NULL here." );
								pLineWeap->doEchoFire( pTarget, pMessage->targetOffset_, launchTime, nRounds, extraData, MachLogLinearWeapon::DO_NOT_CREATE_HOLDER );
							}
							break;
					}
					return;
				}
			while( extraData.size() )
			{
				_DELETE( extraData.back() );
				extraData.erase( extraData.begin() + extraData.size() - 1 );
			}
		}
	}
}

void MachLogMessageBroker::sendCreateSpecialWeaponEffectMessage( const MexPoint3d& position, MachPhys::WeaponType weaponType, UtlId	whichId )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendCreateSpecialWeaponEffectMessage\n" );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendCreateSpecialWeaponEffectMessage " << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogCreateSpecialWeaponEffectMessage* pMessage = _REINTERPRET_CAST( MachLogCreateSpecialWeaponEffectMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = CREATE_SPECIAL_WEAPON_EFFECT_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogCreateSpecialWeaponEffectMessage );
	pMessage->position_ = position;
	pMessage->weaponType_ = weaponType;
	pMessage->whichId_ = whichId;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendCreateSpecialWeaponEffectMessage DONE " << std::endl );
}

void MachLogMessageBroker::processCreateSpecialWeaponEffectMessage( NetMessage* pNetMessage )
{
	//MachLogCreateSpecialWeaponEffectMessage* pMessage = _REINTERPRET_CAST( MachLogCreateSpecialWeaponEffectMessage*, &pNetMessage->body().body()[0] );
	MachLogCreateSpecialWeaponEffectMessage* pMessage = _REINTERPRET_CAST( MachLogCreateSpecialWeaponEffectMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processCreateSpecialWeaponEffectMessage " << pMessage->weaponType_ << " " << pMessage->position_ << std::endl );
	switch( pMessage->weaponType_ )
	{
		case MachPhys::NUCLEAR_MISSILE:
		{
			MachPhysNuclearBomb* pPhysNuclearBomb;
			MachLogNuclearBomb::pNewPhysNuclearBomb( pMessage->position_, &pPhysNuclearBomb );
			PhysRelativeTime relTime = pPhysNuclearBomb->startExplosion( SimManager::instance().currentTime(), MachLogPlanet::instance().surface() );
			W4dGarbageCollector::instance().add( pPhysNuclearBomb, SimManager::instance().currentTime() + relTime );
		}
		break;
		case MachPhys::VORTEX:
		{
			MachPhysVortexBomb* pPhysVortexBomb;
			MachLogVortexBomb::pNewPhysVortexBomb( pMessage->position_, pMessage->whichId_, &pPhysVortexBomb );
			PhysRelativeTime relTime = pPhysVortexBomb->startExplosion( SimManager::instance().currentTime() );
			W4dGarbageCollector::instance().add( pPhysVortexBomb, SimManager::instance().currentTime() + relTime );
		}
		break;
		case MachPhys::ION_ORBITAL_CANNON:
		{
			MachPhysIonBeam* pPhysIonBeam;
			MachLogIonBeam::pNewPhysIonBeam( pMessage->position_, &pPhysIonBeam );
			PhysRelativeTime relTime = pPhysIonBeam->startExplosion( SimManager::instance().currentTime(), *MachLogPlanet::instance().surface() );
			W4dGarbageCollector::instance().add( pPhysIonBeam, SimManager::instance().currentTime() + relTime );
		}
		case MachPhys::BEE_BOMB:
		{
			MachPhysBeeBombExplosion* pPhysBeeBombExplosion;
			MachLogBeeBombExplosion::pNewPhysBeeBombExplosion( pMessage->position_, &pPhysBeeBombExplosion );
			PhysRelativeTime relTime = pPhysBeeBombExplosion->startExplosion( SimManager::instance().currentTime(), *MachLogPlanet::instance().surface() );
			W4dGarbageCollector::instance().add( pPhysBeeBombExplosion, SimManager::instance().currentTime() + relTime );
		}
	}
}

void MachLogMessageBroker::sendApplySpecialWeaponAnimationMessage( UtlId whichId, MachPhys::WeaponType weaponType )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendApplySpecialWeaponAnimationMessage\n" );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendApplySpecialWeaponAnimationMessage " << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogApplySpecialWeaponAnimationMessage* pMessage = _REINTERPRET_CAST( MachLogApplySpecialWeaponAnimationMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = APPLY_SPECIAL_WEAPON_ANIMATION_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogApplySpecialWeaponAnimationMessage );
	pMessage->whichId_ = whichId;
	pMessage->weaponType_ = weaponType;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendApplySpecialWeaponAnimationMessage DONE " << std::endl );
}

void MachLogMessageBroker::processApplySpecialWeaponAnimationMessage( NetMessage* pNetMessage )
{
	//MachLogApplySpecialWeaponAnimationMessage* pMessage = _REINTERPRET_CAST( MachLogApplySpecialWeaponAnimationMessage*, &pNetMessage->body().body()[0] );
	MachLogApplySpecialWeaponAnimationMessage* pMessage = _REINTERPRET_CAST( MachLogApplySpecialWeaponAnimationMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processApplySpecialWeaponAnimationMessage " << pMessage->weaponType_ << std::endl );
	//ASSERT( MachLogRaces::instance().actorExists( pMessage->whichId_ ), "Actor does not exist" );
	if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
	{
		MachActor& actor = MachLogRaces::instance().actor( pMessage->whichId_ );
		const MachActor& cactor = _CONST_CAST( const MachActor&, actor );
		switch( pMessage->weaponType_ )
		{
			case MachPhys::VORTEX:
			{
				MachPhysVortexWeapon::destroy( &actor.physObject() , SimManager::instance().currentTime() );
			}
			break;
			case MachPhys::ION_ORBITAL_CANNON:
			{
				MachPhysIonWeapon::destroy( &actor.physObject() , SimManager::instance().currentTime() );
			}
			break;
		}
	}
}

void MachLogMessageBroker::sendPlayNormalObjectAnimationMessage( UtlId whichId, bool onOrOff )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendPlayNormalObjectAnimationMessage\n" );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogPlayNormalObjectAnimationMessage* pMessage = _REINTERPRET_CAST( MachLogPlayNormalObjectAnimationMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = PLAY_NORMAL_OBJECT_ANIMATION_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogPlayNormalObjectAnimationMessage );
	pMessage->whichId_ = whichId;
	pMessage->onOrOff_ = onOrOff;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendPlayNormalObjectAnimationMessage DONE " << std::endl );
}

void MachLogMessageBroker::processPlayNormalObjectAnimationMessage( NetMessage* pNetMessage )
{
	//MachLogPlayNormalObjectAnimationMessage* pMessage = _REINTERPRET_CAST( MachLogPlayNormalObjectAnimationMessage*, &pNetMessage->body().body()[0] );
	MachLogPlayNormalObjectAnimationMessage* pMessage = _REINTERPRET_CAST( MachLogPlayNormalObjectAnimationMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processPlayNormalObjectAnimationMessage " << pMessage->whichId_ << std::endl );
	//ASSERT( MachLogRaces::instance().actorExists( pMessage->whichId_ ), "Actor does not exist" );
	if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
	{
		MachActor& actor = MachLogRaces::instance().actor( pMessage->whichId_ );
		MachLog::ObjectType ot = actor.objectType();
		if( actor.objectIsConstruction() )
		{
			MachPhysConstruction* pPhysConstruction = &actor.asConstruction().physConstruction();
			pPhysConstruction->isWorking( pMessage->onOrOff_ );
		}
		else
			switch( ot )
			{
				case MachLog::GEO_LOCATOR:
					actor.asGeoLocator().isLocating( pMessage->onOrOff_ );
					break;
				case MachLog::TECHNICIAN:
					actor.asTechnician().research();
					break;
				case MachLog::RESOURCE_CARRIER:
					actor.asResourceCarrier().doLoading();
			}
	}
}

void MachLogMessageBroker::sendPlayConstructAnimationMessage( UtlId constructorId, UtlId constructionId, bool onOrOff )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendPlayNormalObjectAnimationMessage\n" );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogPlayConstructionAnimationMessage* pMessage = _REINTERPRET_CAST( MachLogPlayConstructionAnimationMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = PLAY_CONSTRUCT_ANIMATION_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogPlayConstructionAnimationMessage );
	pMessage->constructorId_ = constructorId;
	pMessage->constructionId_ = constructionId;
	pMessage->onOrOff_ = onOrOff;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendPlayNormalObjectAnimationMessage DONE " << std::endl );
}

void MachLogMessageBroker::processPlayConstructAnimationMessage( NetMessage* pNetMessage )
{
	//MachLogPlayConstructionAnimationMessage* pMessage = _REINTERPRET_CAST( MachLogPlayConstructionAnimationMessage*, &pNetMessage->body().body()[0] );
	MachLogPlayConstructionAnimationMessage* pMessage = _REINTERPRET_CAST( MachLogPlayConstructionAnimationMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"MachLogPlayConstructionAnimationMessage " << pMessage->constructorId_ << std::endl );
	//ASSERT( MachLogRaces::instance().actorExists( pMessage->constructorId_ ), "Actor does not exist" );

	if( MachLogRaces::instance().actorExists( pMessage->constructorId_ )
		and ( pMessage->onOrOff_ == false
			  or MachLogRaces::instance().actorExists( pMessage->constructionId_ ) ) )
	{
		MachActor& constructorActor = MachLogRaces::instance().actor( pMessage->constructorId_ );
		ASSERT( constructorActor.objectType() == MachLog::CONSTRUCTOR, "constructorId_ should be the ID of a constructor." );

		if( pMessage->onOrOff_ == false )
			constructorActor.asConstructor().constructing( false );
		else
		{
			ASSERT_DATA( MachActor& constructionActor = MachLogRaces::instance().actor( pMessage->constructionId_ ) );
			ASSERT( constructionActor.objectIsConstruction(), "constructionId_ should be the ID of a construction." );

			constructorActor.asConstructor().constructing( true, pMessage->constructionId_ );
		}
	}
}

void MachLogMessageBroker::sendWeaponInformationMessage( WeaponInfoFlag weaponInfoFlag, MachPhys::Race race )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendWeaponInformationMessage\n" );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendWeaponInformationMessage " << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogWeaponInformationMessage* pMessage = _REINTERPRET_CAST( MachLogWeaponInformationMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = WEAPON_INFORMATION_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogWeaponInformationMessage );
	pMessage->weaponInfoFlag_ = weaponInfoFlag;
	pMessage->race_ = race;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendWeaponInformationMessage DONE " << std::endl );
}

void MachLogMessageBroker::processWeaponInformationMessage( NetMessage* pNetMessage )
{
	//MachLogWeaponInformationMessage* pMessage = _REINTERPRET_CAST( MachLogWeaponInformationMessage*, &pNetMessage->body().body()[0] );
	MachLogWeaponInformationMessage* pMessage = _REINTERPRET_CAST( MachLogWeaponInformationMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processWeaponInformationMessage " << std::endl );

	MachPhys::Race myRace = MachLogRaces::instance().pcController().race();

	switch( pMessage->weaponInfoFlag_ )
	{
		case ION_CANNON_FIRED:
			MachLogVoiceMailManager::instance().postNewMail( VID_POD_ENEMY_ION_CANNON, MachLogRaces::instance().pcController().race() );
			break;
		case NUKE_FIRED:
			MachLogVoiceMailManager::instance().postNewMail( VID_POD_ENEMY_NUKE_LAUNCH, MachLogRaces::instance().pcController().race() );
			break;
		case ION_CANNON_ONLINE:
			MachLogRaces::instance().builtIonCannon( pMessage->race_ );
			break;
		case NUKE_ONLINE:
			if( MachLogRaces::instance().beaconType( myRace ) == MachLog::LEVEL_3_BEACON )
				MachLogRaces::instance().builtNuclearWeapon( pMessage->race_ );
			break;
		DEFAULT_ASSERT_BAD_CASE( pMessage->weaponInfoFlag_ );
	}

}

void MachLogMessageBroker::sendSetSiteDiscoveredByMessage( const MexPoint3d& position, MachPhys::Race race )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendSetSiteDiscoveredByMessage\n" );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendSetSiteDiscoveredByMessage " << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogSetSiteDiscoveredByMessage* pMessage = _REINTERPRET_CAST( MachLogSetSiteDiscoveredByMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = SET_SITE_DISCOVERED_BY_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogSetSiteDiscoveredByMessage );
	pMessage->position_ = position;
	pMessage->race_ = race;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendSetSiteDiscoveredByMessage DONE " << std::endl );
}

void MachLogMessageBroker::processSetSiteDiscoveredByMessage( NetMessage* pNetMessage )
{
	//MachLogSetSiteDiscoveredByMessage* pMessage = _REINTERPRET_CAST( MachLogSetSiteDiscoveredByMessage*, &pNetMessage->body().body()[0] );
	MachLogSetSiteDiscoveredByMessage* pMessage = _REINTERPRET_CAST( MachLogSetSiteDiscoveredByMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processSetSiteDiscoveredByMessage " << std::endl );
	for( MachLogPlanet::Sites::const_iterator i = MachLogPlanet::instance().sites().begin();
	     i != MachLogPlanet::instance().sites().end(); ++i )
    {
        MachLogMineralSite* pSite = *i;
		if( not pSite->hasBeenDiscovered() and MexPoint3d( pMessage->position_ ) == pSite->position() )
		{
			pSite->beDiscoveredBy( pMessage->race_ );
			break;

		}
	}
}

void MachLogMessageBroker::sendSetSiteOreMessage( int siteId, MachPhys::BuildingMaterialUnits newOreAmount )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendSetSiteOreMessage\n" );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendSetSiteOreMessage " << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogSetSiteOreMessage* pMessage = _REINTERPRET_CAST( MachLogSetSiteOreMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = EXTRACT_ORE_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogSetSiteOreMessage );
	pMessage->siteId_ = siteId;
	pMessage->newOreAmount_ = newOreAmount;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendSetSiteOreMessage DONE " << std::endl );
}

void MachLogMessageBroker::processSetSiteOreMessage( NetMessage* pNetMessage )
{
	//MachLogSetSiteOreMessage* pMessage = _REINTERPRET_CAST( MachLogSetSiteOreMessage*, &pNetMessage->body().body()[0] );
	MachLogSetSiteOreMessage* pMessage = _REINTERPRET_CAST( MachLogSetSiteOreMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));

	DEBUG_STREAM( DIAG_NETWORK,"processSetSiteOreMessage " << std::endl );

	if( MachLogPlanet::instance().siteExists( pMessage->siteId_ ) )
	{
		MachLogPlanet::instance().pSite( pMessage->siteId_ )->setOre( pMessage->newOreAmount_ );
	}
}

void MachLogMessageBroker::sendNewResearchMessage( MachLog::ObjectType objectType, int subType, int hwLevel, MachPhys::WeaponCombo weaponCombo, MachPhys::Race race )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendNewResearchMessage " << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogNewResearchMessage* pMessage = _REINTERPRET_CAST( MachLogNewResearchMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = NEW_RESEARCH_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogNewResearchMessage );
	pMessage->objectType_ = objectType;
	pMessage->subType_ = subType;
	pMessage->hwLevel_ = hwLevel;
	pMessage->weaponCombo_ = weaponCombo;
	pMessage->race_ = race;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendNewResearchMessage DONE " << std::endl );
}

void MachLogMessageBroker::processNewResearchMessage( NetMessage* pNetMessage )
{
	//MachLogNewResearchMessage* pMessage = _REINTERPRET_CAST( MachLogNewResearchMessage*, &pNetMessage->body().body()[0] );
	MachLogNewResearchMessage* pMessage = _REINTERPRET_CAST( MachLogNewResearchMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processNewResearchMessage " << std::endl );
	MachLogResearchItem& ri = MachLogRaces::instance().researchTree().researchItem( pMessage->objectType_, pMessage->subType_, pMessage->hwLevel_, pMessage->weaponCombo_ );
	ri.researched( pMessage->race_, true );
}

void MachLogMessageBroker::sendHealMessage( UtlId whichId, UtlId targetId, MachLogMessageBroker::HealingFlag healingFlag, bool garrisonHealing )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendHealMessage\n" );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendHealMessage " << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogHealMessage* pMessage = _REINTERPRET_CAST( MachLogHealMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = HEAL_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogHealMessage );
	pMessage->whichId_ = whichId;
	pMessage->targetId_ = targetId;
	pMessage->healingFlag_ = healingFlag;
	pMessage->garrisonHealing_ = garrisonHealing;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendHealMessage DONE " << std::endl );
}

void MachLogMessageBroker::processHealMessage( NetMessage* pNetMessage )
{
	//MachLogHealMessage* pMessage = _REINTERPRET_CAST( MachLogHealMessage*, &pNetMessage->body().body()[0] );
	MachLogHealMessage* pMessage = _REINTERPRET_CAST( MachLogHealMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processHealMessage " << std::endl );
	if( pMessage->garrisonHealing_ )
	{
		if( pMessage->healingFlag_ == MachLogMessageBroker::BEGIN_HEALING )
		{
			if( MachLogRaces::instance().actorExists( pMessage->targetId_ ) )
			{
				MachActor& target = MachLogRaces::instance().actor( pMessage->targetId_ );
				target.asMachine().addHealingAuraReference();
			}
		}
		else
		{
			if( MachLogRaces::instance().actorExists( pMessage->targetId_ ) )
			{
				MachActor& target = MachLogRaces::instance().actor( pMessage->targetId_ );
				target.asMachine().releaseHealingAuraReference();
			}
		}
	}
	else
	{
		if( pMessage->healingFlag_ == MachLogMessageBroker::BEGIN_HEALING )
		{
			if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) and
				MachLogRaces::instance().actorExists( pMessage->targetId_ ) )
			{
				MachActor& actor = MachLogRaces::instance().actor( pMessage->whichId_ );
				MachActor& target = MachLogRaces::instance().actor( pMessage->targetId_ );
				actor.asCanAttack().heal( &target );
			}
		}
		else
		{
			if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
			{
				MachActor& actor = MachLogRaces::instance().actor( pMessage->whichId_ );
				actor.asCanAttack().stopAllHealing( actor );
			}
		}
	}
}

void MachLogMessageBroker::sendRemoveConstructionMessage( UtlId whichId )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendRemoveConstructionMessage (" << whichId << ")" << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogRemoveConstructionMessage* pMessage = _REINTERPRET_CAST( MachLogRemoveConstructionMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = REMOVE_CONSTRUCTION_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogRemoveConstructionMessage );
	pMessage->whichId_ = whichId;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendRemoveConstructionMessage DONE " << std::endl );
}

void MachLogMessageBroker::processRemoveConstructionMessage( NetMessage* pNetMessage )
{
	//MachLogRemoveConstructionMessage* pMessage = _REINTERPRET_CAST( MachLogRemoveConstructionMessage*, &pNetMessage->body().body()[0] );
	MachLogRemoveConstructionMessage* pMessage = _REINTERPRET_CAST( MachLogRemoveConstructionMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processRemoveConstructionMessage (" << pMessage->whichId_ << ")" << std::endl );
	if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
	{
		MachActor& actor = MachLogRaces::instance().actor( pMessage->whichId_ );
		if( actor.objectIsConstruction() )
		{
			actor.asConstruction().networkRemove();
		}
		else
		{
			NETWORK_ERRORS_STREAM("NE04: asked to remove a construction THAT IS NOT A CONSTRUCTION (" << pMessage->whichId_ << ")\n" << actor << std::endl );
		}
	}
	else
	{
		NETWORK_ERRORS_STREAM("NE03: asked to remove a construction not in map (" << pMessage->whichId_ << ")\n" );
	}
}

void MachLogMessageBroker::sendChangeRaceMessage( UtlId whichId, MachPhys::Race newRace )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendChangeRaceMessage (" << whichId << ")" << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogChangeRaceMessage* pMessage = _REINTERPRET_CAST( MachLogChangeRaceMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = CHANGE_RACE_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogChangeRaceMessage );
	pMessage->whichId_ = whichId;
	pMessage->newRace_ = newRace;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendChangeRaceMessage DONE " << std::endl );
}

void MachLogMessageBroker::processChangeRaceMessage( NetMessage* pNetMessage )
{
	//MachLogChangeRaceMessage* pMessage = _REINTERPRET_CAST( MachLogChangeRaceMessage*, &pNetMessage->body().body()[0] );
	MachLogChangeRaceMessage* pMessage = _REINTERPRET_CAST( MachLogChangeRaceMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processChangeRaceMessage (" << pMessage->whichId_ << ")" << std::endl );
	if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
	{
		MachActor& actor = MachLogRaces::instance().actor( pMessage->whichId_ );
		//if the actor is already in the correct race then do nothing...this prevents cascade messages acorss the network.
		if( actor.race() != pMessage->newRace_ )
		{
			if( actor.objectIsMachine() )
				MachPhysTreacheryWeapon::traitorAnimation( &actor.asMachine().physMachine(), SimManager::instance().currentTime(), actor.race(), pMessage->newRace_ );
			actor.assignToDifferentRace( MachLogRaces::instance().race( pMessage->newRace_ ) );
		}
	}
}

void MachLogMessageBroker::sendDebrisExpiredMessage( UtlId whichId )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendDebrisExpiredMessage (" << whichId << ")" << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogRemoveConstructionMessage* pMessage = _REINTERPRET_CAST( MachLogRemoveConstructionMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = DEBRIS_EXPIRED_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogRemoveConstructionMessage );
	pMessage->whichId_ = whichId;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendDebrisExpiredMessage DONE " << std::endl );
}

void MachLogMessageBroker::processDebrisExpiredMessage( NetMessage* pNetMessage )
{
	//MachLogRemoveConstructionMessage* pMessage = _REINTERPRET_CAST( MachLogRemoveConstructionMessage*, &pNetMessage->body().body()[0] );
	MachLogRemoveConstructionMessage* pMessage = _REINTERPRET_CAST( MachLogRemoveConstructionMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processDebrisExpiredMessage (" << pMessage->whichId_ << ")" << std::endl );
	if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
	{
		MachActor& actor = MachLogRaces::instance().actor( pMessage->whichId_ );
		ASSERT( actor.objectIsDebris(),"Oops object is not debris\n" );
		if( actor.objectIsDebris() )
		{
			actor.asDebris().hasBeenPickedUp();
		}
		else
		{
			NETWORK_ERRORS_STREAM("NE05: asked to remove actor (" << pMessage->whichId_ << ") (as debris) that is not debris\n" << actor << std::endl );
		}
	}
	else
	{
		NETWORK_ERRORS_STREAM("NW02: debirs expired for actor not in map (" << pMessage->whichId_ << ")\n" );
	}
}

void MachLogMessageBroker::sendLoadArtefactModelsMessage( const SysPathName& pathName )
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sstd::endloadArtefactModelsMessage pathname:" << pathName << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogLoadArtefactModelsMessage* pMessage = _REINTERPRET_CAST( MachLogLoadArtefactModelsMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = MachLogMessageBroker::LOAD_ARTEFACT_MODELS_CODE;
	int length = pathName.pathname().length();

	pMessage->header_.totalLength_ = sizeof( MachLogLoadArtefactModelsMessage ) - 512 + length + 1;
	memcpy( (void*)(&pMessage->arrayData_[0]), (void*)pathName.pathname().c_str(), length );
	//put on termination null character.
	pMessage->arrayData_[ length ] = 0;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sstd::endloadArtefactModelsMessage DONE " << std::endl );
}

void MachLogMessageBroker::processLoadArtefactModelsMessage( NetMessage* pNetMessage )
{
	//MachLogLoadArtefactModelsMessage* pMessage = _REINTERPRET_CAST( MachLogLoadArtefactModelsMessage*, &pNetMessage->body().body()[0] );
	MachLogLoadArtefactModelsMessage* pMessage = _REINTERPRET_CAST( MachLogLoadArtefactModelsMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processLoadArtefactModelsMessage " << std::endl );
	MachLogRaces::instance().artefacts().load( pMessage->arrayData_ );
}

void MachLogMessageBroker::sendAddNewArtefactSubTypeMessage
(
	int subType, const string& name, MachPhys::BuildingMaterialUnits cost,
	MachPhys::ArmourUnits armour, MachPhys::HitPointUnits hitPoints,
	uint stringId, MATHEX_SCALAR height, const ctl_vector< MexPoint2d >* pLocalBoundary
)
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendAddNewArtefactSubTypeMessage name:" << name << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogAddNewArtefactSubTypeMessage* pMessage = _REINTERPRET_CAST( MachLogAddNewArtefactSubTypeMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = ADD_NEW_ARTEFACT_SUB_TYPE_CODE;
	int length = name.length();
	int boundaryPoints = 0;
	if( pLocalBoundary )
		boundaryPoints = pLocalBoundary->size();

	pMessage->header_.totalLength_ = sizeof( MachLogAddNewArtefactSubTypeMessage )		//total size
									 - 512 + length + 1 +								//remove arraysize and add in string size
									 sizeof( MexPoint2d) * boundaryPoints;		//add in size of boundary points
	pMessage->subType_ = subType;
	pMessage->cost_ = cost;
	pMessage->armour_ = armour;
	pMessage->hitPoints_ = hitPoints;
	pMessage->stringId_ = stringId;
	pMessage->height_ = height;
	pMessage->boundaryPoints_ = boundaryPoints;

	//poke local boundary into arrayData_;
	for( int i = 0; i < boundaryPoints; ++i )
		memcpy( (void*)(&pMessage->arrayData_[0] + i * sizeof( MexPoint2d ) ), (void*)&pLocalBoundary->operator[](i), sizeof( MexPoint2d ) );
	//poke in "name"
	int nameOffset = boundaryPoints * sizeof( MexPoint2d );
	memcpy( (void*)(&pMessage->arrayData_[0] + nameOffset ), (void*)name.c_str(), length );
	//put on termination null character.
	int nullterminator = nameOffset + length;
	pMessage->arrayData_[ nullterminator ] = 0;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendAddNewArtefactSubTypeMessage DONE " << std::endl );
}

void MachLogMessageBroker::processAddNewArtefactSubTypeMessage( NetMessage* pNetMessage )
{
	//MachLogAddNewArtefactSubTypeMessage* pMessage = _REINTERPRET_CAST( MachLogAddNewArtefactSubTypeMessage*, &pNetMessage->body().body()[0] );
	MachLogAddNewArtefactSubTypeMessage* pMessage = _REINTERPRET_CAST( MachLogAddNewArtefactSubTypeMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processAddNewArtefactSubTypeMessage " << std::endl );
	//recreate local boundary
	ctl_vector< MexPoint2d > localBoundary;
	for( int i = 0; i < pMessage->boundaryPoints_; ++i )
	{
		MexPoint2d tmp;
		memcpy( (void*)&tmp, (void*)(&pMessage->arrayData_[0] + i * sizeof( MexPoint2d ) ), sizeof( MexPoint2d ) );
		localBoundary.push_back( tmp );
	}
	//recreate name string.
	string name( &pMessage->arrayData_[ pMessage->boundaryPoints_ * sizeof( MexPoint2d ) ] );
	DEBUG_STREAM( DIAG_NETWORK,"subType " << pMessage->subType_ << " name " << name << " boundaryPoints " << pMessage->boundaryPoints_ << std::endl );
	if( pMessage->boundaryPoints_ > 0 )
		MachLogRaces::instance().artefacts().addSubType
		(
			pMessage->subType_, name, pMessage->cost_,
			pMessage->armour_, pMessage->hitPoints_, pMessage->stringId_, pMessage->height_, localBoundary
		);
	else
		MachLogRaces::instance().artefacts().addSubType( pMessage->subType_, name );

}

void MachLogMessageBroker::sendAddArtefactDamageLinkMessage
(
	UtlId destroyId, UtlId damageId, int hpModifier, int armourModifier
)
{
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendAddArtefactDamageLinkMessage " << std::endl );
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogAddArtefactDamageLinkMessage* pMessage = _REINTERPRET_CAST( MachLogAddArtefactDamageLinkMessage*, pLogNetMessage );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = ADD_ARTEFACT_DAMAGE_LINK_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogAddArtefactDamageLinkMessage );
	pMessage->destroyId_ = destroyId;
	pMessage->damageId_ = damageId;
	pMessage->hpModifier_ = hpModifier;
	pMessage->armourModifier_ = armourModifier;

	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendAddArtefactDamageLinkMessage DONE " << std::endl );
}

void MachLogMessageBroker::processAddArtefactDamageLinkMessage( NetMessage* pNetMessage )
{
	//MachLogAddArtefactDamageLinkMessage* pMessage = _REINTERPRET_CAST( MachLogAddArtefactDamageLinkMessage*, &pNetMessage->body().body()[0] );
	MachLogAddArtefactDamageLinkMessage* pMessage = _REINTERPRET_CAST( MachLogAddArtefactDamageLinkMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processAddArtefactDamageLinkMessage " << std::endl );
	MachLogRaces::instance().artefacts().addDamageRelation( pMessage->destroyId_, pMessage->damageId_, pMessage->hpModifier_, pMessage->armourModifier_ );
}

void MachLogMessageBroker::sendCamouflageMachineMessage( UtlId whichId, CamouflageFlag camouflageFlag )
{
	MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
	MachLogCamouflageMachineMessage* pMessage = _REINTERPRET_CAST( MachLogCamouflageMachineMessage*, pLogNetMessage );

	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendCamouflageMachineMessage " << std::endl );
	pMessage->header_.systemCode_ = 0;
	pMessage->header_.messageCode_ = MACHINE_CAMOUFLAGE_CHANGED_CODE;
	pMessage->header_.totalLength_ = sizeof( MachLogCamouflageMachineMessage );
	pMessage->whichId_ = whichId;
	pMessage->camouflageFlag_ = camouflageFlag;
	doSend( pLogNetMessage );
	DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendCamouflageMachineMessage DONE " << std::endl );

}

void MachLogMessageBroker::processCamouflageMachineMessage( NetMessage* pNetMessage )
{
	//MachLogCamouflageMachineMessage* pMessage = _REINTERPRET_CAST( MachLogCamouflageMachineMessage*, &pNetMessage->body().body()[0] );
	MachLogCamouflageMachineMessage* pMessage = _REINTERPRET_CAST( MachLogCamouflageMachineMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
	DEBUG_STREAM( DIAG_NETWORK,"processCamouflageMachineMessage " << pMessage->whichId_ << std::endl );

	ASSERT( MachLogRaces::instance().actorExists( pMessage->whichId_ ), "Actor does not exist" );

	if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
	{
		MachActor& actor = MachLogRaces::instance().actor( pMessage->whichId_ );

		ASSERT( actor.objectIsMachine(), "Should only be receiving camouflage messages about machines, NOT other actor types." );

		MachLogMachine& machine = actor.asMachine();

		ASSERT( pMessage->camouflageFlag_ == BEGIN_CAMOUFLAGE or pMessage->camouflageFlag_ == STOP_CAMOUFLAGE, "Unknown message type received." );

		if( pMessage->camouflageFlag_ == BEGIN_CAMOUFLAGE )
			machine.camouflaged( true );
		else
			machine.camouflaged( false );
	}
}

void MachLogMessageBroker::sendCurrentTargetMessage( UtlId whichId, UtlId targetId )
{
        MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
        MachLogCurrentTargetMessage* pMessage = _REINTERPRET_CAST( MachLogCurrentTargetMessage*, pLogNetMessage );

        DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendCurrentTargetMessage " << std::endl );
        DEBUG_STREAM( DIAG_NETWORK, "  whichId " << whichId << " targetId " << targetId << std::endl );

        pMessage->header_.systemCode_ = 0;
        pMessage->header_.messageCode_ = CURRENT_TARGET_CODE;
        pMessage->header_.totalLength_ = sizeof( MachLogCurrentTargetMessage );
        pMessage->whichId_ = whichId;
        pMessage->targetId_ = targetId;
        pMessage->targetIdValidFlag_ = targetId != 0 ? VALID : INVALID;

        doSend( pLogNetMessage );

        DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendCurrentTargetMessage DONE " << std::endl );
}

void MachLogMessageBroker::processCurrentTargetMessage( NetMessage* pNetMessage )
{
        //MachLogCurrentTargetMessage* pMessage = _REINTERPRET_CAST( MachLogCurrentTargetMessage*, &pNetMessage->body().body()[0] );
        MachLogCurrentTargetMessage* pMessage = _REINTERPRET_CAST( MachLogCurrentTargetMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
        DEBUG_STREAM( DIAG_NETWORK,"processCurrentTargetMessage " << pMessage->whichId_ << std::endl );
        if( MachLogRaces::instance().actorExists( pMessage->whichId_ ) )
        {
                MachActor& actor = MachLogRaces::instance().actor( pMessage->whichId_ );
                if( actor.objectIsCanAttack() )
                {
                    if( pMessage->targetIdValidFlag_ == VALID and MachLogRaces::instance().actorExists( pMessage->targetId_ ) )
                    {
                            actor.asCanAttack().currentTarget( &MachLogRaces::instance().actor( pMessage->targetId_ ) );
                    }
                    else
                            actor.asCanAttack().currentTarget( NULL );
                }
        }
}



void MachLogMessageBroker::sendEchoNewScoreMessage( const MachLogScoreImpl& scoreImpl )
{
        MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
        MachLogEchoNewScoreMessage* pMessage = _REINTERPRET_CAST( MachLogEchoNewScoreMessage*, pLogNetMessage );

        DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendEchoNewScoreMessage " << std::endl );

        pMessage->header_.systemCode_ = 0;
        pMessage->header_.messageCode_ = ECHO_NEW_SCORE_CODE;
        pMessage->header_.totalLength_ = sizeof( MachLogEchoNewScoreMessage );
        pMessage->scoreImpl_ = scoreImpl;

        doSend( pLogNetMessage );

        DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendEchoNewScoreMessage DONE " << std::endl );
}

void MachLogMessageBroker::processEchoNewScoreMessage( NetMessage* pNetMessage )
{
        //MachLogEchoNewScoreMessage* pMessage = _REINTERPRET_CAST( MachLogEchoNewScoreMessage*, &pNetMessage->body().body()[0] );
        MachLogEchoNewScoreMessage* pMessage = _REINTERPRET_CAST( MachLogEchoNewScoreMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
        DEBUG_STREAM( DIAG_NETWORK,"processEchoNewScoreMessage " << std::endl );
        MachLogRaces::instance().score( pMessage->scoreImpl_.race_ ).assignScoreFromImpl( pMessage->scoreImpl_ );
}

void MachLogMessageBroker::sendActorThreatMessage( UtlId whichId, UtlId threatId, MachLogMessageBroker::ThreatFlag threatFlag )
{
    MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
    MachLogActorThreatMessage* pMessage = _REINTERPRET_CAST( MachLogActorThreatMessage*, pLogNetMessage );

    DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendActorThreatMessage " << std::endl );

    pMessage->header_.systemCode_ = 0;
    pMessage->header_.messageCode_ = ACTOR_THREAT_CODE;
    pMessage->header_.totalLength_ = sizeof( MachLogActorThreatMessage );
    pMessage->whichId_ = whichId;
    pMessage->threatId_ = threatId;
    pMessage->threatFlag_ = threatFlag;

    doSend( pLogNetMessage );

    DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendActorThreatMessage DONE " << std::endl );
}

void MachLogMessageBroker::processActorThreatMessage( NetMessage* pNetMessage )
{
    //MachLogActorThreatMessage* pMessage = _REINTERPRET_CAST( MachLogActorThreatMessage*, &pNetMessage->body().body()[0] );
    MachLogActorThreatMessage* pMessage = _REINTERPRET_CAST( MachLogActorThreatMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
    DEBUG_STREAM( DIAG_NETWORK,"processActorThreatMessage whichId " << pMessage->whichId_ << " threat " << pMessage->threatId_ << " Add? " << ( pMessage->threatFlag_ == ADD_THREAT ? true : false ) << std::endl );
	MachLogRaces& races = MachLogRaces::instance();
	if( races.actorExists( pMessage->whichId_ ) and races.actorExists( pMessage->threatId_ ) )
	{
		MachActor& actor = races.actor( pMessage->whichId_ );
		if( pMessage->threatFlag_ == ADD_THREAT )
		{
			if( not actor.hasThisActorAsAThreat( pMessage->threatId_ ) )
				actor.addThreat( pMessage->threatId_ );
		}
		else
		{
			if( actor.hasThisActorAsAThreat( pMessage->threatId_ ) )
				actor.removeThreat( pMessage->threatId_ );
		}
	}
	else
	{
		NETWORK_STREAM("One of the actor IDs does not exists. actorExists(" << pMessage->whichId_ << ") " << races.actorExists( pMessage->whichId_ ) << " actorExists(" << pMessage->threatId_ << ") " << races.actorExists( pMessage->threatId_ ) << std::endl );
	}
    DEBUG_STREAM( DIAG_NETWORK,"processActorThreatMessage DONE " << std::endl );

}

void MachLogMessageBroker::sendChangeOfDispositionMessage( MachPhys::Race checkRace, MachPhys::Race targetRace, int disposition )
{
    MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
    MachLogChangeOfDispositionMessage* pMessage = _REINTERPRET_CAST( MachLogChangeOfDispositionMessage*, pLogNetMessage );

    DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendChangeOfDispositionMessage " << std::endl );

    pMessage->header_.systemCode_ = 0;
    pMessage->header_.messageCode_ = CHANGE_OF_DISPOSITION_CODE;
    pMessage->header_.totalLength_ = sizeof( MachLogChangeOfDispositionMessage );
	pMessage->checkRace_ = checkRace;
	pMessage->targetRace_ = targetRace;
	pMessage->disposition_ = disposition;

    doSend( pLogNetMessage );

    DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendChangeOfDispositionMessage DONE " << std::endl );
}

void MachLogMessageBroker::processChangeOfDispositionMessage( NetMessage* pNetMessage )
{
    //MachLogChangeOfDispositionMessage* pMessage = _REINTERPRET_CAST( MachLogChangeOfDispositionMessage*, &pNetMessage->body().body()[0] );
    MachLogChangeOfDispositionMessage* pMessage = _REINTERPRET_CAST( MachLogChangeOfDispositionMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
    DEBUG_STREAM( DIAG_NETWORK,"processChangeOfDispositionMessage " << std::endl );
	MachLogRaces& races = MachLogRaces::instance();
	races.dispositionToRaceWithoutEcho( pMessage->checkRace_, pMessage->targetRace_, (MachLogRaces::DispositionToRace)pMessage->disposition_ );
    DEBUG_STREAM( DIAG_NETWORK,"processChangeOfDispositionMessage DONE " << std::endl );

}

void MachLogMessageBroker::sendSmeltMachineMessage( UtlId whichId )
{
    MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
    MachLogSmeltMachineMessage* pMessage = _REINTERPRET_CAST( MachLogSmeltMachineMessage*, pLogNetMessage );

    DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendSmeltMachineMessage " << std::endl );

    pMessage->header_.systemCode_ = 0;
    pMessage->header_.messageCode_ = SMELT_MACHINE_CODE;
    pMessage->header_.totalLength_ = sizeof( MachLogSmeltMachineMessage );
	pMessage->whichId_ = whichId;

    doSend( pLogNetMessage );

    DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendSmeltMachineMessage DONE " << std::endl );
}

void MachLogMessageBroker::processSmeltMachineMessage( NetMessage* pNetMessage )
{
    //MachLogSmeltMachineMessage* pMessage = _REINTERPRET_CAST( MachLogSmeltMachineMessage*, &pNetMessage->body().body()[0] );
    MachLogSmeltMachineMessage* pMessage = _REINTERPRET_CAST( MachLogSmeltMachineMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
    DEBUG_STREAM( DIAG_NETWORK,"processSmeltMachineMessage " << std::endl );
	MachLogRaces& races = MachLogRaces::instance();
	UtlId& whichId = pMessage->whichId_;

	if( races.actorExists( whichId ) and races.actor( whichId ).objectIsMachine() )
		races.actor( whichId ).asMachine().smeltMe();
	#ifndef PRODUCTION
	else
	{
		NETWORK_ERRORS_STREAM("NE06: acked to smelt a machine that isn't there or is not a machine\n\tActor exists " << races.actorExists( whichId ) );
		if( races.actorExists( whichId ) )
			NETWORK_ERRORS_STREAM(" objectIsmachine " << races.actor( whichId ).objectIsMachine() << std::endl );
		else
			NETWORK_ERRORS_STREAM( std::endl );
	}
	#endif

    DEBUG_STREAM( DIAG_NETWORK,"processSmeltMachineMessage DONE " << std::endl );

}

void MachLogMessageBroker::sendAdvanceConstructionStateMessage( UtlId whichId, MachPhys::BuildingMaterialUnits addAmount )
{
    MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
    MachLogAdvanceConstructionStateMessage* pMessage = _REINTERPRET_CAST( MachLogAdvanceConstructionStateMessage*, pLogNetMessage );

    DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendAdvanceConstructionStateMessage " << std::endl );

    pMessage->header_.systemCode_ = 0;
    pMessage->header_.messageCode_ = ADVANCE_CONSTRUCTION_STATE_CODE;
    pMessage->header_.totalLength_ = sizeof( MachLogAdvanceConstructionStateMessage );
	pMessage->whichId_ = whichId;
	pMessage->addAmount_ = addAmount;

    doSend( pLogNetMessage );

    DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendAdvanceConstructionStateMessage DONE " << std::endl );
}

void MachLogMessageBroker::processAdvanceConstructionStateMessage( NetMessage* pNetMessage )
{
    //MachLogAdvanceConstructionStateMessage* pMessage = _REINTERPRET_CAST( MachLogAdvanceConstructionStateMessage*, &pNetMessage->body().body()[0] );
    MachLogAdvanceConstructionStateMessage* pMessage = _REINTERPRET_CAST( MachLogAdvanceConstructionStateMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
    DEBUG_STREAM( DIAG_NETWORK,"processAdvanceConstructionStateMessage " << std::endl );
	MachLogRaces& races = MachLogRaces::instance();
	UtlId& whichId = pMessage->whichId_;

	if( races.actorExists( whichId ) and races.actor( whichId ).objectIsConstruction() )
		races.actor( whichId ).asConstruction().advanceConstructionState( pMessage->addAmount_ );

    DEBUG_STREAM( DIAG_NETWORK,"processAdvanceConstructionStateMessage DONE " << std::endl );

}

void MachLogMessageBroker::sendAddRepairPointsMessage( UtlId whichId, MachPhys::HitPointUnits addAmount )
{
    MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
    MachLogAddRepairPointsMessage* pMessage = _REINTERPRET_CAST( MachLogAddRepairPointsMessage*, pLogNetMessage );

    DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendAddRepairPointsMessage " << std::endl );

    pMessage->header_.systemCode_ = 0;
    pMessage->header_.messageCode_ = ADD_REPAIR_POINTS_CODE;
    pMessage->header_.totalLength_ = sizeof( MachLogAddRepairPointsMessage );
	pMessage->whichId_ = whichId;
	pMessage->addAmount_ = addAmount;

    doSend( pLogNetMessage );

    DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendAddRepairPointsMessage DONE " << std::endl );
}

void MachLogMessageBroker::processAddRepairPointsMessage( NetMessage* pNetMessage )
{
    //MachLogAddRepairPointsMessage* pMessage = _REINTERPRET_CAST( MachLogAddRepairPointsMessage*, &pNetMessage->body().body()[0] );
    MachLogAddRepairPointsMessage* pMessage = _REINTERPRET_CAST( MachLogAddRepairPointsMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
    DEBUG_STREAM( DIAG_NETWORK,"processAddRepairPointsMessage " << std::endl );
	MachLogRaces& races = MachLogRaces::instance();
	UtlId& whichId = pMessage->whichId_;

	if( races.actorExists( whichId ) and races.actor( whichId ).objectIsConstruction() )
		races.actor( whichId ).asConstruction().addRepairPoints( pMessage->addAmount_ );

    DEBUG_STREAM( DIAG_NETWORK,"processAddRepairPointsMessage DONE " << std::endl );

}

void MachLogMessageBroker::sendMakeCompleteConstructionMessage( UtlId whichId )
{
    MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
    MachLogMakeCompleteConstructionMessage* pMessage = _REINTERPRET_CAST( MachLogMakeCompleteConstructionMessage*, pLogNetMessage );

    DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendMakeCompleteConstructionMessage " << std::endl );

    pMessage->header_.systemCode_ = 0;
    pMessage->header_.messageCode_ = MAKE_COMPLETE_CONSTRUCTION_CODE;
    pMessage->header_.totalLength_ = sizeof( MachLogMakeCompleteConstructionMessage );
	pMessage->whichId_ = whichId;

    doSend( pLogNetMessage );

    DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendMakeCompleteConstructionMessage DONE " << std::endl );
}

void MachLogMessageBroker::processMakeCompleteConstructionMessage( NetMessage* pNetMessage )
{
    //MachLogMakeCompleteConstructionMessage* pMessage = _REINTERPRET_CAST( MachLogMakeCompleteConstructionMessage*, &pNetMessage->body().body()[0] );
    MachLogMakeCompleteConstructionMessage* pMessage = _REINTERPRET_CAST( MachLogMakeCompleteConstructionMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
    DEBUG_STREAM( DIAG_NETWORK,"processMakeCompleteConstructionMessage " << std::endl );
	MachLogRaces& races = MachLogRaces::instance();
	UtlId& whichId = pMessage->whichId_;

	if( races.actorExists( whichId ) and races.actor( whichId ).objectIsConstruction() )
		races.actor( whichId ).asConstruction().makeComplete( MachLogConstruction::FULL_HP_STRENGTH );

    DEBUG_STREAM( DIAG_NETWORK,"processMakeCompleteConstructionMessage DONE " << std::endl );

}

void MachLogMessageBroker::sendIsEntranceOpenMessage( UtlId whichId, size_t door, bool doOpen )
{
    MachLogNetMessage* pLogNetMessage = _NEW( MachLogNetMessage() );
    MachLogIsEntranceOpenMessage* pMessage = _REINTERPRET_CAST( MachLogIsEntranceOpenMessage*, pLogNetMessage );

    DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendIsEntranceOpenMessage " << std::endl );

    pMessage->header_.systemCode_ = 0;
    pMessage->header_.messageCode_ = IS_ENTRANCE_OPEN_CODE;
    pMessage->header_.totalLength_ = sizeof( MachLogIsEntranceOpenMessage );
	pMessage->whichId_ = whichId;
	pMessage->door_ = door;
	pMessage->doOpen_ = doOpen;

    doSend( pLogNetMessage );

    DEBUG_STREAM( DIAG_NETWORK, "MLMessageBroker::sendIsEntranceOpenMessage DONE " << std::endl );
}

void MachLogMessageBroker::processIsEntranceOpenMessage( NetMessage* pNetMessage )
{
    //MachLogIsEntranceOpenMessage* pMessage = _REINTERPRET_CAST( MachLogIsEntranceOpenMessage*, &pNetMessage->body().body()[0] );
    MachLogIsEntranceOpenMessage* pMessage = _REINTERPRET_CAST( MachLogIsEntranceOpenMessage*, _CONST_CAST(uint8*, &pNetMessage->body().body()[0] ));
    DEBUG_STREAM( DIAG_NETWORK,"processIsEntranceOpenMessage " << std::endl );
	MachLogRaces& races = MachLogRaces::instance();
	UtlId& whichId = pMessage->whichId_;

	if( races.actorExists( whichId ) and races.actor( whichId ).objectIsConstruction() )
		races.actor( whichId ).asConstruction().isEntranceOpenNoEcho( pMessage->door_, pMessage->doOpen_ );

    DEBUG_STREAM( DIAG_NETWORK,"processIsEntranceOpenMessage DONE " << std::endl );

}
