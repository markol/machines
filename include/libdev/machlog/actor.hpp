/*
 * A C T O R . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

#ifndef _MACH_ACTOR_HPP
#define _MACH_ACTOR_HPP

#include "utility/id.hpp"
#include "sim/actor.hpp"
#include "machphys/machphys.hpp"
#include "machlog/machlog.hpp"
#include "machlog/destroy.hpp"
#include <iostream>

/* //////////////////////////////////////////////////////////////// */

// forward refs
//class ostream;
class UtlBoundedIdGenerator;
class W4dEntity;
class SimProcess;
class MachLogMachine;
class MachPhysMachine;
class MachLogOperation;
class MachLogStrategy;
class MachPhysObjectData;

class MachLogAdministrator;
class MachLogAggressor;
class MachLogConstructor;
class MachLogGeoLocator;
class MachLogSpyLocator;
class MachLogTechnician;
class MachLogResourceCarrier;
class MachLogAPC;
class MachLogMobile;
class MachLogCanAttack;
class MachLogConstruction;
class MachLogMine;
class MachLogSmelter;
class MachLogFactory;
class MachLogGarrison;
class MachLogHardwareLab;
class MachLogSoftwareLab;
class MachLogPod;
class MachLogBeacon;
class MachLogMissileEmplacement;
class MachLogRace;
class MexLine3d;
class MachLogDebris;
class MachLogOreHolograph;
class MachLogLandMine;
class MachLogArtefact;
class MachLogSquadron;
class MachActorImpl;
class MachLogMachine;

template < class X > class ctl_pvector;

class MachActor
: public SimActor,
  public MachLogCanBeDestroyed
