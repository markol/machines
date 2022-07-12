/*
 * C P O I N T 2 D . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MexCompressedPoint2d

    For transmitting point2d around network sometimes we want it in a smaller format.
*/

#ifndef _MATHEX_CPOINT2d_HPP
#define _MATHEX_CPOINT2d_HPP

#include "base/base.hpp"

class MexPoint2d;

#pragma pack(push,1)
class MexCompressedPoint2d
// Canonical form revoked
{
public:
    MexCompressedPoint2d();
    MexCompressedPoint2d( const MexPoint2d& );
    ~MexCompressedPoint2d();

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MexCompressedPoint2d& t );
	friend class MexPoint2d;

    enum: uint8 { X = 0, Y = 1 };

    float	point_[ 2 ];
};
#pragma pack(pop)

#endif

/* End CPOINT2d.HPP *************************************************/
