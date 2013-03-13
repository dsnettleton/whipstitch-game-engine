/**
 *  wsKeyboardInput.cpp
 *  Jan 4, 2013
 *  D. Scott Nettleton
 *
 *  This file implements the class wsKeyboard, which is a generic controller
 *  for keyboard inputs into the Whipstitch Game Engine. Keyboard input
 *  may be used in a similar manner to wsInputMethod-derived controls.
 *  This is called Game Mode. But keyboards may also switch to typing
 *  Mode, which calls on the system to provide input for text boxes, etc.
 *  On certain platforms, this is handled by requesting special menus.
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

#include "wsKeyboardInput.h"

wsKeyArray wsKeyboardInput::getAlteredButtons() {
    return buttonStates ^ prevButtonStates;
}

wsKeyArray wsKeyboardInput::getNewlyPressedButtons() {
    return (buttonStates ^ prevButtonStates) & buttonStates;
}

wsKeyArray wsKeyboardInput::getNewlyReleasedButtons() {
    return (buttonStates ^ prevButtonStates) & prevButtonStates;
}

void wsKeyboardInput::pressKey(u64 keyIndex) {    //  Set the indicated keys to down state
    buttonStates.keyButtonStates |= keyIndex;
}

void wsKeyboardInput::pressSpecKey(u64 specKeyIndex) {    //  set the indicated special keys to down state
    buttonStates.specButtonStates |= specKeyIndex;
}

void wsKeyboardInput::releaseKey(u64 keyIndex) {  //  Set the indicated keys to up state
    if (buttonStates.keyButtonStates & keyIndex) { buttonStates.keyButtonStates ^= keyIndex; }
}

void wsKeyboardInput::releaseSpecKey(u64 specKeyIndex) {  //  set the indicated special keys to up state
    if (buttonStates.specButtonStates & specKeyIndex) { buttonStates.specButtonStates ^= specKeyIndex; }
}

void wsKeyboardInput::swapFrames() {
    prevButtonStates = buttonStates;
}
