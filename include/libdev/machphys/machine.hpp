/*
 * M A C H I N E . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysMachine

    Implement the physical side of a machine.

    This class has a fat interface, derived classes will choose
    which of the machine's actions they wish to implement.
*/

#ifndef _MACHINE_HPP
#define _MACHINE_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "mathex/mathex.hpp"
#include "phys/phys.hpp"
#include "world4d/composit.hpp"
#include "machphys/machphys.hpp"
#include "machphys/move.hpp"
#include "machphys/turn.hpp"
#include "machphys/destroy.hpp"
#include "machphys/snddata.hpp"
#include "machphys/plansurf.hpp"

class MexPoint3d;
class MexLine3d;
class MexRadians;
class MexTransform3d;
class W4dCompositePlan;
class W4dLink;
class MachPhysLocomotionMethod;
class MachPhysMachineExplosion;
class MachPhysMachineExplosionData;
class MachPhysCanAttack;
class MachPhysFireball;
class MachPhysMachineData;
class MachPhysMachineBurning;
class MachPhysMachineDamageData;
class MachPhysPlanetSurface;
class MachPhysTurnerTracker;
class MachPhysMachineImpl;
class MachPhysFacePlate;
class MachPhysConstruction;

//template< class T > class ctl_vector< T >;
template< class T > class CtlCountedPtr;
typedef CtlCountedPtr< W4dCompositePlan > W4dCompositePlanPtr;

