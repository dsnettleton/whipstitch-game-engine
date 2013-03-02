/*
 *      wsStack.h
 *      12/20/2012
 *      D. Scott Nettleton
 *
 *      This file declares and implements the class wsStack, which is a generic
 *      (templated) stack object for the Whipstitch Game Engine.
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
        bool isNotEmpty() { return (length > 0); }
        bool isFull() { return (length == maxElements); }
        bool isNotFull() { return (length != maxElements); }
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
