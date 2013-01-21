/**
 *    wsPointerInput.h
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
#ifndef WS_POINTER_INPUT_H_
#define WS_POINTER_INPUT_H_

#include "wsInputMethod.h"

class wsPointerInput : public wsInputMethod {
    private:
        //  Private Data Members
        f32 dx;
        f32 dy;
        f32 posX;
        f32 posY;
        bool _mInitialized;
    public:
        //  Constructors and Deconstructors
        wsPointerInput();
        //  Setters and Getters
        f32 getDx() { return dx; }
        f32 getDy() { return dy; }
        f32 getX() { return posX; }
        f32 getY() { return posY; }
        bool getInitialized() { return _mInitialized; }
        //  Operational Methods
        void poll() { /* Intentionally Empty */ }    //  Inherited from wsInputMethod
        void pressButton(u32 btnIndex);
        void releaseButton(u32 btnIndex);
        void setPos(i32 newPosX, i32 newPosY);
        void swapFrames();
};

#endif //  WS_POINTER_INPUT_H_
