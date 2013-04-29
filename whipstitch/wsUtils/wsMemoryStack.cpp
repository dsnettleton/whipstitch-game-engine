/*
 * wsMemoryStack.cpp
 *
 *  Created on: Jul 18, 2012
 *      Author: dsnettleton
 *
 *      This file implements the class wsMemoryStack and a singleton instance of the class,
 *      wsMem, for the Whipstitch Game Engine.
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
 *      wsMemoryStack contains two double-ended stacks used for loading objects into
 *      preallocated memory.
 *
 *      The First stack, designated the "Primary Stack," can be 2^64 bytes (plenty of room
 *          to grow, and this accommodates stack size larger than 4GB). The Primary Stack
 *          has three tiers: Global, Level, and Rear.
 *      The Global tier is for memory which is not intended to be cleared until the game is
 *          is over; recommendations include menu systems, player models, etc. The Global
 *          tier is indicated with the enumerated variable WS_MEMSTACK_TIER_GLOBAL.
 *      The Front tier is for memory pertinent to a specific level or area. Because it is
 *          in front of the global tier in the memory stack, the fribt tier can be cleared
 *          while keeping the global tier intact. The Front tier is indicated with the
 *          enumarted variable WS_MEMSTACK_TIER_FRONT.
 *      The rear of the first stack has only one tier. This can be used to double-buffer
 *          level loading, or to load levels in compressed format before copying to the
 *          Level tier. Or it can be used for other sorts of temporary storage, at the
 *          programmer's discretion. The Rear tier is indicated with the enumerated
 *          variable WS_MEMSTACK_TIER_REAR.
 *      Primary Stack:
 *          [--GLOBAL--][----FRONT----][xxxxxxEMPTYxxxxxx][----REAR----]
 *                     ^              ^                   ^
 *               mGlobalMarker    mFrontMarker         mRearMarker
 *
 *      The second double-ended stack, designated the "Frame Stack," cannot be larger than
 *          4GB, since it is a 32-bit implementation. This should be plenty for most
 *          purposes, even in the forseeable future. The Frame Stack is intended to be used
 *          for double-buffering frame allocations. Allocations required for the current
 *          frame are placed in the "current" tier. Allocations required for the next frame
 *          are placed in the "next" tier. At the end of each frame, the "current" tier
 *          is cleared, and the "next" tier becomes the "current" tier.
 *      Frame Stack:
 *          [------FRONT------][xxxxxxxEMPTYxxxxxxx][------REAR------]
 *                            ^                     ^
 *                      mFrontMarker_f        mRearMarker_f
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

#include "wsMemoryStack.h"
#include "wsProfiling.h"
#include "wsLog.h"
#include <new>  //  Contains set_new_handler() and placement new function

wsMemoryStack wsMem;

/*  Define the startUp(...) function for this Engine Subsystem  */
void wsMemoryStack::startUp(const u64 numBytes_primaryStack,
                            const u32 numBytes_frameStack) {
    //  Sets our custom function as the new handler
    std::set_new_handler( wsNewHandler );
    //  Store the number of bytes requested for the Primary and Frame Stacks
    mPrimaryStackSize = numBytes_primaryStack;
    mFrameStackSize = numBytes_frameStack;
    //  Log the data
    wsEcho(WS_LOG_MEMORY,    "Initializing Memory Stack\n    Primary Stack Size = %u\n"
                            "    Frame Stack Size = %u\n",
                            mPrimaryStackSize, mFrameStackSize);
    //  Create a byte array encompassing both stacks
    mFullByteArray = new u8[ mPrimaryStackSize + mFrameStackSize ];
    wsAssert( (mFullByteArray != NULL), "Cannot allocate the requested stack memory");
    /*
     *  If memory has been requested for the Primary Stack, we create an array of bytes
     *  storing that much free memory. If not, we set our pointer to NULL to clarify
     *  that no Primary Stack is being created.
     *  The Global and Front Markers are set to the front of the stack (position 0),
     *  whereas the Rear marker is set to the end of the stack.
     */
    if (mPrimaryStackSize > 0) {
        //  Begin at the front of the stack
        mPrimaryStackBytes = &mFullByteArray[0];
        mFrontMarker = mGlobalMarker = 0;
        mRearMarker = mPrimaryStackSize;
    }
    else {
        mPrimaryStackBytes = NULL;
        mRearMarker = mFrontMarker = mGlobalMarker = 0;
    }
    /*
     *  If memory has been requested for the Frame Stack, we create an array of bytes
     *  storing that much free memory. If not, we set our pointer to NULL to clarify
     *  that no Frame Stack is being created.
     *  The Front Marker ix set to the front of the stack (position 0), whereas the Rear
     *  marker is set to the end of the stack.
     */
    if (mFrameStackSize > 0) {
        //  Begin at the end of the primary stack
        mFrameStackBytes = &mFullByteArray[mPrimaryStackSize];
        mFrontMarker_f = 0;
        mRearMarker_f = mFrameStackSize;
    }
    else {
        mFrameStackBytes = NULL;
        mRearMarker_f = mFrontMarker_f = 0;
    }
    //  Set Current Tiers for Primary and Frame Stacks
    mCurrentPrimaryTier = PRIMARY_GLOBAL;
    mCurrentFrameTier = FRAME_FRONT;
}

