/*
 *  wsInputManager.h
 *
 *  Created on: Jan 1, 2013
 *      Author: dsnettleton
 *
 *      This file declares the class wsInputManager, which manages multiple human
 *      interface devices, queueing their inputs for use by the engine.
 *
 *      wsInputManager is an engine subsytem, and must be initialized via the startUp()
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

#ifndef WS_INPUT_MANAGER_H_
#define WS_INPUT_MANAGER_H_

#include "../wsUtils/wsPlatform.h"

#include "wsKeyboardInput.h"
#include "wsController.h"
#include "wsPointerInput.h"

#if (WS_SCREEN_BACKEND == WS_BACKEND_X11)
#elif (WS_SCREEN_BACKEND == WS_BACKEND_GLFW)

  #include "GL/glew.h"
  #include "GL/glfw.h"

  //  Keyboard management/callbacks
  extern u64 wsKeyboardMap[GLFW_KEY_LAST+1];
  extern bool wsSpecialKeys[GLFW_KEY_LAST+1];
  void initKeyboardMaps();
  void GLFWCALL wsKeyboardCallback(i32 key, i32 action);
  //  Mouse management/callbacks
  extern u32 wsMouseButtonMap[GLFW_MOUSE_BUTTON_LAST+1];
  void initMouseButtonMap();
  void GLFWCALL wsMouseMotionCallback(i32 posX, i32 posY);
  void GLFWCALL wsMouseButtonCallback(i32 button, i32 action);
  void GLFWCALL wsMouseWheelCallback(i32 wheelPos);
#endif

struct wsInputEvent {
  u64 buttonVal;
  u32 inputType;   //  Keyboard, joystick, mouse, etc.
  u16 inputIndex;  //  Player number for multiple controllers, key/spec for keyboard
  u16 eventType;   //  Pressed or released?
  wsInputEvent() : buttonVal(0), inputType(0), inputIndex(0), eventType(0) {}
  wsInputEvent(u64 myButtonVal, u32 myInputType, u16 myInputIndex, u16 myEventType) :
    buttonVal(myButtonVal), inputType(myInputType), inputIndex(myInputIndex), eventType(myEventType) {}
};

class wsInputManager {
  private:
    wsKeyboardInput keyboard;
    wsController controllers[WS_MAX_CONTROLLERS];
    wsPointerInput mouse;
    u32 numControllers;
    //  True only when the startUp function has been called
    bool _mInitialized;
  public:
    /*  Empty Constructor and Destructor   */
    //  As an engine subsystem, the input manager takes no action until explicitly
    //  initialized via the startUp(...) function.
    //  uninitialized via the shutDown() function.
    wsInputManager() : _mInitialized(false) { }
    ~wsInputManager() {}
    /*  Startup and shutdown functions  */
    //  Initializes the game loop
    void startUp();
    //  Uninitializes the game loop
    void shutDown();
    /*  Setters and Getters */
    bool getMouseDown(u32 btnIndex = WS_MOUSE_BUTTON_LEFT) { return mouse.getDown(btnIndex); }
    f32 getMouseDx() { return mouse.getDx(); }
    f32 getMouseDy() { return mouse.getDy(); }
    f32 getMouseX() { return mouse.getX(); }
    f32 getMouseY() { return mouse.getY(); }
    bool mouseInitialized() { return mouse.getInitialized(); }
    /*  Operational Methods */
    void pollDevices();
    void swapFrames();
    bool testEvent(const wsInputEvent& event);
    //  Send to the keyboard object
    void pressKey(u64 keyIndex) { keyboard.pressKey(keyIndex); }
    void pressSpecKey(u64 specKeyIndex) { keyboard.pressSpecKey(specKeyIndex); }
    void releaseKey(u64 keyIndex) { keyboard.releaseKey(keyIndex); }
    void releaseSpecKey(u64 specKeyIndex) { keyboard.releaseSpecKey(specKeyIndex); }
    //  Send to the mouse object
    void setMousePos(i32 posX, i32 posY);
    void pressMouseButton(u32 btnState);
    void releaseMouseButton(u32 btnState);
};

extern wsInputManager wsInputs;

#endif  /*  WS_INPUT_MANAGER_H_ */
