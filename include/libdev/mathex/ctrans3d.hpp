/*
 * C T R A N S 3 D . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
    MexCompressedTransform3d

    A brief description of the class should go in here
*/

#ifndef _MATHEX_CTRANS3D_HPP
#define _MATHEX_CTRANS3D_HPP

#include "base/base.hpp"
#include "mathex/cpoint3d.hpp"
#include "mathex/cvec3.hpp"

class MexTransform3d;

#pragma pack(push,1)
class MexCompressedTransform3d
// Canonical form revoked
{
public:
    MexCompressedTransform3d( const MexTransform3d& );
    MexCompressedTransform3d();
    ~MexCompressedTransform3d( void );

    void CLASS_INVARIANT;

private:
    friend ostream& operator <<( ostream& o, const MexCompressedTransform3d& t );
	friend class MexTransform3d;

	MexCompressedPoint3d	position_;
	MexCompressedVec3		xBasis_;
	MexCompressedVec3		yBasis_;

};
#pragma pack(pop)

#endif

/* End CTRANS3D.HPP *************************************************/
