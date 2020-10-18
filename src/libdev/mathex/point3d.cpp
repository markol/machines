/*
 * P O I N T 3 D . C P P
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */

/////////////////////////////////////////////////////////

#include "mathex/mathex.hpp"
#include "mathex/point3d.hpp"
#include "mathex/cpoint3d.hpp"
#include "mathex/vec3.hpp"

#ifndef _INLINE
	#include "mathex/point3d.ipp"
#endif /* _INLINE */


MexPoint3d::MexPoint3d( MATHEX_SCALAR x, MATHEX_SCALAR y, MATHEX_SCALAR z )
{
    point_[ X ] = x;
    point_[ Y ] = y;
    point_[ Z ] = z;
}

//////////////////////////////////////////////////////////////////////

MexPoint3d::MexPoint3d( const MexPoint3d& v )
{
    point_[ X ] = v.x();
    point_[ Y ] = v.y();
    point_[ Z ] = v.z();
}

MexPoint3d::MexPoint3d( const MexVec3& v )
{
    point_[ X ] = v.x();
    point_[ Y ] = v.y();
    point_[ Z ] = v.z();
}

//////////////////////////////////////////////////////////////////////

MexPoint3d::~MexPoint3d()
{
    /* intentionally Empty */
}

//////////////////////////////////////////////////////////////////////

const MexPoint3d& MexPoint3d::zeroPoint()
{
    static MexPoint3d zero_ = MexPoint3d( 0, 0, 0 );
    return zero_;
}

//////////////////////////////////////////////////////////////////////

const MexPoint3d& MexPoint3d::operator =( const MexPoint3d& rhs )
{
	point_[ X ] = rhs.x();
	point_[ Y ] = rhs.y();
	point_[ Z ] = rhs.z();
    return *this;
}


const MexPoint3d& MexPoint3d::operator =( const MexVec3& rhs )
{
    point_[ X ] = rhs.x();
    point_[ Y ] = rhs.y();
    point_[ Z ] = rhs.z();
    return *this;
}

//////////////////////////////////////////////////////////////////////


bool  MexPoint3d::operator ==( const MexPoint3d& v ) const
{
    return Mathex::abs( x() - v.x() ) < MexEpsilon::instance()
    		and Mathex::abs( y() - v.y() ) < MexEpsilon::instance()
    		and Mathex::abs( z() - v.z() ) < MexEpsilon::instance();
}

bool  MexPoint3d::operator !=( const MexPoint3d& v ) const
{
    return !(*this == v);
}

//////////////////////////////////////////////////////////////////////


void MexPoint3d::operator +=( const MexVec3& v )
{
    point_[ X ] += v.x();
    point_[ Y ] += v.y();
    point_[ Z ] += v.z();
}

//////////////////////////////////////////////////////////////////////

void MexPoint3d::operator -=( const MexVec3& v )
{
    point_[ X ] -= v.x();
    point_[ Y ] -= v.y();
    point_[ Z ] -= v.z();
}

//////////////////////////////////////////////////////////////////////

MATHEX_SCALAR MexPoint3d::euclidianDistance( const MexPoint3d& v ) const
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

MATHEX_SCALAR MexPoint3d::sqrEuclidianDistance( const MexPoint3d& v ) const
{
    MATHEX_SCALAR dx_;
    MATHEX_SCALAR dy_;
    MATHEX_SCALAR dz_;

    dx_ = Mathex::abs( x() - v.x() );
    dy_ = Mathex::abs( y() - v.y() );
    dz_ = Mathex::abs( z() - v.z() );

    return ( dx_ * dx_ ) + ( dy_ * dy_ ) + ( dz_ * dz_ );
}

//////////////////////////////////////////////////////////////////////

void MexPoint3d::interpolate( const MexPoint3d& v, MATHEX_SCALAR t )
{
    point_[ X ] += t * ( v.x() - x() );
    point_[ Y ] += t * ( v.y() - y() );
    point_[ Z ] += t * ( v.z() - z() );
}

//////////////////////////////////////////////////////////////////////

void MexPoint3d::setPoint( MATHEX_SCALAR x, MATHEX_SCALAR y, MATHEX_SCALAR z )
{
    point_[ X ] = x;
    point_[ Y ] = y;
    point_[ Z ] = z;
}

bool MexPoint3d::isZeroPoint() const
{
	return ( Mathex::abs( x() ) < MexEpsilon::instance() )
		and ( Mathex::abs( y() ) < MexEpsilon::instance() )
		and ( Mathex::abs( z() ) < MexEpsilon::instance() );
}

ostream& operator <<( ostream& o, const MexPoint3d& rhs )
{
    return o << "( " << rhs.x() << ", " << rhs.y() << ", " << rhs.z() << " ) ";
}

