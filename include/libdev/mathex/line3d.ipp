/*
 * L I N E 3 D . I P P
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */

#ifdef  _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
bool MexLine3d::contains( const MexPlane3d& ) const
{
//	PRE( isValid( p ) );
		 
	// a line cannot contain a plane
	return false;
}

_CODE_INLINE
const MexPoint3d& MexLine3d::end1() const
{
	return end1_;
}

_CODE_INLINE
const MexPoint3d& MexLine3d::end2() const
{
	return end2_;
}

/* End LINE3D.IPP *****************************************************/
