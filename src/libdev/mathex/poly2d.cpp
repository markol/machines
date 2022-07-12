/*
 * P O L Y 2 D . C P P
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */

#include <iostream>
#include "mathex/point2d.hpp"
#include "mathex/vec2.hpp"
#include "mathex/poly2d.hpp"
#include "mathex/line2d.hpp"
#include "mathex/abox2d.hpp"

//////////////////////////////////////////////////////////////////////////////////////////

MexPolygon2d::MexPolygon2d(size_t n)
: 	nVertices_(n),
    pEdgeLengths_( NULL ),
    isCachingData_( false )
{
}
//////////////////////////////////////////////////////////////////////////////////////////

MexPolygon2d::MexPolygon2d(	const MexPolygon2d& p )
:	nVertices_(p.nVertices_),
    pEdgeLengths_( NULL ),
    isCachingData_( p.isCachingData_ )
{
}


//////////////////////////////////////////////////////////////////////////////////////////

MexPolygon2d::~MexPolygon2d()
{
    if( pEdgeLengths_ != NULL ) _DELETE( pEdgeLengths_ );
}
//////////////////////////////////////////////////////////////////////////////////////////

size_t MexPolygon2d::nVertices() const
{
	return nVertices_;
}
//////////////////////////////////////////////////////////////////////////////////////////

void MexPolygon2d::nVertices( size_t n )
{
    PRE( n > 2 );
    nVertices_ = n;
}
//////////////////////////////////////////////////////////////////////////////////////////

ostream& operator<<(ostream& os, const MexPolygon2d& poly)
{
	os << "2D poly: " << (void*)&poly;

	for (size_t i=0; i != poly.nVertices(); ++i)
	{
		const MexPoint2d& vtx = poly.vertex(i);
		os << vtx;
	}

	return os << std::endl;
}
//////////////////////////////////////////////////////////////////////////////////////////

void MexPolygon2d::isCachingData( bool doCache ) const
{
    if( doCache != isCachingData_ )
    {
        //set flag
        MexPolygon2d* nonConstThis = (MexPolygon2d*)this;
        nonConstThis->isCachingData_ = doCache;

        //Delete any cached data if not caching any more
        if( not doCache ) clearCachedData();
    }
}
//////////////////////////////////////////////////////////////////////////////////////////

bool MexPolygon2d::isCachingData() const
{
    return isCachingData_;
}
//////////////////////////////////////////////////////////////////////////////////////////

void MexPolygon2d::computeEdgeLengths() const
{
    //See if we already have a vector
    if( pEdgeLengths_ == NULL )
    {
        //No, so create one
        MexPolygon2d* nonConstThis = (MexPolygon2d*)this;
        nonConstThis->pEdgeLengths_ = _NEW( Lengths );
        pEdgeLengths_->reserve( nVertices_ );
    }
    else
    {
        //Yes, so clear it out
        pEdgeLengths_->erase( pEdgeLengths_->begin(), pEdgeLengths_->end() );
    }

    //Compute all the edge lengths and store them in the vector
    const MexPoint2d* p1 = &vertex( 0 );
    const MexPoint2d* p0 = p1;
    for( size_t i = 1; i != nVertices_; ++i )
    {
        const MexPoint2d* p2 = &vertex( i );
        pEdgeLengths_->push_back( p1->euclidianDistance( *p2 ) );
        p1 = p2;
    }

    pEdgeLengths_->push_back( p1->euclidianDistance( *p0 ) );
}
//////////////////////////////////////////////////////////////////////////////////////////

void MexPolygon2d::clearCachedData() const
{
    MexPolygon2d* nonConstThis = (MexPolygon2d*)this;

    //Clear edge lengths vector if stored
    if( pEdgeLengths_ )
    {
        _DELETE( nonConstThis->pEdgeLengths_ );
        nonConstThis->pEdgeLengths_ = NULL;
    }
}
//////////////////////////////////////////////////////////////////////////////////////////

