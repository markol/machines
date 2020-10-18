/*
 * T U R N T R A K . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "ctl/countptr.hpp"
#include "mathex/radians.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"
#include "phys/phys.hpp"
#include "world4d/axistrak.hpp"
#include "world4d/axisturn.hpp"
#include "world4d/manager.hpp"
#include "world4d/entity.hpp"
#include "world4d/entyplan.hpp"

#include "machphys/turntrak.hpp"

#ifndef _INLINE
    #include "machphys/turntrak.ipp"
#endif

//implementation data object
class ITurnerTracker
{
public:

	~ITurnerTracker()
	{
		// those are deleted by countptr, no idea why it was here and making troubles
		//_DELETE( pAxisTrackerPlan_ );
		//_DELETE( pAxisTurnerPlan_ );
	}

    W4dEntity *pTurningObject_; //The object to turn
    W4dAxisTrackerPlan* pAxisTrackerPlan_; //Used to implement tracking
    PhysMotionPlanPtr axisTrackerPlanPtr_; //References and prevents deletion of pAxisTrackerPlan_
    W4dAxisTurnerPlan* pAxisTurnerPlan_; //Used to implement turning
    PhysMotionPlanPtr axisTurnerPlanPtr_; //References and prevents deletion of pAxisTurnerPlan_
    bool isTracking_; //True when tracking has been activated
};

MachPhysTurnerTracker::MachPhysTurnerTracker
(
    W4dEntity* pTurningObject, W4d::Axis turningAxis, W4d::Axis pointingAxis, MexRadians turnRate
)
:   pImpl_( _NEW( ITurnerTracker ) )
{
    //Set up the data object.
    //Construct a tracker plan for use when tracking, and to store basic data.
    //temporarily set the target also as this truning object. This will be reset to something
    //sensible when tracking is actually invoked.
    ITurnerTracker& impl = *pImpl_;
    PhysRelativeTime duration = 31536000.0; //One year
    impl.pAxisTrackerPlan_ =
        _NEW( W4dAxisTrackerPlan( *pTurningObject, *pTurningObject, pTurningObject->localTransform(),
                                  turningAxis, pointingAxis, turnRate, duration ) );

    //Store the pointer using a counted ptr. This ensures the plan object doesn't get deleted
    //after use by a W4dEntityPlan.
    impl.axisTrackerPlanPtr_ = impl.pAxisTrackerPlan_;

    //Now do likewise for the turner plan
    impl.pAxisTurnerPlan_ =
        _NEW( W4dAxisTurnerPlan( pTurningObject->localTransform(), turningAxis, 0.0, 0.0, 0.0, turnRate, duration ) );

    impl.axisTurnerPlanPtr_ = impl.pAxisTurnerPlan_;

    impl.pTurningObject_ = pTurningObject;
    impl.isTracking_ = false;

    TEST_INVARIANT;
}

MachPhysTurnerTracker::~MachPhysTurnerTracker()
{
    TEST_INVARIANT;

    _DELETE( pImpl_ );
}

void MachPhysTurnerTracker::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

ostream& operator <<( ostream& o, const MachPhysTurnerTracker& t )
{

    o << "MachPhysTurnerTracker " << (void*)&t << " start" << std::endl;
    o << "MachPhysTurnerTracker " << (void*)&t << " end" << std::endl;

    return o;
}

PhysRelativeTime MachPhysTurnerTracker::turnToAngle( MexRadians angle )
{
    ITurnerTracker& impl = *pImpl_;
    W4dAxisTrackerPlan& axisTrackerPlan = *(impl.pAxisTrackerPlan_);
    W4dAxisTurnerPlan& axisTurnerPlan = *(impl.pAxisTurnerPlan_);

    //If tracking, cancel and store the current angle
    if( impl.isTracking_ )
        stopTracking();

    //Enable the turner plan with desired angle
    PhysAbsoluteTime nowTime = W4dManager::instance().time();
    impl.pTurningObject_->entityPlanForEdit().absoluteMotion( impl.axisTurnerPlanPtr_, nowTime );
    return axisTurnerPlan.turnTo( nowTime, angle );
}

PhysRelativeTime MachPhysTurnerTracker::turnByAngle( MexRadians angle )
{
    ITurnerTracker& impl = *pImpl_;
    W4dAxisTrackerPlan& axisTrackerPlan = *(impl.pAxisTrackerPlan_);
    W4dAxisTurnerPlan& axisTurnerPlan = *(impl.pAxisTurnerPlan_);

    //If tracking, cancel and store the current angle
    if( impl.isTracking_ )
        stopTracking();

    //Enable the turner plan with desired angle
    PhysAbsoluteTime nowTime = W4dManager::instance().time();
    impl.pTurningObject_->entityPlanForEdit().absoluteMotion( impl.axisTurnerPlanPtr_, nowTime );
    return axisTurnerPlan.turnBy( nowTime, angle );
}

void MachPhysTurnerTracker::trackTarget( const W4dEntity& targetObject )
{
    ITurnerTracker& impl = *pImpl_;
    W4dAxisTrackerPlan& axisTrackerPlan = *(impl.pAxisTrackerPlan_);

    //Set the target object in our permanent tracker plan, and apply the plan
    axisTrackerPlan.targetObject( targetObject );
    axisTrackerPlan.forceCurrentAngle();
    impl.pTurningObject_->entityPlanForEdit().absoluteMotion( impl.axisTrackerPlanPtr_, W4dManager::instance().time() );

    impl.isTracking_ = true;
}

void MachPhysTurnerTracker::stopTracking()
{
    //Check we are tracking
    ITurnerTracker& impl = *pImpl_;
    if( impl.isTracking_ )
    {
        //Ensure angle up-to-date at current time
        W4dEntity& turningObject = *(impl.pTurningObject_);
        turningObject.localTransform();

        //Set this angle in the turner plan
        impl.pAxisTurnerPlan_->reset( W4dManager::instance().time(), impl.pAxisTrackerPlan_->currentAngle() );

        //Clear any motion plans
        impl.pTurningObject_->entityPlanForEdit().clearMotionPlans();

        impl.isTracking_ = false;
    }
}

bool MachPhysTurnerTracker::isTrackingTarget() const
{
    return pImpl_->isTracking_;
}

const W4dEntity& MachPhysTurnerTracker::targetObject() const
{
    PRE( isTrackingTarget() );

    return pImpl_->pAxisTrackerPlan_->targetObject();
}

void MachPhysTurnerTracker::limits( MexRadians minAngle, MexRadians maxAngle )
{
    PRE( minAngle.asScalar() <= maxAngle.asScalar() );
    pImpl_->pAxisTrackerPlan_->setLimits( minAngle, maxAngle );
}

bool MachPhysTurnerTracker::hasLimits() const
{
    return pImpl_->pAxisTrackerPlan_->hasLimits();
}

MexRadians MachPhysTurnerTracker::minAngle() const
{
    return pImpl_->pAxisTrackerPlan_->minAngle();
}

MexRadians MachPhysTurnerTracker::maxAngle() const
{
    return pImpl_->pAxisTrackerPlan_->maxAngle();
}

bool MachPhysTurnerTracker::nearLimits( MexRadians tolerance ) const
{
    return pImpl_->pAxisTrackerPlan_->nearLimits( tolerance );
}

void MachPhysTurnerTracker::updateBaseLocation()
{
    //Get the turning entity's current location
    MexPoint3d newLocation = pImpl_->pTurningObject_->localTransform().position();

    //Update the axis-tracker's base transform to use this location
    MexTransform3d newTransform = pImpl_->pAxisTrackerPlan_->baseTransform();
    newTransform.position( newLocation );
    pImpl_->pAxisTrackerPlan_->baseTransform( newTransform );
    pImpl_->pAxisTurnerPlan_->baseTransform( newTransform );
}

MexRadians MachPhysTurnerTracker::currentAngle() const
{
    ITurnerTracker& impl = *pImpl_;
    return (impl.isTracking_ ? impl.pAxisTrackerPlan_->currentAngle() :
                               impl.pAxisTurnerPlan_->angle( W4dManager::instance().time()) );
}

void MachPhysTurnerTracker::snapToAngle( const PhysAbsoluteTime& atTime, MexRadians angle )
{
    ITurnerTracker& impl = *pImpl_;
    W4dAxisTurnerPlan& axisTurnerPlan = *(impl.pAxisTurnerPlan_);

    //If tracking, cancel and store the current angle
    if( impl.isTracking_ )
        stopTracking();

    //Set the angle and time for the turner
    axisTurnerPlan.reset( atTime, angle );

    //Enable the turner plan with desired angle
    PhysAbsoluteTime nowTime = W4dManager::instance().time();
    impl.pTurningObject_->entityPlanForEdit().absoluteMotion( impl.axisTurnerPlanPtr_, nowTime );
}

MexRadians MachPhysTurnerTracker::targetAngle() const
{
    PRE( not isTrackingTarget() );

    ITurnerTracker& impl = *pImpl_;
    W4dAxisTurnerPlan& axisTurnerPlan = *(impl.pAxisTurnerPlan_);

    return axisTurnerPlan.startAngle() + axisTurnerPlan.turnAngle();
}

/* End TURNTRAK.CPP *************************************************/
