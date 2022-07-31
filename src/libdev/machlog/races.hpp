/*
 * R A C E S. H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
 * MachLogRaces is a collection of the Machines and Constructions
 * posessed by each of the machine races, indexed by race.
 */

#ifndef _MACHLOG_RACES_HPP
#define _MACHLOG_RACES_HPP

#include "base/base.hpp"
#include "base/istrrep.hpp"
#include "utility/id.hpp"
#include "mathex/point3d.hpp"
#include "mathex/transf3d.hpp"
#include "machphys/machphys.hpp"
#include "machlog/machlog.hpp"
#include "machlog/camera.hpp"

/* //////////////////////////////////////////////////////////////// */

class SysPathName;
class MexPoint2d;
class W4dSceneManager;
class MachLogMachine;
class MachActor;

class MachLogAggressor;
class MachLogAdministrator;
class MachLogTechnician;
class MachLogConstructionTree;
class MachLogConstructor;
class MachLogGeoLocator;
class MachLogSpyLocator;
class MachLogResourceCarrier;
class MachLogAPC;
class MachLogGarrison;
class MachLogHardwareLab;
class MachLogFactory;
class MachLogSmelter;
class MachLogMine;
class MachLogMissileEmplacement;
class MachLogBeacon;
class MachLogPod;
class MachLogRace;
class MachLogController;
class MachLogAIController;
class MachLogPCController;
class MachLogOreHolograph;
class MachLogSquadron;
class MachLogResearchTree;
class MachLogProductionUnit;
class MachLogStats;
class MachLogArtefacts;
class MachLogRacesDataImpl;
class MachPhysStation;
class MachLogArtefact;
class MachLogScore;
class MachLogGameCreationData;
class MachLogLoadSaveGameExtras;
class MachLogRemoteFirstPersonManager;
class MachLogDatabaseHandler;
class MachLogDispositionChangeNotifiable;

class UtlBoundedIdGenerator;
template < class X > class UtlBoundedIdPMap;
template < class X > class ctl_list;
template < class X > class ctl_pvector;
template < class X > class ctl_vector;

class MachLogRaces
{
public:

	enum DispositionToRace { OUR_RACE, NEUTRAL, ALLY, ENEMY };
	enum SafeActorType { AGGRESSIVE_MACHINES, MISSILE_EMPLACEMENTS, GARRISONS, PODS, AGGRESSIVE_MACHINES_AND_MISSILE_EMPLACEMENTS };

	static MachLogRaces& instance();

	~MachLogRaces();
	
	///////////////////////////////

    //Loads up all necessary objects to start a game on planet called planetName, using the
    //scenario called scenarioName.
    //The scene manager for the 3d view is pSceneManager.
    //True iff successful.
    bool loadGame( W4dSceneManager* pSceneManager, const string& planetName, const string& scenarioName, BaseProgressReporter* );

	//different flavour of load game designed for skirmish and multi-player
	bool loadGame( W4dSceneManager* pSceneManager, const string& planetName, const string& scenarioName,
				   const MachLogGameCreationData&, BaseProgressReporter* );


	//loads the planet surface file and config spaces - independantly of any scenario file loading
    bool loadPlanet( W4dSceneManager* pSceneManager, const string& planetName, BaseProgressReporter* );

//Loads up saved game using data found in file savedGameName
	//The scene manager for the 3d view is pSceneManager.
	//The planet to be loaded is planetName.
    //The original scenario file is scenarioFileName. Needed for artefact type data.
	//The perReadExtras method is called on the MLLSGExtras.
 	//MLLSGExtras can be NULL
	//True iff successful.
    bool loadSavedGame( W4dSceneManager* pSceneManager, const string& planetName, const string& scenarioFileName,
                        const SysPathName& savedGameName, MachLogLoadSaveGameExtras*, BaseProgressReporter* );

    //Saves game using saveGameName as file name - the perWrite method of the pointer is also
	//called
	//MLLSGExtras can be NULL
    //True iff successful.
    bool saveGame( const SysPathName& saveGameName, MachLogLoadSaveGameExtras* );

    //Unloads all objects associated with the current game
    void unloadGame();

