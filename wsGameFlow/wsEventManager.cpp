/**
 *    wsEvents.cpp
 *    Jan 9, 2013
 *    D. Scott Nettleton
 *
 *    This file declares the event queue.
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

#include "wsEventManager.h"

wsEventManager wsEvents;

void wsEventManager::startUp() {
    _mInitialized = true;
    events = wsNew(wsStack<wsEvent>, wsStack<wsEvent>(WS_EVENT_QUEUE_SIZE));
}

void wsEventManager::shutDown() {
    //  Nothin' here
}
