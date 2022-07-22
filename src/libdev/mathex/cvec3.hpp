/*
 * C V E C 3 . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MexCompressedVec3

    When transmitting vectors around the system we can send them in a smaller format.
*/

#ifndef _MATHEX_CVEC3_HPP
#define _MATHEX_CVEC3_HPP

#include "base/base.hpp"

class MexVec3;

#pragma pack(push,1)
class MexCompressedVec3
// Canonical form revoked
{
public:
    MexCompressedVec3();
    MexCompressedVec3( const MexVec3& );
    ~MexCompressedVec3();

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MexCompressedVec3& t );
	friend class MexVec3;

    enum: uint8 { X = 0, Y = 1, Z = 2 };

    float	vec_[ 3 ];
};
#pragma pack(pop)

#endif

/* End CVEC3.HPP ****************************************************/
