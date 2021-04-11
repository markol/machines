/*
 * C O M P I L E R . H P P
 * (c) Charybdis Limited, 1995,1996. All Rights Reserved.
 */

#ifndef COMPILER_HPP
#define COMPILER_HPP

//////////////////////////////////////////////////////////////////////

// Disable exceptions in Charybdis code
#define NO_EXCEPTIONS 1

//////////////////////////////////////////////////////////////////////

#ifdef __WATCOMC__

    #if __WATCOMC__ == 1000
        // watcom c++ version 10.00
        #include "base/watc1000.hpp"
        #define _COMPILER_VERSION   10.00
    #endif  /*  #if __WATCOMC__ == 1000 */

    #if __WATCOMC__ == 1050
        // watcom c++ version 10.50
        #include "base/watc1050.hpp"
        #define _COMPILER_VERSION   10.50
    #endif  /*  #if __WATCOMC__ == 1050 */

    #if __WATCOMC__ == 1060
        // watcom c++ version 10.60
        #include "base/watc1060.hpp"
        #define _COMPILER_VERSION   10.60
    #endif  /*  #if __WATCOMC__ == 1060 */

    #define _COMPILER_NAME  WATCOM

    #ifdef M_I386
        #define _LINEAR_ADDRESSING
    #endif  /*  #ifdef M_I386   */

    #ifdef _LINEAR_ADDRESSING
        #include "base/limitw32.hpp"
    #else
        #include "base/limitw16.hpp"
    #endif

    //  Disable "must look ahead to determine whether construct
    //   is a declaration/type or an expression" warning
    #pragma warning 604 9;

    //  Disable "construct resolved as an expression" warning
    #pragma warning 595 9;

    //  Disable "'sizeof' operand contains compiler generated information" warning
    #pragma warning 549 9;

    // Disable "construct resolved as a declaration/type" warning
    #pragma warning 594 9;

    // Disable "assuming unary 'operator &' not overloaded for type" warning
    #pragma warning 666 9;

    // Disable "expression is useful only for its side effects" warning
    #pragma warning 387 9;

    #ifdef __DOS__
        #define _DOSAPP
    #endif

    #ifdef __NT__
        #define _WIN95APP
    #endif

#endif  /*  #ifdef __WATCOMC__  */

#ifdef __GNUC__
    #if __GNUC__ >= 5
        //#include "base/watc1060.hpp"
        #define _COMPILER_VERSION
    #endif

    #define _COMPILER_NAME  GCC

    #if defined(__x86_64__)
        /* 64 bit detected */
        #include "base/limitw64.hpp"
    #endif
    #if defined(__aarch64__)
        /* 64 bit arm detected */
        #include "base/limitw64.hpp"
    #endif
    #if defined(__i386__)
        /* 32 bit x86 detected */
        #include "base/limitw32.hpp"
    #endif
    #if defined(__arm__)
        /* 32 bit arm detected */
        #include "base/limitw32.hpp"
    #endif

    //#define _WIN95APP
    #define _SDLAPP

#endif

//////////////////////////////////////////////////////////////////////

#ifdef NO_BOOL
    #include "base/bool.h"
#endif  /* #ifdef NO_BOOL   */

#ifdef NO_ISO646_DEFINITIONS
    #include "base/iso646.h"
#endif  /* #ifdef NO_ISO646_DEFINITIONS */

//////////////////////////////////////////////////////////////////////

#ifndef _COMPILER_NAME
    #error This compiler is not catered for in file "base/compiler.hpp".
#endif  /*  #ifndef _COMPILER_NAME  */

#ifndef _COMPILER_VERSION
    #error This version of the compiler is not catered for in file "base/compiler.hpp".
#endif  /*  #ifndef _COMPILER_VERSION   */

//////////////////////////////////////////////////////////////////////

#ifndef _DOSAPP
    #ifndef _WIN95APP
        #ifndef _SDLAPP
            #error No operating system defined
        #endif
    #endif
#endif

//////////////////////////////////////////////////////////////////////

#endif  /*  #ifndef COMPILER_HPP    */
