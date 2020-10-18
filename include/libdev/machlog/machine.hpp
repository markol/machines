/*
 * M A C H I N E . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
 *   MachLogMachine
 *   Implement the logical behaviour of Machines.
 */

#ifndef _LOG_MACHINE_HPP
#define _LOG_MACHINE_HPP

/* //////////////////////////////////////////////////////////////// */

#include "base/base.hpp"

#include "phys/phys.hpp"

#include "machlog/message.hpp"

#include "machlog/mobile.hpp"

/* //////////////////////////////////////////////////////////////// */

//forward refs
class MachPhysMachine;
class MachLogRace;
class W4dEntity;
class MexTransform3d;
class MexPoint2d;
class MachPhysMachineData;
class W4dDomain;
class MachPhysStation;
class MachLogEntrance;
class MachLogSquadron;
class MachPhysHealAura;
class MachLogMachineImpl;
class MachLog1stPersonMachineHandler;
template < class X > class ctl_pvector;

class MachLogMachine
: public MachLogMobile,
  public MachLogCanCommunicate
// canonical form revoked
{
public:

	typedef ctl_vector< UtlId > Actors;
	typedef ctl_pvector< MachLogMachine > Machines;
    typedef int Level; // Machine ability level

	virtual void sendMessage( MachLogCommsId to, const MachLogMessage& ) const;
	virtual void receiveMessage( const MachLogMessage& message );
	//return view of machPhys machine data
	virtual const MachPhysObjectData& objectData() const;
	virtual const MachPhysMachineData& machineData() const = 0;

    //True if the machine is exploding and should be regarded as dead
    bool isDying() const;

	//Set up intersects with physMachine
	void isEntering( W4dDomain& );
	//call physical attachTo
	void hasEntered( W4dDomain& );

	//machine has locked an entrance
	//Not the NULL is a valid parameter and indicates that the entrance/station has been unlocked.
	void entranceLocked( MachLogEntrance* );

    //True if an entrance has been locked by this machine
    bool hasLockedEntrance() const;

    //the entrance locked by this machine
    MachLogEntrance& lockedEntrance() const;
    //PRE( hasLockedEntrance() );

    //Ensure the appropriate internal/external entrance polygon for the locked entrance is
    //ignored if setIgnore true, and not ignored if false.
    void ignoreEntrancePolygon( bool setIgnore );
    //PRE( hasLockedEntrance() );

    //Ensure the obstacle polygon for the building we are on the pad of is ignored (or not)
    //according to setIgnore.
    void ignoreConstructionPolygon( bool setIgnore );
    //PRE( inBuildingOrOnPad() );
    //PRE( not insideBuilding() );

	//Set a locked station (or clear if argument is null).
	void stationLocked( MachPhysStation* );

	//return if the machine currently has a locked station.
	bool hasStationLocked() const;
	//return which station the machine currently has locked.
	MachPhysStation& stationLocked() const;

	//same names as used in the physical library.
	Level hwLevel() const;
	Level swLevel() const;

	const MachLogSquadron* squadron() const;
	MachLogSquadron* squadron();
	bool squadron( const MachLogSquadron* );

	//returns which building inside (if any) use insideWhichBuilding
	//query method to return which building the machine is inside.
	bool insideBuilding() const;
	MachLogConstruction& insideWhichBuilding();
	const MachLogConstruction& insideWhichBuilding() const;
	void insideWhichBuilding( MachLogConstruction* );

	// useful for attacking machines to know so they don't begin an enter building op
	bool isLeavingBuilding() const;

	// returns true if machine is currently entering building specified
	bool isEnteringBuilding( const MachLogConstruction& constron ) const;

    //True if in a building or on an entrance pad
    bool inBuildingOrOnPad() const;

    //The building this machine is in or on the pad of
    MachLogConstruction& inOrOnPadWhichBuilding();
    const MachLogConstruction& inOrOnPadWhichBuilding() const;
    //PRE( inBuildingOrOnPad() );

    //Inform in or on an entrance pad of pConstruction
    void inOrOnPadBuilding( MachLogConstruction* pConstruction );

	void insideAPC( bool );
	bool insideAPC( void ) const;
	void APCImInside( MachLogAPC* pAPC );
   	const MachLogAPC& APCImInside() const;

	void isStandingGround( bool newStandGroundStatus );
	bool isStandingGround() const;

	//will make the physical model attach to the planet hidden root and notify observers
	void hidePhysicalMachine();
	//This will move the machine in space to this position and make sure it is attached
	//to the correct root to be displayed.
	void snapTo( const MexPoint3d& );

	//get/set the machines defcon
	MachLog::DefCon defCon() const;
	MachLog::DefCon virtualDefCon() const;
	void defCon( MachLog::DefCon );

	//actors can assign themselves to different races.
	//This function is virtual to allow different behaviour in constructions
	//and machines.
	virtual void assignToDifferentRace( MachLogRace& newRace );

	//Amount of damage is passed in beHit()
	virtual void beHit( const int&, MachPhys::WeaponType byType = MachPhys::N_WEAPON_TYPES,
						MachActor* pByActor = NULL, MexLine3d* pByDirection = NULL, MachActor::EchoBeHit = MachActor::ECHO );

	virtual void beHitWithoutAnimation( int damage, PhysRelativeTime physicalTimeDelay = 0, MachActor* pByActor = NULL, EchoBeHit = ECHO );


	void addHealingAuraReference();
	void releaseHealingAuraReference();

    virtual ~MachLogMachine() = 0;

    // Override updates the logical machine's Ai process.
    // Clearance ignored for now.
    PhysRelativeTime update( const PhysRelativeTime& maxCPUTime, MATHEX_SCALAR clearanceFromDisplayedVolume );

	///////////////////////////////

	void smeltMe();	// will execute a vapourisation animation on associated physobject, and set up a dying event without debris

	virtual MachPhys::MachineType type() const;

	typedef MachLogMachine Self;

	friend bool operator <( const Self& a, const Self& b )
	{
		return a.id() < b.id();
	}

	friend bool operator ==( const Self& a, const Self& b )
	{
		return a.id() == b.id();
	}

	///////////////////////////////

    void CLASS_INVARIANT;

	///////////////////////////////

	virtual void doStartExplodingAnimation();
	virtual void doEndExplodingAnimation();

	///////////////////////////////

	bool inScannerRange( const MachActor& target, MATHEX_SCALAR multiplier = 1.0 ) const;
	bool inScannerRange( const MexPoint3d& targetPosition, MATHEX_SCALAR multiplier = 1.0 ) const;
//	bool hasSightOf( const MachLogMachine& other ) const;

	MATHEX_SCALAR lowClearence() const;
	MATHEX_SCALAR highClearence() const;

	const MachPhysMachine& physMachine() const;
	MachPhysMachine& physMachine();

	MachPhys::BuildingMaterialUnits machineOriginalCost() const;

	// am I physically capable of entering this building?
	bool canEnterConstruction( const MachLogConstruction& ) const;

	// am I physically capable of entering this building AND are there also free bays for me (if applicable)?
	// (Updates *ppFirstFreeStationFound if relevant.)
	bool canFitInsideConstructionNow( const MachLogConstruction& construction ) const;

	// check as canFitInsideConstructionNow but with extra requirement that door be unlocked.
	bool canEnterConstructionNow( const MachLogConstruction& construction ) const;

	void evading( bool status );
	bool evading() const;

	void camouflaged( bool status );
	bool camouflaged() const;

	bool doingTaskAttack() const;

	bool isFreeToAttack() const;

	bool destinationTakesUsClearOfThreats( const Actors& threats );

	// designed only for checking attack-capable otherActors
   	virtual bool fearsThisActor( const MachActor& otherActor ) const;

	// inherited from MachActor
   	int localStrength() const;

   	bool findSafestNearbyAggressiveMachine( MachLogMachine** ) const;
   	bool findSafestNearbyMissileEmplacement( MachLogMissileEmplacement** ) const;

	//inherited from MachActor
	virtual void populateStrongThreats( Actors* pActors );

	bool notAfraidOfStrongThreats( const Actors& strongThreats ) const;

	// populates pMachines, returns true if at least one following machine found.
	bool followingMachines( Machines* pMachines );

   	virtual bool beNotified( W4dSubject* pSubject, W4dSubject::NotificationEvent event, int );


    //  The low clearances are for use inside constructions, the high clearances
    //  are for use outside. The functions return the range of cleanaces necessary
    //  to accomodate all machine types.
    static  MATHEX_SCALAR minLowClearance();
    static  MATHEX_SCALAR maxLowClearance();
    static  MATHEX_SCALAR minHighClearance();
    static  MATHEX_SCALAR maxHighClearance();

	//do various bits and bobs and do common creation work for machines.
	//this will be called from places outside the constructors.
	void machineCreated();

	// ensures the machine acts as though in defcon high for a number of seconds (specified in parmdata.dat -
	// VIRTUAL_DEFCON) after a manual order is issued
	void manualCommandIssued();

	bool hasNVG() const;
	bool usingNVG() const;
	void usingNVG( bool status );
	// PRE( hasNVG() );

	void teleportIntoWorld();
	void teleportOutOfWorld();

	bool isTeleporting() const;

	// returns value between 0.0 (incapacitated by darkness) to 1.0 (full efficiency - effectiveness unimpaired)
	// (actually, 0.333 is currently minimum this function will return - that's darkest possible, no NVG )
	MATHEX_SCALAR lightLevelEfficiency() const;

	// scanner range altered by light efficiency
	MATHEX_SCALAR effectiveScannerRange() const;

	// "true" scanner range (ignoring light efficiency)
	MATHEX_SCALAR scannerRange() const;

	MATHEX_SCALAR scalarSpeedNow() const;

	bool machineIsGlider() const;

	// for use when we get blown up mid-teleport - clears flags so explosion animation can proceed correctly.
	void teleportingInterrupted();

	// takes into account NVG, proximity of administrators etc. 1.0 = total darkness.
	MATHEX_SCALAR perceivedDarkness() const;

	PER_MEMBER_PERSISTENT_ABSTRACT( MachLogMachine );
	PER_FRIEND_READ_WRITE( MachLogMachine );

	friend class MachLogTaskAttackOperation;

	void displayAsRace( MachPhys::Race );

	// makes nextGlobalHitVoiceMailTime ( time now + hitVoiceMailInterval() )
	static void resetNextGlobalHitVoiceMailTime();

	// this flag is used by TaskAttackOperation to keep track of which of its machines
	// have been ordered to go rampaging into the world after having assembled at an assembly point.
	void rampaging( bool status );
	bool rampaging() const;

protected:

    MachLogMachine( MachPhys::MachineType type, Level hwLevel, Level swLevel,
    					MachLogRace * pRace,
    					MachPhysMachine *,
    					MachLog::ObjectType );

    MachLogMachine( MachPhys::MachineType type, Level hwLevel, Level swLevel,
    					MachLogRace * pRace,
    					MachPhysMachine *,
    					MachLog::ObjectType,
    					UtlId withId );
	///////////////////////////////

	virtual void doOutputOperator( ostream& ) const;

	// inherited from MachActor
	virtual void checkAndDoOnDefCon( const Actors& strongThreats );

	///////////////////////////////
	void seeIfNeedRegister();

private:
	friend class MachLogMachineImpl;
	friend class MachLog1stPersonMachineHandler;

    // revoked...
    MachLogMachine( const MachLogMachine& );
    MachLogMachine& operator =( const MachLogMachine& );

	///////////////////////////////

	///////////////////////////////

    PhysRelativeTime startExplodingAnimation( void );
    PhysRelativeTime endExplodingAnimation( void );
	PhysRelativeTime virtualDefConInterval();

	///////////////////////////////

	// for evasion initiation test purposes - tests if you're already in the best position
   	// if you're considering evading towards actors of type safeActorType
   	bool wontFindAnywhereSafer() const;

	// to decide whether or not it's worth sending another voicemail moaning about having been hit
	bool sufficientTimePassedSinceLastHit();

	// inherited from MachActor
	virtual void dispatchSOS( Actors& strongThreats );

	void classify();
	void actUponClassification();

	///////////////////////////////
	//Subclass must override to modify the display.
   	//Called on change of state.
   	virtual void doVisualiseSelectionState();
//	static MachPhysMachine * createPhysicalMachine( Type );

	///////////////////////////////

    bool ignoringEntrancePolygon() const;
    bool ignoringConstructionPolygon() const;

	///////////////////////////////

	static MATHEX_SCALAR thresholdNVGDarkness();

	void doFadeInAnimation();
	void doFadeOutAnimation();

	void pFirstPersonMachineHandler( MachLog1stPersonMachineHandler* );
	MachLog1stPersonMachineHandler* pFirstPersonMachineHandler() const;
	// PRE( isIn1stPersonView() );


	// used when idle inside a building to make sure we're clear of the entrance so others can come in
	void checkAndDoClearEntrance( MachLogConstruction& constron );

	// Will play an appropriate voicemail if the machine is of the pc controller's race and certain other
	// criteria related to time since last hit mail was played etc. are satisfied.
	void checkAndDoHitVoiceMail();

	enum State
	{
		NOT_EXPLODING,
        EXPLODING,
		TELEPORTING_IN,
		TELEPORTING_OUT
	};
	PER_FRIEND_ENUM_PERSISTENT( State );

	///////////////////////////////

	// static const data

	static const PhysRelativeTime& hitVoiceMailInterval();
	static const PhysRelativeTime& militaryStrengthEstimateInterval();

	// these are used to try and cull the number of "I'm hit" voicemails coming from a particular area
	// on the map.
	static MexPoint2d& lastGlobalHitVoiceMailPosition();
	static PhysAbsoluteTime& nextGlobalHitVoiceMailTime();


	//////////////////////////////

    MachLogMachineImpl*     	pImpl_;

	friend ostream& operator <<( ostream&, const MachPhys::MachineType& );
	friend ostream& operator <<( ostream&, const State& );
};

PER_DECLARE_PERSISTENT( MachLogMachine );

/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef _LOG_MACHINE_HPP	*/

/* End MACHINE.HPP *************************************************/
