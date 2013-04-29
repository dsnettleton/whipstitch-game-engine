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

#include "wsThreadPool.h"

wsThreadPool wsThreads;

#ifdef WS_OS_FAMILY_UNIX
void wsThreadPool::lockMutex(wsMutex* myMutex) {
    pthread_mutex_lock(myMutex);
}
void wsThreadPool::unlockMutex(wsMutex* myMutex) {
    pthread_mutex_unlock(myMutex);
}
void* wsRunThread(void* threadID) {
    wsAssert(wsThreads.isInitialized(), "The object wsTasks must be initialized via the startUp() method before use.");
    //  Create a single thread and begin running...
    wsQueue<wsTask*>* tasks = wsThreads.getTaskList();
    u32 threadNum = *(u32*)threadID;
    wsTask* myTask = NULL;
    wsMutex* listMutex = wsThreads.getListMutex();
    wsMutex* logMutex = wsThreads.getLogMutex();
    while (!wsThreads.killSignalReceived()) {
        wsThreads.lockMutex(listMutex);
        if (tasks->isNotEmpty()) {
            wsEcho(WS_LOG_THREADS, "Removing task from front of queue; running on thread number %u", threadNum);
            myTask = tasks->pop();
        }
        wsThreads.unlockMutex(listMutex);
        if (myTask != NULL) {
            myTask->run(threadNum);
            myTask = NULL;
        }
    }
    wsThreads.lockMutex(logMutex);
    wsEcho(WS_LOG_THREADS, "Exiting thread number %u", threadNum);
    wsThreads.unlockMutex(logMutex);
    pthread_exit(NULL);
}
#elif defined(WS_OS_FAMILY_WINDOWS)
#endif  /*  Whipstitch OS families  */

void wsThreadPool::startUp() {
    _mInitialized = true;
    numThreads = maxThreads = WS_NUM_CORES - 1; //  Save one processor for the main thread
    if (numThreads <= 0) {
        wsEcho(WS_LOG_THREADS, "No extra threads have been created.");
    }
    threads = wsNewArray(pthread_t, numThreads);
    threadIndices = wsNewArray(u32, numThreads);
    for (u32 i = 0; i < numThreads; ++i) {
        threadIndices[i] = i;
    }
    #ifdef WS_OS_FAMILY_UNIX
        threadAttributes = wsNew(pthread_attr_t, pthread_attr_t());
        pthread_attr_init(threadAttributes);
        pthread_attr_setdetachstate(threadAttributes, PTHREAD_CREATE_JOINABLE);
    #elif defined(WS_OS_FAMILY_WINDOWS)
    #endif  /*  Whipstitch OS families  */
    listMutex = wsNew(wsMutex, wsMutex());
    logMutex = wsNew(wsMutex, wsMutex());
    wsInitMutex(listMutex);
    wsInitMutex(logMutex);
    taskList = wsNew(wsQueue<wsTask*>, wsQueue<wsTask*>(WS_MAX_TASK_QUEUE_SIZE));
    tasksRunning = 0;
    wsEcho(WS_LOG_THREADS, "Initializing %u Worker Threads", numThreads);
    for (u32 i = 0; i < numThreads; ++i) {
        runThread(i);
    }
}

void wsThreadPool::shutDown() {
    _mKillThreads = true;
    #ifdef WS_OS_FAMILY_UNIX
        /*  Allow All Currently Running Threads to End Safely   */
        void* status;
        for (u32 i = 0; i < numThreads; ++i) {
            pthread_join(threads[i], &status);
        }
    #elif defined(WS_OS_FAMILY_WINDOWS)
    #endif
}

void wsThreadPool::pushTask(wsTask* task) {
    wsAssert(_mInitialized, "The object wsTasks must be initialized via the startUp() method before use.");
    /*  Get a Mutex Lock for the task list  */
    lockMutex(listMutex);

    if (numThreads <= 0) {
        task->run(0);
        return;
    }
    wsAssert(taskList->isNotFull(), "Cannot push task onto the queue.");
    taskList->push(task);

    /*  Unlock the mutex    */
    unlockMutex(listMutex);
}

void wsThreadPool::runThread(const u32 threadNum) {
    wsAssert(_mInitialized, "The object wsTasks must be initialized via the startUp() method before use.");
    wsAssert(threadNum < numThreads, "Cannot run thread beyond the number of processors.");
#ifdef WS_OS_FAMILY_UNIX
    pthread_create(&threads[threadNum], threadAttributes, wsRunThread, (void*)&threadIndices[threadNum]);
#elif defined(WS_OS_FAMILY_WINDOWS)
#endif  /*  Whipstitch OS families  */
}

void wsThreadPool::waitForCompletion() {
    wsAssert(_mInitialized, "The object wsTasks must be initialized via the startUp() method before use.");
    while (tasksRunning > 0) {}
}
