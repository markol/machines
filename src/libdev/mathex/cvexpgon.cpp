/*
 * C V E X P G O N . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "base/base.hpp"
//#include "stdlib/memory.hpp"
#include <memory>
#include "ctl/vector.hpp"

#include "mathex/cvexpgon.hpp"
#include "mathex/point2d.hpp"
#include "mathex/vec2.hpp"
#include "mathex/abox2d.hpp"
#include "mathex/transf3d.hpp"

#ifndef _INLINE
    #include "mathex/cvexpgon.ipp"
#endif

/* //////////////////////////////////////////////////////////////////////////////////// */

MexConvexPolygon2d::MexConvexPolygon2d( const Points& points )
:   MexPolygon2d( points.size() )
{
    PRE( points.size() > 2 );

    //Construct a vector and push copies of all the points onto it
    size_t n = nVertices();
    pPoints_ = _NEW( Points );
    pPoints_->reserve( n );

    for( size_t i = 0; i != n; ++i )
    {
        pPoints_->push_back( points[i] );
    }

    TEST_INVARIANT;

	POST( allVerticesDiscrete() );
	POST( isClockwiseAndConvex() );

}

/* //////////////////////////////////////////////////////////////////////////////////// */

MexConvexPolygon2d::MexConvexPolygon2d( const ctl_vector< MexPoint2d* >& points )
:   MexPolygon2d( points.size() )
{
    PRE( points.size() > 2 );

    //Construct a vector and push copies of all the points onto it
    size_t n = nVertices();
    pPoints_ = _NEW( Points );
    pPoints_->reserve( n );

    for( size_t i = 0; i != n; ++i )
    {
        pPoints_->push_back( MexPoint2d( *points[i] ) );
    }

    TEST_INVARIANT;

	POST( allVerticesDiscrete() );
    POST( isClockwiseAndConvex() );
}

/* //////////////////////////////////////////////////////////////////////////////////// */

MexConvexPolygon2d::MexConvexPolygon2d( std::auto_ptr< Points >& pointsAPtr )
:   MexPolygon2d( pointsAPtr->size() ),
    pPoints_( pointsAPtr.release() )
{
    TEST_INVARIANT;

	POST( allVerticesDiscrete() );
    POST( isClockwiseAndConvex() );
}

/* //////////////////////////////////////////////////////////////////////////////////// */

MexConvexPolygon2d::MexConvexPolygon2d( const MexPolygon2d& rhs, MATHEX_SCALAR expansionDistance )
:   MexPolygon2d( rhs.nVertices() )
{
    PRE( expansionDistance > 0 );

    //Ensure rhs can give access to the edge data
    bool rhsIsCachingData = rhs.isCachingData();
    if( not rhsIsCachingData ) rhs.isCachingData( true );

    //Fill some vectors with unit edge direction and normal vectors
    size_t n = nVertices();
    ctl_vector< MexVec2 > directions;
    ctl_vector< MexVec2 > normals;
    directions.reserve( n );
    normals.reserve( n );

    rhs.edgeDirections( &directions );
    for( size_t i = 0; i != n; ++i )
        normals.push_back( directions[i].normal() );

    //For each vertex, calculate the lambda value, and decide how many new vertices
    MATHEX_SCALAR epsilon = MexEpsilon::instance();
    MATHEX_SCALAR overshootLimit = expansionDistance * 1.2;
    ctl_vector< MATHEX_SCALAR > lambdas;
    lambdas.reserve( n );
    ctl_vector< size_t > newVertexCounts;
    newVertexCounts.reserve( n );
    size_t totalNewVertices = 0;
    const MexVec2* pD0 = &directions[ n - 1 ];
    const MexVec2* pN0 = &normals[ n - 1 ];

    for( size_t i = 0; i != n; ++i )
    {
        ASSERT_INFO(rhs.vertex( i ) );
        const MexVec2* pD1 = &directions[i];
        const MexVec2* pN1 = &normals[i];
        size_t count;

        //Check for parallel edges, in which case we don't generate any vertex
        MATHEX_SCALAR lambda;
        MATHEX_SCALAR denom = pD0->dotProduct( *pN1 );
        if( fabs( denom ) < epsilon )
        {
            count = 0;
            lambda = 0.0; //not used
        }
        else
        {
            lambda = expansionDistance * (1.0 - pN0->dotProduct( *pN1 )) / denom;
            if( lambda < overshootLimit ) count = 1;
            else
            {
                //Generate 2 vertices. Recompute lambda.
                count = 2;
                MATHEX_SCALAR l = sqrt( lambda * lambda + expansionDistance * expansionDistance );
                lambda = (l - expansionDistance ) * ( expansionDistance / lambda );
            }
        }

        lambdas.push_back( lambda );
        newVertexCounts.push_back( count );
        totalNewVertices += count;

        pD0 = pD1;
        pN0 = pN1;
    }

    //Generate the vertices
    pPoints_ = _NEW( Points );
    pPoints_->reserve( totalNewVertices );
    nVertices( totalNewVertices );

    pD0 = &directions[ n - 1 ];
    pN0 = &normals[ n - 1 ];

    for( size_t i = 0; i != n; ++i )
    {
        const MexVec2* pD1 = &directions[i];
        const MexVec2* pN1 = &normals[i];
        const MexPoint2d& oldVertex = rhs.vertex( i );
        size_t count = newVertexCounts[i];

        if( count != 0 )
        {
            MATHEX_SCALAR lambda = lambdas[i];
            MATHEX_SCALAR x = oldVertex.x() + expansionDistance * pN0->x() + lambda * pD0->x();
            MATHEX_SCALAR y = oldVertex.y() + expansionDistance * pN0->y() + lambda * pD0->y();

            pPoints_->push_back( MexPoint2d( x, y ) );

            if( count == 2 )
            {
                x = oldVertex.x() + expansionDistance * pN1->x() - lambda * pD1->x();
                y = oldVertex.y() + expansionDistance * pN1->y() - lambda * pD1->y();

                pPoints_->push_back( MexPoint2d( x, y ) );
            }
        }

        pD0 = pD1;
        pN0 = pN1;
    }

    //Clear rhs cache data if created just for this function call
    if( not rhsIsCachingData ) rhs.isCachingData( false );

	POST( allVerticesDiscrete() );
    POST( isClockwiseAndConvex() );
}

