/*
 * E L E V C L U T . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _ENVIRNMT_ELEVCLUT_HPP
#define _ENVIRNMT_ELEVCLUT_HPP

#include "base/base.hpp"
#include "render/colour.hpp"
#include "mathex/angle.hpp"
#include "ctl/mmvector.hpp"
#include "stdlib/string.hpp"

class SysPathName;

// A table which maps from elevation to colour.  In general, all elevation
// parameters must be in the range -90 <= el <= 90 degrees.  All methods
// have pre-conditions to this effect.
class EnvElevationColourTable
{
public:
    EnvElevationColourTable();

	// Elevations outside the min/max range are valid, the nearest stored
	// value will be returned.
	RenColour colour(MexDegrees el) const;

	// Add a colour entry for the given elevation.  The entry closest to the
	// given elevation will be set.
	void addEntry(MexDegrees el, const RenColour&);

	// Initialise from a bitmap.  Only the first vertical column is used. The
	// texels represent equally spaced entries between the given elevations.
	void readFromBitmap(const SysPathName& bm, MexDegrees minEl, MexDegrees maxEl);

	MexDegrees minElevation() const;
	MexDegrees maxElevation() const;

	void name(const char*);
	void name(const string&);
	const string& name() const;

	int nEntries() const;

    void CLASS_INVARIANT;
    friend ostream& operator <<( ostream& o, const EnvElevationColourTable& t );

	struct Entry
	{
		MexDegrees	elevation;
		RenColour	colour;

		bool operator==(const Entry& e) const { return elevation == e.elevation; }
		bool operator< (const Entry& e) const { return elevation <  e.elevation; }
	};

private:
	typedef ctl_min_memory_vector<Entry> ColourVec;

	ColourVec		colours_;
	MexDegrees 		min_, max_;
	string			name_;

    EnvElevationColourTable( const EnvElevationColourTable& );
    EnvElevationColourTable& operator =( const EnvElevationColourTable& );
    bool operator ==( const EnvElevationColourTable& );
};

#endif

/* End ELEVCLUT.HPP *************************************************/
