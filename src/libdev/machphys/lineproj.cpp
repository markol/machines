/*
 * L I N E P R O J . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/lineproj.hpp"
#include "machphys/internal/lineprji.hpp"

#include "mathex/transf3d.hpp"
#include "phys/phys.hpp"
#include "phys/linemoti.hpp"
#include "phys/rampacce.hpp"
#include "world4d/visplan.hpp"
#include "world4d/soundman.hpp"
#include "world4d/entyplan.hpp"
#include "mathex/mathex.hpp"
#include "mathex/point3d.hpp"
#include "mathex/vec3.hpp"
#include "mathex/epsilon.hpp"
#include "mathex/transf3d.hpp"
#include "machphys/wepdata.hpp"

PER_DEFINE_PERSISTENT(  MachPhysLinearProjectile );

MachPhysLinearProjectile::MachPhysLinearProjectile
(
    W4dEntity* pParent, const MexTransform3d& localTransform
)
:   W4dComposite( pParent, localTransform, W4dEntity::NOT_SOLID ),
    pImpl_( _NEW( MachPhysLinearProjectileImpl() ) )
{
    //Set invisible so it doesn't appear until required
    visible( false );

    //Initialise the flight path using current position
    MexPoint3d startPos = globalTransform().position();
    pImpl_->flightPath_ = MexLine3d( startPos, startPos );

    TEST_INVARIANT;
}

MachPhysLinearProjectile::MachPhysLinearProjectile
(
    const MachPhysLinearProjectile& copyMe,
    W4dEntity* pParent, const W4dTransform3d& localTransform
)
:   W4dComposite( copyMe, pParent, localTransform ),
    pImpl_( _NEW( MachPhysLinearProjectileImpl( *copyMe.pImpl_ ) ) )
{
	sharedCopyCtor();
}

MachPhysLinearProjectile::MachPhysLinearProjectile
(
    const MachPhysLinearProjectile& copyMe,
    W4dEntity* pParent, const W4dTransform3d& localTransform,
	CopyLights copyLights
)
:   W4dComposite( copyMe, pParent, localTransform, copyLights ),
    pImpl_( _NEW( MachPhysLinearProjectileImpl( *copyMe.pImpl_ ) ) )
{
	sharedCopyCtor();

	// If we were asked not to copy any composite lights, set a flag.
	if (copyLights == DONT_COPY_LIGHTS)
		pImpl_->lightsSuppressed_ = true;
}

void MachPhysLinearProjectile::sharedCopyCtor()
{
    //Set invisible so it doesn't appear until required
    visible( false );

    //Set flag indicating it will move
    stationary( false );

    //Initialise the flight path using current position
    MexPoint3d startPos = globalTransform().position();
    pImpl_->flightPath_ = MexLine3d( startPos, startPos );

    TEST_INVARIANT;
}

MachPhysLinearProjectile::~MachPhysLinearProjectile()
{
    TEST_INVARIANT;

    //Stop any playing sound
    W4dSoundManager::instance().stop( this );

	_DELETE( pImpl_ );
}

void MachPhysLinearProjectile::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysLinearProjectile& t )
{

    o << "MachPhysLinearProjectile " << (void*)&t << " start" << std::endl;
    o << "MachPhysLinearProjectile " << (void*)&t << " end" << std::endl;

    return o;
}

PhysRelativeTime MachPhysLinearProjectile::move
(
    const PhysAbsoluteTime& startTime, const MachPhysWeaponData& data
)
{
    //Get the relevant data
    MATHEX_SCALAR distance = data.range();
    MATHEX_SCALAR speed = data.projectileSpeed();

    //Compute end position
    MexTransform3d relativeMotion( MexPoint3d( distance, 0.0, 0.0 ) );
    const MexTransform3d& startTransform = localTransform();
    MexTransform3d endTransform;
    startTransform.transform( relativeMotion, &endTransform );

    //Compute flight duration
    PhysRelativeTime duration = distance / speed;

    //Construct and apply the transform plan
    PhysLinearMotionPlan* pPlan =
        _NEW( PhysLinearMotionPlan( startTransform, endTransform, duration ) );

    PhysMotionPlanPtr planPtr( pPlan );

    W4dEntityPlan& entityPlan = entityPlanForEdit();
    entityPlan.absoluteMotion( planPtr, startTime );

    //Construct and apply a visibility plan to switch it on at startTime
    //and off at end time
    W4dVisibilityPlanPtr visibilityPlanPtr( _NEW( W4dVisibilityPlan( true ) ) );
    visibilityPlanPtr->add( false, duration );

    entityPlan.visibilityPlan( visibilityPlanPtr, startTime );

    //Store default flight path data
    MexPoint3d startPosition = startTransform.position();
    MexPoint3d endPosition = endTransform.position();

    MexPoint3d globalStartPosition( startPosition );
    MexPoint3d globalEndPosition( endPosition );
    const MexTransform3d& parentTransform = pParent()->globalTransform();
    parentTransform.transform( &globalStartPosition );
    parentTransform.transform( &globalEndPosition );

    flightStartTime( startTime );
    flightDuration( duration );
    flightPath( MexLine3d( globalStartPosition, globalEndPosition ) );

    //Make the subclass call to apply any extra bits.
    doMove( startPosition, endPosition, duration );

    return duration;
}

PhysRelativeTime MachPhysLinearProjectile::beDestroyedAt
(
    const PhysAbsoluteTime& time, MachPhys::StrikeType strikeType
)
{
    //Construct and apply the visibility plan switching the projectile off
    W4dVisibilityPlanPtr visibilityPlanPtr( _NEW( W4dVisibilityPlan( false ) ) );
    entityPlanForEdit().visibilityPlan( visibilityPlanPtr, time );

    return doBeDestroyedAt( time, strikeType );
}

//virtual
void MachPhysLinearProjectile::doMove
(
    const MexPoint3d&, const MexPoint3d&, const PhysRelativeTime&
)
{
    //Do nothing
}

//virtual
PhysRelativeTime MachPhysLinearProjectile::doBeDestroyedAt
(
    const PhysAbsoluteTime&, MachPhys::StrikeType
)
{
    //Do nothing taking no time
    return 0.0;
}

//virtual
bool MachPhysLinearProjectile::intersectsLine( const MexLine3d&, MATHEX_SCALAR*, Accuracy ) const
{
    return false;
}

const PhysAbsoluteTime& MachPhysLinearProjectile::flightStartTime() const
{
    _CONST_CAST( MachPhysLinearProjectile*, this)->updateFlightData();
    return pImpl_->flightStartTime_;
}

const PhysRelativeTime& MachPhysLinearProjectile::flightDuration() const
{
    _CONST_CAST( MachPhysLinearProjectile*, this)->updateFlightData();
    return pImpl_->flightDuration_;
}

const MexLine3d& MachPhysLinearProjectile::flightPath() const
{
    _CONST_CAST( MachPhysLinearProjectile*, this)->updateFlightData();
    return pImpl_->flightPath_;
}

void MachPhysLinearProjectile::flightStartTime( const PhysAbsoluteTime& startTime )
{
    pImpl_->flightStartTime_ = startTime;
}

void MachPhysLinearProjectile::flightDuration( const PhysRelativeTime& duration )
{
    pImpl_->flightDuration_ = duration;
}

void MachPhysLinearProjectile::flightPath( const MexLine3d& path )
{
    pImpl_->flightPath_ = path;
}

//virtual
void MachPhysLinearProjectile::updateFlightData()
{
    //Intentionally empty
}

MachPhysLinearProjectile::MachPhysLinearProjectile( PerConstructor con )
: W4dComposite( con )
{
}

void perWrite( PerOstream& ostr, const MachPhysLinearProjectile& projectile )
{
    const W4dComposite& base = projectile;

    ostr << base;
    ostr << projectile.pImpl_;
}

void perRead( PerIstream& istr, MachPhysLinearProjectile& projectile )
{
    W4dComposite& base = projectile;

    istr >> base;
    istr >> projectile.pImpl_;
}

//static
bool MachPhysLinearProjectile::impactData
(
    W4dEntity& target,
    const MexLine3d& fromDirection,
    MexPoint3d* pImpactPoint,
    MexVec3* pUnitDirection
)
{
    PRE( fromDirection.length() != 0.0 );

    //Check for the intersection between the direction line, and the machine point.
    MATHEX_SCALAR distance;
    bool result = target.intersectsLine( fromDirection, &distance, W4dEntity::HIGH );
    if( result )
    {
        //Get the line length. Hence compute the actual point of impact, and
        //unit direction vector back in the direction of bolter flight.
        MATHEX_SCALAR length = fromDirection.length();
        *pUnitDirection = MexVec3( fromDirection.end1(), fromDirection.end2(), length );
        *pImpactPoint = fromDirection.pointAtDistance( distance );
        (*pUnitDirection) *= -1.0;

        //Convert the point and direction into local coordinates relative to the machine
        MexTransform3d inverseTransform = target.globalTransform();
        inverseTransform.invert();

        inverseTransform.transform( pImpactPoint );
        inverseTransform.transform( pUnitDirection );
    }

    return result;
}

void MachPhysLinearProjectile::planetSurface( MachPhysPlanetSurface* pSurface )
{
    CB_DEPIMPL( MachPhysPlanetSurface*, pPlanetSurface_ );
    pPlanetSurface_ = pSurface;
}

bool MachPhysLinearProjectile::hasPlanetSurface() const
{
    CB_DEPIMPL( MachPhysPlanetSurface*, pPlanetSurface_ );
    return pPlanetSurface_ != NULL;
}

MachPhysPlanetSurface& MachPhysLinearProjectile::planetSurface() const
{
    PRE( hasPlanetSurface() );
    CB_DEPIMPL( MachPhysPlanetSurface*, pPlanetSurface_ );
    return *pPlanetSurface_;
}

MexPoint3d MachPhysLinearProjectile::flightPosition( const PhysAbsoluteTime& time ) const
{
    //get linear path data
    PhysAbsoluteTime startTime = flightStartTime();
    PhysRelativeTime duration = flightDuration();
    const MexLine3d& path = flightPath();

    //Work out how far along the path time represents
    PhysRelativeTime timeIntoFlight = time - startTime;
    const MexPoint3d& startPosition = path.end1();
    const MexPoint3d& endPosition = path.end2();
    MexPoint3d position;
    if( time <= 0.0 )
        position = startPosition;
    else if( time >= duration )
        position = endPosition;
    else
    {
        MATHEX_SCALAR f = timeIntoFlight / duration;
        position.x( startPosition.x() * (1.0 - f) + endPosition.x() * f );
        position.y( startPosition.y() * (1.0 - f) + endPosition.y() * f );
        position.z( startPosition.z() * (1.0 - f) + endPosition.z() * f );
    }

    return position;
}

bool MachPhysLinearProjectile::lightsSuppressed() const
{
	return pImpl_->lightsSuppressed_;
}

void MachPhysLinearProjectile::setLauncherControl(MachPhysMachine::ControlType newControlType)
{
    CB_DEPIMPL( MachPhysMachine::ControlType, launchControlType_ );
	launchControlType_ = newControlType;
}

MachPhysMachine::ControlType MachPhysLinearProjectile::getLauncherControl() const
{
    CB_DEPIMPL( MachPhysMachine::ControlType, launchControlType_ );
	return launchControlType_;
}

/* End LINEPROJ.CPP *************************************************/
