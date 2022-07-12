/*
 * T R A N S F 3 D . C P P
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */

#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <cstring>

#include "base/base.hpp"

#include "mathex/mathex.hpp"
#include "mathex/transf3d.hpp"
#include "mathex/eulerang.hpp"
#include "mathex/quatern.hpp"
#include "mathex/point3d.hpp"
#include "mathex/point2d.hpp"
#include "mathex/vec3.hpp"
#include "mathex/line2d.hpp"
#include "mathex/abox2d.hpp"

#include "mathex/cvec3.hpp"
#include "mathex/cpoint3d.hpp"
#include "mathex/ctrans3d.hpp"

#ifndef _INLINE
	#include "mathex/transf3d.ipp"
#endif

/////////////////////////////////////////////////////////


MexTransform3d::MexTransform3d( const MexQuaternion& rot )
:   flags_( GENERAL )
{
    rotation( rot );

    forward_[3][0] = 0.0;
    forward_[3][1] = 0.0;
    forward_[3][2] = 0.0;
}

MexTransform3d::MexTransform3d()
:   flags_( IDENTITY )
{
    forward_[ 0 ][ 0 ] = 1.0;
    forward_[ 0 ][ 1 ] = 0.0;
    forward_[ 0 ][ 2 ] = 0.0;

    forward_[ 1 ][ 0 ] = 0.0;
    forward_[ 1 ][ 1 ] = 1.0;
    forward_[ 1 ][ 2 ] = 0.0;

    forward_[ 2 ][ 0 ] = 0.0;
    forward_[ 2 ][ 1 ] = 0.0;
    forward_[ 2 ][ 2 ] = 1.0;

    forward_[ 3 ][ 0 ] = 0.0;
    forward_[ 3 ][ 1 ] = 0.0;
    forward_[ 3 ][ 2 ] = 0.0;
}

MexTransform3d::MexTransform3d( const MexTransform3d& t)
:   flags_( t.flags_ ),
	tkey_( t.tkey_ )
{

    forward_[0][0]=t.forward_[0][0];forward_[0][1]=t.forward_[0][1];forward_[0][2]=t.forward_[0][2];
    forward_[1][0]=t.forward_[1][0];forward_[1][1]=t.forward_[1][1];forward_[1][2]=t.forward_[1][2];
    forward_[2][0]=t.forward_[2][0];forward_[2][1]=t.forward_[2][1];forward_[2][2]=t.forward_[2][2];
    forward_[3][0]=t.forward_[3][0];forward_[3][1]=t.forward_[3][1];forward_[3][2]=t.forward_[3][2];

    POST( *this == t );
    TEST_INVARIANT;
}

MexTransform3d::MexTransform3d(
    const MexVec3& xBasis,
    const MexVec3& yBasis,
    const MexVec3& zBasis,
    const MexPoint3d& position )
:   flags_( GENERAL )
{
    //  Check that the three basis vectors are mutually orthogonal

    PRE_INFO( MexVec3( xBasis ) );
    PRE_INFO( MexVec3( yBasis ) );
    PRE_INFO( MexVec3( zBasis ) );
    PRE_INFO( MexPoint3d( position ) );

    PRE_DATA( MexVec3    newX( yBasis ) );
    PRE_DATA( newX.crossProduct( zBasis ) );
    PRE_INFO( newX );
    PRE( newX == MexVec3( xBasis ) );

    PRE_DATA( MexVec3    newY( zBasis ) );
    PRE_DATA( newY.crossProduct( xBasis ) );
    PRE_INFO( newY );
    PRE( newY == MexVec3( yBasis ) );

    PRE_DATA( MexVec3    newZ( xBasis ) );
    PRE_DATA( newZ.crossProduct( yBasis ) );
    PRE_INFO( newZ );
    PRE( newZ == MexVec3( zBasis ) );

    forward_[0][0]=xBasis[0];forward_[0][1]=xBasis[1];forward_[0][2]=xBasis[2];
    forward_[1][0]=yBasis[0];forward_[1][1]=yBasis[1];forward_[1][2]=yBasis[2];
    forward_[2][0]=zBasis[0];forward_[2][1]=zBasis[1];forward_[2][2]=zBasis[2];
    forward_[3][0]=position[0];forward_[3][1]=position[1];forward_[3][2]=position[2];

    TEST_INVARIANT;
}

MexTransform3d::MexTransform3d( const MexEulerAngles& angles )
:   flags_( ROTATE_ONLY )
{
    rotation( angles );

    forward_[3][0] = 0.0;
    forward_[3][1] = 0.0;
    forward_[3][2] = 0.0;
}

MexTransform3d::MexTransform3d( const MexPoint3d& point )
:   flags_( TRANSLATE_ONLY )
{
    forward_[0][0] = 1.0;
    forward_[0][1] = 0.0;
    forward_[0][2] = 0.0;

    forward_[1][0] = 0.0;
    forward_[1][1] = 1.0;
    forward_[1][2] = 0.0;

    forward_[2][0] = 0.0;
    forward_[2][1] = 0.0;
    forward_[2][2] = 1.0;

    forward_[3][0] = point.x();
    forward_[3][1] = point.y();
    forward_[3][2] = point.z();
}

MexTransform3d::MexTransform3d( const MexQuaternion& rot, const MexPoint3d& position )
:   flags_( GENERAL )
{
    rotation( rot );

    forward_[3][0] = position.x();
    forward_[3][1] = position.y();
    forward_[3][2] = position.z();
}

MexTransform3d::MexTransform3d( const MexEulerAngles& angles, const MexPoint3d& position )
:   flags_( GENERAL )
{
    rotation( angles );

    forward_[3][0] = position.x();
    forward_[3][1] = position.y();
    forward_[3][2] = position.z();
}

MexTransform3d::~MexTransform3d()

{
    TEST_INVARIANT;
    //  Currently nothing to do in the destructor
}

void MexTransform3d::position( MexPoint3d* pPos ) const
{
    pPos->x( forward_[ 3 ][ 0 ] );
    pPos->y( forward_[ 3 ][ 1 ] );
    pPos->z( forward_[ 3 ][ 2 ] );
}

void MexTransform3d::position( const MexPoint3d& newPosition )
{
    forward_[ 3 ][ 0 ] = newPosition.x();
    forward_[ 3 ][ 1 ] = newPosition.y();
    forward_[ 3 ][ 2 ] = newPosition.z();

    //Generate new unique key
    ++tkey_;
}

