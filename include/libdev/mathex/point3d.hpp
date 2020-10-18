/*
 * P O I N T 3 D . H P P
 * (c) Charybdis Limited, 1995, 1996. All Rights Reserved


	If you change the data members in this class it is very important to remember to change
	the compressed version as well.
 */

#ifndef _MEX_POINT3D_HPP
#define _MEX_POINT3D_HPP

//////////////////////////////////////////////////////////////////////
#include "base/persist.hpp"

#include "mathex/mathex.hpp"

// forward refs
class MexVec3;
class MexLine3d;
class MexPlane3d;
class MexRadians;
class MexPoint2d;
class MexCompressedPoint3d;
//class ostream;

//////////////////////////////////////////////////////////////////////

class MexPoint3d
{
public:

    MexPoint3d( MATHEX_SCALAR x=0, MATHEX_SCALAR y=0, MATHEX_SCALAR z=0 );
    MexPoint3d( const MexPoint3d& );
	MexPoint3d( const MexVec3& );

	MexPoint3d( const MexPoint2d& p2 );
	// POST( *this == MexPoint3d( p2.x(), p2.y(), 0 ) );

	MexPoint3d( const MexCompressedPoint3d& );

    ~MexPoint3d();

    const MexPoint3d& operator =( const MexPoint3d& );
	const MexPoint3d& operator =( const MexVec3& );

    bool operator ==( const MexPoint3d& ) const;
    bool operator !=( const MexPoint3d& ) const;

    void operator +=( const MexVec3& );
    void operator -=( const MexVec3& );

    MATHEX_SCALAR operator []( size_t i ) const;
    MATHEX_SCALAR& operator []( size_t i );

    MATHEX_SCALAR x( void ) const;
    MATHEX_SCALAR y( void ) const;
    MATHEX_SCALAR z( void ) const;

    void x( MATHEX_SCALAR );
    void y( MATHEX_SCALAR );
    void z( MATHEX_SCALAR );

    MATHEX_SCALAR euclidianDistance( const MexPoint3d& ) const;
    MATHEX_SCALAR sqrEuclidianDistance( const MexPoint3d& ) const;

    void interpolate( const MexPoint3d&, MATHEX_SCALAR );
    static const MexPoint3d& zeroPoint();

    void setPoint( MATHEX_SCALAR x, MATHEX_SCALAR y, MATHEX_SCALAR z );

    void rotateAboutAxis(
        const MexVec3& axis,
        const MexPoint3d& pointOnAxis,
        MexRadians angle );
    // PRE( isValid( axis ) );
    // PRE( isValid( pointOnAxis ) );
    // PRE( !axis.isZeroVector() );

	bool contains( const MexPoint3d& ) const;
    bool contains( const MexLine3d& ) const;
    bool contains( const MexPlane3d& ) const;

    bool intersects( const MexPoint3d& ) const;
    bool intersects( const MexLine3d& ) const;
    bool intersects( const MexPlane3d& ) const;

	bool isZeroPoint() const;

    friend ostream& operator <<( ostream&, const MexPoint3d& );

    //Persistence
    PER_MEMBER_PERSISTENT_DEFAULT( MexPoint3d );
    PER_FRIEND_READ_WRITE( MexPoint3d );

private:

    enum { X = 0, Y = 1, Z = 2 };

    MATHEX_SCALAR	point_[ 3 ];
};

bool operator <( const MexPoint3d&, const MexPoint3d& );
bool operator >( const MexPoint3d&, const MexPoint3d& );
bool operator <=( const MexPoint3d&, const MexPoint3d& );
bool operator >=( const MexPoint3d&, const MexPoint3d& );

PER_DECLARE_PERSISTENT( MexPoint3d );
//////////////////////////////////////////////////////////////////////

#ifdef _INLINE
    #include "mathex/point3d.ipp"
#endif

//////////////////////////////////////////////////////////////////////

#endif  /* #ifndef _MEX_POINT3D_HPP */

/* End POINT3D.HPP *****************************************************/
