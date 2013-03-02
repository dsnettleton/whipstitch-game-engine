/*
 * wsScreen.h
 *
 *  Created on: Sep 7, 2012
 *      Author: dsnettleton
 *
 *  This file declares the class wsScreen, which contains the necessary
 *  methods required for defining a screen object. A screen in the Whipstitch
 *  game engine is considered to be anything which may be drawn upon. This
 *  could be a single window or a physical screen, depending upon the platform.
 *
 *  Onscreen coordinates in the Whipstitch Game Engine are given using
 *  a scale 1600 units wide by 900 units tall. This is for consistency
 *  across many window sizes, and may or may not translate directly into
 *  pixels, depending on the resolution. If the window ratio is not 16:9
 *  (widescreen), the shortened dimension (either height or width) is
 *  clipped from the right or top. The windowspace origin (0,0) is at
 *  the lower left corner of the screen, in order to be sensible.
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

#ifndef WS_SCREEN_H_
#define WS_SCREEN_H_

#include "../wsUtils.h"

#define WS_BACKEND_GLFW     0x01
#define WS_BACKEND_X11      0x02
#define WS_BACKEND_GTK      0x03    //  For potential future use
#define WS_BACKEND_WINAPI   0x04    //  For potential future use
#define WS_BACKEND_FREEGLUT 0x05    //  For potential future use

//  Default Backend is GLFW
#ifndef WS_SCREEN_BACKEND
    #define WS_SCREEN_BACKEND   WS_BACKEND_GLFW
#endif

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
        mHeight = wsScreenHeight;
        mWidth = wsScreenWidth;
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
        #if (WS_SCREEN_BACKEND == WS_BACKEND_X11)
            Display* xDisp;
            Window win;
            GLXContext context;
        #endif
    public:
        /** Constructor **/
        //  Creates a new screen with the given settings and opens it
        wsScreen(wsScreenSettings mySettings);
        /** Setters and Getters **/
        #if (WS_SCREEN_BACKEND == WS_BACKEND_X11)
            Display* getDisplay() { return xDisp; }
        #endif
        /** Operational Methods **/
        //  Clears the screen
        void clear();
        //  Closes the screen. It cannot be reopened
        void close();
        //  Draws the screen
        void draw();
        //  Swap the front and back buffers
        void swapBuffers();
};

#endif /* WS_SCREEN_H_ */