const MexPolygon2d::Lengths& MexPolygon2d::edgeLengths() const
{
    PRE( isCachingData() );

    //Check we have the data
    if( pEdgeLengths_ == NULL ) computeEdgeLengths();

    return *pEdgeLengths_;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool MexPolygon2d::intersects
(
    const MexPoint2d& q1, const MexPoint2d& q2, Mathex::TouchingRule rule
) const
{
    bool result;

    //If penetration required, use special function
    if( rule == Mathex::TOUCH_ISNT_INTERSECT )
        result = isPenetratedBy( q1, q2, q1.euclidianDistance( q2 ) );
    else
    {
        bool hit = false;
        bool inside = true;

        //Ensure the edge lengths are available
        if( pEdgeLengths_ == NULL ) computeEdgeLengths();

        //Check each edge for intersection with the line joining p1 to p2
        const MexPoint2d* pP1 = &vertex( 0 );
        for( size_t i = 0, j = 1; not hit and i != nVertices_ ; ++i )
        {
            //Get the edge as point, unit direction vector and length
            const MexPoint2d* pP2 = &vertex( j );
            MATHEX_SCALAR length =(*pEdgeLengths_)[i];
            MexVec2 d( *pP1, *pP2, length );

            //Check for intersection between Q1Q2 and this edge
            Mathex::Side edgeSide;
            hit = MexLine2d::intersects( *pP1, d, length, q1, q2, &edgeSide );

            //If Q1Q2 lies on the inner side of all the edges, it lies inside
            //the polygon.
            if( edgeSide != Mathex::CHOCOLATE ) inside = false;

            pP1 = pP2;
            if( ++j == nVertices_ ) j = 0;
        }

        //If not caching, clear any data
        if( not isCachingData_ ) clearCachedData();

        result = hit or inside;
    }

    return result;
}

//////////////////////////////////////////////////////////////////////////////////////////

void MexPolygon2d::edgeDirections( ctl_vector< MexVec2 >* directions ) const
{
    PRE( directions != NULL );
    PRE( directions->size() == 0 );
    PRE( nVertices_ > 2 );

    //Ensure the edge lengths vector exists
    if( pEdgeLengths_ == NULL ) computeEdgeLengths();

    //Ensure the vector has sensible allocation size
    directions->reserve( nVertices_ );

    //Run round the edges constructing unit direction vectors
    const MexPoint2d* pP1 = &vertex( 0 );
    for( size_t i = 0, j = 1; i != nVertices_; ++i )
    {
        const MexPoint2d* pP2 = &vertex( j );
        directions->push_back( MexVec2( *pP1, *pP2, (*pEdgeLengths_)[i] ) );

        pP1 = pP2;
        if( ++j == nVertices_ ) j = 0;
    }

    //If not caching, clear any data
    if( not isCachingData_ ) clearCachedData();
}
//////////////////////////////////////////////////////////////////////////////////////////

void MexPolygon2d::boundary( MexAlignedBox2d* pBox ) const
{
    PRE( pBox != NULL );

    MATHEX_SCALAR xMin, xMax, yMin, yMax;

    //Initialise using first vertex
    const MexPoint2d& v0 = vertex( 0 );
    xMin = xMax = v0.x();
    yMin = yMax = v0.y();

    //Consider remaining vertices
    for( size_t i = 1; i != nVertices_; ++i )
    {
        const MexPoint2d& vi = vertex( i );
        MATHEX_SCALAR x = vi.x();
        MATHEX_SCALAR y = vi.y();

        if( x > xMax ) xMax = x;
        else if ( x < xMin ) xMin = x;

        if( y > yMax ) yMax = y;
        else if ( y < yMin ) yMin = y;
    }

    *pBox = MexAlignedBox2d( xMin, yMin, xMax, yMax );
}
//////////////////////////////////////////////////////////////////////////////////////////

bool MexPolygon2d::contains( const MexPoint2d& q ) const
{
    bool inside = true;

    //Ensure the edge lengths are available if meant to be caching data
    if( pEdgeLengths_ == NULL  and  isCachingData_ ) computeEdgeLengths();
    bool haveCachedData = pEdgeLengths_ != NULL;

    //Check the point is not outside each edge
    const MexPoint2d* pP1 = &vertex( 0 );
    for( size_t i = 0, j = 1; i != nVertices_ ; ++i )
    {
        //Get the second edge point
        const MexPoint2d* pP2 = &vertex( j );

        //get which side of the line we're on
        Mathex::Side side;
        if( haveCachedData )
        {
            //Get the edge as point, unit direction vector and length
            MATHEX_SCALAR length =(*pEdgeLengths_)[i];
            MexVec2 d( *pP1, *pP2, length );

            //Check which side of the line Q lies
            MexVec2 pq( *pP1, q );
            MexVec2 n = d.normal();
            MATHEX_SCALAR h = n.dotProduct( pq );
            side = Mathex::side( h );
        }
        else
        {
            side = MexLine2d::side( *pP1, *pP2, q );
        }

        //If Q lies on the inner side of all the edges, it lies inside the polygon.
        if( side == Mathex::VANILLA )
        {
            inside = false;
            break;
        }
        else
        {
            pP1 = pP2;
            if( ++j == nVertices_ ) j = 0;
        }
    }

    return inside;
}
//////////////////////////////////////////////////////////////////////////////////////////

bool MexPolygon2d::intersects( const MexPolygon2d& rhs, Mathex::TouchingRule rule ) const
{
    //Check for the case in which rhs completely contains this polygon.
    bool result;

    if( rule == Mathex::TOUCH_IS_INTERSECT )
        result = rhs.contains( vertex( 0 ) );
    else
    {
        //Need the first edge of this polygon, including its length
        const MexPoint2d& firstPoint = vertex( 0 );
        const MexPoint2d& secondPoint = vertex( 1 );
        MATHEX_SCALAR firstEdgeLength =
            (isCachingData() ? edgeLengths()[0] : firstPoint.euclidianDistance( secondPoint ));
        result = rhs.isPenetratedBy( firstPoint, secondPoint, firstEdgeLength );
    }

    //If this is not the case, check for this polygon being intersected by any edge of rhs
    if( not result )
    {
        //Construct bounding box for this
        MexAlignedBox2d thisBoundary;
        boundary( &thisBoundary );

        //If rhs is caching edge lengths, it helps
        const MexPolygon2d::Lengths* pRhsEdgeLengths =
            (rhs.isCachingData() ? &(rhs.edgeLengths()) : NULL);

        //Check each edge of the rhs polygon to see if it intersects this polygon
        size_t nRhsVertices = rhs.nVertices_;
        for( size_t i = 0, j = 1; i != nRhsVertices; ++i )
        {
            const MexPoint2d& q1 = rhs.vertex( i );
            const MexPoint2d& q2 = rhs.vertex( j );
            if( ++j == nRhsVertices ) j = 0;

            if( thisBoundary.intersects( MexAlignedBox2d( q1, q2 ) ) )
            {
                if( rule == Mathex::TOUCH_IS_INTERSECT )
                    result = intersects( q1, q2 );
                else
                    result = isPenetratedBy( q1, q2, pRhsEdgeLengths == NULL ?
                                                         q1.euclidianDistance( q2 ) :
                                                         (*pRhsEdgeLengths)[i] );

                if( result )
                    break;
            }
        }
    }

    return result;
}

PER_DEFINE_PERSISTENT_ABSTRACT( MexPolygon2d );

void perWrite( PerOstream& stream, const MexPolygon2d& t )
{
	int temp = t.nVertices_;
	stream <<  temp;
}

void perRead( PerIstream& stream, MexPolygon2d& t )
{
	int temp;
	stream >> temp;
	t.nVertices_ = temp;

    t.isCachingData_ = false;
}

//////////////////////////////////////////////////////////////////////////////////////////

bool MexPolygon2d::isPenetratedBy
(
    const MexPoint2d& q1, const MexPoint2d& q2, MATHEX_SCALAR lineLength
) const
{
    PRE( fabs( q1.euclidianDistance( q2 ) - lineLength ) < MexEpsilon::instance() );
    bool result = false;

    //Compute unit direction and normal vectors for the line
    MexVec2 lineDirection( q1, q2, lineLength );
    MexVec2 lineNormal( lineDirection.normal() );

    //Use to store locations of intersections between the polygon and the infinite line
    int nLineIntersections = 0;
    MATHEX_SCALAR sOnLine[2];

    //Load up the data for the final vertex, which might be needed to deal with the
    //first vertex
    size_t n = nVertices();
    MexVec2 vertexVector( q1, vertex( n - 1 ) );
    MATHEX_SCALAR hLastVertex = vertexVector.dotProduct( lineNormal );
    Mathex::Side lastVertexSide = Mathex::side( hLastVertex );
    MATHEX_SCALAR sLastVertex = vertexVector.dotProduct( lineDirection );

    //Process each vertex of the polygon
    for( size_t i = 0;; )
    {
        //Compute normal and parallel distance for this vertex
        MexVec2 vertexVector( q1, vertex( i ) );

        MATHEX_SCALAR h = vertexVector.dotProduct( lineNormal );
        Mathex::Side side = Mathex::side( h );

        MATHEX_SCALAR s = vertexVector.dotProduct( lineDirection );

        //Check the combination of side-of-line values for the current edge
        if( side == Mathex::ONEDGE )
        {
            if( lastVertexSide == Mathex::ONEDGE )
            {
                //The line through Q1 and Q2 passes through an edge of the polygon,
                //and cannot therefore penetrate it.
                break;
            }
            else
            {
                //Store the distance along the line of this intersecting vertex
                sOnLine[nLineIntersections++] = s;
            }
        }
        else if( lastVertexSide != Mathex::ONEDGE  and  lastVertexSide != side )
        {
            //The edge ending at current vertex crosses the line.
            //Find the distance along the line of the point of intersection.
            sOnLine[ nLineIntersections++ ] =
                (hLastVertex * s - h * sLastVertex) / (hLastVertex - h);
        }

        //Once we have 2 line intersections, we know which section of the line
        //lies inside the polygon.
        if( nLineIntersections == 2 )
        {
            //Sort the line intersections into order
            if( sOnLine[0] > sOnLine[1] )
            {
                MATHEX_SCALAR temp = sOnLine[0];
                sOnLine[0] = sOnLine[1];
                sOnLine[1] = temp;
            }

            //The section of the line bounded by these two intersection points must
            //penetrate the polygon. Otherwise it would have to pass through an edge,
            //which is dealt with above. The only issue is whether the section of the
            //infinite line significantly overlaps the section from Q1 to Q2.
            MATHEX_SCALAR eps = MexEpsilon::instance();
            result = sOnLine[1] > eps  and sOnLine[0] < lineLength - eps;
            break;
        }

        //If not the final vertex, store the current vertex info
        if( ++i == n )
            break;
        else
        {
            lastVertexSide = side;
            hLastVertex = h;
            sLastVertex = s;
        }
    }


    return result;
}

bool MexPolygon2d::intersects( size_t vertexIndex, const MexPoint2d& p2 ) const
{
    PRE( vertexIndex < nVertices() );
    // PRE( polygon is convex );

    const size_t nextVertexIndex = ( vertexIndex + 1 ) % nVertices();
    const size_t previousVertexIndex = ( vertexIndex + nVertices() - 1 ) % nVertices();

    const MexPoint2d& thisVertex = vertex( vertexIndex );
    const MexPoint2d& nextVertex = vertex( nextVertexIndex );
    const MexPoint2d& previousVertex = vertex( previousVertexIndex );

    const Mathex::Side side1 = MexLine2d::side( previousVertex, thisVertex, p2 );
    const Mathex::Side side2 = MexLine2d::side( thisVertex, nextVertex, p2 );

    bool result = false;

    if( side1 == Mathex::CHOCOLATE and side2 == Mathex::CHOCOLATE )
    {
        result = true;
    }

    POST_DATA( const MexPoint2d& p1 = vertex( vertexIndex ) );
    POST( result == intersects( p1, p2, Mathex::TOUCH_ISNT_INTERSECT ) );

    return result;
}

bool MexPolygon2d::pointIsVertex( const MexPoint2d& point, size_t* pVertexIndex ) const
{
    PRE( pVertexIndex != NULL );

    bool result = false;

    for( size_t i = 0; i < nVertices() and not result; ++i )
    {
        if( point == vertex( i ) )
        {
            result = true;
            *pVertexIndex = i;
        }
    }

    POST( implies( result, *pVertexIndex < nVertices() ) );

    return result;
}
