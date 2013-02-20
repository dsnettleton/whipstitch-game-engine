/*
 * wsHashMap.h
 *
 *  Created on: Jul 15, 2012
 *    Author: dsnettleton
 *
 *    This file declares the class wsHashMap, which is a template class containing
 *    an array of pointers to objects of the templated data type.
 *
 *    Hashes are produced using the wsHash function declared in wsOperations.h.
 *    The hashed u32 integer is then modulated to accommodate the table size
 *    declared with the map. This table size represents the maximum number of elements
 *    allowed in the hash map.
 *
 *    Quatratic probing is used to handle collisions and preferred hash table sizes
 *    are therefore prime numbers.
 *
 *    Usage:
 *      Create a wsHashMap templated object list, along with the size.
 *        myMap = new wsHashMap<myObjectType>(maxSize);
 *      Add objects to the wsHashMap using the method insert(u32, ClassType)
 *        myMap->insert(wsHash(myString), myObjectType);
 *      Retrieve the objects for use using the method retrieve(u32)
 *        myObjectType newObj = myMap->retrieve(wsHash(myString));
 *
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

#ifndef WS_HASHMAP_H_
#define WS_HASHMAP_H_

#include "wsMemoryStack.h"
#include "wsOperations.h"

template <class ClassType>
class wsHashMap {
  protected:
    struct ws_HashKeyPair {
      u32 hashKey;
      //  Next and previous values allow the hashmap to function as a doubly-linked list
      //  A value less than zero indicates no item exists
      i32 next;
      i32 prev;
      ClassType object;
    };
    ws_HashKeyPair* array;
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
      wsHashMap<ClassType>* parent;
      /*  Member Functions  */
      iterator(wsHashMap<ClassType>* myParent); //  Constructor
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
    /*  Constructor and Deconstructor   */
    wsHashMap(u32 maxElements = 53);
    ~wsHashMap();
    /*  Accessors   */
    u32 getMaxElements() { return maxElements; }
    u32 getLength() { return length; }
    const ClassType getArrayItem(u32 arrayIndex) { return array[arrayIndex].object; }
    bool isEmpty() { return (length == 0); }
    bool isFull() { return (length == maxElements); }
    /*  Operational Member Functions  */
    //  Returns an iterator containing the first object in the table
    wsHashMap::iterator begin();
    //  Returns an iterator containing the last object in the table
    wsHashMap::iterator end();
    //  Inserts the element into the hashmap at the given index
    u32 insert(u32 hashIndex, const ClassType& element);
    //  Prints the components of the hashMap using wsLog
    void print(u16 printLog = WS_LOG_MAIN);
    //  Removes the element from the hashmap at the given index
    void remove(u32 hashIndex);
    //  Ignores collisions, overwriting the element at the given index
    void replace(u32 hashIndex, const ClassType& element);
    //  Retrieves the specified element from the hashmap
    const ClassType& retrieve(u32 hashIndex) const;
    //  Sets the specified object to the element from the hashmap
    //  Returns false if the element does not exist
    bool retrieve(u32 hashIndex, ClassType& element) const;
};

/*  Member Functions for wsHashMap::iterator  */
template <class ClassType>
wsHashMap<ClassType>::iterator::iterator(wsHashMap<ClassType>* myParent) {
  parent = myParent;
  mCurrentElement = parent->first;
}

template <class ClassType>
inline ClassType wsHashMap<ClassType>::iterator::get() {
  if (mCurrentElement < 0 || (u32)mCurrentElement >= parent->maxElements) {
    return WS_NULL;
  }
  return parent->array[mCurrentElement].object;
}

template <class ClassType>
inline ClassType wsHashMap<ClassType>::iterator::getNext() {
  if (parent->array[mCurrentElement].next < 0) { return WS_NULL; }   //  There is no next element
  return parent->array[ parent->array[mCurrentElement].next ].object;
}

template <class ClassType>
inline ClassType wsHashMap<ClassType>::iterator::getPrev() {
  if (parent->array[mCurrentElement].prev < 0) { return WS_NULL; }
  return parent->array[ parent->array[mCurrentElement].prev ].object;
}

template <class ClassType>
inline ClassType wsHashMap<ClassType>::iterator::operator++() {
  mCurrentElement = parent->array[mCurrentElement].next;
  return parent->array[ mCurrentElement ].object;
}

template <class ClassType>
inline ClassType wsHashMap<ClassType>::iterator::operator--() {
  mCurrentElement = parent->array[mCurrentElement].next;
  return parent->array[ mCurrentElement ].object;
}

/*  Member Functions for wsHashMap  */
//  Constructor
template <class ClassType>
wsHashMap<ClassType>::wsHashMap(u32 maxElements) {
  maxElements = wsNextPrime(maxElements);
  wsLog(WS_LOG_UTIL, "Creating %u element hashmap\n", maxElements);
  wsAssert((maxElements > 0), "wsHashMap must have more than 0 elements.");
  length = 0;
  this->maxElements = maxElements;
  array = wsNewArray(ws_HashKeyPair, maxElements);
  for (u32 i = 0; i < maxElements; ++i) {
    array[i].hashKey = WS_NULL;
  }
  first = last = -1;
}

