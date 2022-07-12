/*
 * C L I N E 3 D . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MexCompressedLine3d

    Compressed version of line3d - designed for network transmission etc.
*/

#ifndef _MATHEX_CLINE3D_HPP
#define _MATHEX_CLINE3D_HPP

#include "base/base.hpp"
#include "mathex/cpoint3d.hpp"

class MexLine3d;

#pragma pack(push,1)
class MexCompressedLine3d
// Canonical form revoked
{
public:
    MexCompressedLine3d();
    MexCompressedLine3d( const MexLine3d& );
    ~MexCompressedLine3d();

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MexCompressedLine3d& t );
	friend class MexLine3d;

    MexCompressedPoint3d end1_;
    MexCompressedPoint3d end2_;
	//A compressed line does not cache the length - it must be computed when the actual line object is constructed
};
#pragma pack(pop)

#endif

/* End CLINE3D.HPP **************************************************/
