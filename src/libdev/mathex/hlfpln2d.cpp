/*
 * H L F P L A N E 2 D . C P P
 * (c) Charybdis Limited, 1995, 1996. All Rights Reserved
 */

#include <iostream>
#include "mathex/vec2.hpp"
#include "mathex/hlfpln2d.hpp"

MexHalfPlane2d::MexHalfPlane2d(const MexHalfPlane2d& plane):
	vx(plane.vx),
	vy(plane.vy),
	c (plane.c )
{
}

// Create a half plane which intersects the two given co-ordinates.
// Note that initialisation order is important.
MexHalfPlane2d::MexHalfPlane2d(const MexVec2& p0, const MexVec2& p1):
	vx(p0.y() - p1.y()),
	vy(p1.x() - p0.x()),
	c (vx * p0.x() + vy * p0.y())
{
}

// This was timed against a simpler version which returned a
// bool istead of a Mathex::Side.  There was no significant
// difference in performance.
Mathex::Side MexHalfPlane2d::bisect( const MexVec2& pt) const
{
	if (vx * pt.x() + vy * pt.y() <= c)
		return Mathex::VANILLA;
	else
		return Mathex::CHOCOLATE;
}

Mathex::Side MexHalfPlane2d::sideOfLine( const MexVec2& pt) const
{
	const double sum = vx * pt.x() + vy * pt.y();

	// Given precision considerations, it is debatable whether or not
	// the on-edge classification is worthwhile or even valid.
	if (sum < c)
		return Mathex::VANILLA;
	else if (sum > c)
		return Mathex::CHOCOLATE;
	else
		return Mathex::ONEDGE;
}

const MexHalfPlane2d& MexHalfPlane2d::operator =( const MexHalfPlane2d& rhs)
{
	vx = rhs.vx;
	vy = rhs.vy;
	c  = rhs.c;
	return *this;
}

bool MexHalfPlane2d::operator ==( const MexHalfPlane2d& rhs ) const
{
	return vx == rhs.vx && vy == rhs.vy && c == rhs.c;
}

ostream& operator <<( ostream& os, const MexHalfPlane2d& plane)
{
	os << plane.vx << " " << plane.vy << " " << plane.c;
	return os;
}

istream& operator >>( istream& is, MexHalfPlane2d& plane)
{
	is >> plane.vx >> plane.vy >> plane.c;
	return is;
}


PER_DEFINE_PERSISTENT( MexHalfPlane2d );

void perWrite( PerOstream& stream, const MexHalfPlane2d& t )
{
	stream << t.vx;
	stream << t.vy;
	stream << t.c;
}

void perRead( PerIstream& stream, MexHalfPlane2d& t )
{
	stream >> t.vx;
	stream >> t.vy;
	stream >> t.c;
}

