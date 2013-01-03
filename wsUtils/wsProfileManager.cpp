/*
 * wsProfileManager.cpp
 *
 *  Created on: Jul 24, 2012
 *      Author: dsnettleton
 *
 *      This file declares the Whipstitch Engine Subsystem wsProfileManager, which is
 *      initialized directly after the memory manager (wsMemoryStack).
 *
 *      The wsProfileManager is implemented using a singleton instance, wsProfiles.
 *
 *      The Profile Manager contains a hash map listing function names and the amount
 *      of time each function beginning with the macro WS_PROFILE() (defined in
 *      wsProfileManager.h) has run.
 *
 *      The macros and objects defined herein are only used when compiling in Profile
 *      mode, by declaring _PROFILE.
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

#ifdef _PROFILE
#include "wsProfileManager.h"

/*  Define the startUp(...) function for this Engine Subsystem */
void wsProfileManager::startUp(u32 maxFunctions) {
    wsLog(WS_LOG_PROFILING, "Profile Manager Starting Up\n");
    //mMap = new wsHashMap<_wsProfileManager_item>(maxFunctions);
    mMap = wsNew< wsHashMap<_wsProfileManager_item>(maxFunctions) >();
}

/*  Define the shutDown(...) function for this Engine Subsystem */
void wsProfileManager::shutDown() {
    wsLog(WS_LOG_PROFILING, "Profile Manager Shutting Down\n");
}

/*  Operational Member Functions    */
void wsProfileManager::add(const char* functionName, t64 functionTime) {
    _wsProfileManager_item myItem;
    if (mMap->retrieve(wsHash(functionName), myItem)) {
        myItem.mTotalTime += functionTime;
        mMap->replace(wsHash(functionName), myItem);
    }
    else {
        mMap->insert(wsHash(functionName), myItem);
    }
}

void wsProfileManager::print(u16 printLog) {
    wsLog( printLog, "Profiling Results:\n");
    //TODO: Print profiling results
}

//  Global Singleton object
wsProfileManager wsProfiles;

#endif  /*  _PROFILE    */
