/*
 * P O L Y 2 D . H P P
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */

/*
    Note: This class appears to represent both concave and convex
    polygons. In practice, it assumes that the polygons will be
    convex.

    TBD: Change the class so that it does not make this assumption -
    there is already a derived MexConvexPolygon2d class to handle
    this case.
*/

#ifndef _MATHEX_POLY2D_HPP
#define _MATHEX_POLY2D_HPP

/////////////////////////////////////

#include "base/persist.hpp"

#include "ctl/vector.hpp"

#include "mathex/vec2.hpp"
#include "mathex/mathex.hpp"

//Forward declarations
class MexPoint2d;
class MexAlignedBox2d;

//////////////////////////////////////////////////////////////////////
// An abstract base class representing polygons in the x/y plane.
class MexPolygon2d
{
public:
    //Useful typedefs
    typedef ctl_vector< MATHEX_SCALAR > Lengths;

    //dtor.
    virtual ~MexPolygon2d();

    //The vertex count
	size_t nVertices() const;

    //The ith vertex
	virtual const MexPoint2d& vertex( size_t i ) const = 0;
	// PRE( i < nVertices() );

    //Get/set option to store cached data to increase the efficiency of
    //methods like testing for intersection with a line, expansion ctor.
    //Default is false.
    void isCachingData( bool doCache ) const;
    bool isCachingData() const;

    //Returns lengths of the edges. NB must be caching data.
    const Lengths& edgeLengths() const;
    //PRE( isCachingData() )

    //Fills directions with unit direction vectors for each edge
    void edgeDirections( ctl_vector< MexVec2 >* directions ) const;

    //True iff line joining p1 and p2 intersects this polygon.
    //rule indicates whether penetration is required.
    bool intersects( const MexPoint2d& p1, const MexPoint2d& p2,
                     Mathex::TouchingRule rule = Mathex::TOUCH_IS_INTERSECT ) const;

    //True iff line joining the point at vertexIndex and p2 intersects this polygon.
    //Since we know that one of the vertices is on the polygon it is assumed that
    //the rule is that touch is not an intersect.
    bool intersects( size_t vertexIndex, const MexPoint2d& p2 ) const;
    // PRE( vertexIndex < nVertices() );
    // PRE( polygon is convex );

    //True iff the line from q1 to q2 penetrates this polygon by at least MexEpsilon::instance().
    //The distance from q1 to q2 is lineLength.
    bool isPenetratedBy( const MexPoint2d& q1, const MexPoint2d& q2,
                         MATHEX_SCALAR lineLength ) const;
    //PRE( fabs( q1.euclidianDistance( q2 ) - lineLength ) < MexEpsilon::instance() );

    //True if this polygon intersects rhs.
    //rule indicates whether penetration is required.
    bool intersects( const MexPolygon2d& rhs,
                     Mathex::TouchingRule rule = Mathex::TOUCH_IS_INTERSECT ) const;

    //True iff this contains q
    bool contains( const MexPoint2d& q ) const;

    //Returns aligned bounding box in pBox.
    void boundary( MexAlignedBox2d* pBox ) const;

    //  Return true iff the given point is a vertex of this polygon. If it is, also
    //  return the vertex index
    bool pointIsVertex( const MexPoint2d&, size_t* pVertexIndex ) const;
    //  POST( implies( result, *pVertexIndex < nVertices() ) );

    //Persistence
    PER_MEMBER_PERSISTENT_DEFAULT_ABSTRACT( MexPolygon2d );
    PER_FRIEND_READ_WRITE( MexPolygon2d );

protected:
    //ctor. Will have nVertices vertices.
	MexPolygon2d(size_t nVertices);
	MexPolygon2d( const MexPolygon2d& ); //Copy ctor.

    //Reset vertex count
    void nVertices( size_t n );

private:
	// Revoked
	const MexPolygon2d& operator =( const MexPolygon2d& );
	bool operator==( const MexPolygon2d& );
	bool operator!=( const MexPolygon2d& );

    //Compute and cache the polygon edge lengths
    void computeEdgeLengths() const;

    //Delet any data cached in extra RAM
    void clearCachedData() const;

	size_t nVertices_; //Vertex count
    Lengths* pEdgeLengths_; //Optional vector caching the polygon side lengths
    bool isCachingData_ :1; //True if caching side lengths
};

PER_DECLARE_PERSISTENT( MexPolygon2d );

//////////////////////////////////////////////////////////////////////

//class ostream;
ostream& operator<<(ostream&, const MexPolygon2d&);

//////////////////////////////////////////////////////////////////////

#endif      /*   #ifndef _MATHEX_POLY2D_HPP     */
