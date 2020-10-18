/*
 * C O N S T R O N . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachLogConstruction

    ABC for logical construction classes
*/

#ifndef _MACHLOG_CONSTRON_HPP
#define _MACHLOG_CONSTRON_HPP

#include "base/base.hpp"
//#include "ctl/vector.hpp"
#include "phys/phys.hpp"
//#include "machlog/buildpnt.hpp"
#include "machlog/machlog.hpp"
#include "machlog/actor.hpp"
#include "ftl/serialid.hpp"

//Forward declarations
class MachLogRace;
class MachPhysConstruction;
class MexAlignedBox2d;
class MachLogCamera;
class MachLogEntrance;
class W4dDomain;
class MachPhysMarker;
class MachLogConstructor;
class MachLogConstructionImpl;
class MexPoint2d;
class MachPhysConstructionData;
class MachPhysObjectData;
class PhysConfigSpace2d;

class MachLogBuildPoint;

template < class T > class ctl_list;
template < class T > class ctl_vector;


//Orthodox canonical (revoked)
class MachLogConstruction : public MachActor
{
public:

	enum CompleteWithFullHPs { FULL_HP_STRENGTH, HP_STRENGTH_UNALTERED };
	typedef ctl_list< MachLogMachine* >	Machines;
	typedef ctl_vector< PhysAbsoluteTime > Times;

	typedef ctl_pvector< MachLogBuildPoint >	BuildPoints;	
		
    //dtor.
    virtual~MachLogConstruction( void );

    //Inherited from MachLogCanBeDestroyed
    virtual PhysRelativeTime beDestroyed();

    //Inherited from MachActor
    virtual void doStartExplodingAnimation();
    virtual void doEndExplodingAnimation();

	//view of MachPhysObject data
	virtual const MachPhysObjectData& objectData() const;
	virtual const MachPhysConstructionData& constructionData() const = 0;

    //Ensure the physical model doesnt get deleted for the specified time.
    //This is done by adding a dying entity event.
    void preservePhysicalModel( const PhysRelativeTime& forTime );

    //Drop debris model corresponding to this object, starting at specified time.
    //(The time functionality isn't implemented yet - always drops at current time). 
    void dropDebris( const PhysAbsoluteTime& atTime );
    ////////////////////////////////////////
	size_t level() const;
	size_t level();
	
    //Number of entrances that appear externally
    size_t nEntrances() const;
	
	// the number of actual logical entrances (the factories, for example, have externally-visible
	// entrances, but they are all "fake doors", i.e. nLogicalEntrances will be zero)
	size_t nLogicalEntrances() const;

    //Global 2d coordinates of the access point outside this building to the ith
    //entrance
    MexPoint2d entranceExternalPoint( size_t i ) const;
    //PRE( i < nLogicalEntrances() )

    //Global 2d coordinates of the access point inside this building for the ith
    //entrance
    MexPoint2d entranceInternalPoint( size_t i ) const;
    //PRE( i < nLogicalEntrances() )

    //Open/close entrance i. Returns time animation will take to complete
	//Will always echo to the network
    PhysRelativeTime isEntranceOpen( size_t i, bool doOpen );
    //PRE( i < nLogicalEntrances() )

    //Open/close entrance i. Returns time animation will take to complete
    PhysRelativeTime isEntranceOpenNoEcho( size_t i, bool doOpen );
    //PRE( i < nLogicalEntrances() )

    //true if entrance i is open
    bool isEntranceOpen( size_t i ) const;
    //PRE( i < nLogicalEntrances() )

    MachLogEntrance&    entrance( size_t i ) const;
    //PRE( i < nLogicalEntrances() )
    
    //True if location is on the outside of the threshold line for specified entrance
    bool onOutsideSideOfThreshold( size_t entranceIndex, const MexPoint2d& location ) const;
    //PRE( entranceIndex < nEntrances() )
    
    ////////////////////////////////////////

    //Number of pickup points
    size_t nPickUpPoints() const;

    //Global 2d coordinates of the ith pickup point of this building
    MexPoint2d pickUpPoint( size_t i ) const;
    //PRE( i < nPickUpPoints() )

    //Number of putdown points
    size_t nPutDownPoints() const;

