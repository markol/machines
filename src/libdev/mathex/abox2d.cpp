/*
 * A B O X 2 D . C P P
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */

//////////////////////////////////////////////////////////////////////

#include <iostream>
#include <algorithm>
#include <vector>

#include "base/base.hpp"
#include "mathex/outcode.hpp"
#include "mathex/mathex.hpp"
#include "mathex/abox2d.hpp"
#include "mathex/cabox2d.hpp"
#include "mathex/point2d.hpp"
#include "mathex/line2d.hpp"
#include "mathex/tri2d.hpp"
#include "mathex/cvexpgon.hpp"

#ifndef _INLINE
    #include "mathex/abox2d.ipp"
#endif

//////////////////////////////////////////////////////////////////////


MexAlignedBox2d::MexAlignedBox2d( void )
:   vmin_( MexPoint2d( 0, 0 ) ),
    vmax_( MexPoint2d( 0, 0 ) )
{
    isEmpty_ = false;
}


MexAlignedBox2d::MexAlignedBox2d
(
	const MexPoint2d& p1,
	const MexPoint2d& p2
):  vmin_( p1 ),
    vmax_( p2 ),
    isEmpty_(false)
{
    MATHEX_SCALAR x1 = p1.x();
    MATHEX_SCALAR y1 = p1.y();
    MATHEX_SCALAR x2 = p2.x();
    MATHEX_SCALAR y2 = p2.y();

    if( x1 > x2 )
    {
        vmin_.x( x2 );
        vmax_.x( x1 );
    }

    if( y1 > y2 )
    {
        vmin_.y( y2 );
        vmax_.y( y1 );
    }
}


MexAlignedBox2d::MexAlignedBox2d
(
	const MexPoint2d& minCorner,
	double width,
	double height
):	vmin_(minCorner),
	vmax_(minCorner.x() + width, minCorner.y() + height),
	isEmpty_(false)
{
	PRE(width  >= 0);
	PRE(height >= 0);
	POST(vmin_.x() <= vmax_.x() && vmin_.y() <= vmax_.y());
}


MexAlignedBox2d::MexAlignedBox2d( const MATHEX_SCALAR x1, const MATHEX_SCALAR y1, const MATHEX_SCALAR x2, const MATHEX_SCALAR y2 )
{
    PRE( x1 <= x2 and y1 <= y2 );

    MexPoint2d v( x1, y1 );
    MexPoint2d v2( x2, y2 );

    vmin_ = v;
    vmax_ = v2;
    isEmpty_ = false;

    POST( vmin_.x() == x1 );
    POST( vmin_.y() == y1 );
    POST( vmax_.x() == x2 );
    POST( vmax_.y() == y2 );
}


MexAlignedBox2d::MexAlignedBox2d( const MexAlignedBox2d& rhs )
:   vmin_( rhs.vmin_ ),
    vmax_( rhs.vmax_ ),
	isEmpty_(rhs.isEmpty_)
{
    POST( *this == rhs );
}



MexAlignedBox2d::~MexAlignedBox2d( ) {    /* intentionally left blank    */  }

//
// bool MexAlignedBox2d::isEmpty( ) const
// {
//     return isEmpty_;
// }


bool MexAlignedBox2d::contains( const MexAlignedBox2d& rhs ) const
{

    return ( vmin_.x() <= rhs.vmin_.x() )  and
           ( vmax_.x() >= rhs.vmax_.x() )  and
           ( vmin_.y() <= rhs.vmin_.y() )  and
           ( vmax_.y() >= rhs.vmax_.y() );
}


bool MexAlignedBox2d::contains( const MexPoint2d& pt ) const
{

    return ( vmin_.x() <= pt.x() )  and
           ( vmax_.x() >= pt.x() )  and
           ( vmin_.y() <= pt.y() )  and
           ( vmax_.y() >= pt.y() );
}


bool MexAlignedBox2d::intersects( const MexAlignedBox2d& rhs, Mathex::TouchingRule rule ) const
{
    //Set the error allowance based on touching rule
    MATHEX_SCALAR eps = MexEpsilon::instance();
    if( rule == Mathex::TOUCH_ISNT_INTERSECT )
        eps = -eps;

    return rhs.vmin_.x() <= vmax_.x() + eps  and
           rhs.vmin_.y() <= vmax_.y() + eps  and
           rhs.vmax_.x() >= vmin_.x() - eps  and
           rhs.vmax_.y() >= vmin_.y() - eps;
}