    //True while the game is being unloaded
    bool isUnloadingGame() const;

	///////////////////////////////
		
	void remove( MachActor * pObj );

    //Really this method should take care of adding the actor to all appropriate views,
    //but at present it doesn't. It must be called, however.
    void add( MachActor* pActor );

    //Really this method DOES take care of adding the actor to all appropriate views,
    //it should only be called on change of race.
    void addToAllCollections( MachActor* pActor );

    //True if there is an actor with id id
    bool actorExists( UtlId id ) const;

    //The actor with id id
    MachActor& actor( UtlId id );
    //PRE( actorExists( id ) )
        		
	///////////////////////////////

	typedef ctl_list< MachLogMachine * >	Machines;
	Machines& machines();
	const Machines& machines() const;

	bool machineExists( const MachLogMachine * ) const;
	void addToMachinesCollection( MachLogMachine* pMachine );
	
	///////////////////////////////

	typedef ctl_list< MachActor * >	Objects;
	Objects& objects();
	const Objects& objects() const;
	
	bool objectExists( const MachActor * ) const;
					
	Objects& raceObjects( MachPhys::Race );
	const Objects& raceObjects( MachPhys::Race ) const;
	///////////////////////////////

	typedef ctl_list< MachLogAggressor * > 			Aggressors;
	typedef ctl_list< MachLogAdministrator * >		Administrators;
	typedef ctl_list< MachLogTechnician * > 		Technicians;
	typedef ctl_list< MachLogConstructor * >		Constructors;
	typedef ctl_list< MachLogGeoLocator * > 		GeoLocators;
	typedef ctl_list< MachLogSpyLocator * > 				SpyLocators;
	typedef ctl_list< MachLogResourceCarrier * >	ResourceCarriers;
	typedef ctl_list< MachLogAPC * >				APCs;
	
	///////////////////////////////

	typedef ctl_list< MachLogGarrison * >			Garrisons;
	typedef ctl_list< MachLogHardwareLab * >		HardwareLabs;
	typedef ctl_list< MachLogFactory * >			Factories;
	typedef ctl_list< MachLogSmelter * >			Smelters;
	typedef ctl_list< MachLogMine * >				Mines;
	typedef ctl_list< MachLogMissileEmplacement * >	MissileEmplacements;
	typedef ctl_list< MachLogPod * >				Pods;
	typedef ctl_list< MachLogOreHolograph * >		Holographs;
	typedef ctl_list< MachLogBeacon * >				Beacons;

	///////////////////////////////

	typedef ctl_pvector< MachLogSquadron >			Squadrons;
	typedef ctl_vector< MexPoint2d >				AssemblyPoints;

	typedef ctl_pvector< MachLogArtefact > 			Artefacts;

	//find the first available factory of that type to build a machine return indicates true found one 
	//factory returned in ppFactory
	bool getSuitableFactory( MachPhys::Race r, const MachLogProductionUnit& , MachLogFactory**);
	bool addMatchingMachineToSquadron( MachPhys::Race r, const MachLogProductionUnit&, MachLogSquadron* );

	enum FindGarrisonCriterion { FREE_BAYS_ONLY, FREE_BAYS_UNIMPORTANT, FREE_BAYS_PLUS_NO_ENEMY_CANATTACKS };
	bool findFriendlyGarrisonClosestTo( const MachLogMachine&, MachLogGarrison** ppGarrison,
										MachPhysStation** ppStation, MachLogRaces::FindGarrisonCriterion criterion );

	Aggressors& aggressors( MachPhys::Race r );
	const Aggressors& aggressors( MachPhys::Race r ) const;

	Administrators& administrators( MachPhys::Race r );
	const Administrators& administrators( MachPhys::Race r ) const;

	Technicians& technicians( MachPhys::Race r );
	const Technicians& technicians( MachPhys::Race r ) const;

	Constructors& constructors( MachPhys::Race r );
	const Constructors& constructors( MachPhys::Race r ) const;

	GeoLocators& geoLocators( MachPhys::Race r );
	const GeoLocators& geoLocators( MachPhys::Race r ) const;

