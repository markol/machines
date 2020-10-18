/*
 * R A M P A C C E . I P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
//static
MATHEX_SCALAR PhysRampAcceleration::endSpeed
(
    MATHEX_SCALAR startSpeed, MATHEX_SCALAR acceleration, const PhysRelativeTime& time
)
{
    return startSpeed + acceleration * time;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
//static
MATHEX_SCALAR PhysRampAcceleration::distance
(
    MATHEX_SCALAR startValue, MATHEX_SCALAR startSpeed,
    MATHEX_SCALAR acceleration, const PhysRelativeTime& time
)
{
    return startValue + (startSpeed + 0.5 * acceleration * time) * time;
}
//////////////////////////////////////////////////////////////////////////////////////////

_CODE_INLINE
MATHEX_SCALAR PhysRampAcceleration::startSpeed() const
{
    return startSpeed_;
}

_CODE_INLINE
MATHEX_SCALAR PhysRampAcceleration::firstAcceleration() const
{
    return firstAcceleration_;
}

_CODE_INLINE
MATHEX_SCALAR PhysRampAcceleration::secondAcceleration() const
{
    return secondAcceleration_;
}

_CODE_INLINE
const PhysRelativeTime& PhysRampAcceleration::totalTime() const
{
    return totalTime_;
}

_CODE_INLINE
const PhysRelativeTime& PhysRampAcceleration::firstAccelerationTime() const
{
    return firstAccelerationTime_;
}

_CODE_INLINE
const PhysRelativeTime& PhysRampAcceleration::secondAccelerationTime() const
{
    return secondAccelerationTime_;
}
/* End RAMPACCE.IPP *************************************************/
