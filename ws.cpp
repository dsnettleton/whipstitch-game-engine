/*
 * ws.cpp
 *
 *  Created on: Jul 12, 2012
 *      Author: dsnettleton
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

#include "ws.h"

void wsInit(const char* title, const i32 width, const i32 height, bool fullscreen, u64 mainMem, u32 frameStackMem) {
    wsAssert(wsFile::exists(ws_path_cwd),
        "Current Working Directory could not be determined.");
    wsAssert(wsFile::exists(ws_path_home),
        "Current Home Directory could not be determined.");
    if (!exists(ws_path_log_dir)) {
        wsAssert(wsFile::create_directory(ws_path_log_dir),
                        "Could not create Log directory");
    }
    wsLog("Initializing Whipstitch Engine\n");
    wsLog("  CWD =  %s\n    HOME = %s\n    LOGS = %s\n", ws_path_cwd.string().c_str(),
                    ws_path_home.string().c_str(), ws_path_log_dir.string().c_str());
    switch (WS_CURRENT_MODE) {
        case WS_MODE_DEBUG:
            wsLog((WS_LOG_MAIN | WS_LOG_PLATFORM), "MODE = DEBUG\n");
            break;
        case WS_MODE_PROFILE:
            wsLog((WS_LOG_MAIN | WS_LOG_PLATFORM), "MODE = PROFILE\n");
            break;
        case WS_MODE_GAME:
            wsLog((WS_LOG_MAIN | WS_LOG_PLATFORM), "MODE = RELEASE\n");
            break;
    }
    switch (WS_CURRENT_OS) {
        case WS_OS_LINUX:
            wsLog((WS_LOG_MAIN | WS_LOG_PLATFORM), "Platform = Linux\n");
            break;
        case WS_OS_WINDOWS:
            wsLog((WS_LOG_MAIN | WS_LOG_PLATFORM), "Platform = Windows\n");
            break;
        case WS_OS_UNIX:
            wsLog((WS_LOG_MAIN | WS_LOG_PLATFORM), "Platform = Unix\n");
            break;
        case WS_OS_MAC_OSX:
            wsLog((WS_LOG_MAIN | WS_LOG_PLATFORM), "Plaform = Mac OSX\n");
            break;
        default:
        case WS_OS_UNKNOWN:
            wsLog((WS_LOG_MAIN | WS_LOG_PLATFORM), "Platform = Unknown\n");
            break;
    }
    switch (WS_CURRENT_ENDIAN) {
        case WS_LITTLE_ENDIAN:
          wsLog((WS_LOG_MAIN | WS_LOG_PLATFORM), "Byte Order: Little Endian\n");
          break;
        case WS_BIG_ENDIAN:
          wsLog((WS_LOG_MAIN | WS_LOG_PLATFORM), "Byte Order: Big Endian\n");
          break;
        case WS_UNKNOWN_ENDIAN:
          wsLog((WS_LOG_MAIN | WS_LOG_PLATFORM), "Byte Order: Unknown\n");
          break;
    }
    switch (WS_CURRENT_ARCHITECTURE) {
        case WS_ARCH_AMD64:
          wsLog(WS_LOG_PLATFORM, "Architecture: AMD64\n");
          break;
        case WS_ARCH_I32:
          wsLog(WS_LOG_PLATFORM, "Architecture: Intel 32\n");
          break;
        case WS_ARCH_ARM:
          wsLog(WS_LOG_PLATFORM, "Architecture: ARM\n");
          break;
        case WS_ARCH_PPC:
          wsLog(WS_LOG_PLATFORM, "Architecture: PowerPC\n");
          break;
        case WS_ARCH_UNKNOWN:
          wsLog(WS_LOG_PLATFORM, "Architecture: Unknown\n");
          break;
    }
    wsAssert((WS_BIT_ENVIRONMENT == 32 || WS_BIT_ENVIRONMENT == 64),
                    "Only 32-bit and 64-bit systems are supported.");
    wsLog((WS_LOG_MAIN | WS_LOG_PLATFORM), "Environment: %u bits\n", WS_BIT_ENVIRONMENT);
    if (WS_SUPPORTS_SSE4) {
        wsLog(WS_LOG_PLATFORM, "Supports SSE4\n");
    }
    else {
        wsLog(WS_LOG_PLATFORM, "Does not support SSE4\n");
    }
    wsLog(WS_LOG_PLATFORM, "Number of Processor Cores: %u\n", WS_NUM_CORES);
    wsLog(WS_LOG_UTIL, "Generating Lookup Tables, Initializing Random Number Generator\n");
    wsBenchmarkBegin();
    genLookupTables();
    wsBuildCRC32HashTable();
    wsInitRandomizer( wsGetTime() );
    wsLog(WS_LOG_UTIL, "Benchmarked at: %f\n", wsBenchmarkEnd());

    /*  Begin Starting Up Engine Subsystems  */
    wsMem.startUp(mainMem, frameStackMem);
#ifdef _PROFILE
    wsProfiles.startUp(53);
#endif
    wsThreads.startUp();
    wsScreens.startUp(title, width, height, fullscreen);
    wsRenderer.startUp();
    wsSounds.startUp();
    wsEvents.startUp();
    wsInputs.startUp();
    wsGame.startUp();
}

void wsQuit() {
    wsLog(WS_LOG_MAIN, "Shutting Down Whipstitch Engine\n");
    /*  Shut Down Engine Subsystems in reverse order of StartUp  */
    wsGame.shutDown();
    wsInputs.shutDown();
    wsEvents.shutDown();
    wsSounds.shutDown();
    wsRenderer.shutDown();
    wsScreens.shutDown();
    wsThreads.shutDown();
#ifdef _PROFILE
    wsProfiles.shutDown();
#endif
    wsMem.shutDown();
    wsLog(WS_LOG_MAIN, "Whipstitch Engine Shut Down Successfully. G'Bye.");
}


