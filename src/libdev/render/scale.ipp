/*
 * S C A L E . I P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
MATHEX_SCALAR RenUniformScale::factor() const
{
	return factor_;
}

_CODE_INLINE
void RenUniformScale::factor( MATHEX_SCALAR f )
{
	PRE( f > 0.0 );
	factor_ = f;
}

_CODE_INLINE
MATHEX_SCALAR RenNonUniformScale::x() const
{
	return x_;
}

_CODE_INLINE
MATHEX_SCALAR RenNonUniformScale::y() const
{
	return y_;
}

_CODE_INLINE
MATHEX_SCALAR RenNonUniformScale::z() const
{
	return z_;
}

/* End SCALE.IPP *************************************************/
