/*
 * W H E E L S . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysWheels

    A brief description of the class should go in here
*/

#ifndef _WHEELS_HPP
#define _WHEELS_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "machphys/locomoti.hpp"


class W4dLink;
class MachPhysMachineMoveInfo;
class MachPhysWheelsImpl;

class MachPhysWheels : public MachPhysLocomotionMethod
{
public:
    MachPhysWheels(
        MachPhysMachine* pMachine,
        MATHEX_SCALAR wheelRadius, const W4dLinks& leftWheels, const W4dLinks& rightWheels );

    ~MachPhysWheels();

    virtual MachPhysLocomotionMethod* clone( MachPhysMachine* pMachine, const W4dLinks& links );

    virtual void moveAnimations( const PhysLinearTravelPlan& linearTravelPlan, const TurnAngles& turnAngles,
                                 const MachPhysMachineMoveInfo& info );

    //  Stop the move dead. Apply appropriate animations.
    virtual void stopDead();

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysWheels& t );

    PER_MEMBER_PERSISTENT_VIRTUAL( MachPhysWheels );
    PER_FRIEND_READ_WRITE( MachPhysWheels );

private:
    friend class MachPhysWheelsImpl;

    // Operation deliberately revoked
    MachPhysWheels( const MachPhysWheels& );
    MachPhysWheels& operator =( const MachPhysWheels& );
    bool operator ==( const MachPhysWheels& );

    //Callback to do the work of displaying the locomotion animations
    void doFirstPersonMotionAnimations( MachPhysLocomotionMethod::FirstPersonMotionState state );
	void firstPersonMotionAnimations( bool leftForwards, bool rightForwards );

    MachPhysWheelsImpl* pImpl_;
};

PER_DECLARE_PERSISTENT( MachPhysWheels );


#endif

/* End WHEELS.HPP ***************************************************/