	SpyLocators& spyLocators( MachPhys::Race r );
	const SpyLocators& spyLocators( MachPhys::Race r ) const;

	ResourceCarriers& resourceCarriers( MachPhys::Race r );
	const ResourceCarriers& resourceCarriers( MachPhys::Race r ) const;
	
	APCs& apcs( MachPhys::Race r );
	const APCs& apcs( MachPhys::Race r ) const;
	
	///////////////////////////////

	Garrisons& garrisons( MachPhys::Race r );
	const Garrisons& garrisons( MachPhys::Race r ) const;

	HardwareLabs& hardwareLabs( MachPhys::Race r );
	const HardwareLabs& hardwareLabs( MachPhys::Race r ) const;

	Factories& factories( MachPhys::Race r );
	const Factories& factories( MachPhys::Race r ) const;

	Smelters& smelters( MachPhys::Race r );
	const Smelters& smelters( MachPhys::Race r ) const;

	Mines& mines( MachPhys::Race r );
	const Mines& mines( MachPhys::Race r ) const;

	MissileEmplacements& missileEmplacements( MachPhys::Race r );
	const MissileEmplacements& missileEmplacements( MachPhys::Race r ) const;

	///////////////////////////////

	Pods& pods( MachPhys::Race r );
	const Pods& pods( MachPhys::Race r ) const;
	
	Artefacts& artefactsCollection();
	const Artefacts& artefactsCollection() const;

	Squadrons& squadrons( MachPhys::Race r );
	const Squadrons& squadrons( MachPhys::Race r ) const;

	bool freeSquadron( MachPhys::Race r , MachLogSquadron** );
	void addToSquadrons( const MachLogSquadron& );

	//const for statistics helper variables
	int nConstructions( MachPhys::Race r ) const;
	int nMachines( MachPhys::Race r ) const;
	MachPhys::BuildingMaterialUnits nBuildingMaterialUnits( MachPhys::Race r ) const;
	MachPhys::BuildingMaterialUnits nOre( MachPhys::Race r ) const;
	MachPhys::BuildingMaterialUnits nMaxBuildingMaterialUnits( MachPhys::Race r ) const;
	MachPhys::BuildingMaterialUnits nMaxOre( MachPhys::Race r ) const;

	void nBuildingMaterialUnits( MachPhys::Race r, MachPhys::BuildingMaterialUnits newValue );
	// PRE( newValue >= 0 );
	
	//non-const ref for statistics helper variables
	//these have to be public due to the way that they are changed directly 
	//by the relevant MLClasses.	
	int& nConstructions( MachPhys::Race r );
	int& nMachines( MachPhys::Race r );	
	MachPhys::BuildingMaterialUnits& nOre( MachPhys::Race r );
	MachPhys::BuildingMaterialUnits& nMaxBuildingMaterialUnits( MachPhys::Race r );
	MachPhys::BuildingMaterialUnits& nMaxOre( MachPhys::Race r );
	
	bool smeltersFull( MachPhys::Race r ) const;
	
	///////////////////////////////
	
    //handles creation of artefacts
    MachLogArtefacts& artefacts();

    //Handles coordination of first person controlled machines on remote nodes
    MachLogRemoteFirstPersonManager& remoteFirstPersonManager();

    //Set/get the database handler allowing interfacing with MachGui.
    //Ownership is NOT transferred by the set method. Client retains ownership.
    void databaseHandler( MachLogDatabaseHandler* pHandler );
    bool hasDatabaseHandler() const;
    MachLogDatabaseHandler& databaseHandler();
    //PRE( hasDatabaseHandler() );

	///////////////////////////////

	enum CreateDefaultSquadrons { CREATE_SQUADRONS, DO_NOT_CREATE_SQUADRONS };	
	MachLogRace& race( MachPhys::Race r );
	const MachLogRace& race( MachPhys::Race r ) const;
	void race( MachPhys::Race r, MachLogRace * pNewRace, CreateDefaultSquadrons );
	
	///////////////////////////////

	MachLogController& controller( MachPhys::Race r );
	const MachLogController& controller( MachPhys::Race r ) const;
	void controller( MachPhys::Race r, MachLogController * pNewController );

