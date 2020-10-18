/*
 * L O C O M O T I . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/radians.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"
#include "phys/rampacce.hpp"

#include "machphys/locomoti.hpp"
#include "machphys/internal/locomoim.hpp"
#include "machphys/machine.hpp"

#include "mathex/utility.hpp"
#include "mathex/vec2.hpp"
#include "mathex/vec3.hpp"
#include "mathex/utility.hpp"
#include "mathex/quatern.hpp"

#include "ctl/vector.hpp"
#include "ctl/countptr.hpp"

#include "world4d/entity.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/link.hpp"

#include "phys/linemoti.hpp"
#include "phys/motplan.hpp"

#include "sim/manager.hpp"

PER_DEFINE_PERSISTENT_ABSTRACT( MachPhysLocomotionMethod );

MachPhysLocomotionMethod::MachPhysLocomotionMethod( MachPhysLocomotionMethodImpl* pImpl )
:   pImpl_( pImpl )
{

    TEST_INVARIANT;
}

MachPhysLocomotionMethod::MachPhysLocomotionMethod( PerConstructor )
: pImpl_( NULL )
{
}

MachPhysLocomotionMethod::~MachPhysLocomotionMethod()
{
    TEST_INVARIANT;

    _DELETE( pImpl_ );
}

// virtual
void MachPhysLocomotionMethod::reviseProfile( TransformsPtr )
{
}

// virtual
void MachPhysLocomotionMethod::calculateAccelerations(
    const TransformsPtr& transformsPtr,
    MATHEX_SCALAR startSpeed,
    MATHEX_SCALAR capTranslationSpeed,
    MachPhys::FinalState finalState,
    RampAccelerationsPtr rampAccelerationsPtr ) const
{
    PRE( transformsPtr.isDefined() );
    PRE( rampAccelerationsPtr.isDefined() );
    PRE( transformsPtr->size() >= 2 );
    PRE( rampAccelerationsPtr->size() == 0 );
    PRE( MexUtility::inStraightLineXY( *transformsPtr ) );

    doCalculateAccelerations( transformsPtr, startSpeed, capTranslationSpeed, finalState, rampAccelerationsPtr );

    POST( rampAccelerationsPtr->size() == transformsPtr->size() - 1 );
}

void MachPhysLocomotionMethod::doCalculateAccelerations(
    const TransformsPtr& transformsPtr,
    MATHEX_SCALAR startSpeed,
    MATHEX_SCALAR capTranslationSpeed,
    MachPhys::FinalState finalState,
    RampAccelerationsPtr rampAccelerationsPtr ) const
{
    PRE( transformsPtr->size() >= 2 );
    PRE( rampAccelerationsPtr->size() == 0 );
    PRE( MexUtility::inStraightLineXY( *transformsPtr ) );

    const MATHEX_SCALAR translationSpeed = std::min( capTranslationSpeed, pMachine()->maxTranslationSpeed() );
    const MATHEX_SCALAR acceleration = pMachine()->maxTranslationAcceleration();
    const MATHEX_SCALAR deceleration = pMachine()->maxTranslationAcceleration();

    MATHEX_SCALAR   currentSpeed = startSpeed;

    for( size_t i = 0; i < transformsPtr->size() - 1; ++i )
    {
        const MexTransform3d& tx1 = (*transformsPtr)[ i ];
        const MexTransform3d& tx2 = (*transformsPtr)[ i + 1 ];
        const bool  lastSegment = ( i == transformsPtr->size() - 2 );
        const bool  firstSegment = ( i == 0 );

        const MATHEX_SCALAR distance = tx1.position().euclidianDistance( tx2.position() );

        if( distance < MexEpsilon::instance() )
        {
            ASSERT_INFO( currentSpeed );
            ASSERT( currentSpeed == 0.0, "" );

            //  This movement is pure rotation

            const MexRadians rotationAcceleration = pMachine()->maxRotationAcceleration();

            const MexRadians turnAngle = tx1.xBasis().signedAngleBetween( tx2.xBasis() );

            MexRadians rotationSpeed = pMachine()->maxRotationSpeed();

            if( turnAngle.asScalar() < 0.0 )
                rotationSpeed = -rotationSpeed;

            const MATHEX_SCALAR startSpeed = 0.0;
            const MATHEX_SCALAR endSpeed = 0.0;


            PhysRampAcceleration rampAcceleration(
              turnAngle.asScalar(), startSpeed, rotationSpeed.asScalar(), endSpeed,
              rotationAcceleration.asScalar(), rotationAcceleration.asScalar(),
              PhysRampAcceleration::BY_SPEED );

            rampAccelerationsPtr->push_back( rampAcceleration );
            currentSpeed = 0.0;
        }
        else
        {
            //  This movement is a translation

            MATHEX_SCALAR endSpeed = translationSpeed;

            if( lastSegment and finalState == MachPhys::AT_REST )
                endSpeed = 0.0;

            PhysRampAcceleration    rampAcceleration(
              distance,
              currentSpeed,
              translationSpeed,
              endSpeed,
              acceleration,
              deceleration,
              PhysRampAcceleration::BY_SPEED );

            rampAccelerationsPtr->push_back( rampAcceleration );
            currentSpeed = rampAcceleration.endSpeed();
        }
    }

    POST( rampAccelerationsPtr->size() == transformsPtr->size() - 1 );

    POST( implies( finalState == MachPhys::AT_REST,
      MexEpsilon::isWithinEpsilonOf( rampAccelerationsPtr->back().endSpeed(), 0.0 ) ) );
}

void MachPhysLocomotionMethod::forceUpright(
    TransformsPtr transformsPtr )
{
    for( Transforms::iterator i = transformsPtr->begin(); i != transformsPtr->end(); ++i )
    {
        MexVec3 upVector( 0.0, 0.0, 1.0 );

        MexTransform3d tx( MexTransform3d::Z_ZX, upVector, (*i).xBasis(), MexPoint3d() );

        (*i).rotation( tx );
    }
}

MachPhysMachine* MachPhysLocomotionMethod::pMachine() const
{
    return &pImpl_->machine();
}

void MachPhysLocomotionMethod::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysLocomotionMethod& t )
{

    o << "MachPhysLocomotionMethod " << (void*)&t << " start" << std::endl;
    o << "MachPhysLocomotionMethod " << (void*)&t << " end" << std::endl;

    return o;
}

void perWrite( PerOstream& ostr, const MachPhysLocomotionMethod& locomotion )
{
    ostr << locomotion.pImpl_;
}

void perRead( PerIstream& istr, MachPhysLocomotionMethod& locomotion )
{
    istr >> locomotion.pImpl_;
}

// //virtual
// PhysRelativeTime MachPhysLocomotionMethod::reviseTranslationProfile(
// const MATHEX_SCALAR& distance,
// const MATHEX_SCALAR& startSpeed,
// const MATHEX_SCALAR& endSpeed,
// MATHEX_SCALAR* pTranslationSpeed,
// MATHEX_SCALAR* pAcceleration,
// MATHEX_SCALAR* pAccelerationTime,
// MATHEX_SCALAR* pDecelerationTime
// )
// {
//     return  PhysRampAcceleration::totalTime( distance, startSpeed, *pTranslationSpeed,
//                                          endSpeed, *pAcceleration,
//                                          *pAcceleration,
//                                          pAccelerationTime, pDecelerationTime );
//
// }
//
// //virtual
// PhysRelativeTime MachPhysLocomotionMethod::reviseRotationProfile(const MATHEX_SCALAR& turnAngle,
// 								  MATHEX_SCALAR* rotationSpeed,
//                                   MATHEX_SCALAR* pRotationAcceleration,
//                                   MATHEX_SCALAR* pAccelerationTime)
// {
// 	return PhysRampAcceleration::totalTime( 0, turnAngle, *rotationSpeed,
//                                                     *pRotationAcceleration,
//                                                     pAccelerationTime );
// }

void MachPhysLocomotionMethod::machineSurfacePosition
(
    const MachPhysPlanetSurface& surface, const MexPoint3d& baseLocation, const MexVec3& unitFacingDirection,
    MexPoint3d* pLocation, MexVec3* pNormal
) const
{
    pImpl_->machineSurfacePosition( surface, baseLocation, unitFacingDirection, pLocation, pNormal );
}

void MachPhysLocomotionMethod::firstPersonMotionAnimations( MachPhysLocomotionMethod::FirstPersonMotionState state )
{
    //Use the virtual method in the implementation hierarchy
    pImpl_->firstPersonMotionAnimations( state );

    //Update the stored last state
    pImpl_->lastState( state );
}

//static
void  MachPhysLocomotionMethod::pitchOrRollAnimation(W4dLink* pEntity, const MexRadians& angle, const MexVec3& axis, const PhysRelativeTime& duration )
{
    ctl_vector< MexTransform3d >  transforms;
    PhysMotionPlan::Times   times;

	uint nTimes = 3;
	transforms.reserve( 1 + nTimes );
	times.reserve( nTimes );

 	const PhysRelativeTime interval = duration/nTimes;

	for (uint i=0; i<nTimes; ++i)
	{
		PhysRelativeTime evenTime = interval*(i+1);
		times.push_back( evenTime );
	}

	MexPoint3d position = pEntity->localTransform().position();

	transforms.push_back( pEntity->localTransform() );
	transforms.push_back( transform( -angle, axis, position) );
	transforms.push_back( transform( angle, axis, position) );
	transforms.push_back( transform( 0, axis, position) );

    PhysLinearMotionPlan* pPlan =
        _NEW( PhysLinearMotionPlan( transforms[0], transforms[1],  times[0] ) );

	for (uint i=1; i<nTimes; ++i)
	{
		pPlan->add( transforms[i+1], times[i] );
	}

    PhysMotionPlanPtr planPtr( pPlan );

    pEntity->entityPlanForEdit().absoluteMotion( planPtr, SimManager::instance().currentTime());
}

//static
MexTransform3d MachPhysLocomotionMethod::transform(const MexRadians& angle, const MexVec3& axis, const MexPoint3d& position )
{
	MexTransform3d result;
    MexQuaternion params;

    params.set( axis, angle );

    result.position( position );
    result.rotation( params );

	return result;
}

void MachPhysLocomotionMethod::isRemotelyControlledFirstPerson( bool is )
{
    pImpl_->remoteFirstPerson_ = is;
}

bool MachPhysLocomotionMethod::isRemotelyControlledFirstPerson() const
{
    return pImpl_->remoteFirstPerson_;
}

/* End LOCOMOTI.CPP *************************************************/
