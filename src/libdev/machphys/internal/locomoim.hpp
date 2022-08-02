/*
 * L O C O M O I M . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysLocomotionMethodImpl 

    Implementation object for MachPhysLocomotionMethod
*/

#ifndef _MACHPHYS_LOCOMOIM_HPP
#define _MACHPHYS_LOCOMOIM_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "mathex/mathex.hpp"
#include "machphys/locomoti.hpp"

class MachPhysMachine;
class MachPhysPlanetSurface;
class MexPoint2d;
class MexPoint3d;
class MexVec3;

class MachPhysLocomotionMethodImpl
// Canonical form revoked
{
public:
    virtual ~MachPhysLocomotionMethodImpl();

    //The machine to be locomoted
    MachPhysMachine& machine() const;

    //Compute the instantaneous position for the machine at baseLocation, facing along unitFacingDirection.
    //Return the 3d location for the origin in pLocation, and the machine vertical normal in pNormal.
    virtual void machineSurfacePosition
    (
        const MachPhysPlanetSurface& surface, const MexPoint3d& baseLocation, const MexVec3& unitFacingDirection,
        MexPoint3d* pLocation, MexVec3* pNormal
    ) const = 0;

    //Distances in front of and behind the machine
    MATHEX_SCALAR foreDistance() const;
    MATHEX_SCALAR aftDistance() const;

    //Computes the machine surface position based on the terrain height and normals at the fore and aft
    //positions.
    void foreAftSurfacePosition
    (
        const MachPhysPlanetSurface& surface, const MexPoint3d& baseLocation, const MexVec3& unitFacingDirection,
        MexPoint3d* pLocation, MexVec3* pNormal
    ) const;

    //Update the locomotion animations for the machine depending on current activity as specified by state.
    virtual void firstPersonMotionAnimations( MachPhysLocomotionMethod::FirstPersonMotionState state ) = 0;

    //Previous locomotion state
    MachPhysLocomotionMethod::FirstPersonMotionState lastState() const;

    void CLASS_INVARIANT;

    PER_MEMBER_PERSISTENT_ABSTRACT( MachPhysLocomotionMethodImpl );
    PER_FRIEND_READ_WRITE( MachPhysLocomotionMethodImpl );

protected:
    MachPhysLocomotionMethodImpl( MachPhysMachine* pMachine );

private:
    friend class MachPhysLocomotionMethod;

    friend ostream& operator <<( ostream& o, const MachPhysLocomotionMethodImpl& t );

    //Update the previous state. Done by MachPhysLocomotionMethod.
    void lastState( MachPhysLocomotionMethod::FirstPersonMotionState newState );

    //revoked
    MachPhysLocomotionMethodImpl( const MachPhysLocomotionMethodImpl& );
    MachPhysLocomotionMethodImpl& operator =( const MachPhysLocomotionMethodImpl& );

    //data members
    MachPhysMachine* pMachine_;
    MATHEX_SCALAR foreDistance_;
    MATHEX_SCALAR aftDistance_;
    MachPhysLocomotionMethod::FirstPersonMotionState lastState_; //The previous locomotion state
    bool remoteFirstPerson_; //True iff this machine is being controlled in first person from a remote node
};

PER_DECLARE_PERSISTENT( MachPhysLocomotionMethodImpl );


#endif

/* End LOCOMOIM.HPP *************************************************/
