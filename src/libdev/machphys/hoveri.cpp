/*
 * H O V E R I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions
#include "mathex/transf3d.hpp"
#include "phys/rampacce.hpp"

#include "machphys/internal/hoveri.hpp"

#include "mathex/point2d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/vec3.hpp"
#include "machphys/hover.hpp"
#include "machphys/plansurf.hpp"
#include "machphys/machine.hpp"

#include "world4d/link.hpp"

PER_DEFINE_PERSISTENT( MachPhysHoverBootsImpl );

MachPhysHoverBootsImpl::MachPhysHoverBootsImpl( MachPhysMachine* pMachine, MachPhysHoverBoots* pHoverBoots,
        W4dLink* pLHoverBoot,
        W4dLink* pRHoverBoot,
        W4dLink* pBobbingLink,
        MATHEX_SCALAR bobHeight
 )
:   MachPhysLocomotionMethodImpl( pMachine ),
    pHoverBoots_( pHoverBoots ),
	pLHoverBoot_( pLHoverBoot ),
    pRHoverBoot_( pRHoverBoot ),
    pBobbingLink_( pBobbingLink ),
    lastLAngle_( 0 ),
    lastRAngle_( 0 ),
    bobHeight_( bobHeight )

{

    TEST_INVARIANT;
}

MachPhysHoverBootsImpl::~MachPhysHoverBootsImpl()
{
    TEST_INVARIANT;

}

void MachPhysHoverBootsImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysHoverBootsImpl& t )
{

    o << "MachPhysHoverBootsImpl " << (void*)&t << " start" << std::endl;
    o << "MachPhysHoverBootsImpl " << (void*)&t << " end" << std::endl;

    return o;
}

MachPhysHoverBootsImpl::MachPhysHoverBootsImpl( PerConstructor con )
:   MachPhysLocomotionMethodImpl( con ),
    pHoverBoots_( NULL ),
	pLHoverBoot_( NULL ),
    pRHoverBoot_( NULL ),
    pBobbingLink_( NULL ),
    lastLAngle_( 0 ),
    lastRAngle_( 0 ),
    bobHeight_( 0 )
{
}

void perWrite( PerOstream& ostr, const MachPhysHoverBootsImpl& boots )
{
    const MachPhysLocomotionMethodImpl& base = boots;

    ostr << base;

    ostr << boots.pHoverBoots_;
    ostr << boots.pLHoverBoot_;
    ostr << boots.pRHoverBoot_;
    ostr << boots.pBobbingLink_;
    ostr << boots.bobHeight_;
}

void perRead( PerIstream& istr, MachPhysHoverBootsImpl& boots )
{
    MachPhysLocomotionMethodImpl& base = boots;

    istr >> base;

    istr >> boots.pHoverBoots_;
    istr >> boots.pLHoverBoot_;
    istr >> boots.pRHoverBoot_;
    istr >> boots.pBobbingLink_;
    istr >> boots.bobHeight_;
}

//virtual
void MachPhysHoverBootsImpl::machineSurfacePosition
(
    const MachPhysPlanetSurface& surface, const MexPoint3d& baseLocation, const MexVec3&,
    MexPoint3d* pLocation, MexVec3* pNormal
) const
{
    *pLocation = MexPoint3d( baseLocation.x(), baseLocation.y(), surface.terrainHeight( baseLocation.x(), baseLocation.y(), machine().floors() ) );
    *pNormal = MexVec3( 0.0, 0.0, 1.0 );
}

//virtual
void MachPhysHoverBootsImpl::firstPersonMotionAnimations( MachPhysLocomotionMethod::FirstPersonMotionState state )
{
    pHoverBoots_->doFirstPersonMotionAnimations( state );
}

/* End HOVERI.CPP **************************************************/
