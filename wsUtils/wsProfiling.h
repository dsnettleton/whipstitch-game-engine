/*
 * wsProfiling.h
 *
 *  Created on: Jul 12, 2012
 *      Author: dsnettleton
 *
 *      Includes a pair of macros to be called by all whipstitch engine functions, one at
 *      the beginning, and one at the end. These macros are ignored except when the
 *      program is compiled in profiling mode.
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

#ifndef WS_PROFILING_H_
#define WS_PROFILING_H_

#include "wsTime.h"
#include "wsLog.h"

#ifdef _PROFILE
    #include "wsProfileManager.h"

    struct _ws_function_profile_structure {
        const char* name;
        t64 startTime;
        //  Constructor creates the profile structure and sets the startTime
        _ws_function_profile_structure(const char* myName) {
            name = myName;
            startTime = wsGetTime();
            wsLog(WS_LOG_PROFILING, "Beginning Function:  \"%s\"\n", myName);
        }
        //  Destructor is called when the profile structure goes out of scope (when the
        //  function ends).
        ~_ws_function_profile_structure() {
            t64 timePassed = wsGetTime() - startTime;
            wsLog(WS_LOG_PROFILING, "  Ending Function:  \"%s\"\n    Time Spent = %f\n",
                            myName, timePassed);
        }
    };

    #define WS_PROFILE() \
        _ws_func_profile_structure _wsProfiler(__func__)

#else   //  If we're not compiling in Profiling mode
    #define WS_PROFILE()  //  Evaluates to nothing
#endif  /*  _PROFILE    */

#endif /* WS_PROFILING_H_ */
