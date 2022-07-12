/*
 * P O I N T 2 D . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MexPoint2d

    Represents a point in 2d space.

	If you change any data members then don't forget to change the compressed version.
*/

#ifndef _MATHEX_POINT2D_HPP
#define _MATHEX_POINT2D_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "mathex/mathex.hpp"

//Forward declarations
class MexVec2;
class MexPoint3d;
class MexCompressedPoint2d;

//Orthodox canonical
class MexPoint2d
{
public:
    //ctors
    MexPoint2d( MATHEX_SCALAR x = 0, MATHEX_SCALAR y = 0 );
    MexPoint2d( const MexPoint2d& rhs);
    MexPoint2d( const MexVec2& rhs);

    MexPoint2d( const MexPoint3d& p3 );
	// POST( *this == MexPoint2d( p3.x(), p3.y() ) );

    //The point a proportion of the distance f from p to q
    MexPoint2d( const MexPoint2d& p, const MexPoint2d& q, MATHEX_SCALAR f );

    MexPoint2d( const MexCompressedPoint2d& p );

    //dtor.
    ~MexPoint2d();

    // Operator overloads
    MexPoint2d& operator =( const MexPoint2d& rhs );
    bool operator ==( const MexPoint2d& ) const;
    bool operator !=( const MexPoint2d& ) const;

    //Get/Set coordinates
    MATHEX_SCALAR x() const;
    MATHEX_SCALAR y() const;
    void x( MATHEX_SCALAR xNew );
    void y( MATHEX_SCALAR yNew );

    MATHEX_SCALAR operator []( size_t i ) const;
    //PRE( i == 0  or  i == 1 )

    //Add/subtract a vector
    void operator +=( const MexVec2& rhs );
    void operator -=( const MexVec2& rhs );

    //Distance from rhs, and squared distance
    MATHEX_SCALAR euclidianDistance( const MexPoint2d& rhs ) const;
    MATHEX_SCALAR sqrEuclidianDistance( const MexPoint2d& rhs ) const;

    void CLASS_INVARIANT;

    friend ostream& operator <<( ostream& o, const MexPoint2d& t );

    //Persistence
    PER_MEMBER_PERSISTENT_DEFAULT( MexPoint2d );
    PER_FRIEND_READ_WRITE( MexPoint2d );

private:
    //The coordinates
    MATHEX_SCALAR x_;
    MATHEX_SCALAR y_;
};

bool operator <( const MexPoint2d&, const MexPoint2d& );
bool operator >( const MexPoint2d&, const MexPoint2d& );
bool operator <=( const MexPoint2d&, const MexPoint2d& );
bool operator >=( const MexPoint2d&, const MexPoint2d& );

PER_DECLARE_PERSISTENT( MexPoint2d );

#ifdef _INLINE
    #include "Mathex/point2d.ipp"
#endif


#endif

/* End POINT2D.HPP **************************************************/
