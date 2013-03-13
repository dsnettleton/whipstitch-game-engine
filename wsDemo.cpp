/**
 *  wsDemo.cpp
 *  March 1, 2013
 *  D. Scott Nettleton
 *
 *  This file implements the class wsDemo, which serves as an example
 *  of how to implement a game in the Whipstitch game engine.
 *
 *  This class inherits the class wsGame, and implements all operations
 *  specific to this particular game.
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

/*  Operational Methods */

void wsDemo::handleButtonEvents(u32 btnHash, u32 action) {
  if (btnHash == wsHash("Test Button") && action == WS_RELEASE) {
    wsSounds.playSound("Click");
    wsRenderer.nextRenderMode();
  }
}

void wsDemo::handleControllerEvents(u64 controllerNum, u64 btnIndex, u32 action, f32 analogVal) {
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
          //*
          const char* anim = scene->getModel("Griswald")->getCurrentAnimationName();
          if (strcmp(anim, "Walk") == 0) {
            scene->beginAnimation("Griswald", "Idle");
          }
          else if (strcmp(anim, "Idle") == 0) {
            scene->beginAnimation("Griswald", "Jump");
          }
          else if (strcmp(anim, "Jump") == 0) {
            scene->beginAnimation("Griswald", "Walk");
          }
          //*/
        }
        break;
      case WS_BUTTON_START:
        if (action  == WS_PRESS) {
          if (animationsPaused) {
            scene->continueAnimations();
            animationsPaused = false;
          }
          else {
            scene->pauseAnimations();
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
          cam->orbit(vec4(0.0f, 9.0f), Y_AXIS, -analogVal*rotationSpeed);
        }
        break;
      case WS_ANALOG_L_AXIS_Y:
        if (analogVal) {
          f32 rotationSpeed = 4.0f;
          cam->orbit(vec4(0.0f, 9.0f), cam->getRightDir(), -analogVal*rotationSpeed);
        }
        break;
      default:
        break;
    }
  }
}

