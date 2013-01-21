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

#include "wsScreenManager.h"

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