    //Global 2d coordinates of the ith putdown point of this building
    MexPoint2d putDownPoint( size_t i ) const;
    //PRE( i < nPutDownPoints() )
	
	bool bestAvailableBuildPoint( const MachLogConstructor& enquirer, MachLogBuildPoint** ppBestBuildPoint );

    //Aligned boundary in global coordinates defining perimeter of building
    MexAlignedBox2d globalBoundary() const;

    //Exports construction data in local coordinates
//    const MachLogConstructionData& constructionData() const;

    //Number of units currently applied to the construction
    const MachPhys::BuildingMaterialUnits& constructedUnits() const;

    //True iff the building is fully constructed
    bool isComplete() const;

    //Set the building as fully complete
    void makeComplete( CompleteWithFullHPs setHPsFullStrength = HP_STRENGTH_UNALTERED );	// if explicitly set true, bumps all HPs up to full whack.
															// Added with scenario.cpp in mind in particular.
	
	//Returns 100 if complete, 50 if half-complete etc.
	int percentageComplete() const;

    //Increment the constructed units count by addUnits
    void advanceConstructionState( MachPhys::BuildingMaterialUnits addUnits );
    //PRE( addUnits + constructedUnits() <= constructionData().cost() )

    //The id of the obstacle polygon - retypdefed to remove dependancy on phys config space
    typedef FtlSerialId PolygonId; //A registered polygon
    /*PhysConfigSpace2d::*/PolygonId obstaclePolygonId() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachLogConstruction& t );

	// inherited from MachActor
	virtual PhysRelativeTime update( const PhysRelativeTime& maxCPUTime, MATHEX_SCALAR );

    //  Move the given camera into the construction. The position of the camera will
    //  not change but its parent will change to the domain of the construction interior.
    void enterConstruction( MachLogCamera* pCamera );
    // PRE( physConstruction().hasInterior() );
    
    //  Move the given camera out of the construction. Its parent will revert to
    //  the parent it had before entering the construction.
    void leaveConstruction( MachLogCamera* pCamera );
    // PRE( physConstruction().hasInterior() );

	//return the physConstruction interior domain
	//Note that returns non-const reference quite deliberately.
	W4dDomain& interiorDomain( void ) const;

	//return the interior config space
	//Note that returns non-const reference quite deliberately.
	PhysConfigSpace2d& interiorConfigSpace() const;
    
	bool remove( MachLogMachine* );

	//These two functions tell the construction what constructors are building it.
	void addConstructor( MachLogConstructor* );
	void removeConstructor( MachLogConstructor* );
	size_t nConstructors() const;


	//this function can be called via the network from removeConstructor...it simply emulates
	//the functionality of removeConstructor when there are no consructors to remove.
	void networkRemove();

	//These two functions tell the construction what machines are in it.
	void machineEntered( MachLogMachine* );
	void machineExited( MachLogMachine* );
	const Machines& machines() const;
	bool isEnemyCanAttackInside( MachPhys::Race r ) const;	// attack-capable machine of enemy disposition to race r
	bool isEnemyMachineInside( MachPhys::Race r ) const;	// attack-capable machine of enemy disposition to race r
	bool isEmpty() const;	// no machines currently inside
	bool containsMachine( MachLogMachine* pMachine ) const;
	
	void needRebuild( bool );
	bool needRebuild() const;
	
	void addReservation();
	void cancelReservation();

	//actors can assign themselves to different races.
	//This function is virtual to allow different behaviour in constructions
	//and machines.
	virtual void assignToDifferentRace( MachLogRace& newRace );

	//Amount of damage is passed in beHit()
	virtual void beHit( const int&, MachPhys::WeaponType byType = MachPhys::N_WEAPON_TYPES,
						MachActor* pByActor = NULL, MexLine3d* pByDirection = NULL, 
						MachActor::EchoBeHit = MachActor::ECHO );
						
	virtual void beHitWithoutAnimation( int damage, PhysRelativeTime physicalTimeDelay = 0, MachActor* pByActor = NULL, EchoBeHit = ECHO );


	virtual void doOutputOperator( ostream& o ) const;						
						
	bool hasInterior() const;

    //The physical building
    MachPhysConstruction& physConstruction();
    const MachPhysConstruction& physConstruction() const;
	
	//mainly for network. go to a percentage
	bool updateCompletionVisualisation( double toPercentage );

	//do bits of stuff with constructions after creation
	void constructionCreated( MachPhysConstruction* );

	//initialise entrances etc from the phys construction data
	void initialiseFromConstructionData( MachPhysConstruction* );
	
	int constructionId() const;
	void constructionId( int );
	
	int originalRaceConstructionId() const;
	void originalRaceConstructionId( int );
	
	void usedPickupPoint( size_t whichPoint );
	bool pickupPointUsedRecently( size_t whichPoint );
	
	MachPhys::HitPointUnits hitPointValueOfBMUs( MachPhys::BuildingMaterialUnits BMUs );
	
	// both adds HPs and ensures physical structure's visible damage is updated
	void addRepairPoints( MachPhys::HitPointUnits hpsAdded );

	PER_MEMBER_PERSISTENT_ABSTRACT( MachLogConstruction );
	PER_FRIEND_READ_WRITE( MachLogConstruction );

	void displayAsRace( MachPhys::Race );
	
	// makes nextGlobalHitVoiceMailTime ( time now + hitVoiceMailInterval() )
	static void resetNextGlobalHitVoiceMailTime();

