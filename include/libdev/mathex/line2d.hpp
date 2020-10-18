/*
 * L I N E 2 D . H P P
 * (c) Charybdis Limited, 1995, 1996. All Rights Reserved
 */

#ifndef _MEX_LINE2D_HPP
#define _MEX_LINE2D_HPP

#include "base/persist.hpp"

#include "mathex/mathex.hpp"
#include "mathex/point2d.hpp"

//Forward declarations
class MexAlignedBox2d;

//////////////////////////////////////////////////////////////////////
class MexLine2d
{
public:

    MexLine2d( void );
	MexLine2d( const MexPoint2d& v1, const MexPoint2d& v2 );
	MexLine2d( const MexLine2d& );
	~MexLine2d();
	const MexLine2d& operator =( const MexLine2d& );

    void setLine( const MexPoint2d&, const MexPoint2d& );

    const MexPoint2d& end1() const;
    const MexPoint2d& end2() const;

    MATHEX_SCALAR length() const;

    //returns a boundary for the line in pBoundary
    void boundary( MexAlignedBox2d* pBoundary ) const;

    //True if the line defined by p, unit direction vector d and length l
    //intersects the line joining q1 to q2, within clearance MexEpsilon.
    //pSide indicates whether q1q2 crosses the infinite line defined by p and d,
    //or both points lie on a single side.
    //rule determines whether touching at a vertex is considered to be intersection.
    static bool intersects( const MexPoint2d& p, const MexVec2& d, MATHEX_SCALAR length,
                            const MexPoint2d& q1, const MexPoint2d& q2, Mathex::Side* pSide,
                            Mathex::TouchingRule rule = Mathex::TOUCH_IS_INTERSECT );


	// True if 'this' line intersects the line passed in. If true is returned then
	// intersectPoint will have been updated with the point at which the two lines
	// crossed. Note that this intersects the infinite lines.
	bool intersects( const MexLine2d& line, MexPoint2d* pIntersectPoint ) const;
	// PRE( intersectPoint );

	// True if 'this' line segment intersects the line segment passed in. If true
    // is returned then intersectPoint will have been updated with the point at
    //  which the two lines crossed.
    bool segmentIntersects( const MexLine2d& line, MexPoint2d* pIntersectPoint ) const;
	// PRE( pIntersectPoint );


    //The square of the shortest distance between 2 2d lines.
    //Line 1 has endpoints p1, q1, and length l1.
    //Line 2 has endpoints p2, q2, and length l2.
    static MATHEX_SCALAR sqrEuclidianDistance
    (
        const MexPoint2d& p1, const MexPoint2d& q1, MATHEX_SCALAR l1,
        const MexPoint2d& p2, const MexPoint2d& q2, MATHEX_SCALAR l2
    );
    //PRE( l1 > epsilon );
    //PRE( l2 > epsilon );
    //PRE( fabs( p1.euclidianDistance( q1 ) - l1 ) < epsilon );
    //PRE( fabs( p2.euclidianDistance( q2 ) - l2 ) < epsilon );

    //The square of the shortest distance between a 2d line and a 2d point.
    //Line 1 has endpoints p1, p2, and length length.
    //Point is
    static MATHEX_SCALAR sqrEuclidianDistance
    (
        const MexPoint2d& p1, const MexPoint2d& p2, MATHEX_SCALAR length,
        const MexPoint2d& q
    );
    //PRE( length > epsilon );
    //PRE( fabs( p1.euclidianDistance( p2 ) - length ) < epsilon );

    //Returns value indicating the side of the infinite line through q1 and q2 on which
    //r lies. Result undefined if q1 to q2 is too short.
    static Mathex::Side side( const MexPoint2d& q1, const MexPoint2d& q2,
                              const MexPoint2d& r );

	bool isPointOnLine( const MexPoint2d& v ) const;

    //A unit direction vector from end1() to end2()
    MexVec2 unitDirectionVector() const;

    //The point distance along line from end1()
    MexPoint2d pointAtDistance( MATHEX_SCALAR distance ) const;

	// Distance to a point on this line that is nearest to point.
	// The distance is from end1 along to end2 on an infinite line.
	MATHEX_SCALAR distanceAlongLine(MexPoint2d point);

    //Persistence
    PER_MEMBER_PERSISTENT_DEFAULT( MexLine2d );
    PER_FRIEND_READ_WRITE( MexLine2d );

private:

    //  Operation intentionally revoked
	bool operator ==( const MexLine2d& ) const;
	bool operator !=( const MexLine2d& ) const;

    MexPoint2d end1_;
    MexPoint2d end2_;

};

ostream& operator <<( ostream& o, const MexLine2d& t );

PER_DECLARE_PERSISTENT( MexLine2d );

#endif // #ifndef _MEX_LINE2D_HPP //


/* End LINE2D.HPP *****************************************************/
