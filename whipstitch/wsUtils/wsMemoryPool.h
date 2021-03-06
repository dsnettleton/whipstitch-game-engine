/*
 * wsMemoryPool.h
 *
 *  Created on: Jul 25, 2012
 *      Author: dsnettleton
 *
 *      This file implements the class wsMemoryPool, which is a templated data structure
 *      used for storing many objects of the same type. The pool has a maximum size,
 *      which defines the amount of memory allocated.
 *
 *      The Whipstitch Game Engine has three types of memory: double-ended stacks, pools,
 *      and a heap. The stack memory is the default memory provider for the engine. It
 *      consists of two stacks, one for main storage and another for double-buffered
 *      temporary storage. The pool memory provides storage for like objects, allowing them
 *      to remain contiguous in memory and to be easily accessed. The heap memory is for
 *      general-purpose use, and has to be defragmented if in use. This is implemented by
 *      performing one bubble-sort iteration per frame. Since the stack memory is the
 *      primary storage mechanism, it is recommended that the user store their pools and
 *      heaps within the primary stack.
 *
 *      Storing many like objects together in a memory pool can prevent defragmentation,
 *      allowing fewer cache misses. It also prevents us from having to use allocators
 *      with each individual item, saving many costly operations.
 *
 *      Pool memory is ideal for vertex lists, particles, animation components, and any
 *      other data type that is typically stored in arrays.
 *
 *      Free memory is stored as a linked list within the pre-allocated memory; each
 *      block allocated contains, when free, an index to the next free block. This works
 *      only as long as sizeof(T) >= sizeof(u32).
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

#ifndef WS_MEMORYPOOL_H_
#define WS_MEMORYPOOL_H_

#include "wsMemoryStack.h"  //  For wsNewArray()

template <class T>
class wsMemoryPool {
    private:
        void* mFirstFreeBlock;    //  Index location of the first free block
        T* mObjects;   //  array in which to store the objects
        u32 mNumObjects;
    public:
        /*  Constructor */
        wsMemoryPool(u32 numObjects = 32);
        /*  Accessors   */
        u32 getByteSize() const { return ( mNumObjects * sizeof(T) ); }
        u32 getNumObjects() const { return mNumObjects; }
        T* get(u32 index) const {
            wsAssert((index < mNumObjects), "The requested index is out of range.\n");
            return &mObjects[index];
        }
        /*  Operational Member Functions    */
        //  Reserves the required memory for the class, calling the object's constructor.
        //  Returns an index for the object array
        T* add();
        //  Reserves the required memory for the class without calling the object's
        //  constructor. Returns an index for the object array
        T* allocate();
        //  Deallocates the required memory for the class without calling the destructor
        void deallocate(T* pointer);
        //  Deallocates the required memory for the class, destroying the object.
        void remove(T* pointer);
};

template <class T>
wsMemoryPool<T>::wsMemoryPool(u32 numObjects) {
    wsEcho(WS_LOG_MEMORY, "Initializing memory pool of %u objects\n", numObjects);
    wsAssert( (sizeof(T) >= sizeof(u32)),
                    "Object is too small for memory pool. Must be >= 4 bytes");
    mNumObjects = numObjects;
    mFirstFreeBlock = mObjects = wsNewArray(T, numObjects);
    wsAssert(mObjects != NULL, "Could not allocate memory pool.");
    //  Create Links to indicate next free memory blocks
    void* tmpPtr;
    void** nextPtr;
    for (u32 i = 0; i < mNumObjects-1; ++i) {
        tmpPtr = &mObjects[i];
        nextPtr = (void**)tmpPtr;
        *nextPtr = &mObjects[i+1];
    }
}

template <class T>
T* wsMemoryPool<T>::add() {
    wsEcho(WS_LOG_MEMORY, "Adding a value to the memory pool\n");
    wsAssert( (mFirstFreeBlock != NULL), "Has the Memory Pool been initialized?\n");
    void** nextPtr = (void**)mFirstFreeBlock;
    mFirstFreeBlock = *nextPtr;
    T* myClassPtr = new(nextPtr) T();
    return myClassPtr;
}

template <class T>
T* wsMemoryPool<T>::allocate() {
    wsEcho(WS_LOG_MEMORY, "Allocating space in the memory pool\n");
    wsAssert( (mFirstFreeBlock != NULL), "Has the Memory Pool been initialized?\n");
    T* myClassPtr = (T*)mFirstFreeBlock;
    void** nextPtr = (void**)mFirstFreeBlock;
    mFirstFreeBlock = *nextPtr;
    return myClassPtr;
}

template <class T>
void wsMemoryPool<T>::deallocate(T* pointer) {
    wsEcho(WS_LOG_MEMORY, "Freeing space in the memory pool.\n");
    wsAssert( (pointer != NULL), "Cannot remove a null pointer");
    wsAssert( ((u64)pointer >= (u64)&mObjects[0] &&
                    (u64)pointer <= &mObjects[mNumObjects-1]),
                    "This pointer does not fall within the allocated space");
    void** nextPtr = (void**)pointer;
    *nextPtr = mFirstFreeBlock;
    mFirstFreeBlock = (void*)pointer;
}

template <class T>
void wsMemoryPool<T>::remove(T* pointer) {
    wsEcho(WS_LOG_MEMORY, "Freeing space in the memory pool.\n");
    wsAssert( (pointer != NULL), "Cannot remove a null pointer");
    wsAssert( ((u64)pointer >= (u64)&mObjects[0] &&
                    (u64)pointer <= &mObjects[mNumObjects-1]),
                    "This pointer does not fall within the allocated space");
    pointer->~T();  //  Explicitly call deconstructor
    void** nextPtr = (void**)pointer;
    *nextPtr = mFirstFreeBlock;
    mFirstFreeBlock = (void*)pointer;
}

#endif /* WS_MEMORYPOOL_H_ */
