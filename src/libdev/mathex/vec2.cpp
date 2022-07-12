/*
 * V E C 2 . C P P
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */

#include <iostream>
#include "base/base.hpp"

#include "mathex/mathex.hpp"
#include "mathex/vec2.hpp"
#include "mathex/radians.hpp"

#ifndef _INLINE
    #include "mathex/vec2.ipp"
#endif

const MexVec2&
MexVec2::operator =( const MexVec2& v)
{
    if( this != &v )
    {
        x_ = v.x_;
        y_ = v.y_;
    }

    POST( *this == v );

    return *this;
}


bool MexVec2::operator ==( const MexVec2& v ) const
{
    return x_ == v.x_ and y_ == v.y_;
}


bool MexVec2::isZeroVector( ) const
{
    return x_ == 0 and y_ == 0;
}


void MexVec2::operator +=( const MexVec2& v )
{
    x_ += v.x_;
    y_ += v.y_;
}


const MexVec2& MexVec2::zeroVector()
{
    static MexVec2 zero_ = MexVec2( 0, 0 );

    return zero_;
}


/////////////////////////////////////////////////////////////


MATHEX_SCALAR MexVec2::operator []( size_t i ) const
{
    PRE( i < 2 );

	if (i==0)
	    return x_;
	else
		return y_;
}

/////////////////////////////////////////////////////////////


MATHEX_SCALAR& MexVec2::operator []( size_t i )
{
    PRE( i < 2 );

	if (i==0)
	    return x_;
	else
		return y_;
}

/////////////////////////////////////////////////////////////

ostream& operator<<(ostream& os, const MexVec2& vec)
{
	os << "(" << vec.x() << "," << vec.y() << ")";
	return os;
}

// Should cope with:
//		x y
//		x,y
//		(x,y)		i.e., result of output operator
// Where x and y are floating point numbers.
istream& operator>>(istream& is, MexVec2& vec)
{
	MATHEX_SCALAR x,y;
	char c = 0;
	bool brackets = false;

	is >> c;

	// Allow an optional open bracket.
	if (c == '(')
		brackets = true;
	else
		is.putback(c);			// probably a digit

	is >> x;
	if (is)
	{
		// Allow an optional comma.
		is >> c;
		if (c != ',')
			is.putback(c);		// probably a digit

		is >> y;

		if (is && brackets)
		{
			// Expect a closing bracket; produce an error otherwise.
			is >> c;
			if (c != ')')
				is.clear(std::ios::badbit);
		}

		// If the stream state is good after all that, set the vector.
		if (is)
			vec.setVector(x,y);
	}
	return is;
}
//////////////////////////////////////////////////////////////////////////////////////////

MexVec2::MexVec2( const MexPoint2d& p1, const MexPoint2d& p2, MATHEX_SCALAR s )
{
    PRE( s != 0 );
    MATHEX_SCALAR r = 1 / s;
    x_ = r * (p2.x() - p1.x());
    y_ = r * (p2.y() - p1.y());
}
//////////////////////////////////////////////////////////////////////////////////////////

MexVec2::MexVec2( const MexPoint2d& p1, const MexPoint2d& p2 )
{
    x_ = p2.x() - p1.x();
    y_ = p2.y() - p1.y();
}
//////////////////////////////////////////////////////////////////////////////////////////

void MexVec2::makeUnitVector()
{
    PRE( not isZeroVector() );

    //Compute the length
    MATHEX_SCALAR length = x_ * x_ + y_ * y_;

    //No modification needed if already unit
    if( length != 1 )
    {
        MATHEX_SCALAR s = 1.0 / sqrt( length );
        x_ *= s;
        y_ *= s;
    }
}
//////////////////////////////////////////////////////////////////////////////////////////

MexRadians MexVec2::angleBetween( const MexVec2& other ) const
{
    //Construct unit direction vectors
    MexVec2 h( *this );
    h.makeUnitVector();

    MexVec2 d( other );
    d.makeUnitVector();

    MATHEX_SCALAR cosine = h.dotProduct( d );
    MATHEX_SCALAR sine = h.x() * d.y() - h.y() * d.x();

    return MexRadians( atan2( sine, cosine ) );
}
//////////////////////////////////////////////////////////////////////////////////////////

PER_DEFINE_PERSISTENT( MexVec2 );

void perWrite( PerOstream& str, const MexVec2& t )
{
    str << t.x_;
    str << t.y_;
}

void perRead( PerIstream& str, MexVec2& t )
{
    str >> t.x_;
    str >> t.y_;
}

void MexVec2::rotate( const MexRadians& angle )
{
    POST_DATA( MexVec2 original( *this ) );

    MATHEX_SCALAR s = sin( angle );
    MATHEX_SCALAR c = cos( angle );

    MATHEX_SCALAR newX = x_ * c - y_ * s;
    MATHEX_SCALAR newY = x_ * s + y_ * c;

    x_ = newX;
    y_ = newY;

    POST_DATA( angle.asScalar() );
    POST_DATA( original.angleBetween( *this ).asScalar() );

    POST( MexEpsilon::isWithinEpsilonOf( original.angleBetween( *this ).asScalar(), angle.asScalar() ) );
}

void MexVec2::operator -=( const MexVec2& v )
{
    x_ -= v.x_;
    y_ -= v.y_;
}

void MexVec2::operator *=( MATHEX_SCALAR t )
{
    x_ *= t;
    y_ *= t;
}

void MexVec2::operator /=( MATHEX_SCALAR t )
{
    PRE( t != 0 );

    x_ /= t;
    y_ /= t;
}

/* End VEC2.CPP *****************************************************/