/* //////////////////////////////////////////////////////////////////////////////////// */

MexConvexPolygon2d::MexConvexPolygon2d( const MexConvexPolygon2d& copyMe )
: MexPolygon2d( copyMe )
{
    //Construct a vector and push copies of all the points onto it
    size_t n = nVertices();
    pPoints_ = _NEW( Points );
    pPoints_->reserve( n );

    for( size_t i = 0; i != n; ++i )
    {
        pPoints_->push_back( (*copyMe.pPoints_)[i] );
    }

    TEST_INVARIANT;

    POST( allVerticesDiscrete() );
    POST( isClockwiseAndConvex() );
}

/* //////////////////////////////////////////////////////////////////////////////////// */

MexConvexPolygon2d::MexConvexPolygon2d( const MexPoint2d& p1, const MexPoint2d& p2,
                                        MATHEX_SCALAR expansionDistance )
:   MexPolygon2d( 4 )
{
    PRE( expansionDistance > 0 );

    //Construct a vector of size expansionDistance in the direction p1 to p2
    MATHEX_SCALAR separation = p1.euclidianDistance( p2 );
    PRE( separation != 0 );
    MexVec2 d( p1, p2, separation / expansionDistance );
    MexVec2 n = d.normal();

    //Create the points in the right order
    pPoints_ = _NEW( Points );
    pPoints_->reserve( 4 );
    pPoints_->push_back( MexPoint2d( p1.x() - d.x() + n.x(), p1.y() - d.y() + n.y() ) );
    pPoints_->push_back( MexPoint2d( p2.x() + d.x() + n.x(), p2.y() + d.y() + n.y() ) );
    pPoints_->push_back( MexPoint2d( p2.x() + d.x() - n.x(), p2.y() + d.y() - n.y() ) );
    pPoints_->push_back( MexPoint2d( p1.x() - d.x() - n.x(), p1.y() - d.y() - n.y() ) );

    POST( isClockwiseAndConvex() );
}

/* //////////////////////////////////////////////////////////////////////////////////// */

MexConvexPolygon2d::~MexConvexPolygon2d()
{
    TEST_INVARIANT;

    if( pPoints_ != NULL ) _DELETE( pPoints_ );
}

/* //////////////////////////////////////////////////////////////////////////////////// */

//////////////////////////////////////////////////////////////////////////////////////////