class MachPhysMachine
:   public W4dComposite,
    public MachPhysCanMove,
    public MachPhysCanTurn,
    public MachPhysCanBeDestroyed
{
public:
    virtual ~MachPhysMachine();

    //The machine's race
    MachPhys::Race race() const;

    typedef MachPhysCanMove::ProfilePtr ProfilePtr;

    ProfilePtr profile(
        const MexTransform3d& fromGlobalTransform,
        OrientationInfo orientationInfo, const MexPoint3d& destination,
        const MachPhysPlanetSurface& planetSurface ) const;

    //Compute move data including transforms, motion profile data etc for
    //a move from fromGlobalPosition to toPoint, starting at startTime.
    //Initial speed is startSpeed.
    //The orientatinInfo tells us whether we should use the orientation
    //given in fromGlobalPosition or whether we should calculate the starting
    //orientation ourselves
    //If comeToRest is true, must come to rest at toPoint. Otherwise, should
    //be travelling with translation speed.
    //  (From MachPhysCanMove)

    virtual std::auto_ptr< MachPhysMachineMoveInfo > moveInfo(
        ProfilePtr profilePtr,
        const PhysAbsoluteTime& startTime, MATHEX_SCALAR startSpeed,
        MATHEX_SCALAR capSpeed, MachPhys::FinalState finalState );

    //Apply move animations to make the move as defined by info
    virtual void move( const MachPhysMachineMoveInfo& info );

    //  Stop the move dead. Apply appropriate animations.
    virtual void stopDead();

    //Initiate turning through angle radians.
    //returns time taken to complete the turn.
    //(From MachPhysCanTurn)
    virtual PhysRelativeTime turn( const MexRadians& angle );

    //True iff the machine has the capability to turn its upper body while leaving base stationary
    bool canTurnUpperBody() const;

    //Make the upper body keep turning so as to face targetObject
    void upperBodyTrackTarget( const W4dEntity& targetObject );
    //PRE( canTurnUpperBody() );

    //the current tracking target
    const W4dEntity& upperBodyTrackingTarget() const;
    //PRE( upperBodyIsTrackingTarget() );

    //True if tracking a target
    bool upperBodyIsTrackingTarget() const;

    //Stop tracking a target if doing so.
    void upperBodyStopTrackingTarget();

    //Make the upper body rotate by angle from current position, returning time for the animation
    PhysRelativeTime upperBodyTurnBy( MexRadians angle );
    //PRE( canTurnUpperBody() )
    //PRE( not upperBodyIsTrackingTarget() )

    //Make the upper body rotate to angle, returning time for the animation
    PhysRelativeTime upperBodyTurnTo( MexRadians angle );
    //PRE( canTurnUpperBody() )
    //PRE( not upperBodyIsTrackingTarget() )

    //Make the upper body jump to specified angle from specified time
    void upperBodySnapTo( MexRadians angle, const PhysAbsoluteTime& atTime );
    //PRE( canTurnUpperBody() )
    //PRE( not upperBodyIsTrackingTarget() )

    //The current angle between upper body and base
    MexRadians upperBodyCurrentAngle() const;

    //The current angle trying to move to
    MexRadians upperBodyTargetAngle() const;
    //PRE( not upperBodyIsTrackingTarget() )

    //  Destroy yourself
    //  Return the time that will be taken for the animation
    virtual PhysRelativeTime beDestroyed();

    //Do a shaking animation appropriate for eg being hit by a projectile.
    PhysRelativeTime shake( const PhysAbsoluteTime startTime );

    //Set the machine burning from strtTime for duration, with percentage
    //indicating the degree of burning
    void burn( const PhysAbsoluteTime& startTime, const PhysRelativeTime& duration,
               MATHEX_SCALAR percentage );
    //PRE( percentage > 0.0 );
    //PRE( percentage <= 100.0 );

	bool hasDamageData() const;
	MachPhysMachineDamageData& damageData();

	MachPhysMachineBurning& machineBurning( MachPhysMachine* pMachine );
	bool isBurning() const;
	bool isBurningFinished() const;

	void damageLevel( const double& percent );
	const double& damageLevel() const;
	void updateDamageLevel(); //to be called every 60 seconds
	bool isDamaged() const;

	//Change machine's race to newRace
	void changeRace( MachPhys::Race newRace );

    //chnage the colour of the machine to that of newRace's
	void changeColour( MachPhys::Race newRace );

	//virtual method returns the correct data element from the arrays held within MachPhysData
	virtual const MachPhysMachineData& machineData() const = 0;

    MATHEX_SCALAR   maxTranslationSpeed() const;
    MexRadians      maxRotationSpeed() const;
    MATHEX_SCALAR   maxTranslationAcceleration() const;
    MexRadians      maxRotationAcceleration() const;

    //  Attach the given fireball to the given link
    void attachFireball( MachPhysFireball* pFireball, W4dLink* pLink, const MexTransform3d& localTransform );

	void convertMaterials( const MachPhysMachineData& data, MachPhys::Race race );

	bool hasFacePlate() const;
	W4dLink& facePlate();
	//PRE(hasFacePlate());

	//Get the moving and idle sounds for this machine
	void getLocomotionSounds(SoundId* pIdle, SoundId* pMove, const MachPhysMachineData& data) const;

    //Inherited from W4dEntity
    virtual bool intersectsLine( const MexLine3d& line, MATHEX_SCALAR* pDistance,
                             Accuracy accuracy ) const;

    MachPhys::LocomotionType  locomotionType() const;

    //The object which controls various aspects of its locomotion type
    const MachPhysLocomotionMethod& locomotionMethod() const;
    MachPhysLocomotionMethod& locomotionMethod();

	bool hasCanAttack() const;
	//Return the associated MachPhysCanAttack
	MachPhysCanAttack* canAttack() const;
	//PRE(hasCanAttack());
	//POST(pThisAttack);

    //  In order for machines to behave correctly when in, entering
    //  or leaving a construction these calls must be made correctly.
    void inOrOnPadConstruction( MachPhysConstruction* pPhysConstruction );
    void notInOrOnPadConstruction();

    // Return any floors which are currently affecting the movement of
    // this machine. When a machine enters a building a floor is defined
    // at the height of the building's floor. When the machine is over
    // this floor, the z coordinate of the machine is calculated from
    // the floor rather than from the underlying terrain.
    const MachPhysPlanetSurface::Floors& floors() const;

	enum ControlType
	{
		FIRST_PERSON_LOCAL,
		FIRST_PERSON_REMOTE,
		OTHER
	};

	//Get and set machine control types
	ControlType getMachineControlType() const;
	void setMachineControlType(ControlType);

	void doFadeInAnimation();
	void doFadeOutAnimation();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysMachine& t );

    enum { N_BODY_LEVELS = 10 };
    enum { N_BRAIN_LEVELS = 10 };

    PER_MEMBER_PERSISTENT_ABSTRACT( MachPhysMachine );
    PER_FRIEND_READ_WRITE( MachPhysMachine );

