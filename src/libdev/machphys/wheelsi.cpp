/*
 * W H E E L S I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/link.hpp"
#include "mathex/transf3d.hpp"
#include "phys/rampacce.hpp"
#include "machphys/internal/wheelsi.hpp"

#include "mathex/point2d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/vec3.hpp"
#include "machphys/wheels.hpp"
#include "machphys/plansurf.hpp"

PER_DEFINE_PERSISTENT( MachPhysWheelsImpl );

MachPhysWheelsImpl::MachPhysWheelsImpl( MachPhysMachine* pMachine, MachPhysWheels* pWheels,
        MATHEX_SCALAR wheelRadius, const W4dLinks& leftWheels, const W4dLinks& rightWheels
 )
: MachPhysLocomotionMethodImpl( pMachine ),
  pWheels_( pWheels ),
  wheelRadius_( wheelRadius ),
  leftWheels_( leftWheels ),
  rightWheels_( rightWheels ),
  leftLastAngle_( 0 ),
  rightLastAngle_( 0 ),
  leftLastAngleSpeed_( 0 ),
  rightLastAngleSpeed_( 0 )
{

    TEST_INVARIANT;
}

MachPhysWheelsImpl::~MachPhysWheelsImpl()
{
    TEST_INVARIANT;

}

void MachPhysWheelsImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysWheelsImpl& t )
{

    o << "MachPhysWheelsImpl " << (void*)&t << " start" << std::endl;
    o << "MachPhysWheelsImpl " << (void*)&t << " end" << std::endl;

    return o;
}

MachPhysWheelsImpl::MachPhysWheelsImpl( PerConstructor con )
: MachPhysLocomotionMethodImpl( con ),
  pWheels_( NULL )
{
}

void perWrite( PerOstream& ostr, const MachPhysWheelsImpl& wheels )
{
    const MachPhysLocomotionMethodImpl& base = wheels;

    ostr << base;

    ostr << wheels.pWheels_;
    ostr << wheels.wheelRadius_;
    ostr << wheels.leftLastAngle_;
    ostr << wheels.rightLastAngle_;
    ostr << wheels.leftLastAngleSpeed_;
    ostr << wheels.rightLastAngleSpeed_;
    ostr << wheels.leftWheels_;
    ostr << wheels.rightWheels_;
}

void perRead( PerIstream& istr, MachPhysWheelsImpl& wheels )
{
    MachPhysLocomotionMethodImpl& base = wheels;

    istr >> base;

    istr >> wheels.pWheels_;
    istr >> wheels.wheelRadius_;
    istr >> wheels.leftLastAngle_;
    istr >> wheels.rightLastAngle_;
    istr >> wheels.leftLastAngleSpeed_;
    istr >> wheels.rightLastAngleSpeed_;
    istr >> wheels.leftWheels_;
    istr >> wheels.rightWheels_;
}

//virtual
void MachPhysWheelsImpl::machineSurfacePosition
(
    const MachPhysPlanetSurface& surface, const MexPoint3d& baseLocation, const MexVec3& unitFacingDirection,
    MexPoint3d* pLocation, MexVec3* pNormal
) const
{
    //Use the 'bicycle' method
    foreAftSurfacePosition( surface, baseLocation, unitFacingDirection, pLocation, pNormal );
}

//virtual
void MachPhysWheelsImpl::firstPersonMotionAnimations( MachPhysLocomotionMethod::FirstPersonMotionState state )
{
    pWheels_->doFirstPersonMotionAnimations( state );
}

/* End WHEELSI.CPP **************************************************/
