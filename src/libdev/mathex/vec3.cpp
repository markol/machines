/*
 * V E C 3 . C P P
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */

/////////////////////////////////////////////////////////

#include <math.h>

#include "mathex/mathex.hpp"
#include "mathex/vec3.hpp"
#include "mathex/cvec3.hpp"
#include "mathex/point3d.hpp"

#ifndef  _INLINE
    #include "mathex/vec3.ipp"
#endif

/////////////////////////////////////////////////////////


MexVec3::MexVec3( MATHEX_SCALAR x, MATHEX_SCALAR y, MATHEX_SCALAR z )
{
    vec_[ X ] = x;
    vec_[ Y ] = y;
    vec_[ Z ] = z;
}


//////////////////////////////////////////////////////////////////////

MexVec3::MexVec3( const MexVec3& v )
{
    vec_[ X ] = v.x();
    vec_[ Y ] = v.y();
    vec_[ Z ] = v.z();

    POST( *this == v );
}

MexVec3::MexVec3( const MexPoint3d& v )
{
    vec_[ X ] = v.x();
    vec_[ Y ] = v.y();
    vec_[ Z ] = v.z();
}

MexVec3::MexVec3( const MexPoint3d& p, const MexPoint3d& q )
{
    vec_[ X ] = q.x() - p.x();
    vec_[ Y ] = q.y() - p.y();
    vec_[ Z ] = q.z() - p.z();
}

MexVec3::MexVec3( const MexPoint3d& p, const MexPoint3d& q, MATHEX_SCALAR s )
{
    PRE( s != 0.0 );

    vec_[ X ] = q.x() - p.x();
    vec_[ Y ] = q.y() - p.y();
    vec_[ Z ] = q.z() - p.z();

    MATHEX_SCALAR r = 1.0 / s;
    vec_[ X ] *= r;
    vec_[ Y ] *= r;
    vec_[ Z ] *= r;
}

//////////////////////////////////////////////////////////////////////


const MexVec3& MexVec3::operator =( const MexVec3& rhs )
{
	vec_[ X ] = rhs.x();
	vec_[ Y ] = rhs.y();
	vec_[ Z ] = rhs.z();

    return *this;
}

const MexVec3& MexVec3::operator =( const MexPoint3d& rhs )
{
	vec_[ X ] = rhs.x();
	vec_[ Y ] = rhs.y();
	vec_[ Z ] = rhs.z();

    return *this;
}


//////////////////////////////////////////////////////////////////////

bool  MexVec3::operator ==( const MexVec3& v ) const
{
    return Mathex::abs( x() - v.x() ) < MexEpsilon::instance()
    	and Mathex::abs( y() - v.y() ) < MexEpsilon::instance()
    	and Mathex::abs( z() - v.z() ) < MexEpsilon::instance();
}

bool  MexVec3::operator !=( const MexVec3& v ) const
{
    return !(*this == v);
}

//////////////////////////////////////////////////////////////////////

void MexVec3::operator +=( const MexVec3& v )
{
    vec_[ X ] += v.x();
	vec_[ Y ] += v.y();
	vec_[ Z ] += v.z();
}

//////////////////////////////////////////////////////////////////////

void MexVec3::operator -=( const MexVec3& v )
{
	vec_[ X ] -= v.x();
	vec_[ Y ] -= v.y();
	vec_[ Z ] -= v.z();
}

//////////////////////////////////////////////////////////////////////

void MexVec3::operator *=( MATHEX_SCALAR v )
{
	vec_[ X ] *= v;
	vec_[ Y ] *= v;
	vec_[ Z ] *= v;
}

//////////////////////////////////////////////////////////////////////

void MexVec3::operator /=( MATHEX_SCALAR v )
{
    PRE( v != MATHEX_SCALAR( 0 ) );

	vec_[ X ] /= v;
	vec_[ Y ] /= v;
	vec_[ Z ] /= v;
}

//////////////////////////////////////////////////////////////////////

// static
MexVec3 MexVec3::crossProduct( const MexVec3& v1, const MexVec3& v2 )
{
    MexVec3 r;

    r.vec_[ X ] = ( v1.y() * v2.z() ) - ( v1.z() * v2.y() );
    r.vec_[ Y ] = ( v1.z() * v2.x() ) - ( v1.x() * v2.z() );
    r.vec_[ Z ] = ( v1.x() * v2.y() ) - ( v1.y() * v2.x() );

    return r;
}

//////////////////////////////////////////////////////////////////////


void MexVec3::crossProduct( const MexVec3& v2 )
{
    MexVec3 r;

    r.vec_[ X ] = ( y() * v2.z() ) - ( z() * v2.y() );
    r.vec_[ Y ] = ( z() * v2.x() ) - ( x() * v2.z() );
    r.vec_[ Z ] = ( x() * v2.y() ) - ( y() * v2.x() );

    *this = r;

    POST( *this == r );
}

//////////////////////////////////////////////////////////////////////


MATHEX_SCALAR MexVec3::dotProduct( const MexVec3& v ) const
{
    return ( x() * v.x() ) + ( y() * v.y() ) + ( z() * v.z() );
}

//////////////////////////////////////////////////////////////////////

bool  MexVec3::isPerpendicularTo( const MexVec3& v ) const
{
    return Mathex::abs( dotProduct( v ) ) < MexEpsilon::instance();
}

//////////////////////////////////////////////////////////////////////

MATHEX_SCALAR MexVec3::modulus() const
{
    return sqrt( ( x() * x() ) + ( y() * y() ) + ( z() * z() ) );
}

