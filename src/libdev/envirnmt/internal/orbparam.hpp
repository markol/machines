/*
 * O R B P A R A M . H P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#ifndef _ENVIRNMT_ORBPARAM_HPP
#define _ENVIRNMT_ORBPARAM_HPP

#include "base/base.hpp"
#include "mathex/angle.hpp"
#include "stdlib/string.hpp"

class EnvOrbit;
class W4dEntity;

// During parsing of an orbit, this holds all the attributes which are later
// used to construct the EnvOrbit.  We could build an EnvOrbit and then set
// the attributes as they are encountered in the file, but most EnvOrbit
// members are const.  By using this placeholder, we avoid having to make the
// members non-const just for parsing.
class EnvIOrbitParams
{
public:
    EnvIOrbitParams(const string* name);	// PRE(name);
    ~EnvIOrbitParams();

	const string& name() const	{ return name_; }

	void radius(MATHEX_SCALAR);
	void period(MATHEX_SCALAR);
	void startTime(MATHEX_SCALAR);
	void minElevation(MexDegrees);
	void maxElevation(MexDegrees);
	void heading(MexDegrees);

	EnvOrbit* createOrbit(W4dEntity* parent);

    void CLASS_INVARIANT;

private:
	const string	name_;
	MATHEX_SCALAR	radius_, period_, start_;
	MexDegrees		min_, max_, heading_;
	bool			radiusSet_, periodSet_, startSet_, minSet_, maxSet_, headingSet_;

    EnvIOrbitParams( const EnvIOrbitParams& );
    EnvIOrbitParams& operator =( const EnvIOrbitParams& );
    bool operator ==( const EnvIOrbitParams& );
};

#endif

/* End ORBPARAM.HPP *************************************************/
