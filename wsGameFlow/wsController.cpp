/**
 *    wsController.cpp
 *    Jan 6, 2013
 *    D. Scott Nettleton
 *
 *    This file implements the class wsController, which inherits the class
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

#include "wsController.h"
// #include "wsGameLoop.h"

//  Constructor
wsController::wsController() {
    pluggedIn = false;
    #if (WS_SCREEN_BACKEND == WS_BACKEND_X11)
    #elif (WS_SCREEN_BACKEND == WS_BACKEND_GLFW)
        //  Wii Classic Controller
        wiiClassicControllerMap[0] = WS_BUTTON_RIGHT;
        wiiClassicControllerMap[1] = WS_BUTTON_BOTTOM;
        wiiClassicControllerMap[2] = WS_BUTTON_TOP;
        wiiClassicControllerMap[3] = WS_BUTTON_LEFT;
        wiiClassicControllerMap[4] = WS_SHOULDER_LEFT;
        wiiClassicControllerMap[5] = WS_SHOULDER_RIGHT;
        wiiClassicControllerMap[6] = WS_SHOULDER_LEFT_2;
        wiiClassicControllerMap[7] = WS_SHOULDER_RIGHT_2;
        wiiClassicControllerMap[8] = WS_BUTTON_SELECT;
        wiiClassicControllerMap[9] = WS_BUTTON_START;
        wiiClassicControllerMap[10] = WS_BUTTON_SYSTEM;
        numButtons = 11;
        relativeMask = WS_NO_BUTTONS;
        wiiClassicControllerAxesMap[0] = WS_D_PAD_AXIS_X;
        wiiClassicControllerAxesMap[1] = WS_D_PAD_AXIS_Y;
        wiiClassicControllerAxesMap[2] = WS_ANALOG_L_AXIS_X;
        wiiClassicControllerAxesMap[3] = WS_ANALOG_L_AXIS_Y;
        wiiClassicControllerAxesMap[4] = WS_ANALOG_R_AXIS_X;
        wiiClassicControllerAxesMap[5] = WS_ANALOG_R_AXIS_Y;
        numAxes = 6;
        //  Set our default values
        controllerMap = wiiClassicControllerMap;
        axesMap = wiiClassicControllerAxesMap;
        analogAxes = wiiClassicControllerAnalogAxes;
        analogAxes[1].sensitivity = -1.0f;
        analogAxes[3].sensitivity = -1.0f;
        analogMask = (  WS_ANALOG_R_AXIS_Y | 
                        WS_ANALOG_R_AXIS_X | 
                        WS_ANALOG_L_AXIS_Y | 
                        WS_ANALOG_L_AXIS_X  );
    #endif
}

void wsController::poll() {
    if (pluggedIn) {
        //  Poll the joystick controller for input information
        #if (WS_SCREEN_BACKEND == WS_BACKEND_X11)
        #elif (WS_SCREEN_BACKEND == WS_BACKEND_GLFW)
            //  Check analog axis values
            f32* axisValues = wsNewArrayTmp(f32, numAxes);
            glfwGetJoystickPos(playerNumber, axisValues, numAxes);
            u32 analogAxisCounter = 0;
            for (u32 i = 0; i < numAxes; ++i) {
                if (axesMap[i] == WS_D_PAD_AXIS_X) {
                    //  Digital Axis 1
                    if (axisValues[i] < 0) {
                        if (!(buttonStates & WS_D_PAD_LEFT)) {
                            //  Activate D-pad left, deactivate right
                            wsLog(WS_LOG_HID, "D-Pad Left Pressed.");
                            wsEvents.push(wsEvent(WS_EVENT_INPUT, WS_INPUT_TYPE_CONTROLLER, playerNumber,
                                WS_D_PAD_LEFT, WS_PRESS));
                            buttonStates |= WS_D_PAD_LEFT;
                            if (buttonStates & WS_D_PAD_RIGHT) {
                                wsLog(WS_LOG_HID, "D-Pad Right Released.");
                                wsEvents.push(wsEvent(WS_EVENT_INPUT, WS_INPUT_TYPE_CONTROLLER,
                                    playerNumber, WS_D_PAD_RIGHT, WS_RELEASE));
                                buttonStates ^= WS_D_PAD_RIGHT;
                            }
                        }
                    }
                    else if (axisValues[i] > 0) {
                        if (!(buttonStates & WS_D_PAD_RIGHT)) {
                            //  Activate D-pad right, deactivate left
                            wsLog(WS_LOG_HID, "D-Pad Right Pressed.");
                            buttonStates |= WS_D_PAD_RIGHT;
                            wsEvents.push(wsEvent(WS_EVENT_INPUT, WS_INPUT_TYPE_CONTROLLER, playerNumber,
                                WS_D_PAD_RIGHT, WS_PRESS));
                            if (buttonStates & WS_D_PAD_LEFT) {
                                wsLog(WS_LOG_HID, "D-Pad Left Released.");
                                buttonStates ^= WS_D_PAD_LEFT;
                                wsEvents.push(wsEvent(WS_EVENT_INPUT, WS_INPUT_TYPE_CONTROLLER,
                                    playerNumber, WS_D_PAD_LEFT, WS_RELEASE));
                            }
                        }
                    }
                    else {
                        if (buttonStates & WS_D_PAD_LEFT) {
                            wsLog(WS_LOG_HID, "D-Pad Left Released.");
                            buttonStates ^= WS_D_PAD_LEFT;
                            wsEvents.push(wsEvent(WS_EVENT_INPUT, WS_INPUT_TYPE_CONTROLLER,
                                playerNumber, WS_D_PAD_LEFT, WS_RELEASE));
                        }
                        if (buttonStates & WS_D_PAD_RIGHT) {
                            wsLog(WS_LOG_HID, "D-Pad Right Released.");
                            buttonStates ^= WS_D_PAD_RIGHT;
                            wsEvents.push(wsEvent(WS_EVENT_INPUT, WS_INPUT_TYPE_CONTROLLER,
                                playerNumber, WS_D_PAD_RIGHT, WS_RELEASE));
                        }
                    }
                }
                else if (axesMap[i] == WS_D_PAD_AXIS_Y) {
                    //  Digital Axis 2
                    if (axisValues[i] > 0) {
                        if (!(buttonStates & WS_D_PAD_DOWN)) {
                            //  Activate D-pad down, deactivate up
                            wsLog(WS_LOG_HID, "D-Pad Down Pressed.");
                            buttonStates |= WS_D_PAD_DOWN;
                            wsEvents.push(wsEvent(WS_EVENT_INPUT, WS_INPUT_TYPE_CONTROLLER,
                                playerNumber, WS_D_PAD_DOWN, WS_PRESS));
                            if (buttonStates & WS_D_PAD_UP) {
                                wsLog(WS_LOG_HID, "D-Pad Up Released.");
                                buttonStates ^= WS_D_PAD_UP;
                                wsEvents.push(wsEvent(WS_EVENT_INPUT, WS_INPUT_TYPE_CONTROLLER,
                                    playerNumber, WS_D_PAD_UP, WS_RELEASE));
                            }
                        }
                    }
                    else if (axisValues[i] < 0) {
                        if (!(buttonStates & WS_D_PAD_UP)) {
                            //  Activate D-pad up, deactivate down
                            wsLog(WS_LOG_HID, "D-Pad Up Pressed.");
                            buttonStates |= WS_D_PAD_UP;
                            wsEvents.push(wsEvent(WS_EVENT_INPUT, WS_INPUT_TYPE_CONTROLLER,
                                playerNumber, WS_D_PAD_UP, WS_PRESS));
                            if (buttonStates & WS_D_PAD_DOWN) {
                                wsLog(WS_LOG_HID, "D-Pad Down Released.");
                                buttonStates ^= WS_D_PAD_DOWN;
                                wsEvents.push(wsEvent(WS_EVENT_INPUT, WS_INPUT_TYPE_CONTROLLER,
                                    playerNumber, WS_D_PAD_DOWN, WS_RELEASE));
                            }
                        }
                    }
                    else {
                        if (buttonStates & WS_D_PAD_DOWN) {
                            wsLog(WS_LOG_HID, "D-Pad Down Released.");
                            buttonStates ^= WS_D_PAD_DOWN;
                            wsEvents.push(wsEvent(WS_EVENT_INPUT, WS_INPUT_TYPE_CONTROLLER,
                                playerNumber, WS_D_PAD_DOWN, WS_RELEASE));
                        }
                        if (buttonStates & WS_D_PAD_UP) {
                            wsLog(WS_LOG_HID, "D-Pad Up Released.");
                            buttonStates ^= WS_D_PAD_UP;
                            wsEvents.push(wsEvent(WS_EVENT_INPUT, WS_INPUT_TYPE_CONTROLLER,
                                playerNumber, WS_D_PAD_UP, WS_RELEASE));
                        }
                    }
                }
                else {  //  Analog Axis
                    if (analogAxes[analogAxisCounter].set(axisValues[i])) {
                        wsLog(WS_LOG_HID, "Axis %u: Value = %f\n", i, analogAxes[analogAxisCounter].value);
                        wsEvents.push(wsEvent(WS_EVENT_INPUT, WS_INPUT_TYPE_CONTROLLER, playerNumber, axesMap[i], WS_ANALOG,
                                analogAxes[analogAxisCounter].value));
                    }
                    ++analogAxisCounter;
                }
            }

            //  Check Digital Button values
            u8* buttonValues = wsNewArrayTmp(u8, numButtons);
            glfwGetJoystickButtons(playerNumber, buttonValues, numButtons);
            for (u32 i = 0; i < numButtons; ++i) {
                if (!(controllerMap[i] & analogMask)) { //  Ignore button press events for analog inputs
                    if (buttonValues[i] == GLFW_PRESS && !(controllerMap[i] & buttonStates)) {
                        wsLog(WS_LOG_HID, "Button %u pressed", i);
                        buttonStates |= controllerMap[i];
                        wsEvents.push(wsEvent(WS_EVENT_INPUT, WS_INPUT_TYPE_CONTROLLER, playerNumber, controllerMap[i],
                            WS_PRESS));
                    }
                }
                if (buttonValues[i] == GLFW_RELEASE && (controllerMap[i] & buttonStates)) {
                    wsLog(WS_LOG_HID, "Button %u released", i);
                    buttonStates ^= controllerMap[i];
                    wsEvents.push(wsEvent(WS_EVENT_INPUT, WS_INPUT_TYPE_CONTROLLER,playerNumber, controllerMap[i],
                        WS_RELEASE));
                }
            }
        #endif
    }
}
