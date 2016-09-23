/*
 * ws.cpp
 *
 *  Created on: Jul 12, 2012
 *    Author: dsnettleton
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

#include "ws.h"

void wsInit(const char* title, const i32 width, const i32 height, bool fullscreen, u64 mainMem, u32 frameStackMem) {
  wsAssert(wsFile::exists(ws_path_cwd), "Current Working Directory could not be determined.");
  wsAssert(wsFile::exists(ws_path_home), "Current Home Directory could not be determined.");
  if (!exists(ws_path_log_dir)) {
    #ifndef NDEBUG
      wsAssert(wsFile::create_directory(ws_path_log_dir), "Could not create Log directory");
    #else
      wsFile::create_directory(ws_path_log_dir);
    #endif
  }
  wsEcho("Initializing Whipstitch Engine\n");
  wsEcho("  CWD =  %s\n  HOME = %s\n  LOGS = %s\n", ws_path_cwd.string().c_str(), ws_path_home.string().c_str(), ws_path_log_dir.string().c_str());
  switch (WS_CURRENT_MODE) {
    case WS_MODE_DEBUG:
      wsEcho((WS_LOG_MAIN | WS_LOG_PLATFORM), "MODE = DEBUG\n");
      break;
    case WS_MODE_PROFILE:
      wsEcho((WS_LOG_MAIN | WS_LOG_PLATFORM), "MODE = PROFILE\n");
      break;
    case WS_MODE_GAME:
      wsEcho((WS_LOG_MAIN | WS_LOG_PLATFORM), "MODE = RELEASE\n");
      break;
  }
  switch (WS_CURRENT_OS) {
    case WS_OS_LINUX:
      wsEcho((WS_LOG_MAIN | WS_LOG_PLATFORM), "Platform = Linux\n");
      break;
    case WS_OS_WINDOWS:
      wsEcho((WS_LOG_MAIN | WS_LOG_PLATFORM), "Platform = Windows\n");
      break;
    case WS_OS_UNIX:
      wsEcho((WS_LOG_MAIN | WS_LOG_PLATFORM), "Platform = Unix\n");
      break;
    case WS_OS_MAC_OSX:
      wsEcho((WS_LOG_MAIN | WS_LOG_PLATFORM), "Plaform = Mac OSX\n");
      break;
    default:
    case WS_OS_UNKNOWN:
      wsEcho((WS_LOG_MAIN | WS_LOG_PLATFORM), "Platform = Unknown\n");
      break;
  }
  switch (WS_CURRENT_ENDIAN) {
    case WS_LITTLE_ENDIAN:
      wsEcho((WS_LOG_MAIN | WS_LOG_PLATFORM), "Byte Order: Little Endian\n");
      break;
    case WS_BIG_ENDIAN:
      wsEcho((WS_LOG_MAIN | WS_LOG_PLATFORM), "Byte Order: Big Endian\n");
      break;
    case WS_UNKNOWN_ENDIAN:
      wsEcho((WS_LOG_MAIN | WS_LOG_PLATFORM), "Byte Order: Unknown\n");
      break;
  }
  switch (WS_CURRENT_ARCHITECTURE) {
    case WS_ARCH_AMD64:
      wsEcho(WS_LOG_PLATFORM, "Architecture: AMD64\n");
      break;
    case WS_ARCH_I32:
      wsEcho(WS_LOG_PLATFORM, "Architecture: Intel 32\n");
      break;
    case WS_ARCH_ARM:
      wsEcho(WS_LOG_PLATFORM, "Architecture: ARM\n");
      break;
    case WS_ARCH_PPC:
      wsEcho(WS_LOG_PLATFORM, "Architecture: PowerPC\n");
      break;
    case WS_ARCH_UNKNOWN:
      wsEcho(WS_LOG_PLATFORM, "Architecture: Unknown\n");
      break;
  }
  wsAssert((WS_BIT_ENVIRONMENT == 32 || WS_BIT_ENVIRONMENT == 64), "Only 32-bit and 64-bit systems are supported.");
  wsEcho((WS_LOG_MAIN | WS_LOG_PLATFORM), "Environment: %u bits\n", WS_BIT_ENVIRONMENT);
  if (WS_SUPPORTS_SSE4) {
    wsEcho(WS_LOG_PLATFORM, "Supports SSE4\n");
  }
  else {
    wsEcho(WS_LOG_PLATFORM, "Does not support SSE4\n");
  }
  wsEcho(WS_LOG_PLATFORM, "Number of Processor Cores: %u\n", WS_NUM_CORES);
  wsEcho(WS_LOG_UTIL, "Generating Lookup Tables, Initializing Random Number Generator\n");
  wsBenchmarkBegin();
  genLookupTables();
  wsBuildCRC32HashTable();
  wsInitRandomizer( wsGetTime() );
  wsEcho(WS_LOG_UTIL, "Benchmarked at: %f\n", wsBenchmarkEnd());

  /*  Begin Starting Up Engine Subsystems  */
  wsMem.startUp(mainMem, frameStackMem);
#ifdef _PROFILE
  wsProfiles.startUp(53);
#endif
  //wsThreads.startUp();
  wsScreenWidth = (u32)width;
  wsScreenHeight = (u32)height;
  wsScreens.startUp(title, width, height, fullscreen);
  wsRenderer.startUp();
  wsSounds.startUp();
  wsNetworking.startUp();
  wsEvents.startUp();
  wsInputs.startUp();
  wsLoop.startUp();
}

void wsBegin(wsGame* myGame) {
  myGame->onStart();
  wsLoop.beginGame(myGame);
  myGame->onExit();
  wsQuit();
}

void wsQuit() {
  wsEcho(WS_LOG_MAIN, "Shutting Down Whipstitch Engine\n");
  /*  Shut Down Engine Subsystems in reverse order of StartUp  */
  wsLoop.shutDown();
  wsInputs.shutDown();
  wsEvents.shutDown();
  wsNetworking.shutDown();
  wsSounds.shutDown();
  wsRenderer.shutDown();
  wsScreens.shutDown();
  //wsThreads.shutDown();
#ifdef _PROFILE
  wsProfiles.shutDown();
#endif
  wsMem.shutDown();
  wsEcho(WS_LOG_MAIN, "Whipstitch Engine Shut Down Successfully. G'Bye.");
}


