/*
 * T R A N S F 3 D . I P P
 * (c) Charybdis Limited, 1995, 1996. All Rights Reserved
 */

/////////////////////////////////////////////////////////

#ifdef  _INLINE
    #define _CODE_INLINE    inline
#else
    #define _CODE_INLINE
#endif

#include "mathex/eulerang.hpp"
#include "mathex/quatern.hpp"
#include "mathex/point3d.hpp"
#include "mathex/vec3.hpp"

/////////////////////////////////////////////////////////


//---------------------------------------------------------------------
_CODE_INLINE
const MexTransform3dKey& MexTransform3d::key( void ) const
{
	return tkey_;
}
//---------------------------------------------------------------------

_CODE_INLINE
MexVec3 MexTransform3d::xBasis( void ) const
{
    return MexVec3( forward_[0][0], forward_[0][1], forward_[0][2] );
}

_CODE_INLINE
MexVec3 MexTransform3d::yBasis( void ) const
{
    return MexVec3( forward_[1][0], forward_[1][1], forward_[1][2] );
}

_CODE_INLINE
MexVec3 MexTransform3d::zBasis( void ) const
{
    return MexVec3( forward_[2][0], forward_[2][1], forward_[2][2] );
}

_CODE_INLINE
MexPoint3d  MexTransform3d::position( void ) const
{
    return MexPoint3d( forward_[3][0], forward_[3][1], forward_[3][2] );
}

_CODE_INLINE
MexEulerAngles  MexTransform3d::rotationAsEulerAngles( void ) const
{
    MexEulerAngles  angles;
    rotation( &angles );

    return angles;
}

_CODE_INLINE
MexQuaternion  MexTransform3d::rotationAsQuaternion( void ) const
{
    MexQuaternion  quaternion;
    rotation( &quaternion );

    return quaternion;
}

/////////////////////////////////////////////////////////


/* End TRANSF3D.IPP ************************************/