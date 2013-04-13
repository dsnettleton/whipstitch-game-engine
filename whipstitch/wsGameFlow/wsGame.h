/**
 *  wsGame.h
 *  March 1, 2013
 *  D. Scott Nettleton
 *
 *  This file declares the abstract base class wsGame.
 *  The wsGame class is inherited by a game object, which
 *  is different from one game to another. Eventually, the
 *  world builder will generate and compile wsGame objects.
 *  For now, it should be used as a template for implementing
 *  the Whipstitch Game Engine as an API.
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
#ifndef WS_GAME_H_
#define WS_GAME_H_

#include "wsEventManager.h"
#include "wsScene.h"

class wsGame {
  public:
    //  This returns the scene object to be updated by the game loop.
    virtual wsScene* getCurrentScene() = 0;
    //  This is run when the program opens, before the game loop starts.
    //  It runs immediately after the Engine subsystems are initialized.
    virtual void onStart() = 0;
    //  This contains functions which are run once every game loop.
    virtual void onLoop() = 0;
    //  This contains functions which handle events in the whipstitch
    //  game engine.
    virtual void onEvent(const wsEvent& event) = 0;
    //  This is run when the program closes, immediately before the
    //  Engine subsystems are shut down.
    virtual void onExit() = 0;
};

#endif //  WS_GAME_H_
