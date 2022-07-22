/*
 * D E G R E E S . H P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MexDegrees

    Store an angle measured in degrees
*/

//  Because MexAngle, MexRadians and MexDegrees are all extremely
//  closely linked all of the #includes go through angles.hpp. This
//  helps sort out circular dependancy problems.

#include "mathex/angle.hpp"

#ifndef _MATHEX_DEGREES_HPP
#define _MATHEX_DEGREES_HPP

#include "base/persist.hpp"
#include "base/base.hpp"

class MexDegrees : public MexAngle
{
public:
    MexDegrees();
    MexDegrees( MATHEX_SCALAR );
    MexDegrees( const MexAngle& );

    MATHEX_SCALAR   asScalar() const;
    
    friend ostream& operator <<( ostream& o, const MexDegrees& t );

    //Persistence
    PER_MEMBER_PERSISTENT_DEFAULT( MexDegrees );
    PER_FRIEND_READ_WRITE( MexDegrees );

};

PER_DECLARE_PERSISTENT( MexDegrees );

class MexRadians;

const MexDegrees   operator +( const MexDegrees& a, const MexDegrees& b );
const MexDegrees   operator +( const MexDegrees& a, const MexRadians& b );

const MexDegrees   operator -( const MexDegrees& a, const MexDegrees& b );
const MexDegrees   operator -( const MexDegrees& a, const MexRadians& b );
const MexDegrees   operator -( const MexDegrees& a );

const MexDegrees   operator *( const MexDegrees&, MATHEX_SCALAR );
const MexDegrees   operator *( MATHEX_SCALAR, const MexDegrees& );

const MexDegrees   operator /( const MexDegrees&, MATHEX_SCALAR );

#endif

/* End DEGREES.HPP **************************************************/
