/*
 * wsScreen.cpp
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

#include "wsScreen.h"

#if (WS_SCREEN_BACKEND == WS_BACKEND_X11)
    #include "X11/X.h"
    #include "X11/Xlib.h"
    #include "GL/glx.h"
#elif (WS_SCREEN_BACKEND == WS_BACKEND_GLFW)
    #include "GL/glfw.h"
#endif

wsScreen::wsScreen(wsScreenSettings mySettings) {
    settings = mySettings;
    #if (WS_SCREEN_BACKEND == WS_BACKEND_X11)
        Window rootWindow;
        XVisualInfo* visualInfo;
        Colormap colorMap;
        XSetWindowAttributes xWinAttributesStruct;

        i32 winAttr[16];
        u32 current = 0;
        winAttr[current++] = GLX_RGBA;  //  RGBA Color Mode
        winAttr[current++] = GLX_DOUBLEBUFFER;  //  Always double-buffer for games
        if (settings.mDepthBuffer) {    //  Set the bit depth of the depth buffer
            winAttr[current++] = GLX_DEPTH_SIZE;
            winAttr[current++] = settings.mChannelBits*3;
        }
        winAttr[current] = None;    //  Terminate the attribute list

        xDisp = XOpenDisplay(NULL);   //  NULL argument means graphical output is sent to the computer on which this is executed
        wsAssert((xDisp != NULL), "Could not open window. X11 error.");

        rootWindow = DefaultRootWindow(xDisp);  //  "Desktop background" window

        visualInfo = glXChooseVisual(xDisp, 0, winAttr);
        wsAssert((visualInfo != NULL) ,"X Visual Info could not be initialized");
        wsLog(WS_LOG_GRAPHICS, "Visual Id %p selected", (void *)visualInfo->visualid);

        colorMap = XCreateColormap(xDisp, rootWindow, visualInfo->visual, AllocNone);
        xWinAttributesStruct.colormap = colorMap;
        xWinAttributesStruct.event_mask = ExposureMask | KeyPressMask;
        win = XCreateWindow(xDisp,  //  Window to create
                            rootWindow, //  Parent window
                            0, 0,   //  Position (usually relative to parent)
                            settings.mWidth, settings.mHeight,  //  Dimensions of new window
                            0,  //  Border width (meaningless for top-level windows)
                            visualInfo->depth,  //  Color depth
                            InputOutput,    //  Window type
                            visualInfo->visual, //  Visual infomation
                            CWColormap | CWEventMask,   //  Which attributes are set by the xWinAttributesStruct
                            &xWinAttributesStruct); //  X Window Attributes

        XMapWindow(xDisp, win);
        XStoreName(xDisp, win, settings.mTitle);

        //  Create an OpenGL context
        context = glXCreateContext(xDisp, visualInfo, NULL, GL_TRUE);
        glXMakeCurrent(xDisp, win, context);
    #elif (WS_SCREEN_BACKEND == WS_BACKEND_GLFW)
        bool success = glfwOpenWindow( settings.mWidth,    //  window width
                                        settings.mHeight,   //  window height
                                        settings.mChannelBits,  //  red bits
                                        settings.mChannelBits,  //  green bits
                                        settings.mChannelBits,  //  blue bits
                                        settings.mChannelBits,  //  alpha bits
                                        settings.mDepthBuffer ? settings.mChannelBits : 0,  //  depth bits
                                        settings.mStencilBuffer ? settings.mChannelBits : 0, // stencil bits
                                        settings.mFullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW);   //  window mode
        wsAssert(success, "Could not open window. GLFW error.");  //  Assertion fail message
        if (!success) {
          exit(1);
        }
        glfwSetWindowTitle(settings.mTitle);
    #endif
}

void wsScreen::clear() {
    wsAssert(settings.open(), "Cannot clear the screen. This screen has been closed.");
}

void wsScreen::close() {
    #if (WS_SCREEN_BACKEND == WS_BACKEND_X11)
        glXMakeCurrent(xDisp, None, NULL);
        glXDestroyContext(xDisp, context);
        XDestroyWindow(xDisp, win);
        XCloseDisplay(xDisp);
    #elif (WS_SCREEN_BACKEND == WS_BACKEND_GLFW)
        glfwCloseWindow();
    #endif
}

void wsScreen::draw() {
    wsAssert(settings.open(), "Cannot draw the screen. This screen has been closed.");
}

void wsScreen::swapBuffers() {
    #if (WS_SCREEN_BACKEND == WS_BACKEND_X11)
        glXSwapBuffers(xDisp, win);
    #elif (WS_SCREEN_BACKEND == WS_BACKEND_GLFW)
        glfwSwapBuffers();
    #endif
}

