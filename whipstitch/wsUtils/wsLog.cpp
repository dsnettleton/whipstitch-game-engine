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

void wsEcho(u16 channels, const char* str, va_list args) {
    vsnprintf(wsLogBuffer, WS_MAX_LOG_CHARS, str, args);
    wsLogBuffer[WS_MAX_LOG_CHARS] = '\0';

    printf("Log:");
    if (channels & WS_LOG_MAIN) {
        printf("  MAIN");
    }
    if (channels & WS_LOG_PLATFORM) {
        printf("  PLATFORM");
    }
    if (channels & WS_LOG_UTIL) {
        printf("  UTIL");
    }
    if (channels & WS_LOG_PROFILING) {
        printf("  PROFILING");
    }
    if (channels & WS_LOG_ASSERTIONS) {
        printf("  ASSERTIONS");
    }
    if (channels & WS_LOG_INFO) {
        printf("  INFO");
    }
    if (channels & WS_LOG_ERROR) {
        printf("  ERROR");
    }
    if (channels & WS_LOG_MEMORY) {
        printf("  MEMORY");
    }
    if (channels & WS_LOG_GRAPHICS) {
        printf("  GRAPHICS");
    }
    if (channels & WS_LOG_THREADS) {
        printf("  THREADS");
    }
    if (channels & WS_LOG_HID) {
        printf("  HID");
    }
    if (channels & WS_LOG_EVENTS) {
        printf("  EVENTS");
    }
    if (channels & WS_LOG_SHADER) {
        printf("  SHADER");
    }
    if (channels & WS_LOG_SOUND) {
        printf("  SOUND");
    }
    printf("\n  %s", wsLogBuffer);
    fflush(stdout); // Will now print everything in the stdout buffer
}

void wsEcho(const char* str, ...) {
    if (wsActiveLogs & WS_LOG_MAIN) {
        va_list args;
        va_start(args, str);
        wsEcho(WS_LOG_MAIN, str, args);
        va_end(args);
        printf("\n");
    }
    fflush(stdout); // Will now print everything in the stdout buffer
}

void wsEcho(u16 channels, const char*str, ...) {
    if (wsActiveLogs & channels) {
        va_list args;
        va_start(args, str);
        wsEcho(channels, str, args);
        va_end(args);
        printf("\n");
    }
    fflush(stdout); // Will now print everything in the stdout buffer
}

void wsLogAssertionFailure(const char* expr, const char* file, u32 line) {
    if ((wsActiveLogs & WS_LOG_ASSERTIONS) != 0) {
        printf("ASSERTION FAILURE!\n");
        printf("    Expression:  ( %s )\n", expr);
        printf("    File:        %s\n", file);
        printf("    Line:        %u\n", line);
        printf("\n");
    }
    fflush(stdout); // Will now print everything in the stdout buffer
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
    fflush(stdout); // Will now print everything in the stdout buffer
}
