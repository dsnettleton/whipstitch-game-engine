/*
 * wsLog.h
 *
 *  Created on: Jul 11, 2012
 *      Author: dsnettleton
 *
 *      This file declares the functions utilized by the engine for logging
 *      output, debugging information, assertions, etc.
 *      Logs are printed to the console, and can also be saved to various log files
 *      for efficient use.
 *      The functions require a 16-bit unsigned integer representing one or more
 *      log channels. The functions print information to those channels, which can be
 *      printed to the console or to a file. Channels can be examined individually or
 *      several at once, depending on current engine settings.
 */
//	Copyright D. Scott Nettleton, 2013
//	This software is released under the terms of the
//	Lesser GNU Public License (LGPL).

#ifndef WS_LOG_H_
#define WS_LOG_H_

#include "wsPlatform.h"
#include <stdarg.h>

#define WS_LOG_ALL          0xFFFF  //  Do not output to this log channel
#define WS_LOG_DEBUG        0x0073  //  Contains Main, Platform, assertions, info, and error. Do not output to this log
#define WS_LOG_MAIN         0x0001
#define WS_LOG_PLATFORM     0x0002
#define WS_LOG_UTIL         0x0004
#define WS_LOG_PROFILING    0x0008
#define WS_LOG_ASSERTIONS   0x0010
#define WS_LOG_INFO         0x0020
#define WS_LOG_ERROR        0x0040
#define WS_LOG_MEMORY       0x0080
#define WS_LOG_GRAPHICS     0x0100
#define WS_LOG_THREADS      0x0200
#define WS_LOG_HID          0x0400
#define WS_LOG_EVENTS       0x0800
#define WS_LOG_SHADER       0x1000
#define WS_LOG_SOUND        0x2000
#define WS_LOG_NETWORKING   0x4000

#define WS_MAX_LOG_CHARS 511

extern char wsLogBuffer[WS_MAX_LOG_CHARS+1];
extern u16 wsActiveLogs;

void wsEcho(const char* str, ...);
void wsEcho(u16 channels, const char* str, ...);
void wsLogAssertionFailure(const char* expr, const char* file, u32 line);
void wsLogAssertionFailure(const char* expr, const char* file, u32 line, const char* msg);

#endif /* WSLOG_H_ */
