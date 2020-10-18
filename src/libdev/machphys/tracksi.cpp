/*
 * T R A C K S I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/internal/tracksi.hpp"

#include "phys/rampacce.hpp"

#include "mathex/point2d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/vec3.hpp"
#include "machphys/tracks.hpp"
#include "machphys/plansurf.hpp"

PER_DEFINE_PERSISTENT( MachPhysTracksImpl );

MachPhysTracksImpl::MachPhysTracksImpl( MachPhysMachine* pMachine, MachPhysTracks* pTracks )
:   MachPhysLocomotionMethodImpl( pMachine ),
    pTracks_( pTracks )
{

    TEST_INVARIANT;
}

MachPhysTracksImpl::~MachPhysTracksImpl()
{
    TEST_INVARIANT;

}

void MachPhysTracksImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysTracksImpl& t )
{

    o << "MachPhysTracksImpl " << (void*)&t << " start" << std::endl;
    o << "MachPhysTracksImpl " << (void*)&t << " end" << std::endl;

    return o;
}

MachPhysTracksImpl::MachPhysTracksImpl( PerConstructor con )
: MachPhysLocomotionMethodImpl( con ),
  pTracks_( NULL )
{
}

void perWrite( PerOstream& ostr, const MachPhysTracksImpl& wheels )
{
    const MachPhysLocomotionMethodImpl& base = wheels;

    ostr << base;

    ostr << wheels.pTracks_;
}

void perRead( PerIstream& istr, MachPhysTracksImpl& wheels )
{
    MachPhysLocomotionMethodImpl& base = wheels;

    istr >> base;

    istr >> wheels.pTracks_;
}

//virtual
void MachPhysTracksImpl::machineSurfacePosition
(
    const MachPhysPlanetSurface& surface, const MexPoint3d& baseLocation, const MexVec3& unitFacingDirection,
    MexPoint3d* pLocation, MexVec3* pNormal
) const
{
    //Use the 'bicycle' method
    foreAftSurfacePosition( surface, baseLocation, unitFacingDirection, pLocation, pNormal );
}

//virtual
void MachPhysTracksImpl::firstPersonMotionAnimations( MachPhysLocomotionMethod::FirstPersonMotionState state )
{
    pTracks_->doFirstPersonMotionAnimations( state );
}

/* End TRACKSI.CPP **************************************************/