	MachLogAIController& AIController( MachPhys::Race r );
	const MachLogAIController& AIController( MachPhys::Race r ) const;

    //Register a new pc controller
	void pcController( MachLogPCController * pNewPCController );
    //PRE( not hasPCRace() );

    //True iff a PC race has been defined
    bool hasPCRace() const;

    //The player controlled race
    MachLogPCController& pcController() const;
    //PRE( hasPCRace() );
	
	///////////////////////////////

	Holographs& holographs( MachPhys::Race r );
	const Holographs& holographs( MachPhys::Race r ) const;

	///////////////////////////////

	Beacons& beacons( MachPhys::Race r );
	const Beacons& beacons( MachPhys::Race r ) const;
	
	//return the reference to the research tree
	MachLogResearchTree& researchTree();
	const MachLogResearchTree& researchTree() const;
	
	//return the reference to the constructions tree
	MachLogConstructionTree& constructionTree();
   	const MachLogConstructionTree& constructionTree() const;
	
	///////////////////////////////

	bool findTargetClosestTo( const  MachActor& actor, MachActor** ppResult, 
							MachLog::TargetSystemType targetSystemType = MachLog::TARGET_NORMAL, 
							MachLog::ObjectType = MachLog::AGGRESSOR, 
							const MexPoint3d& startingPoint = MexPoint3d(0,0,0),
							MATHEX_SCALAR minEuclidianDistance = 0 ) const;
							
							
	// this method is primarily for use with ion cannons, nukes etc. It finds the enemy target with
	// the highest military value (this takes into account local strength, so clusters of small machines
	// can rank as more valuable than one bigger machine). If it finds nothing with any military value,
	// it simply returns the nearest target using findTargetClosestTo.
	bool findMostValuableTarget( const  MachActor& actor, MachActor** ppResult, 
								 MachLog::TargetSystemType targetSystemType = MachLog::TARGET_NORMAL, 
								 MachLog::ObjectType = MachLog::AGGRESSOR, 
								 const MexPoint3d& startingPoint = MexPoint3d(0,0,0),
								 MATHEX_SCALAR minEuclidianDistance = 0 ) const;

	// only to be used in the case of a canAttack machine that already has a target. Excludes the current target
	// from the search, returning a viable alternative (if any exists within scanner range)						
	bool findAlternativeTargetClosestTo( const MachActor& actor, 
										MachActor** ppResult,
										MATHEX_SCALAR minEuclidianDistance = 0 ) const;
	//PRE( actor.objectIsCanAttack() );
	//PRE( actor.hasCurrentTarget() );

	bool findFriendlyClosestTo( const  MachActor& actor, MachActor** ppResult, 
							MachLog::TargetSystemType targetSystemType = MachLog::TARGET_NORMAL, 
							MachLog::ObjectType = MachLog::AGGRESSOR ) const;
							
	// this method finds the "safest" i.e. best bet for protection target actor for the subject actor, using
   	// criteria of perceived military strength and distance. It returns an integer representing the 'overall'
   	// estimated safety value of the actor found, which can subsequently be used for comparisons against actors
   	// found from searches using different safeActorType criteria etc.
   	int findSafestFriendly( const MachActor& actor, 
   							MachActor ** ppResult,
  							SafeActorType safeActorType );                                         							
	
	void allFriendlyAggressiveMachinesWithinRange( const MachActor* pActor,  
   												MATHEX_SCALAR range, 
 												ctl_pvector< MachActor>* pActors,
												MATHEX_SCALAR minRange = 0 );
							
  	void allFriendlyAggressivesWithinRange( const MachActor* pActor,  
   											MATHEX_SCALAR range, 
 											ctl_pvector< MachActor>* pActors,
											MATHEX_SCALAR minRange = 0 );							

	// this method will return true if passed a production unit specifying an intention to build
	// a construction that does not already have an alternative construction built (i.e. one that has
	// various different possible placings, only one of which should ever be built at any one time).
	bool noAlternativeAlreadyBuilt( MachPhys::Race, const MachLogProductionUnit& prod );
							
