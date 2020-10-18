/*
 * U T I L I T Y . C P P
 * (c) Charybdis Limited, 1995, 1996, 1997, 1998. All Rights Reserved
 */

#include <iomanip>

#include "base/base.hpp"
#include "mathex/utility.hpp"
#include "mathex/point2d.hpp"
#include "mathex/line2d.hpp"
#include "mathex/point3d.hpp"
#include "mathex/vec2.hpp"
#include "mathex/poly2d.hpp"

// static
bool MexUtility::inStraightLineXY( const ctl_vector< MexTransform3d >& transforms )
{
	bool	result;

	if( transforms.size() <= 2 )
		result = true;
	else
	{
		result = true;

		for( size_t i = 0; i < transforms.size() - 2 and result; ++i )
			result = inStraightLine( transforms[ i ].position(), transforms[ i + 1 ].position(), transforms[ i + 2 ].position() );
	}

	return result;
}

// static
bool MexUtility::inStraightLine(
  const MexPoint2d& p1,
  const MexPoint2d& p2,
  const MexPoint2d& p3 )
{
	bool	result;

    ASSERT_INFO( p1 );
    ASSERT_INFO( p2 );
    ASSERT_INFO( p3 );

	if( p1 == p2 or p1 == p3 or p2 == p3 )
		result = true;
	else
	{
		MATHEX_SCALAR dx = p2.x() - p1.x();
		MATHEX_SCALAR dy = p2.y() - p1.y();

        ASSERT_INFO( dx );
        ASSERT_INFO( dy );

		MATHEX_SCALAR t = 0;

		// calculate a value for t whilst being wary of the division by zero
		if( Mathex::abs( dx ) > (MATHEX_SCALAR)MexEpsilon::instance() )
		{
			t = ( p3.x() - p1.x() ) / dx;
		}
		else if( Mathex::abs( dy ) > (MATHEX_SCALAR)MexEpsilon::instance() )
		{
			t = ( p3.y() - p1.y() ) / dy;
		}
		else
		{
			ASSERT_FAIL( "" );
		}

        ASSERT_INFO( t );

		dx *= t;
		dy *= t;

        ASSERT_INFO( dx );
        ASSERT_INFO( dy );

		MexPoint2d p( p1.x() + dx, p1.y() + dy );

        ASSERT_INFO( p );
        ASSERT_INFO( p.sqrEuclidianDistance( p3 ) );

		result = p.sqrEuclidianDistance( p3 ) < MexEpsilon::instance();
	}

	return result;
}

// static
uint MexUtility::quadraticRealRoots( MATHEX_SCALAR a, MATHEX_SCALAR b, MATHEX_SCALAR c,
      MATHEX_SCALAR* pRoot1, MATHEX_SCALAR* pRoot2 )
{
    PRE_INFO( a );
    PRE_INFO( b );
    PRE_INFO( c );

    PRE( implies( fabs( a ) < MexEpsilon::instance(), fabs( b ) > MexEpsilon::instance() ) );

    uint    nRoots;

    if( fabs( a ) < MexEpsilon::instance() )
    {
        nRoots = 1;
        *pRoot1 = - c / b;
    }
    else
    {
        MATHEX_SCALAR t = b * b - 4 * a * c;

        ASSERT_INFO( t );

        if( t > 0.0 )
        {
            //  Easy case - 2 roots
            MATHEX_SCALAR t1 = sqrt( t );
            ASSERT_INFO( t1 );

            nRoots = 2;
            *pRoot1 = ( -b + t1 ) / ( 2 * a );
            *pRoot2 = ( -b - t1 ) / ( 2 * a );
        }
        else if( t == 0.0 )
        {
            //  Easy case - 1 root
            nRoots = 1;
            *pRoot1 = -b / ( 2 * a );
        }
        else
        {
            //  More difficult case. Due to rounding errors, there might
            //  be one root if the turning point on the quadratic is close
            //  enough to zero.

            MATHEX_SCALAR x = -b / ( 2 * a );
            MATHEX_SCALAR y = a * x * x + b * x + c;

            ASSERT_INFO( x );
            ASSERT_INFO( y );

            if( fabs( y ) < MexEpsilon::instance() )
            {
                nRoots = 1;
                *pRoot1 = x;
            }
            else
                nRoots = 0;
        }
    }

    POST_INFO( nRoots );
    POST_INFO( *pRoot1 );
    POST_INFO( *pRoot2 );
    POST_INFO( a * *pRoot1 * *pRoot1 + b * *pRoot1 + c );
    POST_INFO( a * *pRoot2 * *pRoot2 + b * *pRoot2 + c );

    POST( implies( nRoots >= 1, fabs( a * *pRoot1 * *pRoot1 + b * *pRoot1 + c ) < MexEpsilon::instance() ) );
    POST( implies( nRoots == 2, fabs( a * *pRoot2 * *pRoot2 + b * *pRoot2 + c ) < MexEpsilon::instance() ) );

    return nRoots;
}

void MexUtility::slideAlongPolygon
(
    const MexPolygon2d& polygon, const MexPoint2d& startPoint, const MexVec2& motionDirection,
    MATHEX_SCALAR distance, Point2ds* pSlidePoints
)
{
    //Set up line in direction of motion
    MexVec2 offset( motionDirection );
    offset *= distance;
    MexPoint2d destinationPoint( startPoint );
    destinationPoint += offset;

    MexLine2d motionLine( startPoint, destinationPoint );

    //Deal with each edge of the polygon
    const uint nEdges = polygon.nVertices();

	for( uint i = 0; i < nEdges; ++i )
	{
		// Construct obstacle edge
		MexPoint2d edgePoint1 = polygon.vertex( i );
		MexPoint2d edgePoint2 = ( i == nEdges - 1 ) ? polygon.vertex( 0 ) : polygon.vertex( i + 1 );
		MexLine2d edge = MexLine2d( edgePoint1, edgePoint2 );

        //Reject edges if the start point is on the inside side
        if( MexLine2d::side( edgePoint1, edgePoint2, startPoint ) == Mathex::VANILLA )
        {
            //Get the intersection between the line of motion and the edge, using infinite lines
    		MexPoint2d intersectPoint;
            if( edge.intersects( motionLine, &intersectPoint ) )
            {
                //Slide along the edge we have detected. Need a unit edge direction vector.
                MexVec2 edgeVector( edgePoint1, edgePoint2 );
                edgeVector.makeUnitVector();

                //We want to slide in the natural direction, so choose the direction with the
                //positive cosine.
                if( edgeVector.dotProduct( motionDirection ) < 0.0 )
                    edgeVector *= -1.0;

                //Now slide through required distance in that direction
                MexPoint2d slidePoint( startPoint );
                edgeVector *= distance;
                slidePoint += edgeVector;
                pSlidePoints->push_back( slidePoint );
            }
        }
    }
}
