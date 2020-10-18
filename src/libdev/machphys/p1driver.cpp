/*
 * P 1 D R I V E R . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "machphys/p1driver.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/line3d.hpp"
#include "mathex/degrees.hpp"
#include "mathex/eulerang.hpp"
#include "phys/rampacce.hpp"
#include "phys/motplan.hpp"
#include "world4d/manager.hpp"
#include "world4d/scenemgr.hpp"
#include "world4d/camera.hpp"
#include "world4d/generic.hpp"
#include "world4d/entity.hpp"
#include "world4d/entyplan.hpp"
#include "world4d/domain.hpp"
#include "world4d/axisturn.hpp"
#include "machphys/machphys.hpp"
#include "machphys/attack.hpp"
#include "machphys/mphydata.hpp"
#include "machphys/genedata.hpp"
#include "machphys/weapon.hpp"
#include "machphys/wepdata.hpp"

#include "ctl/countptr.hpp"
#include "profiler/profiler.hpp"

//data class
class MachPhys1stPersonDriverImpl
{
public:
    W4dAxisTurnerPlan* pAxisTurnerPlan_; //Turner plan for looking up/down
    PhysMotionPlanPtr axisTurnerPlanPtr_; //Prevents deletion
    W4dEntity* pAntiRollEntity_; //Owns the camera and rolls to level out the view
    MexTransform3d cameraBaseTransform_; //Base transform for the camera from its parent
    bool turnAtFastRate_; //true if to turn whole machine at the faster rate
	MexTransform3dKey lastAntiRollParentTransformKey_; // determines whether the antiRollEntity transform needs update
	MexDegrees lastRollAngle_;
	MexDegrees lastElevationAngle_;
    W4dEntityFilter* pAimDataFilter_; //Used on aim data findNearerEntity check.
};

MachPhys1stPersonDriver::MachPhys1stPersonDriver
(
    W4dEntity* pEntity, MachPhysCanAttack* pCanAttack,
    bool remoteNode
)
:   pEntity_( pEntity ),
    pCanAttack_( pCanAttack ),
    movingForwards_( false ),
    movingBackwards_( false ),
    turningLeft_( false ),
    turningRight_( false ),
    maxWeaponRange_( 0.0 ),
    pCameraEntity_( NULL ),
    pTrackEntity_( NULL ),
    remoteNode_( remoteNode ),
    pHitEntity_( NULL ),
    hitDistance_( 20.0 ),
    lastUpdateTime_( W4dManager::instance().time() ),
    pImpl_( NULL )
{
    pImpl_ = _NEW( MachPhys1stPersonDriverImpl );
    pImpl_->pAxisTurnerPlan_ = NULL;
    pImpl_->turnAtFastRate_ = true;
    pImpl_->pAntiRollEntity_ = NULL;
    pImpl_->pAimDataFilter_ = NULL;

    TEST_INVARIANT;
}

MachPhys1stPersonDriver::~MachPhys1stPersonDriver()
{
    TEST_INVARIANT;

    //Stop tracking any target entity
    if( pCanAttack_ )
        pCanAttack_->stopTrackingTarget();

    if( pCameraEntity_ )
        pCameraEntity_->entityPlanForEdit().clearMotionPlans();

    _DELETE( pTrackEntity_ );

    if( remoteNode_ )
    {
        _DELETE( pCameraEntity_ );
    }
    else
    {
        //If the camera is still attached to the anti-roll entity, attach it to its grandparent,
        //so we can delete the anti-roll entity without inadvertently deleting the camera.
        if( pCameraEntity_->pParent() == pImpl_->pAntiRollEntity_ )
            pCameraEntity_->attachTo( pImpl_->pAntiRollEntity_->pParent() );
    }

    _DELETE( pImpl_->pAntiRollEntity_ );

    _DELETE( pImpl_ );
}

void MachPhys1stPersonDriver::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
    INVARIANT( not( movingForwards_ and movingBackwards_) );
    INVARIANT( not( turningLeft_ and turningRight_) );
}

ostream& operator <<( ostream& o, const MachPhys1stPersonDriver& t )
{

    o << "MachPhys1stPersonDriver " << (void*)&t << " start" << std::endl;
    o << "MachPhys1stPersonDriver " << (void*)&t << " end" << std::endl;

    return o;
}

void MachPhys1stPersonDriver::moveForwards()
{
    movingForwards_ = true;
    movingBackwards_ = false;
}

void MachPhys1stPersonDriver::moveBackwards()
{
    movingForwards_ = false;
    movingBackwards_ = true;
}

void MachPhys1stPersonDriver::stopMoving()
{
    movingForwards_ = false;
    movingBackwards_ = false;
}

void MachPhys1stPersonDriver::turnLeft()
{
    turningLeft_ = true;
    turningRight_ = false;
}

void MachPhys1stPersonDriver::turnRight()
{
    turningLeft_ = false;
    turningRight_ = true;
}

void MachPhys1stPersonDriver::stopTurning()
{
    turningLeft_ = false;
    turningRight_ = false;
}

bool MachPhys1stPersonDriver::isMovingForwards() const
{
    return movingForwards_;
}

bool MachPhys1stPersonDriver::isMovingBackwards() const
{
    return movingBackwards_;
}

bool MachPhys1stPersonDriver::isMoving() const
{
    return movingForwards_ or movingBackwards_;
}

bool MachPhys1stPersonDriver::isTurningLeft() const
{
    return turningLeft_;
}

bool MachPhys1stPersonDriver::isTurningRight() const
{
    return turningRight_;
}

bool MachPhys1stPersonDriver::isTurning() const
{
    return turningLeft_ or turningRight_;
}

bool MachPhys1stPersonDriver::isStationary() const
{
    return not( isTurning() or isMoving() );
}

void MachPhys1stPersonDriver::maxWeaponRange( MATHEX_SCALAR range )
{
    maxWeaponRange_ = range;
}

MATHEX_SCALAR MachPhys1stPersonDriver::maxWeaponRange() const
{
    return maxWeaponRange_;
}

void MachPhys1stPersonDriver::setupTrackEntity()
{
    PRE( pCameraEntity_ == NULL );

    //Get the camera attachment entity and offset transform
    MexTransform3d offsetTransform;
    W4dEntity& cameraParent = cameraAttachment( &offsetTransform );
    pImpl_->cameraBaseTransform_ = offsetTransform;

    //Create an entity whose transform will be adjusted every time the machine is moved to maintain
    //a level view. This entity will own the camera entity.
    W4dEntity* pAntiRollEntity = pImpl_->pAntiRollEntity_ =
        _NEW( W4dGeneric( &cameraParent, offsetTransform, W4dEntity::NOT_SOLID ) );

    //If a remote node we need a dummy camera entity. Otherwise use current camera
    if( remoteNode_ )
    {
        //Create dummy entity
        pCameraEntity_ = _NEW( W4dGeneric( pAntiRollEntity, MexTransform3d(), W4dEntity::NOT_SOLID ) );
    }
    else
    {
        //Get the current camera
        pCameraEntity_ = W4dManager::instance().sceneManager()->currentCamera();

        //Attach it to the anti-roll entity with null transform
        pCameraEntity_->attachTo( pAntiRollEntity, MexTransform3d() );
    }

    //Set up a turner plan for the camera
    PhysRelativeTime duration = 31536000.0; //One year
    PhysAbsoluteTime now = W4dManager::instance().time();
    pImpl_->pAxisTurnerPlan_ =
        _NEW( W4dAxisTurnerPlan( pCameraEntity_->localTransform(), W4d::Y_AXIS, now,
                                 0.0, 0.0, MachPhysData::instance().generalData().firstPersonLookUpDownRate(), duration ) );
    pImpl_->axisTurnerPlanPtr_ = pImpl_->pAxisTurnerPlan_;

    pCameraEntity_->entityPlanForEdit().absoluteMotion( pImpl_->axisTurnerPlanPtr_, now );

    //Set up tracking entity if can attack
    if( pCanAttack_ )
    {
        //Construct an entity for the weapons to track at the target position
        pTrackEntity_ = _NEW( W4dGeneric( pCameraEntity_, MexTransform3d( MexPoint3d( maxWeaponRange_, 0.0, 0.0 ) ),
                              W4dEntity::NOT_SOLID ) );

        //Set up the target tracking
        pCanAttack_->trackTargetWithWeapons( *pTrackEntity_ );
    }

    //Level out the camera
    adjustCameraRoll();
}

void MachPhys1stPersonDriver::update()
{
    if( pCanAttack_ )
    {
        //Update the aim data
        aimData();

        //Update the track entity position
        setTrackEntityPosition();
    }

    //Update the update time
    lastUpdateTime_ = W4dManager::instance().time();
}

void MachPhys1stPersonDriver::aimData()
{
    PRE( pImpl_->pAimDataFilter_ != NULL );
    pHitEntity_ = NULL;

    //Need the current camera
    W4dCamera& camera = *W4dManager::instance().sceneManager()->currentCamera();

    //Construct a 3d line along the line of sight, given current maximum weapon range.
    MATHEX_SCALAR lineLength = maxWeaponRange();
    MexPoint3d farPoint( lineLength, 0.0, 0.0 );
    const MexTransform3d& cameraTransform = camera.globalTransform();
    cameraTransform.transform( &farPoint );

    //We should be able to use the lineLength from above. However, due to rounding errors,
    //the MexLine3d ctor below fails an assertion if we do.
    lineLength = cameraTransform.position().euclidianDistance( farPoint );
    MexLine3d lineOfSight( cameraTransform.position(), farPoint, lineLength );

    //Find what this line intersects.
    ulong checkId = W4dEntity::nextCheckId();
    pEntity_->checkId( checkId );

    pHitEntity_ = NULL;
    hitDistance_ = lineLength;
    camera.containingDomain()->findNearerEntity( lineOfSight, lineLength, checkId, W4dEntity::HIGH,
                                                 &pHitEntity_, &hitDistance_, pImpl_->pAimDataFilter_ );

    //Update the hit point
    hitPoint_ = lineOfSight.pointAtDistance( hitDistance_ );
}

MATHEX_SCALAR MachPhys1stPersonDriver::hitDistance() const
{
    return hitDistance_;
}

bool MachPhys1stPersonDriver::hasHitEntity() const
{
    return pHitEntity_ != NULL;
}

W4dEntity& MachPhys1stPersonDriver::hitEntity() const
{
    PRE( hasHitEntity() );
    return *pHitEntity_;
}

const MexPoint3d& MachPhys1stPersonDriver::hitPoint() const
{
    return hitPoint_;
}

void MachPhys1stPersonDriver::setTrackEntityPosition()
{
    PRE( pTrackEntity_ != NULL );

    //Use the last computed hit distance along the x axis from parent entity
    pTrackEntity_->localTransform( MexTransform3d( MexPoint3d( hitDistance_, 0.0, 0.0 ) ) );
}

void MachPhys1stPersonDriver::lookDown( MexRadians byAngle )
{
    PRE( pImpl_->pAxisTurnerPlan_ != NULL );

    //Get the plan and current angle
    W4dAxisTurnerPlan& turnerPlan = *pImpl_->pAxisTurnerPlan_;
    PhysAbsoluteTime now = W4dManager::instance().time();
    MexRadians currentAngle = turnerPlan.angle( now );

    MexRadians newAngle  = currentAngle + byAngle;

    //Limit the angle as required
    const MachPhysGeneralData& genData = MachPhysData::instance().generalData();
    if( newAngle < genData.firstPersonLookUpDownMinAngle() )
        newAngle = genData.firstPersonLookUpDownMinAngle();
    else if( newAngle > genData.firstPersonLookUpDownMaxAngle() )
        newAngle = genData.firstPersonLookUpDownMaxAngle();

    //Get it moving
    turnerPlan.turnTo( now, newAngle );
}

void MachPhys1stPersonDriver::lookAhead()
{
    PRE( pImpl_->pAxisTurnerPlan_ != NULL );

    //Get the plan
    W4dAxisTurnerPlan& turnerPlan = *pImpl_->pAxisTurnerPlan_;

    //Get it moving
    turnerPlan.turnTo( W4dManager::instance().time(), MexRadians( 0.0 ) );
}

MexRadians MachPhys1stPersonDriver::lookUpDownAngle() const
{
    PRE( pImpl_->pAxisTurnerPlan_ != NULL );

    //Get the plan
    W4dAxisTurnerPlan& turnerPlan = *pImpl_->pAxisTurnerPlan_;

    return turnerPlan.angle( W4dManager::instance().time() );
}

bool MachPhys1stPersonDriver::targetPointAnglesValid( uint weaponIndex, const MexPoint3d& targetPoint ) const
{
    PRE( pCanAttack_ != NULL );

    //get the weapon and data object
    const MachPhysWeapon& weapon = pCanAttack_->weapon( weaponIndex );
    const MachPhysWeaponData& data = weapon.weaponData();

    //get the point coords relative to the weapon
    MexPoint3d localPoint( targetPoint );
    weapon.globalTransform().transformInverse( &localPoint );

    //Check in range
    MATHEX_SCALAR x = localPoint.x();
    return (Mathex::abs( localPoint.y() ) <= 0.95 * x * data.tanHorizontalDeviationAngle()) and
           (Mathex::abs( localPoint.z() ) <= 0.95 * x * data.tanVerticalDeviationAngle());
}

void MachPhys1stPersonDriver::initialiseCamera()
{
    //Set up the camera stuff
    setupTrackEntity();
}

void MachPhys1stPersonDriver::turnAtFastRate( bool fast )
{
    pImpl_->turnAtFastRate_ = fast;
}

bool MachPhys1stPersonDriver::turnAtFastRate() const
{
    return pImpl_->turnAtFastRate_;
}

void MachPhys1stPersonDriver::updateState( const MachPhysFirstPersonStateVector& state )
{
    //Copy some data
//    lastUpdateTime_ = state.lastUpdateTime_;
    movingForwards_ = state.movingForwards_;
    movingBackwards_ = state.movingBackwards_;
    turningLeft_ = state.turningLeft_;
    turningRight_ = state.turningRight_;
    pImpl_->turnAtFastRate_ = state.turnAtFastRate_;

    //Jump to specified position
    snapTo( state.transform_, state.lastSpeed_, lastUpdateTime_ );

    //Deal with head
    if( canTurnHead() )
    {
        snapHeadTo( state.headCurrentAngle_, lastUpdateTime_ );
        turnHeadTo( state.headTargetAngle_ );
    }

    //Set the camera angles
    W4dAxisTurnerPlan& cameraPlan = *pImpl_->pAxisTurnerPlan_;
    cameraPlan.reset( lastUpdateTime_, state.cameraCurrentAngle_ );
    cameraPlan.turnTo( lastUpdateTime_, state.cameraTargetAngle_ );
}

void MachPhys1stPersonDriver::computeState( MachPhysFirstPersonStateVector* pState )
{
    pState->transform_ = pEntity_->globalTransform();
    pState->lastUpdateTime_ = lastUpdateTime_;
    pState->lastSpeed_ = currentSpeed();
    pState->headCurrentAngle_ = currentHeadAngle().asScalar();
    pState->headTargetAngle_ = targetHeadAngle().asScalar();
    pState->movingForwards_ = movingForwards_;
    pState->movingBackwards_ = movingBackwards_;
    pState->turningLeft_ = turningLeft_;
    pState->turningRight_ = turningRight_;
    pState->turnAtFastRate_ = pImpl_->turnAtFastRate_;

    W4dAxisTurnerPlan& cameraPlan = *pImpl_->pAxisTurnerPlan_;
    pState->cameraCurrentAngle_ = cameraPlan.angle( lastUpdateTime_ ).asScalar();
    pState->cameraTargetAngle_ = cameraPlan.startAngle().asScalar() + cameraPlan.turnAngle().asScalar();
}

ostream& operator <<( ostream& o, const MachPhysFirstPersonStateVector& t )
{
    o << "-------------------------" << std::endl;
    o << "transform" << std::endl;
    o << t.transform_ << std::endl;
    o << "lastUpdateTime_ " << t.lastUpdateTime_ << " lastSpeed_ " << t.lastSpeed_ << std::endl;
    o << "headCurrentAngle_ " << t.headCurrentAngle_ << " headTargetAngle_ " << t.headTargetAngle_ << std::endl;
    o << "movingForwards_ " << t.movingForwards_ << " movingBackwards_ " << t.movingBackwards_ << std::endl;
    o << "turningLeft_ " << t.turningLeft_ << " turningRight_ " << t.turningRight_ << " turnAtFastRate_ " << t.turnAtFastRate_ << std::endl;
    o << "cameraCurrentAngle_ " << t.cameraCurrentAngle_ << " cameraTargetAngle_ " << t.cameraTargetAngle_ << std::endl;
    o << "-------------------------" << std::endl;

    return o;
}

#include "system/registry.hpp"

void MachPhys1stPersonDriver::adjustCameraRoll()
{
    //Compute the global transform down to the anti-roll's parent and include the base offset transform.
    W4dEntity* pAntiRollEntity = pImpl_->pAntiRollEntity_;
    W4dEntity* pParent = pAntiRollEntity->pParent();
    const MexTransform3d globalOwnerTransform = pParent->globalTransform();

	static double rollDamper = (double)SysRegistry::instance().queryIntegerValue( "1st Person", "Roll Damper", SysRegistry::CURRENT_USER );
	static double elevationDamper = (double)SysRegistry::instance().queryIntegerValue( "1st Person", "Elevation Damper", SysRegistry::CURRENT_USER );
	static double rollSpeed = (double)SysRegistry::instance().queryIntegerValue( "1st Person", "Roll Speed", SysRegistry::CURRENT_USER );
	static double elevationSpeed = (double)SysRegistry::instance().queryIntegerValue( "1st Person", "Elevation Speed", SysRegistry::CURRENT_USER );
	static bool checkValues = true;

	if ( checkValues )
	{
		checkValues = false;

		if ( MexEpsilon::isWithinEpsilonOf( rollDamper, 0.0 ) )
		{
			rollDamper = 2.0;
		}
		if ( MexEpsilon::isWithinEpsilonOf( elevationDamper, 0.0 ) )
		{
			elevationDamper = 2.0;
		}
		if ( MexEpsilon::isWithinEpsilonOf( rollSpeed, 0.0 ) )
		{
			rollSpeed = 5.0;
		}
		if ( MexEpsilon::isWithinEpsilonOf( elevationSpeed, 0.0 ) )
		{
			elevationSpeed = 5.0;
		}
	}

	// These lines commented out by NA. 2/2/99. Camera moves even if machine is not moving.
	// The antiroll entity transform needs to be updated only if its parent node has moved
	// This allows lazy evaluation of the camera transform to work in W4dCamera::domainRender()
	//if( globalOwnerTransform.key() != pImpl_->lastAntiRollParentTransformKey_ )
	//{
	//	pImpl_->lastAntiRollParentTransformKey_ = globalOwnerTransform.key();

	    MexTransform3d rolledTransform;

	    globalOwnerTransform.transform( pImpl_->cameraBaseTransform_, &rolledTransform );

	    //Create a new global transform with desired levelling.
		MexTransform3d levelledTransform( globalOwnerTransform );
		MexEulerAngles maxAngles;
		levelledTransform.rotation( &maxAngles );

		MexDegrees maxRollAngle = maxAngles.roll();
		MexDegrees maxElevationAngle = maxAngles.elevation();

		maxRollAngle = maxRollAngle / rollDamper;
		maxElevationAngle = maxElevationAngle / elevationDamper;

		MexDegrees diffRollAngle = maxRollAngle - pImpl_->lastRollAngle_;
		MexDegrees diffElevationAngle = maxElevationAngle - pImpl_->lastElevationAngle_;

		diffRollAngle = diffRollAngle / rollSpeed;
		diffElevationAngle = diffElevationAngle / elevationSpeed;

		pImpl_->lastRollAngle_ += diffRollAngle;
		pImpl_->lastElevationAngle_ += diffElevationAngle;

		maxAngles.roll( pImpl_->lastRollAngle_ );
		maxAngles.elevation( pImpl_->lastElevationAngle_ );

		levelledTransform.rotation( maxAngles );

	    //Hence compute the required transform for the anti-roll entity
	    MexTransform3d antiRollTransform;
	    globalOwnerTransform.transformInverse( levelledTransform, &antiRollTransform );

	    //And set the roll entity to this new value
	    pAntiRollEntity->localTransform( antiRollTransform );
	//}
}

void MachPhys1stPersonDriver::aimDataFilter( W4dEntityFilter* pFilter )
{
    pImpl_->pAimDataFilter_ = pFilter;
}

/* End P1DRIVER.CPP *************************************************/
