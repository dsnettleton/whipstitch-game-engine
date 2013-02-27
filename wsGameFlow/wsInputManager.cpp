/*
 *  wsInputManager.h
 *
 *  Created on: Jan 1, 2013
 *    Author: dsnettleton
 *
 *    This file declares the class wsInputManager, which manages multiple human
 *    interface devices, queueing their inputs for use by the engine.
 *
 *    wsInputManager is an engine subsytem, and must be initialized via the startUp()
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

#include "wsInputManager.h"

#include "../wsGraphics/wsRenderSystem.h"

wsInputManager wsInputs;
u64 wsKeyboardMap[GLFW_KEY_LAST+1];
bool wsSpecialKeys[GLFW_KEY_LAST+1];
u32 wsMouseButtonMap[GLFW_MOUSE_BUTTON_LAST+1];

void wsInputManager::startUp() {
  _mInitialized = true;
  #if (WS_SCREEN_BACKEND == WS_BACKEND_X11)
  #elif (WS_SCREEN_BACKEND == WS_BACKEND_GLFW)
    for (u32 i = 0; i <= GLFW_KEY_LAST; ++i) {
      wsKeyboardMap[i] = WS_KEYS_NONE;
      wsSpecialKeys[i] = false;
    }
    initKeyboardMaps();
    initMouseButtonMap();
    glfwSetKeyCallback(wsKeyboardCallback);
    glfwEnable(GLFW_KEY_REPEAT);
    glfwSetMousePosCallback(wsMouseMotionCallback);
    glfwSetMouseButtonCallback(wsMouseButtonCallback);
  #endif
  numControllers = 0;
  pollDevices();
  wsLog(WS_LOG_HID, "Number of Controllers: %u\n", numControllers);
}

#if (WS_SCREEN_BACKEND == WS_BACKEND_X11)
#elif (WS_SCREEN_BACKEND == WS_BACKEND_GLFW)
  //*
  void initKeyboardMaps() {
    wsKeyboardMap['A'] = WS_KEY_A;
    wsKeyboardMap['B'] = WS_KEY_B;
    wsKeyboardMap['C'] = WS_KEY_C;
    wsKeyboardMap['D'] = WS_KEY_D;
    wsKeyboardMap['E'] = WS_KEY_E;
    wsKeyboardMap['F'] = WS_KEY_F;
    wsKeyboardMap['G'] = WS_KEY_G;
    wsKeyboardMap['H'] = WS_KEY_H;
    wsKeyboardMap['I'] = WS_KEY_I;
    wsKeyboardMap['J'] = WS_KEY_J;
    wsKeyboardMap['K'] = WS_KEY_K;
    wsKeyboardMap['L'] = WS_KEY_L;
    wsKeyboardMap['M'] = WS_KEY_M;
    wsKeyboardMap['N'] = WS_KEY_N;
    wsKeyboardMap['O'] = WS_KEY_O;
    wsKeyboardMap['P'] = WS_KEY_P;
    wsKeyboardMap['Q'] = WS_KEY_Q;
    wsKeyboardMap['R'] = WS_KEY_R;
    wsKeyboardMap['S'] = WS_KEY_S;
    wsKeyboardMap['T'] = WS_KEY_T;
    wsKeyboardMap['U'] = WS_KEY_U;
    wsKeyboardMap['V'] = WS_KEY_V;
    wsKeyboardMap['W'] = WS_KEY_W;
    wsKeyboardMap['X'] = WS_KEY_X;
    wsKeyboardMap['Y'] = WS_KEY_Y;
    wsKeyboardMap['Z'] = WS_KEY_Z;
    wsKeyboardMap['0'] = WS_KEY_0;
    wsKeyboardMap['1'] = WS_KEY_1;
    wsKeyboardMap['2'] = WS_KEY_2;
    wsKeyboardMap['3'] = WS_KEY_3;
    wsKeyboardMap['4'] = WS_KEY_4;
    wsKeyboardMap['5'] = WS_KEY_5;
    wsKeyboardMap['6'] = WS_KEY_6;
    wsKeyboardMap['7'] = WS_KEY_7;
    wsKeyboardMap['8'] = WS_KEY_8;
    wsKeyboardMap['9'] = WS_KEY_9;
    wsKeyboardMap['='] = WS_KEY_EQUALS;
    wsKeyboardMap['_'] = WS_KEY_UNDERSCORE;
    wsKeyboardMap['~'] = WS_KEY_TILDE;
    wsKeyboardMap['['] = WS_KEY_BRACKET_L;
    wsKeyboardMap[']'] = WS_KEY_BRACKET_R;
    wsKeyboardMap[';'] = WS_KEY_SEMICOLON;
    wsKeyboardMap['"'] = WS_KEY_APOSTROPHE;
    wsKeyboardMap['<'] = WS_KEY_COMMA;
    wsKeyboardMap['>'] = WS_KEY_PERIOD;
    wsKeyboardMap['/'] = WS_KEY_SLASH_FWD;
    wsKeyboardMap['\\'] = WS_KEY_SLASH_BKWD;
    wsKeyboardMap[GLFW_KEY_SPACE] = WS_KEY_SPACE;
    wsKeyboardMap[GLFW_KEY_TAB] = WS_KEY_TAB;
    wsKeyboardMap[GLFW_KEY_LSHIFT] = WS_KEY_SHIFT_L;
    wsKeyboardMap[GLFW_KEY_RSHIFT] = WS_KEY_SHIFT_R;
    wsKeyboardMap[GLFW_KEY_LCTRL] = WS_KEY_CTRL_L;
    wsKeyboardMap[GLFW_KEY_RCTRL] = WS_KEY_CTRL_R;
    wsKeyboardMap[GLFW_KEY_LALT] = WS_KEY_ALT_L;
    wsKeyboardMap[GLFW_KEY_RALT] = WS_KEY_ALT_R;
    wsKeyboardMap[GLFW_KEY_ENTER] = WS_KEY_ENTER;
    wsKeyboardMap[GLFW_KEY_BACKSPACE] = WS_KEY_BACKSPACE;
    wsKeyboardMap[GLFW_KEY_DEL] = WS_KEY_DELETE;
    wsKeyboardMap[GLFW_KEY_DOWN] = WS_KEY_DOWN;
    wsKeyboardMap[GLFW_KEY_LEFT] = WS_KEY_LEFT;
    wsKeyboardMap[GLFW_KEY_RIGHT] = WS_KEY_RIGHT;
    wsKeyboardMap[GLFW_KEY_UP] = WS_KEY_UP;
    wsKeyboardMap[GLFW_KEY_F1] = WS_KEY_F1;
    wsSpecialKeys[GLFW_KEY_F1] = true;
    wsKeyboardMap[GLFW_KEY_F2] = WS_KEY_F2;
		wsSpecialKeys[GLFW_KEY_F2] = true;
    wsKeyboardMap[GLFW_KEY_F3] = WS_KEY_F3;
		wsSpecialKeys[GLFW_KEY_F3] = true;
    wsKeyboardMap[GLFW_KEY_F4] = WS_KEY_F4;
		wsSpecialKeys[GLFW_KEY_F4] = true;
    wsKeyboardMap[GLFW_KEY_F5] = WS_KEY_F5;
		wsSpecialKeys[GLFW_KEY_F5] = true;
    wsKeyboardMap[GLFW_KEY_F6] = WS_KEY_F6;
		wsSpecialKeys[GLFW_KEY_F6] = true;
    wsKeyboardMap[GLFW_KEY_F7] = WS_KEY_F7;
		wsSpecialKeys[GLFW_KEY_F7] = true;
    wsKeyboardMap[GLFW_KEY_F8] = WS_KEY_F8;
		wsSpecialKeys[GLFW_KEY_F8] = true;
    wsKeyboardMap[GLFW_KEY_F9] = WS_KEY_F9;
		wsSpecialKeys[GLFW_KEY_F9] = true;
    wsKeyboardMap[GLFW_KEY_F10] = WS_KEY_F10;
		wsSpecialKeys[GLFW_KEY_F10] = true;
    wsKeyboardMap[GLFW_KEY_F11] = WS_KEY_F11;
		wsSpecialKeys[GLFW_KEY_F11] = true;
    wsKeyboardMap[GLFW_KEY_F12] = WS_KEY_F12;
		wsSpecialKeys[GLFW_KEY_F12] = true;
    wsKeyboardMap[GLFW_KEY_KP_0] = WS_KEY_NUM_0;
		wsSpecialKeys[GLFW_KEY_KP_0] = true;
    wsKeyboardMap[GLFW_KEY_KP_1] = WS_KEY_NUM_1;
		wsSpecialKeys[GLFW_KEY_KP_1] = true;
    wsKeyboardMap[GLFW_KEY_KP_2] = WS_KEY_NUM_2;
		wsSpecialKeys[GLFW_KEY_KP_2] = true;
    wsKeyboardMap[GLFW_KEY_KP_3] = WS_KEY_NUM_3;
		wsSpecialKeys[GLFW_KEY_KP_3] = true;
    wsKeyboardMap[GLFW_KEY_KP_4] = WS_KEY_NUM_4;
		wsSpecialKeys[GLFW_KEY_KP_4] = true;
    wsKeyboardMap[GLFW_KEY_KP_5] = WS_KEY_NUM_5;
		wsSpecialKeys[GLFW_KEY_KP_5] = true;
    wsKeyboardMap[GLFW_KEY_KP_6] = WS_KEY_NUM_6;
		wsSpecialKeys[GLFW_KEY_KP_6] = true;
    wsKeyboardMap[GLFW_KEY_KP_7] = WS_KEY_NUM_7;
		wsSpecialKeys[GLFW_KEY_KP_7] = true;
    wsKeyboardMap[GLFW_KEY_KP_8] = WS_KEY_NUM_8;
		wsSpecialKeys[GLFW_KEY_KP_8] = true;
    wsKeyboardMap[GLFW_KEY_KP_9] = WS_KEY_NUM_9;
		wsSpecialKeys[GLFW_KEY_KP_9] = true;
    wsKeyboardMap[GLFW_KEY_KP_ADD] = WS_KEY_NUM_PLUS;
		wsSpecialKeys[GLFW_KEY_KP_ADD] = true;
    wsKeyboardMap[GLFW_KEY_KP_SUBTRACT] = WS_KEY_NUM_MINUS;
		wsSpecialKeys[GLFW_KEY_KP_SUBTRACT] = true;
    wsKeyboardMap[GLFW_KEY_KP_MULTIPLY] = WS_KEY_NUM_STAR;
		wsSpecialKeys[GLFW_KEY_KP_MULTIPLY] = true;
    wsKeyboardMap[GLFW_KEY_KP_DIVIDE] = WS_KEY_NUM_SLASH;
		wsSpecialKeys[GLFW_KEY_KP_DIVIDE] = true;
    wsKeyboardMap[GLFW_KEY_KP_DECIMAL] = WS_KEY_NUM_PERIOD;
		wsSpecialKeys[GLFW_KEY_KP_DECIMAL] = true;
    wsKeyboardMap[GLFW_KEY_KP_ENTER] = WS_KEY_NUM_ENTER;
		wsSpecialKeys[GLFW_KEY_KP_ENTER] = true;
    wsKeyboardMap[GLFW_KEY_HOME] = WS_KEY_HOME;
		wsSpecialKeys[GLFW_KEY_HOME] = true;
    wsKeyboardMap[GLFW_KEY_END] = WS_KEY_END;
		wsSpecialKeys[GLFW_KEY_END] = true;
    wsKeyboardMap[GLFW_KEY_PAGEDOWN] = WS_KEY_PG_DN;
		wsSpecialKeys[GLFW_KEY_PAGEDOWN] = true;
    wsKeyboardMap[GLFW_KEY_PAGEUP] = WS_KEY_PG_UP;
		wsSpecialKeys[GLFW_KEY_PAGEUP] = true;
    wsKeyboardMap[GLFW_KEY_INSERT] = WS_KEY_INSERT;
		wsSpecialKeys[GLFW_KEY_INSERT] = true;
    wsKeyboardMap[GLFW_KEY_ESC] = WS_KEY_ESC;
		wsSpecialKeys[GLFW_KEY_ESC] = true;
  }
  void GLFWCALL wsKeyboardCallback(i32 key, i32 action) {
    wsKeyArray my;
    if (action == GLFW_PRESS) wsLog(WS_LOG_HID, "Key Down: %d\n", key);
    u64 btnState = 0;
    bool specialKey = false;
    if (key > 0 && key <= GLFW_KEY_LAST) {
      btnState = wsKeyboardMap[key];
      specialKey = wsSpecialKeys[key];
    }
    if (action == GLFW_PRESS) {
      if (specialKey) {
        wsInputs.pressSpecKey(btnState);
        wsEvents.push(wsEvent(WS_EVENT_INPUT, WS_INPUT_TYPE_KEYBOARD, WS_KB_SPECIAL_KEY, btnState, WS_PRESS));
      }
      else {
        wsInputs.pressKey(btnState);
        wsEvents.push(wsEvent(WS_EVENT_INPUT, WS_INPUT_TYPE_KEYBOARD, WS_KB_KEY, btnState, WS_PRESS));
      }
    }
    else if (action == GLFW_RELEASE) {
      if (specialKey) {
        wsInputs.releaseSpecKey(btnState);
        wsEvents.push(wsEvent(WS_EVENT_INPUT, WS_INPUT_TYPE_KEYBOARD, WS_KB_SPECIAL_KEY, btnState, WS_RELEASE));
      }
      else {
        wsInputs.releaseKey(btnState);
        wsEvents.push(wsEvent(WS_EVENT_INPUT, WS_INPUT_TYPE_KEYBOARD, WS_KB_KEY, btnState, WS_RELEASE));
      }
    }
  }
  void initMouseButtonMap() {
    wsMouseButtonMap[GLFW_MOUSE_BUTTON_1] = WS_MOUSE_BUTTON_LEFT;
    wsMouseButtonMap[GLFW_MOUSE_BUTTON_2] = WS_MOUSE_BUTTON_RIGHT;
    wsMouseButtonMap[GLFW_MOUSE_BUTTON_3] = WS_MOUSE_BUTTON_MIDDLE;
    wsMouseButtonMap[GLFW_MOUSE_BUTTON_4] = WS_MOUSE_SCROLL_UP;
    wsMouseButtonMap[GLFW_MOUSE_BUTTON_5] = WS_MOUSE_SCROLL_DOWN;
    wsMouseButtonMap[GLFW_MOUSE_BUTTON_6] = WS_MOUSE_SCROLL_LEFT;
    wsMouseButtonMap[GLFW_MOUSE_BUTTON_7] = WS_MOUSE_SCROLL_RIGHT;
    wsMouseButtonMap[GLFW_MOUSE_BUTTON_8] = WS_MOUSE_BUTTON_4;
  }
  void GLFWCALL wsMouseMotionCallback(i32 posX, i32 posY) {
    if (wsInputs.mouseInitialized()) {
      wsInputs.setMousePos(posX, posY);
      wsEvents.push(wsEvent(WS_EVENT_INPUT, WS_INPUT_TYPE_MOUSE, WS_ANALOG, posX, posY, wsInputs.getMouseDx(), wsInputs.getMouseDy()));
    }
    else {  //  Initialization
      wsInputs.setMousePos(posX, posY);
    }
  }
  void GLFWCALL wsMouseButtonCallback(i32 button, i32 action) {
    u32 btnState = 0;
    if (action == GLFW_PRESS) { wsLog(WS_LOG_HID, "Mouse Button Down: %d\n", button); }
    if (button >= 0 && button <= GLFW_MOUSE_BUTTON_LAST) {
      btnState = wsMouseButtonMap[button];
    }
    if (action == GLFW_PRESS) {
      wsInputs.pressMouseButton(btnState);
      wsEvents.push(wsEvent(WS_EVENT_INPUT, WS_INPUT_TYPE_MOUSE, WS_PRESS, btnState));
    }
    else if (action == GLFW_RELEASE) {
      wsInputs.releaseMouseButton(btnState);
      wsEvents.push(wsEvent(WS_EVENT_INPUT, WS_INPUT_TYPE_MOUSE, WS_RELEASE, btnState));
    }
  }
  void GLFWCALL wsMouseWheelCallback(i32 wheelPos) {
    //  Intentionally empty for now.
    //  Linux gets its mouse wheel input from mouse button calls
  }
  //*/