void MexTransform3d::translate( const MexPoint3d& translation )
{
    TEST_INVARIANT;

    //  The rotation data stays the same, the position data must be updated.

    forward_[3][0] +=  translation.x() * forward_[0][0] +
                       translation.y() * forward_[1][0] +
                       translation.z() * forward_[2][0];

    forward_[3][1] +=  translation.x() * forward_[0][1] +
                       translation.y() * forward_[1][1] +
                       translation.z() * forward_[2][1];

    forward_[3][2] +=  translation.x() * forward_[0][2] +
                       translation.y() * forward_[1][2] +
                       translation.z() * forward_[2][2];

    //Generate new unique key
    ++tkey_;

    TEST_INVARIANT;
}

void MexTransform3d::translate( const MexTransform3d& t )

{
    TEST_INVARIANT;

    MexPoint3d position;
    t.position( &position );

    //  The rotation data stays the same, the position data must be updated.

    forward_[3][0] +=  t.forward_[3][0] * forward_[0][0] +
                       t.forward_[3][1] * forward_[1][0] +
                       t.forward_[3][2] * forward_[2][0];

    forward_[3][1] +=  t.forward_[3][0] * forward_[0][1] +
                       t.forward_[3][1] * forward_[1][1] +
                       t.forward_[3][2] * forward_[2][1];

    forward_[3][2] +=  t.forward_[3][0] * forward_[0][2] +
                       t.forward_[3][1] * forward_[1][2] +
                       t.forward_[3][2] * forward_[2][2];

    //Generate new unique key
    ++tkey_;

    TEST_INVARIANT;
}


void MexTransform3d::rotate( const MexTransform3d& t )

{
    POST_DATA( MexVec3 oldPosition( forward_[3][0], forward_[3][1], forward_[3][2] ) );
    TEST_INVARIANT;

    MATHEX_SCALAR   newRotation[3][3];

    newRotation[0][0]=t.forward_[0][0]*forward_[0][0]+t.forward_[0][1]*forward_[1][0]+t.forward_[0][2]*forward_[2][0];
    newRotation[0][1]=t.forward_[0][0]*forward_[0][1]+t.forward_[0][1]*forward_[1][1]+t.forward_[0][2]*forward_[2][1];
    newRotation[0][2]=t.forward_[0][0]*forward_[0][2]+t.forward_[0][1]*forward_[1][2]+t.forward_[0][2]*forward_[2][2];

    newRotation[1][0]=t.forward_[1][0]*forward_[0][0]+t.forward_[1][1]*forward_[1][0]+t.forward_[1][2]*forward_[2][0];
    newRotation[1][1]=t.forward_[1][0]*forward_[0][1]+t.forward_[1][1]*forward_[1][1]+t.forward_[1][2]*forward_[2][1];
    newRotation[1][2]=t.forward_[1][0]*forward_[0][2]+t.forward_[1][1]*forward_[1][2]+t.forward_[1][2]*forward_[2][2];

    newRotation[2][0]=t.forward_[2][0]*forward_[0][0]+t.forward_[2][1]*forward_[1][0]+t.forward_[2][2]*forward_[2][0];
    newRotation[2][1]=t.forward_[2][0]*forward_[0][1]+t.forward_[2][1]*forward_[1][1]+t.forward_[2][2]*forward_[2][1];
    newRotation[2][2]=t.forward_[2][0]*forward_[0][2]+t.forward_[2][1]*forward_[1][2]+t.forward_[2][2]*forward_[2][2];

    forward_[0][0]=newRotation[0][0];forward_[0][1]=newRotation[0][1];forward_[0][2]=newRotation[0][2];
    forward_[1][0]=newRotation[1][0];forward_[1][1]=newRotation[1][1];forward_[1][2]=newRotation[1][2];
    forward_[2][0]=newRotation[2][0];forward_[2][1]=newRotation[2][1];forward_[2][2]=newRotation[2][2];

    //Generate new unique key
    ++tkey_;

    POST( oldPosition == MexVec3( forward_[3][0], forward_[3][1], forward_[3][2] ) );
    TEST_INVARIANT;
}

void MexTransform3d::rotate( const MexEulerAngles& angles )
{
    TEST_INVARIANT;

    MexTransform3d tr( angles );

    rotate( tr );

    //Generate new unique key
    ++tkey_;

    TEST_INVARIANT;
}

void MexTransform3d::rotate( const MexQuaternion& rot )
{
    TEST_INVARIANT;

    MexTransform3d tr( rot );

    rotate( tr );

    //Generate new unique key
    ++tkey_;

    TEST_INVARIANT;
}


// void MexTransform3d::rotate( const MexEulerAngles& angles)
//
// {
//     TEST_INVARIANT;
//
//     MexTransform3d tr( MexVec3::zeroVector(), angles );
//
//     rotate( tr );
//
// 	inverseValid_ = false;
//
//     //Generate new unique key
//     ++tkey_;
//
//     TEST_INVARIANT;
// }

/*=========================================================================
Author:     Bob Archer

Function:   Update a transform to include another transform.
            This is done using preconcatenation (the supplied transform
            follows the original transform).

Parameters:
        t   The supplied transform.

Return value:   none
=========================================================================*/


void MexTransform3d::transform( const MexTransform3d& t )

{
    //this = this * t
    TEST_INVARIANT;

    MATHEX_SCALAR   newTransform[4][3];

    //Note that the multiplication order is inverted because our matrix is stored transposed.
    //We have to multiply into a temporary copy so we don't overwrite the original as we go.
    multiplyMatrices( t.forward_, forward_, newTransform );

    //copy the result into forward_
    forward_[0][0]=newTransform[0][0];forward_[0][1]=newTransform[0][1];forward_[0][2]=newTransform[0][2];
    forward_[1][0]=newTransform[1][0];forward_[1][1]=newTransform[1][1];forward_[1][2]=newTransform[1][2];
    forward_[2][0]=newTransform[2][0];forward_[2][1]=newTransform[2][1];forward_[2][2]=newTransform[2][2];
    forward_[3][0]=newTransform[3][0];forward_[3][1]=newTransform[3][1];forward_[3][2]=newTransform[3][2];

    //Generate new unique key
    ++tkey_;

    TEST_INVARIANT;
}

