/*
 * T R I 2 D . H P P
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */

#ifndef _MATHEX_TRI2D_HPP
#define _MATHEX_TRI2D_HPP

/////////////////////////////////////
#include "base/persist.hpp"

#include "mathex/point2d.hpp"
#include "mathex/poly2d.hpp"
#include "mathex/hlfpln2d.hpp"

//////////////////////////////////////////////////////////////////////
// This represents a basic dumb polygon.  It is dumb in the sense that
// it contains its vertices and does nothing clever to recognise
// that polygons share vertices in most situations.
class MexTriangle2d : public MexPolygon2d
{
public:
	// Create triangle with these corners.
	MexTriangle2d(const MexPoint2d& v1, const MexPoint2d& v2, const MexPoint2d& v3);
	MexTriangle2d( const MexTriangle2d& );
	virtual ~MexTriangle2d();

	const MexTriangle2d& operator=( const MexTriangle2d& );

	//bool contains(const MexPoint2d&) const;

	virtual const MexPoint2d& vertex( size_t i ) const;
	// PRE( i < nVertices() );

	bool operator ==( const MexTriangle2d& ) const;
	bool operator !=( const MexTriangle2d& ) const;

    friend ostream& operator<<( ostream& ostr, const MexTriangle2d& t );

    //Persistence
    PER_MEMBER_PERSISTENT_VIRTUAL( MexTriangle2d );
    PER_FRIEND_READ_WRITE( MexTriangle2d );

	void CLASS_INVARIANT
	{
		INVARIANT(nVertices() == NUMBER_OF_VERTICES_);
		geometric_invariant();
	}

private:
    // TODO: check this
    //MexTriangle2d( PerConstructor );

    double cross_product(const MexPoint2d& v1, const MexPoint2d& v2, const MexPoint2d& v3) const;
	void geometric_invariant() const;

	enum { NUMBER_OF_VERTICES_=3 };
	MexPoint2d vertices_[NUMBER_OF_VERTICES_];
	MexHalfPlane2d edge1_, edge2_, edge3_;
};

PER_DECLARE_PERSISTENT( MexTriangle2d );

//////////////////////////////////////////////////////////////////////

#endif      /*   #ifndef _MATHEX_TRI2D_HPP     */
