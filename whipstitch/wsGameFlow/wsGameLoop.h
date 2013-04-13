/*
 * wsGameLoop.h
 *
 *  Created on: Dec 23, 2012
 *    Author: D. Scott Nettleton
 *
 *    This file declares the class wsGameLoop, which is the heart of the game engine.
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

#ifndef WS_GAME_LOOP_H_
#define WS_GAME_LOOP_H_

#include "../wsUtils.h"
#include "../wsAssets.h"
#include "../wsGraphics.h"
#include "wsGame.h"
#include "wsEventManager.h"

#define WS_DEFAULT_FPS 60.0
#define WS_MAX_FRAME_SKIPS 5

class wsGameLoop {
  private:
    wsGame* game;
    u32 maxFrameSkips;
    f32 fps;
    t32 frameDuration;
    bool quit;
    bool paused;
    bool keyPressed;
    //  True only when the startUp function has been called
    bool _mInitialized;
  public:
    /*  Empty Constructor and Destructor   */
    //  As an engine subsystem, the game loop takes no action until explicitly
    //  initialized via the startUp(...) function.
    //  uninitialized via the shutDown() function.
    wsGameLoop() : _mInitialized(false) {}
    ~wsGameLoop() {}
    /*  Startup and shutdown functions  */
    //  Uninitializes the game loop
    void shutDown();
    //  Initializes the game loop
    void startUp(f32 framesPerSecond = WS_DEFAULT_FPS);
    /*  Setters and Getters */
    void setFramesPerSecond(f32 framesPerSecond) { fps = framesPerSecond; frameDuration = 1.0f / fps; }
    void setMaxFrameSkips(u32 myMaxSkips) { maxFrameSkips = myMaxSkips; }
    /*  Operational Methods */
    void beginGame(wsGame* myGame);
    void continueLoop();
    void exit() { quit = true; }
    void handleEvents();
    void handleInputs();  //  Temporary until full HID subsystem is in place
    void iterateLoop();
    void pauseLoop();
    void pushEvent(const wsEvent& myEvent) { wsEvents.push(myEvent); }
    void updateGameState();
};

extern wsGameLoop wsLoop;

#endif /*   WS_GAME_LOOP_H_ */
