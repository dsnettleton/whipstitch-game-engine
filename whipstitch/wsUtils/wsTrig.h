/*
 * wsTrig.h
 *
 *  Created on: Jul 2, 2012
 *      Author: dsnettleton
 *
 *      This file declares some useful trigonometric operations,
 *      which are optimized for speed on this engine. The operations
 *      utilize a set of lookup tables, sacrificing some accuracy
 *      for a lot more speed.
 *
 *      Some useful constants are also declared.
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

#ifndef WS_TRIG_H_
#define WS_TRIG_H_

#include "wsTypes.h"
#include <math.h>

#ifndef PI
#define PI 3.14159265f
#endif

#ifndef TWO_PI
#define TWO_PI 6.283185307f   //  2.0f * PI
#endif

#ifndef HALF_PI
#define HALF_PI 1.570796327f  //  PI / 2.0f
#endif

#ifndef DEG_TO_RAD
#define DEG_TO_RAD 0.017453293f //  PI / 180.0f
#endif

#ifndef RAD_TO_DEG
#define RAD_TO_DEG 57.295779513f  //  180.0f / PI
#endif

extern f32 sine_table[512];
extern f32 cosine_table[512];
extern f32 tangent_table[512];

void genLookupTables();
void printLookupTables(const char* filename);

f32 wsSin(f32 degrees);
f32 wsCos(f32 degrees);
f32 wsTan(f32 degrees);

f32 wsArcSin(f32 val);
f32 wsArcCos(f32 val);
f32 wsArcTan(f32 valX, f32 valY);

#endif /* WS_TRIG_H_ */
