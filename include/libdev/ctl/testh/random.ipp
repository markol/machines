/*
 * R A N D O M . I P P
 * (c) Charybdis Limited, 1996. All Rights Reserved
 */

#ifdef _INLINE
	#define _CODE_INLINE	inline
#else
	#define _CODE_INLINE
#endif

////////////////////////////////

_CODE_INLINE
unsigned int randomInt( unsigned int n )
{
	return rand() % n;
}

_CODE_INLINE
int randomInt( int minInt, int maxInt )
{
	return ( rand() % ( maxInt - minInt ) ) + minInt;
}

_CODE_INLINE
unsigned int randomInt()
{
	return rand();
}

_CODE_INLINE
double randomFloat( double d )
{
	double	randDouble = rand() / (double) RAND_MAX;

	double	result = randDouble * d;

	return result;
}

_CODE_INLINE
double randomFloat()
{
	// get a value between 0 and 1
	double	randDouble = rand() / (double) RAND_MAX;

	double	result = randDouble * rand();
								  
	return result;
}


_CODE_INLINE
double randomFloat( double minValue, double maxValue )
{
	// value between 0 and 1
	double	randDouble = rand() / (double) RAND_MAX;

	double	result = randDouble * ( maxValue - minValue ) + minValue;

	return result;
}

///////////////////////////////////
