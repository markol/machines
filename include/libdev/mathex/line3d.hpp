/*
 * L I N E 3 D . H P P
 * (c) Charybdis Limited, 1995, 1996. All Rights Reserved
 */

#ifndef _MEX_LINE3D_HPP
#define _MEX_LINE3D_HPP

#include "base/persist.hpp"

#include "mathex/point3d.hpp"

//Forward refs
class MexTransform3d;
class MexVec3;
class MexCompressedLine3d;

//////////////////////////////////////////////////////////////////////

class MexLine3d
{
public:

    MexLine3d( void );
	MexLine3d( const MexPoint3d& v1, const MexPoint3d& v2 );
	MexLine3d( const MexLine3d& );

    //Constructor supplies length to save time
	MexLine3d( const MexPoint3d& v1, const MexPoint3d& v2, MATHEX_SCALAR length );
    //PRE( fabs( v1.euclidianDistance( v2) - length ) < MexEpsilon::instance() )

	MexLine3d( const MexCompressedLine3d& );

	~MexLine3d();

	const MexLine3d& operator =( const MexLine3d& );

    MATHEX_SCALAR  length() const;
//    const MexPoint3d& centre() const;

    //A unit direction vector from end1() to end2()
    MexVec3 unitDirectionVector() const;

    bool isPointOnLine( const MexPoint3d& v ) const;

    bool contains( const MexPoint3d& ) const;
    bool contains( const MexLine3d& ) const;
    bool contains( const MexPlane3d& ) const;

    bool intersects( const MexPoint3d& ) const;
    bool intersects( const MexLine3d& ) const;
    bool intersects( const MexPlane3d& ) const;

    enum IntersectSide { INTERSECT_BOTH_SIDES, INTERSECT_FRONT_SIDE };

    //True if this line intersects the triangle formed by p1, p2 and p3.
    //If so returns the distance along the line at the point of intersection.
    //Returns false if line and triangle coplanar.
    //If side == INTERSECT_BOTH_SIDES this will return true no matter what
    //direction the line goes through the triangle. If side == INTERSECT_FRONT_SIDE
    //this will return true only if the line passes through the triangle from the
    //front side (the side where the vertices are clockwise).
    //Note that this intersects the infinite line.
    bool intersects( const MexPoint3d& p1, const MexPoint3d& p2, const MexPoint3d& p3,
                     MATHEX_SCALAR* pDistance, IntersectSide side = INTERSECT_BOTH_SIDES ) const;

    //As intersects but this only intersects the line segment
    bool segmentIntersects( const MexPoint3d& p1, const MexPoint3d& p2, const MexPoint3d& p3,
                     MATHEX_SCALAR* pDistance, IntersectSide side = INTERSECT_BOTH_SIDES ) const;

    //The square of the shortest distance between 2 3d lines.
    //Line 1 has endpoints a1, b1, and length l1.
    //Line 2 has endpoints a2, b2, and length l2.
    static MATHEX_SCALAR sqrEuclidianDistance
    (
        const MexPoint3d& a1, const MexPoint3d& b1, MATHEX_SCALAR l1,
        const MexPoint3d& a2, const MexPoint3d& b2, MATHEX_SCALAR l2
    );
    //PRE( l1 > epsilon );
    //PRE( l2 > epsilon );
    //PRE( fabs( a1.euclidianDistance( b1 ) - l1 ) < epsilon );
    //PRE( fabs( a2.euclidianDistance( b2 ) - l2 ) < epsilon );

    //The square of the shortest distance between a 3d line and a 3d point.
    //Line 1 has endpoints a b, and length length.
	//point is p
    static MATHEX_SCALAR sqrEuclidianDistance
    (
        const MexPoint3d& a, const MexPoint3d& b, MATHEX_SCALAR l,
        const MexPoint3d& p
    );
    //PRE( length > epsilon );
    //PRE( fabs( a.euclidianDistance( b ) - l ) < epsilon );

    void setLine( const MexPoint3d&, const MexPoint3d& );

    const MexPoint3d& end1() const;
    const MexPoint3d& end2() const;

    //Applies the transform to the line
    void transform( const MexTransform3d& transform );

	//Inv(transform) X this
    void transformInverse( const MexTransform3d& transform );

    //The point distance along line from end1()
    MexPoint3d pointAtDistance( MATHEX_SCALAR distance ) const;

    //Persistence
    PER_MEMBER_PERSISTENT_DEFAULT( MexLine3d );
    PER_FRIEND_READ_WRITE( MexLine3d );

private:

    //  Operation intentionally revoked
	bool operator ==( const MexLine3d& ) const;
	bool operator !=( const MexLine3d& ) const;

    //True if this line intersects the triangle formed by p1, p2 and p3.
    //If so returns the distance along the line at the point of intersection.
    //Returns false if line and triangle coplanar.
    //If side == INTERSECT_BOTH_SIDES this will return true no matter what
    //direction the line goes through the triangle. If side == INTERSECT_FRONT_SIDE
    //this will return true only if the line passes through the triangle from the
    //front side (the side where the vertices are clockwise).
    //Returns the length of the line in *pLength
    //Note that this intersects the infinite line.
    bool intersects(
        const MexPoint3d& p1, const MexPoint3d& p2, const MexPoint3d& p3,
        MATHEX_SCALAR* pDistance, MATHEX_SCALAR* pLength, IntersectSide side ) const;

    MexPoint3d end1_;
    MexPoint3d end2_;

    MATHEX_SCALAR length_; //caches the distance from end1_ to end2_
                           //Initialised to -1, indicating not computed, so lazy evaluation
                           //can be used
};

ostream& operator <<( ostream& o, const MexLine3d& t );

PER_DECLARE_PERSISTENT( MexLine3d );

//////////////////////////////////////////////////////////////////////

#ifdef _INLINE
    #include "mathex/line3d.ipp"
#endif

//////////////////////////////////////////////////////////////////////

#endif // #ifndef _MEX_LINE3D_HPP //


/* End LINE3D.HPP *****************************************************/
