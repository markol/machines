/*
 * G L I D E R . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysGlider

    A brief description of the class should go in here
*/

#ifndef _MACHPHYS_GLIDER_HPP
#define _MACHPHYS_GLIDER_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machphys/locomoti.hpp"

class W4dLink;
class MachPhysGliderImpl;

class MachPhysGlider : public MachPhysLocomotionMethod
// Canonical form revoked
{
public:
    MachPhysGlider( MachPhysMachine* pMachine, MATHEX_SCALAR height );
    virtual ~MachPhysGlider( void );

    virtual MachPhysLocomotionMethod* clone( MachPhysMachine* pMachine, const W4dLinks& links );

    virtual void moveAnimations( const PhysLinearTravelPlan&, const TurnAngles& turnAngles,
                                 const MachPhysMachineMoveInfo& info );

    //  Stop the motion dead. Apply appropriate animations.
    virtual void stopDead();

    //  Revise a movement profile so that glider machines
    //  don't tip up when going up slopes. neither fly up and down when fly well clear the surface profile changes
    virtual void reviseProfile( TransformsPtr transformsPtr );

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_VIRTUAL( MachPhysGlider );
    PER_FRIEND_READ_WRITE( MachPhysGlider );

private:
    friend class MachPhysGliderImpl;

    friend ostream& operator <<( ostream& o, const MachPhysGlider& t );

    MachPhysGlider( const MachPhysGlider& );
    MachPhysGlider& operator =( const MachPhysGlider& );

    //Callback to do the work of displaying the locomotion animations
    void doFirstPersonMotionAnimations( MachPhysLocomotionMethod::FirstPersonMotionState state );

    void startBobbing( const PhysAbsoluteTime& startTime );
    void stopBobbing();

    //Snap the hip link to a centralised level position, cancelling the bobbing animation
    void snapToMidBob();

    MachPhysGliderImpl* pImpl_;
};

PER_DECLARE_PERSISTENT( MachPhysGlider );


#endif

/* End GLIDER.HPP ***************************************************/
