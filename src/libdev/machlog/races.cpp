/*
 * R A C E S. C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */
#include "stdlib/string.hpp"
#include "machlog/races.hpp"
#include "mathex/point2d.hpp"
#include "mathex/eulerang.hpp"
#include "ctl/algorith.hpp"
#include "ctl/erase.hpp"
#include "ctl/vector.hpp"
#include "system/pathname.hpp"
#include "utility/bidpmap.hpp"
#include "utility/bndidgen.hpp"
#include "utility/linetok.hpp"
#include "network/netnet.hpp"
#include "world4d/generic.hpp"
#include "world4d/scenemgr.hpp"
#include "world4d/manager.hpp"
#include "sim/manager.hpp"
#include "machphys/ctordata.hpp"
#include "machphys/consdata.hpp"
#include "machphys/random.hpp"
#include "machphys/station.hpp"
#include "machphys/stations.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/genedata.hpp"
#include "machlog/actor.hpp"
#include "machlog/machine.hpp"
#include "machlog/administ.hpp"
#include "machlog/aggressr.hpp"
#include "machlog/apc.hpp"
#include "machlog/beacon.hpp"
#include "machlog/constree.hpp"
#include "machlog/construc.hpp"
#include "machlog/cntrl.hpp"
#include "machlog/cntrl_pc.hpp"
#include "machlog/factory.hpp"
#include "machlog/garrison.hpp"
#include "machlog/geolocat.hpp"
#include "machlog/hwlab.hpp"
#include "machlog/mine.hpp"
#include "machlog/missilem.hpp"
#include "machlog/planet.hpp"
#include "machlog/pod.hpp"
#include "machlog/produnit.hpp"
#include "machlog/rescarr.hpp"
#include "machlog/scenario.hpp"
#include "machlog/smelter.hpp"
#include "machlog/spy.hpp"
#include "machlog/squad.hpp"
#include "machlog/canattac.hpp"
#include "machlog/stats.hpp"
#include "machlog/technici.hpp"
#include "machlog/artfacts.hpp"
#include "machlog/strategy.hpp"
#include "machlog/score.hpp"
#include "machlog/persist.hpp"
#include "machlog/creatdat.hpp"
#include "machlog/p1remman.hpp"
#include "machlog/network.hpp"
#include "machlog/messbrok.hpp"
#include "machlog/internal/racesimp.hpp"
#include "machlog/vmdata.hpp"
#include "machlog/vmman.hpp"
#include "machlog/dbhandlr.hpp"
#include "machlog/dispnoti.hpp"
#include "machlog/artefact.hpp"
#include "machlog/cntrl_ai.hpp"

#include "system/metafile.hpp"
#include "system/metaistr.hpp"
#include <memory>
#include <cstdio>

/* //////////////////////////////////////////////////////////////// */
void checkTypeAndSetDefCon( MachActor* pActor )
{
	if( pActor->objectIsMachine() )
		pActor->asMachine().defCon( MachLogRaces::instance().defCon( pActor->race() ) );
}

//static
MachLogRaces& MachLogRaces::instance()
{
	static MachLogRaces instance_;
	return instance_;
};

MachLogRaces::MachLogRaces()
:	pDataImpl_( _NEW( MachLogRacesDataImpl ) )
{
	//Ensure actor id 0 gets allocated. Then it will not be used inadvertently when wrapping round.
    idGenerator().allocateId( 0 );

	PhysAbsoluteTime timeNow = SimManager::instance().currentTime();

	pDataImpl_->pActorIdMap_ = _NEW( ActorIdMap( maxActors() ) );
    pDataImpl_->thePCController_ = NULL;
	pDataImpl_->pResearchTree_ = NULL;
	pDataImpl_->pConstructionTree_ = NULL;
	pDataImpl_->pStats_ = NULL;
    pDataImpl_->pArtefacts_ = _NEW( MachLogArtefacts );
	pDataImpl_->pGameCreationData_ = NULL;
	pDataImpl_->scores_.reserve( MachPhys::N_RACES );
	MachLogScore* pScore = NULL;
	for( MachPhys::Race r = MachPhys::RED; r < MachPhys::N_RACES; ++((int&)r) )
	{
		pDataImpl_->scores_.push_back( pScore );

		pDataImpl_->builtIonCannon_[ r ] = false;
		pDataImpl_->builtNuclearWeapon_[ r ] = false;
		pDataImpl_->raceInGame_[ r ] = false;		// will be set to true if scenario file dictates
    	pDataImpl_->aggressorAssemblyPoints_[ r ].reserve( 8 );
    	pDataImpl_->administratorAssemblyPoints_[ r ].reserve( 8 );
   		pDataImpl_->squadrons_[ r ].reserve( 20 );

		// set race contact flag to true (trivially) for your own race, false for all others
		for( MachPhys::Race rOther = MachPhys::RED; rOther < MachPhys::N_RACES; ++((int&)rOther) )
		{
			if( r == rOther )
				pDataImpl_->contactBetweenRaces_[ r ] [ rOther ] = true;
			else
				pDataImpl_->contactBetweenRaces_[ r ] [ rOther ] = false;
		}
	}

    pDataImpl_->pRemoteFirstPersonManager_ = _NEW( MachLogRemoteFirstPersonManager );
    pDataImpl_->pDatabaseHandler_ = NULL;

	pDataImpl_->lastFullSmeltersUpdateTime_ = timeNow;
	pDataImpl_->lastLowBMUsUpdateTime_ = timeNow;

	MachLogMachine::resetNextGlobalHitVoiceMailTime();
	MachLogConstruction::resetNextGlobalHitVoiceMailTime();
}

MachLogRaces::~MachLogRaces()
{
	HAL_STREAM("MLRaces::DTOR...\n" );
	//squadrons are now deleted by owning processes
//	for( Squadrons::iterator i = squadrons_[ MachPhys::RED ].begin(); i != squadrons_[ MachPhys::RED ].end(); ++i )
//		_DELETE( (*i) );
//	for( Squadrons::iterator i = squadrons_[ MachPhys::BLUE ].begin(); i != squadrons_[ MachPhys::BLUE ].end(); ++i )
//		_DELETE( (*i) );
	_DELETE( pDataImpl_->pGameCreationData_ );
    _DELETE( pDataImpl_->pActorIdMap_ );
    _DELETE( pDataImpl_->pRemoteFirstPersonManager_ );

	for( Scores::size_type i=0; i<pDataImpl_->scores_.size(); ++i )
		_DELETE( pDataImpl_->scores_[ i ] );

	_DELETE( pDataImpl_->pArtefacts_ );

	_DELETE( pDataImpl_ );
	HAL_STREAM(" done\n" );
}

//static
UtlBoundedIdGenerator& MachLogRaces::idGenerator()
{
	static UtlBoundedIdGenerator idGenerator_( ID_UPPER_BOUND );
    return idGenerator_;
}

//static
int MachLogRaces::maxActors()
{
    return ID_UPPER_BOUND;
}

bool MachLogRaces::objectExists( const MachActor * pObj ) const
{
	return ctl_contains( &pDataImpl_->objects_, pObj );
}

bool MachLogRaces::machineExists( const MachLogMachine * pMach ) const
{
	return ctl_contains( &pDataImpl_->machines_, pMach );
}

/* //////////////////////////////////////////////////////////////// */

MachLogRaces::Machines&
MachLogRaces::machines()
{
	return pDataImpl_->machines_;
}

const MachLogRaces::Machines& MachLogRaces::machines() const
{
	return pDataImpl_->machines_;
}

///////////////////////////////////

MachLogRaces::Objects& MachLogRaces::objects()
{
	return pDataImpl_->objects_;
}

const MachLogRaces::Objects& MachLogRaces::objects() const
{
	return pDataImpl_->objects_;
}

MachLogRaces::Objects& MachLogRaces::raceObjects( MachPhys::Race race )
{
	return pDataImpl_->raceObjects_[ map_MachPhysRace_to_size_t( race ) ];
}
const MachLogRaces::Objects& MachLogRaces::raceObjects( MachPhys::Race race ) const
{
	return pDataImpl_->raceObjects_[ map_MachPhysRace_to_size_t( race ) ];
}
/* //////////////////////////////////////////////////////////////// */

//find the first available factory of that type to build a machine return indicates true found one
//factory returned in ppFactory

bool MachLogRaces::getSuitableFactory( MachPhys::Race r, const MachLogProductionUnit& prod, MachLogFactory** ppFactory )
{
	bool found = false;
	Factories relevantFactories;
	for( Factories::iterator i = factories( r ).begin();
		i != factories( r ).end() and not found; ++i )
		if( (*i)->isAllowedToBuild( prod.type(), prod.subType(), prod.hwLevel(), prod.weaponCombo() ) )
		{
			relevantFactories.push_back( (*i ) );
		}

	if( relevantFactories.size() == 0 )
		found = false;
	else
	{
		if( relevantFactories.size() == 1 )
		{
			found = true;
			*ppFactory = relevantFactories.front();
		}
		else
		{
			//we have more than one factory capable of building machine.
			//if a factory is not currently producing then machine goes to that factory
			//if factory is producing but priority will jump it to front then goes to that factory
			//if none of these are true then it will go to first factory in list

			//default to first factory
			found = true;
			*ppFactory = relevantFactories.front();
			bool jumpOutOfLoop = false;
			for( Factories::iterator i = relevantFactories.begin(); i != relevantFactories.end() and not jumpOutOfLoop; ++i )
			{
				MachLogProductionUnit prodUnit;
				//check for factory not building
				if( not (*i)->currentlyBuildingProductionUnit( &prodUnit ) )
				{
					jumpOutOfLoop = true;
					*ppFactory = *i;
				}
				else
				{
					//are we higher priority anyway to jump to front of queue?
					if( prodUnit.priority() < prod.priority() )
					{
						jumpOutOfLoop = true;
						*ppFactory = *i;
					}
				}
			}
		}
	}
	return found;
}

bool MachLogRaces::addMatchingMachineToSquadron( MachPhys::Race r, const MachLogProductionUnit& prod, MachLogSquadron* pSquadron )
{
	bool found = false;
	for( Objects::iterator i = pDataImpl_->raceObjects_[ map_MachPhysRace_to_size_t( r ) ].begin();
		i != pDataImpl_->raceObjects_[ map_MachPhysRace_to_size_t( r ) ].end() and not found; ++i )
		{
			if( (*i)->objectIsMachine() )
			{
				MachLogMachine& mlm = (*i)->asMachine();
				MachPhys::WeaponCombo wc = MachPhys::N_WEAPON_COMBOS;
				if( mlm.objectIsCanAttack() )
					wc = mlm.asCanAttack().weaponCombo();
				if( mlm.squadron() == NULL and
					mlm.objectType() == prod.type() and
					mlm.subType() == prod.subType() and
					mlm.hwLevel() == prod.hwLevel() and
					wc == prod.weaponCombo() )
					{
						found = true;
						mlm.squadron( pSquadron );
					}
			}
		}
	return found;
}

MachLogRaces::Aggressors&
MachLogRaces::aggressors( MachPhys::Race r )
{
	return pDataImpl_->aggressors_[ map_MachPhysRace_to_size_t( r ) ];
}

const MachLogRaces::Aggressors&
MachLogRaces::aggressors( MachPhys::Race r ) const
{
	return pDataImpl_->aggressors_[ map_MachPhysRace_to_size_t( r ) ];
}

///////////////////////////////////

MachLogRaces::Administrators&
MachLogRaces::administrators( MachPhys::Race r )
{
	return pDataImpl_->administrators_[ map_MachPhysRace_to_size_t( r ) ];
}

const MachLogRaces::Administrators&
MachLogRaces::administrators( MachPhys::Race r ) const
{
	return pDataImpl_->administrators_[ map_MachPhysRace_to_size_t( r ) ];
}

///////////////////////////////////

MachLogRaces::Technicians&
MachLogRaces::technicians( MachPhys::Race r )
{
	return pDataImpl_->technicians_[ map_MachPhysRace_to_size_t( r ) ];
}

const MachLogRaces::Technicians&
MachLogRaces::technicians( MachPhys::Race r ) const
{
	return pDataImpl_->technicians_[ map_MachPhysRace_to_size_t( r ) ];
}

///////////////////////////////////

MachLogRaces::Constructors&
MachLogRaces::constructors( MachPhys::Race r )
{
	return pDataImpl_->constructors_[ map_MachPhysRace_to_size_t( r ) ];
}

const MachLogRaces::Constructors&
MachLogRaces::constructors( MachPhys::Race r ) const
{
	return pDataImpl_->constructors_[ map_MachPhysRace_to_size_t( r ) ];
}

///////////////////////////////////

MachLogRaces::GeoLocators&
MachLogRaces::geoLocators( MachPhys::Race r )
{
	return pDataImpl_->geoLocators_[ map_MachPhysRace_to_size_t( r ) ];
}

const MachLogRaces::GeoLocators&
MachLogRaces::geoLocators( MachPhys::Race r ) const
{
	return pDataImpl_->geoLocators_[ map_MachPhysRace_to_size_t( r ) ];
}

///////////////////////////////////
///////////////////////////////////

MachLogRaces::SpyLocators&
MachLogRaces::spyLocators( MachPhys::Race r )
{
	return pDataImpl_->spyLocators_[ map_MachPhysRace_to_size_t( r ) ];
}

const MachLogRaces::SpyLocators&
MachLogRaces::spyLocators( MachPhys::Race r ) const
{
	return pDataImpl_->spyLocators_[ map_MachPhysRace_to_size_t( r ) ];
}

///////////////////////////////////

MachLogRaces::ResourceCarriers&
MachLogRaces::resourceCarriers( MachPhys::Race r )
{
	return pDataImpl_->resourceCarriers_[ map_MachPhysRace_to_size_t( r ) ];
}

const MachLogRaces::ResourceCarriers&
MachLogRaces::resourceCarriers( MachPhys::Race r ) const
{
	return pDataImpl_->resourceCarriers_[ map_MachPhysRace_to_size_t( r ) ];
}

///////////////////////////////////

MachLogRaces::APCs&
MachLogRaces::apcs( MachPhys::Race r )
{
	return pDataImpl_->apcs_[ map_MachPhysRace_to_size_t( r ) ];
}

const MachLogRaces::APCs&
MachLogRaces::apcs( MachPhys::Race r ) const
{
	return pDataImpl_->apcs_[ map_MachPhysRace_to_size_t( r ) ];
}

/* //////////////////////////////////////////////////////////////// */

MachLogRaces::Garrisons&
MachLogRaces::garrisons( MachPhys::Race r )
{
	return pDataImpl_->garrisons_[ map_MachPhysRace_to_size_t( r ) ];
}

const MachLogRaces::Garrisons&
MachLogRaces::garrisons( MachPhys::Race r ) const
{
	return pDataImpl_->garrisons_[ map_MachPhysRace_to_size_t( r ) ];
}

///////////////////////////////////

MachLogRaces::HardwareLabs&
MachLogRaces::hardwareLabs( MachPhys::Race r )
{
	return pDataImpl_->hardwareLabs_[ map_MachPhysRace_to_size_t( r ) ];
}

const MachLogRaces::HardwareLabs&
MachLogRaces::hardwareLabs( MachPhys::Race r ) const
{
	return pDataImpl_->hardwareLabs_[ map_MachPhysRace_to_size_t( r ) ];
}

///////////////////////////////////

///////////////////////////////////

MachLogRaces::Factories&
MachLogRaces::factories( MachPhys::Race r )
{
	return pDataImpl_->factories_[ map_MachPhysRace_to_size_t( r ) ];
}

const MachLogRaces::Factories&
MachLogRaces::factories( MachPhys::Race r ) const
{
	return pDataImpl_->factories_[ map_MachPhysRace_to_size_t( r ) ];
}

///////////////////////////////////

MachLogRaces::Smelters&
MachLogRaces::smelters( MachPhys::Race r )
{
	return pDataImpl_->smelters_[ map_MachPhysRace_to_size_t( r ) ];
}

const MachLogRaces::Smelters&
MachLogRaces::smelters( MachPhys::Race r ) const
{
	return pDataImpl_->smelters_[ map_MachPhysRace_to_size_t( r ) ];
}

///////////////////////////////////

MachLogRaces::Mines&
MachLogRaces::mines( MachPhys::Race r )
{
	return pDataImpl_->mines_[ map_MachPhysRace_to_size_t( r ) ];
}

const MachLogRaces::Mines&
MachLogRaces::mines( MachPhys::Race r ) const
{
	return pDataImpl_->mines_[ map_MachPhysRace_to_size_t( r ) ];
}

///////////////////////////////////

MachLogRaces::MissileEmplacements&
MachLogRaces::missileEmplacements( MachPhys::Race r )
{
	return pDataImpl_->missileEmplacements_[ map_MachPhysRace_to_size_t( r ) ];
}

const MachLogRaces::MissileEmplacements&
MachLogRaces::missileEmplacements( MachPhys::Race r ) const
{
	return pDataImpl_->missileEmplacements_[ map_MachPhysRace_to_size_t( r ) ];
}

///////////////////////////////////

MachLogRaces::Holographs&
MachLogRaces::holographs( MachPhys::Race r )
{
	return pDataImpl_->holographs_[ map_MachPhysRace_to_size_t( r ) ];
}

const MachLogRaces::Holographs&
MachLogRaces::holographs( MachPhys::Race r ) const
{
	return pDataImpl_->holographs_[ map_MachPhysRace_to_size_t( r ) ];
}

///////////////////////////////////

MachLogRaces::Beacons&
MachLogRaces::beacons( MachPhys::Race r )
{
	return pDataImpl_->beacons_[ map_MachPhysRace_to_size_t( r ) ];
}

const MachLogRaces::Beacons&
MachLogRaces::beacons( MachPhys::Race r ) const
{
	return pDataImpl_->beacons_[ map_MachPhysRace_to_size_t( r ) ];
}

