/*
 * A N G L E . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MexAngle

    The base class for the MexDegrees and MexRadians clases. Store
    an angle in radians and allow various arithmetic operations to
    be performed on it.
*/

#ifndef _MATHEX_ANGLE_HPP
#define _MATHEX_ANGLE_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "mathex/mathex.hpp"

//class ostream;

class MexAngle
{
public:
    MexAngle( void );
    MexAngle( MATHEX_SCALAR );
    MexAngle( const MexAngle& );

    MexAngle& operator =( const MexAngle& );

    void operator +=( const MexAngle& );

    void operator -=( const MexAngle& );

    void operator *=( MATHEX_SCALAR );
    void operator /=( MATHEX_SCALAR );

	friend ostream& operator<<(ostream& str, const MexAngle& t);

    //Persistence
    PER_MEMBER_PERSISTENT_DEFAULT( MexAngle );
    PER_FRIEND_READ_WRITE( MexAngle );

    void CLASS_INVARIANT;

protected:

	//	Only allow angles to be destructed by their
	//  derived classes i.e. you cannot create a MexAngle directly.

    ~MexAngle( void );

    MexAngle& operator =( MATHEX_SCALAR );

    MATHEX_SCALAR   angleInRadians( void ) const;

private:

    MATHEX_SCALAR   angleInRadians_;

    friend  double  sin( const MexAngle& );
    friend  double  cos( const MexAngle& );
    friend  double  tan( const MexAngle& );

    friend  bool operator ==( const MexAngle&, const MexAngle& );
    friend  bool operator <( const MexAngle&, const MexAngle& );
    friend  bool operator >( const MexAngle&, const MexAngle& );
    friend  bool operator <=( const MexAngle&, const MexAngle& );
    friend  bool operator >=( const MexAngle&, const MexAngle& );
};

PER_DECLARE_PERSISTENT( MexAngle );

//  Because MexAngle, MexRadians and MexDegrees are all extremely
//  closely linked all of the #includes go through angles.hpp. This
//  helps sort out circular dependancy problems.

#include "mathex/radians.hpp"
#include "mathex/degrees.hpp"

#ifdef _INLINE
    #include "mathex/angle.ipp"
    #include "mathex/radians.ipp"
    #include "mathex/degrees.ipp"
#endif


#endif

/* End ANGLE.HPP **************************************************/
