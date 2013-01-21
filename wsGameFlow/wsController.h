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
