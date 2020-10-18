/*
 * F I X V E C . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "render/internal/fixptvec.hpp"
#include "mathex/vec3.hpp"

///////////////////////////////////////////////////////////////////////////////
PER_DEFINE_PERSISTENT( RenIVec3FixPtS0_7 );

RenIVec3FixPtS0_7::RenIVec3FixPtS0_7(MATHEX_SCALAR x, MATHEX_SCALAR y, MATHEX_SCALAR z)
{
	PRE_DATA(MexVec3 mexVec(x,y,z));
	PRE_INFO(mexVec);
	PRE(mexVec.modulus() <= 1.0001);

	vec_[0] = _STATIC_CAST(RenIFixPtS0_7, x * 127 + 0.5);
	vec_[1] = _STATIC_CAST(RenIFixPtS0_7, y * 127 + 0.5);
	vec_[2] = _STATIC_CAST(RenIFixPtS0_7, z * 127 + 0.5);
}

// Note this isn't strictly a S1.7 format because 1.0 is represented as 127,
// *not* 128.  However, it probably close enough for our inaccuarte purposes.
void RenIVec3FixPtS0_7::setVector(const MexVec3& vec)
{
	PRE_INFO(vec);
	PRE(vec.modulus() <= 1.0001);
	vec_[0] = _STATIC_CAST(RenIFixPtS0_7, vec.x() * 127 + 0.5);
	vec_[1] = _STATIC_CAST(RenIFixPtS0_7, vec.y() * 127 + 0.5);
	vec_[2] = _STATIC_CAST(RenIFixPtS0_7, vec.z() * 127 + 0.5);
}

void RenIVec3FixPtS0_7::convertToMex(MexVec3* v)
{
	PRE(v);
	v->x(vec_[0] / 127.0);
	v->y(vec_[1] / 127.0);
	v->z(vec_[2] / 127.0);
}

ostream& operator<<(ostream& o, const RenIVec3FixPtS0_7& v)
{
	o << "fixed-pt S0.7 vector (" << (int) v.x() << "," << (int) v.y() << "," << (int) v.z() << ")";
	return o;
}

void perWrite( PerOstream& ostr, const RenIVec3FixPtS0_7& t )
{
    PER_WRITE_RAW_DATA( ostr, t.vec_, sizeof( t.vec_ ) );
}

void perRead( PerIstream& istr, RenIVec3FixPtS0_7& t )
{
    PER_READ_RAW_DATA( istr, t.vec_, sizeof( t.vec_ ) );
}

/* End FIXVEC.CPP ***************************************************/
