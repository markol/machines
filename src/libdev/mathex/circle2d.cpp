/*
 * C I R C L E 2 D . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/circle2d.hpp"
#include "mathex/epsilon.hpp"
#include "mathex/abox2d.hpp"
#include "mathex/poly2d.hpp"
#include "mathex/line2d.hpp"

#ifndef _INLINE
    #include "mathex/circle2d.ipp"
#endif

MexCircle2d::MexCircle2d( const MexPoint2d& centre, MATHEX_SCALAR radius )
:   centre_( centre ),
    radius_( radius )
{
    PRE( radius >= 0.0 );

    TEST_INVARIANT;
}

MexCircle2d::~MexCircle2d()
{
    TEST_INVARIANT;

}

void MexCircle2d::CLASS_INVARIANT
{
	INVARIANT(this != NULL);
}

ostream& operator <<( ostream& o, const MexCircle2d& t )
{

    o << "MexCircle2d " << (void*)&t
      << t.centre_ << " " << t.radius_;

    return o;
}

bool MexCircle2d::intersects( const MexCircle2d& rhs, Mathex::TouchingRule rule ) const
{
    //Compute the limiting squared distance
    MATHEX_SCALAR limit = radius_ + rhs.radius_;
    MATHEX_SCALAR epsilon = MexEpsilon::instance();
    if( rule == Mathex::TOUCH_IS_INTERSECT )
        limit += epsilon;
    else
        limit -= epsilon;

    //Check the square of the centre separation
    return centre_.sqrEuclidianDistance( rhs.centre_ ) < limit * limit;
}

void MexCircle2d::boundary( MexAlignedBox2d* pBox ) const
{
    const MATHEX_SCALAR xMin = centre().x() - radius();
    const MATHEX_SCALAR xMax = centre().x() + radius();
    const MATHEX_SCALAR yMin = centre().y() - radius();
    const MATHEX_SCALAR yMax = centre().y() + radius();

    pBox->corners( MexPoint2d( xMin, yMin ), MexPoint2d( xMax, yMax ) );
}

bool MexCircle2d::intersects( const MexPolygon2d& polygon, Mathex::TouchingRule rule ) const
{
    //Set the clearance from centre
    MATHEX_SCALAR sqrClearance = (rule == Mathex::TOUCH_IS_INTERSECT ?
        radius_ + MexEpsilon::instance() :
        radius_ - MexEpsilon::instance() );

    sqrClearance *= sqrClearance;

    //See if the polygon caches its edge lengths, and if so get the lengths vector
    bool haveLengths = polygon.isCachingData();
    MexPolygon2d::Lengths dummyLengths;
    const MexPolygon2d::Lengths& edgeLengths = ( haveLengths ? polygon.edgeLengths() : dummyLengths );

    //Compute the shortest distance between the centre and
    //each edge of the polygon. If any such distance is less than the
    //radius, we have an intersection.
    bool hit = false;
    size_t nVertices = polygon.nVertices();

    for( size_t i = 0; i != nVertices; ++i )
    {
        //Get index to edge end vertex, and hence the vertices
        size_t j = i + 1;
        if( j == nVertices )
            j = 0;

        const MexPoint2d& p1 = polygon.vertex( i );
        const MexPoint2d& p2 = polygon.vertex( j );

        //Compute the distance of centre from the edge
        MATHEX_SCALAR sqrDistance = MexLine2d::sqrEuclidianDistance(
           p1, p2, haveLengths ? edgeLengths[i] : p1.euclidianDistance( p2 ),
           centre() );

        //Hence decide if we have a hit
        if( sqrDistance <= sqrClearance )
        {
            hit = true;
            break;
        }
    }

    //Finally, if we haven't got a hit, the circle may be completely contained in the
    //polygon. So test for containment of the centre point.
    if( not hit )
    {
        hit = polygon.contains( centre() );
    }

    return hit;
}

PER_DEFINE_PERSISTENT( MexCircle2d );

void perWrite( PerOstream& str, const MexCircle2d& t )
{
    const MATHEX_SCALAR x = t.centre_.x();
    const MATHEX_SCALAR y = t.centre_.y();
    str << x;
    str << y;
    str << t.radius_;
}

void perRead( PerIstream& stream, MexCircle2d& t )
{
    MATHEX_SCALAR x, y;
    stream >> x;
    stream >> y;
    stream >> t.radius_;
    t.centre_.x( x );
    t.centre_.y( y );
}

MexCircle2d::MexCircle2d( PerConstructor )
{
}

/* End CIRCLE2D.CPP *************************************************/
