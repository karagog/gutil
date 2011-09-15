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

#ifndef QUEUE_H
#define QUEUE_H

#include "Core/DataObjects/private/queue_p.h"
#include "Core/DataObjects/private/flexible_type_comparer.h"
#include "Core/Interfaces/iclonable.h"
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** The Queue provides a FIFO list

    You can iterate through it in both directions, but there is no random-access to
    its elements.  If you need random access, see the List.

    It is implemented as a doubly-linked list, so insertions and removals are done in constant time.

    \sa List, Stack
*/
template<class T>class Queue :
        public queue_p,
        public Interfaces::IClonable< Queue<T> >
{
public:

    /** Copies the item and puts it in the queue. */
    inline void Enqueue(const T &i){ push_back(&i); }

    /** Removes the first item from the queue. */
    inline void Dequeue(){ pop_front(); }

    /** The item at the front of the queue. */
    inline const T &Front() const{ return *reinterpret_cast<T const *>(NextNode->Data); }
    /** The item at the front of the queue. */
    inline T &Front(){ return *reinterpret_cast<T *>(NextNode->Data); }

    /** The item at the back of the queue. */
    inline const T &Back() const{ return *reinterpret_cast<T const *>(PreviousNode->Data); }

    /** The item at the back of the queue. */
    inline T &Back(){ return *reinterpret_cast<T *>(PreviousNode->Data); }

    /** A bidirectional iterator through the queue. */
    class iterator :
            public bidirectional_node_iterator
    {
    public:
        inline iterator(bidirectional_node_t *n = 0) :bidirectional_node_iterator(n){}

        /** Advances the iterator */
        inline iterator &operator++(){ advance(); return *this; }

        /** Advances the iterator */
        inline iterator operator++(int){ const_iterator ret(*this); advance(); return ret; }

        /** Advances the iterator the specified number of items */
        inline iterator &operator+=(int n){
            while(n-- > 0) advance();
            return *this;
        }

        /** Returns a copy of the iterator advanced the specified number of times. */
        inline iterator operator+(int n){
            const_iterator ret(*this);
            while(n-- > 0) ret.advance();
            return ret;
        }

        /** Retreats the iterator */
        inline iterator &operator--(){ retreat(); return *this; }

        /** Retreats the iterator */
        inline iterator operator--(int){ const_iterator ret(*this); retreat(); return ret;}

        /** Retreats the iterator the specified number of items */
        inline iterator &operator-=(int n){
            while(n-- > 0) retreat();
            return *this;
        }

        /** Returns a copy of the iterator retreated the specified number of times. */
        inline iterator operator-(int n){
            const_iterator ret(*this);
            while(n-- > 0) ret.retreat();
            return ret;
        }

        inline T &operator *(){ return *reinterpret_cast<T *>(current->Data); }
        inline T *operator ->(){ return reinterpret_cast<T *>(current->Data); }
    };

    /** A bidirectional iterator through the queue. */
    class const_iterator :
            public bidirectional_node_iterator
    {
    public:
        inline const_iterator(bidirectional_node_t *n = 0) :bidirectional_node_iterator(n){}

        /** Advances the iterator */
        inline const_iterator &operator++(){ advance(); return *this; }

        /** Advances the iterator */
        inline const_iterator operator++(int){ const_iterator ret(*this); advance(); return ret; }

        /** Advances the iterator the specified number of items */
        inline const_iterator &operator+=(int n){
            while(n-- > 0) advance();
            return *this;
        }

        /** Returns a copy of the iterator advanced the specified number of times. */
        inline const_iterator operator+(int n){
            const_iterator ret(*this);
            while(n-- > 0) ret.advance();
            return ret;
        }

        /** Retreats the iterator */
        inline const_iterator &operator--(){ retreat(); return *this; }

        /** Retreats the iterator */
        inline const_iterator operator--(int){ const_iterator ret(*this); retreat(); return ret;}

        /** Retreats the iterator the specified number of items */
        inline const_iterator &operator-=(int n){
            while(n-- > 0) retreat();
            return *this;
        }

        /** Returns a copy of the iterator retreated the specified number of times. */
        inline const_iterator operator-(int n){
            const_iterator ret(*this);
            while(n-- > 0) ret.retreat();
            return ret;
        }

        inline const T &operator *() const{ return *reinterpret_cast<T const *>(current->Data); }
        inline const T *operator ->() const{ return reinterpret_cast<T const *>(current->Data); }
    };

    inline iterator begin(){ return NextNode; }
    inline const_iterator begin() const{ return NextNode; }
    inline iterator end(){ return ++iterator(PreviousNode); }
    inline const_iterator end() const{ return ++iterator(PreviousNode); }

    class TypeWrapper :
            public queue_p::queue_type_wrapper,
            public FlexibleTypeComparer<T>
    {
    public:
        virtual T *Copy(const T &o) const{ return new T(o); }
        virtual void Delete(T *t) const{ delete t; }
    private:
        virtual void *CopyVoid(const void *const v) const{
            return Copy(*reinterpret_cast<T const*const>(v));
        }
        virtual void DeleteVoid(void *v) const{ Delete(reinterpret_cast<T *>(v)); }
    };

    /** Builds an empty queue. */
    inline Queue()
        :queue_p(new TypeWrapper)
    {}

    /** Constructs a queue with the item in it. */
    inline Queue(const T &i)
        :queue_p(new TypeWrapper){
        Enqueue(i);
    }

    /** Conducts a deep copy of the queue.
        \note O(N)
    */
    inline Queue(const Queue<T> &o){ *this = o; }

    /** Conducts a deep copy of the queue.
        \note O(N)
    */
    inline Queue<T> &operator =(const Queue<T> &o){ return o.CloneTo(*this); }

    /** This is useful for chaining enqueues together.  Ex: q << 1 << 2 << 3*/
    inline Queue<T> &operator <<(const T &i){ Enqueue(i); return *this; }

    /** Conducts a deep copy of the queue.
        \note O(N)
    */
    virtual Queue<T> &CloneTo(Queue<T> &o) const{
        return o;
    }

};


GUTIL_END_CORE_NAMESPACE;

#endif // QUEUE_H
