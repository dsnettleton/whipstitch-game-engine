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

#include "wsThreadPool.h"

wsThreadPool wsThreads;

#ifdef WS_OS_FAMILY_UNIX
void* wsRunThread(void* threadID) {
    wsAssert(wsThreads.isInitialized(), "The object wsTasks must be initialized via the startUp() method before use.");
    //  Create a single thread and begin running...
    wsQueue<wsTask*>* tasks = wsThreads.getTaskList();
    u32 threadNum = *(u32*)threadID;
    wsTask* myTask = NULL;
    pthread_mutex_t* listMutex = wsThreads.getListMutex();
    pthread_mutex_t* logMutex = wsThreads.getLogMutex();
    while (!wsThreads.killSignalReceived()) {
        pthread_mutex_lock(listMutex);
        if (tasks->isNotEmpty()) {
            wsLog(WS_LOG_THREADS, "Removing task from front of queue; running on thread number %u", threadNum);
            myTask = tasks->pop();
        }
        pthread_mutex_unlock(listMutex);
        if (myTask != NULL) {
            myTask->run(threadNum);
            myTask = NULL;
        }
    }
    pthread_mutex_lock(logMutex);
    wsLog(WS_LOG_THREADS, "Exiting thread number %u", threadNum);
    pthread_mutex_unlock(logMutex);
    pthread_exit(NULL);
}
#elif defined(WS_OS_FAMILY_WINDOWS)
#endif  /*  Whipstitch OS families  */

void wsThreadPool::startUp() {
    _mInitialized = true;
    numThreads = maxThreads = WS_NUM_CORES - 1; //  Save one processor for the main thread
    if (numThreads <= 0) {
        wsLog(WS_LOG_THREADS, "No extra threads have been created.");
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
    listMutex = wsNew(pthread_mutex_t, pthread_mutex_t());
    pthread_mutex_init(listMutex, NULL);
    logMutex = wsNew(pthread_mutex_t, pthread_mutex_t());
    pthread_mutex_init(logMutex, NULL);
#elif defined(WS_OS_FAMILY_WINDOWS)
#endif  /*  Whipstitch OS families  */
    taskList = wsNew(wsQueue<wsTask*>, wsQueue<wsTask*>(WS_MAX_TASK_QUEUE_SIZE));
    tasksRunning = 0;
    wsLog(WS_LOG_THREADS, "Initializing %u Worker Threads", numThreads);
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
#ifdef WS_OS_FAMILY_UNIX
    pthread_mutex_lock(listMutex);
#elif defined(WS_OS_FAMILY_WINDOWS)
#endif

    if (numThreads <= 0) {
        task->run(0);
        return;
    }
    wsAssert(taskList->isNotFull(), "Cannot push task onto the queue.");
    taskList->push(task);

    /*  Unlock the mutex    */
#ifdef WS_OS_FAMILY_UNIX
    pthread_mutex_unlock(listMutex);
#elif defined(WS_OS_FAMILY_WINDOWS)
#endif
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