template <class ClassType>
wsHashMap<ClassType>::~wsHashMap() {
  wsAssert((array != NULL), "Cannot delete NULL array.");
  //delete [] array;
}

//  Operational Member Functions

template <class ClassType>
typename wsHashMap<ClassType>::iterator wsHashMap<ClassType>::begin() {
  //  Start at beginning and find the first item in the hashmap
  wsHashMap<ClassType>::iterator it(this);
  return it;
}

template <class ClassType>
typename wsHashMap<ClassType>::iterator wsHashMap<ClassType>::end() {
  //  Start at end and find the last item in the hashmap
  wsHashMap<ClassType>::iterator it(this);
  it.mCurrentElement = this->last;
  return it;
}

template <class ClassType>
u32 wsHashMap<ClassType>::insert(u32 hashIndex, const ClassType& element) {
  //wsLog(WS_LOG_UTIL, "Inserting item\n");
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
      ++length;
      //  Update linked list
      if (first < 0) {  //  The list is empty
        first = last = moddedHash;
        array[moddedHash].next = array[moddedHash].prev = -1;
      }
      else if (moddedHash == 0) { //  If there can be no other elements preceding this one
        array[moddedHash].prev = -1;
        array[moddedHash].next = first;
        first = array[first].prev = moddedHash;
      }
      else if (moddedHash == maxElements-1) {//   If no other elements can follow this one
        array[moddedHash].next = -1;
        array[moddedHash].prev = last;
        last = array[last].next = moddedHash;
      }
      else {
        if (moddedHash < maxElements/2) {   //  Front half of the hashmap; search backwards for nearest element
          //  Check before this index
          if (moddedHash < (u32)first) {  //  This is now the first element
            array[moddedHash].next = first;
            first = array[first].prev = moddedHash;
            array[moddedHash].prev = -1;
          }
          else {
            //  Iterate to the beginning of the hashmap and find the previous element
            for (u32 i = moddedHash-1; i >= 0; --i) {
              if (array[i].hashKey != WS_NULL) {  //  The index is taken; this will be our prev
                array[moddedHash].prev = i;
                array[moddedHash].next = array[i].next;
                if (array[i].next < 0) {  //  If there was not previously another element in front of this
                  array[i].next = last = moddedHash;
                }
                else {
                  array[i].next = array[ array[i].next ].prev = moddedHash;
                }
                break;
              }
            }
          }
        }
        else {  //  Back half of the hashmap; search forwards for the nearest element
          //  Check in front of this index
          if (moddedHash > (u32)last) {  //  This is now the last element
            array[moddedHash].prev = last;
            last = array[last].next = moddedHash;
            array[moddedHash].next = -1;
          }
          else {
            //  Iterate to the end and find the next element
            for (u32 i = moddedHash+1; i < maxElements; ++i) {
              if (array[i].hashKey != WS_NULL) {  //  The index is taken; this will be our next
                array[moddedHash].next = i;
                array[moddedHash].prev = array[i].prev;
                if (array[i].prev < 0) {  //  If there was not previously another element before this
                  array[i].prev = first = moddedHash;
                }
                else {
                  array[i].prev = array[ array[i].prev ].next = moddedHash;
                }
              }
            }
          }
        }
      }
    }
    else {
      if (array[moddedHash].hashKey == hashIndex) {
        wsLog(WS_LOG_UTIL, "Cannot enter duplicate key into hash map.\n");
        return WS_FAIL;
      }
      else {
        wsLog(WS_LOG_UTIL, "Could not insert element into hash map.\n"
                "  key = %u\n"
                "  moddedHash = %u\n"
                "  array[moddedHash].hashKey = %u\n",
                hashIndex, moddedHash, array[moddedHash].hashKey);
        return WS_FAIL;
      }
    }
  }
  else {
    wsLog(WS_LOG_UTIL, "Hashmap is full.\n");
    return WS_FAIL;
  }
  return WS_SUCCESS;
}

template <class ClassType>
void wsHashMap<ClassType>::print(u16 printLog) {
  wsLog(printLog, "Hashmap Contents\n");
  for (u32 i = 0; i < maxElements; ++i) {
    if (array[i].hashKey != WS_NULL) {
      wsLog(printLog, " Pos %u - Key = %u\n", i, array[i].hashKey);
    }
  }
}

template <class ClassType>
void wsHashMap<ClassType>::remove(u32 hashIndex) {
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
void wsHashMap<ClassType>::replace(u32 hashIndex, const ClassType& element) {
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
  }
  else {
    //wsLog(WS_LOG_UTIL, "No suitable match found for hash key: %u\n", hashIndex);
    insert(hashIndex, element);
  }
}

template <class ClassType>
const ClassType& wsHashMap<ClassType>::retrieve(u32 hashIndex) const {
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
bool wsHashMap<ClassType>::retrieve(u32 hashIndex, ClassType& element) const {
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

#endif /* WS_HASHMAP_H_ */
