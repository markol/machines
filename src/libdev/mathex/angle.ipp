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
MexAngle::MexAngle( void )
: angleInRadians_( 0.0 )
{
}

_CODE_INLINE
MexAngle::MexAngle( MATHEX_SCALAR angle )
: angleInRadians_( angle )
{
}

_CODE_INLINE
MexAngle::MexAngle( const MexAngle& angle )
: angleInRadians_( angle.angleInRadians_ )
{
}

_CODE_INLINE
MATHEX_SCALAR MexAngle::angleInRadians( void ) const
{
	return angleInRadians_;
}

_CODE_INLINE
void MexAngle::operator +=( const MexAngle& angle )
{
	angleInRadians_ += angle.angleInRadians_;
}

_CODE_INLINE
void MexAngle::operator -=( const MexAngle& angle )
{
	angleInRadians_ -= angle.angleInRadians_;
}

_CODE_INLINE
MexAngle& MexAngle::operator =( const MexAngle& angle )
{
    angleInRadians_ = angle.angleInRadians_;
    
    return *this;
}

_CODE_INLINE
void MexAngle::operator *=( MATHEX_SCALAR scalar )
{
    angleInRadians_ *= scalar;
}

_CODE_INLINE
void MexAngle::operator /=( MATHEX_SCALAR scalar )
{
    angleInRadians_ /= scalar;
}

_CODE_INLINE
double  sin( const MexAngle& angle )
{
    return sin( angle.angleInRadians_ );
}

_CODE_INLINE
double  cos( const MexAngle& angle )
{
    return cos( angle.angleInRadians_ );
}

_CODE_INLINE
double  tan( const MexAngle& angle )
{
    return tan( angle.angleInRadians_ );
}

_CODE_INLINE
bool operator ==( const MexAngle& a, const MexAngle& b )
{
    return a.angleInRadians_ == b.angleInRadians_;
}

_CODE_INLINE
bool operator <( const MexAngle& a, const MexAngle& b )
{
    return a.angleInRadians_ < b.angleInRadians_;
}

_CODE_INLINE
bool operator >( const MexAngle& a, const MexAngle& b )
{
    return a.angleInRadians_ > b.angleInRadians_;
}

_CODE_INLINE
bool operator <=( const MexAngle& a, const MexAngle& b )
{
    return a.angleInRadians_ <= b.angleInRadians_;
}

_CODE_INLINE
bool operator >=( const MexAngle& a, const MexAngle& b )
{
    return a.angleInRadians_ >= b.angleInRadians_;
}

/* End RADIANS.IPP **************************************************/
