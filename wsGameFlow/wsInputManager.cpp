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

#include "wsInputManager.h"

wsInputManager wsInputs;

void wsInputManager::startUp() {
    _mInitialized = true;
}

void wsInputManager::shutDown() {
    //  Nothing to see here...
}