void MexAlignedBox2d::intersectWith( const MexAlignedBox2d& rhs )
{

    vmin_.x( std::max( vmin_.x(), rhs.vmin_.x() ) );
    vmin_.y( std::max( vmin_.y(), rhs.vmin_.y() ) );

    vmax_.x( std::min( vmax_.x(), rhs.vmax_.x() ) );
    vmax_.y( std::min( vmax_.y(), rhs.vmax_.y() ) );

    isEmpty_ = vmin_.x() > vmax_.x() or vmin_.y() > vmax_.y();
}


void MexAlignedBox2d::unionWith( const MexAlignedBox2d& rhs )
{

    vmin_.x( std::min( vmin_.x(), rhs.vmin_.x() ) );
    vmin_.y( std::min( vmin_.y(), rhs.vmin_.y() ) );

    vmax_.x( std::max( vmax_.x(), rhs.vmax_.x() ) );
    vmax_.y( std::max( vmax_.y(), rhs.vmax_.y() ) );
}


const MexAlignedBox2d& MexAlignedBox2d::operator =( const MexAlignedBox2d& b )
{

    vmin_ = b.vmin_;
    vmax_ = b.vmax_;
	isEmpty_ = b.isEmpty_;

    POST( *this == b );

    return *this;
}


bool MexAlignedBox2d::operator ==( const MexAlignedBox2d& b ) const
{

    return ( vmin_ == b.vmin_ ) and ( vmax_ == b.vmax_ ) and ( isEmpty_ == b.isEmpty_ );
}

const MexPoint2d& MexAlignedBox2d::minCorner( ) const
{
    const MexPoint2d& result = vmin_;

    POST( &result == &vmin_ );

    return result;
}


const MexPoint2d& MexAlignedBox2d::maxCorner( ) const
{
    const MexPoint2d& result = vmax_;

    POST( &result == &vmax_ );

    return result;
}


/*-----------------------------------------------------------------------------**
** Here is an implementation of a Cohen-Sutherland clip clipper in C++.        **
** This routine is designed to return the value 1 if the line segment can be   **
** clipped to produce a visible OutCode.                                       **
**-----------------------------------------------------------------------------*/
// In this application, we don't actually require the intersection points per
// se.  We only need to know if there is an intersection.  Hence, it might be
// possible to optimise this further.  It hasn't been done for risk of screwing-
// up the algorithm.
static bool CohenSutherlandClip
(
  const MexAlignedBox2d& box,   // the box to test against
  MexPoint2d pt1,				   // one end of the line
  MexPoint2d pt2,		 		   // the other end of the line
  OutCode P,					   // the outcode for pt1
  OutCode Q					   // the outcode for pt2
)
{
	// Use the same names as Foley & Van Dam.
	const MATHEX_SCALAR xmin = box.minCorner().x();
	const MATHEX_SCALAR ymin = box.minCorner().y();
	const MATHEX_SCALAR xmax = box.maxCorner().x();
	const MATHEX_SCALAR ymax = box.maxCorner().y();
	MATHEX_SCALAR x0 = pt1[0];
	MATHEX_SCALAR y0 = pt1[1];
	MATHEX_SCALAR x1 = pt2[0];
	MATHEX_SCALAR y1 = pt2[1];

	if (P.is_inside() || Q.is_inside())
		return true;

	if (P.same_side_as(Q))
		return false;

	// This isn't part of the standard algorithm, but it is a trivial
	// test for an intersection, and so is worth doing.  Also, it
	// allows us to precalculate the slopes below because, together
	// with the same_side_as test above, it rules out horizontal &
	// vertical lines.
	if (P.is_directly_opposite(Q))
	  return true;

	// The above tests means that the line can't be horizontal
	// or vertical.  This *must* be checked before computing slope.
	// Assert: the line is neither horizontal or vertical.
	ASSERT_INFO(pt1);
	ASSERT_INFO(pt2);
	ASSERT_INFO(box);
	ASSERT(pt1.x() != pt2.x(), logic_error());
	ASSERT(pt1.y() != pt2.y(), logic_error());
	const double       slope = (y1 - y0) / (x1 - x0);
	const double recip_slope = 1 / slope;

  while (P.is_outside() || Q.is_outside())  {
    if (P.same_side_as(Q))  return false;

	  if (P.is_outside())  {
	    if (P.is_left())  {
	      pt1.y( y0 + slope * (xmin - x0) );
	      pt1.x( xmin );
	    }
	    else if (P.is_right())  {
	      pt1.y( y0 + slope * (xmax - x0) );
	      pt1.x( xmax );
	    }
	    else if (P.is_above())  {
	      pt1.x( x0 + recip_slope * (ymax - y0) );
	      pt1.y( ymax );
	    }
	    else if (P.is_below())  {
	      pt1.x( x0 + recip_slope * (ymin - y0) );
	      pt1.y( ymin );
	    }
	    P = OutCode(box, pt1);
	  }

    if (P.same_side_as(Q))  return false;

    if (Q.is_outside())  {
	    if (Q.is_left())  {
	      pt2.y( y0 + slope * (xmin - x0) );
	      pt2.x( xmin );
	    }
	    else if (Q.is_right())  {
	      pt2.y( y0 + slope * (xmax - x0) );
	      pt2.x( xmax );
	    }
	    else if (Q.is_above())  {
	      pt2.x( x0 + recip_slope * (ymax - y0) );
	      pt2.y( ymax );
	    }
	    else if (Q.is_below())  {
	      pt2.x( x0 + recip_slope * (ymin - y0) );
	      pt2.y( ymin );
	    }
      Q = OutCode(box, pt2);
    }
  }
  return true;
}

