/*
 * R A N D O M . H P P
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */

#ifndef _RANDOM_HPP
#define _RANDOM_HPP

#include<stdlib.h>
////////////////////////////////

inline
const unsigned int& randomInt( const unsigned int& n )
{
	//PRE( isValid( n ) );

	return rand() % n;
}

inline
const int& randomInt( const int& minValue, const int& maxValue )
{
	//PRE( isValid( n ) );

	return ( rand() % ( maxValue - minValue ) ) + minValue;
}

inline
const unsigned int& randomInt()
{
	return rand();
}

inline
double randomFloat( double d )
{
	double	randDouble = rand() / (double) RAND_MAX;

	double	result = randDouble * d;

	return result;
}

inline
double randomFloat()
{
	// get a value between 0 and 1
	double	randDouble = rand() / (double) RAND_MAX;

	double	result = randDouble * rand();
								  
	return result;
}


inline
double randomFloat( double minValue, double maxValue )
{
//	PRE( minValue < maxValue );

	// value between 0 and 1
	double	randDouble = rand() / (double) RAND_MAX;

	double	result = randDouble * ( maxValue - minValue ) + minValue;

	return result;
}

//////////////////////////////

#endif		/*		_RANDOM_HPP			*/