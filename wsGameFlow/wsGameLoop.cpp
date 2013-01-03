/*
 * wsGameLoop.cpp
 *
 *  Created on: Dec 23, 2012
 *      Author: D. Scott Nettleton
 *
 *      This file implements the class wsGameLoop, which is the heart of the game engine.
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

#include "wsGameLoop.h"

wsGameLoop wsGame;

void wsGameLoop::startUp(f32 framesPerSecond) {
    quit = false;
    paused = true;
    keyPressed = false;
    animationsPaused = false;
    _mInitialized = true;
    fps = framesPerSecond;
    maxFrameSkips = WS_MAX_FRAME_SKIPS;
    frameDuration = 1.0f / fps;
}

void wsGameLoop::shutDown() {
    //  Nothing to see here.
}

void wsGameLoop::beginLoop() {
    wsAssert(_mInitialized, "The object wsGame must be initialized via the startUp() method before use.");
    quit = false;
    paused = false;
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
//    f32 rotation = 1.0f;
    wsRenderer.clearScreen();

//    glRotatef(rotation, 0.0f, 1.0f, 0.0f);
    //wsRenderer.drawMeshes();
    wsRenderer.drawModels();
    wsRenderer.swapBuffers();
}

void wsGameLoop::handleInputs() {
    wsAssert(_mInitialized, "The object wsGame must be initialized via the startUp() method before use.");
    /*
    class Task_checkGLFW : public wsTask {
        public:
            bool quit;
            bool keyPressed;
            bool animationsPaused;
            Task_checkGLFW() : quit(false), keyPressed(false), animationsPaused(false) {}
            void run(u32 threadNum) {
                glfwPollEvents();
                bool quit = glfwGetKey( GLFW_KEY_ESC ) || !glfwGetWindowParam( GLFW_OPENED);
                if (quit) { return; }
                if (glfwGetKey('a') || glfwGetKey('A')) {
                    if (!keyPressed) {
                        const char* anim = wsRenderer.getModel("Griswald")->getCurrentAnimation();
                        if (strcmp(anim, "Walk") == 0) {
                            wsRenderer.beginAnimation("Griswald", "Idle");
                        }
                        else if (strcmp(anim, "Idle") == 0) {
                            wsRenderer.beginAnimation("Griswald", "Jump");
                        }
                        else if (strcmp(anim, "Jump") == 0) {
                            wsRenderer.beginAnimation("Griswald", "Walk");
                        }
                    }
                    keyPressed = true;
                }
                else if (glfwGetKey('b') || glfwGetKey('B')) {
                    if (!keyPressed) {
                        if (wsRenderer.getDrawBones()) {
                            wsRenderer.disable(WS_RENDER_BONES);
                        }
                        else {
                            wsRenderer.enable(WS_RENDER_BONES);
                        }
                    }
                    keyPressed = true;
                }
                else if (glfwGetKey('p') || glfwGetKey('P')) {
                    if (!keyPressed) {
                        if (animationsPaused) {
                            wsRenderer.continueAnimations();
                            animationsPaused = false;
                        }
                        else {
                            wsRenderer.pauseAnimations();
                            animationsPaused = true;
                        }
                    }
                    keyPressed = true;
                }
                else if (glfwGetKey('r') || glfwGetKey('R')) {
                    if (!keyPressed) {
                        if (wsRenderer.getRenderMode() == WS_RENDER_MODE_CEL) {
                            wsRenderer.setRenderMode(WS_RENDER_MODE_LIT);
                        }
                        else {
                            wsRenderer.setRenderMode(WS_RENDER_MODE_CEL);
                        }
                    }
                    keyPressed = true;
                }
                else {
                    keyPressed = false;
                }
            }
    };
    wsTasks.push(Task_checkGLFW());
    /*/
    glfwPollEvents();
    quit = glfwGetKey( GLFW_KEY_ESC ) || !glfwGetWindowParam( GLFW_OPENED);
    if (quit) { return; }
    if (glfwGetKey('a') || glfwGetKey('A')) {
        if (!keyPressed) {
            const char* anim = wsRenderer.getModel("Griswald")->getCurrentAnimation();
            if (strcmp(anim, "Walk") == 0) {
                wsRenderer.beginAnimation("Griswald", "Idle");
            }
            else if (strcmp(anim, "Idle") == 0) {
                wsRenderer.beginAnimation("Griswald", "Jump");
            }
            else if (strcmp(anim, "Jump") == 0) {
                wsRenderer.beginAnimation("Griswald", "Walk");
            }
        }
        keyPressed = true;
    }
    else if (glfwGetKey('b') || glfwGetKey('B')) {
        if (!keyPressed) {
            if (wsRenderer.getDrawBones()) {
                wsRenderer.disable(WS_RENDER_BONES);
            }
            else {
                wsRenderer.enable(WS_RENDER_BONES);
            }
        }
        keyPressed = true;
    }
    else if (glfwGetKey('p') || glfwGetKey('P')) {
        if (!keyPressed) {
            if (animationsPaused) {
                wsRenderer.continueAnimations();
                animationsPaused = false;
            }
            else {
                wsRenderer.pauseAnimations();
                animationsPaused = true;
            }
        }
        keyPressed = true;
    }
    else if (glfwGetKey('r') || glfwGetKey('R')) {
        if (!keyPressed) {
            if (wsRenderer.getRenderMode() == WS_RENDER_MODE_CEL) {
                wsRenderer.setRenderMode(WS_RENDER_MODE_LIT);
            }
            else {
                wsRenderer.setRenderMode(WS_RENDER_MODE_CEL);
            }
        }
        keyPressed = true;
    }
    else {
        keyPressed = false;
    }
    //*/
}

void wsGameLoop::iterateLoop() {
    wsAssert(_mInitialized, "The object wsGame must be initialized via the startUp() method before use.");
    //  Get the starting time of our iteration
    t32 beginTime = wsGetTime();
    u32 framesSkipped = 0;
    //  Update the gamestate and draw the game
    updateGameState();
    drawGameState();
    //  Get the ending time of our state update
    t32 timeDiff = wsGetTime() - beginTime;
    t32 sleepTime = frameDuration - timeDiff;
    if (sleepTime > 0) {    //  Wait so the game won't run too fast
        wsWait(sleepTime);
    }
    else {  //  Update the game as many times as is reasonable to catch up to the rendering.
        while (sleepTime < 0 && framesSkipped < maxFrameSkips) {
            updateGameState();
            sleepTime += frameDuration;
            ++framesSkipped;
        }
    }
}

void wsGameLoop::pauseLoop() {
    wsAssert(_mInitialized, "The object wsGame must be initialized via the startUp() method before use.");
    paused = true;
}

void wsGameLoop::updateGameState() {
    wsAssert(_mInitialized, "The object wsGame must be initialized via the startUp() method before use.");
    handleInputs();
    if (quit) { return; }

    wsRenderer.updateAnimations(frameDuration);

    wsMem.swapFrames(); //  Swap the current memory buffer on the frame stack
}
