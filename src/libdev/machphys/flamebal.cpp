/*
 * F L A M E B A L . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/flamebal.hpp"
#include "render/texture.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/line3d.hpp"
#include "mathex/double.hpp"
#include "mathex/vec2.hpp"

#include "phys/rampacce.hpp"
#include "phys/motplan.hpp"
#include "phys/linemoti.hpp"

#include "render/mesh.hpp"
#include "render/meshinst.hpp"
#include "render/ttfpoly.hpp"
#include "render/colour.hpp"
#include "render/device.hpp"


#include "world4d/root.hpp"
#include "world4d/entity.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/simplsca.hpp"

#include "machphys/effects.hpp"
#include "machphys/random.hpp"
#include "machphys/private/otherper.hpp"

PER_DEFINE_PERSISTENT( MachPhysFlameBall );

//one time ctor
MachPhysFlameBall::MachPhysFlameBall( FlameBallType type )
:MachPhysLinearProjectile(MachPhysOtherPersistence::instance().pRoot(), MexTransform3d())
{
	// The current model is all black with emissive flames and things.  Hence,
	// it should not need lighting.  This could change if the model changes.
	doNotLight(true);

	MATHEX_SCALAR size = 0;

	if ( type == GRUNT_FLAME_BALL )
		size = 0.8;

	else if ( type == TURRET_FLAME_BALL )
		size = 2.4;

	else
	{
		ASSERT(true, " wrong flame ball type " );
	}

    Ren::MeshPtr meshPtr = RenMesh::createEmpty();

	//set the first texture of the flame matPlan
    RenMaterial mat = MachPhysEffects::flameBallMaterialPlan()->materialVec( 0, 0 )->front();

	RenTTFRectangle ttf;
	ttf.width(size);
	ttf.height(size);
	ttf.depthOffset(-2.5);
	ttf.material(mat);

	uint clockWise = MachPhysRandom::randomInt(0, 1);
	static uint16 startCorner = 0;

	switch(startCorner)
	{
		case 0:
			ttf.uv(MexVec2(0,0), 0);
			ttf.uv(MexVec2(1,0), 1);
			ttf.uv(MexVec2(1,1), 2);
			ttf.uv(MexVec2(0,1), 3);
			break;
		case 1:
			ttf.uv(MexVec2(0,0), 3);
			ttf.uv(MexVec2(1,0), 0);
			ttf.uv(MexVec2(1,1), 1);
			ttf.uv(MexVec2(0,1), 2);
			break;
		case 2:
			ttf.uv(MexVec2(0,0), 2);
			ttf.uv(MexVec2(1,0), 3);
			ttf.uv(MexVec2(1,1), 0);
			ttf.uv(MexVec2(0,1), 1);
			break;
		case 3:
			ttf.uv(MexVec2(0,0), 1);
			ttf.uv(MexVec2(1,0), 2);
			ttf.uv(MexVec2(1,1), 3);
			ttf.uv(MexVec2(0,1), 0);
			break;
	}

	++startCorner;
	startCorner %= 4;

	if(clockWise)
	{
		MexVec2 temp;

		temp = ttf.uv(0);
		ttf.uv(ttf.uv(1), 0);
		ttf.uv(temp, 1);

		temp = ttf.uv(2);
		ttf.uv(ttf.uv(3), 2);
		ttf.uv(temp, 3);
	}

	meshPtr->addTTFPolygon(ttf);
     //Add a mesh instance for it, with the fog distance as the visibility range
    RenMeshInstance* pMeshInstance = _NEW( RenMeshInstance( meshPtr ) );
    //add( pMeshInstance, _STATIC_CAST( double, RenDevice::current()->fogEnd() ), W4dLOD( 0 ) );
    //add( pMeshInstance, MexDouble( _STATIC_CAST( double, RenDevice::current()->fogEnd() ) ), W4dLOD( 0 ) );
    add( pMeshInstance, _STATIC_CAST( MexDouble, RenDevice::current()->fogEnd() ), W4dLOD( 0 ) );


    TEST_INVARIANT;
}

//public ctor
MachPhysFlameBall::MachPhysFlameBall(W4dEntity* pParent, const MexTransform3d& localTransform, FlameBallType type)
:MachPhysLinearProjectile( exemplar( type ), pParent, localTransform )
{
	// The current model is all black with emissive flames and things.  Hence,
	// it should not need lighting.  This could change if the model changes.
	doNotLight(true);

    //make invisible until required
    visible( false );

    TEST_INVARIANT;
}


MachPhysFlameBall::MachPhysFlameBall( PerConstructor con )
: MachPhysLinearProjectile( con )
{
}

MachPhysFlameBall::~MachPhysFlameBall()
{
    TEST_INVARIANT;

}

//static
const MachPhysFlameBall& MachPhysFlameBall::exemplar( FlameBallType type )
{
    //Use the one time constructor
    //static MachPhysFlameBall& bomb = *_NEW( MachPhysFlameBall( type ) );

    return MachPhysOtherPersistence::instance().flameBallExemplar( type );
}

void MachPhysFlameBall::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysFlameBall& t )
{

    o << "MachPhysFlameBall " << (void*)&t << " start" << std::endl;
    o << "MachPhysFlameBall " << (void*)&t << " end" << std::endl;

    return o;
}

/*
PhysRelativeTime MachPhysFlameBall::move
(
	const MexTransform3d& startTransform,
	const MexTransform3d& endTransform,
	const PhysAbsoluteTime& burstTime,
	const PhysRelativeTime& waitingTime,
	const PhysRelativeTime& truncateTime,
	MATHEX_SCALAR dip,
	MATHEX_SCALAR propotionEnd,
	MATHEX_SCALAR propotionMiddle,
	MATHEX_SCALAR flameBallSize
)
{

    //Construct and apply the transform plan
    W4dEntityPlan& entityPlan = entityPlanForEdit();

	//material plan
    const W4dMaterialPlanPtr& flameBallMaterialPlanPtr = MachPhysEffects::flameBallMaterialPlan();

    PhysRelativeTime duration = flameBallMaterialPlanPtr->duration();
	PhysRelativeTime t1 =  waitingTime;
	PhysRelativeTime dipTime = 0;
	PhysRelativeTime t2 = t1 + duration - truncateTime;

	PRE(t1 >= 0);
	PRE(t2 >= 0);

 	entityPlan.materialPlan( flameBallMaterialPlanPtr, burstTime+t1);

	//visibility plan
    W4dVisibilityPlanPtr flameBallVisibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	flameBallVisibilityPlanPtr->add(false, t2);

    entityPlan.visibilityPlan( flameBallVisibilityPlanPtr, burstTime );

	//calculate data for setting up travel  and scale plans
  	MexPoint3d startPosition = startTransform.position();
	MexPoint3d   endPosition = endTransform.position();
	MexPoint3d middlePosition;

	MATHEX_SCALAR xEnd = startPosition.x() + (endPosition.x() - startPosition.x() )*propotionEnd;
	MATHEX_SCALAR yEnd = startPosition.y() + (endPosition.y() - startPosition.y() )*propotionEnd;
	MATHEX_SCALAR zEnd = startPosition.z() + (endPosition.z() - startPosition.z() )*propotionEnd;

	MATHEX_SCALAR xMiddle = startPosition.x() + (xEnd - startPosition.x() )*propotionMiddle;
	MATHEX_SCALAR yMiddle = startPosition.y() + (yEnd - startPosition.y() )*propotionMiddle;
	MATHEX_SCALAR zMiddle = startPosition.z() + (zEnd - startPosition.z() )*propotionMiddle + dip;

   	endPosition = MexPoint3d(xEnd, yEnd, zEnd);
	middlePosition = MexPoint3d(xMiddle, yMiddle, zMiddle);

	dipTime =t1 + (t2 - t1) * propotionMiddle;
	POST((dipTime >= t1)&&(dipTime<=t2));

	//travel plan
   	PhysLinearMotionPlan  *pMotionPlan = _NEW( PhysLinearMotionPlan( MexTransform3d( startPosition ),
                          											 MexTransform3d( startPosition ), t1 ) );

 	pMotionPlan->add( MexTransform3d( middlePosition ), dipTime );
 	pMotionPlan->add( MexTransform3d( endPosition ), t2 );

   	PhysMotionPlanPtr flameMotionPlanPtr = pMotionPlan;
	entityPlan.absoluteMotion( flameMotionPlanPtr,  burstTime, 1);

	//scale plan
	MATHEX_SCALAR maxScale = (flameBallSize - 2.0*dip)/flameBallSize;
    PhysLinearScalarPlan::ScalarVec linearTimes;
    	linearTimes.reserve(3);
		linearTimes.push_back(t1);
		linearTimes.push_back(dipTime);
		linearTimes.push_back(t2);

    PhysLinearScalarPlan::ScalarVec scales;
    	scales.reserve(5);
        scales.push_back(1);
        scales.push_back(1);
        scales.push_back(maxScale);
        scales.push_back(1);

    PhysScalarPlanPtr flamePlanPtr( _NEW( PhysLinearScalarPlan(linearTimes, scales) ) );

	W4dScalePlanPtr flameScalePlanPtr( _NEW( W4dGeneralUniformScalePlan(flamePlanPtr) ) );
    entityPlan.scalePlan( flameScalePlanPtr, burstTime );

    //Store default flight path data
    MexPoint3d globalStartPosition( startPosition );
    MexPoint3d globalEndPosition( endPosition );
    const MexTransform3d& parentTransform = pParent()->globalTransform();
    parentTransform.transform( &globalStartPosition );
    parentTransform.transform( &globalEndPosition );

    flightStartTime( burstTime );
    flightDuration( duration );
    flightPath( MexLine3d( globalStartPosition, globalEndPosition ) );

	return duration;
 }
*/

