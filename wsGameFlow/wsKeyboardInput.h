/**
 *  wsKeyboardInput.h
 *  Jan 4, 2013
 *  D. Scott Nettleton
 *
 *  This file declares the class wsKeyboard, which is a generic controller
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
#ifndef WS_KEYBOARD_INPUT_H_
#define WS_KEYBOARD_INPUT_H_

#include "../wsUtils.h"
#include "../wsGraphics/wsScreen.h"
#include "wsEventManager.h"

struct wsKeyArray {
    u64 keyButtonStates;
    u64 specButtonStates;
    wsKeyArray(u64 myKeys=WS_KEYS_NONE, u64 mySpec=WS_KEYS_NONE) : keyButtonStates(myKeys), specButtonStates(mySpec) {}
    wsKeyArray operator&(const wsKeyArray& other) const {   //  Bitwise AND
        return wsKeyArray( keyButtonStates & other.keyButtonStates, specButtonStates & other.specButtonStates );
    }
    wsKeyArray operator|(const wsKeyArray& other) const {   //  Bitwise OR
        return wsKeyArray( keyButtonStates | other.keyButtonStates, specButtonStates | other.specButtonStates );
    }
    wsKeyArray operator^(const wsKeyArray& other) const {   //  Bitwise XOR
        return wsKeyArray( keyButtonStates ^ other.keyButtonStates, specButtonStates ^ other.specButtonStates );
    }
    wsKeyArray operator~() const {  //  Bitwise Complement
        return wsKeyArray( ~keyButtonStates, ~specButtonStates );
    }
    wsKeyArray& operator&=(const wsKeyArray& other) {
        keyButtonStates &= other.keyButtonStates;
        specButtonStates &= other.specButtonStates;
        return *this;
    }
    wsKeyArray& operator|=(const wsKeyArray& other) {
        keyButtonStates |= other.keyButtonStates;
        specButtonStates |= other.specButtonStates;
        return *this;
    }
    wsKeyArray& operator^=(const wsKeyArray& other) {
        keyButtonStates ^= other.keyButtonStates;
        specButtonStates ^= other.specButtonStates;
        return *this;
    }
};

class wsKeyboardInput {
    private:
        //  Private Data Members
        wsKeyArray buttonStates;
        wsKeyArray prevButtonStates;
        bool typingMode;
    public:
        //  Constructors and Deconstructors
        wsKeyboardInput() : buttonStates(), prevButtonStates(), typingMode(false) {}
        //  Setters and Getters
        wsKeyArray getAlteredButtons();
        wsKeyArray getNewlyPressedButtons();
        wsKeyArray getNewlyReleasedButtons();
        bool isKeyDown(u64 keyIndex) { return keyIndex & buttonStates.keyButtonStates; }
        bool isSpecialKeyDown(u64 specKeyIndex) { return specKeyIndex & buttonStates.specButtonStates; }
        bool getTypingMode() { return typingMode; }
        //  Operational Methods
        void activateGameMode();
        void activateTypingMode();
        void pressKey(u64 keyIndex);    //  Set the indicated keys to down state
        void pressSpecKey(u64 specKeyIndex);    //  set the indicated special keys to down state
        void releaseKey(u64 keyIndex);  //  Set the indicated keys to up state
        void releaseSpecKey(u64 specKeyIndex);  //  set the indicated special keys to up state
        void swapFrames();
};

#endif //  WS_KEYBOARD_INPUT_H_