/*  Define the shutDown(...) function for this Engine Subsystem  */
void wsMemoryStack::shutDown() {
    wsEcho(WS_LOG_MEMORY, "Shutting down Memory Stack\n");
    wsAssert((mFullByteArray != NULL), "Pointer to Primary Stack is NULL");
    delete [] mFullByteArray;
}

/*  Operational Methods pertaining to Primary Stack  */

//  Allocate memory from the current tier of the Primary Stack; return its memory address.
void* wsMemoryStack::allocatePrimary(const u32 numBytes) {
    wsEcho(WS_LOG_MEMORY, "Allocating %u bytes\n", numBytes);
    wsAssert(mPrimaryStackSize, "Has the Primary Stack been initialized?");
    void* memAddress;
    switch (mCurrentPrimaryTier) {
        case PRIMARY_GLOBAL:
            if (mGlobalMarker + numBytes > mRearMarker) {
                wsEcho(  (WS_LOG_MEMORY | WS_LOG_ERROR),
                        "Cannot allocate %u bytes to Primary Global Stack\n"
                        "  GlobalMarker = %u, RearMarker = %u\n",
                        numBytes, mGlobalMarker, mRearMarker);
                return (void*)NULL;
            }
            wsAssert((mGlobalMarker < mPrimaryStackSize),"Invalid Primary Stack marker.\n");
            memAddress = &mPrimaryStackBytes[mGlobalMarker];
            mFrontMarker = mGlobalMarker += numBytes;
            break;
        case PRIMARY_REAR:
            if (mRearMarker - numBytes < mFrontMarker) {
                wsEcho(  (WS_LOG_MEMORY | WS_LOG_ERROR),
                        "Cannot allocate %u bytes to Primary Rear Stack\n", numBytes);
                return (void*)NULL;
            }
            mRearMarker -= numBytes;
            wsAssert((mRearMarker < mPrimaryStackSize), "Invalid Primary Stack marker.\n");
            memAddress = &mPrimaryStackBytes[mRearMarker];
            break;
        case PRIMARY_FRONT:
            if (mFrontMarker + numBytes > mRearMarker) {
                wsEcho(  (WS_LOG_MEMORY | WS_LOG_ERROR),
                        "Cannot allocate %u bytes to Primary Front Stack\n", numBytes);
                return (void*)NULL;
            }
            wsAssert((mFrontMarker < mPrimaryStackSize), "Invalid Primary Stack marker.\n");
            memAddress = &mPrimaryStackBytes[mFrontMarker];
            mFrontMarker += numBytes;
            break;
        default:
            wsEcho((WS_LOG_MEMORY | WS_LOG_ERROR), "Unsupported value for Primary tier.\n");
            memAddress = (void*)NULL;
            break;
    }
    wsEcho(WS_LOG_MEMORY, "Memory Allocated.\n");
    return memAddress;
}

//  Completely clear the Primary Memory Stack
void wsMemoryStack::clearPrimaryStack() {
    wsEcho(WS_LOG_MEMORY, "Clearing Primary Stack.\n");
    mFrontMarker = mGlobalMarker = 0;
    mRearMarker = mPrimaryStackSize;
}

