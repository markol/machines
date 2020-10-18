/*
 * V E C 3 . H P P
 * (c) Charybdis Limited, 1995, 1996. All Rights Reserved


	If you change the data members in this class it is very important to remember to change
	the compressed version as well.

 */

#ifndef _MEX_VEC3_HPP
#define _MEX_VEC3_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "mathex/mathex.hpp"
#include "mathex/radians.hpp"

// forward refs
class MexPoint3d;
class MexVec2;
class MexCompressedVec3;

//////////////////////////////////////////////////////////////////////

class MexVec3
{
public:

    MexVec3( MATHEX_SCALAR x=0, MATHEX_SCALAR y=0, MATHEX_SCALAR z=0 );
    MexVec3( const MexVec3& );
 	MexVec3( const MexPoint3d& );

    //Vector from p to q
 	MexVec3( const MexPoint3d& p, const MexPoint3d& q );

    //Vector from p to q, scaled by 1/s. Hence if s is the distance from
    //p to q, result is unit direction vector
 	MexVec3( const MexPoint3d& p, const MexPoint3d& q, MATHEX_SCALAR s );
    //PRE( s != 0.0 )

 	MexVec3( const MexVec2& v2 );
	// POST( *this == MexVec3( v2.x(), v2.y(), 0 ) );

 	MexVec3( const MexCompressedVec3& );

    ~MexVec3();

    const MexVec3& operator =( const MexVec3& );
	const MexVec3& operator =( const MexPoint3d& );

    bool operator ==( const MexVec3& ) const;
    bool operator !=( const MexVec3& ) const;

    void operator +=( const MexVec3& );
    void operator -=( const MexVec3& );
    void operator *=( MATHEX_SCALAR );
    void operator /=( MATHEX_SCALAR );
    // PRE( t != 0 );

    static MexVec3 crossProduct( const MexVec3&, const MexVec3& );
    void crossProduct( const MexVec3& );
    MATHEX_SCALAR dotProduct( const MexVec3& ) const;

  //  void operator *=( const Transform3& );

    MATHEX_SCALAR operator []( size_t i ) const;
    MATHEX_SCALAR& operator []( size_t i );

    MATHEX_SCALAR x( void ) const;
    MATHEX_SCALAR y( void ) const;
    MATHEX_SCALAR z( void ) const;

	void x( MATHEX_SCALAR );
	void y( MATHEX_SCALAR );
	void z( MATHEX_SCALAR );

    bool isPerpendicularTo( const MexVec3& ) const;

    MATHEX_SCALAR modulus() const;
    MexRadians angleBetween( const MexVec3& v ) const;
    //  POST( 0.0 <= result and result <= Mathex::PI );

    MexRadians signedAngleBetween( const MexVec3& v ) const;
    //  POST( -Mathex::PI <= result and result <= Mathex::PI );

    void makeUnitVector();
    // POST( modulus() == 1.0 );

    MATHEX_SCALAR euclidianDistance( const MexVec3& ) const;
    MATHEX_SCALAR sqrEuclidianDistance( const MexVec3& ) const;

    void interpolate( const MexVec3&, MATHEX_SCALAR );

    static const MexVec3& zeroVector();

    static  double  test( void );

    void setVector( MATHEX_SCALAR x, MATHEX_SCALAR y, MATHEX_SCALAR z );

	bool isZeroVector() const;

    //  Return true if the two vectors are parallel to
    //  within asin( MexEpsilon::instance() )
    bool parallel( const MexVec3& ) const;

    friend ostream& operator <<( ostream&, const MexVec3& );

    //Persistence
    PER_MEMBER_PERSISTENT_DEFAULT( MexVec3 );
    PER_FRIEND_READ_WRITE( MexVec3 );

private:

    enum { X = 0, Y = 1, Z = 2 };

    MATHEX_SCALAR	vec_[ 3 ];
};

bool operator <( const MexVec3&, const MexVec3& );
bool operator >( const MexVec3&, const MexVec3& );
bool operator <=( const MexVec3&, const MexVec3& );
bool operator >=( const MexVec3&, const MexVec3& );

PER_DECLARE_PERSISTENT( MexVec3 );

//////////////////////////////////////////////////////////////////////

#ifdef _INLINE
    #include "mathex/vec3.ipp"
#endif

//////////////////////////////////////////////////////////////////////

#endif  /* #ifndef _MEX_VEC3_HPP */

/* End VEC3.HPP *****************************************************/