///////////////////////////////////

MachLogRaces::Pods&
MachLogRaces::pods( MachPhys::Race r )
{
	return pDataImpl_->pods_[ map_MachPhysRace_to_size_t( r ) ];
}

const MachLogRaces::Pods&
MachLogRaces::pods( MachPhys::Race r ) const
{
	return pDataImpl_->pods_[ map_MachPhysRace_to_size_t( r ) ];
}

///////////////////////////////////
MachLogRaces::Artefacts&
MachLogRaces::artefactsCollection()
{
	return pDataImpl_->artefactsCollection_;
}

const MachLogRaces::Artefacts&
MachLogRaces::artefactsCollection() const
{
	return pDataImpl_->artefactsCollection_;
}


///////////////////////////////////

MachLogRaces::Squadrons&
MachLogRaces::squadrons( MachPhys::Race r )
{
	return pDataImpl_->squadrons_[ map_MachPhysRace_to_size_t( r ) ];
}

const MachLogRaces::Squadrons&
MachLogRaces::squadrons( MachPhys::Race r ) const
{
	return pDataImpl_->squadrons_[ map_MachPhysRace_to_size_t( r ) ];
}

bool MachLogRaces::freeSquadron( MachPhys::Race r, MachLogSquadron** ppSquadron )
{
	for( Squadrons::iterator i = pDataImpl_->squadrons_[ map_MachPhysRace_to_size_t( r ) ].begin();
		i != pDataImpl_->squadrons_[ map_MachPhysRace_to_size_t( r ) ].end(); ++i )
			if( (*i)->machines().size() == 0 )
			{
				*ppSquadron = (*i);
				return true;
			}
	return false;
}

//snapshot of race statistics
//these variables will be altered as and when needed by the various MLclasses as needed
int
MachLogRaces::nConstructions( MachPhys::Race r ) const
{
	return pDataImpl_->nConstructions_[ map_MachPhysRace_to_size_t( r ) ];
}

int
MachLogRaces::nMachines( MachPhys::Race r ) const
{
	return pDataImpl_->nMachines_[ map_MachPhysRace_to_size_t( r ) ];
}

MachPhys::BuildingMaterialUnits
MachLogRaces::nBuildingMaterialUnits( MachPhys::Race r ) const
{
	return pDataImpl_->nBuildingMaterialUnits_[ map_MachPhysRace_to_size_t( r ) ];
}

MachPhys::BuildingMaterialUnits
MachLogRaces::nOre( MachPhys::Race r ) const
{
	return pDataImpl_->nOre_[ map_MachPhysRace_to_size_t( r ) ];
}

MachPhys::BuildingMaterialUnits
MachLogRaces::nMaxBuildingMaterialUnits( MachPhys::Race r ) const
{
	return pDataImpl_->nMaxBuildingMaterialUnits_[ map_MachPhysRace_to_size_t( r ) ];
}

MachPhys::BuildingMaterialUnits
MachLogRaces::nMaxOre( MachPhys::Race r ) const
{
	return pDataImpl_->nMaxOre_[ map_MachPhysRace_to_size_t( r ) ];
}

MachPhys::BuildingMaterialUnits MachLogRaces::smartAddBMUs( MachPhys::Race r, MachPhys::BuildingMaterialUnits amountToAdd )
{
	PRE( amountToAdd >= 0 );

	MachPhys::BuildingMaterialUnits currentBMUs = nBuildingMaterialUnits( r );

	if( currentBMUs + amountToAdd > nMaxBuildingMaterialUnits( r ) )
	{
		amountToAdd = nMaxBuildingMaterialUnits( r ) - currentBMUs;
	}

	if( amountToAdd < 0 )
	{
		amountToAdd = 0;
	}

	nBuildingMaterialUnits( r, currentBMUs + amountToAdd );
	MachLogRaces::instance().BMUsMinedScoreAdjust( r, amountToAdd );

	if( pcController().race() == r
		and smeltersFull( r ) )
	{
		PhysAbsoluteTime timeNow = SimManager::instance().currentTime();
		if(	timeNow > pDataImpl_->lastFullSmeltersUpdateTime_ )
		{
			pDataImpl_->lastFullSmeltersUpdateTime_ = timeNow + 120.0;

			bool hasAtLeastOneSmelter = false;

			if( smelters( r ).size() + pods( r ).size() > 0 )
			{
				// need to make sure that at least one of these is actually complete.
				for( MachLogRaces::Smelters::const_iterator i = smelters( r ).begin();
					not hasAtLeastOneSmelter and i != smelters( r ).end();
					++i )
				{
					if( (*i)->isComplete() )
						hasAtLeastOneSmelter = true;
				}
				for( MachLogRaces::Pods::const_iterator i = pods( r ).begin();
					not hasAtLeastOneSmelter and i != pods( r ).end();
					++i)
				{
					if( (*i)->isComplete() )
						hasAtLeastOneSmelter = true;
				}

			}

			// give voicemail. Type given depends on whether we already have one smelter or not.
			if( hasAtLeastOneSmelter )
				MachLogVoiceMailManager::instance().postNewMail( VID_POD_NEED_FURTHER_SMELTERS, r );
			else
				MachLogVoiceMailManager::instance().postNewMail( VID_POD_NEED_AT_LEAST_ONE_SMELTER, r );
		}
	}

	return amountToAdd;
}

MachPhys::BuildingMaterialUnits MachLogRaces::smartSubtractBMUs( MachPhys::Race r, MachPhys::BuildingMaterialUnits amountToSubtract )
{
	PRE( amountToSubtract >= 0 );

	MachPhys::BuildingMaterialUnits currentBMUs = nBuildingMaterialUnits( r );

	ASSERT_INFO( r );
	ASSERT_INFO( currentBMUs );
	ASSERT_INFO( amountToSubtract );
	ASSERT( currentBMUs >= 0, "Race BMUs total should never be less than 0" );

	// Can't remove more BMUs than we have in the bank....
	amountToSubtract = std::min( amountToSubtract, currentBMUs );

	nBuildingMaterialUnits( r, currentBMUs  - amountToSubtract );

	if( pcController().race() == r
		and nBuildingMaterialUnits( r ) < 100 )
	{
		PhysAbsoluteTime timeNow = SimManager::instance().currentTime();
		if(	timeNow > pDataImpl_->lastLowBMUsUpdateTime_ )
		{
			pDataImpl_->lastLowBMUsUpdateTime_ = timeNow + 120.0;
			if( nBuildingMaterialUnits( r ) == 0 )
				MachLogVoiceMailManager::instance().postNewMail( VID_POD_NO_BMUS_LEFT, r );
			else
				MachLogVoiceMailManager::instance().postNewMail( VID_POD_BMUS_LOW, r );
		}
	}

	ASSERT( nBuildingMaterialUnits( r ) >= 0, "Race BMUs total should never be less than 0" );

	return amountToSubtract;
}

MachPhys::BuildingMaterialUnits MachLogRaces::reduceCapacityWithPenalty( MachPhys::Race r, MachPhys::BuildingMaterialUnits amountReducedBy )
{
	PRE( amountReducedBy <= nMaxBuildingMaterialUnits( r ) );

	MachPhys::BuildingMaterialUnits currentMax = nMaxBuildingMaterialUnits( r );
	// actual BMUs owned gets reduced by the loss of capacity

	float percentCapacityLost = (float)amountReducedBy / (float)currentMax;

	ASSERT( percentCapacityLost <= 1.0, "Cannot have lost more than 100% of capacity!" );

	MachPhys::BuildingMaterialUnits currentBMUs = nBuildingMaterialUnits( r );

	nBuildingMaterialUnits( r, currentBMUs * (1.0 - percentCapacityLost) );
	currentMax -= amountReducedBy;

	ASSERT( currentMax >= 0, "Current max cannot be less than 0." );

	nMaxBuildingMaterialUnits( r ) = currentMax;
	return currentMax;
}

MachPhys::BuildingMaterialUnits MachLogRaces::reduceCapacityNoPenalty( MachPhys::Race r, MachPhys::BuildingMaterialUnits amountReducedBy )
{
	PRE( amountReducedBy <= nMaxBuildingMaterialUnits( r ) );

	MachPhys::BuildingMaterialUnits currentMax = nMaxBuildingMaterialUnits( r );
   	currentMax -= amountReducedBy;

	nMaxBuildingMaterialUnits( r ) = currentMax;
	if( nBuildingMaterialUnits( r ) > currentMax )
	{
		nBuildingMaterialUnits( r, currentMax );
	}

	return currentMax;
}


//non-const ref :: snapshot of race statistics
//these variables will be altered as and when needed by the various MLclasses as needed
int&
MachLogRaces::nConstructions( MachPhys::Race r )
{
	return pDataImpl_->nConstructions_[ map_MachPhysRace_to_size_t( r ) ];
}

int&
MachLogRaces::nMachines( MachPhys::Race r )
{
	return pDataImpl_->nMachines_[ map_MachPhysRace_to_size_t( r ) ];
}

void
MachLogRaces::nBuildingMaterialUnits( MachPhys::Race r, MachPhys::BuildingMaterialUnits newValue )
{
	PRE( newValue >= 0 );

	pDataImpl_->nBuildingMaterialUnits_[ map_MachPhysRace_to_size_t( r ) ] = newValue;
}

MachPhys::BuildingMaterialUnits&
MachLogRaces::nOre( MachPhys::Race r )
{
	return pDataImpl_->nOre_[ map_MachPhysRace_to_size_t( r ) ];
}

MachPhys::BuildingMaterialUnits&
MachLogRaces::nMaxBuildingMaterialUnits( MachPhys::Race r )
{
	return pDataImpl_->nMaxBuildingMaterialUnits_[ map_MachPhysRace_to_size_t( r ) ];
}

MachPhys::BuildingMaterialUnits&
MachLogRaces::nMaxOre( MachPhys::Race r )
{
	return pDataImpl_->nMaxOre_[ map_MachPhysRace_to_size_t( r ) ];
}

///////////////////////////////////

MachLogRace&
MachLogRaces::race( MachPhys::Race r )
{
	return (r == MachPhys::NORACE ? *pDataImpl_->pNoRaceRace_
	                              : *pDataImpl_->race_[ map_MachPhysRace_to_size_t( r ) ]);
}

void
MachLogRaces::race( MachPhys::Race r, MachLogRace * pRace, MachLogRaces::CreateDefaultSquadrons doCreate )
{
    if( r == MachPhys::NORACE )
    {
        pDataImpl_->pNoRaceRace_ = pRace;
    }
    else
    {
    	pDataImpl_->race_[ map_MachPhysRace_to_size_t( r ) ] = pRace;
		pDataImpl_->raceInGame_[ r ] = true;
		if( doCreate == MachLogRaces::CREATE_SQUADRONS )
		{
	    	for( int i = 0; i < 10; ++i )
	    	{
	    	    W4dGeneric* pPhysObject = _NEW( W4dGeneric( &MachLogPlanet::instance().hiddenRoot(), MexTransform3d() ) );
	    		pDataImpl_->squadrons_[ map_MachPhysRace_to_size_t( r ) ].push_back( _NEW( MachLogSquadron( pRace, pPhysObject, i + 1 ) ) );
	    	}
		}
    }
}

void MachLogRaces::addToSquadrons( const MachLogSquadron& squad )
{
	pDataImpl_->squadrons_[ map_MachPhysRace_to_size_t( squad.race() ) ].push_back( &_CONST_CAST( MachLogSquadron&, squad ) );
}

const MachLogRace&
MachLogRaces::race( MachPhys::Race r ) const
{
	return (r == MachPhys::NORACE ? *pDataImpl_->pNoRaceRace_
	                              : *pDataImpl_->race_[ map_MachPhysRace_to_size_t( r ) ]);
}

void MachLogRaces::addExtraSquadron( MachPhys::Race r )
{
    W4dGeneric* pPhysObject = _NEW( W4dGeneric( &MachLogPlanet::instance().hiddenRoot(), MexTransform3d() ) );
	int id = pDataImpl_->squadrons_[ map_MachPhysRace_to_size_t( r ) ].size() + 1;
	pDataImpl_->squadrons_[ map_MachPhysRace_to_size_t( r ) ].push_back( _NEW( MachLogSquadron( pDataImpl_->race_[ map_MachPhysRace_to_size_t( r ) ], pPhysObject, id ) ) );
}

/* //////////////////////////////////////////////////////////////// */

MachLogController&
MachLogRaces::controller( MachPhys::Race r )
{
	return *pDataImpl_->controller_[ map_MachPhysRace_to_size_t( r ) ];
}

void
MachLogRaces::controller( MachPhys::Race r, MachLogController * pCtl )
{
	pDataImpl_->controller_[ map_MachPhysRace_to_size_t( r ) ] = pCtl;
}

const MachLogController&
MachLogRaces::controller( MachPhys::Race r ) const
{
	return *pDataImpl_->controller_[ map_MachPhysRace_to_size_t( r ) ];
}

MachLogAIController&
MachLogRaces::AIController( MachPhys::Race r )
{
	ASSERT( pDataImpl_->controller_[ map_MachPhysRace_to_size_t( r ) ]->type() == MachLogController::AI_CONTROLLER,
		"Trying to cast non-AI Controller to AI Controller\n" );
	return _STATIC_CAST( MachLogAIController&, *pDataImpl_->controller_[ map_MachPhysRace_to_size_t( r ) ] );
}

const MachLogAIController&
MachLogRaces::AIController( MachPhys::Race r ) const
{
	ASSERT( pDataImpl_->controller_[ map_MachPhysRace_to_size_t( r ) ]->type() == MachLogController::AI_CONTROLLER,
		"Trying to cast non-AI Controller to AI Controller\n" );
	return _STATIC_CAST( const MachLogAIController&, *pDataImpl_->controller_[ map_MachPhysRace_to_size_t( r ) ] );
}

/* //////////////////////////////////////////////////////////////// */

/* //////////////////////////////////////////////////////////////// */

// static
size_t
MachLogRaces::map_MachPhysRace_to_size_t( MachPhys::Race race )
{
	size_t result = 0;
	ASSERT_INFO( (int)race );
	switch( race )
	{
		case MachPhys::RED		: result = 0;		break;
		case MachPhys::BLUE		: result = 1;		break;
		case MachPhys::GREEN	: result = 2;		break;
		case MachPhys::YELLOW	: result = 3;		break;
		default		: ASSERT_BAD_CASE;	break;
	}

	POST( result < MachPhys::N_RACES );
	return result;
}

/* //////////////////////////////////////////////////////////////// */

template< class CONTAINER, class T >
void
races_ctl_erase( CONTAINER *cPtr, const T& t )
{
	cPtr->erase( remove( cPtr->begin(), cPtr->end(), t ), cPtr->end() );
}

/* //////////////////////////////////////////////////////////////// */

void MachLogRaces::remove( MachActor * pObj )
{
    PRE( pObj != NULL );

    //Do the catchall objects collections
    ctl_erase_first_instance( &objects(), pObj );

    MachPhys::Race objectRace = pObj->race();
    if( objectRace != MachPhys::N_RACES  and  objectRace != MachPhys::NORACE )
        ctl_erase_first_instance( &raceObjects( objectRace ), pObj );

    if( pObj->objectIsMachine() )
    {
        MachLogMachine& machine = pObj->asMachine();

        ctl_erase_first_instance( &machines(), &machine );

		// take care of bookkeeping here
		--nMachines( pObj->race() );
    }
    else if( pObj->objectIsArtefact() )
    {
        Artefacts::iterator it = find( artefactsCollection().begin(), artefactsCollection().end(), &pObj->asArtefact() );
        if( it != artefactsCollection().end() )
            artefactsCollection().erase( it );
    }

    switch( pObj->objectType() )
    {
        case MachLog::ADMINISTRATOR:
        {
            ctl_erase_first_instance( &administrators( objectRace ), &pObj->asAdministrator() );
            break;
        }
        case MachLog::AGGRESSOR:
        {
            ctl_erase_first_instance( &aggressors( objectRace ), &pObj->asAggressor() );
            break;
        }
        case MachLog::CONSTRUCTOR:
        {
            ctl_erase_first_instance( &constructors( objectRace ), &pObj->asConstructor() );
            break;
        }
        case MachLog::GEO_LOCATOR:
        {
            ctl_erase_first_instance( &geoLocators( objectRace ), &pObj->asGeoLocator() );
            break;
        }
        case MachLog::SPY_LOCATOR:
        {
            ctl_erase_first_instance( &spyLocators( objectRace ), &pObj->asSpyLocator() );
            break;
        }
        case MachLog::TECHNICIAN:
        {
            ctl_erase_first_instance( &technicians( objectRace ), &pObj->asTechnician() );
            break;
        }
        case MachLog::RESOURCE_CARRIER:
        {
            ctl_erase_first_instance( &resourceCarriers( objectRace ), &pObj->asResourceCarrier() );
            break;
        }
        case MachLog::APC:
        {
            ctl_erase_first_instance( &apcs( objectRace ), &pObj->asAPC() );
            break;
        }
        case MachLog::BEACON:
        {
            ctl_erase_first_instance( &beacons( objectRace ), &pObj->asBeacon() );
            break;
        }
        case MachLog::FACTORY:
        {
            ctl_erase_first_instance( &factories( objectRace ), &pObj->asFactory() );
            break;
        }
        case MachLog::GARRISON:
        {
            ctl_erase_first_instance( &garrisons( objectRace ), &pObj->asGarrison() );
            break;
        }
        case MachLog::HARDWARE_LAB:
        {
            ctl_erase_first_instance( &hardwareLabs( objectRace ), &pObj->asHardwareLab() );
            break;
        }
        case MachLog::ORE_HOLOGRAPH:
        {
            ctl_erase_first_instance( &holographs( objectRace ), &pObj->asOreHolograph() );
            break;
        }
        case MachLog::POD:
        {
            ctl_erase_first_instance( &pods( objectRace ), &pObj->asPod() );
            break;
        }
        case MachLog::MINE:
        {
            ctl_erase_first_instance( &mines( objectRace ), &pObj->asMine() );
            break;
        }
        case MachLog::MISSILE_EMPLACEMENT:
        {
            ctl_erase_first_instance( &missileEmplacements( objectRace ), &pObj->asMissileEmplacement() );
            break;
        }
        case MachLog::SMELTER:
        {
            ctl_erase_first_instance( &smelters( objectRace ), &pObj->asSmelter() );
            break;
        }
    }

    //Remove from the actor id map
    pDataImpl_->pActorIdMap_->remove( pObj->id() );
}

