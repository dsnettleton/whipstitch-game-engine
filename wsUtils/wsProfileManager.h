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
    public:
        /*  Override Constructor and Deconstructor  */
        wsProfileManager() {}
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
