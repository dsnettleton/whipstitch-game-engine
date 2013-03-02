/*
 * wsScreenManager.h
 *
 *  Created on: Sep 7, 2012
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
 *      wsInit(). The wsScreenManager *must* be initialized *after* the wsMemoryStack
 *      subsystem.
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

#ifndef WS_SCREEN_MANAGER_H_
#define WS_SCREEN_MANAGER_H_

#include "wsScreen.h"

class wsScreenManager {
    private:
        //  Maximum number of screens allowed; subject to change with different backends
        const static i8 MAX_SCREENS = 1;
        //  Current screen used for drawing, etc.
        i8 mCurrentScreen;
        //  Number of screens created
        i8 mNumScreens;
        //  An array of pointers to wsScreen objects
        wsScreen** mScreens;
        //  True only when the startUp function has been called
        bool _mInitialized;
    public:
        /*  Default Constructor and Deconstructor */
        //  As an engine subsystem, the screen manager takes no action until explicitly
        //  initialized via the startUp(...) function.
        //  uninitialized via the shutDown() function.
        wsScreenManager() : _mInitialized(false) {}
        ~wsScreenManager() {}
        //  Uninitializes the screen manager
        void shutDown();
        //  Initializes the screen manager
        void startUp(const char* title, const i32 width, const i32 height, bool fullscreen);
        /*  Setters and Getters */
        #if (WS_SCREEN_BACKEND == WS_BACKEND_X11)
            Display* getDisplay(i32 index = -1) {
                wsAssert(_mInitialized, "You must first initialize the screen manager.");
                if (index < 0 || index >= mNumScreens) {
                    index = (i32)mCurrentScreen;
                }
                return mScreens[index]->getDisplay();
            }
        #endif
        bool closed();
        /*  Operational Methods */
        //  Creates a new screen using the given title and dimensions
        i32 create(const char* title, const i32 width, const i32 height, bool fullscreen);
        //  Creates a new screen using the given settings
        i32 create(const wsScreenSettings& settings);
        //  Swap the buffers on the current screen
        void swapBuffers();
};

extern wsScreenManager wsScreens;

#endif /* WS_SCREEN_MANAGER_H_ */
