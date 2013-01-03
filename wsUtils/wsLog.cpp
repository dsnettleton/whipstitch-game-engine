/*
 * wsLog.cpp
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
 //    Copyright D. Scott Nettleton, 2013
 //    This software is released under the terms of the
 //    Lesser GNU Public License (LGPL).
 
#include "wsLog.h"

#include <stdio.h>

char wsLogBuffer[WS_MAX_LOG_CHARS + 1];
u16 wsActiveLogs = 0xFFFF;  //  All logs active by default

void wsLog(u16 channels, const char* str, va_list args) {
    vsnprintf(wsLogBuffer, WS_MAX_LOG_CHARS, str, args);
    wsLogBuffer[WS_MAX_LOG_CHARS] = '\0';

    printf("Log:");
    if ((channels & WS_LOG_MAIN) != 0) {
        printf("  MAIN");
    }
    if ((channels & WS_LOG_PLATFORM) != 0) {
        printf("  PLATFORM");
    }
    if ((channels & WS_LOG_UTIL) != 0) {
        printf("  UTIL");
    }
    if ((channels & WS_LOG_PROFILING) != 0) {
        printf("  PROFILING");
    }
    if ((channels & WS_LOG_ASSERTIONS) != 0) {
        printf("  ASSERTIONS");
    }
    if ((channels & WS_LOG_INFO) != 0) {
        printf("  INFO");
    }
    if ((channels & WS_LOG_ERROR) != 0) {
        printf("  ERROR");
    }
    if ((channels & WS_LOG_MEMORY) != 0) {
        printf("  MEMORY");
    }
    if ((channels & WS_LOG_GRAPHICS) != 0) {
        printf("  GRAPHICS");
    }
    if ((channels & WS_LOG_THREADS) != 0) {
        printf("  THREADS");
    }
    if ((channels & WS_LOG_HID) != 0) {
        printf("  HID");
    }
    printf("\n  %s", wsLogBuffer);
}

void wsLog(const char* str, ...) {
    if ((wsActiveLogs & WS_LOG_MAIN) != 0) {
        va_list args;
        va_start(args, str);
        wsLog(WS_LOG_MAIN, str, args);
        va_end(args);
        printf("\n");
    }
}

void wsLog(u16 channels, const char*str, ...) {
    if ((wsActiveLogs & channels) != 0) {
        va_list args;
        va_start(args, str);
        wsLog(channels, str, args);
        va_end(args);
        printf("\n");
    }
}

void wsLogAssertionFailure(const char* expr, const char* file, u32 line) {
    if ((wsActiveLogs & WS_LOG_ASSERTIONS) != 0) {
        printf("ASSERTION FAILURE!\n");
        printf("    Expression:  ( %s )\n", expr);
        printf("    File:        %s\n", file);
        printf("    Line:        %u\n", line);
        printf("\n");
    }
}

void wsLogAssertionFailure(const char* expr, const char* file, u32 line, const char* msg) {
    if ((wsActiveLogs & WS_LOG_ASSERTIONS) != 0) {
        printf("ASSERTION FAILURE!\n");
        printf("    *** %s ***\n", msg);
        printf("    Expression:  ( %s )\n", expr);
        printf("    File:        %s\n", file);
        printf("    Line:        %u\n", line);
        printf("\n");
    }
}
