/*
 * C O L P A C K . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
	Low-level functions which replace the D3DRGBA macro.
	TBD: could these be rewritten more efficiently in assembly?
*/

#ifndef _RENDER_COLPACK_HPP
#define _RENDER_COLPACK_HPP

#include "base/base.hpp"

// The lighting calculations can easily overflow the [0,1] range when
// emissive effects add to the usual lambertian diffuse reflection and
// ambient.  Simply clamping each component separately can, of course,
// produce a hue shift.  To avoid this, we would need to scale all the
// components by the same factor to bring the maximum one down to 1.0.
#define RENI_PACK_A(a)	(((long)(a * 0xff000000L)) & 0xff000000L)
#define RENI_PACK_R(r)	(((long)(r * 0x00ff0000L)) & 0x00ff0000L)
#define RENI_PACK_G(g)	(((long)(g * 0x0000ff00L)) & 0x0000ff00L)
#define RENI_PACK_B(b)	 ((long)(b * 0x000000ffL))

inline uint packColourUnChecked(float r, float g, float b, ulong a=0xff000000L)
{
	return a | RENI_PACK_R(r) | RENI_PACK_G(g) | RENI_PACK_B(b);
}

inline uint packColourChecked(float r, float g, float b, ulong a=0xff000000L)
{
	// A minor optimisation: if red, green or blue exceed 1, then skip the packing
	// arithmetic.  The alternative checks the floating point values against a
	// ceiling of 1.0 and then uses the D3DRGB macro.  It is faster done this way.
	// NB: comparing against 1 is faster than any other value.  The compiler
	// generates less machine code for 1!
	return a |
		   ((r>=1)? 0xff0000L: RENI_PACK_R(r)) |
		   ((g>=1)? 0x00ff00L: RENI_PACK_G(g)) |
		   ((b>=1)? 0x0000ffL: RENI_PACK_B(b));
}

inline uint packColour(float r, float g, float b, float a)
{
	return ((a>=1)? 0xff000000L: RENI_PACK_A(a)) |
		   ((r>=1)? 0xff0000L:   RENI_PACK_R(r)) |
		   ((g>=1)? 0x00ff00L:   RENI_PACK_G(g)) |
		   ((b>=1)? 0x0000ffL:   RENI_PACK_B(b));
}

#endif

/* End COLPACK.HPP **************************************************/
