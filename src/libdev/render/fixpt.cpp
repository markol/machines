/*
 * F I X P T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "render/internal/fixpt.hpp"
#include "render/colour.hpp"

RenIColourFixPtS15_16::RenIColourFixPtS15_16():
	r_(0),
	g_(0),
	b_(0)
{
}

RenIColourFixPtS15_16::RenIColourFixPtS15_16(const RenColour& col):
	r_(_STATIC_CAST(RenIFixPtS15_16, col.r() * 65536 + 0.5)),
	g_(_STATIC_CAST(RenIFixPtS15_16, col.g() * 65536 + 0.5)),
	b_(_STATIC_CAST(RenIFixPtS15_16, col.b() * 65536 + 0.5))
{
}

RenIColourFixPtS23_8::RenIColourFixPtS23_8():
	r_(0),
	g_(0),
	b_(0)
{
}

RenIColourFixPtS23_8::RenIColourFixPtS23_8(const RenColour& col):
	r_(_STATIC_CAST(RenIFixPtS23_8, col.r() * 256 + 0.5)),
	g_(_STATIC_CAST(RenIFixPtS23_8, col.g() * 256 + 0.5)),
	b_(_STATIC_CAST(RenIFixPtS23_8, col.b() * 256 + 0.5))
{
}

/* End FIXPT.CPP ****************************************************/
