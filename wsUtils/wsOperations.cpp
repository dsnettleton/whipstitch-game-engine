/*
 * wsOperations.c
 *
 *  Created on: Jul 2, 2012
 *      Author: dsnettleton
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

#include "wsOperations.h"
#include "wsTrig.h"
#include <stdlib.h>
#include <string.h> //  For strlen()

u32 wsScreenWidth = 1280;
u32 wsScreenHeight = 720;

u32 wsCRC32HashFuncTable[256];
bool wsCRC32HashTableGenerated = false;

bool isPositive(f32 myFloat) { //  integer sign tests often run more quickly
    WS_PROFILE();
    i_f_hybrid my(myFloat);

    return (my.i >= 0);
}

void clampZeroOne(f32 &myFloat) {
    WS_PROFILE();
    i_f_hybrid my(myFloat);
    //  Clamp negative values to zero
    my.i &= -(my.i >> 31);
    //  Clamp values > 1 back down to 1
    my.f -= 1.0f;
    my.i &= (my.i >> 31);
    myFloat = my.f + 1.0f;

}

f32 wsAbs(f32 myFloat) {
    WS_PROFILE();
    i_f_hybrid my(myFloat);
    my.i &= 0x7FFFFFFF;

    return my.f;
}

f32 wsSqrt(f32 myFloat) {
    WS_PROFILE();
    f32 my = sqrtf(myFloat);

    return my;
}

f32 wsMax(f32 a, f32 b) {
    return (a > b) ? a : b;
}

f32 wsMin(f32 a, f32 b) {
    return (a < b) ? a : b;
}

i32 wsMax(i32 a, i32 b) {
    return (a > b) ? a : b;
}

i32 wsMin(i32 a, i32 b) {
    return (a < b) ? a : b;
}

u32 wsMax(u32 a, u32 b) {
    return (a > b) ? a : b;
}

u32 wsMin(u32 a, u32 b) {
    return (a < b) ? a : b;
}

//  Hash Functions

u32 wsHash(f32 myFloat) {
    WS_PROFILE();
    u_f_hybrid mybrid(myFloat);

    return mybrid.u;
}

//  Using the CRC-32 algorithm for string hashes
void wsBuildCRC32HashTable() {
    WS_PROFILE();
    u32 tableVal;
    for (u32 i = 0; i < 256; ++i) {
        tableVal = i;
        for (u32 j = 0; j < 8; ++j) {
            if ((tableVal & 1) != 0) {
                tableVal = WS_CRC32_POLYNOMIAL ^ (tableVal >> 1);
            }
            else {
                tableVal >>= 1;
            }
        }
        wsCRC32HashFuncTable[i] = tableVal;
    }
    wsCRC32HashTableGenerated = true;

}

u32 wsHash(const char* myString) {
    wsAssert(wsCRC32HashTableGenerated, "Did you forget wsInit() at startup?");
    WS_PROFILE();
    u32 my = 0;
    u32 length = strlen(myString);
    for (u32 c = 0; c < length; ++c) {
        my = (my >> 8) ^ wsCRC32HashFuncTable[ (my & 0xFF) ^ myString[c] ];
    }

    return my;
}

//  Random number generation:
//  Mersenne Twister algorithm
//  The mersenne twister algorithm uses bitwise operations rather than multiplication,
//  which many other generators rely upon. This gives it quite a bit more speed. It also
//  happens to have a reasonable spread and a long period.
#define WS_MERSENNE_TWISTER_SIZE 624
u32 ws_mt_array[WS_MERSENNE_TWISTER_SIZE];
u32 ws_mt_index = 0;
//  Saving the initial seed will allow us to replicate results later on.
//  It is therefore not recommended to reinitialize the randomizer unless there
//  is a specific reason for doing so (e.g., restarting the game)
u32 wsInitialRandomSeed = 0;
//  This initializes the randomizer with a special seed.
void wsInitRandomizer(u32 seed) {
    WS_PROFILE();
    wsLog(WS_LOG_UTIL, "Initial Random Seed: %u\n", seed);
    wsInitialRandomSeed = seed;
    ws_mt_array[0] = seed;
    for (i32 i = 1; i < WS_MERSENNE_TWISTER_SIZE; ++i) {
        ws_mt_array[i] = 0x6C078965 * ((ws_mt_array[i-1] ^ (ws_mt_array[i-1] >> 30)) + i);
    }

}
//  This produces a bitwise conversion from float to unsigned integer, speeding up
//  and further randomizing any potential floating-point seeds. These are very likely,
//  incidentally, as many implementations (including ours) use time, in seconds, to
//  seed the randomizer.
void wsInitRandomizer(f32 seed) {
    WS_PROFILE();
    u_f_hybrid mybrid(seed);
    wsInitRandomizer(mybrid.u);

}
//  Generates an array of numbers for use by the mersenne twister algorithm
void wsGenRandoms() {
    WS_PROFILE();
    for (u32 i = 0; i < WS_MERSENNE_TWISTER_SIZE; ++i) {
        i32 iPlusOne = (i+1) % WS_MERSENNE_TWISTER_SIZE;
        i32 x = (((ws_mt_array[i] & 0x80000000) |
                (ws_mt_array[iPlusOne] & 0x7FFFFFFF)) >> 1) ^
                ((ws_mt_array[iPlusOne] & 1) ? 0x9908B0DF : 0x00000000);
        if (x%2 != 0) { //  if x is odd
            ws_mt_array[i] = ws_mt_array[i] ^ 0x9908B0DF;
        }
    }

}
u32 wsRand() {
    WS_PROFILE();
    if (ws_mt_index == 0) {
        wsGenRandoms();
    }
    i32 my = ws_mt_array[ws_mt_index];
    my = my ^ (my >> 11);
    my = my ^ ((my << 7) & 0x9D2C5680);
    my = my ^ ((my << 15) & 0xEFC60000);
    my = my ^ (my >> 18);
    ws_mt_index = (ws_mt_index+1) % WS_MERSENNE_TWISTER_SIZE;

    return my;
}
i32 wsRandomInt(i32 min, i32 max) {
    WS_PROFILE();
    if (min > max) {
        i32 tmp = min;
        min = max;
        max = tmp;
    }
    else if (min == max) {
        return max;
    }
    return (wsRand() % (max-min+1) + min);
}

f32 wsRandomFloat(f32 min, f32 max, f32 precision) {
    WS_PROFILE();
    if (min > max) {
        f32 tmp = min;
        min = max;
        max = tmp;
    }
    else if (min == max) {
        return max;
    }
    i32 steps = (max - min) / precision;
    return min + (f32)(wsRand() % (steps+1)) * precision;
}

f32 wsBlendFactor(f32 start, f32 mid, f32 end) {
    end -= start;
    if (end == 0.0f) { return end; }
    mid -= start;
    return mid / end;
}

//  Linear Interpolation
f32 wsLerp(f32 a, f32 b, f32 blendFactor) {
    WS_PROFILE();
    return (1.0f-blendFactor)*a + blendFactor*b;
}

//  Sphereical Linear Interpolation (good for quaternions)
f32 wsSlerp(f32 a, f32 b, f32 theta, f32 blendFactor) {
    WS_PROFILE();
    f32 blendoA = wsSin((1.0f-blendFactor)*theta) / wsSin(theta);
    f32 blendoB = wsSin(blendFactor*theta) / wsSin(theta);
    return blendoA*a + blendoB*b;
}

u32 wsPrimeTable[] = {  //  The first 128 prime numbers
    2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71,
    73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173,
    179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281,
    283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409,
    419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541,
    547, 557, 563, 569, 571, 577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643, 647, 653, 659,
    661, 673, 677, 683, 691, 701, 709, 719
};
