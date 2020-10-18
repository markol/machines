/*
 * V O R T B O M B . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/vortbomb.hpp"
#include "machphys/wsphere.hpp"
#include "machphys/vsphere.hpp"
#include "machphys/bsphere.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/wepdata.hpp"
#include "machphys/snddata.hpp"

#include "world4d/root.hpp"
#include "world4d/alphsimp.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/visplan.hpp"
#include "world4d/gusplan.hpp"
#include "world4d/uvplan.hpp"
#include "world4d/uvtrans.hpp"
#include "world4d/soundman.hpp"
#include "world4d/light.hpp"
#include "world4d/garbcoll.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/abox3d.hpp"
#include "mathex/angle.hpp"
#include "mathex/degrees.hpp"

#include "system/pathname.hpp"

#include "ctl/vector.hpp"

#include "phys/rampacce.hpp"
#include "phys/asclplan.hpp"
#include "phys/lsclplan.hpp"
#include "phys/motplan.hpp"
#include "phys/linemoti.hpp"

#include "render/device.hpp"
//#include "render/capable.hpp"
#include "render/colour.hpp"

PER_DEFINE_PERSISTENT( MachPhysVortexBomb );

MachPhysVortexBomb::MachPhysVortexBomb(W4dEntity* pParent, const MexTransform3d& localTransform)
:   W4dEntity( pParent, localTransform, W4dEntity::NOT_SOLID )
{
	// The current model is all emissive or black.  Hence, it should not need
	// lighting.  This could change if the model changes.
	doNotLight(true);

	pBlackSphere_ = _NEW( MachPhysBlackSphere( this, MexTransform3d() ) );
	pWhiteSphere_ = _NEW( MachPhysWhiteSphere( this, MexTransform3d() ) );
	pVortexSphere_ =_NEW( MachPhysVortexSphere(this, MexTransform3d() ) );

    //Ensure the bounding volumes of the spheres are set to the maximum, so they don't
    //get clipped out by the camera.
    //Should come from single fn - see sphereGrowRamp
    MATHEX_SCALAR vortRadius = 80.0;
    MexAlignedBox3d vortexBound( -vortRadius, -vortRadius, -vortRadius, vortRadius, vortRadius, vortRadius );
    pBlackSphere_->boundingVolume( vortexBound );
    pWhiteSphere_->boundingVolume( vortexBound );
    pVortexSphere_->boundingVolume( vortexBound );

    TEST_INVARIANT;
}

MachPhysVortexBomb::MachPhysVortexBomb( PerConstructor con )
: W4dEntity( con )
{
}

MachPhysVortexBomb::~MachPhysVortexBomb()
{
    TEST_INVARIANT;

    //Stop any playing sound
    W4dSoundManager::instance().stop( this );
}


void MachPhysVortexBomb::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysVortexBomb& t )
{

    o << "MachPhysVortexBomb " << (void*)&t << " start" << std::endl;
    o << "MachPhysVortexBomb " << (void*)&t << " end" << std::endl;

    return o;
}

//virtual
bool MachPhysVortexBomb::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

static const MATHEX_SCALAR defaultRadius()
{
	static const MATHEX_SCALAR RADIUS = 0.5;
	return RADIUS;
}

//non member function
static const PhysRampAcceleration& sphereGrowRamp()
{
	//at the end of this phase, speed = 0, radius = data.extras()[0]
	static const MachPhysWeaponData& data = MachPhysData::instance().weaponData(MachPhys::VORTEX);
	MATHEX_SCALAR vortRadius =  ( data.extras()[0] - 0.75 )/defaultRadius();

	static PhysRampAcceleration acc( 0, 5, 1, vortRadius*0.8 , 0, vortRadius*0.2, PhysRampAcceleration::BY_DISTANCE);
	return acc;
}

//static
bool MachPhysVortexBomb::radiusTime(const  MATHEX_SCALAR& distance, PhysRelativeTime* pTime)
{
	PRE(distance >=0);

	static const PhysRampAcceleration& acc = sphereGrowRamp();
	static const MATHEX_SCALAR acce1 = acc.firstAcceleration();
	static const MATHEX_SCALAR acce2 = acc.secondAcceleration();

	static const MATHEX_SCALAR dis1 = acc.distance1();
	static const MATHEX_SCALAR total = acc.totalDistance();
	static const MATHEX_SCALAR time1 = acc.firstAccelerationTime();
	static const MATHEX_SCALAR constSpeed = acc.constantSpeed();

	ASSERT( acc.distance2()	<= 0.0001, "" );

	const MATHEX_SCALAR dis = (distance-0.75)/defaultRadius();

    PhysRelativeTime time = 0;
	bool reachTarget = false;

	if( dis > total )
	{
		reachTarget = false;
		time = 0;
	}

	if(dis <= 0)
	{
		time = 0;
		reachTarget = true;
	}

	if(dis > 0 && dis <= dis1 )
	{
		time = sqrt( 2.0*dis/acce1 );
		ASSERT(time >= 0, "" );
		time += 2; //plus the 2 seconds of black sphere's invisible period
		reachTarget = true;
	}

	if(dis > dis1 && dis <= total  )
	{
		const MATHEX_SCALAR delta = sqrt( constSpeed*constSpeed + 2.0*(dis-dis1)*acce2 );

		time = (delta-constSpeed)/acce2;

		ASSERT(time >= 0, "" );
		time += 2; //plus the 2 seconds of black sphere's invisible period
		time += time1;

		reachTarget = true;
	}

    *pTime = time;

	return reachTarget;
}


//static
MATHEX_SCALAR MachPhysVortexBomb::radius(const  PhysRelativeTime& timeOffset )
{
	PRE(timeOffset >=0);

	const PhysRampAcceleration& acc = sphereGrowRamp();

   	return 0.75 + acc.distance( timeOffset )*defaultRadius();
}

PhysRelativeTime MachPhysVortexBomb::startExplosion( const PhysAbsoluteTime& startTime )
{
    PhysRelativeTime duration = 10.0;

// 	set  plans for the vortex sphere

	//scale plan
    PhysRampAcceleration rampAcc;
	ctl_vector<PhysRampAcceleration>  bRampAccelerations;

	//the black sphere expands from 0 to 0.75 in the first 2 seconds, (defaultSize = 0.5)
    rampAcc = PhysRampAcceleration( 0, 1, 1, 0.75, 0, 0.75, PhysRampAcceleration::BY_DISTANCE);
	bRampAccelerations.push_back(rampAcc);

	//the black sphere expansion is determined by sphereGrowRamp()
	bRampAccelerations.push_back(sphereGrowRamp());

    rampAcc = PhysRampAcceleration( 0, 1, 1, -40, 0, -39.9995, PhysRampAcceleration::BY_DISTANCE);
	bRampAccelerations.push_back(rampAcc);

	// an initial scale of 0.0 is applied.
    PhysScalarPlanPtr bPlanPtr( _NEW( PhysAcceleratedScalarPlan(bRampAccelerations, 0.0001) ) );

	W4dScalePlanPtr bScalePlanPtr( _NEW( W4dGeneralUniformScalePlan(bPlanPtr) ) );
    pVortexSphere_->entityPlanForEdit().scalePlan( bScalePlanPtr, startTime );

	const PhysRelativeTime splitTime = 8.0;
	//visibility plan
    W4dVisibilityPlanPtr bVisibilityPlanPtr( _NEW( W4dVisibilityPlan( false ) ) );

	bVisibilityPlanPtr->add(true, 2);
	bVisibilityPlanPtr->add(false, splitTime);	//in the last two seconds it is replaced by a pure black sphere
    pVortexSphere_->entityPlanForEdit().visibilityPlan( bVisibilityPlanPtr, startTime );

/*
	//motion plan
	PhysMotionPlan::Angles radians;

	radians.push_back( MexRadians( 0 ) );
	radians.push_back( MexRadians( MexDegrees( 120 ) ) );
	radians.push_back( MexRadians( MexDegrees( 240 ) ) );
 	radians.push_back( MexRadians( MexDegrees( 360 ) ) );

	PhysMotionPlan::AnglesPtr anglesPtr = _NEW( PhysMotionPlan::Angles(radians) );

	PhysMotionPlan::Times times;

	times.push_back(0.2);
	times.push_back(0.4);
	times.push_back(0.6);

	PhysMotionPlan::TimesPtr timesPtr = _NEW( PhysMotionPlan::Times(times) );

	PhysMotionPlanPtr bMotionPlanPtr( _NEW( PhysTimedAnglePlan( anglesPtr, timesPtr, MexVec3(0, 0, 1), MexVec3(0, 0, 0) ) ) );
	pVortexSphere_->entityPlanForEdit().absoluteMotion( bMotionPlanPtr,  startTime, 100);
*/

