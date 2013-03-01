/*
 * wsGameLoop.cpp
 *
 *  Created on: Dec 23, 2012
 *    Author: D. Scott Nettleton
 *
 *    This file implements the class wsGameLoop, which is the heart of the game engine.
 *    Game loops repeat many times per second, managing timing, drawing, input, etc.
 *    The game loop updates at a default of 60 frames per second, drawing when able,
 *    and updating the world state as many times as is necessary (though no more than
 *    the max number of dropped frames) in order to keep a consistent operating speed.
 *
 *    wsGameLoop is an engine subsytem, and must be initialized via the startUp()
 *    function before it may be used. This is done through the engine startup command
 *    wsInit().
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

#include "wsGameLoop.h"
#include "wsInputManager.h"
#include "../wsAudio/wsSoundManager.h"

wsGameLoop wsLoop;

void wsGameLoop::startUp(f32 framesPerSecond) {
  quit = false;
  paused = true;
  keyPressed = false;
  _mInitialized = true;
  fps = framesPerSecond;
  maxFrameSkips = WS_MAX_FRAME_SKIPS;
  frameDuration = 1.0f / fps;
}

void wsGameLoop::shutDown() {
  //  Nothing to see here.
}

void wsGameLoop::beginGame(wsGame* myGame) {
  wsAssert(_mInitialized, "The object wsGame must be initialized via the startUp() method before use.");
  quit = false;
  paused = false;
  game = myGame;
  while (!quit && !paused) {
    iterateLoop();
  }
}

void wsGameLoop::continueLoop() {
  wsAssert(_mInitialized, "The object wsGame must be initialized via the startUp() method before use.");
  paused = false;
  while (!quit && !paused) {
    iterateLoop();
  }
}

void wsGameLoop::drawGameState() {
  wsAssert(_mInitialized, "The object wsGame must be initialized via the startUp() method before use.");
  /*
  wsRenderer.clearScreen();
  wsRenderer.drawModels();
  wsRenderer.swapBuffers();
  //*/
}

void wsGameLoop::handleEvents() {
  while (wsEvents.isNotEmpty()) {
    wsEvent my = wsEvents.pop();
    game->onEvent(my);
  }
}

void wsGameLoop::handleInputs() {
  wsAssert(_mInitialized, "The object wsGame must be initialized via the startUp() method before use.");
  wsInputs.pollDevices();

  quit = quit || wsScreens.closed();

  wsInputs.swapFrames();
}

void wsGameLoop::iterateLoop() {
  wsAssert(_mInitialized, "The object wsGame must be initialized via the startUp() method before use.");
  //  Get the starting time of our iteration
  t32 beginTime = wsGetTime();
  u32 framesSkipped = 0;
  //  Update the gamestate and draw the game
  updateGameState();
  //drawGameState();
  wsRenderer.drawScene();
  //  Get the ending time of our state update
  t32 timeDiff = wsGetTime() - beginTime;
  t32 sleepTime = frameDuration - timeDiff;
  if (sleepTime > 0) {  //  Wait so the game won't run too fast
    //wsLog("Slow down! Sleeping for %f seconds.", sleepTime);
    wsWait(sleepTime);
  }
  else {  //  Update the game as many times as is reasonable to catch up to the rendering.
    while (sleepTime < 0 && framesSkipped < maxFrameSkips) {
      updateGameState();
      sleepTime += frameDuration;
      ++framesSkipped;
    }
  }
  // wsLog(WS_LOG_MAIN, "Iteration time: %f seconds\n", (wsGetTime() - beginTime));
}

void wsGameLoop::pauseLoop() {
  wsAssert(_mInitialized, "The object wsGame must be initialized via the startUp() method before use.");
  paused = true;
}

void wsGameLoop::updateGameState() {
  wsAssert(_mInitialized, "The object wsGame must be initialized via the startUp() method before use.");

  wsSounds.updateStreams();
  game->onLoop();
  handleInputs();
  handleEvents();
  if (quit) { return; }

  wsRenderer.updateAnimations(frameDuration);

  wsMem.swapFrames(); //  Swap the current memory buffer on the frame stack
}
