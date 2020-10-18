/*
 * D E G R E E S . I P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of inline non-template methods and inline global functions

#ifdef _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

_CODE_INLINE
MexDegrees::MexDegrees( void )
: MexAngle()
{
}

_CODE_INLINE
MexDegrees::MexDegrees( MATHEX_SCALAR angle )
: MexAngle( angle * Mathex::PI / 180.0 )
{
}

_CODE_INLINE
MexDegrees::MexDegrees( const MexAngle& angle )
: MexAngle( angle )
{
}

_CODE_INLINE
MATHEX_SCALAR   MexDegrees::asScalar( void ) const
{
    return angleInRadians() * 180.0 / Mathex::PI;
}

_CODE_INLINE
const MexDegrees   operator +( const MexDegrees& a, const MexDegrees& b )
{
    return MexDegrees( a.asScalar() + b.asScalar() );
}

_CODE_INLINE
const MexDegrees   operator +( const MexDegrees& a, const MexRadians& b )
{
    return MexDegrees( a.asScalar() + b.asScalar() * 180.0 / Mathex::PI );
}

_CODE_INLINE
const MexDegrees   operator -( const MexDegrees& a, const MexDegrees& b )
{
    return MexDegrees( a.asScalar() - b.asScalar() );
}

_CODE_INLINE
const MexDegrees   operator -( const MexDegrees& a, const MexRadians& b )
{
    return MexDegrees( a.asScalar() - b.asScalar() * 180.0 / Mathex::PI );
}

_CODE_INLINE
const MexDegrees   operator *( const MexDegrees& a, MATHEX_SCALAR scalar )
{
    return MexDegrees( a.asScalar() * scalar );
}

_CODE_INLINE
const MexDegrees   operator *( MATHEX_SCALAR scalar, const MexDegrees& a )
{
    return MexDegrees( a.asScalar() * scalar );
}

_CODE_INLINE
const MexDegrees   operator /( const MexDegrees& a, MATHEX_SCALAR scalar )
{
    return MexDegrees( a.asScalar() / scalar );
}

_CODE_INLINE
const MexDegrees   operator -( const MexDegrees& a )
{
    return -a.asScalar();
}

_CODE_INLINE
ostream& operator <<( ostream& o, const MexDegrees& t )
{
    o << t.angleInRadians() * 180.0 / Mathex::PI;

    return o;
}

/* End DEGREES.IPP **************************************************/