/*

bool MexPoint3d::contains( const MexHalfSpace3d& ) const
{
	return false;
}
*/

bool MexPoint3d::contains( const MexPoint3d& v) const
{
	return *this == v;
}

/*
bool MexPoint3d::contains( const MexAlignedBox3d& a ) const
{
  MexPoint3d min(a.minCorner());
  MexPoint3d max(a.maxCorner());

  if 	(x() > min.x() and x() < max.x() and
         y() > min.y() and y() < max.y() and
		 x() > min.z() and x() < max.z())

  return true;

  return false;

}
*/
/*

bool MexPoint3d::contains( const MexUnalignedBox3d& a) const
{
  if (a.vertex(0) != *this)
	return false;


  for (int i = 1; i < a.nVertices();++i)
  {
   if (a.vertex(i) != *this)
	return false;
  }

  return true;
}

*/
/*
bool MexPoint3d::contains( const MexBall3d& b) const
{
   // intentionally empty
   return FALSE;
}
*/

/*

_CODE_INLINE
bool MexPoint3d::intersects( const MexBoundary3d& b, MexIntersectionData* i) const

{
    return b.intersects( *this, i );
}
*/
/*
bool MexPoint3d::intersects( const MexHalfSpace3d& rhs, MexIntersectionData* ) const
{
 //PRE(isValid(b));
 return rhs.contains(*this);
}
*/

bool MexPoint3d::intersects( const MexPoint3d& v ) const
{
   return contains( v );
}
/*
bool MexPoint3d::intersects( const MexAlignedBox3d& a, MexIntersectionData* ) const
{
	return a.contains(*this)  ;
}
*/
/*
bool MexPoint3d::intersects( const MexUnalignedBox3d& u, MexIntersectionData* ) const
{
    return u.contains(*this);
}
*/
/*
bool MexPoint3d::intersects( const MexHalfSpace3d& h, MexIntersectionData* b) const
{
    return h.contains(*this);
}
*/
/*
bool MexPoint3d::intersects( const MexBall3d& ball, MexIntersectionData* b)
{
	PRE(isValid(ball)
	// intentionally empty
	return FALSE;
}
*/

/*/////////////////////////////////////////////////////////////////////////

The rotation is defined as clockwise being positive
when looking in the direction of the vector used to
define the axis.

//////////////////////////////////////////////////////////////////////////*/


void MexPoint3d::rotateAboutAxis(
    const MexVec3& axis,
    const MexPoint3d& pointOnAxis,
    MexRadians angle )