//////////////////////////////////////////////////////////////////////

MexRadians MexVec3::angleBetween( const MexVec3& v ) const
{
    PRE( not isZeroVector() );
    PRE( not v.isZeroVector() );

    MATHEX_SCALAR cr = dotProduct( v );
    MexVec3 xp = crossProduct( *this, v );
    MATHEX_SCALAR sr = xp.modulus();

    MexRadians  result( atan2( sr, cr ) );

    POST( MexRadians( 0.0 ) <= result and result <= MexRadians( Mathex::PI ) );

    return result;
}

//////////////////////////////////////////////////////////////////////

MexRadians MexVec3::signedAngleBetween( const MexVec3& v ) const
{
    PRE( not isZeroVector() );
    PRE( not v.isZeroVector() );

    MATHEX_SCALAR cr = dotProduct( v );
    MexVec3 xp = crossProduct( *this, v );
    MATHEX_SCALAR sr = xp.modulus();

    MexRadians  result( atan2( sr, cr ) );

    //  Flip the result depending on the direction of the cross product vector
    if( xp.z() < 0 )
        result = -result;
    else if( xp.z() == 0.0 )
    {
        if( xp.x() < 0 )
            result = -result;
        else if( xp.x() == 0.0 )
        {
            if( xp.y() < 0 )
                result = -result;
        }
    }


    POST( -MexRadians( Mathex::PI ) <= result and result <= MexRadians( Mathex::PI ) );

    return result;
}

//////////////////////////////////////////////////////////////////////

void MexVec3::makeUnitVector()
{
    MATHEX_SCALAR m = modulus();

    PRE( Mathex::abs( m ) != (MATHEX_SCALAR)0 );

    vec_[ X ] /= m;
    vec_[ Y ] /= m;
    vec_[ Z ] /= m;
}

//////////////////////////////////////////////////////////////////////


MATHEX_SCALAR MexVec3::euclidianDistance( const MexVec3& v ) const
{
    MATHEX_SCALAR dx_;
    MATHEX_SCALAR dy_;
    MATHEX_SCALAR dz_;

    dx_ = Mathex::abs( x() - v.x() );
    dy_ = Mathex::abs( y() - v.y() );
    dz_ = Mathex::abs( z() - v.z() );

    return sqrt( ( dx_ * dx_ ) + ( dy_ * dy_ ) + ( dz_ * dz_ ) );
}

//////////////////////////////////////////////////////////////////////

//  Calculate the square of the Euclidian distance
//  between two MexVec3s. This is used in determining
//  the furthest pair of vertices in a model.


MATHEX_SCALAR MexVec3::sqrEuclidianDistance( const MexVec3& v ) const
{
    MATHEX_SCALAR dx_;
    MATHEX_SCALAR dy_;
    MATHEX_SCALAR dz_;

    dx_ = Mathex::abs( x() - v.x() );
    dy_ = Mathex::abs( y() - v.y() );
    dz_ = Mathex::abs( z() - v.z() );

    return ( ( dx_ * dx_ ) + ( dy_ * dy_ ) + ( dz_ * dz_ ) );
}

//////////////////////////////////////////////////////////////////////


void MexVec3::interpolate( const MexVec3& v, MATHEX_SCALAR t )
{
    vec_[ X ] += t * ( v.x() - x() );
    vec_[ Y ] += t * ( v.y() - y() );
    vec_[ Z ] += t * ( v.z() - z() );
}

//////////////////////////////////////////////////////////////////////


void MexVec3::setVector( MATHEX_SCALAR x, MATHEX_SCALAR y, MATHEX_SCALAR z )
{
    vec_[ X ] = x;
    vec_[ Y ] = y;
    vec_[ Z ] = z;
}


ostream& operator <<( ostream& o, const MexVec3& rhs )
{
    return o << "( " << rhs.x() << ", " << rhs.y() << ", " << rhs.z() << " ) ";
}


bool MexVec3::isZeroVector() const
{
	return ( Mathex::abs( x() ) < MexEpsilon::instance() ) and
			( Mathex::abs( y() ) < MexEpsilon::instance() ) and
			( Mathex::abs( z() ) < MexEpsilon::instance() );
}

bool MexVec3::parallel( const MexVec3& v2 ) const
{
    MATHEX_SCALAR l = crossProduct( *this, v2 ).modulus();

    return fabs( l ) < MexEpsilon::instance();
}

bool operator <( const MexVec3& a, const MexVec3& b )
{
    if( a.x() < b.x() )
        return true;
    else if( a.x() > b.x() )
        return false;
    else if( a.y() < b.y() )
        return true;
    else if( a.y() > b.y() )
        return false;
    else if( a.z() < b.z() )
        return true;
    else if( a.z() > b.z() )
        return false;

    return false;
}

PER_DEFINE_PERSISTENT( MexVec3 );

void perWrite( PerOstream& ostr, const MexVec3& t )
{
    PER_WRITE_RAW_DATA( ostr, t.vec_, sizeof( t.vec_ ) );
}

void perRead( PerIstream& istr, MexVec3& t )
{
    PER_READ_RAW_DATA( istr, t.vec_, sizeof( t.vec_ ) );
}


MexVec3::MexVec3( const MexCompressedVec3& vec )
{
    vec_[ X ] = vec.vec_[ 0 ];
    vec_[ Y ] = vec.vec_[ 1 ];
    vec_[ Z ] = vec.vec_[ 2 ];
}
/* End VEC3.CPP *****************************************************/
