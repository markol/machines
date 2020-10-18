/*
 * O U T C O D E . C P P 
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */


#include "mathex/vec2.hpp"
#include "mathex/abox2d.hpp"
#include "mathex/outcode.hpp"

OutCode::OutCode
(
	const MexAlignedBox2d& box,   // the box to test against
	const MexVec2&         pt	 // the x-y point in question
)
{
	code_ = inside;

	if      (pt.x() < box.minCorner().x())  code_ = code_ | left;
	else if (pt.x() > box.maxCorner().x())  code_ = code_ | right;
	if      (pt.y() < box.minCorner().y())  code_ = code_ | below;
	else if (pt.y() > box.maxCorner().y())  code_ = code_ | above;
	
	// Left and right are mutually exclusive, likewise above/below.
	ASSERT(!(is_left () && is_right()), logic_error());
	ASSERT(!(is_above() && is_below()), logic_error());
}

// Are the given outcodes, vertically or horizontally opposite?
bool OutCode::is_directly_opposite
(
	const OutCode& oc
) const
{
	return (code_==left  && oc.code_==right) ||
		   (code_==right && oc.code_==left ) ||
		   (code_==above && oc.code_==below) ||
		   (code_==below && oc.code_==above);
}

ostream& operator<<(ostream& os, const OutCode& oc)
{
	if (oc.is_inside())
		os << "inside";
	else 
	{
		if (oc.is_below())
			os << "below";
		else if (oc.is_above())
			os << "above";	
		else
			os << "centre";
	
		if (oc.is_left())
			os << " left";
		else if (oc.is_right())
			os << " right";	
		else	
			os << " centre";
	}
	return os;	
}

