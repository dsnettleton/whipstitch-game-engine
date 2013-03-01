/*
 * wsProfileManager.h
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
 *      of time each function beginning with the macro WS_PROFILE() (defined below)
 *      has run.
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

#ifndef WS_PROFILEMANAGER_H_
#define WS_PROFILEMANAGER_H_

#ifdef _PROFILE

#include "wsHashMap.h"

class wsProfileManager {
    private:
        struct _wsProfileManager_item {
            t64 mTotalTime;
            const char* mName;
        };
        wsHashMap<_wsProfileManager_item> *mMap;
        bool _mInitialized;
    public:
        /*  Override Constructor and Deconstructor  */
        wsProfileManager() { _mInitialized = false; }
        ~wsProfileManager() {}
        /*  Accessor */
        //  Returns the total amount of time that has been spent in the specified function
        t64 getTime(const char* functionName);
        /*  Operational Member Functions */
        //  Adds an object with the specified name and time to the hashmap
        void add(const char* functionName, t64 functionTime);
        //  Print the values of the items in the profile manager using wsLog
        void print(u16 printLog = WS_LOG_MAIN);
        //  Safely shut down the profile manager
        void shutDown();
        //  Create the profile manager, allowing for a given number of functions
        void startUp(u32 maxFunctions);
};

extern wsProfileManager wsProfiles;

#endif /*   _PROFILE    */

#endif /* WS_PROFILEMANAGER_H_ */