void MexConvexPolygon2d::CLASS_INVARIANT
{
    //Must have at least 3 vertices
    INVARIANT( pPoints_ != NULL );
    INVARIANT( pPoints_->size() > 2 );
    INVARIANT( allVerticesDiscrete() );
	INVARIANT( isClockwiseAndConvex() );
}

//////////////////////////////////////////////////////////////////////////////////////////

bool MexConvexPolygon2d::operator ==( const MexConvexPolygon2d& p )
{
	return pPoints_ == p.pPoints_;
}

//////////////////////////////////////////////////////////////////////////////////////////

MexConvexPolygon2d& MexConvexPolygon2d::operator =( const MexConvexPolygon2d& copyMe )
{
	pPoints_ = copyMe.pPoints_;
	POST(*this == copyMe);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////////////////

//virtual
const MexPoint2d& MexConvexPolygon2d::vertex( size_t i ) const
{
    PRE( i < nVertices() );
    return (*pPoints_)[i];
}

//////////////////////////////////////////////////////////////////////////////////////////

bool MexConvexPolygon2d::isClockwiseAndConvex() const
{
   	PRE( pPoints_->size() > 2 );

    size_t nPoints = pPoints_->size();

	// test each triplet of vertices (except special last case - done after) as being clockwise

	for (size_t triplet=0; triplet<nPoints-2; ++triplet)
	{
		const MexPoint2d& q0 = vertex( triplet );
   		const MexPoint2d& q1 = vertex( triplet+1 );
   		const MexPoint2d& q2 = vertex( triplet+2 );

		ASSERT_INFO( triplet );
		ASSERT_INFO( q0 );
		ASSERT_INFO( q1 );
		ASSERT_INFO( q2 );

	    //z component of cross product of Q0Q1 with Q0Q2 must be positive
		if ( (q1.x() - q0.x()) * (q2.y() - q0.y()) - (q2.x() - q0.x()) * (q1.y() - q0.y()) <= 0 )
			return false;
	}

	// if this is a triangle and it's passed its test on the first vertex, the other 2 must be fine.

	if ( nPoints == 3 )
		return true;

	//  check triplet of penultimate, last and first vertex as being clockwise

    const MexPoint2d& p0 = vertex( nPoints-2 );
    const MexPoint2d& p1 = vertex( nPoints-1 );
    const MexPoint2d& p2 = vertex( 0 );

	ASSERT_INFO( "Penultimate triplet " );
	ASSERT_INFO( p0 );
	ASSERT_INFO( p1 );
	ASSERT_INFO( p2 );

	//z component of cross product of p0p1 with p0p2 must be positive
    if ( (p1.x() - p0.x()) * (p2.y() - p0.y()) - (p2.x() - p0.x()) * (p1.y() - p0.y()) <= 0 )
			return false;

	//  check triplet of last, first and second vertex as being clockwise
	//	if it is - hey presto! - the whole shabang is convex. No quibbles.

    const MexPoint2d& u0 = vertex( nPoints-1 );
    const MexPoint2d& u1 = vertex( 0 );
    const MexPoint2d& u2 = vertex( 1 );

	ASSERT_INFO( "Final triplet " );
	ASSERT_INFO( u0 );
	ASSERT_INFO( u1 );
	ASSERT_INFO( u2 );

	//z component of cross product of u0u1 with u0u2 must be positive
    return (u1.x() - u0.x()) * (u2.y() - u0.y()) - (u2.x() - u0.x()) * (u1.y() - u0.y()) > 0;
}

//////////////////////////////////////////////////////////////////////////////////////////

bool MexConvexPolygon2d::allVerticesDiscrete() const
{
    PRE( pPoints_->size() > 2 );

// cout << "MexConvexPolygon2d::allVerticesDiscrete for " << pPoints_->size() << " vertices\n";
// cout << " Address of this " << (void*)this << endl;
// for(int i = 0; i < pPoints_->size(); ++i)
// {
// 	cout << " " << vertex( i ) << endl;
// }

	for(int i = 0; i < pPoints_->size(); ++i)
    {
		for(int j = 0; j < pPoints_->size(); ++j)
        {
            ASSERT_INFO( i );
            ASSERT_INFO( j );
            ASSERT_INFO( vertex( i ) );
            ASSERT_INFO( vertex( j ) );

			if( i != j and
				vertex( i ) == vertex( j ) )
				return false;
        }
    }
	return true;
}
//////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>

MexConvexPolygon2d::MexConvexPolygon2d( const MexAlignedBox2d& boundary )
:   MexPolygon2d( 4 )
{
    //Construct a vector of size expansionDistance in the direction p1 to p2
    MATHEX_SCALAR xMin = boundary.minCorner().x();
    MATHEX_SCALAR xMax = boundary.maxCorner().x();
    MATHEX_SCALAR yMin = boundary.minCorner().y();
    MATHEX_SCALAR yMax = boundary.maxCorner().y();


    //Create the points in the right order
    pPoints_ = _NEW( Points );
    pPoints_->reserve( 4 );
    pPoints_->push_back( MexPoint2d( xMin, yMin ) );
    pPoints_->push_back( MexPoint2d( xMax, yMin ) );
    pPoints_->push_back( MexPoint2d( xMax, yMax ) );
    pPoints_->push_back( MexPoint2d( xMin, yMax ) );

	ctl_vector< MexPoint2d >::iterator it = pPoints_->begin();
	while (it != pPoints_->end())
	{
		//cout << (*it).x() << " " << (*it).y() << endl;
		++it;
	}

	POST( allVerticesDiscrete() );
}

//////////////////////////////////////////////////////////////////////////////////////////

void MexConvexPolygon2d::transform( const MexTransform3d& transform )
{
    PRE( pPoints_ != NULL );

    for( Points::iterator i = pPoints_->begin(); i != pPoints_->end(); ++i )
        transform.transform( &(*i) );
}

//////////////////////////////////////////////////////////////////////////////////////////

PER_DEFINE_PERSISTENT( MexConvexPolygon2d );

//////////////////////////////////////////////////////////////////////////////////////////

void perWrite( PerOstream& stream, const MexConvexPolygon2d& t )
{
    const MexPolygon2d& base = t;
    stream << t;

	int size = t.pPoints_->size();
	stream << size;
//	cout<<"preWritting 	a MexConvexPolygon2d: "<< endl;
//	cout<<size<<endl;
	ctl_vector< MexPoint2d >::iterator it = t.pPoints_->begin();
	while (it != t.pPoints_->end())
	{
		std::cout<< *it<<std::endl;
		stream << *it++;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////

void perRead( PerIstream& stream, MexConvexPolygon2d& t )
{
	std::cout<< "preReading 	a MexConvexPolygon2d: "<< std::endl;

	stream >> _STATIC_CAST( MexPolygon2d&, t );

    ctl_vector< MexPoint2d >* pPoints = _NEW(ctl_vector< MexPoint2d >); //The vertices
	int size;
	stream >> size;

	std::cout<<size<< std::endl;

    pPoints->reserve( size );

	MexPoint2d point;
	for(int i=0; i<size; ++i)
	{
		stream >> point;
		std::cout<<point<<std::endl;
    	pPoints->push_back( point);
	}

	t = MexConvexPolygon2d(*pPoints);


	_DELETE(pPoints);
}

//////////////////////////////////////////////////////////////////////////////////////////

MexConvexPolygon2d::MexConvexPolygon2d( PerConstructor )
: MexPolygon2d( 0 )
{
}

//////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const MexConvexPolygon2d& t )
{

//    o << "MexConvexPolygon2d " << (void*)&t << " start" << endl;
    const MexPolygon2d& base = t;
    o << base;
//    o << "MexConvexPolygon2d " << (void*)&t << " end" << endl;

    return o;
}

//////////////////////////////////////////////////////////////////////////////////////////

MexConvexPolygon2d::MexConvexPolygon2d( const MexPoint2d& p, MATHEX_SCALAR expansionDistance )
:   MexPolygon2d( 4 )
{
    PRE( expansionDistance > 0.0 );

    //Create the points in the right order
    pPoints_ = _NEW( Points );
    pPoints_->reserve( 4 );
    pPoints_->push_back( MexPoint2d( p.x() - expansionDistance, p.y() - expansionDistance ) );
    pPoints_->push_back( MexPoint2d( p.x() + expansionDistance, p.y() - expansionDistance ) );
    pPoints_->push_back( MexPoint2d( p.x() + expansionDistance, p.y() + expansionDistance ) );
    pPoints_->push_back( MexPoint2d( p.x() - expansionDistance, p.y() + expansionDistance ) );

    POST( isClockwiseAndConvex() );
}
//////////////////////////////////////////////////////////////////////////////////////////

/* End CVEXPGON.CPP *************************************************/
