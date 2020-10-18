/*
 * O B J T R A C K . I P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
const W4dEntity& W4dObjectTracker::parent() const
{
    return *pParent_;
}

_CODE_INLINE
const MexPoint3d& W4dObjectTracker::localOrigin() const
{
    return localOrigin_;
}

_CODE_INLINE
const W4dEntity& W4dObjectTracker::target() const
{
    return *pTarget_;
}

_CODE_INLINE
const MexPoint3d& W4dObjectTracker::targetOffset() const
{
    return targetOffset_;
}

_CODE_INLINE
MATHEX_SCALAR W4dObjectTracker::maxAbsYawSine() const
{
    return maxAbsYawSine_;
}

_CODE_INLINE
MATHEX_SCALAR W4dObjectTracker::maxAbsPitchSine() const
{
    return maxAbsPitchSine_;
}

_CODE_INLINE
MATHEX_SCALAR W4dObjectTracker::minLength() const
{
    return minLength_;
}

_CODE_INLINE
MATHEX_SCALAR W4dObjectTracker::maxLength() const
{
    return maxLength_;
}

_CODE_INLINE
MATHEX_SCALAR W4dObjectTracker::defaultLength() const
{
    return defaultLength_;
}

_CODE_INLINE
void W4dObjectTracker::deltaAngle( MATHEX_SCALAR yawDeltaSine, MATHEX_SCALAR yawDeltaCosine )
{
    PRE( fabs( 1.0 - (yawDeltaSine*yawDeltaSine + yawDeltaCosine*yawDeltaCosine) ) < 0.00001 );
    yawDeltaSine_ = yawDeltaSine;
    yawDeltaCosine_ = yawDeltaCosine;
}

_CODE_INLINE
MATHEX_SCALAR W4dObjectTracker::yawDeltaSine() const
{
    return yawDeltaSine_;
}

_CODE_INLINE
MATHEX_SCALAR W4dObjectTracker::yawDeltaCosine() const
{
    return yawDeltaCosine_;
}
/* End OBJTRACK.IPP *************************************************/