//  Free the Front tier of the Primary Memory stack up to the Global tier
void wsMemoryStack::freePrimaryFront() {
    wsEcho(WS_LOG_MEMORY, "Freeing Front Tier of Primary Stack\n");
    mFrontMarker = mGlobalMarker;
}

//  Free the Rear tier of the Primary Memory Stack
void wsMemoryStack::freePrimaryRear() {
    wsEcho(WS_LOG_MEMORY, "Freeing Rear Tier of Primary Stack\n");
    mRearMarker = mPrimaryStackSize;

}

//  Free the Front and Rear tiers of the Primary Memory Stack,
//  leaving the Global tier intact
void wsMemoryStack::freePrimaryToGlobal() {
    wsEcho(WS_LOG_MEMORY, "Freeing Front and Rear Tiers of Primary Stack\n");
    mFrontMarker = mGlobalMarker;
    mRearMarker = mPrimaryStackSize;

}

//  Set the current tier for the Primary stack. If the tier is set to Global, the Front
//  tier is automatically cleared.
void wsMemoryStack::setTier(_ws_memstack_tier myTier) {
    wsEcho(WS_LOG_MEMORY, "Changing Primary Stack Tier\n");
    mCurrentPrimaryTier = myTier;
    if (mCurrentPrimaryTier == PRIMARY_GLOBAL) {
        wsEcho(WS_LOG_MEMORY, "  Clearing Front Tier of Primary Stack\n");
        mFrontMarker = mGlobalMarker;
    }
}

/*  Operational Methods Pertaining to Frame Stack  */

//  Allocate Memory from the Current tier of the Frame Stack; return its address
void* wsMemoryStack::allocateFrame_current(u32 numBytes) {
    wsEcho(WS_LOG_MEMORY, "Allocating %u bytes for the current frame\n");
    wsAssert(mFrameStackSize, "Has the Frame Stack been initialized?");
    void* memAddress;
    switch (mCurrentFrameTier) {
        case FRAME_FRONT:
            if (mFrontMarker_f + numBytes > mRearMarker_f) {
                wsEcho((WS_LOG_MEMORY | WS_LOG_ERROR),
                                "Cannot allocate %u bytes to Frame Stack.\n", numBytes);
                return NULL;
            }
            wsAssert((mFrontMarker_f < mFrameStackSize), "Invalid Frame Stack Marker\n");
            memAddress = &mFrameStackBytes[mFrontMarker_f];
            mFrontMarker_f += numBytes;
            break;
        case FRAME_REAR:
            if (mRearMarker_f - numBytes < mFrontMarker_f) {
                wsEcho((WS_LOG_MEMORY | WS_LOG_ERROR),
                                "Cannot allocate %u bytes to Frame Stack.\n", numBytes);
                return NULL;
            }
            mRearMarker_f -= numBytes;
            wsAssert((mRearMarker_f < mFrameStackSize), "Invalid Frame Stack Marker\n");
            memAddress = &mFrameStackBytes[mRearMarker_f];
            break;
        default:
            wsEcho((WS_LOG_MEMORY | WS_LOG_ERROR), "Unsupported value for Frame tier\n");
            return NULL;
    }
    return memAddress;
}

//  Allocate memory from the tier of the Frame stack opposite to the current tier.
//  Return its address.
void* wsMemoryStack::allocateFrame_next(u32 numBytes) {
    wsEcho(WS_LOG_MEMORY, "Allocating %u bytes for the current frame\n");
    wsAssert(mFrameStackSize, "Has the Frame Stack been initialized?");
    void* memAddress;
    switch (mCurrentFrameTier) {
        case FRAME_REAR:
            //  If the current tier is the back end of the frame stack, we'll operate
            //  on the front end.
            if (mFrontMarker_f + numBytes > mRearMarker_f) {
                wsEcho((WS_LOG_MEMORY | WS_LOG_ERROR),
                                "Cannot allocate %u bytes to Frame Stack.\n", numBytes);
                return NULL;
            }
            wsAssert((mFrontMarker_f < mFrameStackSize), "Invalid Frame Stack Marker\n");
            memAddress = &mFrameStackBytes[mFrontMarker_f];
            mFrontMarker_f += numBytes;
            break;
        case FRAME_FRONT:
            //  If the current tier is the front end of the frame stack, we'll operate
            //  on the back end.
            if (mRearMarker_f - numBytes < mFrontMarker_f) {
                wsEcho((WS_LOG_MEMORY | WS_LOG_ERROR),
                                "Cannot allocate %u bytes to Frame Stack.\n", numBytes);
                return NULL;
            }
            mRearMarker_f -= numBytes;
            wsAssert((mRearMarker_f < mFrameStackSize), "Invalid Frame Stack Marker\n");
            memAddress = &mFrameStackBytes[mRearMarker_f];
            break;
        default:
            wsEcho((WS_LOG_MEMORY | WS_LOG_ERROR), "Unsupported value for Frame tier\n");
            return NULL;
    }
    return memAddress;
}

