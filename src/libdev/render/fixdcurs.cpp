/*
 * F I X D C U R S . C P P 
 * (c) Charybdis Limited, 1997. All Rights Reserved
 */

#include "render/fixdcurs.hpp"

RenFixedCursor2d::RenFixedCursor2d(const RenSurface& s):
	surf_(s)
{
}

// virtual
RenFixedCursor2d::~RenFixedCursor2d()
{
}

// virtual 
const RenSurface& RenFixedCursor2d::currentBitmap() const
{
	return surf_;
}

// virtual 
void RenFixedCursor2d::print(ostream& o) const
{
	o << "fixed cursor using " << surf_;
}

/* End FIXDCURS.CPP *************************************************/