void wsDemo::handleKeyboardEvents(u64 keyType, u64 btnIndex, u32 action) {
  wsLog(WS_LOG_MAIN, "Key Input %lu\n", keyType);
  switch (keyType) {
    case WS_KB_KEY: //  Regular Keys
      switch (btnIndex) {
        case WS_KEY_A:
          if (action == WS_PRESS) {
            const char* anim = scene->getModel("Griswald")->getCurrentAnimationName();
            if (strcmp(anim, "Walk") == 0) {
              scene->beginAnimation("Griswald", "Idle");
            }
            else if (strcmp(anim, "Idle") == 0) {
              scene->beginAnimation("Griswald", "Jump");
            }
            else if (strcmp(anim, "Jump") == 0) {
              scene->beginAnimation("Griswald", "Walk");
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
              scene->continueAnimations();
              animationsPaused = false;
            }
            else {
              scene->pauseAnimations();
              animationsPaused = true;
            }
          }
          break;
        case WS_KEY_R:
          if (action == WS_PRESS) {
            wsRenderer.nextRenderMode();
          }
          break;
        case WS_KEY_Z:
          if (action == WS_PRESS) {
            if (wsRenderer.isEnabled(WS_DRAW_ANTIALIAS)) {
              wsRenderer.disable(WS_DRAW_ANTIALIAS);
            }
            else {
              wsRenderer.enable(WS_DRAW_ANTIALIAS);
            }
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

void wsDemo::handleMouseButtonEvents(u64 action, u64 btnIndex) {
  // switch (btnIndex) {
  //   case WS_MOUSE_BUTTON_LEFT:
  //     if (action  == WS_PRESS) {
  //       // wsRenderer.nextRenderMode();
  //     }
  //     break;
  //   case WS_MOUSE_BUTTON_RIGHT:
  //     if (action == WS_PRESS) {
  //       // wsSounds.playSound("Click");
  //     }
  //     break;
  //   default:
  //     break;
  // }
}

void wsDemo::handleMouseMotionEvents(i32 posX, i32 posY, f32 dx, f32 dy) {
  if (wsInputs.getMouseDown(WS_MOUSE_BUTTON_RIGHT)) {
    f32 rotationSpeed = 0.5f;
    if (dx) {
      cam->orbit(vec4(0.0f, 9.0f, 0.0f), Y_AXIS, -dx*rotationSpeed);
    }
    if (dy) {
      cam->orbit(vec4(0.0f, 9.0f, 0.0f), cam->getRightDir(), -dy*rotationSpeed);
    }
  }
}

/*  Inherited Methods */

void wsDemo::onStart() {
  scene =  wsNew(wsScene, wsScene());
  cam = wsNew(wsCamera, wsCamera("MainCam", vec4(9.0f, 17.0f, 9.0f), vec4(0.0f, 0.0f, 1.0f), vec4(0.0f, 1.0f, 0.0f),
    vec4(0.0f, 0.0f, wsScreenWidth, wsScreenHeight), WS_CAMERA_MODE_PERSP, 60.0f, wsScreenWidth/wsScreenHeight, 0.01f, 100.0f));
  cam->lookAt(vec4(0.0f, 9.0f, 0.0f));
  scene->addCamera(cam);

  wsRenderer.setClearColor(0.4f, 0.6f, 0.4f, 1.0f);
  wsMesh* griswald = wsNew(wsMesh, wsMesh("models/Griswald.wsMesh"));
  wsMesh* bladeWand = wsNew(wsMesh, wsMesh("models/bladeWand.wsMesh"));
  wsMesh* blueBox = wsNew(wsMesh, wsMesh("models/blueBox.wsMesh"));
  wsModel* Griswald = wsNew(wsModel, wsModel("Griswald", griswald, 7));
  wsModel* Griswald2 = wsNew(wsModel, wsModel("Griswald2", griswald, 3));
  wsModel* BladeWand = wsNew(wsModel, wsModel("BladeWand", bladeWand, 0));
  wsModel* BlueBox = wsNew(wsModel, wsModel("Blue Box", blueBox, 0));
  // Griswald->setPos(vec4(-3.0f, 0.0f, 0.0f));
  Griswald2->setPos(vec4(-5.0f, 0.0f, 0.0f));
  BlueBox->setPos(vec4(5.0f, 0.0f, 0.0f));
  wsSound* Click = wsNew(wsSound, wsSound("sounds/btnClick.wav"));
  wsMusic* Resistors = wsNew(wsMusic, wsMusic("sounds/music/07. We're the Resistors.ogg"));
  wsSounds.addSound("Click", Click);
  wsSounds.addMusic("Resistors", Resistors);
  scene->addModel(Griswald);
  scene->addModel(Griswald2);
  scene->addModel(BladeWand);
  scene->addModel(BlueBox);
  wsAnimation* anim_walk = wsNew(wsAnimation, wsAnimation("models/Walk.wsAnim"));
  wsAnimation* anim_idle = wsNew(wsAnimation, wsAnimation("models/Idle.wsAnim"));
  wsAnimation* anim_jump = wsNew(wsAnimation, wsAnimation("models/Jump.wsAnim"));
  scene->addAnimation(anim_walk, "Griswald");
  scene->addAnimation(anim_walk, "Griswald2");
  scene->addAnimation(anim_idle, "Griswald");
  scene->addAnimation(anim_jump, "Griswald");
  wsLog("Animation Name = \"%s\"", anim_idle->getName());
  scene->beginAnimation("Griswald", "Idle");
  scene->beginAnimation("Griswald2", "Walk");
  // Griswald->setTimeScale(0.5f);
  wsFont* fntUbuntu = wsNew(wsFont, wsFont("/home/dsnettleton/Documents/Programming/Eclipse/workspace/Florin/fonts/Ubuntu-B.ttf", 30));
  wsText* txtHello = wsNew(wsText, wsText(vec4(16, 240, 1024, 256), "Hello World!", fntUbuntu, 0, WS_HUD_VISIBLE));
  wsText* txtLine2 = wsNew(wsText, wsText(vec4(16, 204, 1024, 256), "ABCDEFGHIJKLMNOPQRSTUVWXYZ 0123456789", fntUbuntu, 1, WS_HUD_VISIBLE));
  wsText* txtLine3 = wsNew(wsText, wsText(vec4(16, 168, 1024, 256), "abcdefghijklmnopqrstuvwxyz", fntUbuntu, 2, WS_HUD_VISIBLE));
  txtHello->setColor(vec4(0.0f, 1.0f, 1.0f, 1.0f));
  txtLine2->setColor(vec4(0.945f, 0.69f, 0.114f, 1.0f));
  txtLine3->setColor(txtLine2->getColor());

  //scene->setScale("Griswald", 2.0f);
  //scene->setScale("BladeWand", 2.0f);
  // BladeWand->setPos( vec4(0.0f, 8.0f, 2.0f) );
  // BladeWand->setRotation( quat(0.0f, 0.0f, 0.707f, 0.707f) );
  Griswald->attachModel(BladeWand,"tag_Hand.r");

  wsPanel* testPanel = wsNew(wsPanel, wsPanel(vec4(10,10,800,450), 1, "test2.png"));
  wsPanel* testPanel2 = wsNew(wsPanel, wsPanel(vec4(30, 260, 800,450), 0, "test.png"));
  wsPanel* textBlock = wsNew(wsPanel, wsPanel(vec4(288, 64, 1024, 256), 5, "textBlock.png", WS_HUD_VISIBLE));
  textBlock->addElement(txtHello);
  textBlock->addElement(txtLine2);
  textBlock->addElement(txtLine3);

  wsRenderer.addPanel("TestPanel", testPanel);
  wsRenderer.addPanel("TestPanel2", testPanel2);
  wsRenderer.addPanel("Text Block", textBlock);

  wsButton* testButton = wsNew(wsButton, wsButton("Test Button", vec4(15, 15, 64, 64), 10, "btnTemplate.png", WS_HUD_VISIBLE));
  textBlock->addElement(testButton);

  wsTextBox* testBox = wsNew(wsTextBox, wsTextBox("Test Box", vec4(94, 15, 256, 64), 32, 14, 10, 5, "textBox.png", WS_HUD_VISIBLE));
  textBlock->addElement(testBox);
}

void wsDemo::onLoop() {
  if (quit) {
    wsLoop.exit();
  }
}

void wsDemo::onEvent(const wsEvent& event) {
  switch (event.eventType) {
    case WS_EVENT_INPUT:
      switch (event.eventSubType) {
        case WS_INPUT_TYPE_KEYBOARD:
          //  uVal1 = WS_KB_KEY / WS_KB_SPECIAL_KEY
          //  uVal2 = Button Index
          //  uVal3 = WS_PRESS / WS_RELEASE
          handleKeyboardEvents(event.uVal1, event.uVal2, event.uVal3);
          break;
        case WS_INPUT_TYPE_MOUSE:
          //  uVal1 = WS_PRESS/WS_RELEASE/WS_ANALOG (analog indicates mouse motion)
          switch (event.uVal1) {
            case WS_ANALOG:
              //  uVal2 = posX
              //  uVal3 = posY
              //  fVal1 = dx
              //  fVal2 = dy
              handleMouseMotionEvents(event.uVal2, event.uVal3, event.fVal1, event.fVal2);
              break;
            default:  //  Button press/release
              //  uVal2 = btnIndex
              handleMouseButtonEvents(event.uVal1, event.uVal2);
              break;
          }
          break;
        case WS_INPUT_TYPE_CONTROLLER:
          //  uVal1 = Controller Index
          //  uVal2 = Button Index
          //  uVal3 = WS_PRESS/WS_RELEASE/WS_ANALOG
          //  fVal1 = analog value (if any)
          handleControllerEvents(event.uVal1, event.uVal2, event.uVal3, event.fVal1);
          break;
        default:
          break;
      }
      break;
    case WS_EVENT_PHYSICS:
      break;
    case WS_EVENT_HUD_BUTTON:
      //  uVal1 = button hash
      handleButtonEvents(event.uVal1, event.eventSubType);
      break;
    default:
      break;
  }
}

void wsDemo::onExit() {
}
