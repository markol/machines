/*
 * R A D I A N S . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MexRadians

    Store an angle in radians
*/

//  Because MexAngle, MexRadians and MexDegrees are all extremely
//  closely linked all of the #includes go through angles.hpp. This
//  helps sort out circular dependancy problems.

#include "mathex/angle.hpp"

#ifndef _MATHEX_RADIANS_HPP
#define _MATHEX_RADIANS_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

class MexRadians : public MexAngle
{
public:

    MexRadians();
    MexRadians( MATHEX_SCALAR );
    MexRadians( const MexAngle& );

    MATHEX_SCALAR   asScalar() const;

    friend ostream& operator <<( ostream& o, const MexRadians& t );
    //Persistence
    PER_MEMBER_PERSISTENT_DEFAULT( MexRadians );
    PER_FRIEND_READ_WRITE( MexRadians );
};

PER_DECLARE_PERSISTENT( MexRadians );

class MexDegrees;

const MexRadians   operator +( const MexRadians& a, const MexRadians& b );
const MexRadians   operator +( const MexRadians& a, const MexDegrees& b );

const MexRadians   operator -( const MexRadians& a, const MexRadians& b );
const MexRadians   operator -( const MexRadians& a, const MexDegrees& b );
const MexRadians   operator -( const MexRadians& );

const MexRadians   operator *( const MexRadians&, MATHEX_SCALAR );
const MexRadians   operator *( MATHEX_SCALAR, const MexRadians& );

const MexRadians   operator /( const MexRadians&, MATHEX_SCALAR );

#endif

/* End RADIANS.HPP **************************************************/
