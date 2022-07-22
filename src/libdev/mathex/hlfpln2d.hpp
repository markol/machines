/*
 * H L F P L N 2 D . H P P
 * (c) Charybdis Limited, 1995, 1996. All Rights Reserved
 */

#ifndef _MEX_HLFPLN2D_HPP
#define _MEX_HLFPLN2D_HPP

#include "base/persist.hpp"

#include "mathex/mathex.hpp"

//////////////////////////////////
//class ostream;
class MexVec2;

// Is "half-plane" a misnomer?  This class really defines lines.
// Dividing the plane into two is just one of their properties.
class MexHalfPlane2d
{
public:
	MexHalfPlane2d(): vx(0), vy(0), c(0)	{}

	// Construct a half plane passing through these two points.
	MexHalfPlane2d(const MexVec2& p0, const MexVec2& p1);
	MexHalfPlane2d(const MexHalfPlane2d&);

	// This method will classify the given point into exactly two cases,
	// where points on the edge are assigned to the VANILLA side.
	Mathex::Side bisect( const MexVec2& ) const;
	// POST(result != ONEDGE && result != STRADDLE);

	// Like bisect but can also classify points as being "on the edge".
	Mathex::Side sideOfLine(const MexVec2&) const;
	// POST(result != STRADDLE);

	const MexHalfPlane2d& operator =( const MexHalfPlane2d& );
	bool operator ==( const MexHalfPlane2d& ) const;

    //Persistence
    PER_MEMBER_PERSISTENT_DEFAULT( MexHalfPlane2d );
    PER_FRIEND_READ_WRITE( MexHalfPlane2d );

private:
	// Line eqn constants, same names as Graphics Gems IV, page 35.
	// Initialisation order is important: c is calculated from vx & vy.
	// Note: these are *not* MATHEX_SCALARs.  Will this work if
	// MATHEX_SCALAR is a non-floating-point type???
	double vx, vy, c;

	friend ostream& operator <<( ostream&, const MexHalfPlane2d& );
	friend istream& operator >>( istream&,       MexHalfPlane2d& );
};

PER_DECLARE_PERSISTENT( MexHalfPlane2d );

//////////////////////////////////////////////////////////////////////

#endif    /* _MEX_HLFPLN2D_HPP */
