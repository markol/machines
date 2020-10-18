/*
 * G L I D E R I . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MachPhysGliderImpl

    Implementation class for MachPhysGlider.
*/

#ifndef _MACHPHYS_GLIDERI_HPP
#define _MACHPHYS_GLIDERI_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "machphys/internal/locomoim.hpp"
#include "mathex/point3d.hpp"

class MachPhysGlider;

class MachPhysGliderImpl : public MachPhysLocomotionMethodImpl
// Canonical form revoked
{
public:

    PER_MEMBER_PERSISTENT_VIRTUAL( MachPhysGliderImpl );
    PER_FRIEND_READ_WRITE( MachPhysGliderImpl );

private:
    friend class MachPhysGlider;

    MachPhysGliderImpl( MachPhysMachine* pMachine, MachPhysGlider* pGlider, MATHEX_SCALAR height );

    virtual ~MachPhysGliderImpl();

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

    friend ostream& operator <<( ostream& o, const MachPhysGliderImpl& t );

    MachPhysGliderImpl( const MachPhysGliderImpl& );
    MachPhysGliderImpl& operator =( const MachPhysGliderImpl& );

    //data members
    MachPhysGlider* pGlider_; //The owning object
    MATHEX_SCALAR   height_;
	W4dLink* 		pBody_;
	W4dLink* 		pHip_;
	MexPoint3d		hipPosition_;
	W4dLink* 		pBrake_;
	W4dLink* 		pThrust_;
	W4dLink*		pLFan_;
	W4dLink*		pRFan_;
	W4dLink*		pLTurb_;
	W4dLink*		pRTurb_;
};

PER_DECLARE_PERSISTENT( MachPhysGliderImpl );

#endif

/* End GLIDERI.HPP **************************************************/
