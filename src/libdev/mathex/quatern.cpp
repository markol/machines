/*
 * Q U A T E R N . C P P
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

//  Definitions of non-inline non-template methods and global functions

#include <math.h>

#include "mathex/mathex.hpp"
#include "mathex/quatern.hpp"
#include "mathex/vec3.hpp"
// #include "mathex/transf3d.hpp"
#include "mathex/radians.hpp"

#ifndef _INLINE
    #include "mathex/quatern.ipp"
#endif

    //Set the euler parameters directly.
MexQuaternion::MexQuaternion( MATHEX_SCALAR x, MATHEX_SCALAR y, MATHEX_SCALAR z, MATHEX_SCALAR s )
{
	PRE_INFO( x );
	PRE_INFO( y );
	PRE_INFO( z );
	PRE_INFO( s );

    PRE( fabs( x*x + y*y + z*z + s*s - 1.0 )  < MexEpsilon::instance() );

    v_.setVector( x, y, z );
    s_ = s;

    TEST_INVARIANT;
}

    //Set the euler parameters from a non-zero direction vector and angle in radians
MexQuaternion::MexQuaternion( const MexVec3& d, const MexRadians& angle)
{
    PRE( d.modulus() > 0.0 );

    //Store the vector and normalise it
    v_ = d;
    v_.makeUnitVector();

    //Get the cos and sine of the half-angle
    s_ = cos( angle * 0.5 );
    MATHEX_SCALAR f = sin( angle * 0.5 );
    v_ *= f;

    TEST_INVARIANT;
}

//////////////////////////////////////////////////////////////////////////////////////////

void MexQuaternion::set( MATHEX_SCALAR x, MATHEX_SCALAR y, MATHEX_SCALAR z, MATHEX_SCALAR s )
{
    TEST_INVARIANT;

	PRE_INFO( x );
	PRE_INFO( y );
	PRE_INFO( z );
	PRE_INFO( s );
	PRE_INFO( x*x + y*y + z*z + s*s );

    PRE( fabs( x*x + y*y + z*z + s*s - 1.0 )  < MexEpsilon::instance() )

    v_.setVector( x, y, z );
    s_ = s;

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

void MexQuaternion::set( const MexVec3& d, const MexRadians& angle )
{
    TEST_INVARIANT;

    //Store the vector and normalise it
    v_ = d;
    v_.makeUnitVector();

    //Get the cos and sine of the half-angle
    s_ = cos( angle * 0.5 );
    MATHEX_SCALAR f = sin( angle * 0.5 );
    v_ *= f;

    TEST_INVARIANT;
}
//////////////////////////////////////////////////////////////////////////////////////////

MATHEX_SCALAR MexQuaternion::rotation( MexVec3* pUnitVector ) const
{
    TEST_INVARIANT;

    //Compute the sine of the half - angle
    MATHEX_SCALAR halfSine = v_.modulus();

    //Hence compute the sine and cosine of the angle
    MATHEX_SCALAR sine = 2.0 * halfSine * s_;
    MATHEX_SCALAR cosine = 1.0 - 2.0 * halfSine * halfSine;
    MATHEX_SCALAR angle = atan2( sine, cosine );

    //Compute the unit direction vector. If the angle is very small,
    //use a dummy x vector
    if( halfSine < 0.000001)
    {
        pUnitVector->setVector( 1.0, 0.0, 0.0 );
    }
    else
    {
        pUnitVector->setVector( v_.x() / halfSine,
                                v_.y() / halfSine,
                                v_.z() / halfSine );
    }

    POST( fabs( pUnitVector->modulus() - 1.0 ) < MexEpsilon::instance() );

    TEST_INVARIANT;

    return angle;
}
//////////////////////////////////////////////////////////////////////////////////////////

void MexQuaternion::CLASS_INVARIANT
{
    //Sum of the squares must be 1
    MATHEX_SCALAR squareSum = v_.x() * v_.x() +
                              v_.y() * v_.y() +
                              v_.z() * v_.z() +
                              s_ * s_;

    INVARIANT( abs( squareSum - 1.0 ) < 0.000001 );
}
//////////////////////////////////////////////////////////////////////////////////////////

ostream& operator <<( ostream& o, const MexQuaternion& t )
{

    o << "MexQuaternion " << (void*)&t << " start" << std::endl;
    o << t.v_ << t.s_ << std::endl;
    o << "MexQuaternion " << (void*)&t << " end" << std::endl;

    return o;
}
//////////////////////////////////////////////////////////////////////////////////////////

PER_DEFINE_PERSISTENT( MexQuaternion );

void perWrite( PerOstream& str, const MexQuaternion& t )
{
    str << t.v_;
    str << t.s_;
}

void perRead( PerIstream& str, MexQuaternion& t )
{
    str >> t.v_;
    str >> t.s_;
}

// static
void MexQuaternion::normalise(
	MATHEX_SCALAR* pX, MATHEX_SCALAR* pY,
	MATHEX_SCALAR* pZ, MATHEX_SCALAR* pS )
{
	MATHEX_SCALAR k = (*pX)*(*pX) + (*pY)*(*pY) + (*pZ)*(*pZ) + (*pS)*(*pS);

	if( k != 1.0 )
	{
		k = sqrt( k );
		*pX /= k;
		*pY /= k;
		*pZ /= k;
		*pS /= k;
	}

	POST( fabs( (*pX)*(*pX) + (*pY)*(*pY) + (*pZ)*(*pZ) + (*pS)*(*pS) - 1.0 )  < MexEpsilon::instance() );
}

/* End EULPARAM.CPP *************************************************/
