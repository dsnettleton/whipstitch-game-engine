/*
 * wsTime.c
 *
 *  Created on: Jul 2, 2012
 *      Author: dsnettleton
 *
 *      This file implements a couple of types for measuring time.
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

#include "wsTime.h"

#include <omp.h>

t64 wsGetTime() {
  return omp_get_wtime();
}

void wsWait(t64 waitTime) {
  t64 startTime = wsGetTime();
  while (wsGetTime() - startTime < waitTime) {}   //  Do nothing
}

t64 wsTimeElapsed(t64 startTime) {
  return wsGetTime() - startTime;
}

//  Returns zero if wait time is expired. Otherwise returns a positive t64
t64 wsWaitAsync(t64 duration) {
  duration = wsGetTime() - duration;
  return duration > 0.0 ? duration : 0.0;
}

t64 wsBenchmark_time = 0.0;
void wsBenchmarkBegin() {
  wsBenchmark_time = wsGetTime();
}
t64 wsBenchmarkEnd() {
  return wsGetTime() - wsBenchmark_time;
}
