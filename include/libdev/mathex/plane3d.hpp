/*
 * P L A N E 3 D . H P P
 * (c) Charybdis Limited, 1995, 1996. All Rights Reserved
 */

#ifndef _MEX_PLANE3D_HPP
#define _MEX_PLANE3D_HPP

/////////////////////////////////////

#include "base/persist.hpp"

#include "mathex/vec3.hpp"
#include "mathex/point3d.hpp"
#include "mathex/transf3d.hpp"

// forward refs

class MexLine3d;

/////////////////////////////////////

class MexPlane3d
{
public:

	MexPlane3d();
	MexPlane3d( const MexPoint3d&,
					const MexPoint3d&,
					const MexPoint3d& );

	MexPlane3d( const MexPlane3d& );
    MexPlane3d( const MexTransform3d& );
	~MexPlane3d();

	bool contains( const MexPoint3d& ) const;
	bool contains( const MexLine3d& ) const;
	bool contains( const MexPlane3d& ) const;

	bool intersects( const MexPoint3d& ) const;
	bool intersects( const MexLine3d& ) const;
	bool intersects( const MexPlane3d& ) const;

	MATHEX_SCALAR distanceFrom( const MexPoint3d& ) const;

	MATHEX_SCALAR a() const;
	MATHEX_SCALAR b() const;
	MATHEX_SCALAR c() const;
	MATHEX_SCALAR d() const;

	void definePlane( const MexPoint3d&,
		 			  const MexPoint3d&,
					  const MexPoint3d& );

    void definePlane( const MexTransform3d& );

    const MexVec3& normal() const;

	const MexPlane3d& operator =( const MexPlane3d& );
	bool operator ==( const MexPlane3d& ) const;

    enum PlaneSide
    { NEGATIVE_PLANE_SIDE = -1,
	   POSITIVE_PLANE_SIDE = 1,
	   ON_PLANE			   = 0 };

	PlaneSide pointRelativeToPlane( const MexPoint3d& ) const;

    const MexTransform3d& planeTransform( void ) const;

    //  Return the X coordinate of a point on the plane given the Y and Z coordinates
    MATHEX_SCALAR   x( MATHEX_SCALAR y, MATHEX_SCALAR z ) const;
    // PRE( a() != 0.0 );

    //  Return the Y coordinate of a point on the plane given the X and Z coordinates
    MATHEX_SCALAR   y( MATHEX_SCALAR x, MATHEX_SCALAR z ) const;
    // PRE( b() != 0.0 );

    //  Return the Z coordinate of a point on the plane given the X and Y coordinates
    MATHEX_SCALAR   z( MATHEX_SCALAR x, MATHEX_SCALAR y ) const;
    // PRE( c() != 0.0 );

	friend ostream& operator<<( ostream&, const MexPlane3d& );

    void CLASS_INVARIANT
    {
        INVARIANT( a_ == normal_.x() );
        INVARIANT( b_ == normal_.y() );
        INVARIANT( c_ == normal_.z() );
        INVARIANT( Mathex::abs( normal_.modulus() - 1.0 ) < MexEpsilon::instance() );

        MexPoint3d   p0;
        planeTransform_.position( &p0 );

        MexPoint3d   p1( p0 );
        MexVec3 xBasis;
        planeTransform_.xBasis( &xBasis );
        p1 += xBasis;

        MexPoint3d   p2( p0 );
        MexVec3 zBasis;
        planeTransform_.zBasis( &zBasis );
        p2 += zBasis;

        INVARIANT( contains( p0 ) );
        INVARIANT( contains( p1 ) );
        INVARIANT( contains( p2 ) );
    }

    //Persistence
    PER_MEMBER_PERSISTENT_DEFAULT( MexPlane3d );
    PER_FRIEND_READ_WRITE( MexPlane3d );

private:

	MATHEX_SCALAR a_;
	MATHEX_SCALAR b_;
	MATHEX_SCALAR c_;
	MATHEX_SCALAR d_;

    MexTransform3d planeTransform_;
	MexVec3 normal_;
};

PER_DECLARE_PERSISTENT( MexPlane3d );

#ifdef _INLINE
    #include "mathex/plane3d.ipp"
#endif

//////////////////////////////////////////////////////////////////////

#endif      /*   #ifndef _PLANE3D_HPP     */

/* End PLANE3D.HPP *****************************************************/
