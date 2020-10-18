/*
	Useful macros / functions for testing the persistence code
*/

#ifndef _PERSIST_PERSIST_HPP
#define _PERSIST_PERSIST_HPP

#include <stdlib.h>
#include <algorithm>

#define	COMPARE_OBJECTS( a, b )									\
	if( a != b )												\
	{															\
		std::cout << #a << " and " << #b << " not equal" << std::endl;	\
		result = false;											\
	}

#define	COMPARE_POINTERS( pA, pB )                              \
    if( pA == NULL )                                            \
    {                                                           \
        if( pB != NULL )                                        \
        {                                                       \
    		std::cout << #pA << " and " << #pB << " not equal" << std::endl;	\
	    	result = false;										\
        }                                                       \
    }                                                           \
    else if( pB == NULL )                                       \
    {                                                           \
   		std::cout << #pA << " and " << #pB << " not equal" << std::endl;	\
    	result = false;											\
    }                                                           \
    else if( *pA != *pB )                                       \
    {                                                           \
		std::cout << "*" << #pA << " and *" << #pB << " not equal" << std::endl;	\
		result = false;											\
    }

#define	COMPARE_COUNTED_POINTERS( pA, pB )                      \
    if( not pA.isDefined() )                                    \
    {                                                           \
        if( pB.isDefined() )                                    \
        {                                                       \
    		std::cout << #pA << " and " << #pB << " not equal" << std::endl;	\
	    	result = false;										\
        }                                                       \
    }                                                           \
    else if( not pB.isDefined() )                               \
    {                                                           \
   		std::cout << #pA << " and " << #pB << " not equal" << std::endl;	\
    	result = false;											\
    }                                                           \
    else if( *pA != *pB )                                       \
    {                                                           \
		std::cout << "*" << #pA << " and *" << #pB << " not equal" << std::endl;	\
		result = false;											\
    }



inline
unsigned int randomInt( void )
{
	return rand();
}

inline
unsigned int randomInt( unsigned int n )
{
	return rand() % n;
}

inline
float randomFloat( void )
{
	float	randFloat = rand() / (double) RAND_MAX;

	float	result = randFloat * 1000.0;

	return result;
}

inline
double randomDouble( void )
{
	double	randDouble = rand() / (double) RAND_MAX;

	double	result = randDouble * 100000000.0;

	return result;
}


#endif
