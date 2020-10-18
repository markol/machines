/*
 * G L I D E R I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/internal/glideri.hpp"

#include "mathex/point2d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/vec3.hpp"

#include "phys/rampacce.hpp"
#include "world4d/link.hpp"

#include "machphys/glider.hpp"
#include "machphys/plansurf.hpp"
#include "machphys/machine.hpp"

PER_DEFINE_PERSISTENT( MachPhysGliderImpl );

MachPhysGliderImpl::MachPhysGliderImpl( MachPhysMachine* pMachine, MachPhysGlider* pGlider, MATHEX_SCALAR height )
:   MachPhysLocomotionMethodImpl( pMachine ),
    pGlider_( pGlider ),
	height_( height ),
	pBody_( NULL ),
	pHip_( NULL ),
	pBrake_( NULL ),
	pThrust_( NULL ),
	pLFan_( NULL ),
	pRFan_( NULL ),
	pLTurb_( NULL ),
	pRTurb_( NULL )
{

    TEST_INVARIANT;
}

MachPhysGliderImpl::~MachPhysGliderImpl()
{
    TEST_INVARIANT;

}

void MachPhysGliderImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysGliderImpl& t )
{

    o << "MachPhysGliderImpl " << (void*)&t << " start" << std::endl;
    o << "MachPhysGliderImpl " << (void*)&t << " end" << std::endl;

    return o;
}

MachPhysGliderImpl::MachPhysGliderImpl( PerConstructor con )
: MachPhysLocomotionMethodImpl( con ),
  pGlider_( NULL )
{
}

void perWrite( PerOstream& ostr, const MachPhysGliderImpl& glider )
{
    const MachPhysLocomotionMethodImpl& base = glider;

    ostr << base;

    ostr << glider.pGlider_;
    ostr << glider.pBody_;
    ostr << glider.pHip_;
    ostr << glider.height_;
    ostr << glider.hipPosition_;
	ostr << glider.pBrake_,
	ostr << glider.pThrust_;
	ostr << glider.pLFan_;
	ostr << glider.pRFan_;
	ostr << glider.pLTurb_;
	ostr << glider.pRTurb_;
}

void perRead( PerIstream& istr, MachPhysGliderImpl& glider )
{
    MachPhysLocomotionMethodImpl& base = glider;

    istr >> base;

    istr >> glider.pGlider_;
    istr >> glider.pBody_;
    istr >> glider.pHip_;
    istr >> glider.height_;
    istr >> glider.hipPosition_;
	istr >> glider.pBrake_,
	istr >> glider.pThrust_;
	istr >> glider.pLFan_;
	istr >> glider.pRFan_;
	istr >> glider.pLTurb_;
	istr >> glider.pRTurb_;
}

//virtual
void MachPhysGliderImpl::machineSurfacePosition
(
    const MachPhysPlanetSurface& surface, const MexPoint3d& baseLocation, const MexVec3&,
    MexPoint3d* pLocation, MexVec3* pNormal
) const
{
    *pLocation = MexPoint3d( baseLocation.x(), baseLocation.y(), surface.terrainHeight( baseLocation.x(), baseLocation.y(), machine().floors() ) );
    *pNormal = MexVec3( 0.0, 0.0, 1.0 );
}

//virtual
void MachPhysGliderImpl::firstPersonMotionAnimations( MachPhysLocomotionMethod::FirstPersonMotionState state )
{
    pGlider_->doFirstPersonMotionAnimations( state );
}

/* End GLIDERI.CPP **************************************************/
