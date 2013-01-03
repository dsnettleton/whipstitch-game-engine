/*
 * wsScreen.cpp
 *
 *  Created on: Sep 7, 2012
 *      Author: dsnettleton
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

wsScreen::wsScreen(wsScreenSettings mySettings) {
    settings = mySettings;
    wsAssert(
        glfwOpenWindow( settings.mWidth,    //  window width
                        settings.mHeight,   //  window height
                        settings.mChannelBits,  //  red bits
                        settings.mChannelBits,  //  green bits
                        settings.mChannelBits,  //  blue bits
                        settings.mChannelBits,  //  alpha bits
                        settings.mDepthBuffer ? settings.mChannelBits : 0,  //  depth bits
                        settings.mStencilBuffer ? settings.mChannelBits : 0, // stencil bits
                        settings.mFullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW),   //  window mode
            "Could not open window. GLFW error.");  //  Assertion fail message
    glfwSetWindowTitle(settings.mTitle);
}

void wsScreen::clear() {
    wsAssert(settings.open(), "Cannot clear the screen. This screen has been closed.");
}

void wsScreen::close() {
    glfwCloseWindow();
}

void wsScreen::draw() {
    wsAssert(settings.open(), "Cannot draw the screen. This screen has been closed.");
}

