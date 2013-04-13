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