	// Helper methods
    //The generator used to generate the ids
	//this used to be in MachActor but the correct place for it is here
	//it has also been made public
    enum { ID_UPPER_BOUND = 2001 }; //This means no more than 2000 actors can exist.
	static UtlBoundedIdGenerator& idGenerator();
    //The maximum number of actors that can be created
    static int maxActors();

	//get set default defcon and also defcon of all machines of race
	void defCon( MachPhys::Race, MachLog::DefCon );
	MachLog::DefCon defCon( MachPhys::Race ) const;

	void cascadeUpdateForResearch( MachPhys::Race );

	const AssemblyPoints& aggressorAssemblyPoints( const MachPhys::Race );
	const AssemblyPoints& administratorAssemblyPoints( const MachPhys::Race );
	void addAggressorAssemblyPoint( const MachPhys::Race, const MexPoint2d& );
	void addAdministratorAssemblyPoint( const MachPhys::Race, const MexPoint2d& );

	DispositionToRace dispositionToRace( const MachPhys::Race checkRace, const MachPhys::Race targetRace ) const;
	void dispositionToRace( const MachPhys::Race checkRace, const MachPhys::Race targetRace, DispositionToRace newDisposition );
	void dispositionToRaceWithoutEcho( const MachPhys::Race checkRace, const MachPhys::Race targetRace, DispositionToRace newDisposition );

	void outputRace( ostream&, MachPhys::Race );

	//note use of non-const version as well.
	MachLogStats&			stats();
	const MachLogStats&		stats() const;

	void addExtraSquadron( MachPhys::Race );

	struct CameraInfo
	{
		MexTransform3d position_;
		MachLogCamera::Type type_;
	};

	// Get the initial camera position and type for the beginning of a game.
	const CameraInfo& cameraInfo( MachPhys::Race ) const;
	// Set the starting camera position and type. When a game is first loaded this information
	// is used to set the initial camera position etc.
	void cameraInfo( MachPhys::Race, const MexTransform3d& pos, MachLogCamera::Type );
	
	// these methods remove value passed as second parameter from race's BMU total, after clipping for being over maximum or less than 0
	// return value == value actually added or subtracted after clipping
	MachPhys::BuildingMaterialUnits smartAddBMUs( MachPhys::Race r, MachPhys::BuildingMaterialUnits amountToAdd );
	//PRE( amountToAdd >= 0 );
	
	MachPhys::BuildingMaterialUnits smartSubtractBMUs( MachPhys::Race r, MachPhys::BuildingMaterialUnits amountToAdd );
	//PRE( amountToSubtract >= 0 );
	
	MachPhys::BuildingMaterialUnits reduceCapacityWithPenalty( MachPhys::Race r, MachPhys::BuildingMaterialUnits amountReducedBy );
	MachPhys::BuildingMaterialUnits reduceCapacityNoPenalty( MachPhys::Race r, MachPhys::BuildingMaterialUnits amountReducedBy );
	MachPhys::BuildingMaterialUnits increaseRaceCapacity( MachPhys::Race r, MachPhys::BuildingMaterialUnits amountIncreasedBy );
	
	MachLog::BeaconType	beaconType( MachPhys::Race );
	void virtualBeacon( MachPhys::Race, MachLog::BeaconType );

	const string&	currentPlanet() const;

	typedef ctl_pvector< MachLogScore >			Scores;
	void totalResearchScoreAdjust( MachPhys::Race researchingRace, int value );
	void newItemResearchedScoreAdjust( MachPhys::Race researchingRace );
	void BMUsMinedScoreAdjust( MachPhys::Race miningRace, int value );
	void machinesBuiltScoreAdjust( MachPhys::Race buildingRace, const MachLogMachine& machine );
	void constructionsBuiltScoreAdjust( MachPhys::Race buildingRace, const MachLogConstruction& construction );
	void actorDestroyedScoreAdjust( MachPhys::Race losingRace, int value, const MachActor& victimActor, MachPhys::Race gainingRace = MachPhys::N_RACES );
	void scoreShouldBeDisplayed( MachPhys::Race race, bool value );
	void removeNotDisplayedScores();
   	const MachLogScore& score( MachPhys::Race ) const;
	const Scores& scores() const;

