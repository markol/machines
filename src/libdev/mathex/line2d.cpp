/*
 * L I N E 2 D . C P P
 * (c) Charybdis Limited, 1995, 1996. All Rights Reserved
 */

#include <iostream>

#include "mathex/line2d.hpp"
#include "mathex/epsilon.hpp"
#include "mathex/point2d.hpp"
#include "mathex/vec2.hpp"
#include "mathex/mathex.hpp"
#include "mathex/abox2d.hpp"

MexLine2d::MexLine2d( void ):
	end1_(),
	end2_()
{
}

MexLine2d::MexLine2d( const MexPoint2d& v1, const MexPoint2d& v2 ):
	end1_(v1),
	end2_(v2)
{
}

MexLine2d::MexLine2d( const MexLine2d& rhs):
	end1_(rhs.end1_),
	end2_(rhs.end2_)
{
}

MexLine2d::~MexLine2d()
{
}

const MexLine2d& MexLine2d::operator =( const MexLine2d& rhs)
{
	end1_ = rhs.end1_;
	end2_ = rhs.end2_;
	return *this;
}

void MexLine2d::setLine( const MexPoint2d& v1, const MexPoint2d& v2 )
{
	end1_ = v1;
	end2_ = v2;
}

const MexPoint2d& MexLine2d::end1() const
{
	return end1_;
}

const MexPoint2d& MexLine2d::end2() const
{
	return end2_;
}

ostream& operator<<(ostream& os, const MexLine2d& line)
{
	os << "2D line: " << line.end1() << " to " << line.end2();
	return os;
}

//static
bool MexLine2d::intersects( const MexPoint2d& p, const MexVec2& d, MATHEX_SCALAR length,
                            const MexPoint2d& q1, const MexPoint2d& q2, Mathex::Side* pSide,
                            Mathex::TouchingRule rule )
{
    bool result;

    //Construct utility vectors
    MexVec2 pq1( p, q1 );
    MexVec2 pq2( p, q2 );

    //Compute distances of projections of qi onto the test line
    MATHEX_SCALAR s1 = d.dotProduct( pq1 );
    MATHEX_SCALAR s2 = d.dotProduct( pq2 );

    //Construct normal vector to line, and compute offsets
    MexVec2 n = d.normal();
    MATHEX_SCALAR h1 = n.dotProduct( pq1 );
    MATHEX_SCALAR h2 = n.dotProduct( pq2 );

    //Hence compute the side flags for each qi
    Mathex::Side side1 = Mathex::side( h1 );
    Mathex::Side side2 = Mathex::side( h2 );
    *pSide = Mathex::side( side1, side2 );

    //Take action depending on side flags
    bool touchingBad = rule == Mathex::TOUCH_IS_INTERSECT;
    MATHEX_SCALAR low = (touchingBad ? -MexEpsilon::instance() : MexEpsilon::instance());
    MATHEX_SCALAR high = length - low;

    if( side1 == Mathex::ONEDGE )
    {
        if( touchingBad )
        {
            if( side2 == Mathex::ONEDGE )
                result = not ((s1 > high and s2 > high) or (s1 < low and s2 < low));
            else
                result = s1 < high and s1 > low;
        }
        else
            result = false;
    }
    else if( side2 == Mathex::ONEDGE )
    {
        result = touchingBad and (s2 < high and s2 > low);
    }
    else if( side1 == side2 )
    {
        result = false;
    }
    else
    {
        //Compute the point of intersection between the 2 infinite lines
        MATHEX_SCALAR t = (h1 * s2 - h2 * s1) / (h1 - h2);
        result = t < high and t > low;
    }

    return result;
}

void MexLine2d::boundary( MexAlignedBox2d* pBoundary ) const
{
    MATHEX_SCALAR x1 = end1_.x();
    MATHEX_SCALAR x2 = end2_.x();
    MATHEX_SCALAR y1 = end1_.y();
    MATHEX_SCALAR y2 = end2_.y();

    if( x1 > x2 )
    {
        MATHEX_SCALAR x = x1;
        x1 = x2;
        x2 = x;
    }

    if( y1 > y2 )
    {
        MATHEX_SCALAR y = y1;
        y1 = y2;
        y2 = y;
    }

    pBoundary->corners( MexPoint2d( x1, y1 ), MexPoint2d( x2, y2 ) );
}

Mathex::Side MexLine2d::side( const MexPoint2d& q1, const MexPoint2d& q2,
                              const MexPoint2d& r )
{
    MATHEX_SCALAR q1x = q1.x();
    MATHEX_SCALAR q1y = q1.y();
    MATHEX_SCALAR q2x = q2.x();
    MATHEX_SCALAR q2y = q2.y();
    MATHEX_SCALAR rx = r.x();
    MATHEX_SCALAR ry = r.y();
    MATHEX_SCALAR dx = q2x - q1x;
    MATHEX_SCALAR dy = q2y - q1y;
    MATHEX_SCALAR approxLength = (dx < 0 ? -dx : dx ) + (dy < 0 ? -dy : dy);

    Mathex::Side result;
    if( approxLength < MexEpsilon::instance() )
        result = Mathex::VANILLA;
    else
    {
        MATHEX_SCALAR h = ((dy * (rx - q1x)) + (dx * (q1y - ry))) / approxLength;
        result = Mathex::side( h );
    }

    return result;
}

