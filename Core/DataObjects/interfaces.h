/*Copyright 2010-2012 George Karagoulis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.*/

#ifndef GUTIL_DATAOBJECTS_INTERFACES_H
#define GUTIL_DATAOBJECTS_INTERFACES_H

/** \file

    Defines a set of interfaces used by the data objects, which allow you to
    take advantage of several container implementations to do the same things.  For
    example, you can have a stack which is implemented as a vector, singly-linked list
    or doubly-linked list.
*/


#include "Core/macros.h"
NAMESPACE_GUTIL1(DataObjects);


/** An abstract interface that says you can treat me like a stack. */
template<class T>class Stack
{
public:

    /** Push an item on the stack. */
    virtual void Push(const T &) = 0;

    /** Pop an item from the top of the stack. */
    virtual void Pop() = 0;

    /** A const reference to the top of the stack. */
    virtual const T &Top() const = 0;

    /** A reference to the top of the stack. */
    virtual T &Top() = 0;

    /** How many items on the stack? */
    virtual GUINT32 CountStackItems() const = 0;

    /** Remove all items from the stack. */
    virtual void FlushStack() = 0;

    /** So derived classes can be deleted by the interface. */
    virtual ~Stack(){}

};


/** An abstract interface that says you're a random-access container. */
template <class T>class RandomAccessContainer
{
public:

    /** Fetch the item at the specified index. */
    virtual const T &At(GUINT32) const = 0;

    /** Fetch the item at the specified index. */
    virtual T &At(GUINT32) = 0;

    /** Insert an item at the given index. */
    virtual void InsertAt(const T &, GUINT32 indx) = 0;

    /** Remove the item at the given index. */
    virtual void RemoveAt(GUINT32 indx) = 0;

    /** Tell us how many items in the container. */
    virtual GUINT32 CountContainerItems() const = 0;

    /** Remove all items from the container. */
    virtual void FlushContainer() = 0;

    /** So derived classes can be deleted by the interface. */
    virtual ~RandomAccessContainer(){}

};


/** An abstract interface that says you can behave like a queue. */
template<class T>class Queue
{
public:

    /** Put a new item at the back of the queue. */
    virtual void Enqueue(const T &) = 0;

    /** Remove an item from the front of the queue. */
    virtual void Dequeue() = 0;

    /** Return the item at the front of the queue. */
    virtual const T &Front() const = 0;

    /** Return the item at the front of the queue. */
    virtual T &Front() = 0;

    /** How many items in the queue? */
    virtual GUINT32 CountQueueItems() const = 0;

    /** Remove all items from the Queue. */
    virtual void FlushQueue() = 0;

    /** So derived classes can be deleted by the interface. */
    virtual ~Queue(){}

};


/** An abstract interface that says you can behave like a deque (double-ended queue). */
template<class T>class Deque
{
public:

    /** Push an item on the front of the queue. */
    virtual void PushFront(const T &) = 0;

    /** Push an item on the back of the queue.
        \note This is the normal Enqueue operation.
    */
    virtual void PushBack(const T &) = 0;

    /** Pop the item at the front of the queue.
        \note This is the normal Dequeue operation.
    */
    virtual void PopFront() = 0;

    /** Pop the item from the back of the queue. */
    virtual void PopBack() = 0;

    /** Returns the item at the front of the queue. */
    virtual const T &Front() const = 0;

    /** Returns the item at the front of the queue. */
    virtual T &Front() = 0;

    /** Returns the item at the back of the queue. */
    virtual const T &Back() const = 0;

    /** Returns the item at the back of the queue. */
    virtual T &Back() = 0;

    /** Returns how many items are in the deque. */
    virtual GUINT32 CountDequeItems() const = 0;

    /** Deletes all items from the deque. */
    virtual void FlushDeque() = 0;

    /** So derived classes can be deleted by the interface. */
    virtual ~Deque(){}

};


END_NAMESPACE_GUTIL1;

#endif // GUTIL_DATAOBJECTS_INTERFACES_H
