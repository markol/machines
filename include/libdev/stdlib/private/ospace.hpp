/*
 * O S P A C E . H P P
 * (c) Charybdis Limited, 1997. All Rights Reserved.
 */

/*
	This file must be included before including any of the Object Space STL files
*/

//  Including base is largely to make sure we pick up the Charybdis version of bool.

#include "base/base.hpp"

#define OS_DOS_6
#define OS_ANSI
#define _OBJECTSPACE_STL

#include <config/compiler.h>

#include <errno.h>
#define os_errno errno
#define os_socket_errno errno
#define OS_EVENT_ERRNO_H

#include <ospace/stl/heapaloc.h>

//  Speed up the compilations by cutting out the ObjectSpace template
//  instantiation unless we are specifically instantiating templates

#ifndef _INSTANTIATE_TEMPLATE_CLASSES
    #ifdef OS_NO_AUTO_INSTANTIATE
        #undef OS_NO_AUTO_INSTANTIATE
    #endif
#endif

// #define Allocator   heap_allocator