/*
 * F I X P T . H P P 
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

/*
	Simple, non-class, fixed-point types used internally.
*/

#ifndef _RENDER_FIXPT_HPP
#define _RENDER_FIXPT_HPP

#include "base/base.hpp"

class RenColour;

typedef unsigned char	RenIFixPtU0_8;
typedef   signed char	RenIFixPtS0_7;
typedef int				RenIFixPtS23_8;
typedef int				RenIFixPtS15_16;

struct RenIColourFixPtS15_16
{
	RenIColourFixPtS15_16();
	RenIColourFixPtS15_16(const RenColour&);
	RenIFixPtS15_16 r_, g_, b_;
};

struct RenIColourFixPtS23_8
{
	RenIColourFixPtS23_8();
	RenIColourFixPtS23_8(const RenColour&);
	RenIFixPtS23_8 r_, g_, b_;
};

#endif

/* End FIXPT.HPP ****************************************************/
