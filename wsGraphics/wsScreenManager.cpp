/*
 * wsScreenManager.cpp
 *
 *  Created on: Sep 8, 2012
 *      Author: dsnettleton
 *
 *      This file declares the class wsScreenManager, which stores multiple objects
 *      of type wsScreen. The Screen Manager is built to allow more than one Window
 *      or screen to be used by the program. This could be especially useful on gaming
 *      platforms with multiple screens such as the Nintendo DS or Wii U, but can also be
 *      used for stereoscopic 3D.
 *
 *      wsScreenManager is an engine subsytem, and must be initialized via the startUp()
 *      function before it may be used. This is done through the engine startup command
 *      wsInit().
 *
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

#include "wsScreenManager.h"
#if WS_SCREEN_BACKEND == WS_BACKEND_GLFW
  #include "GL/glfw.h"
#endif

//  Define the singleton instance of this class
wsScreenManager wsScreens;

void wsScreenManager::startUp(const char* title, const i32 width, const i32 height, bool fullscreen) {
    _mInitialized = true;
    //  Initialize our screen array on the primary memory stack
    mScreens = wsNewArray(wsScreen*, MAX_SCREENS);
    mCurrentScreen = mNumScreens = 0;
    #if (WS_SCREEN_BACKEND == WS_BACKEND_X11)
    #elif (WS_SCREEN_BACKEND == WS_BACKEND_GLFW)
        glfwInit();
    #endif
    create(title, width, height, fullscreen);
}

void wsScreenManager::shutDown() {
    #if (WS_SCREEN_BACKEND == WS_BACKEND_X11)
    #elif (WS_SCREEN_BACKEND == WS_BACKEND_GLFW)
        glfwTerminate();
    #endif
}

bool wsScreenManager::closed() {
    #if (WS_SCREEN_BACKEND == WS_BACKEND_GLFW)
        return !glfwGetWindowParam(GLFW_OPENED);
    #endif
}

i32 wsScreenManager::create(const char* title, const i32 width, const i32 height, bool fullscreen) {
    wsAssert(_mInitialized, "A screen cannot be created until the screen manager has been initialized.");
    wsAssert( (mNumScreens < MAX_SCREENS), "Cannot create another screen. Max screens reached");
    mCurrentScreen = mNumScreens++;

    wsScreenSettings settings(title, width, height, fullscreen);
    //mScreens[mCurrentScreen] = wsNew wsScreen(settings);
    //  Placement new
    mScreens[mCurrentScreen] = new (wsMem.allocatePrimary( sizeof(wsScreen) )) wsScreen(settings);
    return mCurrentScreen;
}

i32 wsScreenManager::create(const wsScreenSettings& settings) {
    wsAssert(_mInitialized, "A screen cannot be created until the screen manager has been initialized.");
    wsAssert( (mNumScreens < MAX_SCREENS), "Cannot create another screen. Max screens reached");
    mCurrentScreen = mNumScreens++;
    //  Placement new
    mScreens[mCurrentScreen] = new (wsMem.allocatePrimary( sizeof(wsScreen) )) wsScreen(settings);
    return mCurrentScreen;
}

void wsScreenManager::swapBuffers() {
    //  Swap the buffers on the current screen
    mScreens[mCurrentScreen]->swapBuffers();
}
