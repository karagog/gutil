/*Copyright 2011 George Karagoulis

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

/** \file Defines a set of interfaces used by the data objects, which allow you to
    take advantage of several container implementations to do the same things.  For
    example, you can have a stack which is implemented as a vector, singly-linked list
    or doubly-linked list.
*/


/** An abstract interface that says you can treat me like a stack. */
template<class T>class IStack
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

    /** So derived classes can be deleted by the interface. */
    virtual ~IStack(){}

};


/** An abstract interface that says you're a random-access container. */
template <class T>class IContainer
{
public:

    /** Fetch the item at the specified index. */
    virtual const T &At(long) const = 0;
    /** Fetch the item at the specified index. */
    virtual T &At(long) = 0;

    /** Tell us how many items in the container. */
    inline long Count() const = 0;

    /** For convenience, define a default index operator. */
    inline const T &operator [](long i) const{ return At(i); }
    /** For convenience, define a default index operator. */
    inline T &operator [](long i){ return At(i); }

    /** So derived classes can be deleted by the interface. */
    virtual ~IContainer(){}

};


/** An abstract interface that says you can behave like a queue. */
template<class T>class IQueue
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

    /** So derived classes can be deleted by the interface. */
    virtual ~IContainer(){}

};


/** An abstract interface that says you can behave like a dequeue (double-ended queue).
    It automatically inherits IQueue.
*/
template<class T>class IDequeue : public IQueue<T>
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

    /** Returns the item at the back of the queue. */
    virtual const T &Back() const = 0;

    /** Returns the item at the back of the queue. */
    virtual T &Back() = 0;

    /** Automatically calls PushBack(), for convenience. */
    virtual void Enqueue(const T &){ PushBack(); }

    /** Automatically calls PopFront(), for convenience. */
    virtual void Dequeue(){ PopFront(); }

};


#endif // GUTIL_DATAOBJECTS_INTERFACES_H
