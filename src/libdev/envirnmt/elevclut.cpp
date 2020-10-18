/*
 * E L E V C L U T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "envirnmt/elevclut.hpp"
#include "system/pathname.hpp"
#include <algorithm>

EnvElevationColourTable::EnvElevationColourTable():
	min_(MexDegrees(90)),
	max_(MexDegrees(-90))
{
	colours_.reserve(10);
	TEST_INVARIANT;
}

static void linearInterpolate
(
	EnvElevationColourTable::Entry e1,
	EnvElevationColourTable::Entry e2,
	RenColour* result,
	MexDegrees el
)
{
	PRE(e1.elevation < el);
	PRE(e2.elevation > el);

	const RenColour& c1 = e1.colour;
	const RenColour& c2 = e2.colour;
	const MATHEX_SCALAR ang1 = e1.elevation.asScalar();
	const MATHEX_SCALAR ang2 = e2.elevation.asScalar();

	result->linearInterpolate(el.asScalar(), c1, ang1, c2, ang2);
}

RenColour EnvElevationColourTable::colour(MexDegrees el) const
{
	if (colours_.size() == 0)
		return RenColour::black();
	else if (colours_.size() == 1)
		return colours_.front().colour;
	else
	{
		// Check the given elevation against the min and max values, this may
		// obviate the binary search in many cases.
		if (el <= min_)
			return colours_.front().colour;
		else if (el >= max_)
			return colours_.back().colour;
		else
		{
			Entry target;
			target.elevation = el;

			// The given elevation is between min and max.  An interpolation will
			// be necessary unless we hit an entry nail-on-the-head.
			ColourVec::const_iterator lower = lower_bound(colours_.begin(), colours_.end(), target);

			// This should not happen because el is between min_ and max_.
			ASSERT(lower > colours_.begin(), "Elevation binary search error.");
			ASSERT(lower < colours_.end(),   "Elevation binary search error.");

			const Entry& entry = *lower;

			// If we've hit an exact match, or the end of the vector, then
			// we can return the colour without interpolation.
			if (entry.elevation == el)
				return entry.colour;
			else
			{
				RenColour interp;
				linearInterpolate(*(--lower), entry, &interp, el);
				return interp;
			}
		}
	}
}

void EnvElevationColourTable::addEntry(MexDegrees el, const RenColour& col)
{
	PRE(el >= MexDegrees(-90));
	PRE(el <= MexDegrees(90));

	Entry entry;
	entry.elevation = el;
	entry.colour    = col;

	if (el < min_)
		min_ = el;
	if (el > max_)
		max_ = el;

	// There appears to be a Watcom bug.  It can't determine that begin and
	// end() have overloaded non-const versions in release mode.
	#ifdef NDEBUG
	ColourVec::iterator insertAt = lower_bound(colours_.begin(), colours_.end(), entry);
//	ColourVec::const_iterator crufty = lower_bound(colours_.begin(), colours_.end(), entry);
//	ColourVec::iterator insertAt = _CONST_CAST(ColourVec::iterator, crufty);
	#else
	ColourVec::iterator insertAt = lower_bound(colours_.begin(), colours_.end(), entry);
	#endif

	// The insertAt position may contain an entry for the given elevation value.
	// In that case, overwrite the existing colour.  Otherwise add a new entry.
	if (insertAt != colours_.end() && *insertAt == entry)
		(*insertAt).colour = col;
	else
		colours_.insert(insertAt, entry);
}

void EnvElevationColourTable::readFromBitmap(const SysPathName& bm, MexDegrees minEl, MexDegrees maxEl)
{
	PRE(bm.existsAsFile());
	PRE(minEl < maxEl);
}

void EnvElevationColourTable::name(const char* n)
{
	name_ = n;
}

void EnvElevationColourTable::name(const string& n)
{
	name_ = n;
}

const string& EnvElevationColourTable::name() const
{
	return name_;
}

MexDegrees EnvElevationColourTable::minElevation() const
{
	return min_;
}

MexDegrees EnvElevationColourTable::maxElevation() const
{
	return max_;
}

int EnvElevationColourTable::nEntries() const
{
	return colours_.size();
}

void EnvElevationColourTable::CLASS_INVARIANT
{
    INVARIANT(this);
	INVARIANT(minElevation() >= MexDegrees(-90));
	INVARIANT(maxElevation() <= MexDegrees(90));
    INVARIANT(implies(colours_.size() > 1,  minElevation() <  maxElevation()));
    INVARIANT(implies(colours_.size() == 1, minElevation() == maxElevation()));
}

ostream& operator <<( ostream& o, const EnvElevationColourTable& t )
{
    o << "EnvElevationColourTable ";

	if (t.name_.length() > 0)
		o << "\"" << t.name_ << "\"";

	o << ":" << std::endl;

	for (int i=0; i!=t.colours_.size(); ++i)
		o << "  " << t.colours_[i].elevation << "\t" << t.colours_[i].colour << std::endl;

	o << std::endl;

    return o;
}

/* End ELEVCLUT.CPP *************************************************/
