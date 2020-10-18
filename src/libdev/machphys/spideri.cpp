/*
 * S P I D E R I . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "ctl/countptr.hpp"
#include "machphys/internal/spideri.hpp"

#include "mathex/point2d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/vec3.hpp"
#include "phys/rampacce.hpp"
#include "world4d/compplan.hpp"
#include "machphys/spider.hpp"
#include "machphys/plansurf.hpp"

PER_DEFINE_PERSISTENT( MachPhysSpiderLegsImpl );

MachPhysSpiderLegsImpl::MachPhysSpiderLegsImpl( MachPhysMachine* pMachine, MachPhysSpiderLegs* pSpider,
					        const W4dCompositePlanPtr& restingPlanPtr,
					        const W4dCompositePlanPtr& walkingPlanPtr,
					        const W4dCompositePlanPtr& turningLeftPlanPtr,
					        const W4dCompositePlanPtr& turningRightPlanPtr,
					        const W4dCompositePlanPtr& startWalkingPlanPtr,
					    	const W4dCompositePlanPtr& stopWalkingPlanPtr
 )
: MachPhysLocomotionMethodImpl( pMachine ),
  pSpider_( pSpider ),
  restingPlanPtr_( restingPlanPtr ),
  walkingPlanPtr_( walkingPlanPtr ),
  turningLeftPlanPtr_( turningLeftPlanPtr ),
  turningRightPlanPtr_( turningRightPlanPtr ),
  startWalkingPlanPtr_( startWalkingPlanPtr ),
  stopWalkingPlanPtr_( stopWalkingPlanPtr )
{

    TEST_INVARIANT;
}

MachPhysSpiderLegsImpl::~MachPhysSpiderLegsImpl()
{
    TEST_INVARIANT;

}

void MachPhysSpiderLegsImpl::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysSpiderLegsImpl& t )
{

    o << "MachPhysSpiderLegsImpl " << (void*)&t << " start" << std::endl;
    o << "MachPhysSpiderLegsImpl " << (void*)&t << " end" << std::endl;

    return o;
}

MachPhysSpiderLegsImpl::MachPhysSpiderLegsImpl( PerConstructor con )
: MachPhysLocomotionMethodImpl( con ),
  pSpider_( NULL )
{
}

void perWrite( PerOstream& ostr, const MachPhysSpiderLegsImpl& legs )
{
    const MachPhysLocomotionMethodImpl& base = legs;

    ostr << base;

    ostr << legs.pSpider_;
    ostr << legs.restingPlanPtr_;
    ostr << legs.walkingPlanPtr_;
    ostr << legs.turningLeftPlanPtr_;
    ostr << legs.turningRightPlanPtr_;
    ostr << legs.startWalkingPlanPtr_;
    ostr << legs.stopWalkingPlanPtr_;
}

void perRead( PerIstream& istr, MachPhysSpiderLegsImpl& legs )
{
    MachPhysLocomotionMethodImpl& base = legs;

    istr >> base;

    istr >> legs.pSpider_;
    istr >> legs.restingPlanPtr_;
    istr >> legs.walkingPlanPtr_;
    istr >> legs.turningLeftPlanPtr_;
    istr >> legs.turningRightPlanPtr_;
    istr >> legs.startWalkingPlanPtr_;
    istr >> legs.stopWalkingPlanPtr_;
}

//virtual
void MachPhysSpiderLegsImpl::machineSurfacePosition
(
    const MachPhysPlanetSurface& surface, const MexPoint3d& baseLocation, const MexVec3& unitFacingDirection,
    MexPoint3d* pLocation, MexVec3* pNormal
) const
{
    //Use the 'bicycle' method
    foreAftSurfacePosition( surface, baseLocation, unitFacingDirection, pLocation, pNormal );
}

//virtual
void MachPhysSpiderLegsImpl::firstPersonMotionAnimations( MachPhysLocomotionMethod::FirstPersonMotionState state )
{
    pSpider_->doFirstPersonMotionAnimations( state );
}
/* End SPIDERI.CPP **************************************************/
