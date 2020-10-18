/*
 * A X I S T R A K . I P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
bool W4dAxisTrackerPlan::hasLimits() const
{
    return limitAngles_;
}

_CODE_INLINE
const W4dEntity& W4dAxisTrackerPlan::turningObject() const
{
    return *pTurningObject_;
}

_CODE_INLINE
const W4dEntity& W4dAxisTrackerPlan::targetObject() const
{
    return *pTargetObject_;
}

_CODE_INLINE
W4d::Axis W4dAxisTrackerPlan::turningAxis() const
{
    return turningAxis_;
}

_CODE_INLINE
W4d::Axis W4dAxisTrackerPlan::pointingAxis() const
{
    return pointingAxis_;
}

_CODE_INLINE
const MexTransform3d& W4dAxisTrackerPlan::baseTransform() const
{
    return baseTransform_;
}

_CODE_INLINE
MexRadians W4dAxisTrackerPlan::turnRate() const
{
    return turnRate_;
}

_CODE_INLINE
MexRadians W4dAxisTrackerPlan::minAngle() const
{
    return minAngle_;
}

_CODE_INLINE
MexRadians W4dAxisTrackerPlan::maxAngle() const
{
    return maxAngle_;
}

_CODE_INLINE
uint W4dAxisTrackerPlan::nFramesPerTargetOffsetUpdate() const
{
    return nFramesPerTargetOffsetUpdate_;
}
/* End AXISTRAK.IPP *************************************************/