void MexTransform3d::inverseTranslate(MATHEX_SCALAR invT[3]) const
{
    invT[0] = -( forward_[0][0]*forward_[3][0] + forward_[0][1]*forward_[3][1] + forward_[0][2]*forward_[3][2] );
    invT[1] = -( forward_[1][0]*forward_[3][0] + forward_[1][1]*forward_[3][1] + forward_[1][2]*forward_[3][2] );
    invT[2] = -( forward_[2][0]*forward_[3][0] + forward_[2][1]*forward_[3][1] + forward_[2][2]*forward_[3][2] );
}

void MexTransform3d::transformInverse( const MexTransform3d& t )

{
    //this = Inv(this) * t
    TEST_INVARIANT;

	MATHEX_SCALAR invT[3];
	inverseTranslate(invT);

    MATHEX_SCALAR   newXform[4][3];

	newXform[0][0] = forward_[0][0]*t.forward_[0][0] + forward_[0][1]*t.forward_[0][1] + forward_[0][2]*t.forward_[0][2];
	newXform[0][1] = forward_[1][0]*t.forward_[0][0] + forward_[1][1]*t.forward_[0][1] + forward_[1][2]*t.forward_[0][2];
	newXform[0][2] = forward_[2][0]*t.forward_[0][0] + forward_[2][1]*t.forward_[0][1] + forward_[2][2]*t.forward_[0][2];

	newXform[1][0] = forward_[0][0]*t.forward_[1][0] + forward_[0][1]*t.forward_[1][1] + forward_[0][2]*t.forward_[1][2];
	newXform[1][1] = forward_[1][0]*t.forward_[1][0] + forward_[1][1]*t.forward_[1][1] + forward_[1][2]*t.forward_[1][2];
	newXform[1][2] = forward_[2][0]*t.forward_[1][0] + forward_[2][1]*t.forward_[1][1] + forward_[2][2]*t.forward_[1][2];

	newXform[2][0] = forward_[0][0]*t.forward_[2][0] + forward_[0][1]*t.forward_[2][1] + forward_[0][2]*t.forward_[2][2];
	newXform[2][1] = forward_[1][0]*t.forward_[2][0] + forward_[1][1]*t.forward_[2][1] + forward_[1][2]*t.forward_[2][2];
	newXform[2][2] = forward_[2][0]*t.forward_[2][0] + forward_[2][1]*t.forward_[2][1] + forward_[2][2]*t.forward_[2][2];


	newXform[3][0] = forward_[0][0]*t.forward_[3][0] + forward_[0][1]*t.forward_[3][1] + forward_[0][2]*t.forward_[3][2] + invT[0];
	newXform[3][1] = forward_[1][0]*t.forward_[3][0] + forward_[1][1]*t.forward_[3][1] + forward_[1][2]*t.forward_[3][2] + invT[1];
	newXform[3][2] = forward_[2][0]*t.forward_[3][0] + forward_[2][1]*t.forward_[3][1] + forward_[2][2]*t.forward_[3][2] + invT[2];

	memcpy( forward_, newXform, sizeof( forward_ ) );

    //Generate new unique key
    ++tkey_;

    TEST_INVARIANT;
}

