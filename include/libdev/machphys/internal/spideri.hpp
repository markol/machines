/*
 * S P I D E R I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysSpiderLegsImpl

    Implementation class for MachPhysSpiderLegs.
*/

#ifndef _MACHPHYS_SPIDERI_HPP
#define _MACHPHYS_SPIDERI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machphys/internal/locomoim.hpp"
#include "world4d/compplan.hpp"

class MachPhysSpiderLegs;
class W4dCompositePlan;

class MachPhysSpiderLegsImpl : public MachPhysLocomotionMethodImpl
// Canonical form revoked
{
public:

    PER_MEMBER_PERSISTENT_VIRTUAL( MachPhysSpiderLegsImpl );
    PER_FRIEND_READ_WRITE( MachPhysSpiderLegsImpl );

private:
    friend class MachPhysSpiderLegs;

    MachPhysSpiderLegsImpl( MachPhysMachine* pMachine, MachPhysSpiderLegs* pSpider,
					        const W4dCompositePlanPtr& restingPlanPtr,
					        const W4dCompositePlanPtr& walkingPlanPtr,
					        const W4dCompositePlanPtr& turningLeftPlanPtr,
					        const W4dCompositePlanPtr& turningRightPlanPtr,
					        const W4dCompositePlanPtr& startWalkingPlanPtr,
					    	const W4dCompositePlanPtr& stopWalkingPlanPtr ); 

    virtual ~MachPhysSpiderLegsImpl();

    ///////////////////////////////////////////////////
    // Inherited from MachPhysLocomotionMethodImpl

    //Compute the instantaneous position for the machine at baseLocation, facing along unitFacingDirection.
    //Return the 3d location for the origin in pLocation, and the machine vertical normal in pNormal.
    virtual void machineSurfacePosition
    (
        const MachPhysPlanetSurface& surface, const MexPoint3d& baseLocation, const MexVec3& unitFacingDirection,
        MexPoint3d* pLocation, MexVec3* pNormal
    ) const;

    //Update the locomotion animations for the machine depending on current activity as specified by state.
    virtual void firstPersonMotionAnimations( MachPhysLocomotionMethod::FirstPersonMotionState state );

    ///////////////////////////////////////////////////

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MachPhysSpiderLegsImpl& t );

    MachPhysSpiderLegsImpl( const MachPhysSpiderLegsImpl& );
    MachPhysSpiderLegsImpl& operator =( const MachPhysSpiderLegsImpl& );

    //data members
    MachPhysSpiderLegs* pSpider_; //The owning object
    W4dCompositePlanPtr restingPlanPtr_; //Resting animation
    W4dCompositePlanPtr walkingPlanPtr_; //Walking forwards animation
    W4dCompositePlanPtr turningLeftPlanPtr_; //Turning left animation
    W4dCompositePlanPtr turningRightPlanPtr_; //Turning right animation
    W4dCompositePlanPtr startWalkingPlanPtr_; //Start walking animation
    W4dCompositePlanPtr stopWalkingPlanPtr_; //Stop walking animation
};

PER_DECLARE_PERSISTENT( MachPhysSpiderLegsImpl );

#endif

/* End SPIDERI.HPP **************************************************/
