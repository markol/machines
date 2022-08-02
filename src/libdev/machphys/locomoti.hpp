/*
 * L O C O M O T I . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysLocomotionMethod

    Base class for the different locomotion methods that machines have available.
    The job of this class is to construct various types of motion plan and attach
    them to the appropriate links in the W4dComposite. The plans that this class
    creates are purely for aesthetic purposes, it does not have responsibility
    for the fundamental plan to move a machine from one location to another.
*/

#ifndef _LOCOMOTI_HPP
#define _LOCOMOTI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "phys/phys.hpp"
#include "machphys/machphys.hpp"

class MexPoint2d;
class MexPoint3d;
class MexVec3;
class MexRadians;
class PhysLinearTravelPlan;
class MachPhysMachine;
class W4dLink;
class MachPhysMachineMoveInfo;
class MachPhysLocomotionMethodImpl;
class MachPhysPlanetSurface;

class MexTransform3d;
class PhysRampAcceleration;

template <class T > class ctl_vector;
template <class T > class CtlCountedPtr;

class MachPhysLocomotionMethod
// Canonoical form revoked
{
public:
    typedef    ctl_vector< W4dLink* >  W4dLinks;
    
    //  Create a copy of a MachPhysLocomotionMethod and pass ownership
    //  of that copy back to the calling function. pMachine is a pointer
    //  to the machine which will own the copy.
    
    virtual MachPhysLocomotionMethod* clone( MachPhysMachine* pMachine, const W4dLinks& links ) = 0;
    
    virtual ~MachPhysLocomotionMethod();


    typedef ctl_vector< MexRadians >    TurnAngles;
    
    virtual void moveAnimations( const PhysLinearTravelPlan&, const TurnAngles& turnAngles,
                                 const MachPhysMachineMoveInfo& info ) = 0;


    // Create the appropriate motion plans to make the given motion look
    // correct according to the locomotion method being used by the
    // machine.

    typedef ctl_vector< MexTransform3d >        Transforms;
    typedef CtlCountedPtr< Transforms >         TransformsPtr;

    typedef ctl_vector< PhysRampAcceleration >  RampAccelerations;
    typedef CtlCountedPtr< RampAccelerations >  RampAccelerationsPtr;

    //  Revise the movement profile according to the locomotion
    //  type involved (e.g. hover boots always remain upright)    
    virtual void reviseProfile(
        TransformsPtr transformsPtr );
        
    //  Given a motion profile calculate the appropriate rampAccelerations
    virtual void calculateAccelerations(
        const TransformsPtr& transformsPtr,
        MATHEX_SCALAR startSpeed,
        MATHEX_SCALAR capTranslationSpeed,
        MachPhys::FinalState finalState,
        RampAccelerationsPtr rampAccelerationsPtr ) const;

    //  PRE( pTransforms->size() >= 2 );
    //  PRE( pRampAccelerations->size() == 0 ;
    //  PRE( MexUtility::inStraightLine( *tran)sformsPtr ) );
    //  POST( pRampAccelerations->size() == pTransforms->size() - 1 );

    //  Stop the motion dead. Apply appropriate animations.
    virtual void stopDead() = 0;

    //Compute the instantaneous position for the machine at baseLocation, facing along unitFacingDirection.
    //Return the 3d location for the origin in pLocation, and the machine vertical normal in pNormal.
    void machineSurfacePosition
    (
        const MachPhysPlanetSurface& surface, const MexPoint3d& baseLocation, const MexVec3& unitFacingDirection,
        MexPoint3d* pLocation, MexVec3* pNormal
    ) const;

    //Update the locomotion animations for the machine depending on current activity as specified by state.
    enum FirstPersonMotionState
    {
        STOPPED, MOVING_FORWARDS, MOVING_BACKWARDS, TURNING_LEFT, TURNING_RIGHT,
        ENTERING, LEAVING
    };
    void firstPersonMotionAnimations( MachPhysLocomotionMethod::FirstPersonMotionState state );

    //Set/get mode in which the associated machine is in 1st person, and being controlled
    //on a remote node
    void isRemotelyControlledFirstPerson( bool is );
    bool isRemotelyControlledFirstPerson() const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysLocomotionMethod& t );

    PER_MEMBER_PERSISTENT_ABSTRACT( MachPhysLocomotionMethod );
    PER_FRIEND_READ_WRITE( MachPhysLocomotionMethod );
    
protected:
    MachPhysLocomotionMethod(  MachPhysLocomotionMethodImpl* pImpl  );
    //  PRE( pImpl != NULL );

    MachPhysMachine* pMachine() const;
    //  POST( result != NULL );

    //  The default function to calculate ramp accelerations
    void doCalculateAccelerations(
        const TransformsPtr& transformsPtr,
        MATHEX_SCALAR startSpeed,
        MATHEX_SCALAR capTranslationSpeed,
        MachPhys::FinalState finalState,
        RampAccelerationsPtr rampAccelerationsPtr ) const;

    //  Rotate the given transforms so that their Z axis is pointing straight up
    void forceUpright( TransformsPtr transformsPtr );

	static void  pitchOrRollAnimation(W4dLink* pEntity, 
	                               const MexRadians& angle, 
	                               const MexVec3& axis, 
	                               const PhysRelativeTime& duration );

	static MexTransform3d transform(const MexRadians& angle, const MexVec3& axis, const MexPoint3d& position );

	enum {BOBBING};

private:
    // Operation deliberately revoked
    MachPhysLocomotionMethod( const MachPhysLocomotionMethod& );

    // Operation deliberately revoked
    MachPhysLocomotionMethod& operator =( const MachPhysLocomotionMethod& );

    // Operation deliberately revoked
    bool operator ==( const MachPhysLocomotionMethod& );

    MachPhysLocomotionMethodImpl* pImpl_; //implementation object
};

PER_DECLARE_PERSISTENT( MachPhysLocomotionMethod );

#endif

/* End LOCOMOTI.HPP *************************************************/
