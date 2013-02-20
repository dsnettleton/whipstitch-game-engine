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
  cam = wsNew(wsCamera, wsCamera("MainCam", vec4(9.0f, 17.0f, 9.0f), vec4(0.0f, 0.0f, 1.0f), vec4(0.0f, 1.0f, 0.0f),
    vec4(0.0f, 0.0f, 1280.0f, 720.0f), WS_CAMERA_MODE_PERSP, 60.0f, 1280.0f/720.0f, 0.01f, 100.0f));
  cam->lookAt(vec4(0.0f, 9.0f, 0.0f));
  wsRenderer.addCamera(cam);
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
  /*
  wsRenderer.clearScreen();
  wsRenderer.drawModels();
  wsRenderer.swapBuffers();
  //*/
}

void wsGameLoop::handleControllerEvents(u64 controllerNum, u64 btnIndex, u32 action, f32 analogVal) {
  if (action != WS_ANALOG) {  //  Digital Button presses
    switch (btnIndex) {
      case WS_BUTTON_BOTTOM:
        if (action  == WS_PRESS) {
          wsRenderer.nextRenderMode();
        }
        break;
      case WS_BUTTON_RIGHT:
        if (action  == WS_PRESS) {
          if (wsRenderer.isEnabled(WS_DRAW_BONES)) {
            wsRenderer.disable(WS_DRAW_BONES);
          }
          else {
            wsRenderer.enable(WS_DRAW_BONES);
          }
        }
        break;
      case WS_BUTTON_LEFT:
        if (action == WS_PRESS) {
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
        break;
      case WS_BUTTON_START:
        if (action  == WS_PRESS) {
          if (animationsPaused) {
            wsRenderer.continueAnimations();
            animationsPaused = false;
          }
          else {
            wsRenderer.pauseAnimations();
            animationsPaused = true;
          }
        }
        break;
      case WS_BUTTON_SYSTEM:
        if (action == WS_PRESS) {
          quit = true;
        }
        break;
      default:
        break;
    }
  }
  else {  //  Analog controls
    switch (btnIndex) {
      case WS_ANALOG_L_AXIS_X:
        if (analogVal) {
          f32 rotationSpeed = 4.0f;
          //glRotatef(-analogVal*rotationSpeed, 0.0f, 1.0f, 0.0f);
          cam->orbit(vec4(0.0f, 9.0f), Y_AXIS, -analogVal*rotationSpeed);
        }
        break;
      case WS_ANALOG_L_AXIS_Y:
        if (analogVal) {
          f32 rotationSpeed = 4.0f;
          //glRotatef(-analogVal*rotationSpeed, 0.0f, 1.0f, 0.0f);
          cam->orbit(vec4(0.0f, 9.0f), cam->getRightDir(), -analogVal*rotationSpeed);
        }
        break;
      default:
        break;
    }
  }
}

void wsGameLoop::handleKeyboardEvents(u64 keyType, u64 btnIndex, u32 action) {
  wsLog(WS_LOG_MAIN, "Key Input %lu\n", keyType);
  switch (keyType) {
    case WS_KB_KEY: //  Regular Keys
      switch (btnIndex) {
        case WS_KEY_A:
          if (action == WS_PRESS) {
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
          break;
        case WS_KEY_B:
          if (action == WS_PRESS) {
            if (wsRenderer.isEnabled(WS_DRAW_BONES)) {
              wsRenderer.disable(WS_DRAW_BONES);
            }
            else {
              wsRenderer.enable(WS_DRAW_BONES);
            }
          }
          break;
        case WS_KEY_C:
          if (action == WS_PRESS) {
            if (wsRenderer.isEnabled(WS_DRAW_CEL)) {
              wsRenderer.disable(WS_DRAW_CEL);
            }
            else {
              wsRenderer.enable(WS_DRAW_CEL);
            }
          }
          break;
        case WS_KEY_M:
          if (action == WS_PRESS) {
            wsSounds.playMusic("Resistors");
          }
          break;
        case WS_KEY_O:
          if (action == WS_PRESS) {
            if (wsRenderer.isEnabled(WS_DRAW_OUTLINE)) {
              wsRenderer.disable(WS_DRAW_OUTLINE);
            }
            else {
              wsRenderer.enable(WS_DRAW_OUTLINE);
            }
          }
          break;
        case WS_KEY_P:
          if (action == WS_PRESS) {
            if (animationsPaused) {
              wsRenderer.continueAnimations();
              animationsPaused = false;
            }
            else {
              wsRenderer.pauseAnimations();
              animationsPaused = true;
            }
          }
          break;
        case WS_KEY_R:
          if (action == WS_PRESS) {
            wsRenderer.nextRenderMode();
          }
          break;
        case WS_KEY_LEFT:
          cam->orbit(vec4(0.0f, 9.0f, 0.0f), Y_AXIS, -3.0f);
          break;
        case WS_KEY_RIGHT:
          cam->orbit(vec4(0.0f, 9.0f, 0.0f), Y_AXIS, 3.0f);
          break;
        case WS_KEY_DOWN:
          cam->orbit(vec4(0.0f, 9.0f, 0.0f), cam->getRightDir(), 3.0f);
          break;
        case WS_KEY_UP:
          cam->orbit(vec4(0.0f, 9.0f, 0.0f), cam->getRightDir(), -3.0f);
          break;
        default:
          break;
      }
      break;
    case WS_KB_SPECIAL_KEY: //  Special Keys
      switch (btnIndex) {
        case WS_KEY_ESC:
          if (action == WS_PRESS) {
            quit = true;
          }
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

void wsGameLoop::handleMouseButtonEvents(u64 action, u64 btnIndex) {
  switch (btnIndex) {
    case WS_MOUSE_BUTTON_LEFT:
      if (action  == WS_PRESS) {
        wsRenderer.nextRenderMode();
      }
      break;
    case WS_MOUSE_BUTTON_RIGHT:
      if (action == WS_PRESS) {
        wsSounds.playSound("Click");
      }
      break;
    default:
      break;
  }
}

void wsGameLoop::handleMouseMotionEvents(i32 posX, i32 posY, f32 dx, f32 dy) {
  f32 rotationSpeed = 0.5f;
  if (dx) {
    cam->orbit(vec4(0.0f, 9.0f, 0.0f), Y_AXIS, -dx*rotationSpeed);
  }
  if (dy) {
    cam->orbit(vec4(0.0f, 9.0f, 0.0f), cam->getRightDir(), -dy*rotationSpeed);
    // cam->getRightDir().print(WS_LOG_MAIN);
  }
}

void wsGameLoop::handleEvents() {
  while (wsEvents.isNotEmpty()) {
    wsEvent my = wsEvents.pop();
    switch (my.eventType) {
      case WS_EVENT_INPUT:
        switch (my.eventSubType) {
          case WS_INPUT_TYPE_KEYBOARD:
            //  uVal1 = WS_KB_KEY / WS_KB_SPECIAL_KEY
            //  uVal2 = Button Index
            //  uVal3 = WS_PRESS / WS_RELEASE
            handleKeyboardEvents(my.uVal1, my.uVal2, my.uVal3);
            break;
          case WS_INPUT_TYPE_MOUSE:
            //  uVal1 = WS_PRESS/WS_RELEASE/WS_ANALOG (analog indicates mouse motion)
            switch (my.uVal1) {
              case WS_ANALOG:
                //  uVal2 = posX
                //  uVal3 = posY
                //  fVal1 = dx
                //  fVal2 = dy
                handleMouseMotionEvents(my.uVal2, my.uVal3, my.fVal1, my.fVal2);
                break;
              default:  //  Button press/release
                //  uVal2 = btnIndex
                handleMouseButtonEvents(my.uVal1, my.uVal2);
                break;
            }
            break;
          case WS_INPUT_TYPE_CONTROLLER:
            //  uVal1 = Controller Index
            //  uVal2 = Button Index
            //  uVal3 = WS_PRESS/WS_RELEASE/WS_ANALOG
            //  fVal1 = analog value (if any)
            handleControllerEvents(my.uVal1, my.uVal2, my.uVal3, my.fVal1);
            break;
          default:
            break;
        }
        break;
      case WS_EVENT_PHYSICS:
        break;
      default:
        break;
    }
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

  handleInputs();
  handleEvents();
  if (quit) { return; }

  wsRenderer.updateAnimations(frameDuration);

  wsMem.swapFrames(); //  Swap the current memory buffer on the frame stack
}
