/*
 * wsTrig.c
 *
 *  Created on: Jul 2, 2012
 *      Author: dsnettleton
 *
 *      This file implements some useful trigonometric operations,
 *      which are optimized for speed on this engine. The operations
 *      utilize a set of lookup tables, sacrificing some accuracy
 *      for a lot more speed.
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

#include "wsTrig.h"
#include "wsOperations.h"

//  Implement Trig Tables
f32 sine_table[512];
f32 cosine_table[512];
f32 tangent_table[512];

//  This function builds the lookup tables used by the trig engine
void genLookupTables() {
    WS_PROFILE();
    for (i32 i = 0; i < 512; ++i) {
        f64 radians = (f64) i * TWO_PI / 512.0;
        sine_table[i] = (f32) sin(radians);
        cosine_table[i] = (f32) cos(radians);
        if (radians != HALF_PI) {
            tangent_table[i] = (f32) tan(radians);
        }
        else {
            tangent_table[i] = 999999999999.0f; //  A really big number
        }
    }

}

//  These trig functions require the operation DEG_TO_RAD*(512.0f / TWO_PI)
//  This can be simplified to the constant:
#define TRIG_OPTIMIZER 1.422222222222f

f32 wsSin(f32 degrees) {
    WS_PROFILE();
    i_f_hybrid mybrid(degrees * TRIG_OPTIMIZER + BIAS_NEG.f);
    f32 my = sine_table[(mybrid.i & 511)];

    return my;
}
f32 wsCos(f32 degrees) {
    WS_PROFILE();
    i_f_hybrid mybrid(degrees * TRIG_OPTIMIZER + BIAS_NEG.f);
    f32 my = cosine_table[(mybrid.i & 511)];

    return my;
}

f32 wsTan(f32 degrees) {
    WS_PROFILE();
    i_f_hybrid mybrid(degrees * TRIG_OPTIMIZER + BIAS_NEG.f);
    f32 my = tangent_table[(mybrid.i & 511)];

    return my;
}

f32 wsArcSin(f32 val) {
    WS_PROFILE();
    f32 my = asinf(val)*RAD_TO_DEG;

    return my;
}

f32 wsArcCos(f32 val) {
    WS_PROFILE();
    f32 my = acosf(val)*RAD_TO_DEG;

    return my;
}

f32 wsArcTan(f32 valX, f32 valY) {
    WS_PROFILE();
    f32 my = atan2f(valX, valY)*RAD_TO_DEG;

    return my;
}
