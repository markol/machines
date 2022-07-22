/*
 * V E C 2 . H P P
 * (c) Charybdis Limited, 1995, 1996. All Rights Reserved
 */

#ifndef _MEX_VEC2_HPP
#define _MEX_VEC2_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "mathex/mathex.hpp"

//Forward declarations
class MexPoint2d;
class MexRadians;
class MexVec3;

class MexVec2
{
public:

	MexVec2( MATHEX_SCALAR x = 0, MATHEX_SCALAR y = 0 );
	MexVec2( const MexVec2& );
	MexVec2( const MexPoint2d& );

    //Constructs vector from p1 to p2
    MexVec2( const MexPoint2d& p1, const MexPoint2d& p2 );

    //Constructs vector in direction from p1 to p2, scaling by 1/s.
    //If s is previously computed as the distance between p1 and p2, this
    //results in a unit direction vector.
    MexVec2( const MexPoint2d& p1, const MexPoint2d& p2, MATHEX_SCALAR s );
    //PRE( s != 0)

    MexVec2( const MexVec3& v3 );
	// POST( *this == MexVec2( v3.x(), v3.y() ) );

	~MexVec2();

	const MexVec2& operator =( const MexVec2& );
	bool operator ==( const MexVec2& ) const;
	bool operator !=( const MexVec2& ) const;

	bool isZeroVector() const;
	// POST( iff( result, x_ == 0 and y_ == 0 ) );

    void operator +=( const MexVec2& );
    void operator -=( const MexVec2& );
    void operator *=( MATHEX_SCALAR );
    void operator /=( MATHEX_SCALAR );
    // PRE( t != 0 );

    static const MexVec2& zeroVector();

    MATHEX_SCALAR  operator []( size_t i ) const;
    MATHEX_SCALAR& operator []( size_t i );

    void setVector( MATHEX_SCALAR x, MATHEX_SCALAR y );

    MATHEX_SCALAR x() const;
    MATHEX_SCALAR y() const;

    //Modify the vector so that it becomes a unit vector
    void makeUnitVector();

    //Returns scalar dot product this.rhs
    MATHEX_SCALAR dotProduct( const MexVec2& rhs ) const;

    //Returns vector normal to this.
    MexVec2 normal() const;

    //return angle in rotating from this to other
    MexRadians angleBetween( const MexVec2& other ) const;

    //Size of the vector
    MATHEX_SCALAR modulus() const;

    //Squared size of the vector
    MATHEX_SCALAR squareModulus() const;

    //  Rotate the vector by the given angle
    void rotate( const MexRadians& angle );

    //Persistence
    PER_MEMBER_PERSISTENT_DEFAULT( MexVec2 );
    PER_FRIEND_READ_WRITE( MexVec2 );

private:

	MATHEX_SCALAR x_, y_;
};

PER_DECLARE_PERSISTENT( MexVec2 );

//class ostream;
//class istream;
ostream& operator<<(ostream& os, const MexVec2& vec);
istream& operator>>(istream& is,       MexVec2& vec);

//////////////////////////////////////////////////////////////////////

#ifdef _INLINE
    #include "mathex/vec2.ipp"
#endif

//////////////////////////////////////////////////////////////////////

#endif

/* End VEC2.HPP *****************************************************/
