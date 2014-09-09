/*Copyright 2010-2013 George Karagoulis
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_ICOLLECTIONS_H
#define GUTIL_ICOLLECTIONS_H

#include "gutil_macros.h"

NAMESPACE_GUTIL;


/** Defines some base functions common to most data containers. */
class IDataContainer
{
public:

    /** Returns the number of objects in the data container. */
    virtual GUINT32 Size() const = 0;

    /** Removes all items from the container. */
    virtual void Clear() = 0;

};





/** Defines a random access container interface for type T.
 *
 *  The implementation class should throw exceptions whenever errors happen,
 *  for example when the index is out of range.
*/
template<class T> class IRandomAccessContainer :
        public IDataContainer
{
public:

    /** Returns the item at the given index. */
    virtual T &At(GUINT32 index) = 0;

    /** Returns the item at the given index. */
    virtual T const &At(GUINT32 index) const = 0;

    /** Inserts a copy of the item at the given index, and returns a reference to the inserted item. */
    virtual T &Insert(const T &item, GUINT32 index) = 0;

    /** Removes the item at the given index. */
    virtual void Remove(GUINT32 index) = 0;

    virtual ~IRandomAccessContainer(){}

};



/** Defines the interface of a stack of type T.
 *  The implementation class should throw exceptions whenever errors happen.
*/
template<class T> class IStack :
        public IDataContainer
{
public:

    /** Pushes a copy of the item onto the top of the stack and returns a reference to it. */
    virtual T &Push(const T &) = 0;

    /** Pops the item at the top of the stack.  If there are no items the behavior is undefined (so don't do it). */
    virtual void Pop() = 0;

    /** Returns a reference to the item at the top of the stack. */
    virtual T &Top() = 0;

    /** Returns a const reference to the item at the top of the stack. */
    virtual T const &Top() const = 0;

    virtual ~IStack(){}

};



/** Defines the interface of a queue of type T.
 *  The implementation class should throw exceptions whenever errors happen.
 */
template<class T> class IQueue :
        public IDataContainer
{
public:

    /** Enqueues a copy of the given item, and returns a reference to it. */
    virtual T &Enqueue(const T &) = 0;

    /** Removes the item at the front of the queue.  If there are no items the behavior is undefined. */
    virtual void Dequeue() = 0;

    /** Returns a reference to the item at the front of the queue. */
    virtual T &Front() = 0;

    /** Returns a const reference to the item at the front of the queue. */
    virtual T const &Front() const = 0;

    virtual ~IQueue(){}

};


/** Defines the interface of a deque of type T.
 *  The implementation class should throw exceptions whenever errors happen.
 */
template<class T> class IDeque :
        public IDataContainer
{
public:

    /** Enqueues a copy of the given item at the front, and returns a reference to it. */
    virtual T &EnqueueFront(const T &) = 0;

    /** Enqueues a copy of the given item at the back, and returns a reference to it. */
    virtual T &EnqueueBack(const T &) = 0;

    /** Removes the item at the front of the queue.  If there are no items the behavior is undefined. */
    virtual void DequeueFront() = 0;

    /** Removes the item at the back of the queue.  If there are no items the behavior is undefined. */
    virtual void DequeueBack() = 0;

    /** Returns a reference to the item at the front of the queue. */
    virtual T &Front() = 0;

    /** Returns a const reference to the item at the front of the queue. */
    virtual T const &Front() const = 0;

    /** Returns a reference to the item at the back of the queue. */
    virtual T &Back() = 0;

    /** Returns a const reference to the item at the back of the queue. */
    virtual T const &Back() const = 0;

    virtual ~IDeque(){}

};


END_NAMESPACE_GUTIL;

#endif // GUTIL_ICOLLECTIONS_H
