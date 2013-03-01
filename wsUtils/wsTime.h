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
