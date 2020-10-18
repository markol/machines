/*
 * C A M S H A K E . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "world4d/camshake.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/random.hpp"

#include "phys/linemoti.hpp"
#include "ctl/countptr.hpp"

#include "world4d/manager.hpp"

#ifndef _INLINE
    #include "world4d/camshake.ipp"
#endif

static PhysMotionPlanPtr shakePlanPtr(const uint& nPositions, const MATHEX_SCALAR& angle, const PhysRelativeTime& duration );

W4dCameraShake::W4dCameraShake()
:   scale_( FIRST_DEGREE )
{

    TEST_INVARIANT;
}

W4dCameraShake::~W4dCameraShake()
{
    TEST_INVARIANT;

}

void W4dCameraShake::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

void W4dCameraShake::epicCentre( const MexPoint3d& centre )
{
	epicCentre_ = centre;
}

void W4dCameraShake::scale( const W4dCameraShake::QuakeScale& scale )
{
	scale_ = scale;
}

void W4dCameraShake::startTime( const PhysAbsoluteTime& startTime )
{
	startTime_ = startTime;
}

void W4dCameraShake::duration( const PhysRelativeTime& duration )
{
	duration_ = duration;
}

void W4dCameraShake::offsetTransform( MexTransform3d* pTransform )
{
	PhysAbsoluteTime now = W4dManager::instance().time();
	PhysRelativeTime timeOffset = now - startTime_;

	if(timeOffset >= 0)
	{
		switch( scale_ )
		{
			case W4dCameraShake::FIRST_DEGREE:

				shakePlanPtr1()->transform( timeOffset, pTransform );
				break;

			case W4dCameraShake::SECOND_DEGREE:

				shakePlanPtr2()->transform( timeOffset, pTransform );
				break;

			case W4dCameraShake::THIRD_DEGREE:

				shakePlanPtr3()->transform( timeOffset, pTransform );
				break;

	        default:
	            ASSERT_BAD_CASE_INFO( scale_ );
	            break;
		}
	}
}

bool W4dCameraShake::shakeIsOver() const
{
	PhysRelativeTime timeOffset = W4dManager::instance().time() - startTime_;

	return timeOffset > duration_;
}

ostream& operator <<( ostream& o, const W4dCameraShake& t )
{

    o << "W4dCameraShake " << (void*)&t << " start" << std::endl;
    o << "W4dCameraShake " << (void*)&t << " end" << std::endl;

    return o;
}

const PhysMotionPlanPtr& W4dCameraShake::shakePlanPtr1() const
{
	PRE(scale_ == W4dCameraShake::FIRST_DEGREE);

    static int nPositions = 45;
    static MATHEX_SCALAR totalAngle =15.0;

	static const PhysMotionPlanPtr planPtr = shakePlanPtr(nPositions, totalAngle, duration_ );

 	return planPtr;
}

const PhysMotionPlanPtr& W4dCameraShake::shakePlanPtr2() const
{
	PRE(scale_ == W4dCameraShake::SECOND_DEGREE);

    static int nPositions = 37;
    static MATHEX_SCALAR totalAngle =10.0;

	static const PhysMotionPlanPtr planPtr = shakePlanPtr(nPositions, totalAngle, duration_ );

	return planPtr;
}

const PhysMotionPlanPtr& W4dCameraShake::shakePlanPtr3() const
{
	PRE(scale_ == W4dCameraShake::THIRD_DEGREE);

    static int nPositions = 32;
    static MATHEX_SCALAR totalAngle =5.5;

	static const PhysMotionPlanPtr planPtr = shakePlanPtr(nPositions, totalAngle, duration_ );

 	return planPtr;
}

static PhysMotionPlanPtr shakePlanPtr(const uint& nPositions, const MATHEX_SCALAR& angle, const PhysRelativeTime& duration )
{
	PRE( nPositions > 0 );
	static MexBasicRandom seed = MexBasicRandom::constructSeededFromTime();
    MexTransform3d rotationTransform[100];
    PhysRelativeTime interval = duration / (nPositions+1);

	MATHEX_SCALAR shakeAngle = angle;
	MATHEX_SCALAR reduceAngle = ( angle / (double)nPositions );

	for( size_t j = 0; j < nPositions; ++j )
	{
		MATHEX_SCALAR azi = shakeAngle * mexRandomScalar( &seed, -1.0, 1.0 );
		MATHEX_SCALAR ele = shakeAngle * mexRandomScalar( &seed, -1.0, 1.0 );
		MATHEX_SCALAR rol = shakeAngle * mexRandomScalar( &seed, -1.0, 1.0 );

		rotationTransform[j] = MexTransform3d( MexEulerAngles(MexDegrees(azi), MexDegrees(ele), MexDegrees(rol) ) );

		shakeAngle -= reduceAngle;
	}

	MexTransform3d unityTransform = MexTransform3d();
	PhysLinearMotionPlan* pShakePlan =  _NEW( PhysLinearMotionPlan( unityTransform,
			                                         rotationTransform[0],
			                                         interval ) );
	for(size_t j=1; j< nPositions; ++j)
	{
		pShakePlan->add( rotationTransform[j], interval * j );
	}

	pShakePlan->add( unityTransform, duration );

	PhysMotionPlanPtr planPtr( pShakePlan );

	return 	planPtr;
}

/* End CAMSHAKE.CPP *************************************************/