{
    PRE( !axis.isZeroVector() );

    POST_DATA( MexPoint3d originalPoint( *this ) );

    //  This code was originally written for a right handed axis set,
    //  we have to negate the angle to make it work for a left handed
    //  axis set.

    angle = -angle;

	MexRadians      xangle, zangle;
	MATHEX_SCALAR	cz, sz, ca, sa, v0, v1, sx, cx;
	MATHEX_SCALAR	a0, a1, b0, b1, b2, d0, d1, d2;

    MATHEX_SCALAR   newVec[3];

    //  This function has been highly optimised for speed and is
    //  therefore pretty much unreadable. If it ever fails you will
    //  probably have to recode from scratch.

	zangle = atan2 (axis[0], axis[1]);
	cz = cos (zangle);
	sz = sin (zangle);

	xangle = -atan2 (axis[2],(axis[0] * sz + axis[1] * cz));
	sx = sin (xangle);
	cx = cos (xangle);

	ca = cos (angle);
	sa = sin (angle);

	d0 = cz*ca+sz*sx*sa;
	d1 = sz*sx*ca-cz*sa;
	d2 = sz*cx*cx+d1*sx;

	b0 = d2*sz+d0*cz;
	b1 = d2*cz-d0*sz;
	b2 = d1*cx-sz*cx*sx;

    newVec[0] = point_[ X ] * b0;
    newVec[1] = point_[ X ] * b1;
    newVec[2] = point_[ X ] * b2;

	d0 = cz*sx*sa-sz*ca;
	d1 = sz*sa+cz*sx*ca;
	d2 = cz*cx*cx+d1*sx;

	b0 = d2*sz+d0*cz;
	b1 = d2*cz-d0*sz;
	b2 = d1*cx-cz*cx*sx;

    newVec[0] += point_[ Y ] * b0;
    newVec[1] += point_[ Y ] * b1;
    newVec[2] += point_[ Y ] * b2;

	d0 = -sx*cx+cx*ca*sx;

	b0 = d0*sz+cx*sa*cz;
	b1 = d0*cz-cx*sa*sz;
	b2 = sx*sx+cx*ca*cx;

    newVec[0] += point_[ Z ] * b0;
    newVec[1] += point_[ Z ] * b1;
    newVec[2] += point_[ Z ] * b2;

	v0 = -pointOnAxis[0]*cz+pointOnAxis[1]*sz;
	v1 = -pointOnAxis[0]*sz-pointOnAxis[1]*cz;

	a0 = v1*cx+pointOnAxis[2]*sx;
	a1 = v1*sx-pointOnAxis[2]*cx;

	d0 = a1*ca-v0*sa;
	d1 = v0*ca+a1*sa;
	d2 = a0*cx+d0*sx;

	b0 = d2*sz+d1*cz+pointOnAxis[0];
	b1 = d2*cz-d1*sz+pointOnAxis[1];
	b2 = d0*cx-a0*sx+pointOnAxis[2];

    point_[ X ] = newVec[0] + b0;
    point_[ Y ] = newVec[1] + b1;
    point_[ Z ] = newVec[2] + b2;

    //  Calculate the plane going through the original point with the
    //  axis as its normal

    POST_DATA( MATHEX_SCALAR A=axis[0] );
    POST_DATA( MATHEX_SCALAR B=axis[1] );
    POST_DATA( MATHEX_SCALAR C=axis[2] );
    POST_DATA( MATHEX_SCALAR D=-(originalPoint[0]*axis[0]+originalPoint[1]*axis[1]+originalPoint[2]*axis[2]) );

	/*
    cout << "Plane " << A << " " << B << " " << C << " " << D << endl;
    cout << "Transformed point " << *this << endl;
    cout << "A*point_[ X ] + B*point_[ Y ] + C*point_[ Z ] + D " << A*point_[ X ] + B*point_[ Y ] + C*point_[ Z ] + D << endl;
	*/

    //  Make sure that the new point lies on the plane

    POST( ( A*point_[ X ] + B*point_[ Y ] + C*point_[ Z ] + D ) < MexEpsilon::instance() );

    //  Calculate the point on the plane that the vector goes through

    POST_DATA( MATHEX_SCALAR t = - ( A * pointOnAxis.x() + B * pointOnAxis.y() + C * pointOnAxis.z() + D ) /
                       ( A * axis.x() + B * axis.y() + C * axis.z() ) );

    POST_DATA( MexPoint3d rotationPoint(
        pointOnAxis.x() + t * axis.x(),
        pointOnAxis.y() + t * axis.y(),
        pointOnAxis.z() + t * axis.z() ) );

    //  As a double check, make sure that this point is on the plane

    POST( ( A*rotationPoint.x() + B*rotationPoint.y() + C*rotationPoint.z() + D ) < MexEpsilon::instance() );

    //  Calculate the vectors from our rotation point to the original point and
    //  the transformed point

    POST_DATA( MexVec3 vec1( originalPoint ) );
    POST_DATA( vec1 -= rotationPoint );

    POST_DATA( MexVec3 vec2( *this ) );
    POST_DATA( vec2 -= rotationPoint );

    POST( Mathex::abs( vec1.modulus() - vec2.modulus() ) < MexEpsilon::instance() );

    POST_DATA( MexRadians normalisedAngle = angle );
    POST_DATA( while( normalisedAngle < MexRadians( -Mathex::PI ) ) normalisedAngle += MexRadians( 2.0 * Mathex::PI ); );
    POST_DATA( while( normalisedAngle > MexRadians( Mathex::PI ) ) normalisedAngle -= MexRadians( 2.0 * Mathex::PI ); );

//     cout << angle << endl;
//     cout << normalisedAngle << endl;
//     cout << vec1.angleBetween( vec2 ) << endl;
//     cout << vec2.angleBetween( vec1 ) << endl;
//     cout << vec2.angleBetween( vec1 ) - normalisedAngle << endl;

    POST( vec1.isZeroVector() ?
        ( originalPoint == *this ) :
        ( Mathex::abs( (MATHEX_SCALAR)Mathex::abs( vec2.angleBetween( vec1 ).asScalar() )
        					- (MATHEX_SCALAR)Mathex::abs( normalisedAngle.asScalar() ) ) < MexEpsilon::instance() ) );
}

bool operator <( const MexPoint3d& a, const MexPoint3d& b )
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

/* //////////////////////////////////////////////////////////////// */

PER_DEFINE_PERSISTENT( MexPoint3d );

void perWrite( PerOstream& ostr, const MexPoint3d& t )
{
    PER_WRITE_RAW_DATA( ostr, t.point_, sizeof( t.point_ ) );
}

void perRead( PerIstream& istr, MexPoint3d& t )
{
    PER_READ_RAW_DATA( istr, t.point_, sizeof( t.point_ ) );
}

MexPoint3d::MexPoint3d( const MexCompressedPoint3d& p )
{
    point_[ X ] = p.point_[0];
    point_[ Y ] = p.point_[1];
    point_[ Z ] = p.point_[2];
}

/* End POINT3D.CPP **************************************************/