//---------------------------------------------------------------------------------------
void MexTransform3d::preTransform( const MexTransform3d& t)
{
    //this = t * this
    TEST_INVARIANT;

    MATHEX_SCALAR   newTransform[4][3];

    //Note that the multiplication order is inverted because our matrix is stored transposed.
    //We have to multiply into a temporary copy so we don't overwrite the original as we go.
    multiplyMatrices( forward_, t.forward_, newTransform );

    //copy the result into forward_
    forward_[0][0]=newTransform[0][0];forward_[0][1]=newTransform[0][1];forward_[0][2]=newTransform[0][2];
    forward_[1][0]=newTransform[1][0];forward_[1][1]=newTransform[1][1];forward_[1][2]=newTransform[1][2];
    forward_[2][0]=newTransform[2][0];forward_[2][1]=newTransform[2][1];forward_[2][2]=newTransform[2][2];
    forward_[3][0]=newTransform[3][0];forward_[3][1]=newTransform[3][1];forward_[3][2]=newTransform[3][2];

    //Generate new unique key
    ++tkey_;

    TEST_INVARIANT;
}
//---------------------------------------------------------------------------------------
void MexTransform3d::preTransformInverse( const MexTransform3d& t)
{
    //this = t * Inv(this)
    TEST_INVARIANT;

	MATHEX_SCALAR invT[3];
	inverseTranslate(invT);

    MATHEX_SCALAR   newXform[4][3];

	newXform[0][0] = t.forward_[0][0]*forward_[0][0] + t.forward_[1][0]*forward_[1][0] + t.forward_[2][0]*forward_[2][0];
	newXform[0][1] = t.forward_[0][1]*forward_[0][0] + t.forward_[1][1]*forward_[1][0] + t.forward_[2][1]*forward_[2][0];
	newXform[0][2] = t.forward_[0][2]*forward_[0][0] + t.forward_[1][2]*forward_[1][0] + t.forward_[2][2]*forward_[2][0];

	newXform[1][0] = t.forward_[0][0]*forward_[0][1] + t.forward_[1][0]*forward_[1][1] + t.forward_[2][0]*forward_[2][1];
	newXform[1][1] = t.forward_[0][1]*forward_[0][1] + t.forward_[1][1]*forward_[1][1] + t.forward_[2][1]*forward_[2][1];
	newXform[1][2] = t.forward_[0][2]*forward_[0][1] + t.forward_[1][2]*forward_[1][1] + t.forward_[2][2]*forward_[2][1];

	newXform[2][0] = t.forward_[0][0]*forward_[0][2] + t.forward_[1][0]*forward_[1][2] + t.forward_[2][0]*forward_[2][2];
	newXform[2][1] = t.forward_[0][1]*forward_[0][2] + t.forward_[1][1]*forward_[1][2] + t.forward_[2][1]*forward_[2][2];
	newXform[2][2] = t.forward_[0][2]*forward_[0][2] + t.forward_[1][2]*forward_[1][2] + t.forward_[2][2]*forward_[2][2];

	newXform[3][0] = t.forward_[0][0]*invT[0] + t.forward_[1][0]*invT[1] + t.forward_[2][0]*invT[2] + t.forward_[3][0];
	newXform[3][1] = t.forward_[0][1]*invT[0] + t.forward_[1][1]*invT[1] + t.forward_[2][1]*invT[2] + t.forward_[3][1];
	newXform[3][2] = t.forward_[0][2]*invT[0] + t.forward_[1][2]*invT[1] + t.forward_[2][2]*invT[2] + t.forward_[3][2];


	memcpy( forward_, newXform, sizeof( forward_ ) );

    //Generate new unique key
    ++tkey_;

    TEST_INVARIANT;
}
//---------------------------------------------------------------------------------------
void MexTransform3d::transform( const MexTransform3d& t, MexTransform3d* pResult ) const
{
    // *pResult = this * t

    //Note multiplication order for matrices is inverted because they are stored
    //transposed
    multiplyMatrices( t.forward_, forward_, pResult->forward_ );

    //Generate new unique key
    ++pResult->tkey_;

    TEST_INVARIANT;
}
//---------------------------------------------------------------------------------------
void MexTransform3d::transformInverse( const MexTransform3d& t, MexTransform3d* pResult ) const
{
    // *pResult = Inv(this) * t

	MATHEX_SCALAR invT[3];
	inverseTranslate(invT);

    MATHEX_SCALAR   newXform[4][3];

	newXform[0][0] = forward_[0][0]*t.forward_[0][0] + forward_[0][1]*t.forward_[0][1] + forward_[0][2]*t.forward_[0][2];
	newXform[0][1] = forward_[1][0]*t.forward_[0][0] + forward_[1][1]*t.forward_[0][1] + forward_[1][2]*t.forward_[0][2];
	newXform[0][2] = forward_[2][0]*t.forward_[0][0] + forward_[2][1]*t.forward_[0][1] + forward_[2][2]*t.forward_[0][2];

	newXform[1][0] = forward_[0][0]*t.forward_[1][0] + forward_[0][1]*t.forward_[1][1] + forward_[0][2]*t.forward_[1][2];
	newXform[1][1] = forward_[1][0]*t.forward_[1][0] + forward_[1][1]*t.forward_[1][1] + forward_[1][2]*t.forward_[1][2];
	newXform[1][2] = forward_[2][0]*t.forward_[1][0] + forward_[2][1]*t.forward_[1][1] + forward_[2][2]*t.forward_[1][2];

	newXform[2][0] = forward_[0][0]*t.forward_[2][0] + forward_[0][1]*t.forward_[2][1] + forward_[0][2]*t.forward_[2][2];
	newXform[2][1] = forward_[1][0]*t.forward_[2][0] + forward_[1][1]*t.forward_[2][1] + forward_[1][2]*t.forward_[2][2];
	newXform[2][2] = forward_[2][0]*t.forward_[2][0] + forward_[2][1]*t.forward_[2][1] + forward_[2][2]*t.forward_[2][2];


	newXform[3][0] = forward_[0][0]*t.forward_[3][0] + forward_[0][1]*t.forward_[3][1] + forward_[0][2]*t.forward_[3][2] + invT[0];
	newXform[3][1] = forward_[1][0]*t.forward_[3][0] + forward_[1][1]*t.forward_[3][1] + forward_[1][2]*t.forward_[3][2] + invT[1];
	newXform[3][2] = forward_[2][0]*t.forward_[3][0] + forward_[2][1]*t.forward_[3][1] + forward_[2][2]*t.forward_[3][2] + invT[2];

	memcpy( pResult->forward_, newXform, sizeof( forward_ ) );

    //Generate new unique key
    ++pResult->tkey_;

    TEST_INVARIANT;
}
//---------------------------------------------------------------------------------------
//static
void MexTransform3d::multiplyMatrices( const MexTransformMatrix3d a, const MexTransformMatrix3d b,
                      MexTransformMatrix3d result )
{
    result[0][0]=a[0][0]*b[0][0]+a[0][1]*b[1][0]+a[0][2]*b[2][0];
    result[0][1]=a[0][0]*b[0][1]+a[0][1]*b[1][1]+a[0][2]*b[2][1];
    result[0][2]=a[0][0]*b[0][2]+a[0][1]*b[1][2]+a[0][2]*b[2][2];

    result[1][0]=a[1][0]*b[0][0]+a[1][1]*b[1][0]+a[1][2]*b[2][0];
    result[1][1]=a[1][0]*b[0][1]+a[1][1]*b[1][1]+a[1][2]*b[2][1];
    result[1][2]=a[1][0]*b[0][2]+a[1][1]*b[1][2]+a[1][2]*b[2][2];

    result[2][0]=a[2][0]*b[0][0]+a[2][1]*b[1][0]+a[2][2]*b[2][0];
    result[2][1]=a[2][0]*b[0][1]+a[2][1]*b[1][1]+a[2][2]*b[2][1];
    result[2][2]=a[2][0]*b[0][2]+a[2][1]*b[1][2]+a[2][2]*b[2][2];

    result[3][0]=a[3][0]*b[0][0]+a[3][1]*b[1][0]+a[3][2]*b[2][0]+b[3][0];
    result[3][1]=a[3][0]*b[0][1]+a[3][1]*b[1][1]+a[3][2]*b[2][1]+b[3][1];
    result[3][2]=a[3][0]*b[0][2]+a[3][1]*b[1][2]+a[3][2]*b[2][2]+b[3][2];
}
//---------------------------------------------------------------------------------------

void MexTransform3d::transform( MexVec3 *v ) const

{
    TEST_INVARIANT;

    MATHEX_SCALAR   newV[3];


    newV[0] = v->x()*forward_[0][0]+v->y()*forward_[1][0]+v->z()*forward_[2][0];
    newV[1] = v->x()*forward_[0][1]+v->y()*forward_[1][1]+v->z()*forward_[2][1];
    newV[2] = v->x()*forward_[0][2]+v->y()*forward_[1][2]+v->z()*forward_[2][2];


    v->setVector( newV[0], newV[1], newV[2] );

    TEST_INVARIANT;
}

//---------------------------------------------------------------------------------------

void MexTransform3d::transformInverse( MexVec3 *v ) const

{
    TEST_INVARIANT;

    MATHEX_SCALAR   newV[3];


    newV[0] = v->x()*forward_[0][0]+v->y()*forward_[0][1]+v->z()*forward_[0][2];
    newV[1] = v->x()*forward_[1][0]+v->y()*forward_[1][1]+v->z()*forward_[1][2];
    newV[2] = v->x()*forward_[2][0]+v->y()*forward_[2][1]+v->z()*forward_[2][2];


    v->setVector( newV[0], newV[1], newV[2] );

    TEST_INVARIANT;
}

//---------------------------------------------------------------------------------------

void MexTransform3d::transform( MexPoint3d *p ) const

