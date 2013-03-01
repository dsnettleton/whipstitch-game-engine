/*
 * wsOrderedHashMap.h
 *
 *  Created on: Jul 15, 2012
 *      Author: dsnettleton
 *
 *      This file declares the class wsOrderedHashMap, which is derived from the
 *      class wsHashMap. An ordered hashmap differes from a regular hashmap in that
 *      each of the elements is stored with a numerical value indicating what order
 *      the iterator will list the item in (lowest to highest).
 *
 *      Hashes are produced using the wsHash function declared in wsOperations.h.
 *      The hashed u32 integer is then modulated to accommodate the table size
 *      declared with the map. This table size represents the maximum number of elements
 *      allowed in the hash map.
 *
 *      Quatratic probing is used to handle collisions and preferred hash table sizes
 *      are therefore prime numbers.
 *
 *      Usage:
 *          Create a wsOrderedHashMap templated object list, along with the size.
 *              myMap = new wsOrderedHashMap<myObjectType>(maxSize);
 *          Add objects to the wsHashMap using the method insert(u32, ClassType, u32)
 *              myMap->insert(wsHash(myString), myObjectType, 10);
 *          Retrieve the objects for use using the method retrieve(u32)
 *              myObjectType newObj = myMap->retrieve(wsHash(myString));
 *
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

#ifndef WS_ORDERED_HASHMAP_H_
#define WS_ORDERED_HASHMAP_H_

#include "wsMemoryStack.h"
#include "wsOperations.h"

template <class ClassType>
class wsOrderedHashMap {
  protected:
    struct ws_OrderedHashKeyPair {
      u32 hashKey;
      //  Next and previous values allow the hashmap to function as a doubly-linked list
      //  A value less than zero indicates no item exists
      i32 next;
      i32 prev;
      i32 order;  //  Order in which the hashkey pair should be iterated through
      ClassType object;
    };
    ws_OrderedHashKeyPair* array;
    u32 length;
    u32 maxElements;
    //  First and last elements allow the hashmap to function as a doubly-linked list
    //  A value less than zero indicates no item exists
    i32 first;
    i32 last;
  public:
    struct iterator {
      /*  Member Variable */
      i32 mCurrentElement;
      wsOrderedHashMap<ClassType>* parent;
      /*  Member Functions  */
      iterator(wsOrderedHashMap<ClassType>* myParent); //  Constructor
      //  Retrieve the item from the hashmap
      ClassType get();
      //  Return the next item in the hashmap
      ClassType getNext();
      //  Return the previous item in the hashmap
      ClassType getPrev();
      //  Go to the next item in the hashmap
      ClassType operator++();  //  preincrement operator
      //  Go to the previous item in the hashmap
      ClassType operator--();  //  predecrement operator
    };
    /*  Constructor and Destructor */
    wsOrderedHashMap(u32 maxElements = 53);
    /*  Accessors   */
    u32 getMaxElements() { return maxElements; }
    u32 getLength() { return length; }
    const ClassType getArrayItem(u32 arrayIndex) { return array[arrayIndex].object; }
    bool isEmpty() { return (length == 0); }
    bool isFull() { return (length == maxElements); }
    /*  Operational Methods */
    //  Returns an iterator containing the first object in the table
    wsOrderedHashMap::iterator begin();
    //  Returns an iterator containing the last object in the table
    wsOrderedHashMap::iterator end();
    //  Inserts the element into the hashmap at the given index
    u32 insert(u32 hashIndex, const ClassType& element, u32 orderPos = 0);
    //  Prints the components of the hashMap using wsLog
    void print(u16 printLog = WS_LOG_MAIN);
    //  Removes the element from the hashmap at the given index
    void remove(u32 hashIndex);
    //  Ignores collisions, overwriting the element at the given index
    void replace(u32 hashIndex, const ClassType& element, u32 orderPos = 0);
    //  Retrieves the specified element from the hashmap
    const ClassType& retrieve(u32 hashIndex) const;
    //  Sets the specified object to the element from the hashmap
    //  Returns false if the element does not exist
    bool retrieve(u32 hashIndex, ClassType& element) const;
};

/*  Member Functions for wsOrderedHashMap::iterator  */
  template <class ClassType>
  wsOrderedHashMap<ClassType>::iterator::iterator(wsOrderedHashMap<ClassType>* myParent) {
    parent = myParent;
    mCurrentElement = parent->first;
  }

  template <class ClassType>
  inline ClassType wsOrderedHashMap<ClassType>::iterator::get() {
    if (mCurrentElement < 0 || (u32)mCurrentElement >= parent->maxElements) {
      return WS_NULL;
    }
    return parent->array[mCurrentElement].object;
  }

  template <class ClassType>
  inline ClassType wsOrderedHashMap<ClassType>::iterator::getNext() {
    if (parent->array[mCurrentElement].next < 0) { return WS_NULL; }   //  There is no next element
    return parent->array[ parent->array[mCurrentElement].next ].object;
  }

  template <class ClassType>
  inline ClassType wsOrderedHashMap<ClassType>::iterator::getPrev() {
    if (parent->array[mCurrentElement].prev < 0) { return WS_NULL; }
    return parent->array[ parent->array[mCurrentElement].prev ].object;
  }

  template <class ClassType>
  inline ClassType wsOrderedHashMap<ClassType>::iterator::operator++() {
    mCurrentElement = parent->array[mCurrentElement].next;
    return parent->array[ mCurrentElement ].object;
  }

  template <class ClassType>
  inline ClassType wsOrderedHashMap<ClassType>::iterator::operator--() {
    mCurrentElement = parent->array[mCurrentElement].next;
    return parent->array[ mCurrentElement ].object;
  }

