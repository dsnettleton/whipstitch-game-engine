/*
 * wsPlatform.h
 *
 *  Created on: Jul 2, 2012
 *      Author: dsnettleton
 *
 *      This file imports OpenMP libraries and defines a few values
 *      letting us know what platform we're running on.
 *
 *  This software is provided under the terms of the MIT license
 *  Copyright (c) D. Scott Nettleton, 2013
 *
 *  Permission is hereby granted, free of charge, to any person
 *  obtaining a copy of this software and associated documentation
 *  files (the "Software"), to deal in the Software without
 *  restriction, including without limitation the rights to use, copy,
 *  modify, merge, publish, distribute, sublicense, and/or sell copies
 *  of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *  OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef WSPLATFORM_H_
#define WSPLATFORM_H_

#include "wsTypes.h"
#include "omp.h"    //  Link with -lgomp (GCC)

#define WS_NULL 0
#define WS_FALSE    0x00
#define WS_TRUE     0xFF
#define WS_SUCCESS  0x00000000
#define WS_FAIL     0xFFFFFFFF

#define WS_BUFFER_OFFSET(bytes) ((char*)NULL + (bytes))

#ifdef __SSE4_1__
    //  Be sure to use the compiler build option -msse4
    //#define WS_SUPPORTS_SSE4    WS_TRUE
    #define WS_SUPPORTS_SSE4    WS_FALSE
#else
    #define WS_SUPPORTS_SSE4    WS_FALSE
#endif

#define WS_NUM_CORES omp_get_num_procs()
const u32 WS_BIT_ENVIRONMENT = sizeof(void*) * 8;

#define WS_OS_UNKNOWN   0x00
#define WS_OS_LINUX     0x01
#define WS_OS_WINDOWS   0x02
#define WS_OS_UNIX      0x03
#define WS_OS_MAC_OSX   0x04
#define WS_OS_ANDROID   0x05

#if defined(ANDROID) || defined(__ANDROID__)
    #define WS_CURRENT_OS WS_OS_ANDROID     //  Ostensibly using Android NDK
    #define WS_OS_FAMILY_UNIX
#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__TOS_LINUX__)
    #define WS_CURRENT_OS   WS_OS_LINUX
    #define WS_OS_FAMILY_UNIX
#elif defined(_WIN32_WCE) || defined(_WIN32) || defined(_WIN64)
    #define WS_CURRENT_OS   WS_OS_WINDOWS
    #define WS_OS_FAMILY_WINDOWS
#elif defined(__APPLE__) || defined(__TOS_MACOS__)
    #define WS_CURRENT_OS   WS_OS_MAC_OSX
    #define WS_OS_FAMILY_UNIX
#elif defined(__FreeBSD__) || defined(hpux) || defined(_hpux) || defined(__NetBSD__) || \
    defined(__OpenBSD__) || defined(sun) || defined(__sun) || defined(__CYGWIN__) || \
    defined(unix) || defined(__unix) || defined(__unix__)
    #define WS_CURRENT_OS   WS_OS_UNIX
    #define WS_OS_FAMILY_UNIX
#endif

#define WS_ARCH_UNKNOWN 0
#define WS_ARCH_AMD64   1
#define WS_ARCH_I32     2
#define WS_ARCH_PPC     3
#define WS_ARCH_ARM     4

#define WS_UNKNOWN_ENDIAN   0
#define WS_LITTLE_ENDIAN    1
#define WS_BIG_ENDIAN       2

#if defined(i386) || defined(__i386) || defined(__i386__) || defined(_M_IX86)
    #define WS_CURRENT_ARCHITECTURE WS_ARCH_I32
    #define WS_CURRENT_ENDIAN       WS_LITTLE_ENDIAN
#elif defined(__x86_64__) || defined(_M_X64)
    #define WS_CURRENT_ARCHITECTURE WS_ARCH_AMD64
    #define WS_CURRENT_ENDIAN       WS_LITTLE_ENDIAN
#elif defined(__ppc) || defined(__ppc__) || defined(__PPC) || defined(__PPC__) || \
    defined(__powerpc) || defined(__powerpc__) ||  defined(__POWERPC__) || \
    defined(_ARCH_PPC) || defined(_M_PPC)
    #define WS_CURRENT_ARCHITECTURE WS_ARCH_PPC
    #define WS_CURRENT_ENDIAN       WS_BIG_ENDIAN
#elif defined(__arm) || defined(__arm__) || defined(ARM) || defined(_ARM_) || \
    defined(__ARM__) || defined(_M_ARM)
    #define WS_CURRENT_ARCHITECTURE WS_ARCH_ARM
    #if defined(__ARMEB__)
        #define WS_CURRENT_ENDIAN   WS_BIG_ENDIAN
    #else
        #define WS_CURRENT_ENDIAN   WS_LITTLE_ENDIAN
    #endif
#endif

#if WS_CURRENT_ARCHITECTURE == WS_ARCH_I32 || WS_CURRENT_ARCHITECTURE == WS_ARCH_AMD64
    #if WS_CURRENT_OS == WS_OS_WINDOWS
        #define WS_DEBUG_BREAK()        __asm { int 3 }
    #else
        #define WS_DEBUG_BREAK()        __asm__("int $0x03")
    #endif
#else   //  Power-PC or ARM Architectures
    #define WS_DEBUG_BREAK()            __asm__("trap")
#endif

#define WS_MODE_DEBUG   0x00
#define WS_MODE_PROFILE 0x01
#define WS_MODE_GAME    0x02

#ifndef NDEBUG  //  If this is Debug Mode
    #if defined(_PROFILE)   //  Profiling
        #define WS_CURRENT_MODE WS_MODE_PROFILE
    #else                   //  Not profiling
        #define WS_CURRENT_MODE WS_MODE_DEBUG
    #endif
    #define wsAssert(expr, message) \
        if (expr) {} \
        else { \
            wsLogAssertionFailure(#expr, __FILE__, __LINE__, (message)); \
            WS_DEBUG_BREAK(); \
        }
#else   //  If this is Release mode
    #define WS_CURRENT_MODE WS_MODE_GAME
    #define wsAssert(expr, message) expr //  Evaluates to the expression to avoid assertions in game mode
#endif

#endif /* WSPLATFORM_H_ */
