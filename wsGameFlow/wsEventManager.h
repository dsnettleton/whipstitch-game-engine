/**
 *    wsEventManager.h
 *    Jan 9, 2013
 *    D. Scott Nettleton
 *
 *    This file declares several macros used for event management, as well as
 *    a definition of the wsEvent structure.
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
#ifndef WS_EVENT_MANAGER_H_
#define WS_EVENT_MANAGER_H_

#include "../wsUtils.h"

#define WS_EVENT_QUEUE_SIZE 32

#define WS_EVENT_NULL       0x0000
#define WS_EVENT_INPUT      0x0001
#define WS_EVENT_PHYSICS    0x0002

#define WS_RELEASE          0x00000001
#define WS_PRESS            0x00000002
#define WS_PRESS_OR_RELEASE 0x00000003
#define WS_ANALOG           0x00000004

#define WS_INPUT_TYPE_KEYBOARD      0x01
#define WS_INPUT_TYPE_MOUSE         0x02
#define WS_INPUT_TYPE_CONTROLLER    0x03

#define WS_NO_BUTTONS       0x00000000
#define WS_BUTTON_BOTTOM    0x00000001
#define WS_BUTTON_LEFT      0x00000002
#define WS_BUTTON_RIGHT     0x00000004
#define WS_BUTTON_TOP       0x00000008
#define WS_D_PAD_DOWN       0x00000010
#define WS_D_PAD_LEFT       0x00000020
#define WS_D_PAD_RIGHT      0x00000040
#define WS_D_PAD_UP         0x00000080
#define WS_BUTTON_START     0x00000100
#define WS_BUTTON_SELECT    0x00000200
#define WS_SHOULDER_LEFT    0x00000400
#define WS_SHOULDER_RIGHT   0x00000800
#define WS_SHOULDER_LEFT_2  0x00001000
#define WS_SHOULDER_RIGHT_2 0x00002000
#define WS_ANALOG_L_AXIS_X  0x00004000  //  Horizontal axis of the left analog stick
#define WS_ANALOG_L_AXIS_Y  0x00008000  //  Vertical axis of the left analog stick
#define WS_ANALOG_R_AXIS_X  0x00010000  //  Horizontal axis of the right analog stick
#define WS_ANALOG_R_AXIS_Y  0x00020000  //  Vertical axis of the right analog stick
#define WS_ANALOG_L_CLICK   0x00040000  //  Clicking of the left analog stick
#define WS_ANALOG_R_CLICK   0x00080000  //  Clicking of the right analog stick
#define WS_BUTTON_SYSTEM    0x00100000
#define WS_ACCELEROMETER_X  0x00200000  //  X axis of an accelerometer
#define WS_ACCELEROMETER_Y  0x00400000  //  X axis of an accelerometer
#define WS_ACCELEROMETER_Z  0x00800000  //  X axis of an accelerometer

//  Some joysticks use a single axis for D-pad controls
//  These are defined to handle them specially
#define WS_D_PAD_AXIS_X     0x00000060
#define WS_D_PAD_AXIS_Y     0x00000090

#define WS_ANALOG_FILTERING_CONSTANT    0.1

#define WS_DEFAULT_DEAD_ZONE    0.2
#define WS_DEFAULT_SENSITIVITY  1.0

#define WS_PLAYER_1     0
#define WS_PLAYER_2     1
#define WS_PLAYER_3     2
#define WS_PLAYER_4     3
#define WS_PLAYER_5     4
#define WS_PLAYER_6     5
#define WS_PLAYER_7     6
#define WS_PLAYER_8     7
#define WS_PLAYER_9     8
#define WS_PLAYER_10    9
#define WS_PLAYER_11    10
#define WS_PLAYER_12    11
#define WS_PLAYER_13    12
#define WS_PLAYER_14    13
#define WS_PLAYER_15    14
#define WS_PLAYER_16    15

#define WS_MOUSE_BUTTON_LEFT        0x00000001  //  Left mouse button
#define WS_MOUSE_BUTTON_RIGHT       0x00000002  //  Right mouse button
#define WS_MOUSE_BUTTON_MIDDLE      0x00000004  //  Middle mouse button
#define WS_MOUSE_SCROLL_DOWN        0x00000008  //  Scrolling the mouse wheel down
#define WS_MOUSE_SCROLL_UP          0x00000010  //  Scrolling the mouse wheel up
#define WS_MOUSE_SCROLL_LEFT        0x00000020  //  Scrolling the mouse wheel to the left
#define WS_MOUSE_SCROLL_RIGHT       0x00000040  //  Scrolling the mouse wheel to the right
#define WS_MOUSE_BUTTON_4           0x00000080
#define WS_MOUSE_BUTTON_5           0x00000100
#define WS_MOUSE_MOTION_X           0x00000200
#define WS_MOUSE_MOTION_Y           0x00000400

#ifndef WS_MAX_CONTROLLERS
#define WS_MAX_CONTROLLERS 4
#endif

#define WS_KB_KEY           0x01
#define WS_KB_SPECIAL_KEY   0x02

#define WS_KEYS_NONE        0

//  First 64-bit integer
#define WS_KEY_A            0x0000000000000001
#define WS_KEY_B            0x0000000000000002
#define WS_KEY_C            0x0000000000000004
#define WS_KEY_D            0x0000000000000008
#define WS_KEY_E            0x0000000000000010
#define WS_KEY_F            0x0000000000000020
#define WS_KEY_G            0x0000000000000040
#define WS_KEY_H            0x0000000000000080
#define WS_KEY_I            0x0000000000000100
#define WS_KEY_J            0x0000000000000200
#define WS_KEY_K            0x0000000000000400
#define WS_KEY_L            0x0000000000000800
#define WS_KEY_M            0x0000000000001000
#define WS_KEY_N            0x0000000000002000
#define WS_KEY_O            0x0000000000004000
#define WS_KEY_P            0x0000000000008000
#define WS_KEY_Q            0x0000000000010000
#define WS_KEY_R            0x0000000000020000
#define WS_KEY_S            0x0000000000040000
#define WS_KEY_T            0x0000000000080000
#define WS_KEY_U            0x0000000000100000
#define WS_KEY_V            0x0000000000200000
#define WS_KEY_W            0x0000000000400000
#define WS_KEY_X            0x0000000000800000
#define WS_KEY_Y            0x0000000001000000
#define WS_KEY_Z            0x0000000002000000
#define WS_KEY_0            0x0000000004000000
#define WS_KEY_1            0x0000000008000000
#define WS_KEY_2            0x0000000010000000
#define WS_KEY_3            0x0000000020000000
#define WS_KEY_4            0x0000000040000000
#define WS_KEY_5            0x0000000080000000
#define WS_KEY_6            0x0000000100000000
#define WS_KEY_7            0x0000000200000000
#define WS_KEY_8            0x0000000400000000
#define WS_KEY_9            0x0000000800000000
//#define WS_KEY_PLUS       0x0000001000000000
//#define WS_KEY_MINUS      0x0000002000000000
#define WS_KEY_EQUALS       0x0000004000000000
#define WS_KEY_UNDERSCORE   0x0000008000000000
#define WS_KEY_TILDE        0x0000010000000000
#define WS_KEY_BRACKET_L    0x0000020000000000
#define WS_KEY_BRACKET_R    0x0000040000000000
#define WS_KEY_SEMICOLON    0x0000080000000000
#define WS_KEY_APOSTROPHE   0x0000100000000000
#define WS_KEY_COMMA        0x0000200000000000
#define WS_KEY_PERIOD       0x0000400000000000
#define WS_KEY_SLASH_FWD    0x0000800000000000
#define WS_KEY_SLASH_BKWD   0x0001000000000000
#define WS_KEY_SPACE        0x0002000000000000
#define WS_KEY_TAB          0x0004000000000000
#define WS_KEY_SHIFT_L      0x0008000000000000
#define WS_KEY_SHIFT_R      0x0010000000000000
#define WS_KEY_CTRL_L       0x0020000000000000
#define WS_KEY_CTRL_R       0x0040000000000000
#define WS_KEY_ALT_L        0x0080000000000000
#define WS_KEY_ALT_R        0x0100000000000000
#define WS_KEY_ENTER        0x0200000000000000
#define WS_KEY_BACKSPACE    0x0400000000000000
#define WS_KEY_DELETE       0x0800000000000000
#define WS_KEY_DOWN         0x1000000000000000
#define WS_KEY_LEFT         0x2000000000000000
#define WS_KEY_RIGHT        0x4000000000000000
#define WS_KEY_UP           0x8000000000000000
//  Second 64-bit integer
#define WS_KEY_F1           0x0000000000000001
#define WS_KEY_F2           0x0000000000000002
#define WS_KEY_F3           0x0000000000000004
#define WS_KEY_F4           0x0000000000000008
#define WS_KEY_F5           0x0000000000000010
#define WS_KEY_F6           0x0000000000000020
#define WS_KEY_F7           0x0000000000000040
#define WS_KEY_F8           0x0000000000000080
#define WS_KEY_F9           0x0000000000000100
#define WS_KEY_F10          0x0000000000000200
#define WS_KEY_F11          0x0000000000000400
#define WS_KEY_F12          0x0000000000000800
//  Keys on the Number pad
#define WS_KEY_NUM_0        0x0000000000001000
#define WS_KEY_NUM_1        0x0000000000002000
#define WS_KEY_NUM_2        0x0000000000004000
#define WS_KEY_NUM_3        0x0000000000008000
#define WS_KEY_NUM_4        0x0000000000010000
#define WS_KEY_NUM_5        0x0000000000020000
#define WS_KEY_NUM_6        0x0000000000040000
#define WS_KEY_NUM_7        0x0000000000080000
#define WS_KEY_NUM_8        0x0000000000100000
#define WS_KEY_NUM_9        0x0000000000200000
#define WS_KEY_NUM_PLUS     0x0000000000400000
#define WS_KEY_NUM_MINUS    0x0000000000800000
#define WS_KEY_NUM_STAR     0x0000000001000000
#define WS_KEY_NUM_SLASH    0x0000000002000000
#define WS_KEY_NUM_PERIOD   0x0000000004000000
#define WS_KEY_NUM_ENTER    0x0000000008000000
#define WS_KEY_HOME         0x0000000010000000
#define WS_KEY_END          0x0000000020000000
#define WS_KEY_PG_DN        0x0000000040000000
#define WS_KEY_PG_UP        0x0000000080000000
#define WS_KEY_INSERT       0x0000000100000000
#define WS_KEY_ESC          0x0000000200000000

struct wsEvent {
    u32 eventType;
    u32 eventSubType;
    u64 uVal1;
    u64 uVal2;
    u32 uVal3;
    f64 fVal1;
    f64 fVal2;
    f32 fVal3;
    wsEvent(u32 type = WS_EVENT_NULL, u32 subType = WS_EVENT_NULL, u64 u1 = 0, u64 u2 = 0, u32 u3 = 0,
        f64 f1 = 0.0f, f32 f2 = 0.0f, f32 f3 = 0.0f) : eventType(type), eventSubType(subType),
        uVal1(u1), uVal2(u2), uVal3(u3), fVal1(f1), fVal2(f2), fVal3(f3) { }
    void print(u32 logs = WS_LOG_EVENTS) {
        wsLog(logs, "Type     %u\n  Subtype  %u\n  uVal1    %lu\n  uVal2    %lu\n  uVal3    %u\n  fVal1    %f\n  fVal2    %f\n"
        "  fVal3    %f\n", eventType, eventSubType, uVal1, uVal2, uVal3, fVal1, fVal2, fVal3);
    }
};

class wsEventManager {
    private:
        wsStack<wsEvent>* events;
        //  True only when the startUp function has been called
        bool _mInitialized;
    public:
        /*  Empty Constructor and Destructor   */
        //  As an engine subsystem, the event manager takes no action until explicitly
        //  initialized via the startUp(...) function.
        //  uninitialized via the shutDown() function.
        wsEventManager() : _mInitialized(false) {}
        ~wsEventManager() {}
        /*  Startup and shutdown functions  */
        //  Uninitializes the game loop
        void shutDown();
        //  Initializes the game loop
        void startUp();
        /*  Operational Methods */
        bool isNotEmpty() {
            wsAssert(_mInitialized, "Initialize the Event Manager first.");
            return events->isNotEmpty();
        }
        void push(const wsEvent& my) {
            wsAssert(_mInitialized, "Initialize the Event Manager first.");
            events->push(my);
        }
        wsEvent pop() {
            wsAssert(_mInitialized, "Initialize the Event Manager first.");
            return events->pop();
        }
};

extern wsEventManager wsEvents;

#endif //  WS_EVENT_MANAGER_H_
