/*
 * W H E E L S I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysWheelsImpl

    Implementation class for MachPhysWheels.
*/

#ifndef _MACHPHYS_WHEELSI_HPP
#define _MACHPHYS_WHEELSI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machphys/internal/locomoim.hpp"
#include "mathex/mathex.hpp"
#include "mathex/radians.hpp"
#include "ctl/vector.hpp"

class W4dLink;
class MachPhysWheels;

class MachPhysWheelsImpl : public MachPhysLocomotionMethodImpl
// Canonical form revoked
{
public:

    PER_MEMBER_PERSISTENT_VIRTUAL( MachPhysWheelsImpl );
    PER_FRIEND_READ_WRITE( MachPhysWheelsImpl );

private:
    friend class MachPhysWheels;

    typedef    ctl_vector< W4dLink* >  W4dLinks;

    MachPhysWheelsImpl( MachPhysMachine* pMachine, MachPhysWheels* pWheels,
        MATHEX_SCALAR wheelRadius, const W4dLinks& leftWheels, const W4dLinks& rightWheels );

    virtual ~MachPhysWheelsImpl();

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

    friend ostream& operator <<( ostream& o, const MachPhysWheelsImpl& t );

    MachPhysWheelsImpl( const MachPhysWheelsImpl& );
    MachPhysWheelsImpl& operator =( const MachPhysWheelsImpl& );

    //data members
    MachPhysWheels* pWheels_; //The owning object
    MATHEX_SCALAR wheelRadius_;
    MexRadians leftLastAngle_; //Angle implied by previous move fro left wheels
    MexRadians rightLastAngle_; //Angle implied by previous move fro right wheels
    MexRadians leftLastAngleSpeed_; //Final rotation rate implied by previous move.
                                    //Values close to zero are rounded to zero.
    MexRadians rightLastAngleSpeed_; //Final rotation rate implied by previous move.
                                    //Values close to zero are rounded to zero.
    W4dLinks    leftWheels_;
    W4dLinks    rightWheels_;
};

PER_DECLARE_PERSISTENT( MachPhysWheelsImpl );

#endif

/* End WHEELSI.HPP **************************************************/