#endif

void wsInputManager::pollDevices() {
  //  Poll For New Devices
  #if (WS_SCREEN_BACKEND == WS_BACKEND_X11)
  #elif (WS_SCREEN_BACKEND == WS_BACKEND_GLFW)
    for (u32 i = 0; (i <= GLFW_JOYSTICK_LAST && i < WS_MAX_CONTROLLERS); ++i) {
      if (glfwGetJoystickParam(i, GLFW_PRESENT) && !controllers[i].getPluggedIn()) {
        controllers[i].setPluggedIn(true);
        ++numControllers;
        controllers[i].setPlayerNumber(i);
        u32 numAxes = glfwGetJoystickParam(i, GLFW_AXES);
        u32 numButtons = glfwGetJoystickParam(i, GLFW_BUTTONS);
        wsLog(WS_LOG_HID,  "Controller %u has been plugged in.\nNumAxes = %u\nNumButtons=%u\n", i, numAxes, numButtons);
      }
      else if (!glfwGetJoystickParam(i, GLFW_PRESENT) && controllers[i].getPluggedIn()) {
        controllers[i].setPluggedIn(false);
        --numControllers;
        wsLog(WS_LOG_HID, "Controller %u has been unplugged.\n", i);
      }
    }
    glfwPollEvents();
  #endif
  //  Update existing device states
  for (u32 i = 0; i < numControllers; ++i) {
    controllers[i].poll();
  }
}

