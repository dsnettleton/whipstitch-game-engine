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
            wsEcho(WS_LOG_PROFILING, "Beginning Function:  \"%s\"\n", myName);
        }
        //  Destructor is called when the profile structure goes out of scope (when the
        //  function ends).
        ~_ws_function_profile_structure() {
            t64 timePassed = wsGetTime() - startTime;
            wsEcho(WS_LOG_PROFILING, "  Ending Function:  \"%s\"\n    Time Spent = %f\n",
                            myName, timePassed);
        }
    };

    #define WS_PROFILE() \
        _ws_func_profile_structure _wsProfiler(__func__)

#else   //  If we're not compiling in Profiling mode
    #define WS_PROFILE()  //  Evaluates to nothing
#endif  /*  _PROFILE    */

#endif /* WS_PROFILING_H_ */