	size_t  AICommandId();
	const size_t AINoCommandID() const;
	//these sets of methods are used by the MLConditions code only and are very specific to certain
	//scenarios.
	bool vortexHasBeenFired( MachPhys::Race ) const;
	void vortexHasBeenFired( MachPhys::Race, bool );
	int     techniciansTreacheried( MachPhys::Race ) const;
	void techniciansTreacheried( MachPhys::Race, int );
	bool firedAt( MachPhys::Race firingRace, MachPhys::Race firedAtRace ) const;
	void firedAt( MachPhys::Race firingRace, MachPhys::Race firedAtRace, bool );
	bool hasWon( MachPhys::Race ) const;
	void hasWon( MachPhys::Race, bool );
	//has lost can be independantly set to hasWon in a multiplayer situation.
	bool hasLost( MachPhys::Race ) const;
	void hasLost( MachPhys::Race, bool );
	void endingFlic( const string& );
	const string& endingFlic() const;
	//certain scenarios win on capturing the pod - note how specific this is note good in the general sense
	//but the amount of work required in making this generic is prohibitive (need to check arrays
	//at run time etc)
	bool podCaptured( MachPhys::Race ) const;
	void podCaptured( MachPhys::Race, bool );
	
	void builtIonCannon( MachPhys::Race );
	void builtNuclearWeapon( MachPhys::Race );
	
	// whether or not a race is/has been taking part in this game
   	bool raceInGame( MachPhys::Race ) const;

	//these two functions will persist elements of MachLogRaces directly such as the BMUs per race etc
	//(The collections themselves are not persisted etc).
	void persistenceWriteNumbers( PerOstream& );
	void persistenceReadNumbers( PerIstream& );

	//add and remove to the special map.
	void removeSpecialUpdateActor( MachActor * pActor, MachLog::ForceRemove );
    void addSpecialUpdateActor( MachActor* pActor, const PhysRelativeTime& );
	//process the calls. This will be called (for the moment) from machgui loopcycle mechanism
	//it should really be called from a specially created updateEveryFrameActor.
	void specialActorUpdate();
	bool inSpecialActorUpdate() const;
	
	// this notifies all non-scavenger resource carriers of the race. They may want to try the
	// new smelter and make it their destination smelter
	void newSmelterCompleted( MachPhys::Race );
	
	// this notifies all mines of the race. They may want to try the
	// new site to assign to if not already assigned
	void newMineralSiteDiscovered( MachPhys::Race r );

	int nScavengers( MachPhys::Race r ) const;

	//Helper function which can easily compute number of complete buildings per type per race.
	int nCompleteConstructions( MachPhys::Race r, MachLog::ObjectType ) const;

	void registerDispositionChangeNotifiable( MachLogDispositionChangeNotifiable* );
	void unregisterDispositionChangeNotifiable( MachLogDispositionChangeNotifiable* );

	void gameType( MachLog::GameType );
	MachLog::GameType gameType() const;

	//returns true if nmachines+nconstructions >= machPhysdata::generaldata::maxUnitsPerRace	
	bool maxUnitsExist( MachPhys::Race race ) const;

private:
	//the data impl needs access to private typedefs etc.
	friend class MachLogRacesDataImpl;
	friend ostream& operator<<( ostream&, DispositionToRace );

	MachLogRaces();
	MachLogRaces( const MachLogRaces& );
	MachLogRaces& operator =( const MachLogRaces& );
	bool operator ==( const MachLogRaces& ) const;

    //Useful typedefs
    typedef class UtlBoundedIdPMap< class MachActor > ActorIdMap;	

	static size_t map_MachPhysRace_to_size_t( MachPhys::Race );
	// POST( result < MachPhys::N_RACES );

    //Sets up new copies of all dependent objects as required for a new game
    void initialise();

    //Destruct dependent objects
    void clear();
	
	//The is caled as part of the global loadSavedGame to refix machine config spaces etc.
	void doGlobalFixup();

	MachLogRacesDataImpl* pDataImpl_;
};

PER_ENUM_PERSISTENT( MachLogRaces::DispositionToRace );

/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef _MACHLOG_RACES_HPP	*/

/* End RACES.HPP *****************************************************/