/* //////////////////////////////////////////////////////////////// */

bool MachLogRaces::findTargetClosestTo( const MachActor& actor,
										MachActor** ppResult,
										MachLog::TargetSystemType targetSystemType,
										MachLog::ObjectType ot,
										const MexPoint3d& startingPoint,
										MATHEX_SCALAR minEuclidianDistance ) const
{
	PRE( actor.objectIsCanAttack() );

	bool found = false;
	bool targetResource = false;
	bool isLocalHumanPlayer = ( actor.race() == pcController().race() );
	MexPoint3d pos = actor.position();
	MATHEX_SCALAR closest = 1000000000;
	const MachLogCanAttack& actorAsCanAttack = actor.asCanAttack();

	//if the startingPoint provided is all zeros then do not override actor.position otherwise do..
	if( startingPoint != MexPoint3d(0,0,0) )
		pos = startingPoint;

	if( targetSystemType == MachLog::TARGET_RESOURCE )
		targetResource = true;

	if( ot == MachLog::ARTEFACT )
	{
		for( Artefacts::const_iterator i = artefactsCollection().begin(); i != artefactsCollection().end(); ++i )
		{
			MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

			if( dist < closest
				and dist >= minEuclidianDistance
				and actorAsCanAttack.canFireAt( (**i) )
				and not (*i)->isDead() )
			{
				found = true;
				closest = dist;
				*ppResult = *i;
			}
		}
		return found;
	}

	//if the targetresource flag is set then only try mines+smelters+carriers
	if( targetResource )
	{
		for( MachPhys::Race ridx = MachPhys::RED; ridx != MachPhys::N_RACES; ++((int&)ridx) )
		{
			MachLogRaces::DispositionToRace disp = dispositionToRace( actor.race(), ridx );

			if( disp != MachLogRaces::ALLY and
				disp != MachLogRaces::OUR_RACE )
			{
				for( ResourceCarriers::const_iterator i = resourceCarriers( ridx ).begin(); i != resourceCarriers( ridx ).end(); ++i )
				{
					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );
					if( dist < closest
						and dist >= minEuclidianDistance
						and actorAsCanAttack.canFireAt( (**i) )
						and (not (*i)->isDead() ) )
					{
						found = true;
						closest = dist;
						*ppResult = *i;
					}
				}
				for( Mines::const_iterator i = mines( ridx ).begin(); i != mines( ridx ).end(); ++i )
				{
					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );
					if( dist < closest
						and dist >= minEuclidianDistance
						and actorAsCanAttack.canFireAt( (**i) )
						and (not (*i)->isDead() ) )
					{
						found = true;
						closest = dist;
						*ppResult = *i;
					}
				}
				for( Smelters::const_iterator i = smelters( ridx ).begin(); i != smelters( ridx ).end(); ++i )
				{
					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );
					if( dist < closest
						and dist >= minEuclidianDistance
						and actorAsCanAttack.canFireAt( (**i) )
						and (not (*i)->isDead() ) )
					{
						found = true;
						closest = dist;
						*ppResult = *i;
					}
				}
			}
		}
		return found;
	}

	MATHEX_SCALAR sqrScannerRange = 2500;

	if( actor.objectIsMachine() )
		sqrScannerRange = sqr( actor.asMachine().effectiveScannerRange() );
	else if( actor.objectIsMissileEmplacement() )
		sqrScannerRange = sqr( actor.asMissileEmplacement().scannerRange() );

	for( MachPhys::Race ridx = MachPhys::RED; ridx != MachPhys::N_RACES; ++((int&)ridx) )
	{
		MachLogRaces::DispositionToRace disp = dispositionToRace( actor.race(), ridx );

		if( disp != MachLogRaces::ALLY and
			disp != MachLogRaces::OUR_RACE )
		{
			if( targetSystemType == MachLog::TARGET_NORMAL or
				ot == MachLog::AGGRESSOR )
				for( Aggressors::const_iterator i = aggressors( ridx ).begin(); i != aggressors( ridx ).end(); ++i )
				{
					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

					if( dist < closest
						and dist >= minEuclidianDistance
						and actorAsCanAttack.canFireAt( (**i) )
						and not (*i)->isDead() )
					{
						found = true;
						closest = dist;
						*ppResult = *i;
					}
				}

			if( targetSystemType == MachLog::TARGET_NORMAL or
				ot == MachLog::ADMINISTRATOR )
				for( Administrators::const_iterator i = administrators( ridx ).begin(); i != administrators( ridx ).end(); ++i )
				{
					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

					if( dist < closest
						and dist >= minEuclidianDistance
						and actorAsCanAttack.canFireAt( (**i) )
						and not (*i)->isDead() )
					{
						found = true;
						closest = dist;
						*ppResult = *i;
					}
				}

			// Check for non-ICBM turrets
			if( targetSystemType == MachLog::TARGET_NORMAL or
				ot == MachLog::MISSILE_EMPLACEMENT )
				for( MissileEmplacements::const_iterator i = missileEmplacements( ridx ).begin(); i != missileEmplacements( ridx ).end(); ++i )
				{
					MachLogMissileEmplacement* pTurret = (*i);
					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( pTurret->position() );

					if( dist < closest
						and dist >= minEuclidianDistance
						and pTurret->subType() != MachPhys::ICBM
						and actorAsCanAttack.canFireAt( (*pTurret) )
						and ( not( isLocalHumanPlayer ) or pTurret->isComplete() )  // note that local player machines will not consider incomplete turrets as viable targets
						and not pTurret->isDead() )
					{
						found = true;
						closest = dist;
						*ppResult = pTurret;
					}
				}

			// --------- EFFICIENCY-RELATED CHANGE ------------------
			// aggressors, administrators and turrets are our prime targets - stop here if we now have a match within
			// scanner range (ends search for this particular race)
			if( found
				and MachPhysRandom::randomInt( 0, 10 ) != 0		// 10% chance of checking everything regardless
				and not actor.objectIsMissileEmplacement()			// Missile emps always find ACTUAL closest target
				and not( actor.objectIsMachine() and actor.asMachine().insideBuilding() ) // For god's sake, check for technicians etc. if we're inside a building!
				and closest < std::min( sqrScannerRange, 22500.0 ) )		// 150m is furthest away aggressive can be if we are to abort the search
				continue;
			// --------- EFFICIENCY-RELATED CHANGE ------------------


			if( targetSystemType == MachLog::TARGET_NORMAL or
				ot == MachLog::TECHNICIAN )
				for( Technicians::const_iterator i = technicians( ridx ).begin(); i != technicians( ridx ).end(); ++i )
				{
					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

					if( dist < closest
						and dist >= minEuclidianDistance
						and actorAsCanAttack.canFireAt( (**i) )
						and not (*i)->isDead() )
					{
						found = true;
						closest = dist;
						*ppResult = *i;
					}
				}

			if( targetSystemType == MachLog::TARGET_NORMAL or
				ot == MachLog::CONSTRUCTOR )
				for( Constructors::const_iterator i = constructors( ridx ).begin(); i != constructors( ridx ).end(); ++i )
				{
					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

					if( dist < closest
						and dist >= minEuclidianDistance
						and actorAsCanAttack.canFireAt( (**i) )
						and not (*i)->isDead() )
					{
						found = true;
						closest = dist;
						*ppResult = *i;
					}
				}

			if( targetSystemType == MachLog::TARGET_NORMAL or
				ot == MachLog::GEO_LOCATOR )
				for( GeoLocators::const_iterator i = geoLocators( ridx ).begin(); i != geoLocators( ridx ).end(); ++i )
				{
					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

					if( dist < closest
						and dist >= minEuclidianDistance
						and actorAsCanAttack.canFireAt( (**i) )
						and not (*i)->isDead() )
					{
						found = true;
						closest = dist;
						*ppResult = *i;
					}
				}

			if( targetSystemType == MachLog::TARGET_NORMAL or
				ot == MachLog::SPY_LOCATOR )
				for( SpyLocators::const_iterator i = spyLocators( ridx ).begin(); i != spyLocators( ridx ).end(); ++i )
				{
					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

					if( dist < closest
						and dist >= minEuclidianDistance
						and actorAsCanAttack.canFireAt( (**i) )
						and not (*i)->isDead()
						and not (*i)->camouflaged() )	// special clause for spies
					{
						found = true;
						closest = dist;
						*ppResult = *i;
					}
				}

			if( targetSystemType == MachLog::TARGET_NORMAL or
				ot == MachLog::RESOURCE_CARRIER )
				for( ResourceCarriers::const_iterator i = resourceCarriers( ridx ).begin(); i != resourceCarriers( ridx ).end(); ++i )
				{
					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

					if( dist < closest
						and dist >= minEuclidianDistance
						and actorAsCanAttack.canFireAt( (**i) )
						and not (*i)->isDead() )
					{
						found = true;
						closest = dist;
						*ppResult = *i;
					}
				}

			if( targetSystemType == MachLog::TARGET_NORMAL or
				ot == MachLog::APC )
				for( APCs::const_iterator i = apcs( ridx ).begin(); i != apcs( ridx ).end(); ++i )
				{
					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

					if( dist < closest
						and dist >= minEuclidianDistance
						and actorAsCanAttack.canFireAt( (**i) )
						and not (*i)->isDead() )
					{
						found = true;
						closest = dist;
						*ppResult = *i;
					}
				}

			////////////////////////////////////// BEGIN AI RACES ONLY SECTION /////////////////////////////////////

			// only AI races also check through constructions and the like if none of the more viable targets
			// above were found
			if( /*not found and*/ controller( actor.race() ).type() == MachLogController::AI_CONTROLLER )
			{
				if( targetSystemType == MachLog::TARGET_NORMAL or
					ot == MachLog::BEACON )
					for( Beacons::const_iterator i = beacons( ridx ).begin(); i != beacons( ridx ).end(); ++i )
					{
						MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

						if( dist < closest
							and dist >= minEuclidianDistance
							and actorAsCanAttack.canFireAt( (**i) )
							and not (*i)->isDead() )
						{
							found = true;
							closest = dist;
							*ppResult = *i;
						}
					}

				if( targetSystemType == MachLog::TARGET_NORMAL or
					ot == MachLog::FACTORY )
					for( Factories::const_iterator i = factories( ridx ).begin(); i != factories( ridx ).end(); ++i )
					{
						MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

						if( dist < closest
							and dist >= minEuclidianDistance
							and actorAsCanAttack.canFireAt( (**i) )
							and not (*i)->isDead() )
						{
							found = true;
							closest = dist;
							*ppResult = *i;
						}
					}

				if( targetSystemType == MachLog::TARGET_NORMAL or
					ot == MachLog::GARRISON )
					for( Garrisons::const_iterator i = garrisons( ridx ).begin(); i != garrisons( ridx ).end(); ++i )
					{
						MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

						if( dist < closest
							and dist >= minEuclidianDistance
							and actorAsCanAttack.canFireAt( (**i) )
							and not (*i)->isDead() )
						{
							found = true;
							closest = dist;
							*ppResult = *i;
						}
					}

				if( targetSystemType == MachLog::TARGET_NORMAL or
					ot == MachLog::HARDWARE_LAB )
					for( HardwareLabs::const_iterator i = hardwareLabs( ridx ).begin(); i != hardwareLabs( ridx ).end(); ++i )
					{
						MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

						if( dist < closest
							and dist >= minEuclidianDistance
							and actorAsCanAttack.canFireAt( (**i) )
							and not (*i)->isDead() )
						{
							found = true;
							closest = dist;
							*ppResult = *i;
						}
					}

				if( targetSystemType == MachLog::TARGET_NORMAL or
					ot == MachLog::POD )
					for( Pods::const_iterator i = pods( ridx ).begin(); i != pods( ridx ).end(); ++i )
					{
						MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

						if( dist < closest
							and dist >= minEuclidianDistance
							and actorAsCanAttack.canFireAt( (**i) )
							and not (*i)->isDead() )
						{
							found = true;
							closest = dist;
							*ppResult = *i;
						}
					}

				if( targetSystemType == MachLog::TARGET_NORMAL or
					ot == MachLog::MINE )
					for( Mines::const_iterator i = mines( ridx ).begin(); i != mines( ridx ).end(); ++i )
					{
						MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

						if( dist < closest
							and dist >= minEuclidianDistance
							and actorAsCanAttack.canFireAt( (**i) )
							and not (*i)->isDead() )
						{
							found = true;
							closest = dist;
							*ppResult = *i;
						}
					}

				if( targetSystemType == MachLog::TARGET_NORMAL or
					ot == MachLog::SMELTER )
					for( Smelters::const_iterator i = smelters( ridx ).begin(); i != smelters( ridx ).end(); ++i )
					{
						MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

						if( dist < closest
							and dist >= minEuclidianDistance
							and actorAsCanAttack.canFireAt( (**i) )
							and not (*i)->isDead() )
						{
							found = true;
							closest = dist;
							*ppResult = *i;
						}
					}

				// check for ICBM turrets only

				if( targetSystemType == MachLog::TARGET_NORMAL or
					ot == MachLog::MISSILE_EMPLACEMENT )
					for( MissileEmplacements::const_iterator i = missileEmplacements( ridx ).begin(); i != missileEmplacements( ridx ).end(); ++i )
					{
						MachLogMissileEmplacement* pEmplacement = (*i);
   						MATHEX_SCALAR dist = pos.sqrEuclidianDistance( pEmplacement->position() );

						if( pEmplacement->subType() == MachPhys::ICBM
							and dist < closest
							and dist >= minEuclidianDistance
							and actorAsCanAttack.canFireAt( *pEmplacement )
							and not pEmplacement->isDead() )
						{
							found = true;
							closest = dist;
							*ppResult = pEmplacement;
						}
					}
			}
			////////////////////////////////////// END AI RACES ONLY SECTION /////////////////////////////////////
		}
	}

	return found;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogRaces::findFriendlyClosestTo( const MachActor& actor,
										MachActor** ppResult,
										MachLog::TargetSystemType targetSystemType,
										MachLog::ObjectType ot ) const
{
	bool found = false;
	bool targetObject = false;
	MexPoint3d pos = actor.position();
	MATHEX_SCALAR closest = 1000000000;

	if( targetSystemType == MachLog::TARGET_OBJECT )
		targetObject = true;

	for( MachPhys::Race ridx = MachPhys::RED; ridx != MachPhys::N_RACES; ++((int&)ridx) )
	{
		MachLogRaces::DispositionToRace disp = dispositionToRace( actor.race(), ridx );

		if( disp == MachLogRaces::ALLY or disp == MachLogRaces::OUR_RACE )
		{
			if( targetSystemType == MachLog::TARGET_NORMAL or
				ot == MachLog::TECHNICIAN )
				for( Technicians::const_iterator i = technicians( ridx ).begin(); i != technicians( ridx ).end(); ++i )
				{
			//        if( (*i)->motionSeq().isUsingSpaceDomain() )
			//            continue;

					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );
					if(  dist < closest and (*i) != &actor )
					{
						found = true;
						closest = dist;
						*ppResult = *i;
					}
				}

			if( targetSystemType == MachLog::TARGET_NORMAL or
				ot == MachLog::ADMINISTRATOR )
				for( Administrators::const_iterator i = administrators( ridx ).begin(); i != administrators( ridx ).end(); ++i )
				{
					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );
					if(  dist < closest and (*i) != &actor )
					{
						found = true;
						closest = dist;
						*ppResult = *i;
					}
				}

			if( targetSystemType == MachLog::TARGET_NORMAL or
				ot == MachLog::CONSTRUCTOR )
				for( Constructors::const_iterator i = constructors( ridx ).begin(); i != constructors( ridx ).end(); ++i )
				{
					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );
					if(  dist < closest and (*i) != &actor )
					{
						found = true;
						closest = dist;
						*ppResult = *i;
					}
				}

			if( targetSystemType == MachLog::TARGET_NORMAL or
				ot == MachLog::GEO_LOCATOR )
				for( GeoLocators::const_iterator i = geoLocators( ridx ).begin(); i != geoLocators( ridx ).end(); ++i )
				{
					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );
					if(  dist < closest and (*i) != &actor )
					{
						found = true;
						closest = dist;
						*ppResult = *i;
					}
				}

			if( targetSystemType == MachLog::TARGET_NORMAL or
				ot == MachLog::SPY_LOCATOR )
				for( SpyLocators::const_iterator i = spyLocators( ridx ).begin(); i != spyLocators( ridx ).end(); ++i )
				{
					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );
					if(  dist < closest and (*i) != &actor )
					{
						found = true;
						closest = dist;
						*ppResult = *i;
					}
				}

			if( targetSystemType == MachLog::TARGET_NORMAL or
				ot == MachLog::RESOURCE_CARRIER )
				for( ResourceCarriers::const_iterator i = resourceCarriers( ridx ).begin(); i != resourceCarriers( ridx ).end(); ++i )
				{
					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );
					if(  dist < closest and (*i) != &actor )
					{
						found = true;
						closest = dist;
						*ppResult = *i;
					}
				}

			if( targetSystemType == MachLog::TARGET_NORMAL or
				ot == MachLog::APC )
				for( APCs::const_iterator i = apcs( ridx ).begin(); i != apcs( ridx ).end(); ++i )
				{
					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );
					if(  dist < closest and (*i) != &actor )
					{
						found = true;
						closest = dist;
						*ppResult = *i;
					}
				}

			if( targetSystemType == MachLog::TARGET_NORMAL or
				ot == MachLog::AGGRESSOR )
				for( Aggressors::const_iterator i = aggressors( ridx ).begin(); i != aggressors( ridx ).end(); ++i )
				{
					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );
					if(  dist < closest and (*i) != &actor )
					{
						found = true;
						closest = dist;
						*ppResult = *i;
					}
				}

			if( targetSystemType == MachLog::TARGET_NORMAL or
				ot == MachLog::MISSILE_EMPLACEMENT )
				for( MissileEmplacements::const_iterator i = missileEmplacements( ridx ).begin(); i != missileEmplacements( ridx ).end(); ++i )
				{
						MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );
						if(  dist < closest and (*i) != &actor and (*i)->isComplete() )
						{
							found = true;
							closest = dist;
							*ppResult = *i;
						}
				}
		}
	}

	return found;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

