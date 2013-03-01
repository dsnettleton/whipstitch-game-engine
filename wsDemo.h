/**
 *  wsDemo.h
 *  March 1, 2013
 *  D. Scott Nettleton
 *
 *  This file declares the class wsDemo, which serves as an example
 *  of how to implement a game in the Whipstitch game engine.
 *
 *  This class inherits the class wsGame, and implements all operations
 *  specific to this particular game.
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
#ifndef WS_DEMO_H_
#define WS_DEMO_H_

#include "ws.h"

class wsDemo : public wsGame {
  private:
    //  Private Data Members
    wsCamera* cam;
    bool quit;
    bool animationsPaused;
  public:
    //  Constructors and Deconstructors
    wsDemo() : quit(false), animationsPaused(false) {}
    //  Setters and Getters
    //  Go Here
    //  Operational Methods
    void handleButtonEvents(u32 btnHash, u32 action);
    void handleControllerEvents(u64 controllerNum, u64 btnIndex, u32 action, f32 analogVal);
    void handleKeyboardEvents(u64 keyType, u64 btnIndex, u32 action);
    void handleMouseButtonEvents(u64 action, u64 btnIndex);
    void handleMouseMotionEvents(i32 posX, i32 posY, f32 dx, f32 dy);
    //  Inherited Methods
    void onStart();
    void onLoop();
    void onEvent(const wsEvent& event);
    void onExit();
};

#endif //  WS_DEMO_H_