protected:
    MachPhysMachine(
      W4dEntity* pParent,
      const W4dTransform3d& localTransform,
      const SysPathName& compositeFileName,
      const MachPhysMachineData& data );

    // Copy a machine. Only the machine will be copied, nothing it is holding will be copied.
    MachPhysMachine(
      const MachPhysMachine& copyMe,
      W4dEntity* pParent,
      const W4dTransform3d& localTransform,
	  size_t bodyLevel,
      size_t brainLevel,
      MachPhys::Race race,
      const MachPhysMachineData& data );

    std::auto_ptr< MachPhysMachineMoveInfo > doMoveInfo
    (
        ProfilePtr profilePtr,
        const PhysAbsoluteTime& startTime, MATHEX_SCALAR startSpeed,
        MATHEX_SCALAR capSpeed, MachPhys::FinalState finalState
    ) const;

    void doMove( const MachPhysMachineMoveInfo& info );

    PhysRelativeTime doTurn( MATHEX_SCALAR angle );
    PhysRelativeTime doExplode();

    PhysRelativeTime    explodeLinks();
    void    fireballEffect( PhysAbsoluteTime time );

    void    maxTranslationSpeed( MATHEX_SCALAR newSpeed );
//     void            maxRotationSpeed( MATHEX_SCALAR newSpeed );
//
//     void            maxTranslationAcceleration( MATHEX_SCALAR newAcceleration );
//     void            maxRotationAcceleration( MATHEX_SCALAR newAcceleration );

    const   MachPhysMachineExplosionData& explosionData() const;
    MachPhysMachineExplosionData& explosionDataForEdit();

	size_t	bodyLevel() const;
	size_t  brainLevel() const;

private:
    // Operation deliberately revoked
    MachPhysMachine( const MachPhysMachine& );

    // Operation deliberately revoked
    MachPhysMachine& operator =( const MachPhysMachine& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysMachine& );

    //Create the locomotion method
    void createLocomotion( const MachPhysMachineData& data );

    void createHoverBootLocomotion( MATHEX_SCALAR bobHeight );
    void createWheelLocomotion( MATHEX_SCALAR wheelRadius );
    void createSpiderLocomotion();
    void createGliderLocomotion( MATHEX_SCALAR height );
    void createTracksLocomotion(MATHEX_SCALAR repeatsPerMeter);

    void createRandomTransform(
      const MexPoint3d& explosionPosition,
      const MexTransform3d& objectTransform,
      MexTransform3d* pResult );

    static  MachPhysFacePlate&  faceplateFactory( size_t brainLevel );

    void defaultExplosionData();

    //Set up the shakeLinks_ data
    void initialiseShakeLinks();

    //Add the locomotion sound for the machine
    void addLocomotionSound( const MachPhysMachineData& data );

    typedef CtlCountedPtr< ctl_vector< MexTransform3d > > TransformsPtr;

    void smoothProfile(
        const ctl_vector< MexTransform3d >& inputTransforms,
        const ctl_vector< MexTransform3d >& postMoveTransforms,
        MATHEX_SCALAR machineLength,
        TransformsPtr outputTransformsPtr ) const;

    MATHEX_SCALAR   angleWithHorizontal( const MexVec3& vec ) const;

    void smoothTransition(
        const MexTransform3d& lastTransform,
        const MexTransform3d& thisTransform,
        const MexTransform3d& nextTransform,
        MATHEX_SCALAR machineLength,
        ctl_vector< MexTransform3d >* pTransforms ) const;

    void moveTransform( const MexTransform3d& transform,
        MexVec3 direction,
        MATHEX_SCALAR distance,
        MexTransform3d* pResult ) const;

    void addTurnTransformIfNecessary(
        TransformsPtr transformsPtr ) const;

    //  Data members
    MachPhysMachineImpl* pImpl_; //pimple implementation object
};

PER_DECLARE_PERSISTENT( MachPhysMachine );

#endif

/* End MACHINE.HPP **************************************************/
