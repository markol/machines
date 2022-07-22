/* 
 * P L A N E 3 D . I P P
 * (c) Charybdis Limited, 1995, 1996. All Rights Reserved
 */

//////////////////////////////////////////////////////////////////////

#ifdef  _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

//////////////////////////////////////////////////////////////////////

_CODE_INLINE
MATHEX_SCALAR MexPlane3d::a() const
{
    return a_;
}

_CODE_INLINE
MATHEX_SCALAR MexPlane3d::b() const
{
    return b_;
}

_CODE_INLINE
MATHEX_SCALAR MexPlane3d::c() const
{
    return c_;
}

_CODE_INLINE
MATHEX_SCALAR MexPlane3d::d() const
{
    return d_;
}

_CODE_INLINE
const MexTransform3d& MexPlane3d::planeTransform( void ) const
{
    return planeTransform_;
}

_CODE_INLINE
const MexVec3& MexPlane3d::normal() const
{
	return normal_;
}

/* End PLANE3D.IPP *****************************************************/
