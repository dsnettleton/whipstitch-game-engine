//  main.cpp
//  D. Scott Nettleton
/*
 *  This is the test file (driver) for the wsMath functions; it will
 *  also be used to generate a file containing tables for quick trig
 *  lookups.
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

#include "wsDemo.h"

int main(int argc, char** argv) {
  #ifdef _PROFILE
    wsActiveLogs = (WS_LOG_PROFILING | WS_LOG_MAIN | WS_LOG_UTIL | WS_LOG_ERROR);
  #else
    wsActiveLogs = (WS_LOG_MAIN | WS_LOG_ERROR);
    // wsActiveLogs = (WS_LOG_DEBUG | WS_LOG_ERROR | WS_LOG_UTIL | WS_LOG_SHADER);
    //wsActiveLogs = WS_LOG_ALL;
    //sActiveLogs = WS_LOG_MAIN;
  #endif
  wsInit("Whipstitch Game Engine", 1280, 720, false, 512*wsMB, 32*wsMB);  //  512MB, 32MB

  wsDemo* demoGame = wsNew(wsDemo, wsDemo());

  demoGame->onStart();

  wsLoop.beginGame(demoGame);

  demoGame->onExit();

  wsQuit();
  return 0;
}
