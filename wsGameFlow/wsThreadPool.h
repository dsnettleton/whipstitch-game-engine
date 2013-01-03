/*
 *  wsThreadPool.h
 *
 *  Created on: Dec 25, 2012
 *      Author: dsnettleton
 *
 *      This file declares the class wsThreadPool, which manages multiple threads on
 *      multi-processor systems. Concurrent tasks are added to the thread pool, and
 *      are run as soon as a processor is available.
 *
 *      wsThreadPool is an engine subsytem, and must be initialized via the startUp()
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

#ifndef WS_THREAD_POOL_H_
#define WS_THREAD_POOL_H_

#define WS_MAX_TASK_QUEUE_SIZE 64

#include "wsTask.h"

#ifdef WS_OS_FAMILY_UNIX
#include <pthread.h>
#elif defined(WS_OS_FAMILY_WINDOWS)
#endif  /*  Whipstitch OS families  */

class wsThreadPool {
    private:
#ifdef WS_OS_FAMILY_UNIX
        pthread_t* threads;
        pthread_attr_t* threadAttributes;
        pthread_mutex_t* listMutex;
        pthread_mutex_t* logMutex;
#elif defined(WS_OS_FAMILY_WINDOWS)
#endif  /*  Whipstitch OS families  */
        wsQueue<wsTask*>* taskList;
        u32* threadIndices;
        u32 maxThreads;
        u32 numThreads;
        u32 tasksRunning;
        //  Signal to extant threads to stop running
        bool _mKillThreads;
        //  True only when the startUp function has been called
        bool _mInitialized;
    public:
        /*  Empty Constructor and Destructor   */
        //  As an engine subsystem, the thread pool takes no action until explicitly
        //  initialized via the startUp(...) function.
        //  uninitialized via the shutDown() function.
        wsThreadPool() : _mKillThreads(false), _mInitialized(false) {}
        ~wsThreadPool() {}
        /*  Startup and shutdown functions  */
        //  Uninitializes the game loop
        void shutDown();
        //  Initializes the game loop
        void startUp();
        /*  Setters and Getters */
#ifdef WS_OS_FAMILY_UNIX
        pthread_mutex_t* getListMutex() { return listMutex; }
        pthread_mutex_t* getLogMutex() { return logMutex; }
        pthread_attr_t* getThreadAttributes() { return threadAttributes; }
#elif defined(WS_OS_FAMILY_WINDOWS)
#endif  /*  Whipstitch OS families  */
        u32 getMaxThreads() { return maxThreads; }
        u32 getNumThreads() { return numThreads; }
        wsQueue<wsTask*>* getTaskList() { return taskList; }
        u32 getTasksRunning() { return tasksRunning; }
        bool isComplete() { return (tasksRunning == 0); }
        bool isInitialized() { return _mInitialized; }
        bool killSignalReceived() { return _mKillThreads; }
        void setNumThreads(u32 myNumThreads) { numThreads = myNumThreads; }
        /*  Operational Methods */
        void pushTask(wsTask* task);
        void runThread(const u32 threadNum);
        void waitForCompletion();
};

extern wsThreadPool wsThreads;

#ifdef WS_OS_FAMILY_UNIX
void* wsRunThread(void* threadID);
#elif defined(WS_OS_FAMILY_WINDOWS)
#endif  /*  Whipstitch OS families  */


#endif  /*  WS_THREAD_POOL_H_   */