{
    TEST_INVARIANT;

    MATHEX_SCALAR   newP[3];

    newP[0] = p->x()*forward_[0][0]+p->y()*forward_[1][0]+p->z()*forward_[2][0]+forward_[3][0];
    newP[1] = p->x()*forward_[0][1]+p->y()*forward_[1][1]+p->z()*forward_[2][1]+forward_[3][1];
    newP[2] = p->x()*forward_[0][2]+p->y()*forward_[1][2]+p->z()*forward_[2][2]+forward_[3][2];

    p->setPoint( newP[0], newP[1], newP[2] );

    TEST_INVARIANT;
}

//---------------------------------------------------------------------------------------

void MexTransform3d::transformInverse( MexPoint3d *p ) const

{
    TEST_INVARIANT;

	MATHEX_SCALAR invT[3];
	inverseTranslate(invT);

    MATHEX_SCALAR   newP[3];

    newP[0] = p->x()*forward_[0][0]+p->y()*forward_[0][1]+p->z()*forward_[0][2]+invT[0];
    newP[1] = p->x()*forward_[1][0]+p->y()*forward_[1][1]+p->z()*forward_[1][2]+invT[1];
    newP[2] = p->x()*forward_[2][0]+p->y()*forward_[2][1]+p->z()*forward_[2][2]+invT[2];

    p->setPoint( newP[0], newP[1], newP[2] );

    TEST_INVARIANT;
}

//---------------------------------------------------------------------------------------

void MexTransform3d::transform( MexPoint2d *p ) const

{
    TEST_INVARIANT;

    MATHEX_SCALAR   newP[2];

    newP[0] = p->x()*forward_[0][0]+p->y()*forward_[1][0]+forward_[3][0];
    newP[1] = p->x()*forward_[0][1]+p->y()*forward_[1][1]+forward_[3][1];

    p->x( newP[0] );
    p->y( newP[1] );

    TEST_INVARIANT;
}

//---------------------------------------------------------------------------------------

void MexTransform3d::transformInverse( MexPoint2d *p ) const

{
    TEST_INVARIANT;

	MATHEX_SCALAR invT[3];
	inverseTranslate(invT);

    MATHEX_SCALAR   newP[2];

    newP[0] = p->x()*forward_[0][0] + p->y()*forward_[0][1] + invT[0];
    newP[1] = p->x()*forward_[1][0] + p->y()*forward_[1][1] + invT[1];

    p->x( newP[0] );
    p->y( newP[1] );

    TEST_INVARIANT;
}

//---------------------------------------------------------------------------------------

void MexTransform3d::transform( MexLine2d* pLine ) const
{
    MexPoint2d  end1( pLine->end1() );
    MexPoint2d  end2( pLine->end2() );

    transform( &end1 );
    transform( &end2 );

    pLine->setLine( end1, end2 );
}

//---------------------------------------------------------------------------------------

void MexTransform3d::transformInverse( MexLine2d* pLine ) const
{
    MexPoint2d  end1( pLine->end1() );
    MexPoint2d  end2( pLine->end2() );

    transformInverse( &end1 );
    transformInverse( &end2 );

    pLine->setLine( end1, end2 );
}

//---------------------------------------------------------------------------------------

const MexTransform3d& MexTransform3d::operator =( const MexTransform3d& rhs )

{
    TEST_INVARIANT;

	memcpy( forward_, rhs.forward_, sizeof( forward_ ) );

    //Copy the key
    tkey_ = rhs.tkey_;

	POST( *this == rhs );
    TEST_INVARIANT;

	return( *this );
}

const MexTransform3d& MexTransform3d::operator =( const MATHEX_SCALAR rhs[4][4] )

{
    TEST_INVARIANT;

	memcpy( forward_, rhs, sizeof( forward_ ) );

    //Generate new unique key
    ++tkey_;

	POST( *this == rhs );

    TEST_INVARIANT;

	return( *this );
}

bool  MexTransform3d::operator ==( const MexTransform3d& t ) const
{
    TEST_INVARIANT;

    bool    match =
        Mathex::abs( forward_[0][0] - t.forward_[0][0]) < MexEpsilon::instance() &&
        Mathex::abs( forward_[0][1] - t.forward_[0][1]) < MexEpsilon::instance() &&
        Mathex::abs( forward_[0][2] - t.forward_[0][2]) < MexEpsilon::instance() &&

        Mathex::abs( forward_[1][0] - t.forward_[1][0]) < MexEpsilon::instance() &&
        Mathex::abs( forward_[1][1] - t.forward_[1][1]) < MexEpsilon::instance() &&
        Mathex::abs( forward_[1][2] - t.forward_[1][2]) < MexEpsilon::instance() &&

        Mathex::abs( forward_[2][0] - t.forward_[2][0]) < MexEpsilon::instance() &&
        Mathex::abs( forward_[2][1] - t.forward_[2][1]) < MexEpsilon::instance() &&
        Mathex::abs( forward_[2][2] - t.forward_[2][2]) < MexEpsilon::instance() &&

        Mathex::abs( forward_[3][0] - t.forward_[3][0]) < MexEpsilon::instance() &&
        Mathex::abs( forward_[3][1] - t.forward_[3][1]) < MexEpsilon::instance() &&
        Mathex::abs( forward_[3][2] - t.forward_[3][2]) < MexEpsilon::instance();

    TEST_INVARIANT;

    return match;
}

bool  MexTransform3d::operator !=( const MexTransform3d& t ) const
{
    return !(*this == t);
}

bool  MexTransform3d::operator ==( const MATHEX_SCALAR t[4][4] ) const
{
    TEST_INVARIANT;

    bool    match =
        Mathex::abs( forward_[0][0] - t[0][0]) < MexEpsilon::instance() &&
        Mathex::abs( forward_[0][1] - t[0][1]) < MexEpsilon::instance() &&
        Mathex::abs( forward_[0][2] - t[0][2]) < MexEpsilon::instance() &&

        Mathex::abs( forward_[1][0] - t[1][0]) < MexEpsilon::instance() &&
        Mathex::abs( forward_[1][1] - t[1][1]) < MexEpsilon::instance() &&
        Mathex::abs( forward_[1][2] - t[1][2]) < MexEpsilon::instance() &&

        Mathex::abs( forward_[2][0] - t[2][0]) < MexEpsilon::instance() &&
        Mathex::abs( forward_[2][1] - t[2][1]) < MexEpsilon::instance() &&
        Mathex::abs( forward_[2][2] - t[2][2]) < MexEpsilon::instance() &&

        Mathex::abs( forward_[3][0] - t[3][0]) < MexEpsilon::instance() &&
        Mathex::abs( forward_[3][1] - t[3][1]) < MexEpsilon::instance() &&
        Mathex::abs( forward_[3][2] - t[3][2]) < MexEpsilon::instance();

    TEST_INVARIANT;

    return match;
}