/*  Member Functions for wsOrderedHashMap  */
//  Constructor
template <class ClassType>
wsOrderedHashMap<ClassType>::wsOrderedHashMap(u32 maxElements) {
  maxElements = wsNextPrime(maxElements);
  wsLog(WS_LOG_UTIL, "Creating %u element ordered hashmap\n", maxElements);
  wsAssert((maxElements > 0), "wsOrderedHashMap must have more than 0 elements.");
  length = 0;
  this->maxElements = maxElements;
  array = wsNewArray(ws_OrderedHashKeyPair, maxElements);
  for (u32 i = 0; i < maxElements; ++i) {
    array[i].hashKey = WS_NULL;
  }
  first = last = -1;
}

//  Operational Methods

template <class ClassType>
typename wsOrderedHashMap<ClassType>::iterator wsOrderedHashMap<ClassType>::begin() {
  //  Start at beginning and find the first item in the hashmap
  wsOrderedHashMap<ClassType>::iterator it(this);
  return it;
}

template <class ClassType>
typename wsOrderedHashMap<ClassType>::iterator wsOrderedHashMap<ClassType>::end() {
  //  Start at end and find the last item in the hashmap
  wsOrderedHashMap<ClassType>::iterator it(this);
  it.mCurrentElement = last;
  return it;
}

template <class ClassType>
u32 wsOrderedHashMap<ClassType>::insert(u32 hashIndex, const ClassType& element, u32 orderPos) {
  wsLog(WS_LOG_UTIL, "Inserting item\n");
  u32 moddedHash = hashIndex % maxElements;
  if (length != maxElements) {  //  If the hashMap is not full already
    u32 probeCount = 0, offset = 1;
    //  Loop for quadratic probing
    while ( array[moddedHash].hashKey != WS_NULL &&  //  The Array index is taken
        array[moddedHash].hashKey != hashIndex &&  //  The elements are not the same
        probeCount < maxElements/2) {   //  probe half of the elements
      ++probeCount;
      moddedHash += offset;
      moddedHash %= maxElements;
      offset += 2;
    }
    if (array[moddedHash].hashKey == WS_NULL) { //  The index is free
      array[moddedHash].hashKey = hashIndex;
      array[moddedHash].object = element;
      array[moddedHash].order = orderPos;
      ++length;
      //  Update linked list
      if (first < 0) {  //  The list is empty
        first = last = moddedHash;
        array[moddedHash].next = array[moddedHash].prev = -1;
      }
      else {
        if (array[moddedHash].order <= array[first].order) { //  This is now the first element
          array[moddedHash].next = first;
          array[moddedHash].prev = -1;
          first = array[first].prev = moddedHash;
        }
        else if (array[moddedHash].order >= array[last].order) {  //  This is now the last element
          array[moddedHash].prev = last;
          last = array[last].next = moddedHash;
          array[moddedHash].next = -1;
        }
        else {
          i32 current = first;
          while (array[moddedHash].order > array[current].order) {
            current = array[current].next;
          }
          wsAssert(current >= 0, "Ordered Hash - invalid value!");
          array[moddedHash].prev = array[current].prev;
          array[moddedHash].next = current;
          array[ array[moddedHash].prev ].next = array[current].prev = moddedHash;
        }
      }
    }
    else {
      if (array[moddedHash].hashKey == hashIndex) {
        wsLog(WS_LOG_UTIL, "Cannot enter duplicate key into ordered hash map.\n");
        return WS_FAIL;
      }
      else {
        wsLog(WS_LOG_UTIL, "Could not insert element into ordered hash map.\n"
                "  key = %u\n"
                "  moddedHash = %u\n"
                "  array[moddedHash].hashKey = %u\n",
                hashIndex, moddedHash, array[moddedHash].hashKey);
        return WS_FAIL;
      }
    }
  }
  else {
    wsLog(WS_LOG_UTIL, "Ordered Hashmap is full.\n");
    return WS_FAIL;
  }
  return WS_SUCCESS;
}

template <class ClassType>
void wsOrderedHashMap<ClassType>::print(u16 printLog) {
  wsLog(printLog, "Ordered Hashmap Contents");
  i32 current = first;
  while (current >= 0) {
    wsLog(printLog, " Order %d - Key = %u", array[current].order, array[current].hashKey);
    current = array[current].next;
  }
}

