/*
 * K E Y B O A R D . H P P
 * (c) Charybdis Limited, 1995, 1996. All Rights Reserved.
 */
// Include this file to use the keyboard under any supported
// operating system.  The actual choice of keyboard implmentation
// must be compiled into the client code, all keyboard implementations
// should be source code compatible.  You must of course link
// with the corresponding library.

#ifndef DEVICE_KEYBOARD
#define DEVICE_KEYBOARD

#include "base/base.hpp"

#if defined _DOSAPP
    #include "device/private/doskeybd.hpp"
#elif defined _WIN95APP
    #include "device/private/w95keybd.hpp"
#elif defined _SDLAPP
    #include "device/private/sdlkeybd.hpp"
#else
    #error Use of the device lib: one of _DOSAPP, _WIN95APP, etc. must be defined.
#endif

#endif

/* End KEYBOARD.HPP ****************************************************/
