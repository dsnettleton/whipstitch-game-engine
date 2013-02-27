/*
 * wsGameLoop.h
 *
 *  Created on: Dec 23, 2012
 *      Author: D. Scott Nettleton
 *
 *      This file declares the class wsGameLoop, which is the heart of the game engine.
 *      Game loops repeat many times per second, managing timing, drawing, input, etc.
 *      The game loop updates at a default of 60 frames per second, drawing when able,
 *      and updating the world state as many times as is necessary (though no more than
 *      the max number of dropped frames) in order to keep a consistent operating speed.
 *
 *      wsGameLoop is an engine subsytem, and must be initialized via the startUp()
 *      function before it may be used. This is done through the engine startup command
 *      wsInit().
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

#ifndef WS_GAME_LOOP_H_
#define WS_GAME_LOOP_H_

#include "../wsUtils.h"
#include "../wsAssets.h"
#include "../wsGraphics.h"
#include "wsEventManager.h"

#define WS_DEFAULT_FPS 60.0
#define WS_MAX_FRAME_SKIPS 5

class wsGameLoop {
    private:
        u32 maxFrameSkips;
        f32 fps;
        t32 frameDuration;
        wsCamera* cam;
        bool quit;
        bool paused;
        bool animationsPaused;
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
        void beginLoop();
        void continueLoop();
        void drawGameState();
        void handleButtonEvents(u32 btnHash, u32 action);
        void handleControllerEvents(u64 controllerNum, u64 btnIndex, u32 action, f32 analogVal);
        void handleKeyboardEvents(u64 keyType, u64 btnIndex, u32 action);
        void handleMouseButtonEvents(u64 action, u64 btnIndex);
        void handleMouseMotionEvents(i32 posX, i32 posY, f32 dx, f32 dy);
        void handleEvents();
        void handleInputs();    //  Temporary until full HID subsystem is in place
        void iterateLoop();
        void pauseLoop();
        void pushEvent(const wsEvent& myEvent) { wsEvents.push(myEvent); }
        void updateGameState();
};

extern wsGameLoop wsGame;

#endif /*   WS_GAME_LOOP_H_ */
