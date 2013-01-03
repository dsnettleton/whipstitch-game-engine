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
