/*
 *  wsTask.h
 *
 *  Created on: Dec 25, 2012
 *      Author: dsnettleton
 *
 *      This file declares the class wsTask, which is the base class for
 *      a task to be run on the Whipstitch Engine's thread pool.
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

#ifndef WS_TASK_H_
#define WS_TASK_H_

#include "../wsUtils.h"

class wsTask {
    private:
    public:
        virtual ~wsTask() {}
        virtual void run(u32 threadNum) = 0;//{ wsLog(WS_LOG_THREADS, "NULL Task Running."); }
};

class wsTask_test : public wsTask {
    public:
        u32 iVal;
        void run(u32 threadNum) {
            wsLog(WS_LOG_MAIN, "Running thread %u - iVal = %u", threadNum, iVal);
        }
};

#endif  /*  WS_TASK_H_  */
