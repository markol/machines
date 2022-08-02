/*
 * R A N D O M . H P P
 * (c) Charybdis Limited, 1995. All Rights Reserved
 */

#ifndef _RANDOM_HPP
#define _RANDOM_HPP

#include <stdlib.h>

//////////////////////////////

//unsigned int randomInt( unsigned int );
int randomInt( int, int );
unsigned int randomInt();
double randomFloat( double d );
double randomFloat();
double randomFloat( double minValue, double maxValue );

//////////////////////////////

#ifdef _INLINE
	#include "testh/random.ipp"
#endif

//////////////////////////////

#endif
