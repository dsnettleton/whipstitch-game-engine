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
#ifndef WS_DEMO_H_
#define WS_DEMO_H_

#include "whipstitch/ws.h"

class wsDemo : public wsGame {
  private:
    //  Private Data Members
    wsScene* scene;
    wsCamera* cam;
    bool quit;
    bool animationsPaused;
  public:
    //  Constructors and Deconstructors
    wsDemo() : quit(false), animationsPaused(false) {}
    //  Operational Methods
    void handleButtonEvents(u32 btnHash, u32 action);
    void handleControllerEvents(u64 controllerNum, u64 btnIndex, u32 action, f32 analogVal);
    void handleKeyboardEvents(u64 keyType, u64 btnIndex, u32 action);
    void handleMouseButtonEvents(u64 action, u64 btnIndex);
    void handleMouseMotionEvents(i32 posX, i32 posY, f32 dx, f32 dy);
    //  Inherited Methods
    wsScene* getCurrentScene() { return scene; }
    void onStart();
    void onLoop();
    void onEvent(const wsEvent& event);
    void onExit();
};

#endif //  WS_DEMO_H_