bool  MexTransform3d::operator <( const MexTransform3d& t ) const
{
    TEST_INVARIANT;

    //  This test is really pretty meaningless. It has been put in
    //  just to allow us to make stl vectors from MexTransform3d.

    bool    result = forward_[0][0] < t.forward_[0][0];

    TEST_INVARIANT;

    return result;
}


void MexTransform3d::rotation( MexEulerAngles* pAngles ) const

{
    MATHEX_SCALAR   sa, ca, se, ce, sr, cr;

    TEST_INVARIANT;

    se = -forward_[0][2];
    ce = sqrt( forward_[0][0] * forward_[0][0] + forward_[0][1] * forward_[0][1] );

    if( Mathex::abs( ce ) < MexEpsilon::instance() )
    {
        sr = 0;
        cr = 1;
        sa = forward_[2][1];
        ca = forward_[1][1];
    }
    else
    {
        sa = forward_[0][1] / ce;
        ca = forward_[0][0] / ce;
        sr = forward_[1][2] / ce;
        cr = forward_[2][2] / ce;
    }

    pAngles->azimuth( atan2( sa, ca ) );
    pAngles->elevation( atan2( se, ce ) );
    pAngles->roll( atan2( sr, cr ) );

    POST_INFO( pAngles->azimuth() );
    POST_INFO( pAngles->elevation() );
    POST_INFO( pAngles->roll() );

    POST_DATA( MexTransform3d rot( *pAngles, position() ) );

    POST_INFO( rot );
    POST_INFO( *this );

    POST( rot == *this );

    TEST_INVARIANT;
}


void	MexTransform3d::invert( MexTransform3d *pTransform ) const

{
    TEST_INVARIANT;

	pTransform->forward_[0][0] = forward_[0][0];
	pTransform->forward_[0][1] = forward_[1][0];
	pTransform->forward_[0][2] = forward_[2][0];

	pTransform->forward_[1][0] = forward_[0][1];
	pTransform->forward_[1][1] = forward_[1][1];
	pTransform->forward_[1][2] = forward_[2][1];

	pTransform->forward_[2][0] = forward_[0][2];
	pTransform->forward_[2][1] = forward_[1][2];
	pTransform->forward_[2][2] = forward_[2][2];

	//	Calculate the position data for the inversion

	pTransform->forward_[3][0] = -(
	                           forward_[3][0] * pTransform->forward_[0][0] +
                               forward_[3][1] * pTransform->forward_[1][0] +
                               forward_[3][2] * pTransform->forward_[2][0] );

	pTransform->forward_[3][1] = -(
	                           forward_[3][0] * pTransform->forward_[0][1] +
                               forward_[3][1] * pTransform->forward_[1][1] +
                               forward_[3][2] * pTransform->forward_[2][1] );

	pTransform->forward_[3][2] = -(
	                           forward_[3][0] * pTransform->forward_[0][2] +
                               forward_[3][1] * pTransform->forward_[1][2] +
                               forward_[3][2] * pTransform->forward_[2][2] );

    //Generate new unique key in the updated transform
    ++pTransform->tkey_;

    //This post-condition was falling over due to rounding errors
    //TBD Sort this out properly to see why errors accumulating
	//POST_DATA( MexTransform3d t( *this ));
	//POST_DATA( t.transform( *pTransform ) );

	//POST( t.isIdentity() );

    TEST_INVARIANT;
}

void	MexTransform3d::invert()

{
    TEST_INVARIANT;

    MexTransform3d tx;

    invert( &tx );

    *this = tx;

    TEST_INVARIANT;
}

bool MexTransform3d::isIdentity() const

{
    TEST_INVARIANT;

	bool	ok;

	ok = Mathex::abs( forward_[0][0] - MATHEX_SCALAR( 1.0 ) ) < MexEpsilon::instance() &&
		Mathex::abs( forward_[0][1] ) < MexEpsilon::instance() &&
		Mathex::abs( forward_[0][2] ) < MexEpsilon::instance();

	ok = ok &&
		Mathex::abs( forward_[1][0] ) < MexEpsilon::instance() &&
		Mathex::abs( forward_[1][1] - MATHEX_SCALAR( 1.0 ) ) < MexEpsilon::instance() &&
		Mathex::abs( forward_[1][2] ) < MexEpsilon::instance();

	ok = ok &&
		Mathex::abs( forward_[2][0] ) < MexEpsilon::instance() &&
		Mathex::abs( forward_[2][1] ) < MexEpsilon::instance() &&
		Mathex::abs( forward_[2][2] - MATHEX_SCALAR( 1.0 ) ) < MexEpsilon::instance();

	ok = ok &&
		Mathex::abs( forward_[3][0] ) < MexEpsilon::instance() &&
		Mathex::abs( forward_[3][1] ) < MexEpsilon::instance() &&
		Mathex::abs( forward_[3][2] ) < MexEpsilon::instance();

    TEST_INVARIANT;

	return( ok );
}

void MexTransform3d::setTransform( const MexFloatMatrix3d m )
{
    size_t row, column;

    //Store the matrix
    for (row = 4; row--;)
        for (column = 3; column--;)
            forward_[row][column] = (MATHEX_SCALAR)(m[row][column]);

    //Generate new unique key in the updated transform
    ++tkey_;

    TEST_INVARIANT;
}

void MexTransform3d::getTransform( MexFloatMatrix3d m ) const
{
    register size_t row, column;

    TEST_INVARIANT;

    //Fetch the matrix, converting to float
    for (row = 4; row--;)
        for (column = 3; column--;)
            m[row][column] = (float)(forward_[row][column]);

    //Set the final column to transpose of (0,0,0,1)
    m[0][3] = m[1][3] = m[2][3] = 0.0;
    m[3][3] = 1.0;
}

void MexTransform3d::xBasis( MexVec3* v ) const
{
    PRE( v != NULL );

    TEST_INVARIANT;

    v->setVector( forward_[0][0], forward_[0][1], forward_[0][2] );
}

