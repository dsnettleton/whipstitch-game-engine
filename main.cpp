//  main.cpp
//  D. Scott Nettleton
/*
 *  This is the test file (driver) for the wsMath functions; it will
 *  also be used to generate a file containing tables for quick trig
 *  lookups.
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

#include "wsDemo.h"

int main(int argc, char** argv) {
  #ifdef _PROFILE
    wsActiveLogs = (WS_LOG_PROFILING | WS_LOG_MAIN | WS_LOG_UTIL | WS_LOG_ERROR);
  #else
    // wsActiveLogs = (WS_LOG_MAIN | WS_LOG_ERROR);
    // wsActiveLogs = (WS_LOG_DEBUG | WS_LOG_ERROR | WS_LOG_UTIL | WS_LOG_SHADER);
    // wsActiveLogs = WS_LOG_ALL;
    wsActiveLogs = WS_LOG_MAIN;
  #endif
  wsInit("Whipstitch Game Engine", 1280, 720, false, 512*wsMB, 32*wsMB);  //  512MB, 32MB

  wsDemo* demoGame = wsNew(wsDemo, wsDemo());

  demoGame->onStart();

  wsLoop.beginGame(demoGame);

  demoGame->onExit();

  wsQuit();
  return 0;
}
