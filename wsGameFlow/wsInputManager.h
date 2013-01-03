/*
 *  wsInputManager.h
 *
 *  Created on: Jan 1, 2013
 *      Author: dsnettleton
 *
 *      This file declares the class wsInputManager, which manages multiple human
 *      interface devices, queueing their inputs for use by the engine.
 *
 *      wsInputManager is an engine subsytem, and must be initialized via the startUp()
 *      function before it may be used. This is done through the engine startup command
 *      wsInit().
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

#ifndef WS_INPUT_MANAGER_H_
#define WS_INPUT_MANAGER_H_

#include "../wsUtils/wsPlatform.h"

class wsInputManager {
    private:
        //  True only when the startUp function has been called
        bool _mInitialized;
    public:
        /*  Empty Constructor and Destructor   */
        //  As an engine subsystem, the thread pool takes no action until explicitly
        //  initialized via the startUp(...) function.
        //  uninitialized via the shutDown() function.
        wsInputManager() : _mInitialized(false) {}
        ~wsInputManager() {}
        /*  Startup and shutdown functions  */
        //  Initializes the game loop
        void startUp();
        //  Uninitializes the game loop
        void shutDown();
        /*  Setters and Getters */
        /*  Operational Methods */
        void pollInputEvents();
        void pollForNewDevices();
};

extern wsInputManager wsInputs;

#endif  /*  WS_INPUT_MANAGER_H_ */