//static
MATHEX_SCALAR MexLine2d::sqrEuclidianDistance
(
    const MexPoint2d& p1, const MexPoint2d& q1, MATHEX_SCALAR l1,
    const MexPoint2d& p2, const MexPoint2d& q2, MATHEX_SCALAR l2
)
{
    MATHEX_SCALAR epsilon = MexEpsilon::instance();
    PRE( l1 > epsilon );
    PRE( l2 > epsilon );
    PRE( fabs( p1.euclidianDistance( q1 ) - l1 ) < epsilon );
    PRE( fabs( p2.euclidianDistance( q2 ) - l2 ) < epsilon );

    //Compute unit direction and normal vectors for each line
    MexVec2 d1( p1, q1, l1 );
    MexVec2 n1( d1.normal() );
    MexVec2 d2( p2, q2, l2 );
    MexVec2 n2( d2.normal() );

    //Compute the distance of each point along the other line from its P,
    //and its normal distance to the other line
    MexVec2 p1p2( p1, p2 );
    MexVec2 q1q2( q1, q2 );
    MexVec2 p1q2( p1, q2 );
    MexVec2 q1p2( q1, p2 );
    MATHEX_SCALAR s1p2 = d1.dotProduct( p1p2 );
    MATHEX_SCALAR h1p2 = n1.dotProduct( p1p2 );
    MATHEX_SCALAR s1q2 = d1.dotProduct( p1q2 );
    MATHEX_SCALAR h1q2 = n1.dotProduct( p1q2 );

    MATHEX_SCALAR s2p1 = -d2.dotProduct( p1p2 );
    MATHEX_SCALAR h2p1 = -n2.dotProduct( p1p2 );
    MATHEX_SCALAR s2q1 = -d2.dotProduct( q1p2 );
    MATHEX_SCALAR h2q1 = -n2.dotProduct( q1p2 );

    //See if the lines intersect
    MATHEX_SCALAR denom1 = h1p2 - h1q2;
    MATHEX_SCALAR denom2 = h2p1 - h2q1;
    MATHEX_SCALAR result;
    bool linesIntersect = false;

    if( fabs( denom1 ) > epsilon and fabs( denom2 ) > epsilon )
    {
        MATHEX_SCALAR x1 = (s1q2 * h1p2 - s1p2 * h1q2) / denom1;
        if( x1 >= 0.0 and x1 <= l1 )
        {
            MATHEX_SCALAR x2 = (s2q1 * h2p1 - s2p1 * h2q1) / denom2;
            linesIntersect = x2 >= 0.0 and x2 <= l2;
        }
    }

    if( linesIntersect )
        result = 0.0;
    else
    {
        //Compute the square distance of each endpoint from the other line, and take
        //the minimum.
        MATHEX_SCALAR f;

        //Deal with P2 to P1Q1
        if( s1p2 < 0.0 )
            f = p1p2.squareModulus();
        else if( s1p2 > l1 )
            f = q1p2.squareModulus();
        else
            f = fabs( h1p2 * h1p2 );

        result = f;

        //Deal with Q2 to P1Q1
        if( s1q2 < 0.0 )
            f = p1q2.squareModulus();
        else if( s1q2 > l1 )
            f = q1q2.squareModulus();
        else
            f = fabs( h1q2 * h1q2 );

        if( f < result )
            result = f;

        //Deal with P1 to P2Q2
        if( s2p1 < 0.0 )
            f = p1p2.squareModulus();
        else if( s2p1 > l2 )
            f = p1q2.squareModulus();
        else
            f = fabs( h2p1 * h2p1 );

        if( f < result )
            result = f;

        //Deal with Q1 to P2Q2
        if( s2q1 < 0.0 )
            f = q1p2.squareModulus();
        else if( s2q1 > l2 )
            f = q1q2.squareModulus();
        else
            f = fabs( h2q1 * h2q1 );

        if( f < result )
            result = f;
    }

    return result;
}

//static
MATHEX_SCALAR MexLine2d::sqrEuclidianDistance
(
    const MexPoint2d& p1, const MexPoint2d& p2, MATHEX_SCALAR length,
    const MexPoint2d& q
)
{
    PRE_DATA( MATHEX_SCALAR epsilon = MexEpsilon::instance() );
    PRE( length > epsilon );
    PRE( fabs( p1.euclidianDistance( p2 ) - length ) < epsilon );

    //Compute unit direction vector for the line
    MexVec2 d( p1, p2, length );

    //Hence compute distance of projection of Q onto the line from P1
    MexVec2 p1q( p1, q );
    MATHEX_SCALAR s = d.dotProduct( p1q );

    //Hence compute the squared distance
    MATHEX_SCALAR result;

    if( s <= 0 )
        result = p1q.squareModulus();
    else if( s > length )
    {
        MexVec2 p2q( p2, q );
        result = p2q.squareModulus();
    }
    else
    {
        MexVec2 n = d.normal();
        result = n.dotProduct( p1q );
        result *= result;
    }

    return result;
}

