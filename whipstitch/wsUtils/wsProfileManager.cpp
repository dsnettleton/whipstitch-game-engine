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

#ifdef _PROFILE
#include "wsProfileManager.h"
#include "wsMemoryStack.h"

/*  Define the startUp(...) function for this Engine Subsystem */
void wsProfileManager::startUp(u32 maxFunctions) {
    wsEcho(WS_LOG_PROFILING, "Profile Manager Starting Up\n");
    _mInitialized = true;
    //mMap = new wsHashMap<_wsProfileManager_item>(maxFunctions);
    mMap = wsNew< wsHashMap<_wsProfileManager_item>(maxFunctions) >();
}

/*  Define the shutDown(...) function for this Engine Subsystem */
void wsProfileManager::shutDown() {
    wsEcho(WS_LOG_PROFILING, "Profile Manager Shutting Down\n");
}

/*  Operational Member Functions    */
void wsProfileManager::add(const char* functionName, t64 functionTime) {
    wsAssert(_mInitialized, "The Profile Manager must first be intialized.");
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
    wsAssert(_mInitialized, "The Profile Manager must first be intialized.");
    wsEcho( printLog, "Profiling Results:\n");
    //TODO: Print profiling results
}

//  Global Singleton object
wsProfileManager wsProfiles;

#endif  /*  _PROFILE    */
