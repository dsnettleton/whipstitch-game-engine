/*
 * wsTime.h
 *
 *  Created on: Jul 2, 2012
 *      Author: dsnettleton
 *
 *      This file declares a couple of types for measuring time.
 *      The type t64 is recommended for all long-term purposes, though
 *      t32 can be used for "delta-t," i.e. small changes in time.
 *
 *      The time is retrieved using the openMP wall clock, which is
 *      not only very accurate, but also useful for multi-threaded
 *      environments.
 *
 *      This file also contains benchmarkBegin() and benchmarkEnd()
 *      functions. The benchmarkEnd() always returns the time elapsed
 *      since the last call to benchmarkBegin().
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

#ifndef WS_TIME_H_
#define WS_TIME_H_

#include "wsTypes.h"

typedef f32 t32;    //  Stores time in seconds with a floating point
typedef f64 t64;    //  Stores time in seconds with a double floating point

t64 wsGetTime();
void wsWait(t64 waitTime);

extern t64 wsBenchmark_time;
void wsBenchmarkBegin();
t64 wsBenchmarkEnd(); //  Returns the difference in seconds

#endif /* WS_TIME_H_ */
