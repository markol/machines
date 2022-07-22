/*
 * A B O X 3 D . H P P
 * (c) Charybdis Limited, 1995, 1996. All Rights Reserved
 */

#ifndef _MEX_ABOX3D_HPP
#define _MEX_ABOX3D_HPP

//////////////////////////////////////////

#include "base/persist.hpp"

#include "mathex/transf3d.hpp"
#include "mathex/point3d.hpp"

//////////////////////////////////////////

typedef size_t Index;

class MexAlignedBox3d
{
public:

	MexAlignedBox3d();
	MexAlignedBox3d( const MexPoint3d& minCorner, const MexPoint3d& maxCorner );

	MexAlignedBox3d( const MexPoint3d& centre,
	  MATHEX_SCALAR xDimension, MATHEX_SCALAR yDimension, MATHEX_SCALAR zDimension );
    // PRE( xDimension >= 0 );
    // PRE( yDimension >= 0 );
    // PRE( zDimension >= 0 );

	MexAlignedBox3d( const MATHEX_SCALAR x1, const MATHEX_SCALAR y1, const MATHEX_SCALAR z1,
	          		 const MATHEX_SCALAR x2, const MATHEX_SCALAR y2, const MATHEX_SCALAR z2 );
	// PRE( x1 <= x2 and y1 <= y2 and z1 <= z2 );

	MexAlignedBox3d( const MexAlignedBox3d& );
	// POST( *this == rhs );

    //  Construct a box from another box modifed by a transform
	MexAlignedBox3d( const MexAlignedBox3d&, const MexTransform3d& t );

	~MexAlignedBox3d();

    //Adjusts the dimensions to form a modified aligned box which will
    //be guaranteed to include the original in under any rotation about
    //its centroid. This means the new box will bound the original under
    //any global transform.
    void makeSpinSafe();

    //The square of the shortest distance from p to this
    MATHEX_SCALAR sqrEuclidianDistance( const MexPoint3d& p ) const;

	const MexAlignedBox3d& operator =( const MexAlignedBox3d& );
	// POST( vmin_ == result.vmin_ and vmax_ == result.vmax_ );

	bool operator ==( const MexAlignedBox3d& ) const;
	bool operator !=( const MexAlignedBox3d& ) const;
	// POST( iff( result, rhs.contains( *this ) and contains( rhs ) ) )


    static const MexAlignedBox3d& zeroBox();

    //the corners
    const MexPoint3d& minCorner() const;
    const MexPoint3d& maxCorner() const;

    //change the corners
    void minCorner( const MexPoint3d& p );
    void maxCorner( const MexPoint3d& q );
    void corners( const MexPoint3d& p, const MexPoint3d& q );
    //POST( minCorner() <= maxCorner() in all dimensions )

    //Modifies this boundary to include rhs transformed by t
    void uniteWith( const MexAlignedBox3d& rhs, const MexTransform3d& t );

    //True if line intersects this box anywhere along its length.
    //If so returns the distance along line (relative to its first point) at which
    //the infinite line through it enters and exits the box, in pEntryDistance
    //and pExitDistance respectively.
    //rule determines whether simple touching is treated as an intersection.
    bool intersects( const MexLine3d& line,
                     MATHEX_SCALAR* pEntryDistance, MATHEX_SCALAR* pExitDistance,
                     Mathex::TouchingRule rule = Mathex::TOUCH_IS_INTERSECT ) const;

    //The product of the side lengths
	MATHEX_SCALAR volume() const;
	MATHEX_SCALAR xLength() const;
	MATHEX_SCALAR yLength() const;
	MATHEX_SCALAR zLength() const;

    //the centre point
    MexPoint3d centroid() const;

	friend ostream& operator <<( ostream&, const MexAlignedBox3d& );

    //Persistence
    PER_MEMBER_PERSISTENT_DEFAULT( MexAlignedBox3d );
    PER_FRIEND_READ_WRITE( MexAlignedBox3d );

    void CLASS_INVARIANT;

private:

    //Compute the min/max of x, y, z, x+y, x+z, y+z, x+y+z
    static MATHEX_SCALAR boxMin( MATHEX_SCALAR x,  MATHEX_SCALAR y,  MATHEX_SCALAR z );
    static MATHEX_SCALAR boxMax( MATHEX_SCALAR x,  MATHEX_SCALAR y,  MATHEX_SCALAR z );

    //Data members
    MexPoint3d minCorner_; //Boundary limits
    MexPoint3d maxCorner_;

	bool  isEmpty_ :1;
    bool  spinSafe_ :1; // True iff the box has been made spin-safe
};

PER_DECLARE_PERSISTENT( MexAlignedBox3d );

//////////////////////////////////////////////////////////////////////

#ifdef _INLINE
    #include "mathex/abox3d.ipp"
#endif

//////////////////////////////////////////////////////////////////////

#endif		/*  	_MEX_ABOX3D_HPP   	*/

/* End ABOX3D.HPP *****************************************************/
