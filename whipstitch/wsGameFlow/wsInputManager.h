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
