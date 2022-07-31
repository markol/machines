/*
 * C A N A T T A C . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */
 
/*
 * Mixin for logical objects that support attacking
 * operations.
 */

#ifndef _MACHLOG_CANATTAC_HPP
#define _MACHLOG_CANATTAC_HPP

#include "utility/id.hpp"
#include "phys/phys.hpp"

#include "world4d/observer.hpp"
#include "machphys/machphys.hpp"

// forward refs
//class MachLogAggressor;
//class MachLogAdministrator;
class MexRadians;
class MexLine3d;
class MachPhysCanAttack;
class MachPhysWeaponData;
class MachLogMachine;
class MachLogMissileEmplacement;
class MachLogPod;
class MachActor;
class MachLogWeapon;
class MachLogFireData;
class MachLogCanAttackImpl;

template < class T > class ctl_pvector;
template < class T > class ctl_vector;


class MachLogCanAttack : public W4dObserver
{
public:
	typedef ctl_pvector< MachLogWeapon >	Weapons;
	typedef ctl_vector< UtlId > Actors;		

	virtual ~MachLogCanAttack();

    //Play attacking/firing animation with nTimes repetitions.
    //return time it will take.
	virtual PhysRelativeTime attack( MachActor* pTarget );
	virtual PhysRelativeTime attackWithPunch( MachActor* pTarget );
    //PRE( canPunch() );
	    
	PhysRelativeTime attackWithTreachery( MachActor* pTarget );
	PhysRelativeTime heal( MachActor* pTarget );

	bool hasTreacheryWeapon() const;
	bool hasHealingWeapon() const;
	bool hasPunchWeapon() const;
	void stopAllHealing( const MachActor& me );

	const MachActor& currentTarget() const;
	MachActor& currentTarget();
	bool hasCurrentTarget() const;
	void currentTarget( MachActor* );
	
	bool currentTargetIsOneOfThese( const Actors& strongThreats );

	virtual bool beNotified( W4dSubject* pSubject,
	                         W4dSubject::NotificationEvent event, int clientData = 0 );

    virtual void domainDeleted( W4dDomain* pDomain );

	//weapons collections is available non-const to allow the persistence mechanism to get its hands on the weapons
	//so that the physical weapons can be reassigned (not recreated).
	const Weapons&	weapons() const;
	Weapons&	weapons();

	void addWeapon( MachLogWeapon* );
	const MATHEX_SCALAR getMaximumWeaponRange() const;
	const MATHEX_SCALAR getMaximumHealWeaponRange() const;
	const MATHEX_SCALAR getMaximumPunchWeaponRange() const;
	const MATHEX_SCALAR getMaximumTreacheryWeaponRange() const;

	void doOutputOperator( ostream& o ) const;
	MachPhys::WeaponCombo weaponCombo() const;

	bool canAttack( const MachActor& other ) const;
	bool canTurnToAttack( const MachActor& other ) const;
	
	bool canPunch( const MachActor& other ) const;

    //true if relevant weapon of me is in range of other.
	enum SpecialWeaponRange { NONE, ORB_OF_TREACHERY, SUPERCHARGE, GORILLA_PUNCH };
	bool inWeaponRange( const MachActor& other, SpecialWeaponRange ) const;
	
	// simply returns true if sqrDistance represents a scalar value less than the squared maximum weapon
	// range of this canAttack actor
	bool withinSqrMaximumWeaponRange( MATHEX_SCALAR sqrDistance ) const;

    //true if angle about z axis ok for firing at target.
    //If result rue, pInVerticalAngleRange returns true iff angle about y axis ok for firing
    //The minimum deviation angles for all the mounted weapons is used.
    bool inAngleRange( const MachActor& target, bool* pInVerticalAngleRange ) const;

    //Used to indicate positioning of a weapon relative to a target
    enum WeaponDisposition
    { 
        NOT_IN_DISTANCE_RANGE, NOT_IN_HORIZONTAL_ANGLE_RANGE, NOT_IN_VERTICAL_ANGLE_RANGE,
        BEHIND_COVER, BEHIND_FRIENDLY_COVER,
        IN_RANGE
    };

    //Does in range testing for object target with respect to a weapon at global position attackFrame.
    //The associated weapon data object is supplied.
    //A multiplier for the weapon range based on the relative heights of the attacker and target is
    //passed in heightModifier.
    //If the target is IN_RANGE, pAimPoint returns a global point which is in the target and is within the
    //angle deviation constraints of the weapon.
    static WeaponDisposition rangeData
    (
        const MexTransform3d& attackFrame, const W4dEntity& target, const MachPhysWeaponData& weaponData, 
        MATHEX_SCALAR heightModifier, MexPoint3d* pAimPoint
    );

    //The disposition with respect to cover, given a projectile path from this attacker's weapon
    //to its current target. If precision is SAME_RACE_OK, then machines of enemy race will be ignored.
	enum SoftCoverAccuracy { TARGET_ONLY_ALLOWED, SAME_RACE_OK };
	
    WeaponDisposition behindCover( const MexLine3d& projectilePath, SoftCoverAccuracy precision ) const;
	//PRE( hasCurrentTarget() );
	
	// for use when testing cover on an actor we haven't necessarily set as current target
	static WeaponDisposition testActorBehindCover( MachActor* pSubjectActor, MachActor* pTestActor, const MexLine3d& projectilePath, SoftCoverAccuracy precision );	

    //true if this attacker can track targets by turning its head
    bool canTurnHead() const;

    //true if the attacker's first weapon is at its tilt limits
    bool atTiltLimits() const;