protected:
		
    //Construction is member of pRace, and physical model is pConstruction.
    //Dimensional data etc can be obtained from data, which must persist as long
    //as this building persists.
    MachLogConstruction( MachLogRace* pRace, MachPhysConstruction* pConstruction, MachLog::ObjectType,
                         const MachPhysConstructionData& data );
    MachLogConstruction( MachLogRace* pRace, MachPhysConstruction* pConstruction, MachLog::ObjectType, UtlId withId ,
                         const MachPhysConstructionData& data );
    //Updates the animation of construction for current state.
    //Needs to be called from the derived class update() method.
    //True when visualisation is up-to-date
    bool updateCompletionVisualisation();
	Machines&	machines();

private:
	friend class MachLogConstructionImpl;

	enum damageInterval{ VOICE_MAIL_HIT_INTERVAL = 15 };

    // Operations deliberately revoked
    MachLogConstruction( const MachLogConstruction& );
    MachLogConstruction& operator =( const MachLogConstruction& );
    bool operator ==( const MachLogConstruction& );

    //Useful constants
    enum
    {
        CONSTRUCTION_DELAY = 5 //Time taken to visualise added building material
    };

    //Converts 2d point coordinates local to building to global coordinates
    MexPoint2d globalPoint2d( const MexPoint2d& localPoint ) const;

    //Add an obstacle polygon to the config space for the building, which has
    //boundary boundary.
    void addObstaclePolygon( const MexAlignedBox2d& boundary );

    void setupInterior( const MachPhysConstructionData& data );
	
	// to decide whether or not it's worth sending another voicemail moaning about having been hit
	bool sufficientTimePassedSinceLastHit();
	
	// may need to issue voicemail when damage is taken
	void dealWithHitVoiceMail();
	
	static PhysRelativeTime activePadInterval();

    void    setupInteriorConfigSpace();
    // PRE( physConstruction().hasInterior() );
	
	// removes the buildpoint which would otherwise block the entrance (if any )
	void removeDoorPoint();
	
	// true if a behemoth has reserved either of the two closest build points to this one
	bool behemothsRestrict( const MachLogBuildPoint& candidatePoint );
	
	// these are used to try and cull the number of "I'm hit" voicemails coming from a particular area 
	// on the map.
	static MexPoint2d& lastGlobalHitVoiceMailPosition();
	static PhysAbsoluteTime& nextGlobalHitVoiceMailTime();

	//Inherited from MachActor
	//Subclass must override to modify the display.
	//Called on change of state.
	virtual void doVisualiseSelectionState();
    //Data members
    typedef ctl_vector< MachLogEntrance* >  Entrances;
	typedef ctl_list< MachLogConstructor* >	Constructors;

	MachLogConstructionImpl* pImpl_;
};

PER_DECLARE_PERSISTENT( MachLogConstruction );
PER_ENUM_PERSISTENT( MachLogConstruction::CompleteWithFullHPs );
#ifdef _INLINE
    #include "machlog/constron.ipp"
#endif


#endif

/* End CONSTRON.HPP *************************************************/
