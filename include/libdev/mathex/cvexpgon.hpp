/*
 * C V E X P G O N . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MexConvexPolygon2d

    This class represents a convex polygon in 2d space.
*/

#ifndef _MATHEX_CVEXPGON_HPP
#define _MATHEX_CVEXPGON_HPP

#include "base/persist.hpp"
#include "base/base.hpp"
//#include "stdlib/memory.hpp"
#include <memory>
#include "ctl/vector.hpp"

#include "mathex/mathex.hpp"
#include "mathex/poly2d.hpp"
#include "mathex/point2d.hpp"

//Forward declarations
//class ostream;
class MexAlignedBox2d;
class MexTransform3d;

//Orthodox canonical (revoked)
class MexConvexPolygon2d : public MexPolygon2d
{
public:
    //Useful types
    typedef ctl_vector< MexPoint2d > Points;

    //Construct a polygon from vector of points.
    MexConvexPolygon2d( const Points& points );

    //Construct a polygon from vector of pointers to points.
    MexConvexPolygon2d( const ctl_vector< MexPoint2d* >& points );

    //Construct the polygon using the vector of vertices provided.
    //The polygon becomes the owner of *pointsAPtr.
    MexConvexPolygon2d(  std::auto_ptr< Points >& pointsAPtr );

    //Construct a new polygon which bounds rhs expanded by expansionDistance
    MexConvexPolygon2d( const MexPolygon2d& rhs, MATHEX_SCALAR expansionDistance );

    //Construct a polygon by expanding expansionDistance around the line joining
    //p1 to p2.
    MexConvexPolygon2d( const MexPoint2d& p1, const MexPoint2d& p2,
                        MATHEX_SCALAR expansionDistance );

    //Construct a polygon using the 4 corners of boundary
    MexConvexPolygon2d( const MexAlignedBox2d& boundary );

    //Construct a polygon by expanding expansionDistance around p
    MexConvexPolygon2d( const MexPoint2d& p, MATHEX_SCALAR expansionDistance );
    //PRE( expansionDistance > 0.0 )

    MexConvexPolygon2d( const MexConvexPolygon2d& );

    //dtor.
    virtual ~MexConvexPolygon2d( void );

    //The ith vertex
	virtual const MexPoint2d& vertex( size_t i ) const;

    //True iff whole polygon is convex
    bool isClockwiseAndConvex() const;

	//True iff all the points are discrete (unique)
	bool allVerticesDiscrete() const;

    //  Transform the polygon by the given transform
    void transform( const MexTransform3d& transform );

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MexConvexPolygon2d& t );

    bool operator ==( const MexConvexPolygon2d& );

   //Persistence
    PER_MEMBER_PERSISTENT_VIRTUAL( MexConvexPolygon2d );
    PER_FRIEND_READ_WRITE( MexConvexPolygon2d );


private:
    // Operation deliberately revoked
    MexConvexPolygon2d& operator =( const MexConvexPolygon2d& );

    //MexConvexPolygon2d( PerConstructor );

    //Data members
    Points* pPoints_; //The vertices
};

PER_DECLARE_PERSISTENT( MexConvexPolygon2d );

#ifdef _INLINE
    #include "mathex/cvexpgon.ipp"
#endif


#endif

/* End CVEXPGON.HPP *************************************************/
