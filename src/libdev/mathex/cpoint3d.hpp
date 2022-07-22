/*
 * C P O I N T 3 D . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MexCompressedPoint3d

    For transmitting point3d around network sometimes we want it in a smaller format.
*/

#ifndef _MATHEX_CPOINT3D_HPP
#define _MATHEX_CPOINT3D_HPP

#include "base/base.hpp"

class MexPoint3d;

#pragma pack(push,1)
class MexCompressedPoint3d
// Canonical form revoked
{
public:
    MexCompressedPoint3d();
    MexCompressedPoint3d( const MexPoint3d& );
    ~MexCompressedPoint3d();

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MexCompressedPoint3d& t );
	friend class MexPoint3d;

    enum: uint8 { X = 0, Y = 1, Z = 2 };

    float	point_[ 3 ];
};
#pragma pack(pop)


#endif

/* End CPOINT3D.HPP *************************************************/
