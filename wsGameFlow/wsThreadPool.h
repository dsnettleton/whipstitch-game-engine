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

#ifndef WS_THREAD_POOL_H_
#define WS_THREAD_POOL_H_

#define WS_MAX_TASK_QUEUE_SIZE 64

#include "wsTask.h"

#ifdef WS_OS_FAMILY_UNIX
    #include <pthread.h>
    #define wsMutex pthread_mutex_t
    #define wsInitMutex(myMutex) pthread_mutex_init(myMutex, NULL)
#elif defined(WS_OS_FAMILY_WINDOWS)
    #define wsMutex
#endif  /*  Whipstitch OS families  */

class wsThreadPool {
    private:
#ifdef WS_OS_FAMILY_UNIX
        pthread_t* threads;
        pthread_attr_t* threadAttributes;
#elif defined(WS_OS_FAMILY_WINDOWS)
#endif  /*  Whipstitch OS families  */
        wsMutex* listMutex;
        wsMutex* logMutex;
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
        pthread_attr_t* getThreadAttributes() { return threadAttributes; }
#elif defined(WS_OS_FAMILY_WINDOWS)
#endif  /*  Whipstitch OS families  */
        wsMutex* getListMutex() { return listMutex; }
        wsMutex* getLogMutex() { return logMutex; }
        u32 getMaxThreads() { return maxThreads; }
        u32 getNumThreads() { return numThreads; }
        wsQueue<wsTask*>* getTaskList() { return taskList; }
        u32 getTasksRunning() { return tasksRunning; }
        bool isComplete() { return (tasksRunning == 0); }
        bool isInitialized() { return _mInitialized; }
        bool killSignalReceived() { return _mKillThreads; }
        void setNumThreads(u32 myNumThreads) { numThreads = myNumThreads; }
        /*  Operational Methods */
        void lockMutex(wsMutex* myMutex);
        void unlockMutex(wsMutex* myMutex);
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
