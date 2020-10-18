/*
 * R A D I A N S . I P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
MexRadians::MexRadians( void )
: MexAngle()
{
}

_CODE_INLINE
MexRadians::MexRadians( MATHEX_SCALAR angle )
: MexAngle( angle )
{
}

_CODE_INLINE
MexRadians::MexRadians( const MexAngle& angle )
: MexAngle( angle )
{
}

_CODE_INLINE
MATHEX_SCALAR   MexRadians::asScalar( void ) const
{
    return angleInRadians();
}

_CODE_INLINE
ostream& operator <<( ostream& o, const MexRadians& t )
{
    o << t.angleInRadians();

    return o;
}

_CODE_INLINE
const MexRadians   operator +( const MexRadians& a, const MexRadians& b )
{
    return MexRadians( a.asScalar() + b.asScalar() );
}

_CODE_INLINE
const MexRadians   operator +( const MexRadians& a, const MexDegrees& b )
{
    return MexRadians( a.asScalar() + ( b.asScalar() * Mathex::PI / 180.0 ) );
}

_CODE_INLINE
const MexRadians   operator -( const MexRadians& a, const MexRadians& b )
{
    return MexRadians( a.asScalar() - b.asScalar() );
}

_CODE_INLINE
const MexRadians   operator -( const MexRadians& a, const MexDegrees& b )
{
    return MexRadians( a.asScalar() - ( b.asScalar() * Mathex::PI / 180.0 ) );
}

_CODE_INLINE
const MexRadians   operator *( const MexRadians& a, MATHEX_SCALAR scalar )
{
    return MexRadians( a.asScalar() * scalar );
}

_CODE_INLINE
const MexRadians   operator *( MATHEX_SCALAR scalar, const MexRadians& a )
{
    return MexRadians( a.asScalar() * scalar );
}

_CODE_INLINE
const MexRadians   operator /( const MexRadians& a, MATHEX_SCALAR scalar )
{
    return MexRadians( a.asScalar() / scalar );
}

_CODE_INLINE
const MexRadians   operator -( const MexRadians& a )
{
    return -a.asScalar();
}

/* End RADIANS.IPP **************************************************/
