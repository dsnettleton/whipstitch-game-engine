/*
 *      wsQueue.h
 *      Dec. 25, 2012
 *      D. Scott Nettleton
 *
 *      This file declares and implements the class wsQueue, which is a generic
 *      (templated) double-ended queue object for the Whipstitch Game Engine.
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

#ifndef WS_QUEUE_H_
#define WS_QUEUE_H_

template <class ClassType>
class wsQueue {
    protected:
        ClassType* queue;
        u32 length;
        u32 maxElements;
        u32 frontIndex;
        u32 rearIndex;
    public:
        /*  Constructor */
        wsQueue(const u32 maxQueueSize = 32, const bool temporary = false);
        /*  Accessors   */
        u32 getMaxElements() { return maxElements; }
        u32 getLength() { return length; }
        bool isEmpty() { return (length == 0); }
        bool isNotEmpty() { return (length > 0); }
        bool isFull() { return (length == maxElements); }
        bool isNotFull() { return (length != maxElements); }
        /*  Operational Methods */
        void clear();   //  Removes all items from the queue
        u32 push(ClassType object); //  The same as pushBack(ClassType)
        u32 pushBack(ClassType object); //  Adds the item to the rear of the queue and returns the new length
        u32 pushFront(ClassType object); //  Adds the item to the front of the queue and returns the new length
        ClassType pop();  //  The same as popFront(ClassType)
        ClassType popBack();    //  Removes and returns the rear item of the queue
        ClassType back();   //  Returns the rear item of the queue without removing it
        ClassType popFront();   //  Removes and returns the front item of the queue
        ClassType front();   //  Returns the front item of the queue without removing it
};

/*  Member Functions for wsQueue  */
//  Constructor
template <class ClassType>
wsQueue<ClassType>::wsQueue(const u32 maxQueueSize, const bool temporary) {
    maxElements = maxQueueSize;
    if (temporary) {
        queue = wsNewArrayTmp(ClassType, maxElements);
    }
    else {
        queue = wsNewArray(ClassType, maxElements);
    }
    frontIndex = rearIndex = length = 0;
}

template <class ClassType>
u32 wsQueue<ClassType>::push(ClassType object) {
    if (length == maxElements) { return length; }
    queue[rearIndex] = object;
    rearIndex = (rearIndex + 1) % maxElements;
    return ++length;
}

template <class ClassType>
u32 wsQueue<ClassType>::pushBack(ClassType object) {
    if (length == maxElements) { return length; }
    queue[rearIndex] = object;
    rearIndex = (rearIndex + 1) % maxElements;
    return ++length;
}

template <class ClassType>
u32 wsQueue<ClassType>::pushFront(ClassType object) {
    if (length == maxElements) { return length; }
    queue[frontIndex] = object;
    frontIndex = (frontIndex - 1) % maxElements;
    return ++length;
}

template <class ClassType>
void wsQueue<ClassType>::clear() {
    frontIndex = rearIndex = length = 0;
}

template <class ClassType>
ClassType wsQueue<ClassType>::pop() {
    if (length == 0) { return ClassType(); }
    --length;
    frontIndex = (frontIndex + 1) % maxElements;
    return queue[frontIndex];
}

template <class ClassType>
ClassType wsQueue<ClassType>::popBack() {
    if (length == 0) { return ClassType(); }
    --length;
    rearIndex = (rearIndex - 1) % maxElements;
    return queue[rearIndex];
}

template <class ClassType>
ClassType wsQueue<ClassType>::popFront() {
    if (length == 0) { return ClassType(); }
    --length;
    frontIndex = (frontIndex + 1) % maxElements;
    return queue[frontIndex];
}

template <class ClassType>
ClassType wsQueue<ClassType>::back() {
    if (length == 0) { return ClassType(); }
    return queue[(rearIndex - 1) % maxElements];
}

template <class ClassType>
ClassType wsQueue<ClassType>::front() {
    if (length == 0) { return ClassType(); }
    return queue[(frontIndex - 1) % maxElements];
}

#endif  /*  WS_QUEUE_H_ */
