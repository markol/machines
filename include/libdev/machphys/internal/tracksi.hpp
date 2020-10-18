/*
 * T R A C K S I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysTracksImpl

    Implementation class for MachPhysTracks.
*/

#ifndef _MACHPHYS_TRACKSI_HPP
#define _MACHPHYS_TRACKSI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machphys/internal/locomoim.hpp"

class MachPhysTracks;

class MachPhysTracksImpl : public MachPhysLocomotionMethodImpl
// Canonical form revoked
{
public:

    PER_MEMBER_PERSISTENT_VIRTUAL( MachPhysTracksImpl );
    PER_FRIEND_READ_WRITE( MachPhysTracksImpl );

private:
    friend class MachPhysTracks;

    MachPhysTracksImpl( MachPhysMachine* pMachine, MachPhysTracks* pTracks );

    virtual ~MachPhysTracksImpl();

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

    friend ostream& operator <<( ostream& o, const MachPhysTracksImpl& t );

    MachPhysTracksImpl( const MachPhysTracksImpl& );
    MachPhysTracksImpl& operator =( const MachPhysTracksImpl& );

    //data members
    MachPhysTracks* pTracks_; //The owning object
};

PER_DECLARE_PERSISTENT( MachPhysTracksImpl );

#endif

/* End TRACKSI.HPP **************************************************/
