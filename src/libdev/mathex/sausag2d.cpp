/*
 * S A U S A G 2 D . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include "mathex/sausag2d.hpp"
#include "mathex/abox2d.hpp"
#include "mathex/poly2d.hpp"
#include "mathex/line2d.hpp"
#include "mathex/circle2d.hpp"

#ifndef _INLINE
    #include "mathex/sausag2d.ipp"
#endif

MexSausage2d::MexSausage2d
(
    const MexPoint2d& startPoint, const MexPoint2d& endPoint, MATHEX_SCALAR radius
)
:   startPoint_( startPoint ),
    endPoint_( endPoint ),
    radius_( radius ),
    length_( endPoint.euclidianDistance( startPoint ) )
{

    TEST_INVARIANT;
}

MexSausage2d::~MexSausage2d()
{
    TEST_INVARIANT;

}

ostream& operator <<( ostream& o, const MexSausage2d& t )
{

    o << "MexSausage2d " << (void*)&t
      << t.startPoint_ << " " << t.endPoint_
      << "  radius " << t.radius_
      << "  length " << t.length_
      << std::endl;

    return o;
}

void MexSausage2d::boundary( MexAlignedBox2d* pBox ) const
{
    MATHEX_SCALAR xMin, xMax, yMin, yMax;
    MATHEX_SCALAR xStart = startPoint_.x();
    MATHEX_SCALAR yStart = startPoint_.y();
    MATHEX_SCALAR xEnd = endPoint_.x();
    MATHEX_SCALAR yEnd = endPoint_.y();

    if( xStart < xEnd )
    {
        xMin = xStart - radius_;
        xMax = xEnd + radius_;
    }
    else
    {
        xMin = xEnd - radius_;
        xMax = xStart + radius_;
    }

    if( yStart < yEnd )
    {
        yMin = yStart - radius_;
        yMax = yEnd + radius_;
    }
    else
    {
        yMin = yEnd - radius_;
        yMax = yStart + radius_;
    }

    pBox->corners( MexPoint2d( xMin, yMin ), MexPoint2d( xMax, yMax ) );
}

bool MexSausage2d::intersects( const MexPolygon2d& polygon, Mathex::TouchingRule rule ) const
{
    //Set the square clearance from sausage centre line to test
    MATHEX_SCALAR clearance = (rule == Mathex::TOUCH_IS_INTERSECT ?
                               radius_ + MexEpsilon::instance() :
                               radius_ - MexEpsilon::instance() );

    clearance *= clearance;

    //See if the polygon caches its edge lengths, and if so get the lengths vector
    bool haveLengths = polygon.isCachingData();
    MexPolygon2d::Lengths dummyLengths;
    const MexPolygon2d::Lengths edgeLengths = (haveLengths ? polygon.edgeLengths() : dummyLengths );

    //Set flag if the sausage is effectively a cicrle
    bool sausageIsCircle = length_ <= MexEpsilon::instance();

    //Compute the shortest distance between the sausage centre line and
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

        //Compute the distance of sausage centre line from the edge
        MATHEX_SCALAR sqrDistance;
        if( sausageIsCircle )
            sqrDistance = MexLine2d::sqrEuclidianDistance( p1, p2, haveLengths ? edgeLengths[i] :
                                                                   p1.euclidianDistance( p2 ),
                                                           startPoint_ );
        else
            sqrDistance = MexLine2d::sqrEuclidianDistance( p1, p2, haveLengths ? edgeLengths[i] :
                                                                   p1.euclidianDistance( p2 ),
                                                           startPoint_, endPoint_, length_ );

        //Hence decide if we have a hit
        if( sqrDistance <= clearance )
        {
            hit = true;
            break;
        }
    }

    //Finally, if we haven't got a hit, the sausage may be completely contained in the
    //polygon. So test for containment of any point in the sausage.
    if( not hit )
        hit = polygon.contains( startPoint_ );

    return hit;
}

bool MexSausage2d::intersects( const MexSausage2d& rhs, Mathex::TouchingRule rule ) const
{
    //test clearance depends on touching rule
    MATHEX_SCALAR clearance = radius_ + rhs.radius_;
    MATHEX_SCALAR epsilon = MexEpsilon::instance();
    if( rule == Mathex::TOUCH_IS_INTERSECT )
        clearance += epsilon;
    else
        clearance -= epsilon;

    //Get the shortest square distance between the 2 centre lines.
    //Allow for either or both sausages to effectively be a circle.
    MATHEX_SCALAR sqrDistance;
    if( length_ <= epsilon )
    {
        if( rhs.length_ <= epsilon )
        {
            sqrDistance = rhs.startPoint_.sqrEuclidianDistance( startPoint_ );
        }
        else
        {
            sqrDistance =
                MexLine2d::sqrEuclidianDistance( rhs.startPoint_, rhs.endPoint_, rhs.length_,
                                                 startPoint_ );
        }
    }
    else if( rhs.length_ <= epsilon )
    {
        sqrDistance =
            MexLine2d::sqrEuclidianDistance( startPoint_, endPoint_, length_, rhs.startPoint_ );
    }
    else
    {
        sqrDistance =
            MexLine2d::sqrEuclidianDistance( startPoint_, endPoint_, length_,
                                             rhs.startPoint_, rhs.endPoint_, rhs.length_ );
    }

    return sqrDistance < clearance * clearance;
}

bool MexSausage2d::intersects( const MexCircle2d& circle, Mathex::TouchingRule rule ) const
{
    //test clearance depends on touching rule
    MATHEX_SCALAR clearance = radius_ + circle.radius();
    MATHEX_SCALAR epsilon = MexEpsilon::instance();
    if( rule == Mathex::TOUCH_IS_INTERSECT )
        clearance += epsilon;
    else
        clearance -= epsilon;

    //Get the shortest square distance between the centre line and circle radius
    MATHEX_SCALAR sqrDistance;
    if( length_ <= epsilon )
        sqrDistance = startPoint_.sqrEuclidianDistance( circle.centre() );
    else
        sqrDistance =
            MexLine2d::sqrEuclidianDistance( startPoint_, endPoint_, length_, circle.centre() );

    return sqrDistance < clearance * clearance;
}

PER_DEFINE_PERSISTENT( MexSausage2d );

void perWrite( PerOstream& str, const MexSausage2d& t )
{
    str << t.startPoint_;
    str << t.endPoint_;
    str << t.radius_;
    str << t.length_;
}

void perRead( PerIstream& str, MexSausage2d& t )
{
    str >> t.startPoint_;
    str >> t.endPoint_;
    str >> t.radius_;
    str >> t.length_;
}

MexSausage2d::MexSausage2d( PerConstructor )
{
}

void MexSausage2d::CLASS_INVARIANT
{
	INVARIANT(this != NULL);
}
/* End SAUSAG2D.CPP *************************************************/
