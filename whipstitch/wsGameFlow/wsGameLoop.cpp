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
  wsRenderer.drawScene(game->getCurrentScene());
  //  Get the ending time of our state update
  t32 timeDiff = wsGetTime() - beginTime;
  t32 sleepTime = frameDuration - timeDiff;
  if (sleepTime > 0) {  //  Wait so the game won't run too fast
    //wsEcho("Slow down! Sleeping for %f seconds.", sleepTime);
    wsWait(sleepTime);
  }
  else {  //  Update the game as many times as is reasonable to catch up to the rendering.
    while (sleepTime < 0 && framesSkipped < maxFrameSkips) {
      updateGameState();
      sleepTime += frameDuration;
      ++framesSkipped;
    }
  }
  // wsEcho(WS_LOG_MAIN, "Iteration time: %f seconds\n", (wsGetTime() - beginTime));
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
  //  Update physics
  game->getCurrentScene()->updatePhysics(frameDuration);
  handleEvents();
  if (quit) { return; }

  game->getCurrentScene()->updateAnimations(frameDuration);

  wsMem.swapFrames(); //  Swap the current memory buffer on the frame stack
}
