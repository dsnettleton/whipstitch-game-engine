/**
 *  wsInputMethod.h
 *  Jan 3, 2013
 *  D. Scott Nettleton
 *
 *  This file declares the class wsInputMethod, which is an abstract base
 *  class representing any sort of control device.
 *  
 *  Buttons are represented using a standard controller layout:
 *    Four face buttons on the left, plus two in the center
 *    Two clickable Dual analog sticks
 *    A single direction pad
 *    Two shoulder buttons
 *  Analog controls are handled with the same on/off representation,
 *  indicating a useable value (a value higher than the dead zone),
 *  as well as a separate float value in the range (-1.0f, 1.0f).
 *  Consequently, analog controls must be checked for their value
 *  regardless of whether they have been triggered on or off.
 *  Each input method has its own mask representing which controls
 *  are analog and which are not.
 *
 *  Relative controls are also determined with a bitmask and utilze
 *  a float value to indicate the amount of change undergone by that
 *  control. An common example of a relative control would be a mouse
 *  or trackball axis. This is a control that triggers an event when
 *  a change is detected, but has an analog value. The value returned
 *  is relative to its previous position, rather than absolute, and
 *  is therefore not clamped to the range (-1.0f, 1.0f). As such,
 *  games which utilize relative controls as an alternative to analog
 *  controls should incorporate a multiplier with the analog control
 *  in order to place the two controls on relatively even footing.
 *  The sensitivity value for each control may be used to do this within
 *  the engine itself.
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
#ifndef WS_INPUT_METHOD_H_
#define WS_INPUT_METHOD_H_

#include "../wsUtils.h"
#include "../wsGraphics/wsScreen.h"
#include "wsEventManager.h"

//  For both analog and relative controls
struct wsAnalogControl {
    u32 controlIndex;   //  The control or controls this corresponds with
    f32 value;
    f32 deadZone;   //  The minimum analog value which must be achieved before input is registered
    f32 sensitivity;
    wsAnalogControl() : controlIndex(WS_NO_BUTTONS),
                        value(0.0f),
                        deadZone(WS_DEFAULT_DEAD_ZONE),
                        sensitivity(WS_DEFAULT_SENSITIVITY) {}
    f32 set(f32 newValue) {
        if (wsAbs(newValue) > deadZone) {
            //value = newValue*sensitivity;
            lowPassFilter(newValue, 0.1);
        }
        else {
            value = 0.0f;
        }
        return value;
    }
    void lowPassFilter(f32 unfilteredValue, t32 timeSinceLastVal) {
        //  Used to filter out signal noise (jitter) from analog controls
        f32 alpha = timeSinceLastVal / (WS_ANALOG_FILTERING_CONSTANT + timeSinceLastVal);
        value = (1.0f - alpha) * value + alpha*unfilteredValue;
    }
};

class wsInputMethod {
  protected:
    //  Private Data Members
    u32 analogMask; //  Button mask indicating which controls have analog values
    u32 relativeMask;   //  Button mask indicating which controls have relative values (e.g. mouse movement)
    u32 buttonStates;
    u32 prevButtonStates;
  public:
    //  Constructors and Deconstructors
    wsInputMethod() : analogMask(0), relativeMask(0), buttonStates(0), prevButtonStates(0) {}
    //  Setters and Getters
    u32 getAlteredButtons() {
        //  Return any buttons newly up or down, as well as any analog buttons which are down,
        //  regardless of how long they've been down.
        return (buttonStates ^ prevButtonStates) | (buttonStates & analogMask);
    }
    u32 getNewlyPressedButtons() {
        //  Return any buttons newly pressed, as well as any analog buttons which are down,
        //  regardless of how long they've been down.
        return ((prevButtonStates ^ buttonStates) & buttonStates) | (buttonStates & analogMask);
    }
    u32 getNewlyReleasedButtons() {
        //  Returns any buttons newly raised, including analog controls which have crossed
        //  inside their dead zone
        return (prevButtonStates ^ buttonStates) & prevButtonStates;
    }
    //  Operational Methods
    virtual void poll() = 0;    //  Run once per game loop to update input values
    void swapFrames() { prevButtonStates = buttonStates; }
};

#endif //  WS_INPUT_METHOD_H_
