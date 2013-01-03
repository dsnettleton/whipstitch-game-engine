/*
 * wsOperations.h
 *
 *  Created on: Jul 2, 2012
 *      Author: dsnettleton
 *
 *      This file declares various common numerical operations, optimizing
 *      wherever able. Trigonometric functions are confined to their own
 *      files, but random numbers and square root / absolute value operations
 *      are dealt with here. Wherever possible, SSE2 operations are used for
 *      speed.
 *
 *      Temporary variables are also declared to be used for any functions
 *      which desire them, avoiding the allocation overhead otherwise
 *      required for mathematical operations. These should be used sparingly
 *      where multi-threading might be performed.
 *
 *      Random number generation is performed using the Mersenne Twister algorithm
 *
 *  Copyright D. Scott Nettleton, 2013
 *  This software is released under the terms of the
 *  Lesser GNU Public License (LGPL).
 *  This file is part of the Whipstitch Game Engine.
 *  The Whipstitch Game Engine is free software: you can redistribute it
 *  and/or modify it under the terms of the GNU Lesser Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *  The Whipstitch Game Engine is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser Public License for more details.
 *  You should have received a copy of the GNU Lesser Public License
 *  along with The Whipstitch Game Engine.
 *  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef WS_OPERATIONS_H_
#define WS_OPERATIONS_H_

#include "wsPlatform.h"
#include "wsProfiling.h"
#include <math.h>

#if WS_SUPPORTS_SSE4 == WS_TRUE
#include <smmintrin.h>

//  Macros for shuffling SSE register values
//  Shuffles x,y,z,w values across the register
#define WS_SSE_SHUFF_VALS(x, y, z, w) \
    ( ((w) << 6) | ((z) << 4) | ((y) << 2) | (x) )
//  Generic shuffle macro
#define ws_sseShuffle(v, x, y, z, w) \
    _mm_shuffle_ps((v), (v), WS_SSE_SHUFF_VALS((x), (y), (z), (w)))
//  Distribute the x value across the entire register
#define ws_sseDistributeX(v) \
    ws_sseShuffle((v), 0, 0, 0, 0)
//  Distribute the y value across the entire register
#define ws_sseDistributeY(v) \
    ws_sseShuffle((v), 1, 1, 1, 1)
//  Distribute the z value across the entire register
#define ws_sseDistributeZ(v) \
    ws_sseShuffle((v), 2, 2, 2, 2)
//  Distribute the w value across the entire register
#define ws_sseDistributeW(v) \
    ws_sseShuffle((v), 3, 3, 3, 3)
//  Multiply-and-Add instruction (multiply a and b, then add to c)
#define ws_sseMadd(a, b, c) \
    _mm_add_ps(_mm_mul_ps((a), (b)), (c))

//  Macro to align data types for SIMD Registers
#ifdef _MSC_VER //  For Visual Studio
#define sseAlign(X) __declspec(align(16)) X
#else //  For everything else
#define sseAlign(X) X __attribute__((aligned(16)))
#endif
#endif  /* WS_SUPPORTS_SSE4 */

extern u32 wsCRC32HashFuncTable[256];
extern bool wsCRC32HashTableGenerated;
#define WS_CRC32_POLYNOMIAL 0x82F63B78  //  CRC-32C Reversed order representation
#define WS_PRIME_TABLE_SIZE 128
extern u32 wsPrimeTable[WS_PRIME_TABLE_SIZE];   //  Stores the first 128 prime numbers, beginning with 2.

bool isPositive(f32 myFloat); //  Uses the i_f_hybrid, since integer sign tests are faster.
void clampZeroOne(f32 &myFloat);  //  Clamps the value to the range [0, 1]
f32 wsAbs(f32 myFloat);
f32 wsSqrt(f32 myFloat);
//  Minimums and Maximums
f32 wsMax(f32 a, f32 b);
f32 wsMin(f32 a, f32 b);
i32 wsMax(i32 a, i32 b);
i32 wsMin(i32 a, i32 b);
u32 wsMax(u32 a, u32 b);
u32 wsMin(u32 a, u32 b);
//  Hash Functions
void wsBuildCRC32HashTable();
u32 wsHash(f32 myFloat);
u32 wsHash(const char* myString);
//  Random Numbers
void wsInitRandomizer(f32 seed);
void wsGenRandoms();
u32 wsRand();
i32 wsRandomInt(i32 min, i32 max);
f32 wsRandomFloat(f32 min, f32 max, f32 precision = 0.000001f);
//  Interpolations Algorithms
f32 wsBlendFactor(f32 start, f32 mid, f32 end); //  Determines a blend factor based on the three points given.
f32 wsLerp(f32 a, f32 b, f32 blendFactor);  //  Linear Interpolation
f32 wsSlerp(f32 a, f32 b, f32 theta, f32 blendFactor); //  Sphereical Linear Interpolation
//  Swap bytes for writing opposite-endian data files (implemented for all value types)
inline u16 wsEndianSwap(u16 val) {
    return ((val & 0x00FF) << 8) | ((val & 0xFF00) >> 8);
}
inline u32 wsEndianSwap(u32 val) {
    return ((val & 0x000000FF) << 24) | ((val & 0x0000FF00) << 8) |
            ((val & 0x00FF0000) >> 8) | ((val & 0xFF000000) >> 24);
}
inline u64 wsEndianSwap(u64 val) {
    return ((val & 0x00000000000000FF) << 56) | ((val & 0x000000000000FF00) << 40) |
            ((val & 0x0000000000FF0000) << 24) | ((val & 0x00000000FF000000) << 8) |
            ((val & 0x000000FF00000000) >> 8) | ((val & 0x0000FF0000000000) >> 24) |
            ((val & 0x00FF000000000000) >> 40) | ((val & 0xFF00000000000000) >> 56);
}
inline i16 wsEndianSwap(i16 val) {
    return ((val & 0x00FF) << 8) | ((val & 0xFF00) >> 8);
}
inline i32 wsEndianSwap(i32 val) {
    return ((val & 0x000000FF) << 24) | ((val & 0x0000FF00) << 8) |
            ((val & 0x00FF0000) >> 8) | ((val & 0xFF000000) >> 24);
}
inline i64 wsEndianSwap(i64 val) {
    return ((val & 0x00000000000000FF) << 56) | ((val & 0x000000000000FF00) << 40) |
            ((val & 0x0000000000FF0000) << 24) | ((val & 0x00000000FF000000) << 8) |
            ((val & 0x000000FF00000000) >> 8) | ((val & 0x0000FF0000000000) >> 24) |
            ((val & 0x00FF000000000000) >> 40) | ((val & 0xFF00000000000000) >> 56);
}
inline f32 wsEndianSwap(f32 val) {
    i_f_hybrid mybrid(val);
    return wsEndianSwap(mybrid.i);
}
inline f64 wsEndianSwap(f64 val) {
    i_f_hybrid64 mybrid(val);
    return wsEndianSwap(mybrid.i);
}

//  Prime numbers are important for efficient hashmaps
inline u32 wsNextPrime(u32 val) {
    for (int i = 0; i <= WS_PRIME_TABLE_SIZE; ++i) {
        if (val < wsPrimeTable[i]) {
            return wsPrimeTable[i];
        }
    }
    return 27644437;    //  Sufficiently high, I hope...
}

#endif /* WS_OPERATIONS_H_ */
