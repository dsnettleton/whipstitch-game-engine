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
#ifndef WS_GAME_H_
#define WS_GAME_H_

#include "wsEventManager.h"

class wsGame {
  public:
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