template <class ClassType>
void wsOrderedHashMap<ClassType>::remove(u32 hashIndex) {
  u32 moddedHash = hashIndex % maxElements;
  //  Loop for quadratic probing
  u32 probeCount = 0, offset = 1;
  while ( array[moddedHash].hashKey != WS_NULL &&  //  The Array index is taken
      array[moddedHash].hashKey != hashIndex &&  //  The elements are not the same
      probeCount < maxElements/2) {   //  probe half of the elements
    ++probeCount;
    moddedHash += offset;
    moddedHash %= maxElements;
    offset += 2;
  }
  if (array[moddedHash].hashKey == hashIndex) {   //  We have a match!
    if (array[moddedHash].prev >= 0) {
      array[ array[moddedHash].prev ].next = array[moddedHash].next;
    }
    if (array[moddedHash].next >= 0) {
      array[ array[moddedHash].next ].prev = array[moddedHash].prev;
    }
    array[moddedHash].hashKey = WS_NULL;
    --length;
  }
  else {
    wsLog(WS_LOG_UTIL, "No suitable match found for hash key: %u\n", hashIndex);
  }
}

template <class ClassType>
void wsOrderedHashMap<ClassType>::replace(u32 hashIndex, const ClassType& element, u32 orderPos) {
  u32 moddedHash = hashIndex % maxElements;
  //  Loop for quadratic probing
  u32 probeCount = 0, offset = 1;
  while ( array[moddedHash].hashKey != WS_NULL &&  //  The Array index is taken
      array[moddedHash].hashKey != hashIndex &&  //  The elements are not the same
      probeCount < maxElements/2) {   //  probe half of the elements
    ++probeCount;
    moddedHash += offset;
    moddedHash %= maxElements;
    offset += 2;
  }
  if (array[moddedHash].hashKey == hashIndex) {   //  We have a match!
    array[moddedHash].object = element;
    if (orderPos != array[moddedHash].order) {
      array[moddedHash].order = orderPos;
      //  Remove from linked list
      if (array[moddedHash].prev >= 0) {
        array[ array[moddedHash].prev ].next = array[moddedHash].next;
      }
      if (array[moddedHash].next >= 0) {
        array[ array[moddedHash].next ].prev = array[moddedHash].prev;
      }
      //  Update linked list
      if (first < 0) {  //  The list is empty
        first = last = moddedHash;
        array[moddedHash].next = array[moddedHash].prev = -1;
      }
      else {
        if (array[moddedHash].order < array[first].order) { //  This is now the first element
          array[moddedHash].next = first;
          first = array[first].prev = moddedHash;
          array[moddedHash].prev = -1;
        }
        else if (array[moddedHash].order > array[last].order) {  //  This is now the last element
          array[moddedHash].prev = last;
          last = array[last].next = moddedHash;
          array[moddedHash].next = -1;
        }
        else {
          i32 current = first;
          while (array[moddedHash].order > array[current].order) {
            current = array[current].next;
          }
          wsAssert(current >= 0, "Ordered Hash - invalid value!");
          array[moddedHash].prev = array[current].prev;
          array[moddedHash].next = current;
          array[array[moddedHash].prev].next = array[current].prev = moddedHash;
        }
      }
    }
  }
  else {
    //wsLog(WS_LOG_UTIL, "No suitable match found for hash key: %u\n", hashIndex);
    insert(hashIndex, element);
  }
}

template <class ClassType>
const ClassType& wsOrderedHashMap<ClassType>::retrieve(u32 hashIndex) const {
  u32 moddedHash = hashIndex % maxElements;
  //  Loop for quadratic probing
  u32 probeCount = 0, offset = 1;
  while ( array[moddedHash].hashKey != WS_NULL &&  //  The Array index is taken
      array[moddedHash].hashKey != hashIndex &&  //  The elements are not the same
      probeCount < maxElements/2) {   //  probe half of the elements
    ++probeCount;
    moddedHash += offset;
    moddedHash %= maxElements;
    offset += 2;
  }
  if (array[moddedHash].hashKey == hashIndex) {   //  We have a match!
    return array[moddedHash].object;
  }
  else {
    wsLog(WS_LOG_UTIL, "No suitable match found for hash key: %u\n", hashIndex);
  }
  return array[moddedHash].object;
}

template <class ClassType>
bool wsOrderedHashMap<ClassType>::retrieve(u32 hashIndex, ClassType& element) const {
  u32 moddedHash = hashIndex % maxElements;
  //  Loop for quadratic probing
  u32 probeCount = 0, offset = 1;
  while ( array[moddedHash].hashKey != WS_NULL &&  //  The Array index is taken
      array[moddedHash].hashKey != hashIndex &&  //  The elements are not the same
      probeCount < maxElements/2) {   //  probe half of the elements
    ++probeCount;
    moddedHash += offset;
    moddedHash %= maxElements;
    offset += 2;
  }
  if (array[moddedHash].hashKey == hashIndex) {   //  We have a match!
    element = array[moddedHash].object;
    return true;
  }
  else {
    wsLog(WS_LOG_UTIL, "No suitable match found for hash key: %u\n", hashIndex);
    return false;
  }
}

#endif  /*  WS_ORDERED_HASHMAP_H_   */
