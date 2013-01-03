/*
 *      wsStack.h
 *      12/20/2012
 *      D. Scott Nettleton
 *
 *      This file declares and implements the class wsStack, which is a generic
 *      (templated) stack object for the Whipstitch Game Engine.
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

#ifndef WS_STACK_H_
#define WS_STACK_H_

template <class ClassType>
class wsStack {
    protected:
        ClassType* stack;
        u32 length;
        u32 maxElements;
    public:
        /*  Constructor */
        wsStack(const u32 maxStackSize = 32, const bool temporary = false);
        /*  Accessors   */
        u32 getMaxElements() { return maxElements; }
        u32 getLength() { return length; }
        bool isEmpty() { return (length == 0); }
        bool isFull() { return (length == maxElements); }
        /*  Operational Methods */
        void clear();   //  Removes all items from the stack
        u32 push(ClassType object); //  Adds the item and returns the new length
        ClassType pop();   //  Removes and returns the top item of the stack
        ClassType top();   //  Returns the top item of the stack without removing it
};

/*  Member Functions for wsStack  */
//  Constructor
template <class ClassType>
wsStack<ClassType>::wsStack(const u32 maxStackSize, const bool temporary) {
    maxElements = maxStackSize;
    if (temporary) {
        stack = wsNewArrayTmp(ClassType, maxElements);
    }
    else {
        stack = wsNewArray(ClassType, maxElements);
    }
    length = 0;
}

template <class ClassType>
u32 wsStack<ClassType>::push(ClassType object) {
    if (length == maxElements) { return length; }
    stack[length] = object;
    return ++length;
}

template <class ClassType>
void wsStack<ClassType>::clear() {
    length = 0;
}

template <class ClassType>
ClassType wsStack<ClassType>::pop() {
    if (length == 0) { return ClassType(); }
    return stack[--length];
}

template <class ClassType>
ClassType wsStack<ClassType>::top() {
    if (length == 0) { return ClassType(); }
    return stack[length-1];
}


#endif /*   WS_STACK_H_ */
