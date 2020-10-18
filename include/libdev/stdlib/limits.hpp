/*
 * L I M I T S
 * Copyright (c) Charybdis Limited, 1996. All Rights Reserved.
 */

#ifndef _STD_LIMITS
#define _STD_LIMITS

extern "C"
{
    #include <float.h>
}

//////////////////////////////////////////////////////////////////////

enum float_round_style
{
	round_indeterminate 		= -1,
	round_toward_zero			=  0,
	round_to_nearest			=  1,
	round_toward_infinity		=  2,
	round_toward_neg_infinity	=  3
};

//////////////////////////////////////////////////////////////////////

template < class T >
class numeric_limits
{
public:

	static const bool is_specialised;

	static T min();
	static T max();

	static const int digits;
	static const int digits10;

	static const bool is_signed;
	static const bool is_integer;
	static const bool is_exact;

	static const int radix;

	static T epsilon();
	static T round_error();

	static const int min_exponent;
	static const int min_exponent10;
	static const int max_exponent;
	static const int max_exponent10;

	static const bool has_infinity;
	static const bool has_quiet_NaN;
	static const bool has_signalling_NaN;
	static const bool has_denorm;

	static T infinity();
	static T quiet_NaN();
	static T signalling_NaN();
	static T denorm_min();

	static const bool is_iec559;
	// same as iee754

	static const bool is_bounded;
	static const bool is_modulo;

	static const bool traps;
	static const bool tinyness_before;
	static const float_round_style round_type;
};

//////////////////////////////////////////////////////////////////////

class numeric_limits< float >
{
public:

	static const bool is_specialised;

	static float min();
	static float max();

	static const int digits;
	static const int digits10;

	static const bool is_signed;
	static const bool is_integer;
	static const bool is_exact;

	static const int radix;

	static float epsilon();
	static float round_error();
	// returns 0.5 at present

	static const int min_exponent;
	static const int min_exponent10;
	static const int max_exponent;
	static const int max_exponent10;

	static const bool has_infinity;
	static const bool has_quiet_NaN;
	static const bool has_signalling_NaN;
	static const bool has_denorm;

	static float infinity();
	// returns max() at present

	static float quiet_NaN();
	// returns 0.0 at present

	static float signalling_NaN();
	// returns 0.0 at present

	static float denorm_min();

	static const bool is_iec559;

	static const bool is_bounded;
	static const bool is_modulo;

	static const bool traps;
	static const bool tinyness_before;
	static const float_round_style round_type;
};

//////////////////////////////////////////////////////////////////////

const bool numeric_limits< float >::is_specialised = true;

const int numeric_limits< float >::digits = FLT_MANT_DIG;
const int numeric_limits< float >::digits10 = FLT_DIG;

const bool numeric_limits< float >::is_signed = true;
const bool numeric_limits< float >::is_integer = false;
const bool numeric_limits< float >::is_exact = false;
const int numeric_limits< float >::radix = FLT_RADIX;

const int numeric_limits< float >::min_exponent = FLT_MIN_EXP;
const int numeric_limits< float >::min_exponent10 = FLT_MIN_10_EXP;
const int numeric_limits< float >::max_exponent = FLT_MAX_EXP;
const int numeric_limits< float >::max_exponent10 FLT_MAX_10_EXP;

const bool numeric_limits< float >::has_infinity = true;
const bool numeric_limits< float >::has_quiet_NaN = true;
const bool numeric_limits< float >::has_signalling_NaN = true;
const bool numeric_limits< float >::has_denorm = false;

const bool numeric_limits< float >::is_iec559 = true;
const bool numeric_limits< float >::is_bounded = true;
const bool numeric_limits< float >::is_modulo = false;

const bool numeric_limits< float >::traps= true;
const bool numeric_limits< float >::tinyness_before = true;
const float_round_style numeric_limits< float >::round_type = float_round_style( FLT_ROUNDS );

float numeric_limits< float >::min()
{
	return FLT_MIN;
}

float numeric_limits< float >::max()
{
	return FLT_MAX;
}

float numeric_limits< float >::epsilon()
{
	return FLT_EPSILON;
}

float numeric_limits< float >::round_error()
{
	return 0.5f;
}

float numeric_limits< float >::infinity()
{
	return max();
}

float numeric_limits< float >::quiet_NaN()
{
	return 0;
}

float numeric_limits< float >::signalling_NaN()
{
	return 0;
}

float numeric_limits< float >::denorm_min()
{
	return min();
}

//////////////////////////////////////////////////////////////////////

#endif	/* #ifndef _STD_LIMITS	*/