void wsInputManager::setMousePos(i32 posX, i32 posY) {
  mouse.setPos(posX, posY);
  wsRenderer.scanHUD();
}

void wsInputManager::pressMouseButton(u32 btnState) {
  mouse.pressButton(btnState);
  wsRenderer.scanHUD();
}

void wsInputManager::releaseMouseButton(u32 btnState) {
  mouse.releaseButton(btnState);
  wsRenderer.scanHUD();
}

void wsInputManager::swapFrames() {
  keyboard.swapFrames();
  for (u32 i = 0; i < numControllers; ++i) {
    controllers[i].swapFrames();
  }
  mouse.swapFrames();
}

bool wsInputManager::testEvent(const wsInputEvent& event) {
  if (event.inputType == WS_INPUT_TYPE_KEYBOARD) {  //  Test for keyboard event
    wsKeyArray buttonStates;
    if (event.eventType == WS_PRESS) {
      buttonStates = keyboard.getNewlyPressedButtons();
    }
    else if (event.eventType == WS_RELEASE) {
      buttonStates = keyboard.getNewlyReleasedButtons();
    }
    else if (event.eventType == WS_PRESS_OR_RELEASE) {
      buttonStates = keyboard.getAlteredButtons();
    }

    if (event.inputIndex == WS_KB_KEY) {
      return (event.buttonVal & buttonStates.keyButtonStates);
    }
    else if (event.inputIndex == WS_KB_SPECIAL_KEY) {
      return (event.buttonVal & buttonStates.specButtonStates);
    }
  }
  else if (event.inputType == WS_INPUT_TYPE_CONTROLLER) {
    u32 buttonStates = WS_NULL;
    if (event.inputIndex < numControllers) {
      if (event.eventType == WS_PRESS) {
        buttonStates = controllers[event.inputIndex].getNewlyPressedButtons();
      }
      else if (event.eventType == WS_RELEASE) {
        buttonStates = controllers[event.inputIndex].getNewlyReleasedButtons();
      }
      else if (event.eventType == WS_PRESS_OR_RELEASE) {
        buttonStates = controllers[event.inputIndex].getAlteredButtons();
      }
      return (event.buttonVal & buttonStates);
    }
  }
  return false;
}

void wsInputManager::shutDown() {
  //  Nothing to see here...
}
