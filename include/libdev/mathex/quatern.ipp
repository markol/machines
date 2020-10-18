/*
 * Q U A T E R N . I P P
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
MexQuaternion::MexQuaternion()
: s_( 1.0 )
{
	TEST_INVARIANT
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
const MexVec3& MexQuaternion::vector( void ) const
{
	return v_;
}
//////////////////////////////////////////////////////////////////////////////////////////
_CODE_INLINE
const MATHEX_SCALAR& MexQuaternion::scalar( void ) const
{
	return s_;
}

// friend
_CODE_INLINE
bool operator ==( const MexQuaternion& a, const MexQuaternion& b )
{
    return a.v_ == b.v_ and a.s_ == b.s_;
}

// friend
_CODE_INLINE
bool operator !=( const MexQuaternion& a, const MexQuaternion& b )
{
    return !(a == b);
}
//////////////////////////////////////////////////////////////////////////////////////////

/* End EULPARAM.IPP *************************************************/
