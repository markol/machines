/*
 * C P O I N T 2 D . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MexCompressedPoint2d

    For transmitting point2d around network sometimes we want it in a smaller format.
*/

#ifndef _MATHEX_CABOX2D_HPP
#define _MATHEX_CABOX2D_HPP

#include "base/base.hpp"
#include "mathex/cpoint2d.hpp"

class MexAlignedBox2d;

#pragma pack(push,1)
class MexCompressedAlignedBox2d
// Canonical form revoked
{
public:
    MexCompressedAlignedBox2d();
    MexCompressedAlignedBox2d( const MexAlignedBox2d& );
    ~MexCompressedAlignedBox2d();

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MexCompressedAlignedBox2d& t );
	friend class MexAlignedBox2d;

    MexCompressedPoint2d    vmin_;
	MexCompressedPoint2d    vmax_;
    bool                    isEmpty_;
};
#pragma pack(pop)

#endif

/* End CPOINT2d.HPP *************************************************/
