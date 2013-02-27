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
