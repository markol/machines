/*
 * E N V I R N M T . C P P
 * (c) Charybdis Limited, 1998. All Rights Reserved
 */

#include "world4d/envirnmt.hpp"

W4dEnvironment::W4dEnvironment()

	:lowerTransitionBound_(0.25),
	upperTransitionBound_(0.35)
{
    TEST_INVARIANT;
}

void W4dEnvironment::darknessRange(MATHEX_SCALAR lower, MATHEX_SCALAR upper)
{
	PRE(lower <= upper);
	PRE(inIntervalOO(lower, 0.0, 1.0));
	PRE(inIntervalOO(upper, 0.0, 1.0));

	lowerTransitionBound_ = lower;
	upperTransitionBound_ = upper;
}

MATHEX_SCALAR W4dEnvironment::lowerTransitionBound() const
{
	return lowerTransitionBound_;
}

MATHEX_SCALAR W4dEnvironment::upperTransitionBound() const
{
	return upperTransitionBound_;
}

MATHEX_SCALAR W4dEnvironment::darkness() const
{
	MATHEX_SCALAR result;
	MATHEX_SCALAR illumination = averageIllumination();

	if(illumination >= upperTransitionBound())
		result = 0.0;
	else if(illumination <= lowerTransitionBound())
		result = 1.0;
	else
	{
		result = illumination - lowerTransitionBound();

		MATHEX_SCALAR transitionRange = upperTransitionBound() - lowerTransitionBound();

		if(transitionRange != 0.0)
		{
			result *= 1 / transitionRange;
			result = 1 - result;
		}
		else
			result = 0.5;
	}

	return result;
}

W4dEnvironment::PhaseOfDay W4dEnvironment::phaseOfDay() const
{
	MATHEX_SCALAR darknessValue = darkness();

	PhaseOfDay result = TRANSITION;

	if(darknessValue >= 1.0)
		result = NIGHT;
	else if(darknessValue <= 0.0)
		result = DAY;

	return result;
}


W4dEnvironment::~W4dEnvironment()
{
    TEST_INVARIANT;

}

void W4dEnvironment::CLASS_INVARIANT
{
    INVARIANT( this != NULL );
	INVARIANT(lowerTransitionBound_ <= upperTransitionBound_);
}

ostream& operator <<( ostream& o, const W4dEnvironment& t )
{

    o << "W4dEnvironment " << (void*)&t << " start" << std::endl;
    o << "W4dEnvironment " << (void*)&t << " end" << std::endl;

    return o;
}

/* End ENVIRNMT.CPP *************************************************/