// A home-grown algorithm which determines if a polygon intersects an axis-aligned
// box.  This is based on the Sutherland-Cohen line clipping algorithm.
// It tests three major cases:
//		1) if any poly vertex is inside the box, then true
//		2) "   "   "   edge  intersects  "   "    "    "
//	 	3) if the poly completely encloses the box, then true
//		4) otherwise, false.
// The way this is written, it should work for any convex polygon.
// It would be nice to generalise the argument to allow an
// abstract polygon.  However, there is no way to guarantee that such a
// poly will be convex.
bool MexAlignedBox2d::intersects( const MexTriangle2d& poly ) const
{
	// First determine the out-code for every polygon vertex.
	std::vector<OutCode> codes(poly.nVertices());

	for (int i=0; i!=poly.nVertices(); ++i)
	{
		const OutCode c(*this, poly.vertex(i));

		// If any vertex is inside the box, we can trivially say there is
		// an intersection.
		if (c.is_inside())
			return true;

		codes[i] = c;
	}

	// Having done all the trivial tests, resort to the standard Cohen-Sutherland
	// algorithm to see if we have any intersections.
	for (int i=0; i!=poly.nVertices(); ++i)
	{
		const int i2 = (i+1) % poly.nVertices();
		const MexPoint2d& v1 = poly.vertex(i);
		const MexPoint2d& v2 = poly.vertex(i2);
		if (CohenSutherlandClip(*this, v1, v2, codes[i], codes[i2]))
			return true;
	}

	// There are no intersections whatsoever, so is the box completely
	// within the polygon?  Try the point-in-poly test on any corner of
	// the box.  (There are no edge-box intersections, so the box's
	// corners are either all completely outside the poly or all
	// completely inside.  Hence, we only need to test one corner.)
	if (poly.contains(minCorner()))
		return true;

	return false;
}


bool MexAlignedBox2d::intersects( const MexLine2d& line ) const
{
	const OutCode c1(*this, line.end1());
	const OutCode c2(*this, line.end2());
	return CohenSutherlandClip(*this, line.end1(), line.end2(), c1, c2);
}

ostream& operator<<(ostream& os, const MexAlignedBox2d& box)
{
	os << "2D box: " << box.minCorner() << " to " << box.maxCorner();
	return os;
}

void MexAlignedBox2d::corners( const MexPoint2d& newMinCorner, const MexPoint2d& newMaxCorner )
{
    PRE( newMinCorner.x() <= newMaxCorner.x() )
    PRE( newMinCorner.y() <= newMaxCorner.y() )
    vmin_ = newMinCorner;
    vmax_ = newMaxCorner;
}

