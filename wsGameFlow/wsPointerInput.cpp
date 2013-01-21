/**
 *    wsPointerInput.cpp
 *    Jan 8, 2013
 *    D. Scott Nettleton
 *
 *    This file declares the class wsPointerInput, which handles pointer
 *    controls, such as from a mouse, touchscreen, or (possibly) Wii
 *    remote pointer.
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
