/*
 * L O C O M O I M . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/internal/locomoim.hpp"

#include "mathex/abox3d.hpp"
#include "mathex/vec3.hpp"
#include "mathex/point3d.hpp"
#include "mathex/transf3d.hpp"
#include "phys/rampacce.hpp"
#include "machphys/machine.hpp"
#include "machphys/plansurf.hpp"

PER_DEFINE_PERSISTENT_ABSTRACT( MachPhysLocomotionMethodImpl );

MachPhysLocomotionMethodImpl::MachPhysLocomotionMethodImpl( MachPhysMachine* pMachine )
:   pMachine_( pMachine ),
    lastState_( MachPhysLocomotionMethod::STOPPED ),
    remoteFirstPerson_( false )
{

    //Get the bounding volume for the machine, and hence deduce the fore and aft distances
    const MexAlignedBox3d& boundary = pMachine->compositeBoundingVolume();
    foreDistance_ = boundary.maxCorner().x();
    aftDistance_ = boundary.minCorner().x();
    ASSERT( foreDistance_ > 0.01  and  aftDistance_ < -0.01, "Funny shaped machine" );

    TEST_INVARIANT;
}

MachPhysLocomotionMethodImpl::~MachPhysLocomotionMethodImpl()
{
    TEST_INVARIANT;

}

void MachPhysLocomotionMethodImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysLocomotionMethodImpl& t )
{

    o << "MachPhysLocomotionMethodImpl " << (void*)&t << " start" << std::endl;
    o << "MachPhysLocomotionMethodImpl " << (void*)&t << " end" << std::endl;

    return o;
}

MachPhysLocomotionMethodImpl::MachPhysLocomotionMethodImpl( PerConstructor )
{
}

void perWrite( PerOstream& ostr, const MachPhysLocomotionMethodImpl& t )
{
    ostr << t.pMachine_;
    ostr << t.foreDistance_;
    ostr << t.aftDistance_;
    ostr << t.remoteFirstPerson_;
}

void perRead( PerIstream& istr, MachPhysLocomotionMethodImpl& t )
{
    istr >> t.pMachine_;
    istr >> t.foreDistance_;
    istr >> t.aftDistance_;
    istr >> t.remoteFirstPerson_;
}

MachPhysMachine& MachPhysLocomotionMethodImpl::machine() const
{
    return *pMachine_;
}

MATHEX_SCALAR MachPhysLocomotionMethodImpl::foreDistance() const
{
    return foreDistance_;
}

MATHEX_SCALAR MachPhysLocomotionMethodImpl::aftDistance() const
{
    return aftDistance_;
}

void MachPhysLocomotionMethodImpl::foreAftSurfacePosition
(
    const MachPhysPlanetSurface& surface, const MexPoint3d& baseLocation, const MexVec3& unitFacingDirection,
    MexPoint3d* pLocation, MexVec3* pNormal
) const
{
    //Get the terrain height and normal at the fore and aft positions
    MexVec3 foreOffset( unitFacingDirection );
    foreOffset *= foreDistance_;
    MexPoint3d foreLocation( baseLocation );
    foreLocation += foreOffset;

    MexVec3 foreUnitNormal;
    MATHEX_SCALAR zFore = surface.terrainHeight( foreLocation.x(), foreLocation.y(), machine().floors(), &foreUnitNormal );
    foreLocation.z( zFore );

    MexVec3 aftOffset( unitFacingDirection );
    aftOffset *= aftDistance_;
    MexPoint3d aftLocation( baseLocation );
    aftLocation += aftOffset;

    MexVec3 aftUnitNormal;
    MATHEX_SCALAR zAft = surface.terrainHeight( aftLocation.x(), aftLocation.y(), machine().floors(), &aftUnitNormal );
    aftLocation.z( zAft );

    //Interpolate the height for the base point
    MATHEX_SCALAR zBase = (foreDistance_ * zAft - aftDistance_ * zFore) / (foreDistance_ - aftDistance_);

    //Construct a unit vector in the direction from the aft point to the fore point
    MexVec3 aftForeUnitDirection( aftLocation, foreLocation );
    aftForeUnitDirection.makeUnitVector();

    //To get a vector along the base surface we do the following.
    //Fore each of fore and aft take the cross product of the joining line vector,
    //and the surface normal at the point. These 2 vectors lie in the surface at
    //the contact points, and perpendicular to adjoining line. Average them to get one for the machine.
    MexVec3 foreGroundVector( MexVec3::crossProduct( foreUnitNormal, aftForeUnitDirection ) );
    MexVec3 aftGroundVector( MexVec3::crossProduct( aftUnitNormal, aftForeUnitDirection ) );
    MexVec3 averageGroundVector( foreGroundVector );
    averageGroundVector += aftGroundVector;
    averageGroundVector.makeUnitVector();

    //To get the vertical direction for the machine, get a vector normal to both the
    //fore/aft direction line, and the averaged ground vector.
    MexVec3 baseVerticalNormal = MexVec3::crossProduct( aftForeUnitDirection, averageGroundVector );
    baseVerticalNormal.makeUnitVector();

    //Return the results
    *pLocation = baseLocation;
    pLocation->z( zBase );
    *pNormal = baseVerticalNormal;
}

void MachPhysLocomotionMethodImpl::lastState( MachPhysLocomotionMethod::FirstPersonMotionState newState )
{
    lastState_ = newState;
}

MachPhysLocomotionMethod::FirstPersonMotionState MachPhysLocomotionMethodImpl::lastState() const
{
    return lastState_;
}
/* End LOCOMOIM.CPP *************************************************/
