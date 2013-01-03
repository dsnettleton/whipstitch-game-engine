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