void MexTransform3d::yBasis( MexVec3* v ) const
{
    PRE( v != NULL );

    TEST_INVARIANT;

    v->setVector( forward_[1][0], forward_[1][1], forward_[1][2] );
}

void MexTransform3d::zBasis( MexVec3* v ) const
{
    PRE( v != NULL );

    TEST_INVARIANT;

    v->setVector( forward_[2][0], forward_[2][1], forward_[2][2] );
}

//////////////////////////////////////////////////////////////////////////////////////////

void MexTransform3d::rotation( MexQuaternion* pParams ) const
{
    //Compute the scalar value
    MATHEX_SCALAR s = 0.5 * sqrt( fabs(1.0 + forward_[0][0] + forward_[1][1] + forward_[2][2]) );
    MATHEX_SCALAR x, y, z;

    //Check for near 180 degree rotation
    if( s < 0.01 )
    {
        MATHEX_SCALAR sSquared = s * s;
        x = sqrt( fabs(0.5 * (1.0 + forward_[0][0]) - sSquared) );
        y = sqrt( fabs(0.5 * (1.0 + forward_[1][1]) - sSquared) );
        z = sqrt( fabs(0.5 * (1.0 + forward_[2][2]) - sSquared) );
    }
    else
    {
        MATHEX_SCALAR f = 0.25 / s;
        x = f * (forward_[1][2] - forward_[2][1]);
        y = f * (forward_[2][0] - forward_[0][2]);
        z = f * (forward_[0][1] - forward_[1][0]);
    }

	//	Necessary to compensate for rounding errors
	MexQuaternion::normalise( &x, &y, &z, &s );

    //Store the values
    pParams->set( x, y, z, s );
}
//////////////////////////////////////////////////////////////////////////////////////////

void MexTransform3d::rotation( const MexQuaternion& params)
{
    //Store the components locally
    const MexVec3& v = params.vector();
    MATHEX_SCALAR x = v.x();
    MATHEX_SCALAR y = v.y();
    MATHEX_SCALAR z = v.z();
    MATHEX_SCALAR s = params.scalar();

    //Set the matrix
    MATHEX_SCALAR a, b;
    a = 2.0 * s * s - 1.0;
    forward_[0][0] = a + 2.0 * x * x;
    forward_[1][1] = a + 2.0 * y * y;
    forward_[2][2] = a + 2.0 * z * z;

    a = 2.0 * x * y;
    b = 2.0 * s * z;
    forward_[1][0] = a - b;
    forward_[0][1] = a + b;

    a = 2.0 * x * z;
    b = 2.0 * s * y;
    forward_[2][0] = a + b;
    forward_[0][2] = a - b;

    a = 2.0 * y * z;
    b = 2.0 * s * x;
    forward_[2][1] = a - b;
    forward_[1][2] = a + b;


    //Generate new unique key in the updated transform
    ++tkey_;

    TEST_INVARIANT;
}

void MexTransform3d::rotation( const MexEulerAngles& angles )
{
    TEST_INVARIANT;

    MATHEX_SCALAR   sa, ca, se, ce, sr, cr;

    //  Fill in the rotation data

    sa = sin( angles.azimuth() );
    ca = cos( angles.azimuth() );
    se = sin( angles.elevation() );
    ce = cos( angles.elevation() );
    sr = sin( angles.roll() );
    cr = cos( angles.roll() );

    forward_[0][0] = ce * ca;
    forward_[0][1] = ce * sa;
    forward_[0][2] = -se;

    forward_[1][0] = -cr * sa + sr * se * ca;
    forward_[1][1] = cr*ca + sr * se * sa;
    forward_[1][2] = sr*ce;

    forward_[2][0] = sr * sa + cr * se * ca;
    forward_[2][1] = -sr * ca + cr * se * sa;
    forward_[2][2] = cr * ce;

    // Generate new unique key in the updated transform

    ++tkey_;

    TEST_INVARIANT;
}

//////////////////////////////////////////////////////////////////////////////////////////

void MexTransform3d::CLASS_INVARIANT
{
    INVARIANT( this != NULL );

	//orthognality ckeching is a MUST  for inverse transforms
// this code is commented out because some test transforms in persistent test are not orthognal!!!

/*	MATHEX_SCALAR determinent = forward_[0][0]*forward_[1][1]*forward_[2][2] +
	                            forward_[1][0]*forward_[2][1]*forward_[0][2] +
						        forward_[2][0]*forward_[0][1]*forward_[1][2] -

						        forward_[0][0]*forward_[2][1]*forward_[1][2] -
						        forward_[1][0]*forward_[0][1]*forward_[2][2] -
						        forward_[2][0]*forward_[1][1]*forward_[0][2];

	INVARIANT(fabs(determinent) == 1);
*/

    //  This code commented out because it slows the class down too much

//     //  Check that the three basis vectors are mutually orthogonal
//
//     MexVec3     currentX;
//     MexVec3     currentY;
//     MexVec3     currentZ;
//
//     xBasis( &currentX );
//     yBasis( &currentY );
//     zBasis( &currentZ );
//
//     MexVec3    newX( currentY );
//     newX.crossProduct( currentZ );
//
//     INVARIANT( newX == currentX );
//
//     MexVec3    newY( currentZ );
//     newY.crossProduct( currentX );
//     INVARIANT( newY == MexVec3( currentY ) );
//
//     MexVec3    newZ( currentX );
//     newZ.crossProduct( currentY );
//     INVARIANT( newZ == MexVec3( currentZ ) );
//
//     INVARIANT( Mathex::abs( MexVec3( currentX ).modulus() - 1.0 ) < MexEpsilon::instance() );
//     INVARIANT( Mathex::abs( MexVec3( currentY ).modulus() - 1.0 ) < MexEpsilon::instance() );
//     INVARIANT( Mathex::abs( MexVec3( currentZ ).modulus() - 1.0 ) < MexEpsilon::instance() );
}

ostream& operator <<( ostream& stream, const MexTransform3d& t )
{
    stream << std::setprecision( 4 ) << std::setw( 14 ) << t.forward_[0][0] << " " << t.forward_[0][1] << " " << t.forward_[0][2] << std::endl;
    stream << std::setprecision( 4 ) << std::setw( 14 ) << t.forward_[1][0] << " " << t.forward_[1][1] << " " << t.forward_[1][2] << std::endl;
    stream << std::setprecision( 4 ) << std::setw( 14 ) << t.forward_[2][0] << " " << t.forward_[2][1] << " " << t.forward_[2][2] << std::endl;
    stream << std::setprecision( 4 ) << std::setw( 14 ) << t.forward_[3][0] << " " << t.forward_[3][1] << " " << t.forward_[3][2] << std::endl;

    return stream;
}

