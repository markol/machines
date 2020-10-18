/*
 * H O V E R I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysHoverBootsImpl

    Implementation class for MachPhysHoverBoots.
*/

#ifndef _MACHPHYS_HOVERI_HPP
#define _MACHPHYS_HOVERI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machphys/internal/locomoim.hpp"
#include "mathex/radians.hpp"

class MachPhysHoverBoots;
class W4dLink;

class MachPhysHoverBootsImpl : public MachPhysLocomotionMethodImpl
// Canonical form revoked
{
public:

    PER_MEMBER_PERSISTENT_VIRTUAL( MachPhysHoverBootsImpl );
    PER_FRIEND_READ_WRITE( MachPhysHoverBootsImpl );

private:
    friend class MachPhysHoverBoots;

    MachPhysHoverBootsImpl( MachPhysMachine* pMachine, MachPhysHoverBoots* pHoverBoots,
        W4dLink* pLHoverBoot,
        W4dLink* pRHoverBoot,
        W4dLink* pBobbingLink,
        MATHEX_SCALAR bobHeight
     );

    virtual ~MachPhysHoverBootsImpl();

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

    friend ostream& operator <<( ostream& o, const MachPhysHoverBootsImpl& t );

    MachPhysHoverBootsImpl( const MachPhysHoverBootsImpl& );
    MachPhysHoverBootsImpl& operator =( const MachPhysHoverBootsImpl& );

    //data members
    MachPhysHoverBoots* pHoverBoots_; //The owning object
    W4dLink*    pLHoverBoot_;
    W4dLink*    pRHoverBoot_;
    W4dLink*    pBobbingLink_;
    MexRadians lastLAngle_; //Last angle setup in moveAnimation
    MexRadians lastRAngle_; //Last angle setup in moveAnimation

    MATHEX_SCALAR       bobHeight_;
};

PER_DECLARE_PERSISTENT( MachPhysHoverBootsImpl );

#endif

/* End HOVERI.HPP **************************************************/