PhysRelativeTime MachPhysFlameBall::move
(
	const PhysAbsoluteTime& launchTime,
	const MexTransform3d& startTransform,
	const MexTransform3d& endTransform,
	const PhysRelativeTime& materialPlanCycleStartDelay,
	MATHEX_SCALAR endScale
)
{
    //Get the plan object
    W4dEntityPlan& entityPlan = entityPlanForEdit();

	//Get the material plan and its duration, and apply it
    const W4dMaterialPlanPtr& flameBallMaterialPlanPtr = MachPhysEffects::flameBallMaterialPlan();
    PhysRelativeTime materialPlanDuration = flameBallMaterialPlanPtr->duration();

 	entityPlan.materialPlan( flameBallMaterialPlanPtr, launchTime );

	//travel plan
    PhysRelativeTime flightTime = materialPlanDuration - materialPlanCycleStartDelay;
    PhysAbsoluteTime flightBeginTime = launchTime + materialPlanCycleStartDelay;
   	PhysLinearMotionPlan* pMotionPlan =
   	    _NEW( PhysLinearMotionPlan( startTransform, endTransform, flightTime ) );

   	PhysMotionPlanPtr flameMotionPlanPtr = pMotionPlan;
	entityPlan.absoluteMotion( flameMotionPlanPtr, flightBeginTime );

	//scale plan
    W4dSimpleUniformScalePlan* pScalePlan =
        _NEW( W4dSimpleUniformScalePlan( 1.0, endScale, flightTime ) );

	W4dScalePlanPtr flameScalePlanPtr( pScalePlan );
    entityPlan.scalePlan( flameScalePlanPtr, flightBeginTime );

	//visibility plan
    W4dVisibilityPlanPtr flameBallVisibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	flameBallVisibilityPlanPtr->add(false, materialPlanDuration);

    entityPlan.visibilityPlan( flameBallVisibilityPlanPtr, launchTime );

    //Store default flight path data
    MexPoint3d globalStartPosition( startTransform.position() );
    MexPoint3d globalEndPosition( endTransform.position() );
    const MexTransform3d& parentTransform = pParent()->globalTransform();
    parentTransform.transform( &globalStartPosition );
    parentTransform.transform( &globalEndPosition );

    flightStartTime( flightBeginTime );
    flightDuration( flightTime );
    flightPath( MexLine3d( globalStartPosition, globalEndPosition ) );

	return flightTime;
}


void perWrite( PerOstream& ostr, const MachPhysFlameBall& flameBall )
{
    const MachPhysLinearProjectile& base = flameBall;

    ostr << base;
}

void perRead( PerIstream& istr, MachPhysFlameBall& flameBall )
{
    MachPhysLinearProjectile& base = flameBall;

    istr >> base;
}

/* End FLAMEBAL.CPP *************************************************/