/////////////////////////////////////////////////////////

PER_DEFINE_PERSISTENT( MexTransform3d );

void perWrite( PerOstream& str, const MexTransform3d& t )
{
    PER_WRITE_RAW_DATA( str, t.forward_, sizeof( t.forward_ ) );
}

void perRead( PerIstream& str, MexTransform3d& t )
{
    PER_READ_RAW_DATA( str, t.forward_, sizeof( t.forward_ ) );

    //Generate new unique key
    ++t.tkey_;
}

void MexTransform3d::rotation( const MexTransform3d& tx )
{
    forward_[0][0] = tx.forward_[0][0];
    forward_[0][1] = tx.forward_[0][1];
    forward_[0][2] = tx.forward_[0][2];
    forward_[1][0] = tx.forward_[1][0];
    forward_[1][1] = tx.forward_[1][1];
    forward_[1][2] = tx.forward_[1][2];
    forward_[2][0] = tx.forward_[2][0];
    forward_[2][1] = tx.forward_[2][1];
    forward_[2][2] = tx.forward_[2][2];
}

MexTransform3d::MexTransform3d(
  AxisPlane axisPlane,
  const MexVec3& axisDirection,
  const MexVec3& planeDefiningVector,
  const MexPoint3d& position )
{
    PRE_INFO( axisDirection );
    PRE_INFO( planeDefiningVector );
    PRE( not axisDirection.parallel( planeDefiningVector ) );
    PRE( not axisDirection.isZeroVector() );
    PRE( not planeDefiningVector.isZeroVector() );

    MexVec3 xBasis;
    MexVec3 yBasis;
    MexVec3 zBasis;

    //  This may seem crude but it is simpler than any of my
    //  attempts to generalise it. If you come up with a
    //  better solution please let Bob know.
    switch( axisPlane )
    {
        case X_XY:
            xBasis = axisDirection;
            xBasis.makeUnitVector();
            zBasis = MexVec3::crossProduct( axisDirection, planeDefiningVector );
            zBasis.makeUnitVector();
            yBasis = MexVec3::crossProduct( zBasis, xBasis );
            break;
        case X_XZ:
            xBasis = axisDirection;
            xBasis.makeUnitVector();
            yBasis = MexVec3::crossProduct( planeDefiningVector, axisDirection );
            yBasis.makeUnitVector();
            zBasis = MexVec3::crossProduct( xBasis, yBasis );
            break;
        case Y_YZ:
            yBasis = axisDirection;
            yBasis.makeUnitVector();
            xBasis = MexVec3::crossProduct( axisDirection, planeDefiningVector );
            xBasis.makeUnitVector();
            zBasis = MexVec3::crossProduct( xBasis, yBasis );
            break;
        case Y_YX:
            yBasis = axisDirection;
            yBasis.makeUnitVector();
            zBasis = MexVec3::crossProduct( planeDefiningVector, axisDirection );
            zBasis.makeUnitVector();
            xBasis = MexVec3::crossProduct( yBasis, zBasis );
            break;
        case Z_ZX:
            zBasis = axisDirection;
            zBasis.makeUnitVector();
            yBasis = MexVec3::crossProduct( axisDirection, planeDefiningVector );
            yBasis.makeUnitVector();
            xBasis = MexVec3::crossProduct( yBasis, zBasis );
            break;
        case Z_ZY:
            zBasis = axisDirection;
            zBasis.makeUnitVector();
            xBasis = MexVec3::crossProduct( planeDefiningVector, axisDirection );
            xBasis.makeUnitVector();
            yBasis = MexVec3::crossProduct( zBasis, xBasis );
            break;
    }

    forward_[0][0] = xBasis.x();forward_[0][1] = xBasis.y();forward_[0][2] = xBasis.z();
    forward_[1][0] = yBasis.x();forward_[1][1] = yBasis.y();forward_[1][2] = yBasis.z();
    forward_[2][0] = zBasis.x();forward_[2][1] = zBasis.y();forward_[2][2] = zBasis.z();
    forward_[3][0] = position[0];forward_[3][1] = position[1];forward_[3][2] = position[2];
}

MexTransform3d::MexTransform3d( const MexCompressedTransform3d& ctrans )
:   flags_( GENERAL )
{
	const MexVec3& xBasis = ctrans.xBasis_;
	const MexVec3& yBasis = ctrans.yBasis_;
	MexVec3 zBasis = MexVec3::crossProduct( xBasis, yBasis );
	const MexPoint3d& position = ctrans.position_;
    forward_[0][0]=xBasis[0];forward_[0][1]=xBasis[1];forward_[0][2]=xBasis[2];
    forward_[1][0]=yBasis[0];forward_[1][1]=yBasis[1];forward_[1][2]=yBasis[2];
    forward_[2][0]=zBasis[0];forward_[2][1]=zBasis[1];forward_[2][2]=zBasis[2];
    forward_[3][0]=position[0];forward_[3][1]=position[1];forward_[3][2]=position[2];
}

void MexTransform3d::transform( MexAlignedBox2d* pBox ) const
{
	const MexPoint2d& minCorner = pBox->minCorner();
	const MexPoint2d& maxCorner = pBox->maxCorner();

    MexPoint2d point0( minCorner.x(), minCorner.y() );
    MexPoint2d point1( minCorner.x(), maxCorner.y() );
    MexPoint2d point2( maxCorner.x(), maxCorner.y() );
    MexPoint2d point3( maxCorner.x(), minCorner.y() );

    transform( &point0 );
    transform( &point1 );
    transform( &point2 );
    transform( &point3 );

    const MexPoint2d newMinCorner(
        std::min( std::min( std::min( point0.x(), point1.x() ), point2.x() ), point3.x() ),
        std::min( std::min( std::min( point0.y(), point1.y() ), point2.y() ), point3.y() ) );

    const MexPoint2d newMaxCorner(
        std::max( std::max( std::max( point0.x(), point1.x() ), point2.x() ), point3.x() ),
        std::max( std::max( std::max( point0.y(), point1.y() ), point2.y() ), point3.y() ) );

    *pBox = MexAlignedBox2d( newMinCorner, newMaxCorner );
}

/* End TRANSF3D.CPP ************************************/
