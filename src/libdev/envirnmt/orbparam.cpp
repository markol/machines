/*
 * O R B P A R A M . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "envirnmt/internal/orbparam.hpp"
#include "envirnmt/orbit.hpp"

EnvIOrbitParams::EnvIOrbitParams(const string* name):
	name_(*name),
	period_(-1),
	radius_(100000),
	start_(0),
	max_(10),
	min_(-2),
	heading_(0)
{
	PRE(name);
    TEST_INVARIANT;
}

EnvIOrbitParams::~EnvIOrbitParams()
{
    TEST_INVARIANT;
}

EnvOrbit* EnvIOrbitParams::createOrbit(W4dEntity* parent)
{
	return _NEW(EnvOrbit(name_, min_, max_, radius_, heading_, period_, start_, parent));
}

void EnvIOrbitParams::radius(MATHEX_SCALAR s)
{
	radius_ = s;
	radiusSet_ = true;
}

void EnvIOrbitParams::period(MATHEX_SCALAR s)
{
	period_ = s;
	periodSet_ = true;
}

void EnvIOrbitParams::startTime(MATHEX_SCALAR s)
{
	start_ = s;
	startSet_ = true;
}

void EnvIOrbitParams::minElevation(MexDegrees d)
{
	min_ = d;
	minSet_ = true;
}

void EnvIOrbitParams::maxElevation(MexDegrees d)
{
	max_ = d;
	maxSet_ = true;
}

void EnvIOrbitParams::heading(MexDegrees d)
{
	heading_ = d;
	headingSet_ = true;
}

void EnvIOrbitParams::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
}

/* End ORBPARAM.CPP *************************************************/