PER_DEFINE_PERSISTENT( MexLine2d );

void perWrite( PerOstream& str, const MexLine2d& t )
{
    str << t.end1_;
    str << t.end2_;
}

void perRead( PerIstream& str, MexLine2d& t )
{
    str >> t.end1_;
    str >> t.end2_;
}

bool MexLine2d::intersects( const MexLine2d& line, MexPoint2d* pIntersectPoint ) const
{
	PRE( pIntersectPoint );

	MATHEX_SCALAR line1xvec = end2_.x() - end1_.x();
	MATHEX_SCALAR line1yvec = end2_.y() - end1_.y();
	MATHEX_SCALAR line2xvec = line.end2_.x() - line.end1_.x();
	MATHEX_SCALAR line2yvec = line.end2_.y() - line.end1_.y();
	MATHEX_SCALAR divider = ( line1xvec * line2yvec ) - ( line1yvec * line2xvec );

	if ( divider == 0.0 )
		return false; // Lines do not intersect

	MATHEX_SCALAR eq = ( ( line.end1().x() - end1_.x() ) * line2yvec ) - ( ( line.end1().y() - end1_.y() ) * line2xvec );

	MATHEX_SCALAR x = end1_.x() + ( line1xvec * ( eq / divider ) );
	MATHEX_SCALAR y = end1_.y() + ( line1yvec * ( eq / divider ) );

	pIntersectPoint->x( x );
	pIntersectPoint->y( y );

	return true;
}

bool MexLine2d::isPointOnLine( const MexPoint2d& v ) const
{
	// calculate direction vector
	MATHEX_SCALAR dxVec = end2().x() - end1().x();
	MATHEX_SCALAR dyVec = end2().y() - end1().y();

	// first check to see if the points are the same
	if( end1() == end2() )
		return end1() == v;

	MATHEX_SCALAR t = 0;

	// calculate a value for t whilst being wary of the division by zero
	if( Mathex::abs( dxVec ) > (MATHEX_SCALAR)MexEpsilon::instance() )
	{
		t = ( v.x() - end1().x() ) / dxVec;
	}
	else if( Mathex::abs( dyVec ) > (MATHEX_SCALAR)MexEpsilon::instance() )
	{
		t = ( v.y() - end1().y() ) / dyVec;
	}

	// Point is off line.
	if ( t < 0.0 or t > 1.0 )
		return false;

	dxVec *= t;
	dyVec *= t;

	MexVec2 result( end1().x() + dxVec, end1().y() + dyVec );

	return v.sqrEuclidianDistance( result ) < (MATHEX_SCALAR)MexEpsilon::instance();
}

MATHEX_SCALAR MexLine2d::length() const
{
    //  TBD: Cache the length

    return end1().euclidianDistance( end2() );
}

MexVec2 MexLine2d::unitDirectionVector() const
{
    PRE( length() > 0.0 );

    //Get the length
    MATHEX_SCALAR s = length();

    MexVec2 result( end1(), end2(), s );

    POST( fabs( result.modulus() - 1.0 ) < MexEpsilon::instance() );

    return result;
}

MexPoint2d MexLine2d::pointAtDistance( MATHEX_SCALAR distance ) const
{
    //Get direction vector etc
    MexVec2 d = unitDirectionVector();

    return MexPoint2d( end1_.x() + distance * d.x(),
                       end1_.y() + distance * d.y() );
}

MATHEX_SCALAR MexLine2d::distanceAlongLine(MexPoint2d point)
{
	return unitDirectionVector().dotProduct(MexVec2(end1(), point));
}

bool MexLine2d::segmentIntersects( const MexLine2d& line, MexPoint2d* pIntersectPoint ) const
{
	PRE( pIntersectPoint );

	MATHEX_SCALAR line1xvec = end2_.x() - end1_.x();
	MATHEX_SCALAR line1yvec = end2_.y() - end1_.y();
	MATHEX_SCALAR line2xvec = line.end2_.x() - line.end1_.x();
	MATHEX_SCALAR line2yvec = line.end2_.y() - line.end1_.y();
	MATHEX_SCALAR divider = ( line1xvec * line2yvec ) - ( line1yvec * line2xvec );

	if ( divider == 0.0 )
		return false; // Lines do not intersect

	MATHEX_SCALAR eq = ( ( line.end1().x() - end1_.x() ) * line2yvec ) - ( ( line.end1().y() - end1_.y() ) * line2xvec );

    if( ( eq / divider < 0.0 ) or ( eq / divider > 1.0 ) )
        return false;

	MATHEX_SCALAR x = end1_.x() + ( line1xvec * ( eq / divider ) );
	MATHEX_SCALAR y = end1_.y() + ( line1yvec * ( eq / divider ) );

	pIntersectPoint->x( x );
	pIntersectPoint->y( y );

    //  Check that the point is actually within the range of line as well as in range of this

    if( not line.isPointOnLine( *pIntersectPoint ) )
        return false;

	return true;
}
