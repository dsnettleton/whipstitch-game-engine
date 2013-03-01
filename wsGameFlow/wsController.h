/**
 *    wsController.h
 *    Jan 6, 2013
 *    D. Scott Nettleton
 *
 *    This file declares the class wsController, which inherits the class
 *    wsInputMethod. A wsController object is a standard-style controller,
 *    with six face buttons, four shoulder buttons, two clickable analog
 *    sticks, and a direction pad.
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
#ifndef WS_CONTROLLER_H_
#define WS_CONTROLLER_H_

#include "wsInputMethod.h"

class wsController : public wsInputMethod {
    private:
        #if (WS_SCREEN_BACKEND == WS_BACKEND_X11)
        #elif (WS_SCREEN_BACKEND == WS_BACKEND_GLFW)
            wsAnalogControl wiiClassicControllerAnalogAxes[4];
            u32 wiiClassicControllerAxesMap[6]; 
            u32 wiiClassicControllerMap[11];
        #endif
        wsAnalogControl* analogAxes;
        u32* axesMap;
        u32* controllerMap;
        u16 playerNumber;
        u16 numButtons;
        u16 numAxes;
        //  Private Data Members
        bool pluggedIn;
    public:
        //  Constructors and Deconstructors
        wsController();
        //  Setters and Getters
        bool getPluggedIn() { return pluggedIn; }
        void setPluggedIn(bool myState) { pluggedIn = myState; }
        void setPlayerNumber(u32 my) { playerNumber = my; }
        //  Operational Methods
        void poll();    //  Inherited from wsInputMethod
};

#endif //  WS_CONTROLLER_H_
