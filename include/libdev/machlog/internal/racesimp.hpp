/*
 * R A C E S I M P . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachLogRacesDataImpl

    MachLogRaces does not have data members.
*/

#ifndef _MACHLOG_RACESIMP_HPP
#define _MACHLOG_RACESIMP_HPP

#include "base/base.hpp"
#include "machlog/races.hpp"
#include "ctl/list.hpp"
#include "ctl/pvector.hpp"
#include "stdlib/string.hpp"

class MachLogDatabaseHandler;

class MachLogRacesDataImpl
// Canonical form revoked
{
public:
    MachLogRacesDataImpl();
    ~MachLogRacesDataImpl();

    void CLASS_INVARIANT;
	PER_MEMBER_PERSISTENT_DEFAULT( MachLogRacesDataImpl );
	PER_FRIEND_READ_WRITE( MachLogRacesDataImpl );
private:
	friend class MachLogRaces;
    friend ostream& operator <<( ostream& o, const MachLogRacesDataImpl& t );

    MachLogRacesDataImpl( const MachLogRacesDataImpl& );
    MachLogRacesDataImpl& operator =( const MachLogRacesDataImpl& );

	///////////////////////////////

	MachLogRaces::Aggressors			aggressors_[ MachPhys::N_RACES ];
	MachLogRaces::Administrators		administrators_[ MachPhys::N_RACES ];
	MachLogRaces::Technicians			technicians_[ MachPhys::N_RACES ];
	MachLogRaces::Constructors			constructors_[ MachPhys::N_RACES ];
	MachLogRaces::GeoLocators			geoLocators_[ MachPhys::N_RACES ];
	MachLogRaces::SpyLocators			spyLocators_[ MachPhys::N_RACES ];
	MachLogRaces::APCs					apcs_[ MachPhys::N_RACES ];
	MachLogRaces::ResourceCarriers		resourceCarriers_[ MachPhys::N_RACES ];
	
	///////////////////////////////

	MachLogRaces::Garrisons				garrisons_[ MachPhys::N_RACES ];
	MachLogRaces::HardwareLabs			hardwareLabs_[ MachPhys::N_RACES ];
	MachLogRaces::HardwareLabs			softwareLabs_[ MachPhys::N_RACES ];
	MachLogRaces::Factories				factories_[ MachPhys::N_RACES ];
	MachLogRaces::Smelters				smelters_[ MachPhys::N_RACES ];
	MachLogRaces::Mines					mines_[ MachPhys::N_RACES ];
	MachLogRaces::MissileEmplacements	missileEmplacements_[ MachPhys::N_RACES ];
	MachLogRaces::Holographs			holographs_[ MachPhys::N_RACES ];
	MachLogRaces::Beacons				beacons_[ MachPhys::N_RACES ];
		
	///////////////////////////////

	MachLogRaces::Pods					pods_[ MachPhys::N_RACES ];
	MachLogRaces::Artefacts	   			artefactsCollection_;

	MachLogRace *						race_[ MachPhys::N_RACES ];
    MachLogRace *						pNoRaceRace_; //The process for actors not belonging to a specific race
	MachLogController *					controller_[ MachPhys::N_RACES ];

	MachLogRaces::Machines				machines_;
	MachLogRaces::Objects				objects_;
	MachLogRaces::Objects				raceObjects_[ MachPhys::N_RACES ];

	MachLogRaces::AssemblyPoints		aggressorAssemblyPoints_[ MachPhys::N_RACES ];
	MachLogRaces::AssemblyPoints		administratorAssemblyPoints_[ MachPhys::N_RACES ];

	MachLogRaces::Squadrons				squadrons_[ MachPhys::N_RACES ];

    MachLogRaces::ActorIdMap* 			pActorIdMap_;//Map from actor id to MachActor*
    MachLogRaces::Objects 				specialUpdateActors_;
	MachLogRaces::Objects 				toBeRemovedFromSpecialUpdateActors_;

    MachLogPCController* 				thePCController_;//The one and only controller for the player

    MachLogArtefacts* 					pArtefacts_; //Handles the creation of artefacts
    MachLogRemoteFirstPersonManager* 	pRemoteFirstPersonManager_; //First person controlling in network game

	PhysAbsoluteTime					lastFullSmeltersUpdateTime_;
	PhysAbsoluteTime					lastLowBMUsUpdateTime_;
	
	//quick counting cache variables per race.
	int 							nConstructions_[ MachPhys::N_RACES ];
	int 							nMachines_[ MachPhys::N_RACES ];
	MachPhys::BuildingMaterialUnits nBuildingMaterialUnits_[ MachPhys::N_RACES ];
	MachPhys::BuildingMaterialUnits nOre_[ MachPhys::N_RACES ];
	MachPhys::BuildingMaterialUnits nMaxBuildingMaterialUnits_[ MachPhys::N_RACES ];
	MachPhys::BuildingMaterialUnits nMaxOre_[ MachPhys::N_RACES ];
	MachLogConstructionTree*       	pConstructionTree_;
	MachLogResearchTree*			pResearchTree_;
	MachLog::DefCon					defaultDefCon_[ MachPhys::N_RACES ];
	MachLogRaces::DispositionToRace	disposition_[ MachPhys::N_RACES ][ MachPhys::N_RACES ];
	MachLogStats*					pStats_;
	MachLogRaces::CameraInfo		cameraInfo_[ MachPhys::N_RACES ];
	MachLog::BeaconType				beaconType_[ MachPhys::N_RACES ];
	MachLog::BeaconType				virtualBeacon_[ MachPhys::N_RACES ];
	string							currentPlanet_;
	MachLogRaces::Scores			scores_;
	bool							vortexHasBeenFired_[ MachPhys::N_RACES ];
	int								techniciansTreacheried_[ MachPhys::N_RACES ];
	bool							firedAt_[ MachPhys::N_RACES ][ MachPhys::N_RACES ];
	bool							hasWon_[ MachPhys::N_RACES ];
	bool							hasLost_[ MachPhys::N_RACES ];
	bool							builtIonCannon_[ MachPhys::N_RACES ];
	bool							builtNuclearWeapon_[ MachPhys::N_RACES ];
	bool							raceInGame_[ MachPhys::N_RACES ];
	string							endingFlic_;
	bool							podCaptured_[ MachPhys::N_RACES ];
	size_t							AICommandId_;
	MachLogGameCreationData*		pGameCreationData_;
	bool							contactBetweenRaces_[ MachPhys::N_RACES] [ MachPhys::N_RACES ];
	bool							inSpecialActorUpdate_;
    bool                            isUnloadingGame_; //True while the current game is being destructed
    MachLogDatabaseHandler*         pDatabaseHandler_;
	typedef ctl_pvector< MachLogDispositionChangeNotifiable >	DispositionNotifiables;
	//Note that these are *****NOT***** persisted during a save game and should be recreated
	DispositionNotifiables			dispositionNotifiables_;
	MachLog::GameType				gameType_;

};

PER_DECLARE_PERSISTENT( MachLogRacesDataImpl );

#endif

/* End RACESIMP.HPP *************************************************/
