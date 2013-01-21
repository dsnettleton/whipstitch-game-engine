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
