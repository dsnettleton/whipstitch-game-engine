/**
 *    wsPointerInput.cpp
 *    Jan 8, 2013
 *    D. Scott Nettleton
 *
 *    This file declares the class wsPointerInput, which handles pointer
 *    controls, such as from a mouse, touchscreen, or (possibly) Wii
 *    remote pointer.
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

#include "wsPointerInput.h"

wsPointerInput::wsPointerInput() : dx(0.0f), dy(0.0f), posX(-1.0f), posY(-1.0f), _mInitialized(false) {
    analogMask = WS_NO_BUTTONS;
}

void wsPointerInput::pressButton(u32 btnIndex) {
    buttonStates |= btnIndex;
}

void wsPointerInput::releaseButton(u32 btnIndex) {
    if (buttonStates & btnIndex) { buttonStates ^= btnIndex; }
}

void wsPointerInput::setPos(i32 newPosX, i32 newPosY) {
    _mInitialized = true;
    dx = newPosX - posX;
    dy = newPosY - posY;
    posX = newPosX;
    posY = newPosY;
}

void wsPointerInput::swapFrames() {
    prevButtonStates = buttonStates;
}
