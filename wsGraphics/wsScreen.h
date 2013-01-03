/*
 * wsScreen.h
 *
 *  Created on: Sep 7, 2012
 *      Author: dsnettleton
 *
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

#ifndef WS_SCREEN_H_
#define WS_SCREEN_H_

#include "../wsUtils.h"
#include "GL/glfw.h"

#define WS_BACKEND_GLFW     0x01
#define WS_BACKEND_GTK      0x02    //  For potential future use
#define WS_BACKEND_WINAPI   0x04    //  For potential future use
#define WS_BAKCEND_FREEGLUT 0x08    //  For potential future use

//  Default Backend is GLFW
#define WS_SCREEN_BACKEND   WS_BACKEND_GLFW

struct wsScreenSettings {
    public:
        i32 mChannelBits;
        i32 mHeight;
        i32 mWidth;
        bool mFullscreen;
        bool mDepthBuffer;
        bool mStencilBuffer;
    private:
        bool mOpen;
    public:
        const char* mTitle;
    wsScreenSettings() {
        mTitle = "Whipstitch Game Engine";
        mHeight = 720;
        mWidth = 1280;
        mChannelBits = 8;   //  times 4 channels = 32 bit color
        mFullscreen = false;
        mDepthBuffer = true;
        mStencilBuffer = false;
        mOpen = true;
    }
    wsScreenSettings(const char* title, const i32 width, const i32 height, bool fullscreen) {
        mTitle = title;
        mWidth = width;
        mHeight = height;
        mChannelBits = 8;   //  times 4 channels = 32 bit color
        mFullscreen = fullscreen;
        mDepthBuffer = true;
        mStencilBuffer = false;
        mOpen = true;
    }
    bool open() { return mOpen; }
};

class wsScreen {
    private:
        wsScreenSettings settings;
    public:
        /** Constructor **/
        //  Creates a new screen with the given settings and opens it
        wsScreen(wsScreenSettings mySettings);
        /** Operational Methods **/
        //  Clears the screen
        void clear();
        //  Closes the screen. It cannot be reopened
        void close();
        //  Draws the screen
        void draw();
};

#endif /* WS_SCREEN_H_ */