// cannonical form revoked
{
public:

	typedef ctl_vector< UtlId > Actors;
	typedef ctl_pvector< MachLogMachine > Machines;

	MachActor( 	MachLogRace * pRace, W4dEntity *, MachLog::ObjectType );
	MachActor( 	MachLogRace * pRace, W4dEntity *, MachLog::ObjectType, UtlId withId );

	virtual ~MachActor();

	MachPhys::Race race() const;

	const MachLogRace& logRace() const;
	const MachLogRace& logOriginalRace() const;

	///////////////////////////////

	// inherited from SimActor...
	virtual PhysRelativeTime update( const PhysRelativeTime& maxCPUTime, MATHEX_SCALAR clearanceFromDisplayedVolume );

    // Must be overriden to return the W4dDomain
    // in which the actor currently resides
	// virtual const W4dDomain& renDomain() const = 0;

	///////////////////////////////

	//Amount of damage is passed in beHit()
	//echo is for network situations.
	enum EchoBeHit { ECHO, NO_ECHO };
	virtual void beHit( const int&, MachPhys::WeaponType byType = MachPhys::N_WEAPON_TYPES,
						MachActor* pByActor = NULL, MexLine3d* pByDirection = NULL, EchoBeHit = ECHO );

	virtual void beHitWithoutAnimation( int damage, PhysRelativeTime physicalTimeDelay = 0, MachActor* pByActor = NULL, EchoBeHit = ECHO );
	// virtual PhysRelativeTime beDestroyed() = 0;
	// virtual bool exists() const = 0;
	// const MexTransform3d& globalTransform() const = 0;

	//return object data view of actor
	virtual const MachPhysObjectData& objectData() const = 0;

	///////////////////////////////

	bool isIdle() const;

	PhysRelativeTime startExplodingAnimation();
	void endExplodingAnimation();

	///////////////////////////////

	void newOperation( MachLogOperation * pNewOperation );
//	void subOperation( MachOperation * pNewOperation );

    //Set/get the selection state
    void selectionState( MachLog::SelectionState nextState );
    MachLog::SelectionState selectionState() const;
	///////////////////////////////

	bool isIn1stPersonView() const;
	void isIn1stPersonView( bool isIn1st );

	MachLog::ObjectType	objectType() const;

	MachPhys::ArmourUnits armour( void ) const;
	MachPhys::HitPointUnits hp( void ) const;
	MachPhys::HitPointUnits maximumhp( void ) const;
	MATHEX_SCALAR hpRatio( void ) const;


	///////////////////////////////
	friend std::ostream& operator<<( std::ostream&, const MachActor& );

	//Sets the self destruct mechanism. This in Actor rather than machine as buildings (may)
	//need it as well. (When the pod goes everything goes).
	void selfDestruct( const PhysRelativeTime& );
	bool selfDestruct() const;
	PhysRelativeTime selfDestructTime() const;

	//actors can assign themselves to different races.
	//This function is virtual to allow different behaviour in constructions
	//and machines.
	virtual void assignToDifferentRace( MachLogRace& newRace );

	//This set of functions return a reference to objects via a static_cast
	//The functions all exist here so that high level rules may be performed
	//before the cast is performed to check the validity.

	MachLogAdministrator& asAdministrator();
	const MachLogAdministrator& asAdministrator() const;
	MachLogAggressor& asAggressor();
	const MachLogAggressor& asAggressor() const;
	MachLogConstructor& asConstructor();
	const MachLogConstructor& asConstructor() const;
	MachLogGeoLocator& asGeoLocator();
	const MachLogGeoLocator& asGeoLocator() const;
	MachLogSpyLocator& asSpyLocator();
	const MachLogSpyLocator& asSpyLocator() const;
	MachLogTechnician& asTechnician();
	const MachLogTechnician& asTechnician() const;
	MachLogResourceCarrier& asResourceCarrier();
	const MachLogResourceCarrier& asResourceCarrier() const;
	MachLogAPC& asAPC();
	const MachLogAPC& asAPC() const;
	MachLogMobile& asMobile();
	const MachLogMobile& asMobile() const;
	MachLogCanAttack& asCanAttack();
	const MachLogCanAttack& asCanAttack() const;
	MachLogMachine& asMachine();
	const MachLogMachine& asMachine() const;
	MachLogConstruction& asConstruction();
	const MachLogConstruction& asConstruction() const;
	MachLogMine& asMine();
	const MachLogMine& asMine() const;
	MachLogSmelter& asSmelter();
	const MachLogSmelter& asSmelter() const;
	MachLogGarrison& asGarrison();
	const MachLogGarrison& asGarrison() const;
	MachLogFactory& asFactory();
	const MachLogFactory& asFactory() const;
	MachLogHardwareLab& asHardwareLab();
	const MachLogHardwareLab& asHardwareLab() const;
	MachLogSoftwareLab& asSoftwareLab();
	const MachLogSoftwareLab& asSoftwareLab() const;
	MachLogMissileEmplacement& asMissileEmplacement();
	const MachLogMissileEmplacement& asMissileEmplacement() const;
	MachLogPod& asPod();
	const MachLogPod& asPod() const;
	MachLogBeacon& asBeacon();
	const MachLogBeacon& asBeacon() const;
	MachLogDebris& asDebris();
	const MachLogDebris& asDebris() const;
	MachLogOreHolograph& asOreHolograph();
	const MachLogOreHolograph& asOreHolograph() const;
	MachLogLandMine& asLandMine();
	const MachLogLandMine& asLandMine() const;
	MachLogArtefact& asArtefact();
	const MachLogArtefact& asArtefact() const;
	MachLogSquadron& asSquadron();
	const MachLogSquadron& asSquadron() const;
	bool objectIsMachine() const;
	bool objectIsCanAttack() const;
	bool objectIsConstruction() const;
	bool objectIsILF() const;
	bool objectIsDebris() const;
	bool objectIsOreHolograph() const;
	bool objectIsLandMine() const;
	bool objectIsArtefact() const;
	bool objectIsMissileEmplacement() const;
	bool objectIsCanSmelt() const;

	const MachLogStrategy& strategy() const;
	MachLogStrategy& strategy();

	//Half the actor types have a sub type...the ones that don't will return 0
	int	subType() const;

	MachLog::SelectableType	selectableType() const;

	void addHitPoints( MachPhys::HitPointUnits );
	const PhysAbsoluteTime& changeRaceStartTime() const;
	//this is only ever called if you wish to override the permanancy of the change race - certain actions can do this.
	void changeRaceStartTime( const PhysAbsoluteTime& );
	ulong lastBeHitFrame() const;

	void setHPAndArmour( MachPhys::HitPointUnits, MachPhys::ArmourUnits );

	//a prediction function used for making sure network lag does not impact on the game too much
	bool willHitCauseDeath( int );

	void addThreat( UtlId );
	void removeThreat( UtlId );
	bool hasThisActorAsAThreat( UtlId ) const;
	bool hasThreats() const;	// do I have at least 1 enemy actor marked as threatening me?

   	int militaryValueOfIncomingThreats() const;
   	virtual int militaryStrength() const;
   	int militaryValueFromHere( MachActor& otherActor ) const;
   	virtual int localStrength() const;
   	int localStrengthFromPosition( const MexPoint3d& pos ) const;
   	bool thisActorIsStrongerThanMe( const MachActor& otherActor ) const;
   	void updateLocalStrengthEstimate();

   	// version returns true if military value of incoming threats is greater than my own military strength
	bool isOutGunned() const;

	//these two methods are needed so that the strategy data member of an actor may be persisted in/out after
	//the most derived class has been fully created. This allows for manipulation of data items in more derived
	//classes from the operations on the strategy when they are persisted.
	void persistenceWriteStrategy( PerOstream& ) const;
	void persistenceReadStrategy( PerIstream& );

	PER_MEMBER_PERSISTENT_ABSTRACT( MachActor );
	PER_FRIEND_READ_WRITE( MachActor );

	bool isInSpecialUpdateActorsList() const;
	void isInSpecialUpdateActorsList( bool );

	const PhysRelativeTime&	maximumTimeInSpecialUpdateActors() const;
	void maximumTimeInSpecialUpdateActors( const PhysRelativeTime& );
	const PhysAbsoluteTime&	startTimeInSpecialUpdateActors() const;
	void startTimeInSpecialUpdateActors( const PhysAbsoluteTime& );

	// returns the position of the predicted position of the machine in t seconds' time
	// given its current velocity vector (and compensating for change in terrain height)
	const MexPoint3d& predictedTargetOffset( PhysRelativeTime t ) ;

	// set all local chums onto full alert
	void dispatchCaveeCall();

	void setBusyPeriod( PhysRelativeTime busyPeriod );

	bool busy() const;

	//Override the display colour for the map and icons here
	//(of course ML knows nothing about the map and icons :) )
	void displayMapAndIconRace( MachPhys::Race );
	MachPhys::Race	displayMapAndIconRace() const;

protected:

	MachLogRace& logRace();
	MachLogRace& logOriginalRace();

	virtual void checkAndDoOnDefCon( const Actors& strongThreats );

	virtual void doStartExplodingAnimation() = 0;
	virtual void doEndExplodingAnimation() = 0;

	///////////////////////////////

	friend class MachLogOperation;

	virtual void doOutputOperator( ostream& ) const;

	void objectType( const MachLog::ObjectType& );
	//set methods are protected
	void armour( const MachPhys::ArmourUnits& );
	void hp( const MachPhys::HitPointUnits& );

	int localStrengthValue() const;
	int localStrengthMaximumRadius() const;

	void actorCreated();

	void lastBeHitFrame( ulong newVal );

	Actors&							actorsThreateningMe();

private:
	friend class MachOperation;

	MachActor( const MachActor& );
	MachActor& operator =( const MachActor& );
	bool operator ==( const MachActor& ) const;
   	//Subclass must override to modify the display.
   	//Called on change of state.
   	virtual void doVisualiseSelectionState() = 0;

	void dealWithThreats();
	void dealWithFirstPersonThreat( MachActor* pFirstPersonThreatActor );

	// populates an Actors vector pointed to by pActors. It is only intended to populate blank vectors.
	// PRE( pActors->size() == 0)
	virtual void populateStrongThreats( Actors* pActors );

	MachActorImpl*					pImpl_;

	// time which current time must be greater for in order for SOS to be dispatched
	PhysAbsoluteTime nextSOSOpportunity() const;

	// caches basic centroid offset to prevent the need for wasteful future recalculation
	void setupBasicTargetOffset();
	const MexPoint3d& basicTargetOffset() const;

	// common functionality for both beHit and beHitWithoutAnimation when the actor has survived the damage
	void doHitButStillAliveStuff( MachActor* pByActor );

	void updateArmourAndHps( int damageTaken );

	// if this is a self-destructing eradicator, final death causes a vortex to be created at the machine's
	// current position. if ICBM then set off nuke etc etc
	void checkAndDoSpecialDeaths();

	// !! Use of 2nd protected section is a necessary horror caused by need for derived class method
	// to now call base class method - was previously private, and we can't change the v-table order AND
	// cheat, so........(apologies...it *is* a bit grubby......should really be reorganised if ever this
	// header must undergo a non-cheatable change).
protected:

	// attempts to muster nearby troops to help dispose of the most dangerous of the strongThreats
	// stops recruiting when sufficient firepower has been deemed to be mustered
	virtual void dispatchSOS( Actors& strongThreats );

	//Goes through the actorsThreateningMe list and removes any actors not valid.
	//(protected as called from derived classes).
	void validateActorsThreateningMe();
};

PER_DECLARE_PERSISTENT( MachActor );

/* //////////////////////////////////////////////////////////////// */

#endif	/*	#ifndef _MACH_STRATEGY_HPP	*/

/* End STRATEGY.HPP *************************************************/