//  Completely clear the Frame Stack
void wsMemoryStack::clearFrameStack() {
    wsEcho(WS_LOG_MEMORY, "Clearing Frame Stack.\n");
    mFrontMarker_f = 0;
    mRearMarker_f = mFrameStackSize;
}

//  Swaps the current tier for the Frame Stack. The opposite tier becomes the current
//  and the previously-current tier is wiped.
void wsMemoryStack::swapFrames() {
    if (mFrameStackSize == 0) {
        return;
    }
    wsEcho(WS_LOG_MEMORY, "Swapping frames in Frame Stack.\n");
    switch (mCurrentFrameTier) {
        case FRAME_FRONT:
            mFrontMarker_f = 0;
            mCurrentFrameTier = FRAME_REAR;
            break;
        case FRAME_REAR:
            mRearMarker_f = mFrameStackSize;
            mCurrentFrameTier = FRAME_FRONT;
            break;
        default:
            wsEcho((WS_LOG_MEMORY | WS_LOG_ERROR), "Unsupported value for Frame tier\n");
            break;
    }
}

//  Print Helpful information about the Primary and Frame stacks
void wsMemoryStack::print(u16 printLog) {
    wsEcho(  printLog,
            "Whipstitch Memory Stacks\n"
            "    Primary Stack:\n"
            "      Total Size:    %u bytes\n"
            "      Global Memory: %u bytes\n"
            "      Front Memory:  %u bytes\n"
            "      Rear Memory:   %u bytes\n"
            "      Total Memory:  %u bytes\n"
            "      Free Memory:   %u bytes\n"
            "    Frame Stack:\n"
            "      Total Size:    %u bytes\n"
            "      Front Memory:  %u bytes\n"
            "      Rear Memory:   %u bytes\n"
            "      Total Memory:  %u bytes\n"
            "      Free Memory:   %u bytes\n"
            "      Current Tier:  %s\n",
            mPrimaryStackSize,  //  Total Primary Stack Size
            mGlobalMarker,      //  Global Memory allocated
            (mFrontMarker - mGlobalMarker), //  Front tier memory allocated
            (mPrimaryStackSize - mRearMarker),  //  Rear tier memory allocated
            (mFrontMarker + (mPrimaryStackSize - mRearMarker)), //  Total Memory allocated
            (mRearMarker - mFrontMarker),   //  Free Memory in Primary Stack
            mFrameStackSize,    //  Total Frame Stack Size
            mFrontMarker_f,   //  Front Memory Allocated
            (mFrameStackSize - mRearMarker_f),    //  Rear Memory Allocated
            (mFrontMarker_f - (mFrameStackSize - mRearMarker_f)),   //  Total allocated
            (mRearMarker_f - mFrontMarker_f),   //  Free Memory in Frame Stack
            ( (mCurrentFrameTier == FRAME_FRONT) ?  "FRONT TIER" :  "BACK TIER") );
}

/*  Overridden Allocation Operators */
/*
void* operator new(size_t size) {
    void* allocation = wsMem.allocatePrimary( size );
    wsAssert(allocation, "Could not allocate Memory");
    return allocation;
}

void* operator new[](size_t size) {
    void* allocation = wsMem.allocatePrimary( size );
    wsAssert(allocation, "Could not allocate Memory For Array");
    return allocation;
}
//*/