	bool targetBehindSoftCover( MachActor* pTarget, SoftCoverAccuracy precision = TARGET_ONLY_ALLOWED );
	bool canTurnToAttackAndNoCover( MachActor* pTargetActor );
	
	static bool IHaveClearWeaponPathToHim( MachActor* pSubjectActor, MachActor* pTargetActor );

	// non-static version of the above; calls into the above with pMe_ automatically used as the first argument
	bool IHaveClearWeaponPathToHim( MachActor* pTargetActor ) const;

	ulong lastFireFrame() const;
	
	bool canDetect( const MachActor& other );

   //Used by persistence mechanism since the physical object is not persisted.
   //Don't use for anything else.
   void setPhysCanAttack( MachPhysCanAttack* pPhysCanAttack );
   
   //The physical attack object
   MachPhysCanAttack& physCanAttack();
   const MachPhysCanAttack& physCanAttack() const;
	
	void resetInaccuracyToDefault();
	
	void improveAim();
	
	MATHEX_SCALAR inaccuracy() const;
	MATHEX_SCALAR minimumInaccuracy() const;
	
	// for gaining experience etc.
	void decreaseMinimumInaccuracy( MATHEX_SCALAR amountToDecreaseBy );
	
	// for being damaged etc.
	void increaseCurrentInaccuracy( MATHEX_SCALAR amountToIncreaseBy );
	
	// for being repaired etc.
	void decreaseCurrentInaccuracy( MATHEX_SCALAR amountToDecreaseBy );

	void update();
	
	MachLogFireData createFireData();	// returns new MachLogFireData struct to use

	bool currentlyAttached() const;
	
	// will not change alertness level if minimumNewLevel is less than current value
	void setMinimumAlertness( int minimumNewLevel );
	
	// do not initiate checkAndAttackCloserTarget for HowLong seconds after this call is made
	void suppressAttackingUrges( PhysRelativeTime forHowLong = 7.0 );
	
	// immediately cancel this (used when hit by enemy fire etc.)
	void cancelAnyAttackingUrgeSuppression();
	
	// checks restrictions for being able to attack ground or air machines etc.
	bool canFireAt( const MachActor& potentialTarget ) const;
	
	// helper function mainly for SOS calls.......
	// pc race machines inside a building don't respond to SOS calls targetting enemies outside that building
	// otherwise, returns result of canFireAt.
	bool recruitableToAttack( const MachActor& potentialTarget ) const;
	
	// level of alertness below which the actor can never drop
	int minimumAlertnessForActor() const;
	
	// whether or not we'll do a CAACT on the actor we've just been shot by. This is dependent upon
	// whether or not we already have a target, whether sufficent time has elapsed since we last did one of these etc.
	bool willTestHitByCandidate( const MachActor& hitByActor ) const;
	
	// returns true if this actor is capable of launching weaponry at me in a conventional combat situation 
	// NB excludes ICBMs and pods with ion cannons
	static bool actorIsDirectThreat( const MachActor& candidateActor );
	
	// chance decreases the more wounded a machine is (will always respond if above 60% hps)
	bool willingToRespondToSOS() const;
	
	// note that this is NOT a SQUARE distance
	MATHEX_SCALAR distanceBeyondMaximumWeaponRange( const MachActor& targetActor ) const;
	
	PER_MEMBER_PERSISTENT_VIRTUAL( MachLogCanAttack );
	PER_FRIEND_READ_WRITE( MachLogCanAttack );

protected:

	MachLogCanAttack( MachActor* pMe, MachPhysCanAttack* pPhysCanAttack, MachPhys::WeaponCombo );
	//for mobiles
	void checkAndAttackCloserTarget( MachLogMachine*, MachActor* pFiredAtMe = NULL );
	void checkAndAttackCloserTarget( MachLogMissileEmplacement* pActor, MachActor* pFiredAtMe = NULL );
	void checkAndAttackCloserTarget( MachLogPod* pActor, MachActor* pFiredAtMe = NULL );
//	void checkAndAttackCloserTarget( MachLogAdministrator* );
//	bool targetBehindSoftCover( MachLogMachine* pActor, MachActor* );


    bool inAngleRange( const W4dEntity& entity ) const;
	static MexRadians angleRange();
	static MATHEX_SCALAR sinAngleRange();
	
	// returns true if allowedToCheckForNewTarget(), combined with stochastic element depending on
	// current state of alertness
	bool willCheckForTargets() const;
	
	// will return false if within time specified for suppressing attacking urges
	bool allowedToCheckForNewTarget() const;
	void diminishAlertnessAndInaccuracy();	
	int alertness() const;
			
private:
	void dt( const char* text, bool start );
	
	void aimSpoiled();
	
	bool acceptableAggressiveTargetForHigherDefconsMachine( MachActor* pTarget, MATHEX_SCALAR sqrDistanceToTarget ) const;	
	
	// note that *pAlternativeActor may actually be changed by this operation under certain circumstances
	bool switchFromExistingTargetToThisOne( MachActor* pAlternativeActor, MATHEX_SCALAR sqrDistanceToCurrentTarget, MachActor* pFiredAtMe );
	// PRE( pMe_->objectIsMachine() );
	
	MachLogCanAttackImpl*	pImpl_;	
	static const MATHEX_SCALAR improveAimInterval();	// interval from last firing time within which aim is improved from previous shot
	
	// distance beyond which a target is considered too far to engage from a CAACT
	static MATHEX_SCALAR sqrMaximumEngagementDistance();
	
};

ostream& operator <<( ostream& o, MachLogCanAttack::WeaponDisposition disp );

PER_DECLARE_PERSISTENT( MachLogCanAttack );
#endif