// 	the pure black spher eonly shrinks from the size of the vortex sphere at the 8th second	 to nothing in the last 2 seconds
    W4dVisibilityPlanPtr pureVisibilityPlanPtr( _NEW( W4dVisibilityPlan( false ) ) );

	pureVisibilityPlanPtr->add(true, splitTime);
	pureVisibilityPlanPtr->add(false, 10);	//only be visible in the last two seconds
    pBlackSphere_->entityPlanForEdit().visibilityPlan( pureVisibilityPlanPtr, startTime );

	//apply the vortex sphere's scale plan
	pBlackSphere_->entityPlanForEdit().scalePlan( bScalePlanPtr, startTime );

// 	set  plans for the white sphere

    W4dEntityPlan& whiteEntityPlan = pWhiteSphere_->entityPlanForEdit();

	ctl_vector<PhysRampAcceleration>  wRampAccelerations;

	//The white sphere contracts from r=7. to 0.75 in 2 seconds
    rampAcc = PhysRampAcceleration( 0, 1, 1, -6.25, 0, -6.25, PhysRampAcceleration::BY_DISTANCE);
	wRampAccelerations.push_back(rampAcc);

	static const MachPhysWeaponData& data = MachPhysData::instance().weaponData(MachPhys::VORTEX);
	MATHEX_SCALAR vortRadius =  data.extras()[0]/defaultRadius();
	//			  vortRadius -=	0.75/defaultRadius();
	//			  vortRadius *= 0.5;
    //rampAcc = PhysRampAcceleration( 0, 3, 3, vortRadius, 0, vortRadius, PhysRampAcceleration::BY_DISTANCE);
	//wRampAccelerations.push_back(rampAcc);
	wRampAccelerations.push_back(sphereGrowRamp());

    rampAcc = PhysRampAcceleration( 0, 1, 1, 30, 0, 30, PhysRampAcceleration::BY_DISTANCE);
	wRampAccelerations.push_back(rampAcc);

    PhysScalarPlanPtr wPlanPtr( _NEW( PhysAcceleratedScalarPlan(wRampAccelerations, 15.0) ) );

	W4dScalePlanPtr wScalePlanPtr( _NEW( W4dGeneralUniformScalePlan(wPlanPtr) ) );
    whiteEntityPlan.scalePlan( wScalePlanPtr, startTime );

	//visibility plan
    W4dVisibilityPlanPtr wVisibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );

	wVisibilityPlanPtr->add(false, 2);
	wVisibilityPlanPtr->add(true, 8);
	wVisibilityPlanPtr->add(false, 10);

    whiteEntityPlan.visibilityPlan( wVisibilityPlanPtr, startTime );

    //a simple alpha material plan

    PhysLinearScalarPlan::ScalarVec linearTimes;
    	linearTimes.reserve(3);
		linearTimes.push_back(2);
		linearTimes.push_back(8.2);
		linearTimes.push_back(10);

    PhysLinearScalarPlan::ScalarVec scales;
    	scales.reserve(4);
        scales.push_back(0);
        scales.push_back(0.5);
        scales.push_back(0.5);
        scales.push_back(0);

    PhysLinearScalarPlan* pAlphaPlan = _NEW( PhysLinearScalarPlan(linearTimes, scales) );
    PhysScalarPlanPtr alphaPlanPtr( pAlphaPlan );

	RenMaterial glowingWhite;
	glowingWhite.diffuse(RenColour::black());
	glowingWhite.emissive(RenColour::white());

    W4dSimpleAlphaPlan* pPlan =
                    _NEW( W4dSimpleAlphaPlan( glowingWhite, 1, alphaPlanPtr, 1 ) );

    W4dMaterialPlanPtr planPtr( pPlan );

    whiteEntityPlan.materialPlan( planPtr, startTime );

	//a white light which totally saturates an area just larger than the initial white sphere. it fades out before the main vortex starts
	W4dUniformLight* pLight = _NEW(W4dUniformLight(this, MexVec3(0, 0, 1), data.extras()[5]));
	pLight->colour(RenColour(6.0, 6.0, 6.0));
	pLight->constantAttenuation(0.1);
	pLight->linearAttenuation(0.8);
	pLight->quadraticAttenuation(0.1);
	pLight->scope(W4dLight::DYNAMIC);
	pLight->localTransform( MexPoint3d(0, 0, data.extras()[6] ) );

    W4dVisibilityPlanPtr visibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
	visibilityPlanPtr->add(false, 2);
    pLight->entityPlanForEdit().visibilityPlan( visibilityPlanPtr, startTime );


    PhysLinearScalarPlan::ScalarVec lightTimes;
    	lightTimes.reserve(1);
		lightTimes.push_back(data.extras()[9]);

    PhysLinearScalarPlan::ScalarVec intensities;
    	intensities.reserve(2);
        intensities.push_back(data.extras()[4]);
        intensities.push_back(0.1);

    PhysLinearScalarPlan* plightIntensityPlan = _NEW( PhysLinearScalarPlan(lightTimes, intensities) );

	PhysScalarPlanPtr intensityPlanPtr = plightIntensityPlan;
	pLight->intensityPlan(intensityPlanPtr, startTime);

    W4dGarbageCollector::instance().add( pLight, startTime + 2.0 );

	// main light like the nuke light
	const MATHEX_SCALAR lightRange = vortRadius;
	W4dUniformLight* pLight2 = _NEW(W4dUniformLight(this, MexVec3(0, 0, 1), lightRange));
	pLight2->colour(RenColour(data.extras()[1], data.extras()[2], data.extras()[3] ));
	pLight2->constantAttenuation(0.2);
	pLight2->linearAttenuation(0.8);
	pLight2->quadraticAttenuation(0.0);
	pLight2->scope(W4dLight::DYNAMIC);

	PhysRelativeTime ligh2Duration = duration;
    W4dVisibilityPlanPtr light2VisibilityPlanPtr( _NEW( W4dVisibilityPlan( false ) ) );
	light2VisibilityPlanPtr->add(true, 2.0);
	light2VisibilityPlanPtr->add(false, ligh2Duration);
    pLight2->entityPlanForEdit().visibilityPlan( light2VisibilityPlanPtr, startTime );


	PhysLinearMotionPlan* pLinePlan = _NEW( PhysLinearMotionPlan( MexTransform3d(MexPoint3d(0, 0, lightRange)),
	                                                             MexTransform3d(MexPoint3d(0, 0, 0.5*lightRange)),  8 ) );
	pLinePlan->add( MexTransform3d(MexPoint3d(0, 0, 0)), splitTime );

	PhysMotionPlanPtr motionPlanPtr = pLinePlan;

    pLight2->entityPlanForEdit().absoluteMotion( motionPlanPtr, startTime );


    PhysLinearScalarPlan::ScalarVec light2Times;
    	light2Times.reserve(4);
		light2Times.push_back(2);
		light2Times.push_back(splitTime-0.05);
		light2Times.push_back(splitTime);
		light2Times.push_back(ligh2Duration);

    PhysLinearScalarPlan::ScalarVec intensities2;
    	intensities2.reserve(5);
        intensities2.push_back(0.001);
        intensities2.push_back(0.01);
        intensities2.push_back(data.extras()[7]);
        intensities2.push_back(data.extras()[8]);
        intensities2.push_back(0.001);

    PhysLinearScalarPlan* pLight2IntensityPlan = _NEW( PhysLinearScalarPlan(light2Times, intensities2) );

	PhysScalarPlanPtr intensity2PlanPtr = pLight2IntensityPlan;
	pLight2->intensityPlan(intensity2PlanPtr, startTime);

    W4dGarbageCollector::instance().add( pLight2, startTime + ligh2Duration );

    //Play the sound
	W4dSoundManager::instance().play( this, SID_VORTEX, startTime, 1 );
//    W4dSoundManager::instance().play( this, SysPathName( "sounds/vortex.wav" ), startTime,
//                                      100.0, 100.0, W4dSoundManager::PLAY_ONCE );

    return duration; //lightEnd;
}

void perWrite( PerOstream& ostr, const MachPhysVortexBomb& bomb )
{
    const W4dEntity& base = bomb;

    ostr << base;
    ostr << bomb.pBlackSphere_;
    ostr << bomb.pWhiteSphere_;
    ostr << bomb.pVortexSphere_;
}

void perRead( PerIstream& istr, MachPhysVortexBomb& bomb )
{
    W4dEntity& base = bomb;

    istr >> base;
    istr >> bomb.pBlackSphere_;
    istr >> bomb.pWhiteSphere_;
    istr >> bomb.pVortexSphere_;
}


/* End VORTBOMB.CPP *************************************************/

