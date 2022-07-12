/*
 * Q U A T E R N . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

/*
    MexQuaternion

    Represents a rotation as a quaternion. This is represented as
    a rotation of angle phi about a direction vector u. The data is stored as
    a vector with value (u * sin( 0.5 * phi )), and a scalar cos( 0.5 * phi ).
*/

#ifndef _MATHEX_QUATERN_HPP
#define _MATHEX_QUATERN_HPP

#include "base/base.hpp"
#include "base/persist.hpp"

#include "mathex/mathex.hpp"
#include "mathex/vec3.hpp"

class MexRadians;

//Memberwise canonical
class MexQuaternion
{
public:
    //Default constructor initialises to null rotation
    MexQuaternion();

    //Set the euler parameters directly.
    MexQuaternion( MATHEX_SCALAR x, MATHEX_SCALAR y, MATHEX_SCALAR z, MATHEX_SCALAR s );
    // PRE( fabs( x*x + y*y + z*z + s*s - 1.0 )  < MexEpsilon::instance() )

    //Set the euler parameters from a non-zero direction vector and angle in radians
    MexQuaternion( const MexVec3& d, const MexRadians& angle );
    // PRE( d.modulus() > 0.0 );

    //Export the vector and scalar values
    const MexVec3& vector() const;
    const MATHEX_SCALAR& scalar() const;

    //Return the direction of the rotation axis in pUnitVector, and the angle of
    //rotation in radians as result
    MATHEX_SCALAR rotation( MexVec3* pUnitVector ) const;
    // POST( fabs( pUnitVector.modulus - 1.0 ) < MexEpsilon::instance() );


    //Set the euler parameters directly.
    void set( MATHEX_SCALAR x, MATHEX_SCALAR y, MATHEX_SCALAR z, MATHEX_SCALAR s );
    //  PRE( fabs( x*x + y*y + z*z + s*s - 1.0 )  < MexEpsilon::instance() )

    //Set the euler parameters from a non-zero direction vector and angle in radians
    void set( const MexVec3& d, const MexRadians& angle );

	static void normalise( MATHEX_SCALAR* pX, MATHEX_SCALAR* pY, MATHEX_SCALAR* pZ, MATHEX_SCALAR* pS );
	// POST( fabs( (*pX)*(*pX) + (*pY)*(*pY) + (*pZ)*(*pZ) + (*pS)*(*pS) - 1.0 )  < MexEpsilon::instance() );

    void CLASS_INVARIANT;
    friend ostream& operator <<( ostream& o, const MexQuaternion& t );

    //Persistence
    PER_MEMBER_PERSISTENT_DEFAULT( MexQuaternion );
    PER_FRIEND_READ_WRITE( MexQuaternion );

private:
    //Data members
    MexVec3         v_; //Unit direction vector * sin(half rotation angle)
    MATHEX_SCALAR   s_; // cos(half rotation angle)

    friend bool operator ==( const MexQuaternion&, const MexQuaternion& );
    friend bool operator !=( const MexQuaternion&, const MexQuaternion& );
};


PER_DECLARE_PERSISTENT( MexQuaternion );

#ifdef _INLINE
    #include "mathex/quatern.ipp"
#endif


#endif

/* End EULPARAM.HPP *************************************************/