int MachLogRaces::findSafestFriendly( const MachActor& actor,
										MachActor** ppResult,
										SafeActorType safeActorType )
{
	int bestMilitaryStrength = 0;
	MexPoint3d pos = actor.position();

	for( MachPhys::Race ridx = MachPhys::RED; ridx != MachPhys::N_RACES; ++((int&)ridx) )
	{
		MachLogRaces::DispositionToRace disp = dispositionToRace( actor.race(), ridx );

		if( disp == MachLogRaces::ALLY or disp == MachLogRaces::OUR_RACE )
		{

			switch( safeActorType )
			{
				case AGGRESSIVE_MACHINES_AND_MISSILE_EMPLACEMENTS:
				case AGGRESSIVE_MACHINES:
				{
					// first, check aggressors
					for( Aggressors::const_iterator i = aggressors( ridx ).begin(); i != aggressors( ridx ).end(); ++i )
					{
						int candidateSafetyValue = (*i)->localStrengthFromPosition( pos );

						// note that one cannot select an already-evading machine as a "safest machine". As well as the
						// AI implications (it'll not be concerned about protecting you), this avoids the potentially
						// nasty situation of 2 evading machines running towards each other, and thus both observing each
						// other - nasty order-dependency problems for the persistence mechanism
						if( candidateSafetyValue > bestMilitaryStrength and (*i) != &actor and not( (*i)->evading() ) )
						{
							bestMilitaryStrength = candidateSafetyValue;
							*ppResult = *i;
						}
					}

					// then check administrators
					for( Administrators::const_iterator i = administrators( ridx ).begin(); i != administrators( ridx ).end(); ++i )
					{
						int candidateSafetyValue = (*i)->localStrengthFromPosition( pos );

						// note that one cannot select an already-evading machine as a "safest machine". As well as the
						// AI implications (it'll not be concerned about protecting you), this avoids the potentially
						// nasty situation of 2 evading machines running towards each other, and thus both observing each
						// other - nasty order-dependency problems for the persistence mechanism
						if( candidateSafetyValue > bestMilitaryStrength and (*i) != &actor and not( (*i)->evading() ) )
						{
							bestMilitaryStrength = candidateSafetyValue;
							*ppResult = *i;
						}
					}

					if( safeActorType != AGGRESSIVE_MACHINES_AND_MISSILE_EMPLACEMENTS )
						break;
				}
				case MISSILE_EMPLACEMENTS:
				{
					for( MissileEmplacements::const_iterator i = missileEmplacements( ridx ).begin(); i != missileEmplacements( ridx ).end(); ++i )
					{
						int candidateSafetyValue = (*i)->localStrengthFromPosition( pos );

						if(  candidateSafetyValue > bestMilitaryStrength and (*i) != &actor )
						{
							bestMilitaryStrength = candidateSafetyValue;
							*ppResult = *i;
						}
					}
 					break;
				}
				case GARRISONS:
				{
					for( Garrisons::const_iterator i = garrisons( ridx ).begin(); i != garrisons( ridx ).end(); ++i )
					{
						int candidateSafetyValue = (*i)->localStrengthFromPosition( pos );

						if(  candidateSafetyValue > bestMilitaryStrength and (*i) != &actor )
						{
							bestMilitaryStrength = candidateSafetyValue;
							*ppResult = *i;
						}
					}
					break;
				}
				case PODS:
				{
					for( Pods::const_iterator i = pods( ridx ).begin(); i != pods( ridx ).end(); ++i )
					{
						int candidateSafetyValue = (*i)->localStrengthFromPosition( pos );

						if(  candidateSafetyValue > bestMilitaryStrength and (*i) != &actor )
						{
							bestMilitaryStrength = candidateSafetyValue;
							*ppResult = *i;
						}
					}
					break;
				}
				default:
				ASSERT_INFO( safeActorType );
				ASSERT_BAD_CASE;
			}
		}
	}

	return bestMilitaryStrength;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////


void MachLogRaces::allFriendlyAggressiveMachinesWithinRange( const MachActor* pActor, MATHEX_SCALAR range, ctl_pvector< MachActor>* pActors, MATHEX_SCALAR minRange )
{
	MATHEX_SCALAR sqrMaxRange = range * range;
	MATHEX_SCALAR sqrMinRange = minRange * minRange;

	// note that the requirement that distance > sqrMinRange rules out possibility of adding yourself to the vector

	for( MachPhys::Race ridx = MachPhys::RED; ridx != MachPhys::N_RACES; ++((int&)ridx) )
	{
		MachLogRaces::DispositionToRace disp = dispositionToRace( pActor->race(), ridx );

		if( disp == MachLogRaces::ALLY or disp == MachLogRaces::OUR_RACE )
		{
			// first, check aggressors
			for( Aggressors::iterator i = aggressors( ridx ).begin(); i != aggressors( ridx ).end(); ++i )
			{
				MATHEX_SCALAR distance = pActor->position().sqrEuclidianDistance( (*i)->position() );
				if( not (*i)->insideAPC() and distance > sqrMinRange and distance <= sqrMaxRange )
					pActors->push_back( (*i) );
			}
			// then check administrators
			for( Administrators::iterator i = administrators( ridx ).begin(); i != administrators( ridx ).end(); ++i )
			{
				MATHEX_SCALAR distance = pActor->position().sqrEuclidianDistance( (*i)->position() );
				if( not (*i)->insideAPC() and distance > sqrMinRange and distance <= sqrMaxRange )
					pActors->push_back( (*i) );
			}
		}
	}
}


void MachLogRaces::allFriendlyAggressivesWithinRange( const MachActor* pActor, MATHEX_SCALAR range, ctl_pvector< MachActor>* pActors, MATHEX_SCALAR minRange )
{
	// populate the machines part of the pVector.
	allFriendlyAggressiveMachinesWithinRange( pActor, range, pActors, minRange );

	// now add missile emplacements

	MATHEX_SCALAR sqrMaxRange = range * range;
	MATHEX_SCALAR sqrMinRange = minRange * minRange;

	// note that the requirement that distance > sqrMinRange rules out possibility of adding yourself to the vector

	for( MachPhys::Race ridx = MachPhys::RED; ridx != MachPhys::N_RACES; ++((int&)ridx) )
	{
		MachLogRaces::DispositionToRace disp = dispositionToRace( pActor->race(), ridx );

		if( disp == MachLogRaces::ALLY or disp == MachLogRaces::OUR_RACE )
		{
	  		for( MissileEmplacements::iterator i = missileEmplacements( ridx ).begin(); i != missileEmplacements( ridx ).end(); ++i )
			{
				MATHEX_SCALAR distance = pActor->position().sqrEuclidianDistance( (*i)->position() );
				if( distance > sqrMinRange and distance <= sqrMaxRange and (*i)->isComplete() )
					pActors->push_back( (*i) );
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

bool MachLogRaces::noAlternativeAlreadyBuilt( MachPhys::Race race, const MachLogProductionUnit& prod )
{
	// instant success if the production unit has a constructionId of -1
	if( prod.constructionId() == -1 )
		return true;


	// otherwise.......

	bool noBuiltAlternativeFound = true;

	switch( prod.type() )
	{
		case MachLog::GARRISON:
			for( Garrisons::iterator i = garrisons( race ).begin();
				 noBuiltAlternativeFound and i != garrisons( race ).end();
				 ++ i )
			{
				if( (*i)->constructionId() == prod.constructionId() )
					noBuiltAlternativeFound = false;
			}
			break;
		case MachLog::HARDWARE_LAB:
			for( HardwareLabs::iterator i = hardwareLabs( race ).begin();
				 noBuiltAlternativeFound and i != hardwareLabs( race ).end();
				 ++ i )
			{
				if( (*i)->constructionId() == prod.constructionId() )
					noBuiltAlternativeFound = false;
			}
			break;
		case MachLog::FACTORY:
			for( Factories::iterator i = factories( race ).begin();
				 noBuiltAlternativeFound and i != factories( race ).end();
				 ++ i )
			{
				if( (*i)->constructionId() == prod.constructionId() )
					noBuiltAlternativeFound = false;
			}
			break;
		case MachLog::SMELTER:
			for( Smelters::iterator i = smelters( race ).begin();
				 noBuiltAlternativeFound and i != smelters( race ).end();
				 ++ i )
			{
				if( (*i)->constructionId() == prod.constructionId() )
					noBuiltAlternativeFound = false;
			}
			break;
		case MachLog::MINE:
			for( Mines::iterator i = mines( race ).begin();
				 noBuiltAlternativeFound and i != mines( race ).end();
				 ++ i )
			{
				if( (*i)->constructionId() == prod.constructionId() )
					noBuiltAlternativeFound = false;
			}
			break;
		case MachLog::MISSILE_EMPLACEMENT:
			for( MissileEmplacements::iterator i = missileEmplacements( race ).begin();
				 noBuiltAlternativeFound and i != missileEmplacements( race ).end();
				 ++ i )
			{
				if( (*i)->constructionId() == prod.constructionId() )
					noBuiltAlternativeFound = false;
			}
			break;
		case MachLog::POD:
			for( Pods::iterator i = pods( race ).begin();
				 noBuiltAlternativeFound and i != pods( race ).end();
				 ++ i )
			{
				if( (*i)->constructionId() == prod.constructionId() )
					noBuiltAlternativeFound = false;
			}
			break;
		case MachLog::BEACON:
			for( Beacons::iterator i = beacons( race ).begin();
				 noBuiltAlternativeFound and i != beacons( race ).end();
				 ++ i )
			{
				if( (*i)->constructionId() == prod.constructionId() )
					noBuiltAlternativeFound = false;
			}
			break;
	}

	return noBuiltAlternativeFound;
}


bool MachLogRaces::findFriendlyGarrisonClosestTo( const MachLogMachine& machine, MachLogGarrison** ppGarrison, MachPhysStation** ppStation, MachLogRaces::FindGarrisonCriterion criterion )
 {

 	MATHEX_SCALAR sqrDistance = 0;
 	MATHEX_SCALAR range = 1000000000;

 	*ppGarrison = NULL;
 	*ppStation = NULL;

 	bool found = false;
 	bool insideGarrison = false;
 	const MachLogGarrison* pGarrisonImInside = NULL;	// only used if we ARE inside a garrison

 	// if we're already inside a garrison, we don't want to include that in the list.
 	if( machine.insideBuilding() and machine.insideWhichBuilding().objectType() == MachLog::GARRISON )
 	{
 		insideGarrison = true;
 		pGarrisonImInside = &( machine.insideWhichBuilding().asGarrison() );
 	}

 	for( MachPhys::Race ridx = MachPhys::RED; ridx != MachPhys::N_RACES; ++((int&)ridx) )
 	{
 		MachLogRaces::DispositionToRace disp = dispositionToRace( machine.race(), ridx );
 		if( disp == MachLogRaces::ALLY or
 			disp == MachLogRaces::OUR_RACE )
 		{

 			Garrisons& friendlyGarrisons = garrisons( ridx );

			for( MachLogRaces::Garrisons::iterator i = friendlyGarrisons.begin(); i != friendlyGarrisons.end(); ++i )
			{
                MachLogGarrison* pCandidateGarrison = (*i);

				// don't check this garrison if it's the one we're inside.
				// also, don't consider any garrisons I'm too fat to fit inside.
				if( not ( insideGarrison and pGarrisonImInside == pCandidateGarrison )
					and
					machine.canEnterConstruction(*pCandidateGarrison) )
				{
                    MachPhysStation* pCandidateStation = NULL;
					sqrDistance = machine.position().sqrEuclidianDistance( pCandidateGarrison->entranceExternalPoint( 0 ) );

                    switch( criterion )
					{
						case FREE_BAYS_ONLY:
 						{
 							MachPhysConstructionData& conData = _CONST_CAST( MachPhysConstructionData&, pCandidateGarrison->constructionData() );
 							//const MachPhysConstructionData& conData = (*i)->constructionData();
 							if( sqrDistance < range and conData.stations().freeStation( MachPhysStation::PARKING_BAY, &pCandidateStation ) )
 							{
 								found = true;
 								range = sqrDistance;
 								*ppGarrison = pCandidateGarrison;
                                *ppStation = pCandidateStation;
 							}
 						}
                        break;

 						case FREE_BAYS_PLUS_NO_ENEMY_CANATTACKS:
 						{
 							MachPhysConstructionData& conData = _CONST_CAST( MachPhysConstructionData&, pCandidateGarrison->constructionData() );
 							//const MachPhysConstructionData& conData = (*i)->constructionData();
 							if( sqrDistance < range
 								and	conData.stations().freeStation( MachPhysStation::PARKING_BAY, &pCandidateStation )
 								and not pCandidateGarrison->isEnemyCanAttackInside( machine.race() ) )
 							{
 								found = true;
 								range = sqrDistance;
 								*ppGarrison = pCandidateGarrison;
                                *ppStation = pCandidateStation;
 							}
 						}
                        break;

 						default:
 						{
 							if( sqrDistance < range )
 							{
 								found = true;
 								range = sqrDistance;
 								*ppGarrison = pCandidateGarrison;
 							}
 						}
 					}
 				}
 				// end if( not ( insideGarrison and pGarrisonImInside = (*i) )
 			}
 		}
 	}

 	POST( implies( found, *ppGarrison != NULL ) );
 	POST( implies( found and criterion != FREE_BAYS_UNIMPORTANT, *ppStation != NULL ) );

 	return found;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////


void MachLogRaces::outputRace( ostream& o, MachPhys::Race t )
{
	o << "Race " << t << std::endl;

	o << " nBMU " << MachLogRaces::instance().nBuildingMaterialUnits( t );
	o << " nMaxBMU " << MachLogRaces::instance().nMaxBuildingMaterialUnits( t ) << std::endl;

	o << " nORE " << MachLogRaces::instance().nOre( t );
	o << " nMaxORE " << MachLogRaces::instance().nMaxOre( t ) << std::endl;

	o << " nMachines " << MachLogRaces::instance().nMachines( t ) << std::endl;
	o << " Adm: " << MachLogRaces::instance().administrators( t ).size();
	o << " Agg: " << MachLogRaces::instance().aggressors( t ).size();
	o << " APC: " << MachLogRaces::instance().apcs( t ).size();
	o << " Con: " << MachLogRaces::instance().constructors( t ).size();
	o << " Geo: " << MachLogRaces::instance().geoLocators( t ).size();
	o << " Res: " << MachLogRaces::instance().resourceCarriers( t ).size();
	o << " Spy: " << MachLogRaces::instance().spyLocators( t ).size();
	o << " Tek: " << MachLogRaces::instance().technicians( t ).size() << std::endl;
	o << " nConstructions " << MachLogRaces::instance().nConstructions( t ) << std::endl;
	o << " Mines: " << MachLogRaces::instance().mines( t ).size() << std::endl;
	o << " Factories: " << MachLogRaces::instance().factories( t ).size() << std::endl;
	o << " Garrisons: " << MachLogRaces::instance().garrisons( t ).size() << std::endl;
	o << " Controller: " << std::endl;
//	MachLogRaces::instance().pDataImpl_->controller_[ t ]->doOutputOperator( o );
}
/* //////////////////////////////////////////////////////////////// */

void MachLogRaces::add( MachActor* pActor )
{
    //Add the entry for this actor to the id map
    pDataImpl_->pActorIdMap_->add( pActor->id(), pActor );
	if( pActor->objectIsArtefact() or pActor->objectIsDebris() or pActor->objectIsOreHolograph() )
	{
	    //Add to the actor collection
	    objects().push_back( pActor );

        //Add to the race-specific collection if a race actor
        MachPhys::Race race = pActor->race();
        if( race != MachPhys::NORACE )
    		raceObjects( race ).push_back( pActor );
	}
	else
	{
	    //Add to the actor collection
	    objects().push_back( pActor );
		raceObjects( pActor->race() ).push_front( pActor );
	}
}

void MachLogRaces::addToAllCollections( MachActor* pActor )
{
	add( pActor );

	/*
	if( pActor->objectIsMachine() )
		ctl_append( &MachLogRaces::instance().machines(), &pActor->asMachine() );
	*/

	switch( pActor->objectType() )
	{
		case MachLog::ADMINISTRATOR: administrators( pActor->race() ).push_back( &pActor->asAdministrator() );break;
		case MachLog::AGGRESSOR: aggressors( pActor->race() ).push_back( &pActor->asAggressor() );break;
		case MachLog::APC: APCs( pActor->race() ).push_back( &pActor->asAPC() );break;
		case MachLog::CONSTRUCTOR:
				{
					MachLogConstructor& constructor = pActor->asConstructor();
					constructors( pActor->race() ).push_back( &constructor );
					constructionTree().updateAvailableConstructions( constructor.race(),
																					  constructor.data().mostAdvancedConstructionType(),
																					  constructor.data().mostAdvancedSubType(),
																					  constructor.data().mostAdvancedConstructionLevel() );
				}
		break;
		case MachLog::GEO_LOCATOR: geoLocators( pActor->race() ).push_back( &pActor->asGeoLocator() );break;
		case MachLog::RESOURCE_CARRIER: resourceCarriers( pActor->race() ).push_back( &pActor->asResourceCarrier() );break;
		case MachLog::TECHNICIAN:
			technicians( pActor->race() ).push_back( &pActor->asTechnician() );
			cascadeUpdateForResearch( pActor->race() );
			break;
		case MachLog::SPY_LOCATOR: spyLocators( pActor->race() ).push_back( &pActor->asSpyLocator() );break;
		case MachLog::BEACON: beacons( pActor->race() ).push_back( &pActor->asBeacon() );break;
		case MachLog::FACTORY: factories( pActor->race() ).push_back( &pActor->asFactory() );break;
		case MachLog::GARRISON: garrisons( pActor->race() ).push_back( &pActor->asGarrison() );break;
		case MachLog::HARDWARE_LAB: hardwareLabs( pActor->race() ).push_back( &pActor->asHardwareLab() );break;
		case MachLog::MINE: mines( pActor->race() ).push_back( &pActor->asMine() );break;
		case MachLog::MISSILE_EMPLACEMENT: missileEmplacements( pActor->race() ).push_back( &pActor->asMissileEmplacement() );break;
		case MachLog::SMELTER: smelters( pActor->race() ).push_back( &pActor->asSmelter() );break;
		case MachLog::POD: pods( pActor->race() ).push_back( &pActor->asPod() );break;
		case MachLog::ARTEFACT: artefactsCollection().push_back( &pActor->asArtefact() );break;
	}
}

bool MachLogRaces::actorExists( UtlId id ) const
{
    return pDataImpl_->pActorIdMap_->contains( id );
}

MachActor& MachLogRaces::actor( UtlId id )
{
    PRE( actorExists( id ) );
    return *(pDataImpl_->pActorIdMap_->operator[]( id ));
}

/* End RACES.CPP ****************************************************/


//void MachLogRaces::constructRedRace()
//{
//	MachLogRace * pRace = _NEW( MachLogRace( MachPhys::RED ) );
//	race( MachPhys::RED, pRace );
//
////	MachLogMine * pPreloadHack = _NEW( MachLogMine( pRace, 4, MexPoint3d( 525, 525, -1000 ), MexDegrees( 90 ) ) );
////	ctl_append( &mines( MachPhys::RED ), pPreloadHack );
////	_DELETE( pPreloadHack );
//
//    MexTransform3d localTransform;
//    W4dEntity* pAnyDomain =
//        MachLogPlanetDomains::pDomainPosition( MexPoint3d(0,0,0), 0, &localTransform );
//    W4dEntity* pPhysObject = _NEW( W4dGeneric( pAnyDomain, localTransform ) );
//
//	MachLogController * pCtl = _NEW( MachLogController( MachPhys::RED, pPhysObject, pRace ) );
//	controller( MachPhys::RED, pCtl );
//
//	_NEW( MachLogAdministrator( 3, pRace, MexPoint3d( 920, 160, 0 ) ) );
//	_NEW( MachLogLocator( 7, pRace, MexPoint3d( 910,160, 0 ) ) );
//	_NEW( MachLogAggressor( 8, pRace, MexPoint3d( 900,160, 0 ) ) );
//	_NEW( MachLogConstructor( 1, pRace, MexPoint3d( 472,175, 0 ) ) );
//	_NEW( MachLogTechnician( 3, pRace, MexPoint3d( 501, 248, 0 ) ) );
//	_NEW( MachLogResourceCarrier( 3, pRace, MexPoint3d( 846, 204, 0 ) ) );
//
//	ctl_append( &smelters( MachPhys::RED ), _NEW( MachLogSmelter( pRace, 9, MexPoint3d(686, 158, 0), MexDegrees( 90 ) ) ) );
//	ctl_append( &garrisons( MachPhys::RED ), _NEW( MachLogGarrison( pRace, 8, MexPoint3d(875, 291, 0), MexDegrees( -90 ) ) ) );
//	ctl_append( &garrisons( MachPhys::RED ), _NEW( MachLogGarrison( pRace, 8, MexPoint3d(525, 350, 0), MexDegrees( 180 ) ) ) );
//	ctl_append( &pods( MachPhys::RED ), _NEW( MachLogPod( pRace, 1, MexPoint3d(788, 175, 0), MexDegrees( 90 ) ) ) );
//
////	ctl_append( &factories( MachPhys::RED ), _NEW( MachLogFactory( pRace, 10, MexPoint3d( 598, 161, 0), MexDegrees( 90 ) ) ) );
//	ctl_append( &factories( MachPhys::RED ), _NEW( MachLogFactory( pRace, 10, MexPoint3d( 616, 158, 0), MexDegrees( 90 ) ) ) );
//	ctl_append( &hardwareLabs( MachPhys::RED ), _NEW( MachLogHardwareLab( pRace, 4, MexPoint3d(482, 256, 0), MexDegrees( 0 ) ) ) );
//	ctl_append( &softwareLabs( MachPhys::RED ), _NEW( MachLogSoftwareLab( pRace, 1, MexPoint3d( 569, 276, 0), MexDegrees( 180 ) ) ) );
////	ctl_append( &softwareLabs( MachPhys::RED ), _NEW( MachLogSoftwareLab( pRace, 1, MexPoint3d( 520, 300, 0), MexDegrees( 180 ) ) ) );
//
//    _NEW( MachLogMissileEmplacement( pRace, 7, MexPoint3d(452, 365, 0), MexDegrees( 180 ) ) );
//
//	_NEW( MachLogBeacon( pRace, 3, MexPoint3d(218, 305, 0), MexDegrees( 0 ) ) );
//	_NEW( MachLogBeacon( pRace, 3, MexPoint3d(394, 394, 0), MexDegrees( 0 ) ) );
//    _NEW( MachLogBeacon( pRace, 3, MexPoint3d(539, 219, 0), MexDegrees( 0 ) ) );
//    _NEW( MachLogBeacon( pRace, 3, MexPoint3d(860, 220, 0), MexDegrees( 0 ) ) );
//    _NEW( MachLogBeacon( pRace, 3, MexPoint3d(778, 458, 0), MexDegrees( 0 ) ) );
//    _NEW( MachLogBeacon( pRace, 3, MexPoint3d(798, 661, 0), MexDegrees( 0 ) ) );
//}
//
///* //////////////////////////////////////////////////////////////// */
//
//void MachLogRaces::constructBlueRace()
//{
//	MachLogRace * pRace = _NEW( MachLogRace( MachPhys::BLUE ) );
//	race( MachPhys::BLUE, pRace );
//
//    MexTransform3d localTransform;
//    W4dEntity* pAnyDomain =
//        MachLogPlanetDomains::pDomainPosition( MexPoint3d(0,0,0), 0, &localTransform );
//    W4dEntity* pPhysObject = _NEW( W4dGeneric( pAnyDomain, localTransform ) );
//
//	MachLogController * pCtl = _NEW( MachLogController( MachPhys::BLUE, pPhysObject, pRace ) );
//	controller( MachPhys::BLUE, pCtl );
//
//	_NEW( MachLogAdministrator( 3, pRace, MexPoint3d( 920, 780, 0 ) ) );
//	_NEW( MachLogLocator( 7, pRace, MexPoint3d( 920,770, 0 ) ) );
//	_NEW( MachLogAggressor( 8, pRace, MexPoint3d( 920,760, 0 ) ) );
//
//	ctl_append( &smelters( MachPhys::BLUE ), _NEW( MachLogSmelter( pRace, 9, MexPoint3d( 830, 890, 0), MexDegrees( -90 ) ) ) );
//	ctl_append( &garrisons( MachPhys::BLUE ), _NEW( MachLogGarrison( pRace, 8, MexPoint3d( 175, 700, 0), MexDegrees( 90 ) ) ) );
//	ctl_append( &garrisons( MachPhys::BLUE ), _NEW( MachLogGarrison( pRace, 8, MexPoint3d( 739, 885, 0), MexDegrees( -90 ) ) ) );
//	ctl_append( &pods( MachPhys::BLUE ), _NEW( MachLogPod( pRace, 1, MexPoint3d(175, 875, 0 ), MexDegrees( 0 ) ) ) );
//	ctl_append( &factories( MachPhys::BLUE ), _NEW( MachLogFactory( pRace, 10, MexPoint3d( 900,890, 0), MexDegrees( -90 ) ) ) );
//
//	ctl_append( &hardwareLabs( MachPhys::BLUE ), _NEW( MachLogHardwareLab( pRace, 4, MexPoint3d(394,919, 0), MexDegrees( 180 ) ) ) );
//	ctl_append( &softwareLabs( MachPhys::BLUE ), _NEW( MachLogSoftwareLab( pRace, 1, MexPoint3d(306,852, 0), MexDegrees( 90 ) ) ) );
//
//    _NEW( MachLogMissileEmplacement( pRace, 7, MexPoint3d(248,628, 0), MexDegrees( -90 ) ) );
//
////Note this new bit down here!!!
//	_NEW( MachLogTechnician( 3, pRace, MexPoint3d( 321, 919, 0 ) ) );
//
//    _NEW( MachLogBeacon( pRace, 3, MexPoint3d(336,452, 0), MexDegrees( 0 ) ) );
//    _NEW( MachLogBeacon( pRace, 3, MexPoint3d(190,657, 0), MexDegrees( 0 ) ) );
//    _NEW( MachLogBeacon( pRace, 3, MexPoint3d(832,657, 0), MexDegrees( 0 ) ) );
//    _NEW( MachLogBeacon( pRace, 3, MexPoint3d(452,744, 0), MexDegrees( 0 ) ) );
//    _NEW( MachLogBeacon( pRace, 3, MexPoint3d(646,832, 0), MexDegrees( 0 ) ) );
//    _NEW( MachLogBeacon( pRace, 3, MexPoint3d(321,904, 0), MexDegrees( 0 ) ) );
//}
//
///* //////////////////////////////////////////////////////////////// */

bool MachLogRaces::hasPCRace() const
{
    return pDataImpl_->thePCController_ != NULL;
}

MachLogPCController& MachLogRaces::pcController() const
{
    PRE( hasPCRace() );

    return *pDataImpl_->thePCController_;
}

void MachLogRaces::pcController( MachLogPCController * pNewPCController )
{
    PRE( not hasPCRace() );

    //Store the special pointer
    pDataImpl_->thePCController_ = pNewPCController;

    //Register as an abstract controller
    controller( pNewPCController->race(), pNewPCController );
}

MachLogResearchTree& MachLogRaces::researchTree()
{
	return *pDataImpl_->pResearchTree_;
}

const MachLogResearchTree& MachLogRaces::researchTree() const
{
	return *pDataImpl_->pResearchTree_;
}

MachLogConstructionTree& MachLogRaces::constructionTree()
{
	return *pDataImpl_->pConstructionTree_;
}

const MachLogConstructionTree& MachLogRaces::constructionTree() const
{
	return *pDataImpl_->pConstructionTree_;
}

void MachLogRaces::cascadeUpdateForResearch( MachPhys::Race race )
{
	//HAL_STREAM("MLRaces::cascadeUpdateForResearch\n" );
	for( HardwareLabs::iterator i = pDataImpl_->hardwareLabs_[ race ].begin(); i != pDataImpl_->hardwareLabs_[ race ].end(); ++i )
		(*i)->initialiseResearchItems();

	pDataImpl_->pResearchTree_->updated( race );
}

const MachLogRaces::AssemblyPoints& MachLogRaces::aggressorAssemblyPoints( const MachPhys::Race race )
{
	return pDataImpl_->aggressorAssemblyPoints_[ race ];
}

const MachLogRaces::AssemblyPoints& MachLogRaces::administratorAssemblyPoints( const MachPhys::Race race )
{
	return pDataImpl_->administratorAssemblyPoints_[ race ];
}

void MachLogRaces::addAggressorAssemblyPoint( const MachPhys::Race race, const MexPoint2d& point )
{
	pDataImpl_->aggressorAssemblyPoints_[ race ].push_back( point );
}

void MachLogRaces::addAdministratorAssemblyPoint( const MachPhys::Race race, const MexPoint2d& point )
{
	pDataImpl_->administratorAssemblyPoints_[ race ].push_back( point );
}

void MachLogRaces::defCon( MachPhys::Race race, MachLog::DefCon newDefCon )
{
	pDataImpl_->defaultDefCon_[ race ] = newDefCon;
	for_each( pDataImpl_->raceObjects_[ race ].begin(), pDataImpl_->raceObjects_[race].end(), checkTypeAndSetDefCon );
}

MachLog::DefCon MachLogRaces::defCon( MachPhys::Race race ) const
{
	return pDataImpl_->defaultDefCon_[race];
}

MachLogStats&			MachLogRaces::stats()
{
	return *pDataImpl_->pStats_;
}

const MachLogStats&		MachLogRaces::stats() const
{
	return *pDataImpl_->pStats_;
}

bool MachLogRaces::loadGame( W4dSceneManager* pSceneManager, const string& planetName, const string& scenarioName, BaseProgressReporter* pReporter )
{
	MachLogGameCreationData gameData;

	MachLogVoiceMailManager::instance().acceptMailPostings( false );

	loadGame( pSceneManager, planetName, scenarioName, gameData, pReporter );

	MachLogVoiceMailManager::instance().acceptMailPostings( true );

    return true;
}

bool MachLogRaces::loadGame(
	W4dSceneManager* pSceneManager, const string& planetName, const string& scenarioName, const MachLogGameCreationData& gameData, BaseProgressReporter* pReporter
)
{

	MachLogVoiceMailManager::instance().acceptMailPostings( false );

	//Cache GameCreationData
	if( pDataImpl_->pGameCreationData_ )
		_DELETE( pDataImpl_->pGameCreationData_ );
	pDataImpl_->pGameCreationData_ = _NEW( MachLogGameCreationData( gameData ) );
    //Ensure previous game cleared out
    unloadGame();

	pReporter->report( 20, 100 );

    //Set up the dependent objects used in the game
    initialise();
    databaseHandler().initialiseTaskFlags();

	//go read in the planet bits
	loadPlanet( pSceneManager, planetName, pReporter );

	pReporter->report( 50, 100 );

    //If a scenario is defined, load it
    if( scenarioName.length() != 0 )
    	MachLogScenario::load( scenarioName, gameData );

	pReporter->report( 80, 100 );

	MachLogFactory::loadGame();

	pReporter->report( 100, 100 );

	MachLogVoiceMailManager::instance().acceptMailPostings( true );

	MachLogMachine::resetNextGlobalHitVoiceMailTime();
	MachLogConstruction::resetNextGlobalHitVoiceMailTime();

    //Ensure the artifact map gets cleared out. Clients in a network game rely on this.
    artefacts().finish();

    return true;
}

bool MachLogRaces::loadPlanet( W4dSceneManager* pSceneManager, const string& planetName, BaseProgressReporter* pReporter )
{
    //Reset the simulation time before we start, since current time is used during setup
    SimManager::instance().resetTime();
    W4dManager::instance().time( PhysAbsoluteTime( 0.0 ) );

	// Setup default camera location/type for each race. This can be overridden by
	// specifying a camera position in the scenario file.
	for( MachPhys::Race ridx = MachPhys::RED; ridx != MachPhys::N_RACES; ++((int&)ridx) )
	{
		pDataImpl_->cameraInfo_[ ridx ].position_ = MexTransform3d( MexEulerAngles( MexDegrees( 111 ), 0.0, 0.0 ), MexPoint3d( 150.0, 150.0, 1.5 ) );
		pDataImpl_->cameraInfo_[ ridx ].type_ = MachLogCamera::GROUND;
	}

    //Construct the pathname for the planet surface file
   	string psfPath = "models/planet/";
    psfPath += planetName;
    psfPath += "/";
    psfPath += planetName;
	psfPath += ".psf";

    //Load the planet surface
    MachLogPlanet::instance().surface( pSceneManager, psfPath, pReporter );

	pDataImpl_->currentPlanet_ = planetName;

    return true;
}

void MachLogRaces::unloadGame()
{
    //Set flag so clients can guard against erroneous work
    pDataImpl_->isUnloadingGame_ = true;

	//Clear down all actor operations here before destruction of the actor lists.
	//Otherwise actor destruction can lead to order dependancy problems.
	Objects& objects = pDataImpl_->objects_;
	for( Objects::iterator i = objects.begin(); i != objects.end(); ++i )
		(*i)->strategy().beInterrupted();

	//Clear out all squadrons to prevent problems in machgui
    for( size_t i = 0; i != MachPhys::N_RACES; ++i )
    {
        Squadrons& squads = pDataImpl_->squadrons_[i];
        for( Squadrons::iterator it = squads.begin(); it != squads.end(); ++it )
            (*it)->releaseAllMachines();
    }

    //Destruct all processes. This will result in the destruction of all the actors and races.
    SimManager::instance().deleteAllProcesses();

    //Clear any pending simulation discrete events
    SimManager::instance().clearDiary();

    //Delete the planet surface model
    MachLogPlanet::instance().clear();

    //Clear out the artefacts loaded for teh scenario
    pDataImpl_->pArtefacts_->unload();

	//Clear the voicemail queue
	MachLogVoiceMailManager::instance().clearMailQueue();

    //Delete the dependent objects
    clear();
    databaseHandler().clearTaskFlags();

	MachLogFactory::unloadGame();

    //Reset flag
    pDataImpl_->isUnloadingGame_ = false;
}

bool MachLogRaces::loadSavedGame
(
    W4dSceneManager* pSceneManager, const string& planetName, const string& scenarioFileName,
    const SysPathName& pathname, MachLogLoadSaveGameExtras* pExtras, BaseProgressReporter* pReporter
)
{
	MachLogVoiceMailManager::instance().acceptMailPostings( false );

	//clear away all old processes etc.
	unloadGame();

	pReporter->report( 10, 100 );

    //Set up the dependent objects used in the game
    initialise();
    databaseHandler().initialiseTaskFlags();

	pReporter->report( 12, 100 );

    //Load up the planet
    loadPlanet( pSceneManager, planetName, pReporter );

	pReporter->report( 50, 100 );

	{
        //Load the artefact data from the scenario file
        //Construct the scenario pathname and create a parser to parse the file
        string scenarioPath = "data\\" + scenarioFileName;
        const SysPathName scenarioPathName( scenarioPath );

        SysMetaFile metaFile( "mach1.met" );

        std::auto_ptr< std::istream > pIstream;

        if( SysMetaFile::useMetaFile() && metaFile.hasFile( scenarioPath ) )
        {
            //pIstream = _NEW( SysMetaFileIstream( metaFile, scenarioPathName, ios::text ) );
            pIstream = std::auto_ptr< std::istream > (
                _NEW( SysMetaFileIstream( metaFile, scenarioPathName, std::ios::in ) ));
        }
        else
        {
            ASSERT_FILE_EXISTS( scenarioPathName.c_str() );
            //pIstream = _NEW( ifstream( scenarioPathName.c_str(), ios::text | ios::in ) );
            pIstream = std::auto_ptr< std::istream > (
                _NEW( std::ifstream( scenarioPathName.c_str(), std::ios::in ) ));
        }

		UtlLineTokeniser parser( *pIstream, scenarioPathName );

        //Fast forward to the artefacts section
		while( not parser.finished()  and  not ( parser.tokens()[0] == "ARTEFACTS" ) )
			parser.parseNextLine();

        if( not parser.finished() )
    		artefacts().parseArtefactsSection( &parser, MachLogArtefacts::DO_NOT_CREATE_ARTEFACT_INSTANCES );
	}

	pReporter->report( 60, 100 );

	//must set the scene manager here or all kinds of things go to pot as the planet cannot be loaded.
	MachLogPersistence::instance().sceneManager( pSceneManager );
	MachLogPersistence::instance().extras( pExtras );

   	std::ifstream	str( pathname.c_str(), std::ios::binary );
    PerIstream	istr( str );

	HAL_STREAM("MachLogRaces::loadSavedGame read MLPersistence\n" );
    istr >> MachLogPersistence::instance();

	pReporter->report( 70, 100 );

	doGlobalFixup();

	pReporter->report( 80, 100 );

	//the load game will force the numbers to be put back into the factories.
	MachLogFactory::loadGame();

	pReporter->report( 100, 100 );

	MachLogVoiceMailManager::instance().acceptMailPostings( true );

	return true;
}

bool MachLogRaces::saveGame( const SysPathName& pathname, MachLogLoadSaveGameExtras* pExtras )
{
	//set up the file to be a binary output stream and write out the persisted data
	//using the MLPersistence singleton.
	std::ofstream	str( pathname.c_str(), std::ios::binary );

	if ( not str.fail() )
	{
	    PerOstream	ostr( str );

		HAL_STREAM("MachLogRaces::saveGame pExtras " << (void*)pExtras << std::endl );
		MachLogPersistence::instance().extras( pExtras );
	    ostr << MachLogPersistence::instance();
	}

	// Check to see if the save screwed up.
	bool success = not str.fail();

	// Remove any half finished save causes by a mistake (probably lack of disk space) during the save game.
	if ( not success )
	{
		::remove( pathname.c_str() );
	}

	#ifndef PRODUCTION
	if( getenv("CB_GENERATE_SCENARIO_ENTRIES") )
	{
		std::ofstream o("temp.scn");
		for( MachPhys::Race r = MachPhys::RED; r < MachPhys::N_RACES; ++((int&)r) )
		{
			if( raceInGame( r ) )
			{
				o << "RACE " << r << std::endl;
				o << "CONSTRUCTIONS\n";
				for( Objects::iterator i = raceObjects( r ).begin(); i != raceObjects( r ).end(); ++i )
				{
					if( (*i)->objectIsConstruction() )
					{
						MachLogConstruction& c = (*i)->asConstruction();
						o << c.objectType() << " " << c.level() << " " << c.position().x() << " " << c.position().y() << " " << c.position().z() << " ";
						o << MexDegrees( c.globalTransform().rotationAsEulerAngles().azimuth() ) << " ";
						switch( c.objectType() )
						{
							case MachLog::FACTORY:
								o << (MachPhys::FactorySubType)c.subType() << " ";
								break;
							case MachLog::HARDWARE_LAB:
								o << (MachPhys::HardwareLabSubType)c.subType() << " ";
								break;
							case MachLog::MISSILE_EMPLACEMENT:
								o << (MachPhys::MissileEmplacementSubType)c.subType() << " ";
								break;
						}
						if( c.objectIsCanAttack() )
						{
							o << c.asCanAttack().weaponCombo() << " ";
						}
						o << std::endl;
					}
				}
				o << "MACHINES\n";
				for( Objects::iterator i = raceObjects( r ).begin(); i != raceObjects( r ).end(); ++i )
				{
					if( (*i)->objectIsMachine() )
					{
						MachLogMachine& m = (*i)->asMachine();
						o << m.objectType() << " " << m.hwLevel() << " " << m.swLevel() << " " << m.position().x() << " " << m.position().y() << " " << m.position().z() << " ";
						switch( m.objectType() )
						{
							case MachLog::AGGRESSOR:
								o << (MachPhys::AggressorSubType)m.subType() << " ";
								break;
							case MachLog::ADMINISTRATOR:
								o << (MachPhys::AdministratorSubType)m.subType() << " ";
								break;
							case MachLog::CONSTRUCTOR:
								o << (MachPhys::ConstructorSubType)m.subType() << " ";
								break;
							case MachLog::TECHNICIAN:
								o << (MachPhys::TechnicianSubType)m.subType() << " ";
								break;
						}
						if( m.objectIsCanAttack() )
						{
							o << m.asCanAttack().weaponCombo() << " ";
						}
						o << std::endl << std::endl;
					}
				}
			}
		}
	}
	#endif

	return success;
}

void MachLogRaces::initialise()
{
    PRE( pDataImpl_->pResearchTree_ == NULL );
    PRE( pDataImpl_->pConstructionTree_ == NULL );
    PRE( pDataImpl_->pStats_ == NULL );

    //Construct the dependent objects
	pDataImpl_->pResearchTree_ = _NEW( MachLogResearchTree() );
	pDataImpl_->pConstructionTree_ = _NEW( MachLogConstructionTree() );
	pDataImpl_->pStats_ = _NEW( MachLogStats );
	pDataImpl_->endingFlic_ = "";
	pDataImpl_->AICommandId_ = 100000;
	pDataImpl_->inSpecialActorUpdate_ = false;

    //Initialise the statistics
	for( MachPhys::Race i = MachPhys::RED; i < MachPhys::N_RACES; ++((int&)i) )
	{
		pDataImpl_->nMachines_[ i ] = 0;
		pDataImpl_->nConstructions_[ i ] = 0;
		pDataImpl_->nBuildingMaterialUnits_[ i ] = 0;
		pDataImpl_->nMaxBuildingMaterialUnits_[ i ] = 0;
		pDataImpl_->nOre_[ i ] = 0;
		pDataImpl_->nMaxOre_[ i ] = 0;
 		pDataImpl_->virtualBeacon_[ i ] = MachLog::NO_BEACON;
		pDataImpl_->techniciansTreacheried_[ i ] = 0;
		pDataImpl_->vortexHasBeenFired_[ i ] = false;
		pDataImpl_->hasWon_[ i ] = false;
		pDataImpl_->hasLost_[ i ] = false;
		pDataImpl_->podCaptured_[ i ] = false;
		pDataImpl_->builtIonCannon_[ i ] = false;
		pDataImpl_->builtNuclearWeapon_[ i ] = false;
		pDataImpl_->raceInGame_[ i ] = false;
		if( pDataImpl_->scores_[ i ] )
		{
			_DELETE( pDataImpl_->scores_[i] );
			pDataImpl_->scores_[i] = NULL;
		}
		pDataImpl_->scores_[i] = _NEW( MachLogScore( i ) );
	}

	for( int i = MachPhys::RED; i < MachPhys::N_RACES; ++i )
		for( int j = MachPhys::RED; j < MachPhys::N_RACES; ++j )
		{
			pDataImpl_->firedAt_[i][j] = false;
			if( i == j )
				pDataImpl_->disposition_[i][j] = OUR_RACE;
			else
				pDataImpl_->disposition_[i][j] = NEUTRAL;
		}

	for( int i = MachPhys::RED; i < MachPhys::N_RACES; ++i )
		pDataImpl_->virtualBeacon_[ i ] = MachLog::NO_BEACON;
}

void MachLogRaces::clear()
{
    _DELETE( pDataImpl_->pStats_ );
    pDataImpl_->pStats_ = NULL;

	_DELETE( pDataImpl_->pResearchTree_ );
    pDataImpl_->pResearchTree_ = NULL;

	_DELETE( pDataImpl_->pConstructionTree_ );
    pDataImpl_->pConstructionTree_ = NULL;

	_DELETE( pDataImpl_->pGameCreationData_ );
    pDataImpl_->pGameCreationData_ = NULL;

    //Clear out per race data
    for( size_t i = 0; i != MachPhys::N_RACES; ++i )
    {
        //Nullificify the race and controller pointers
        pDataImpl_->race_[i] = NULL;
        pDataImpl_->controller_[i] = NULL;
        pDataImpl_->thePCController_ = NULL;

        //Clear away the list of assembly points
        if( pDataImpl_->aggressorAssemblyPoints_[i].size() != 0 )
            pDataImpl_->aggressorAssemblyPoints_[i].erase( pDataImpl_->aggressorAssemblyPoints_[i].begin(),
                                               pDataImpl_->aggressorAssemblyPoints_[i].end() );

        if( pDataImpl_->administratorAssemblyPoints_[i].size() != 0 )
            pDataImpl_->administratorAssemblyPoints_[i].erase( pDataImpl_->administratorAssemblyPoints_[i].begin(),
                                                   pDataImpl_->administratorAssemblyPoints_[i].end() );

        //Clear out the squadron pointers
        if( pDataImpl_->squadrons_[i].size() != 0 )
            pDataImpl_->squadrons_[i].erase( pDataImpl_->squadrons_[i].begin(), pDataImpl_->squadrons_[i].end() );

    }
}

const MachLogRaces::CameraInfo& MachLogRaces::cameraInfo( MachPhys::Race race ) const
{
	return pDataImpl_->cameraInfo_[ race ];
}

void MachLogRaces::cameraInfo( MachPhys::Race race, const MexTransform3d& pos, MachLogCamera::Type type )
{
	pDataImpl_->cameraInfo_[ race ].position_ = pos;
	pDataImpl_->cameraInfo_[ race ].type_ = type;
}

MachLog::BeaconType MachLogRaces::beaconType( MachPhys::Race race )
{
	if( pDataImpl_->virtualBeacon_[ race ] == MachLog::LEVEL_3_BEACON )
		return MachLog::LEVEL_3_BEACON;

	MachLog::BeaconType result = pDataImpl_->virtualBeacon_[ race ];
	for( Beacons::const_iterator i = beacons( race ).begin(); i != beacons( race ).end(); ++i )
	{
		if( (*i)->isComplete() )
		{
			if( (*i)->level() == 3 )
				result = MachLog::LEVEL_3_BEACON;
			else if( (*i)->level() == 1 and result != MachLog::LEVEL_3_BEACON )
				result = MachLog::LEVEL_1_BEACON;
		}
	}
	return result;
}


void MachLogRaces::virtualBeacon( MachPhys::Race race, MachLog::BeaconType beaconType )
{
	pDataImpl_->virtualBeacon_[ race ] = beaconType;
}

MachLogArtefacts& MachLogRaces::artefacts()
{
    return *(pDataImpl_->pArtefacts_);
}

const string& MachLogRaces::currentPlanet() const
{
	return pDataImpl_->currentPlanet_;
}

void MachLogRaces::totalResearchScoreAdjust( MachPhys::Race researchingRace, int value )
{
	MachLogScore& researchingRaceScore =  *( pDataImpl_->scores_[ researchingRace ] );

	researchingRaceScore.totalResearchIncreased( value );
}

void MachLogRaces::newItemResearchedScoreAdjust( MachPhys::Race researchingRace )
{
	MachLogScore& researchingRaceScore =  *( pDataImpl_->scores_[ researchingRace ] );

	researchingRaceScore.itemResearched();
}

void MachLogRaces::BMUsMinedScoreAdjust( MachPhys::Race miningRace, int value )
{
	MachLogScore& miningRaceScore =  *( pDataImpl_->scores_[ miningRace ] );

	miningRaceScore.BMUsMinedIncreased( value );
}

void MachLogRaces::machinesBuiltScoreAdjust( MachPhys::Race buildingRace, const MachLogMachine& machine )
{
	MachLogScore& buildingRaceScore =  *( pDataImpl_->scores_[ buildingRace ] );

	switch( machine.objectType() )
	{
		case MachLog::AGGRESSOR:
		case MachLog::ADMINISTRATOR:
		case MachLog::APC:
		case MachLog::SPY_LOCATOR:
			buildingRaceScore.militaryMachineBuilt();
		default:
			;
	}

	buildingRaceScore.machineBuilt();
}

void MachLogRaces::actorDestroyedScoreAdjust( MachPhys::Race losingRace, int value, const MachActor& actor, MachPhys::Race gainingRace )
{
	MachLogScore& losingRaceScore =  *( pDataImpl_->scores_[ losingRace ] );

	if( gainingRace == MachPhys::N_RACES )
	{
		// looks like this is a unilateral, err, score loss.
		losingRaceScore.actorDestroyedNoCulprit( value, actor );
	}
	else
	{
		// we have both winners and losers from this saddening use of violence
		MachLogScore& gainingRaceScore =  *( pDataImpl_->scores_[ gainingRace ] );

		losingRaceScore.actorDestroyedByRace( value, actor, gainingRace );
		gainingRaceScore.destroyedAnotherRacesActor( value, actor, losingRace );
	}
}

const MachLogScore& MachLogRaces::score( MachPhys::Race r ) const
{
	return *pDataImpl_->scores_[ r ];
}

const MachLogRaces::Scores& MachLogRaces::scores() const
{
	return pDataImpl_->scores_;
}

void MachLogRaces::persistenceWriteNumbers( PerOstream& ostr )
{
	MachLogRacesDataImpl& dataImpl = *pDataImpl_;
	PER_WRITE_RAW_OBJECT( ostr, dataImpl );
	string endingFlicName( endingFlic() );
	PER_WRITE_RAW_OBJECT( ostr, endingFlicName );
}
void MachLogRaces::persistenceReadNumbers( PerIstream& istr )
{
	MachLogRacesDataImpl& dataImpl = *pDataImpl_;
	PER_READ_RAW_OBJECT( istr, dataImpl );
	string endingFlicName;
	PER_READ_RAW_OBJECT( istr, endingFlicName );
	endingFlic( endingFlicName );
}

void MachLogRaces::doGlobalFixup()
{
}


bool MachLogRaces::vortexHasBeenFired( MachPhys::Race race ) const
{
	return pDataImpl_->vortexHasBeenFired_[ race ];
}
void MachLogRaces::vortexHasBeenFired( MachPhys::Race race , bool newValue )
{
	pDataImpl_->vortexHasBeenFired_[ race ] = newValue;
}

int	MachLogRaces::techniciansTreacheried( MachPhys::Race race ) const
{
	return pDataImpl_->techniciansTreacheried_[ race ];
}
void MachLogRaces::techniciansTreacheried( MachPhys::Race race , int newValue )
{
	pDataImpl_->techniciansTreacheried_[ race ] = newValue;
}

bool MachLogRaces::firedAt( MachPhys::Race firingRace, MachPhys::Race firedAtRace ) const
{
	return pDataImpl_->firedAt_[ firingRace ][firedAtRace ];
}

void MachLogRaces::firedAt( MachPhys::Race firingRace, MachPhys::Race firedAtRace, bool newValue )
{
	pDataImpl_->firedAt_[ firingRace ][firedAtRace ] = newValue;
}

bool MachLogRaces::hasWon( MachPhys::Race race ) const
{
	return pDataImpl_->hasWon_[ race ];
}
void MachLogRaces::hasWon( MachPhys::Race race, bool newValue )
{
	pDataImpl_->hasWon_[ race ] = newValue;
}

bool MachLogRaces::hasLost( MachPhys::Race race ) const
{
	return pDataImpl_->hasLost_[ race ];
}
void MachLogRaces::hasLost( MachPhys::Race race, bool newValue )
{
	pDataImpl_->hasLost_[ race ] = newValue;
	if( newValue and MachLogNetwork::instance().isNetworkGame() and MachLogNetwork::instance().remoteStatus( race ) == MachLogNetwork::LOCAL_PROCESS )
	{
		MachLogNetwork::instance().messageBroker().sendLostFlagSetMessage( race );
	}
	if( gameType() == MachLog::SKIRMISH_SINGLE_PLAYER
		or ( MachLogNetwork::instance().isNetworkGame() and MachLogNetwork::instance().remoteStatus( race ) == MachLogNetwork::LOCAL_PROCESS ) )
	{
		MachLogRaces::Objects& objects = raceObjects( race );
		for( MachLogRaces::Objects::iterator i = objects.begin(); i != objects.end(); ++i )
		{
			if( (*i)->objectIsMachine() or (*i)->objectIsConstruction() )
				(*i)->selfDestruct( 0.01 );
		}
	}
}

void MachLogRaces::endingFlic( const string& newValue )
{
	pDataImpl_->endingFlic_ = newValue;
}
const string& MachLogRaces::endingFlic() const
{
	return pDataImpl_->endingFlic_;
}

bool MachLogRaces::podCaptured( MachPhys::Race race ) const
{
	return pDataImpl_->podCaptured_[ race ];
}

void MachLogRaces::podCaptured( MachPhys::Race race , bool newValue )
{
	pDataImpl_->podCaptured_[ race ] = newValue;
}

void MachLogRaces::builtIonCannon( MachPhys::Race r )
{
	if( not pDataImpl_->builtIonCannon_[ r ] )
	{
		// this is the first time this race has built a weapon of this type.

		pDataImpl_->builtIonCannon_[ r ] = true;

		// we can detect another race's acquisition of ion cannon technology if we have an uplink and satellite -
		// if so, give voicemail notification

		MachPhys::Race myRace = pcController().race();

		if( myRace != r
			and beaconType( myRace ) == MachLog::LEVEL_3_BEACON )
		{
			// yup, we've detected this new acquisition. Give warning voicemail.
			MachLogVoiceMailManager::instance().postNewMail( VID_POD_ENEMY_ION_CANNON_ONLINE, myRace );
		}

		// irrespective of monitoring technology, we need to activate the new graphical pod dome stuff for all pods of
		// that race.
		for( Pods::iterator i = pods( r ).begin(); i != pods( r ).end(); ++i )
		{
			(*i)->activateIonCannon();
		}
	}
}

void MachLogRaces::builtNuclearWeapon( MachPhys::Race r )
{
	if( not pDataImpl_->builtNuclearWeapon_[ r ] )
	{
		// this is the first time this race has built a weapon of this type.

		pDataImpl_->builtNuclearWeapon_[ r ] = true;

		// we can detect another race's building of a nuclear silo if we have an uplink and satellite -
		// if so, give voicemail notification

		MachPhys::Race myRace = pcController().race();

		if( myRace != r
			and beaconType( myRace ) == MachLog::LEVEL_3_BEACON )
		{
			// yup, we've detected this new acquisition. Give warning voicemail.
			MachLogVoiceMailManager::instance().postNewMail( VID_POD_ENEMY_NUKE_ONLINE, myRace );
	}
	}
}

size_t	MachLogRaces::AICommandId()
{
	return ++pDataImpl_->AICommandId_;
}

const size_t MachLogRaces::AINoCommandID() const
{
	return 0;
}

ostream& operator<<( ostream& o, const MachLogRaces::SafeActorType& actorType )
{
	switch( actorType )
	{
		case MachLogRaces::AGGRESSIVE_MACHINES: o << "AGGRESSIVE_MACHINES"; break;
		case MachLogRaces::MISSILE_EMPLACEMENTS: o << "MISSILE_EMPLACEMENTS"; break;
		case MachLogRaces::GARRISONS: o << "GARRISONS"; break;
		case MachLogRaces::PODS: o << "PODS"; break;
		case MachLogRaces::AGGRESSIVE_MACHINES_AND_MISSILE_EMPLACEMENTS: o << "AGGRESSIVE_MACHINES_AND_MISSILE_EMPLACEMENTS"; break;
	}
	return o;
}

MachLogRemoteFirstPersonManager& MachLogRaces::remoteFirstPersonManager()
{
    return *pDataImpl_->pRemoteFirstPersonManager_;
}

bool MachLogRaces::findAlternativeTargetClosestTo( const MachActor& actor,
										MachActor** ppResult, MATHEX_SCALAR minEuclidianDistance ) const
{
	PRE( actor.objectIsCanAttack() );
	PRE( actor.asCanAttack().hasCurrentTarget() );

	bool found = false;
	bool isLocalHumanPlayer = ( actor.race() == pcController().race() );
	MexPoint3d pos = actor.position();
	MATHEX_SCALAR closest = 1000000000;
	const MachLogCanAttack& actorAsCanAttack = actor.asCanAttack();

	const MachActor* pCurrentTarget = &( actor.asCanAttack().currentTarget() );

	MATHEX_SCALAR sqrScannerRange = 2500;

	if( actor.objectIsMachine() )
		sqrScannerRange = sqr( actor.asMachine().effectiveScannerRange() );
	else if( actor.objectIsMissileEmplacement() )
		sqrScannerRange = sqr( actor.asMissileEmplacement().scannerRange() );

	for( MachPhys::Race ridx = MachPhys::RED; ridx != MachPhys::N_RACES; ++((int&)ridx) )
	{
		MachLogRaces::DispositionToRace disp = dispositionToRace( actor.race(), ridx );

		if( disp != MachLogRaces::ALLY and
			disp != MachLogRaces::OUR_RACE )
		{
			for( Aggressors::const_iterator i = aggressors( ridx ).begin(); i != aggressors( ridx ).end(); ++i )
			{
				MachActor* pTargetActor = (*i);

				if( pCurrentTarget != pTargetActor )
				{
					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

					if( dist < closest
						and dist >= minEuclidianDistance
						and actorAsCanAttack.canFireAt( (**i) )
						and not (*i)->isDead() )
					{
						found = true;
						closest = dist;
						*ppResult = *i;
					}
				}
			}

			for( Administrators::const_iterator i = administrators( ridx ).begin(); i != administrators( ridx ).end(); ++i )
			{
				MachActor* pTargetActor = (*i);

				if( pCurrentTarget != pTargetActor )
				{
					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

					if( dist < closest
						and dist >= minEuclidianDistance
						and actorAsCanAttack.canFireAt( (**i) )
						and not (*i)->isDead() )
					{
						found = true;
						closest = dist;
						*ppResult = *i;
					}
				}
			}

			// Check for non-ICBM turrets
			for( MissileEmplacements::const_iterator i = missileEmplacements( ridx ).begin(); i != missileEmplacements( ridx ).end(); ++i )
			{
				MachLogMissileEmplacement* pTurret = (*i);
				if( pCurrentTarget != pTurret )
				{
					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( pTurret->position() );

					if( dist < closest
						and dist >= minEuclidianDistance
						and pTurret->subType() != MachPhys::ICBM
						and actorAsCanAttack.canFireAt( (*pTurret) )
						and ( not( isLocalHumanPlayer ) or pTurret->isComplete() )  // note that local player machines will not consider incomplete turrets as viable targets
						and not pTurret->isDead() )
					{
						found = true;
						closest = dist;
						*ppResult = pTurret;
					}
				}
			}

			// --------- EFFICIENCY-RELATED CHANGE ------------------
			// aggressors, administrators and turrets are our prime targets - stop here if we now have a match within
			// scanner range (ends search for this particular race)
			if( found
				and MachPhysRandom::randomInt( 0, 10 ) != 0		// 10% chance of checking everything regardless
				and not actor.objectIsMissileEmplacement()			// Missile emps always find ACTUAL closest target
				and not( actor.objectIsMachine() and actor.asMachine().insideBuilding() ) // For god's sake, check for technicians etc. if we're inside a building!
				and closest < std::min( sqrScannerRange, 22500.0 ) )		// 150m is furthest away aggressive can be if we are to abort the search
				continue;
			// --------- EFFICIENCY-RELATED CHANGE ------------------


			for( Technicians::const_iterator i = technicians( ridx ).begin(); i != technicians( ridx ).end(); ++i )
			{
				MachActor* pTargetActor = (*i);

				if( pCurrentTarget != pTargetActor )
				{
					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

					if( dist < closest
						and dist >= minEuclidianDistance
						and actorAsCanAttack.canFireAt( (**i) )
						and not (*i)->isDead() )
					{
						found = true;
						closest = dist;
						*ppResult = *i;
					}
				}
			}

			for( Constructors::const_iterator i = constructors( ridx ).begin(); i != constructors( ridx ).end(); ++i )
			{
				MachActor* pTargetActor = (*i);

				if( pCurrentTarget != pTargetActor )
				{
					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

					if( dist < closest
						and dist >= minEuclidianDistance
						and actorAsCanAttack.canFireAt( (**i) )
						and not (*i)->isDead() )
					{
						found = true;
						closest = dist;
						*ppResult = *i;
					}
				}
			}

			for( GeoLocators::const_iterator i = geoLocators( ridx ).begin(); i != geoLocators( ridx ).end(); ++i )
			{
				MachActor* pTargetActor = (*i);

				if( pCurrentTarget != pTargetActor )
				{
					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

					if( dist < closest
						and dist >= minEuclidianDistance
						and actorAsCanAttack.canFireAt( (**i) )
						and not (*i)->isDead() )
					{
						found = true;
						closest = dist;
						*ppResult = *i;
					}
				}
			}

			for( SpyLocators::const_iterator i = spyLocators( ridx ).begin(); i != spyLocators( ridx ).end(); ++i )
			{
				MachActor* pTargetActor = (*i);

				if( pCurrentTarget != pTargetActor )
				{
					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

					if( dist < closest
						and dist >= minEuclidianDistance
						and actorAsCanAttack.canFireAt( (**i) )
						and not (*i)->isDead()
						and not (*i)->camouflaged() )	// special clause for spies
					{
						found = true;
						closest = dist;
						*ppResult = *i;
					}
				}
			}

			for( ResourceCarriers::const_iterator i = resourceCarriers( ridx ).begin(); i != resourceCarriers( ridx ).end(); ++i )
			{
				MachActor* pTargetActor = (*i);

				if( pCurrentTarget != pTargetActor )
				{
					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

					if( dist < closest
						and dist >= minEuclidianDistance
						and actorAsCanAttack.canFireAt( (**i) )
						and not (*i)->isDead() )
					{
						found = true;
						closest = dist;
						*ppResult = *i;
					}
				}
			}

			for( APCs::const_iterator i = apcs( ridx ).begin(); i != apcs( ridx ).end(); ++i )
			{
				MachActor* pTargetActor = (*i);

				if( pCurrentTarget != pTargetActor )
				{
					MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

					if( dist < closest
						and dist >= minEuclidianDistance
						and actorAsCanAttack.canFireAt( (**i) )
						and not (*i)->isDead() )
					{
						found = true;
						closest = dist;
						*ppResult = *i;
					}
				}
			}

			////////////////////////////////////// BEGIN AI RACES ONLY SECTION /////////////////////////////////////

			// only AI races also check through constructions and the like if none of the more viable targets
			// above were found
			if( controller( actor.race() ).type() == MachLogController::AI_CONTROLLER )
			{
				for( Beacons::const_iterator i = beacons( ridx ).begin(); i != beacons( ridx ).end(); ++i )
				{
					MachActor* pTargetActor = (*i);

					if( pCurrentTarget != pTargetActor )
					{
						MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

						if( dist < closest
							and dist >= minEuclidianDistance
							and actorAsCanAttack.canFireAt( (**i) )
							and not (*i)->isDead() )
						{
							found = true;
							closest = dist;
							*ppResult = *i;
						}
					}
				}

				for( Factories::const_iterator i = factories( ridx ).begin(); i != factories( ridx ).end(); ++i )
				{
					MachActor* pTargetActor = (*i);

					if( pCurrentTarget != pTargetActor )
					{
						MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

						if( dist < closest
							and dist >= minEuclidianDistance
							and actorAsCanAttack.canFireAt( (**i) )
							and not (*i)->isDead() )
						{
							found = true;
							closest = dist;
							*ppResult = *i;
						}
					}
				}

				for( Garrisons::const_iterator i = garrisons( ridx ).begin(); i != garrisons( ridx ).end(); ++i )
				{
					MachActor* pTargetActor = (*i);

					if( pCurrentTarget != pTargetActor )
					{
						MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

						if( dist < closest
							and dist >= minEuclidianDistance
							and actorAsCanAttack.canFireAt( (**i) )
							and not (*i)->isDead() )
						{
							found = true;
							closest = dist;
							*ppResult = *i;
						}
					}
				}

				for( HardwareLabs::const_iterator i = hardwareLabs( ridx ).begin(); i != hardwareLabs( ridx ).end(); ++i )
				{
					MachActor* pTargetActor = (*i);

					if( pCurrentTarget != pTargetActor )
					{
						MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

						if( dist < closest
							and dist >= minEuclidianDistance
							and actorAsCanAttack.canFireAt( (**i) )
							and not (*i)->isDead() )
						{
							found = true;
							closest = dist;
							*ppResult = *i;
						}
					}
				}

				for( Pods::const_iterator i = pods( ridx ).begin(); i != pods( ridx ).end(); ++i )
				{
					MachActor* pTargetActor = (*i);

					if( pCurrentTarget != pTargetActor )
					{
						MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

						if( dist < closest
							and dist >= minEuclidianDistance
							and actorAsCanAttack.canFireAt( (**i) )
							and not (*i)->isDead() )
						{
							found = true;
							closest = dist;
							*ppResult = *i;
						}
					}
				}

				for( Mines::const_iterator i = mines( ridx ).begin(); i != mines( ridx ).end(); ++i )
				{
					MachActor* pTargetActor = (*i);

					if( pCurrentTarget != pTargetActor )
					{
						MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

						if( dist < closest
							and dist >= minEuclidianDistance
							and actorAsCanAttack.canFireAt( (**i) )
							and not (*i)->isDead() )
						{
							found = true;
							closest = dist;
							*ppResult = *i;
						}
					}
				}

				for( Smelters::const_iterator i = smelters( ridx ).begin(); i != smelters( ridx ).end(); ++i )
				{
					MachActor* pTargetActor = (*i);

					if( pCurrentTarget != pTargetActor )
					{
						MATHEX_SCALAR dist = pos.sqrEuclidianDistance( (*i)->position() );

						if( dist < closest
							and dist >= minEuclidianDistance
							and actorAsCanAttack.canFireAt( (**i) )
							and not (*i)->isDead() )
						{
							found = true;
							closest = dist;
							*ppResult = *i;
						}
					}
				}

				// check for ICBM turrets only

				for( MissileEmplacements::const_iterator i = missileEmplacements( ridx ).begin(); i != missileEmplacements( ridx ).end(); ++i )
				{
					MachLogMissileEmplacement* pEmplacement = (*i);

					if( pCurrentTarget != pEmplacement )
					{
   						MATHEX_SCALAR dist = pos.sqrEuclidianDistance( pEmplacement->position() );

						if( pEmplacement->subType() == MachPhys::ICBM
							and dist < closest
							and dist >= minEuclidianDistance
							and actorAsCanAttack.canFireAt( *pEmplacement )
							and not pEmplacement->isDead() )
						{
							found = true;
							closest = dist;
							*ppResult = pEmplacement;
						}
					}
				}
			}
			////////////////////////////////////// END AI RACES ONLY SECTION /////////////////////////////////////
		}
	}

	return found;
}

void MachLogRaces::addSpecialUpdateActor( MachActor* pActor, const PhysRelativeTime& maxTime )
{
    //add to the special update actor id map
	MachLogRaces::Objects&	specialUpdateActors = pDataImpl_->specialUpdateActors_;
//	HAL_STREAM("(" << pActor->id() << ") MachLogRaces::addSpecialUpdateActor (void*)" << (void*)pActor << std::endl );
	if( not ctl_contains( &specialUpdateActors, pActor ) )
	{
//		HAL_STREAM(" added\n" );
		specialUpdateActors.push_back( pActor );
		pActor->isInSpecialUpdateActorsList( true );
		pActor->maximumTimeInSpecialUpdateActors( maxTime );
		pActor->startTimeInSpecialUpdateActors( SimManager::instance().currentTime() );
	}
}

void MachLogRaces::removeSpecialUpdateActor( MachActor* pActor, MachLog::ForceRemove forceRemove )
{
	//remove from the special update actor id map
//	HAL_STREAM("(" << pActor->id() << ") MachLogRaces::removeSpecialUpdateActor (void*)" << (void*)pActor << std::endl );
	const PhysRelativeTime& maxTime = pActor->maximumTimeInSpecialUpdateActors();
	if( forceRemove == MachLog::DO_NOT_FORCE_REMOVE and maxTime >= MexEpsilon::instance() )
		if( maxTime + pActor->startTimeInSpecialUpdateActors() > SimManager::instance().currentTime() )
			return;

	if( pDataImpl_->inSpecialActorUpdate_ )
		pDataImpl_->toBeRemovedFromSpecialUpdateActors_.push_back( pActor );
	else
	{
		MachLogRaces::Objects&	specialUpdateActors = pDataImpl_->specialUpdateActors_;
		if( ctl_contains( &specialUpdateActors, pActor ) )
			races_ctl_erase( &specialUpdateActors, pActor );
		pActor->isInSpecialUpdateActorsList( false );
	}
}

void MachLogRaces::specialActorUpdate()
{
	MachLogRaces::Objects&	specialUpdateActors = pDataImpl_->specialUpdateActors_;
	MachLogRaces::Objects&	toBeRemovedFromSpecialUpdateActors = pDataImpl_->toBeRemovedFromSpecialUpdateActors_;
	const PhysAbsoluteTime& now = SimManager::instance().currentTime();
//	HAL_STREAM(" MachLogRaces::specialActorUpdate siez " << specialUpdateActors.size() << std::endl );
	pDataImpl_->inSpecialActorUpdate_ = true;
	for( Objects::iterator i = specialUpdateActors.begin(); i != specialUpdateActors.end(); ++i )
	{
//		HAL_STREAM("  calling update for actor " << (void*)(*i) << std::endl );
		MachLogNetwork& network = MachLogNetwork::instance();
		if( network.isNetworkGame() )
		{
			if( network.remoteStatus( (*i)->race() ) == MachLogNetwork::LOCAL_PROCESS )
			{
				if( not NetNetwork::instance().imStuffed() )
					(*i)->update(0,0);
			}
			else
				removeSpecialUpdateActor( *i, MachLog::FORCE_REMOVE );
		}
		else
			(*i)->update( 0, 0 );
		const PhysRelativeTime& maxTime = (*i)->maximumTimeInSpecialUpdateActors();
		if( maxTime >= MexEpsilon::instance() and maxTime + (*i)->startTimeInSpecialUpdateActors() < now )
			removeSpecialUpdateActor( *i, MachLog::FORCE_REMOVE );
	}
	pDataImpl_->inSpecialActorUpdate_ = false;
//	HAL_STREAM(" calling remove on actor pointers " << toBeRemovedFromSpecialUpdateActors.size() << std::endl );
	for( Objects::iterator i = toBeRemovedFromSpecialUpdateActors.begin(); i != toBeRemovedFromSpecialUpdateActors.end(); ++i )
		removeSpecialUpdateActor( *i, MachLog::FORCE_REMOVE );

	toBeRemovedFromSpecialUpdateActors.erase( toBeRemovedFromSpecialUpdateActors.begin(), toBeRemovedFromSpecialUpdateActors.end() );
//	ostream& out = W4dManager::instance().sceneManager()->out();
//	out << "Special Update Actors " << specialUpdateActors.size() << std::endl;
//	for( Objects::iterator i = specialUpdateActors.begin(); i != specialUpdateActors.end(); ++i)
//	{
//		out << (*i)->id() << " " << (*i)->objectType() << " startTime " << (*i)->startTimeInSpecialUpdateActors() << " maxTime " << (*i)->maximumTimeInSpecialUpdateActors() << std::endl;
//	}
//	HAL_STREAM(" MachLogRaces::specialActorUpdate DONE size " << specialUpdateActors.size() << std::endl );
}

bool	MachLogRaces::inSpecialActorUpdate() const
{
	return pDataImpl_->inSpecialActorUpdate_;
}

void MachLogRaces::newSmelterCompleted( MachPhys::Race r )
{
	for( ResourceCarriers::iterator i = resourceCarriers( r ).begin(); i != resourceCarriers( r ).end(); ++i )
	{
		if( (*i)->isNormalResourceCarrier() )
			(*i)->informOfNewSmeltingBuildingCompletion();
	}
}

void MachLogRaces::newMineralSiteDiscovered( MachPhys::Race r )
{
	// makes any mine if race r which is currently without a mineral site
	// check to see if the one(s) newly-available can be assigned to

	for( Mines::iterator i = mines( r ).begin(); i != mines( r ).end(); ++i )
	{
		if( not (*i)->hasMineralSite() )
			(*i)->newMineralSiteAvailable();
	}
}

void MachLogRaces::databaseHandler( MachLogDatabaseHandler* pHandler )
{
    pDataImpl_->pDatabaseHandler_ = pHandler;
}

bool MachLogRaces::hasDatabaseHandler() const
{
    return pDataImpl_->pDatabaseHandler_ != NULL;
}

MachLogDatabaseHandler& MachLogRaces::databaseHandler()
{
    PRE( hasDatabaseHandler() );
    return *pDataImpl_->pDatabaseHandler_;
}

bool MachLogRaces::raceInGame( MachPhys::Race r ) const
{
	return pDataImpl_->raceInGame_[ r ] == true;
}

int MachLogRaces::nScavengers( MachPhys::Race r ) const
{
	int nScavs = 0;
	for( ResourceCarriers::const_iterator i = resourceCarriers( r ).begin(); i != resourceCarriers( r ).end(); ++i )
	{
		if( (*i)->isScavenger() )
			++nScavs;
	}

	return nScavs;
}

bool MachLogRaces::findMostValuableTarget( 	const MachActor& actor,
											MachActor** ppResult,
											MachLog::TargetSystemType targetSystemType,
											MachLog::ObjectType ot,
											const MexPoint3d& startingPoint,
											MATHEX_SCALAR minEuclidianDistance ) const
{
	PRE( actor.objectIsCanAttack() );

	bool found = false;
	bool favourBuildings = false;
	MexPoint3d pos = actor.position();

	int highestValueFound = 0;

	//if the startingPoint provided is all zeros then do not override actor.position otherwise do..
	if( startingPoint != MexPoint3d(0,0,0) )
		pos = startingPoint;

	if( targetSystemType == MachLog::FAVOUR_STATIC_TARGETS )
		favourBuildings = true;

	//the only restriction on distance here is the minimum distance.
	if( targetSystemType == MachLog::TARGET_NORMAL
		or targetSystemType == MachLog::FAVOUR_STATIC_TARGETS )
	{
		for( MachPhys::Race ridx = MachPhys::RED; ridx != MachPhys::N_RACES; ++((int&)ridx) )
		{
			MachLogRaces::DispositionToRace disp = dispositionToRace( actor.race(), ridx );
			if( disp != MachLogRaces::ALLY and
				disp != MachLogRaces::OUR_RACE )
			{
				for( Objects::const_iterator i = raceObjects( ridx ).begin();
					i != raceObjects( ridx ).end(); ++i )
				{
					MachLog::ObjectType ot = (*i)->objectType();
					if( ot != MachLog::ORE_HOLOGRAPH and ot != MachLog::DEBRIS and ot !=
						MachLog::LAND_MINE and ot != MachLog::SQUADRON and ot != MachLog::ARTEFACT )
					{
						MachActor* pTargetActor = (*i);

						MATHEX_SCALAR dist = pos.sqrEuclidianDistance( pTargetActor->position() );

						int militaryValue = pTargetActor->localStrength();

						//special-case clause for nuclear silos - they'd count as having 0 military value otherwise

						if( favourBuildings )
						{
							if( pTargetActor->objectIsConstruction() )
								militaryValue = _STATIC_CAST( int, pTargetActor->hpRatio() * _STATIC_CAST( MATHEX_SCALAR, pTargetActor->objectData().cost() ) );
							else
								// mobile targets not so valuable as they may move out of range (of e.g. a nuclear strike)
								militaryValue *= 0.5;
						}

						if( not favourBuildings
							and pTargetActor->objectType() == MachLog::MISSILE_EMPLACEMENT )
						{
							if( pTargetActor->asMissileEmplacement().subType() == MachPhys::ICBM  )
								militaryValue = 500;			// yeah, yeah, pretty arbitrary but that'll probably be okay.
							else if( pTargetActor->asMissileEmplacement().subType() != MachPhys::LAUNCHER  )
								militaryValue *= 0.5;			// turrets not seen as so valuable militarily to a global strike weapon
						}

						if(  (not pTargetActor->isDead() ) and militaryValue > highestValueFound and dist >= minEuclidianDistance )
						{
							if( pTargetActor->objectIsMachine() )
							{
								if( not( pTargetActor->asMachine().camouflaged() ) )
 								{
									found = true;
									highestValueFound = militaryValue;
									*ppResult = *i;
								}
							}
							else
							{
								found = true;
								highestValueFound = militaryValue;
								*ppResult = *i;
							}
						}
					}
				}
			}
		}
	}

	// if no targets with any military strength were found, simply attack the nearest target.
	if( not found )
		found = findTargetClosestTo( actor, ppResult, targetSystemType,ot,startingPoint, minEuclidianDistance);

	return found;
}


int MachLogRaces::nCompleteConstructions( MachPhys::Race r, MachLog::ObjectType type ) const
{
	HAL_STREAM("MachLogRaces::nCompleteConstructions " << r << " " << type << std::endl );
	HAL_INDENT( 2 );
	int result = 0;
	switch( type )
	{
		case MachLog::BEACON:
		{
			for( Beacons::const_iterator i = beacons( r ).begin(); i != beacons( r ).end(); ++i )
				if( (*i)->isComplete() )
					++result;
			break;
		}
		case MachLog::FACTORY:
		{
			for( Factories::const_iterator i = factories( r ).begin(); i != factories( r ).end(); ++i )
				if( (*i)->isComplete() )
					++result;
			break;
		}
		case MachLog::GARRISON:
		{
			for( Garrisons::const_iterator i = garrisons( r ).begin(); i != garrisons( r ).end(); ++i )
				if( (*i)->isComplete() )
					++result;
			break;
		}
		case MachLog::HARDWARE_LAB:
		{
			for( HardwareLabs::const_iterator i = hardwareLabs( r ).begin(); i != hardwareLabs( r ).end(); ++i )
				if( (*i)->isComplete() )
					++result;
			break;
		}
		case MachLog::MINE:
		{
			HAL_STREAM("mines " << mines(r).size() << std::endl );
			for( Mines::const_iterator i = mines( r ).begin(); i != mines( r ).end(); ++i )
				if( (*i)->isComplete() )
					++result;
			break;
		}
		case MachLog::MISSILE_EMPLACEMENT:
		{
			for( MissileEmplacements::const_iterator i = missileEmplacements( r ).begin(); i != missileEmplacements( r ).end(); ++i )
				if( (*i)->isComplete() )
					++result;
			break;
		}
		case MachLog::POD:
		{
			for( Pods::const_iterator i = pods( r ).begin(); i != pods( r ).end(); ++i )
				if( (*i)->isComplete() )
					++result;
			break;
		}
		case MachLog::SMELTER:
		{
			HAL_STREAM("Smelters " << smelters(r).size() << std::endl );
			for( Smelters::const_iterator i = smelters( r ).begin(); i != smelters( r ).end(); ++i )
				if( (*i)->isComplete() )
					++result;
			break;
		}
		default:
			ASSERT_INFO( type );
			ASSERT_BAD_CASE;
	}
	HAL_INDENT( -2 );
	HAL_STREAM("MachLogRaces::nCompleteConstructions " << r << " " << type << " result " << result << std::endl );
	return result;
}

bool MachLogRaces::smeltersFull( MachPhys::Race r ) const
{
	return nBuildingMaterialUnits( r ) >= nMaxBuildingMaterialUnits( r );
}

void MachLogRaces::constructionsBuiltScoreAdjust( MachPhys::Race buildingRace, const MachLogConstruction& /*construction*/ )
{
	MachLogScore& buildingRaceScore =  *( pDataImpl_->scores_[ buildingRace ] );

	buildingRaceScore.constructionBuilt();
}

void MachLogRaces::scoreShouldBeDisplayed( MachPhys::Race race, bool value )
{
	pDataImpl_->scores_[ race ]->scoreShouldBeDisplayed( value );
}

void MachLogRaces::removeNotDisplayedScores()
{
	for( MachPhys::Race i = MachPhys::RED; i != MachPhys::N_RACES; ++((int&)i) )
	{
		if( not pDataImpl_->scores_[ i ]->scoreShouldBeDisplayed() )
		{
			pDataImpl_->scores_[i]->resetNumbers();
			for( MachPhys::Race j = MachPhys::RED; j != MachPhys::N_RACES; ++((int&)j) )
			{
				pDataImpl_->scores_[j]->resetPartialNumbers( j );
			}
		}
	}
}

void MachLogRaces::registerDispositionChangeNotifiable( MachLogDispositionChangeNotifiable* pNotifiable )
{
	HAL_STREAM("MachLogRaces::registerDispositionChangeNotifiable " << (void*)pNotifiable << std::endl );
	MachLogRacesDataImpl::DispositionNotifiables& notifiables = pDataImpl_->dispositionNotifiables_;
	if( find( notifiables.begin(), notifiables.end(), pNotifiable ) == notifiables.end() )
		notifiables.push_back( pNotifiable );
}

void MachLogRaces::unregisterDispositionChangeNotifiable( MachLogDispositionChangeNotifiable* pNotifiable )
{
	HAL_STREAM("MachLogRaces::unregisterDispositionChangeNotifiable " << (void*)pNotifiable << std::endl );
	MachLogRacesDataImpl::DispositionNotifiables& notifiables = pDataImpl_->dispositionNotifiables_;
	MachLogRacesDataImpl::DispositionNotifiables::iterator i = find( notifiables.begin(), notifiables.end(), pNotifiable );
	if( i != notifiables.end() )
		notifiables.erase( i );
}

ostream& operator<<( ostream& o, MachLogRaces::DispositionToRace d)
{
	switch( d )
	{
		case MachLogRaces::OUR_RACE: o << "OUR_RACE"; break;
		case MachLogRaces::ALLY: o << "ALLY"; break;
		case MachLogRaces::NEUTRAL: o << "NEUTRAL"; break;
		case MachLogRaces::ENEMY: o << "ENEMY"; break;
	}
	return o;
}

MachLogRaces::DispositionToRace MachLogRaces::dispositionToRace( const MachPhys::Race checkRace, const MachPhys::Race targetRace ) const
{
    MachLogRaces::DispositionToRace result;

    if( checkRace != MachPhys::NORACE  and  targetRace != MachPhys::NORACE )
        result = pDataImpl_->disposition_[ map_MachPhysRace_to_size_t( checkRace ) ][map_MachPhysRace_to_size_t(targetRace)];
    else
        result = NEUTRAL;

	return result;
}

void MachLogRaces::dispositionToRace( const MachPhys::Race checkRace, const MachPhys::Race targetRace, MachLogRaces::DispositionToRace newDisposition )
{
	DispositionToRace oldDisposition = pDataImpl_->disposition_[ map_MachPhysRace_to_size_t( checkRace ) ][map_MachPhysRace_to_size_t(targetRace)];
	HAL_STREAM("MachLogRaces::dispositionToRace " << checkRace << " to " << targetRace << " new Disposition " << newDisposition << " old " << oldDisposition << std::endl );
	pDataImpl_->disposition_[ map_MachPhysRace_to_size_t( checkRace ) ][map_MachPhysRace_to_size_t(targetRace)] = newDisposition;
	MachLogRacesDataImpl::DispositionNotifiables& notifiables = pDataImpl_->dispositionNotifiables_;
	if( oldDisposition != newDisposition )
	{
		HAL_STREAM(" number of disposition notifiables " << notifiables.size() << std::endl );
		for( MachLogRacesDataImpl::DispositionNotifiables::iterator i = notifiables.begin(); i != notifiables.end(); ++i )
		{
			(*i)->notifyGeneralDispositionChange();
			(*i)->notifySpecificDispositionChange( checkRace, targetRace );
			if( newDisposition == ALLY )
			{
				(*i)->notifyDispositionChangeToAlly( checkRace, targetRace );
			}
			else if( newDisposition == NEUTRAL or newDisposition == ENEMY )
			{
				(*i)->notifyDispositionChangeToNoneAlly( checkRace, targetRace );
			}
		}
	}
	if( MachLogNetwork::instance().isNetworkGame() )
	{
		MachLogNetwork::instance().messageBroker().sendChangeOfDispositionMessage( checkRace, targetRace, (int)newDisposition );
	}
}

void MachLogRaces::dispositionToRaceWithoutEcho( const MachPhys::Race checkRace, const MachPhys::Race targetRace, DispositionToRace newDisposition )
{
	DispositionToRace oldDisposition = pDataImpl_->disposition_[ map_MachPhysRace_to_size_t( checkRace ) ][map_MachPhysRace_to_size_t(targetRace)];
	HAL_STREAM("MachLogRaces::dispositionToRaceWithoutEcho " << checkRace << " to " << targetRace << " new Disposition " << newDisposition << " old " << oldDisposition << std::endl );
	pDataImpl_->disposition_[ map_MachPhysRace_to_size_t( checkRace ) ][map_MachPhysRace_to_size_t(targetRace)] = newDisposition;
	MachLogRacesDataImpl::DispositionNotifiables& notifiables = pDataImpl_->dispositionNotifiables_;
	if( oldDisposition != newDisposition )
	{
		HAL_STREAM(" number of disposition notifiables " << notifiables.size() << std::endl );
		for( MachLogRacesDataImpl::DispositionNotifiables::iterator i = notifiables.begin(); i != notifiables.end(); ++i )
		{
			(*i)->notifyGeneralDispositionChange();
			(*i)->notifySpecificDispositionChange( checkRace, targetRace );
			if( newDisposition == ALLY )
			{
				(*i)->notifyDispositionChangeToAlly( checkRace, targetRace );
			}
			else if( newDisposition == NEUTRAL or newDisposition == ENEMY )
			{
				(*i)->notifyDispositionChangeToNoneAlly( checkRace, targetRace );
			}
		}
	}
}

void MachLogRaces::gameType( MachLog::GameType t )
{
	pDataImpl_->gameType_ = t;
}
MachLog::GameType MachLogRaces::gameType() const
{
	return pDataImpl_->gameType_;
}

void MachLogRaces::addToMachinesCollection( MachLogMachine* pMachine )
{
	ASSERT( not machineExists( pMachine ), "Tried to add duplicate of machine into machines collection!"  );
	ctl_append( &machines(), pMachine );

	++nMachines( pMachine->race() );
}

MachPhys::BuildingMaterialUnits MachLogRaces::increaseRaceCapacity( MachPhys::Race r, MachPhys::BuildingMaterialUnits amountIncreasedBy )
{
	PRE_INFO( amountIncreasedBy );
	PRE( amountIncreasedBy >= 0 );

	MachPhys::BuildingMaterialUnits& raceCapacity = nMaxBuildingMaterialUnits( r );

	raceCapacity += amountIncreasedBy;

	POST_INFO( raceCapacity );
	POST( raceCapacity >= 0 );

	return raceCapacity;
}

bool MachLogRaces::isUnloadingGame() const
{
    return pDataImpl_->isUnloadingGame_;
}

bool MachLogRaces::maxUnitsExist( MachPhys::Race race ) const
{
	bool result = false;
	size_t maxUnits = MachPhysData::instance().generalData().maxUnitsPerRace();
	if( gameType() == MachLog::CAMPAIGN_SINGLE_PLAYER )
	{
		maxUnits += MachPhysData::instance().generalData().campaignExtraUnitsPerRace();
	}
	if( raceInGame( race ) )
	{
		if( ( nMachines( race ) + nConstructions( race ) ) >= maxUnits )
			result = true;
	}
	return result;
}