MexAlignedBox2d::MexAlignedBox2d
(
    const MexPoint2d& p1, const MexPoint2d& p2, MATHEX_SCALAR clearance
)
:   isEmpty_( false )
{
    PRE( clearance >= 0 );

    MATHEX_SCALAR x1 = p1.x();
    MATHEX_SCALAR y1 = p1.y();
    MATHEX_SCALAR x2 = p2.x();
    MATHEX_SCALAR y2 = p2.y();

    MATHEX_SCALAR xMin, xMax, yMin, yMax;
    if( x1 < x2 )
    {
        xMin = x1;
        xMax = x2;
    }
    else
    {
        xMin = x2;
        xMax = x1;
    }

    if( y1 < y2 )
    {
        yMin = y1;
        yMax = y2;
    }
    else
    {
        yMin = y2;
        yMax = y1;
    }

    vmin_.x( xMin - clearance);
    vmin_.y( yMin - clearance );
    vmax_.x( xMax + clearance );
    vmax_.y( yMax + clearance );

    TEST_INVARIANT
}

MexAlignedBox2d::MexAlignedBox2d( const MexPoint2d& centre, MATHEX_SCALAR halfSideLength )
:   vmin_( centre.x() - halfSideLength, centre.y() - halfSideLength ),
    vmax_( centre.x() + halfSideLength, centre.y() + halfSideLength ),
    isEmpty_( false )
{
    PRE( halfSideLength > 0 );
}

MexAlignedBox2d::MexAlignedBox2d( const MexCompressedAlignedBox2d& t):
    vmin_(t.vmin_),
    vmax_(t.vmax_),
    isEmpty_(t.isEmpty_)
{
}

PER_DEFINE_PERSISTENT( MexAlignedBox2d );

void perWrite( PerOstream& str, const MexAlignedBox2d& t )
{
    str << t.vmin_;
    str << t.vmax_;
    str << t.isEmpty_;
}

void perRead( PerIstream& str, MexAlignedBox2d& t )
{
    str >> t.vmin_;
    str >> t.vmax_;
    str >> t.isEmpty_;
}

MexPoint2d MexAlignedBox2d::nearestPointToExternalPoint( const MexPoint2d& externalPoint ) const
{
	PRE( not ( 		externalPoint.x() > minCorner().x()
			   	and externalPoint.x() < maxCorner().x()
			   	and externalPoint.y() > minCorner().y()
			   	and externalPoint.y() < maxCorner().y() ) );

	MATHEX_SCALAR xVal;
	MATHEX_SCALAR yVal;

	if( externalPoint.x() >= minCorner().x() and externalPoint.x() <= maxCorner().x() )
	{
		// nearest point must be on one of the horizontal lines
		if( externalPoint.y() <= minCorner().y() )
			yVal = minCorner().y();
		else
			yVal = maxCorner().y();

		xVal = externalPoint.x();
	}
	else if( externalPoint.y() >= minCorner().y() and externalPoint.y() <= maxCorner().y() )
	{
		// nearest point must be on one of the vertical lines
		if( externalPoint.x() <= minCorner().x() )
			xVal = minCorner().x();
		else
			xVal = maxCorner().x();

		yVal = externalPoint.y();
	}
	else
	{
		// nearest point must be one of the four corners
		if( externalPoint.x() <= minCorner().x() )
			xVal = minCorner().x();
		else
			xVal = maxCorner().x();

		if( externalPoint.y() <= minCorner().y() )
			yVal = minCorner().y();
		else
			yVal = maxCorner().y();
	}

	return MexPoint2d( xVal, yVal );
}

bool operator <( const MexAlignedBox2d& a, const MexAlignedBox2d& b )
{
    if( a.vmin_ < b.vmin_ )
        return true;
    else if( a.vmin_ > b.vmin_ )
        return false;
    else if( a.vmax_ < b.vmax_ )
        return true;

    return false;
}

void MexAlignedBox2d::unionWith( const MexConvexPolygon2d& rhs )
{
    for( size_t i = 0; i < rhs.nVertices(); ++i )
    {
        vmin_.x( std::min( vmin_.x(), rhs.vertex( i ).x() ) );
        vmin_.y( std::min( vmin_.y(), rhs.vertex( i ).y() ) );

        vmax_.x( std::max( vmax_.x(), rhs.vertex( i ).x() ) );
        vmax_.y( std::max( vmax_.y(), rhs.vertex( i ).y() ) );
    }
}


/* End ABOX2D.CPP *****************************************************/
