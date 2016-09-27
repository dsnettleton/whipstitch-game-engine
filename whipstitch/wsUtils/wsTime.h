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
 *  Copyright (c) D. Scott Nettleton, 2013-2016
*/

#ifndef WS_TIME_H_
#define WS_TIME_H_

#include "wsTypes.h"

typedef f32 t32;    //  Stores time in seconds with a floating point
typedef f64 t64;    //  Stores time in seconds with a double floating point

t64 wsGetTime();
void wsWait(t64 waitTime);
t64 wsWaitAsync(t64 duration);

extern t64 wsBenchmark_time;
void wsBenchmarkBegin();
t64 wsBenchmarkEnd(); //  Returns the difference in seconds

#endif /* WS_TIME_H_ */
