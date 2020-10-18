/*
 * C I R C L E 2 D . I P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
const MexPoint2d& MexCircle2d::centre() const
{
    return centre_;
}

_CODE_INLINE
MATHEX_SCALAR MexCircle2d::radius() const
{
    return radius_;
}

/* End CIRCLE2D.IPP *************************************************/
