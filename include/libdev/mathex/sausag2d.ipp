/*
 * S A U S A G 2 D . I P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
const MexPoint2d& MexSausage2d::startPoint() const
{
    return startPoint_;
}

_CODE_INLINE
const MexPoint2d& MexSausage2d::endPoint() const
{
    return endPoint_;
}

_CODE_INLINE
MATHEX_SCALAR MexSausage2d::radius() const
{
    return radius_;
}

_CODE_INLINE
MATHEX_SCALAR MexSausage2d::length() const
{
    return length_;
}

/* End SAUSAG2D.IPP *************************************************/
