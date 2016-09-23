/*
 * wsMemoryStack.h
 *
 *  Created on: Jul 18, 2012
 *      Author: dsnettleton
 *
 *      This file declares the class wsMemoryStack and a singleton instance of the class,
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
 *          in front of the global tier in the memory stack, the front tier can be cleared
 *          while keeping the global tier intact. The Front tier is indicated with the
 *          enumerated variable WS_MEMSTACK_TIER_FRONT.
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

#ifndef WS_MEMORYSTACK_H_
#define WS_MEMORYSTACK_H_

#include "wsPlatform.h"
#include "wsLog.h"

/// Shortcuts for Primary Stack Allocations
//  Used to pass custom constructors into the macro
#define wsNew(classtype, constructor) \
    new (wsMem.allocatePrimary( sizeof(classtype) )) constructor
//  Another one to be used for arrays
#define wsNewArray(classtype, arraySize) \
    (classtype*)wsMem.allocatePrimary( sizeof(classtype) * arraySize )

/// Shortcuts for Frame Stack Allocations
//  Used to pass custom constructors into the macro
#define wsNewTmp(classtype, constructor) \
    new (wsMem.allocateFrame_current( sizeof(classtype) )) constructor
//  Another one to be used for arrays
#define wsNewArrayTmp(classtype, arraySize) \
    (classtype*)wsMem.allocateFrame_current( sizeof(classtype) * arraySize )

class wsMemoryStack {
  public:
    /*  Enumerated Memory Stack tiers */
    enum _ws_memstack_tier {
      PRIMARY_GLOBAL,
      PRIMARY_FRONT,
      PRIMARY_REAR
    };
    enum _ws_memstack_frame_tier {
      FRAME_FRONT,
      FRAME_REAR
    };
    /*  Default Constructor and Deconstructor */
    //  As an engine subsystem, the memory stack takes no action until explicitly
    //  initialized via the startUp(...) function.
    //  uninitialized via the shutDown() function.
    wsMemoryStack() {}
    ~wsMemoryStack() {}
    /*  Accessors  */
    _ws_memstack_tier getCurrentTier() const { return mCurrentPrimaryTier; }
    //  Returns the total size of the Frame Stack
    u32 getFrameStackSize() const { return mFrameStackSize; }
    //  Returns the total space left in the Frame Stack
    u32 getFrameStackSpace() const {return (mRearMarker_f - mFrontMarker_f);}
    //  Returns the total size of the Primary Stack
    u64 getPrimaryStackSize() const { return mPrimaryStackSize; }
    //  Returns the total space left in the Primary Stack
    u64 getPrimaryStackSpace() const { return (mRearMarker - mFrontMarker); }
    /*  Operational Member functions  */
    //  Allocate space in the Current Frame Stack and return a pointer to the memory
    void* allocateFrame_current(const u32 numBytes);
    //  Allocate space in the Next Frame Stack and return a pointer to the memory
    void* allocateFrame_next(const u32 numBytes);
    //  Allocate space in the Primary Stack and return a pointer to the memory
    void* allocatePrimary(const u32 numBytes);
    //  Clear both ends of the Frame Stack
    void clearFrameStack();
    //  Clear both ends of the Primary Stack
    void clearPrimaryStack();
    //  Free the Front end of the Primary Stack back to the Global Tier
    void freePrimaryFront();
    //  Free the Rear end of the Primary Stack
    void freePrimaryRear();
    //  Free both ends the Primary Stack back to the Global Tier
    void freePrimaryToGlobal();
    //  Print Information about the Primary and Frame stacks
    void print(u16 printLog = WS_LOG_MAIN);
    //  Sets the tier for the Primary Stack, freeing space if necessary.
    void setTier(_ws_memstack_tier myTier);
    //  Initialize the Primary and Frame Stacks to the given sizes
    void startUp(const u64 numBytes_primaryStack, const u32 numBytes_frameStack);
    //  Free the memory by closing down the stack
    void shutDown();
    //  Swap the current frame on the Frame Stack
    void swapFrames();
  private:
    //  Total size of the Primary Stack
    u64 mPrimaryStackSize;
    //  Primary Stack Markers
    u64 mGlobalMarker;
    u64 mFrontMarker;
    u64 mRearMarker;
    //  Frame Stack Markers
    u32 mFrontMarker_f;
    u32 mRearMarker_f;
    //  Pointers for dynamic byte arrays
    u8* mFullByteArray;
    u8* mPrimaryStackBytes;
    u8* mFrameStackBytes;
    //  Total size of the frame stack
    u32 mFrameStackSize;
    //  Current tiers used by their respective stacks
    _ws_memstack_tier mCurrentPrimaryTier;
    _ws_memstack_frame_tier mCurrentFrameTier;
};

extern wsMemoryStack wsMem;

//  This is referrenced by a function pointer when wsMem starts up,
//  using the function set_new_handler(). Basically, this logs the problem
//  and pauses the operation, much like wsAssert().
inline void wsNewHandler() {
  wsEcho((WS_LOG_ERROR | WS_LOG_MEMORY), "Could not allocate the requested memory.\n    File: %s\n    Line: %s\n", __FILE__, __LINE__);
  WS_DEBUG_BREAK();
}

#endif /* WS_MEMORYSTACK_H_ */
