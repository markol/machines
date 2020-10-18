/*
 * A X I S T R A K . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    W4dAxisTrackerPlan

    A transform plan that turns an object object a nominated principal axis, so that
    one of the other principal axes points in the direction of an offset point from
    a target entity. This plan is evaluated on the fly, and so is reactive rather than
    predictive.
    The angle of turn may be constrained within given limits. A turning rate is supplied.
    This is used as a flat rate without considering accelerations.
*/

#ifndef _WORLD4D_AXISTRAK_HPP
#define _WORLD4D_AXISTRAK_HPP

#include "base/base.hpp"
#include "mathex/point3d.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/radians.hpp"
#include "phys/motplan.hpp"
#include "world4d/world4d.hpp"

class W4dEntity;

class W4dAxisTrackerPlan : public PhysMotionPlan
// Canonical form revoked
{
public:
    //Supplies a transform for turningObject to rotate around turningAxis so that
    //pointAxis points in the direction of targetObject.
    //The central position at angle zero is baseTransform.
    //The rate at which the object can turn is turnRate.
    //The time for which the plan should execute.
    //NB client must ensure this plan is deleted if targetObject is deleted.
    W4dAxisTrackerPlan( const W4dEntity& turningObject, W4dEntity& targetObject,
                        const MexTransform3d& baseTransform,
                        W4d::Axis turningAxis, W4d::Axis pointingAxis, MexRadians turnRate,
                        const PhysRelativeTime& duration );
    //PRE( turningAxis != pointingAxis );
    //PRE( turnRate > 0.0 );

    virtual ~W4dAxisTrackerPlan();

    /////////////////////////////////////////////
    // Inherited from PhysMotionPlan

    //Override to define result as a function of timeOffset.
    //If time is greater than duration, the transform at time duration is to be returned.
    virtual void transform( const PhysRelativeTime& timeOffset, MexTransform3d* result) const;
    /////////////////////////////////////////////

    //Reset the target object
    void targetObject( const W4dEntity& targetObject );

    //The offset from targetObject to the target point.
    //Default implementation returns bounding volume centroid.
    virtual MexPoint3d targetOffset( const W4dEntity& targetObject ) const;

    //Resets the current angle based on current position. Allows client to jump to new
    //angle, or restart tracking after turning through alternative method.
    void forceCurrentAngle();

    //Compute the current angle represented by the turner's existing transform
    MexRadians currentAngle();

    //Set limits on the motion as specified.
    //Default is no limits.
    void setLimits( MexRadians minAngle, MexRadians maxAngle );
    //PRE( minAngle.asScalar() <= maxAngle.asScalar() );

    //true if angular limits on the motion have been set.
    bool hasLimits() const;

    //true if the current angle is within tolerance of any limits
    bool nearLimits( MexRadians tolerance ) const;

    //Set the number of frames between calls to targetOffset().
    //This is an efficiency issue, as the targetOffset() computation may be
    //expensive. Default value is 50.
    void nFramesPerTargetOffsetUpdate( uint nFrames );
    //PRE( nFrames != 0 );

    //Accessors
    const W4dEntity& turningObject() const;
    const W4dEntity& targetObject() const;
    W4d::Axis turningAxis() const;
    W4d::Axis pointingAxis() const;
    const MexTransform3d& baseTransform() const;
    MexRadians turnRate() const;
    MexRadians minAngle() const;
    MexRadians maxAngle() const;
    uint nFramesPerTargetOffsetUpdate() const;

    //Set new base transform
    void baseTransform( const MexTransform3d& transform );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const W4dAxisTrackerPlan& t );

    W4dAxisTrackerPlan( const W4dAxisTrackerPlan& );
    W4dAxisTrackerPlan& operator =( const W4dAxisTrackerPlan& );

    //data members
    const W4dEntity* pTurningObject_; //Object being turned
    const W4dEntity* pTargetObject_; //target object
    MexPoint3d targetOffset_; //Offset of target point from target object
    MexTransform3d baseTransform_; //Unrotated transform for pTurningObject_
    W4d::Axis turningAxis_; //Principal axis of rotation
    W4d::Axis pointingAxis_; //Principal axis to aim at target
    MexRadians turnRate_; //Rotation rate
    MexRadians minAngle_; //upper angular limit if limitAngles_ is true
    MexRadians maxAngle_; //lower angular limit if limitAngles_ is true
    bool limitAngles_; //True if angular motion is constrained
    uint nFramesPerTargetOffsetUpdate_; //The number of frames between calls to targetOffset to
                                       //recompute the offset.
    PhysAbsoluteTime lastUpdateTime_; //Time the angle was last updated
    MexRadians lastAngle_; //Angle set at last update
};

#ifdef _INLINE
    #include "world4d/axistrak.ipp"
#endif


#endif

/* End AXISTRAK.HPP *************************************************/
