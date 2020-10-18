/*
 * A L G O R I T H M 
 * (c) Charybdis Limited, 1995, 1996, 1997. All Rights Reserved.
 */

#ifndef _STD_ALGORITHM
#define _STD_ALGORITHM

#include "base/base.hpp"

#include "stdlib/private/ospace.hpp"

// Because the ObjectSpace method of not instantiating all functions doesn't
// discriminate between member functions and non-member functions we have to
// use this trick to make sure that the template functions associated with
// the algorithms are always included.

#ifndef OS_NO_AUTO_INSTANTIATE
    #ifdef _INSTANTIATE_TEMPLATE_FUNCTIONS
        #define OS_NO_AUTO_INSTANTIATE
        #define AUTO_INSTANTIATE_FORCED
    #endif
#endif

#include "include/algorith.h"

#ifdef AUTO_INSTANTIATE_FORCED
    #undef OS_NO_AUTO_INSTANTIATE
    #undef AUTO_INSTANTIATE_FORCED
#endif

#endif	/*	#ifndef _STD_ALGORITHM	*/
