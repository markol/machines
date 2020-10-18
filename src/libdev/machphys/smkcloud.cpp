/*
 * S M K C L O U D . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/smkcloud.hpp"

#include "mathex/point3d.hpp"
#include "mathex/vec3.hpp"
#include "mathex/transf3d.hpp"
#include "phys/rampacce.hpp"
#include "phys/linemoti.hpp"
#include "render/matvec.hpp"
#include "world4d/sprite3d.hpp"
#include "world4d/visplan.hpp"
#include "world4d/simplsca.hpp"
#include "world4d/entyplan.hpp"
#include "machphys/random.hpp"

PER_DEFINE_PERSISTENT( MachPhysSmokeCloud );

MachPhysSmokeCloud::MachPhysSmokeCloud
(
    W4dEntity* pParent, const MexPoint3d& startPosition
)
:   W4dEntity( pParent, MexTransform3d( startPosition ), W4dEntity::NOT_SOLID )
{
    TEST_INVARIANT;
}

MachPhysSmokeCloud::MachPhysSmokeCloud( PerConstructor con )
: W4dEntity( con )
{
}

PhysRelativeTime MachPhysSmokeCloud::startSmoking
(
    uint nWisps, const MexPoint3d& endPosition, const MexVec3& randomOffset,
    const PhysAbsoluteTime& startTime, const PhysRelativeTime& wispInterval,
    MATHEX_SCALAR wispSize, MATHEX_SCALAR zDepthOffset,
    const W4dMaterialPlanPtr& materialPlanPtr
)
{
	return startSmoking(nWisps,
	             endPosition,
	             randomOffset,
	             startTime,
	             wispInterval,
	             wispSize,
	             zDepthOffset,
	             materialPlanPtr,
				 0,
				 0);
}

PhysRelativeTime MachPhysSmokeCloud::startSmoking
(
    uint nWisps, const MexPoint3d& endPosition, const MexVec3& randomOffset,
    const PhysAbsoluteTime& startTime, const PhysRelativeTime& wispInterval,
    MATHEX_SCALAR wispSize, MATHEX_SCALAR zDepthOffset,
    const W4dMaterialPlanPtr& materialPlanPtr, uint nRepetations, uint nBlankPeriods
)
{
    PRE( nWisps != 0 );
    PRE( wispSize > 0.0 );

    //Define a common visibility plan
    PhysRelativeTime planDuration = materialPlanPtr->duration();
    PhysRelativeTime visibilityPlanDuration = planDuration * ( nBlankPeriods + 1 );
    W4dVisibilityPlan* pVisibilityPlan = _NEW( W4dVisibilityPlan( true ) );

    pVisibilityPlan->add( false, planDuration );

	if(	visibilityPlanDuration > planDuration )
    	pVisibilityPlan->add( true, visibilityPlanDuration );

    W4dVisibilityPlanPtr visibilityPlanPtr( pVisibilityPlan );

    //Define a common scale plan to scale by a factor of 2
    W4dScalePlanPtr scalePlanPtr( _NEW( W4dSimpleUniformScalePlan( 1.0, 2.0, planDuration ) ) );

    //Compute the total duration of the animation
    PhysRelativeTime animationDuration = visibilityPlanDuration * ( nRepetations + 1 ) +
                                         (nWisps - 1) * wispInterval;

    //Generate the wisps
    PhysAbsoluteTime wispStartTime = startTime;
    while( nWisps-- )
    {
        //Get a random set of offsets in each axis
        MATHEX_SCALAR x = fabs( randomOffset.x() );
        MATHEX_SCALAR y = fabs( randomOffset.y() );
        MATHEX_SCALAR z = fabs( randomOffset.z() );
        if( x != 0.0 )
            x = MachPhysRandom::randomDouble( -x, x );

        if( y != 0.0 )
            y = MachPhysRandom::randomDouble( -y, y );
        if( z != 0.0 )
            z = MachPhysRandom::randomDouble( -z, z );

        //Construct a sprite at the required position
        MexTransform3d startTransform( MexPoint3d( x, y, z ) );
        W4dSprite3d* pWisp =
            _NEW( W4dSprite3d( this, startTransform, wispSize, wispSize,
                               materialPlanPtr->materialVec( 0.0, 0 )->front() ) );

        //Set LOD distance
        pWisp->visibleRange( 350.0 );

        //Start off invisible
        pWisp->visible( false );

        //Attach the common plans
        const uint nPeriods = nRepetations*(nBlankPeriods+1);
        W4dEntityPlan& wispEntityPlan = pWisp->entityPlanForEdit();
        wispEntityPlan.visibilityPlan( visibilityPlanPtr, wispStartTime, nRepetations);
        wispEntityPlan.scalePlan( scalePlanPtr, wispStartTime, nPeriods);
        wispEntityPlan.materialPlan( materialPlanPtr, wispStartTime, nPeriods);

        //Define a linear motion plan from start to end, offset by the random amount
        MexTransform3d endTransform( MexPoint3d( endPosition.x() + x,
                                                 endPosition.y() + y,
                                                 endPosition.z() + z ) );

        PhysMotionPlanPtr motionPlanPtr
        (
            _NEW( PhysLinearMotionPlan( startTransform, endTransform, planDuration ) )
        );

        wispEntityPlan.absoluteMotion( motionPlanPtr, wispStartTime, nPeriods);

        //set the zbuffer depth offset
        pWisp->depthOffset( zDepthOffset );

        //Increment wisp start time
        wispStartTime += wispInterval;
    }

    TEST_INVARIANT;

    return animationDuration;
}

MachPhysSmokeCloud::~MachPhysSmokeCloud()
{
    TEST_INVARIANT;

}

void MachPhysSmokeCloud::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysSmokeCloud& t )
{

    o << "MachPhysSmokeCloud " << (void*)&t << " start" << std::endl;
    o << "MachPhysSmokeCloud " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysSmokeCloud::intersectsLine
(
    const MexLine3d&, MATHEX_SCALAR*, Accuracy
) const
{
    return false;
}

void perWrite( PerOstream& ostr, const MachPhysSmokeCloud& cloud )
{
    const W4dEntity& base = cloud;

    ostr << base;

}

void perRead( PerIstream& istr, MachPhysSmokeCloud& cloud )
{
    W4dEntity& base = cloud;

    istr >> base;
}

/* End SMKCLOUD.CPP *************************************************/

