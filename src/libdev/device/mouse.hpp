/*
 * M O U S E . H P P
 * (c) Charybdis Limited, 1995, 1996, 1997. All Rights Reserved.
 */

// Include this file to use the mouse under any supported
// operating system.  The actual choice of mouse implmentation
// must be compiled into the client code, all mouse implementations
// should be source code compatible.  You must of course link
// with the corresponding library.

#ifndef DEVICE_MOUSE
#define DEVICE_MOUSE

#include "base/base.hpp"

#if defined _DOSAPP
    #include "device/private/dosmouse.hpp"
#elif defined _WIN95APP
    #include "device/private/w95mouse.hpp"
#elif defined _SDLAPP
    #include "device/private/sdlmouse.hpp"
#else
    #error Use of the device lib: one of _DOSAPP, _WIN95APP, etc. must be defined.
#endif

#endif

/* End MOUSE.HPP ****************************************************/
