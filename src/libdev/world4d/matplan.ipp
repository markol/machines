/*
 * M A T P L A N . I P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
const PhysRelativeTime& W4dMaterialPlan::duration() const
{
    return duration_;
}

_CODE_INLINE
bool W4dMaterialPlan::isDone( const PhysRelativeTime& timeOffset ) const
{
    return timeOffset >= duration_;
}

_CODE_INLINE
void W4dMaterialPlan::duration( const PhysRelativeTime& newDuration )
{
    duration_ = newDuration;
}

_CODE_INLINE
W4dLOD W4dMaterialPlan::maxLOD() const
{
    return maxLOD_;
}
/* End MATPLAN.IPP **************************************************/
