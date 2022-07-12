/*
 * S P I D E R . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MachPhysSpiderLegs

    A brief description of the class should go in here
*/

#ifndef _SPIDER_HPP
#define _SPIDER_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "ctl/vector.hpp"

#include "machphys/locomoti.hpp"

//Forward refs
class W4dCompositePlan;
class MachPhysMachine;
class MachPhysMachineMoveInfo;
class MachPhysSpiderLegsImpl;

class MachPhysSpiderLegs : public MachPhysLocomotionMethod
{
public:
    //ctor.
    //The machine being locomoted is pMachine.
    //The animation for resting is restingPlanPtr
    //The animation for walking is pWalkingPlan
    //The animation for turning left is pTurningLeftPlan
    //The animation for turning right is pTurningRightPlan
    //All the pointers must reference objects which persist during the lifetime
    //of this object.
    MachPhysSpiderLegs( MachPhysMachine* pMachine,
        const W4dCompositePlanPtr& restingPlanPtr,
        const W4dCompositePlanPtr& walkingPlanPtr,
        const W4dCompositePlanPtr& turningLeftPlanPtr,
        const W4dCompositePlanPtr& turningRightPlanPtr,
        const W4dCompositePlanPtr& startWalkingPlanPtr,
    	const W4dCompositePlanPtr& stopWalkingPlanPtr );

    //dtor
    ~MachPhysSpiderLegs();

    virtual MachPhysLocomotionMethod* clone( MachPhysMachine* pMachine,
        const W4dLinks& links );

    void CLASS_INVARIANT;

    //Initiate the movement animations
    virtual void moveAnimations( const PhysLinearTravelPlan& linearTravelPlan, const TurnAngles& turnAngles,
                                 const MachPhysMachineMoveInfo& info );


    //  Given a motion profile calculate the appropriate rampAccelerations
    virtual void calculateAccelerations(
        const TransformsPtr& transformsPtr,
        MATHEX_SCALAR startSpeed,
        MATHEX_SCALAR capTranslationSpeed,
        MachPhys::FinalState finalState,
        RampAccelerationsPtr rampAccelerationsPtr ) const;
    //  PRE( pTransforms->size() >= 2 );
    //  PRE( pRampAccelerations->size() == 0 );
    //  PRE( MexUtility::inStraightLine( *transformsPtr ) );
    //  POST( pRampAccelerations->size() == pTransforms->size() - 1 );

    //  Stop the move dead. Apply appropriate animations.
    virtual void stopDead();

    friend ostream& operator <<( ostream& o, const MachPhysSpiderLegs& t );

    PER_MEMBER_PERSISTENT_VIRTUAL( MachPhysSpiderLegs );
    PER_FRIEND_READ_WRITE( MachPhysSpiderLegs );

private:
    friend class MachPhysSpiderLegsImpl;

    // Operations deliberately revoked
    MachPhysSpiderLegs( const MachPhysSpiderLegs& );
    MachPhysSpiderLegs& operator =( const MachPhysSpiderLegs& );
    bool operator ==( const MachPhysSpiderLegs& );

    void translationAnimations( PhysAbsoluteTime startTime, PhysRelativeTime duration );
    void rotationAnimations( MexRadians angle, PhysAbsoluteTime startTime, PhysRelativeTime duration );

	//  Adjust the time of the ramp acceleration so that the motion
    //  uses a whole number of turning animations.
    void reviseRotationProfile( RampAccelerations::iterator iAcc ) const;

	//  Adjust the times so that the overall motion uses a whole
    //  number of walking animations.
    void reviseTranslationProfile(
        RampAccelerations::iterator begin,
        RampAccelerations::iterator end
    ) const;
    // PRE( fabs( (*begin).startSpeed() ) < MexEpsilon::instance() );
    // PRE( fabs( (*(end-1)).endSpeed() ) < MexEpsilon::instance() );

    //Callback to do the work of displaying the locomotion animations
    void doFirstPersonMotionAnimations( MachPhysLocomotionMethod::FirstPersonMotionState state );

    //data members
    MachPhysSpiderLegsImpl* pImpl_;
};

PER_DECLARE_PERSISTENT( MachPhysSpiderLegs );

#endif

/* End SPIDER.HPP ***************************************************/
