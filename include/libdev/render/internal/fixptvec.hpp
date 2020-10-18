/*
 * F I X V E C . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
	A fixed-point S0.7 vector type which is used for storing vertex normals.
*/

#ifndef _RENDER_FIXVEC_HPP
#define _RENDER_FIXVEC_HPP

#include "base/base.hpp"
#include "base/persist.hpp"
#include "render/internal/fixpt.hpp"
#include "mathex/mathex.hpp"
#include <cstring>

class MexVec3;

class RenIVec3FixPtS0_7
{
public:
	// PRE(modulus of (x,y,z) <= 1);
	RenIVec3FixPtS0_7(MATHEX_SCALAR x, MATHEX_SCALAR y, MATHEX_SCALAR z);

	RenIVec3FixPtS0_7()						{ vec_[0] = vec_[1] = vec_[2] = 0; }
	RenIVec3FixPtS0_7(const MexVec3& vec)	{ setVector(vec); }
	RenIFixPtS0_7 x() const					{ return vec_[0]; }
	RenIFixPtS0_7 y() const					{ return vec_[1]; }
	RenIFixPtS0_7 z() const					{ return vec_[2]; }

	// This only works for unit vectors.  Otherwise, the range of
	// the fixed-point couldn't be guaranteed to represent it.
	// PRE(vec.modulus() <= 1.0);
	void setVector(const MexVec3& vec);

	// No implicit conversion to Mathex type.
	void convertToMex(MexVec3* v);		// PRE(v);

	// By clamping to zero, we avoid the need to adjust the point of a signed
	// value.  (A shift of a signed value is implementation dependent).  By a
	// nice co-incidence, this is exactly the result required for lighting.
	RenIFixPtU0_8 dotProductClampZero(const RenIVec3FixPtS0_7& v) const
	{
		// result is in S2.14.  Note the compiler does integral promotion for the arithmetic.
		short result = _STATIC_CAST(short, vec_[0]*v.vec_[0] + vec_[1]*v.vec_[1] + vec_[2]*v.vec_[2]);

		if (result < 0)
			result = 0;

		ushort usResult = _STATIC_CAST(ushort, result);		// now U2.14

		return _STATIC_CAST(RenIFixPtU0_8, usResult >> 6);	// convert to U0.8
	}

	bool operator==(const RenIVec3FixPtS0_7& v) const	{ return 0 == memcmp(vec_, v.vec_, 3); }

    PER_MEMBER_PERSISTENT_DEFAULT( RenIVec3FixPtS0_7 );
    PER_FRIEND_READ_WRITE( RenIVec3FixPtS0_7 );

private:
	RenIFixPtS0_7	vec_[3];
};

PER_DECLARE_PERSISTENT( RenIVec3FixPtS0_7 );

ostream& operator<<(ostream& o, const RenIVec3FixPtS0_7& v);

#endif

/* End FIXVEC.HPP ***************************************************/
