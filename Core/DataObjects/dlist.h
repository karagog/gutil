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

#include "Core/DataObjects/private/dlist_p.h"
#include "Core/DataObjects/private/flexible_type_comparer.h"
#include "Core/Interfaces/iclonable.h"
#include "Core/DataObjects/interfaces.h"
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** The DList provides a doubly-linked list.

    Doubly-linked lists are optimized for lots of insertions/removals, but do not allow random-access
    to the elements.  The only way to access items inside the list (other than the front and back)
    is with an iterator.

    If you need random access, you can use the Vector or List.

    \sa List, Vector, SList
*/
template<class T>class DList :
        public dlist_p,
        public Interfaces::IClonable< DList<T> >,
        public Stack<T>,
        public Deque<T>
{
public:

    /** Satisfies the Dequeue abstract interface. */
    void PushFront(const T &i){ push_front(&i); }

    /** Satisfies the Dequeue abstract interface. */
    void PushBack(const T &i){ push_back(&i); }

    /** Satisfies the Dequeue abstract interface. */
    void PopFront(){ pop_front(); }

    /** Satisfies the Dequeue abstract interface. */
    void PopBack(){ pop_back(); }

    /** Satisfies the Dequeue abstract interface. */
    const T &Front() const{ return *reinterpret_cast<T const *>(NextNode->Data);; }

    /** Satisfies the Dequeue abstract interface. */
    T &Front(){ return *reinterpret_cast<T *>(NextNode->Data); }

    /** Satisfies the Dequeue abstract interface. */
    const T &Back() const{ return *reinterpret_cast<T const *>(PreviousNode->Data); }

    /** Satisfies the Dequeue abstract interface. */
    T &Back(){ return *reinterpret_cast<T *>(PreviousNode->Data); }

    /** Satisfies the Dequeue abstract interface. */
    void FlushQueue(){ Clear(); }

    /** Satisfies the Dequeue abstract interface. */
    long CountQueueItems() const{ return Size(); }

    /** Satisfies the Stack abstract interface. */
    void Push(const T &i){ PushFront(i); }

    /** Satisfies the Stack abstract interface. */
    void Pop(){ PopFront(); }

    /** Satisfies the Stack abstract interface. */
    const T &Top() const{ return Front(); }

    /** Satisfies the Stack abstract interface. */
    T &Top(){ return Front(); }

    /** Satisfies the Stack abstract interface. */
    void FlushStack(){ Clear(); }

    /** Satisfies the Stack abstract interface. */
    long CountStackItems() const{ return Size(); }

    /** A bidirectional iterator through the list. */
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

        inline const T &operator *() const{ return *reinterpret_cast<T const*>(current->Data); }
        inline T &operator *(){ return *reinterpret_cast<T *>(current->Data); }
        inline const T *operator ->() const{ return reinterpret_cast<T const*>(current->Data); }
        inline T *operator ->(){ return reinterpret_cast<T *>(current->Data); }
    };

    /** A bidirectional iterator through the list. */
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
        inline T &operator *(){ return *reinterpret_cast<T *>(current->Data); }
        inline const T *operator ->() const{ return reinterpret_cast<T const *>(current->Data); }
        inline T *operator ->(){ return reinterpret_cast<T *>(current->Data); }
    };

    inline iterator begin(){ return iterator(NextNode); }
    inline const_iterator begin() const{ return const_iterator(NextNode); }
    inline iterator end(){ return ++iterator(PreviousNode); }
    inline const_iterator end() const{ return ++const_iterator(PreviousNode); }
    inline iterator rbegin(){ return iterator(PreviousNode); }
    inline const_iterator rbegin() const{ return const_iterator(PreviousNode); }
    inline iterator rend(){ return --iterator(NextNode); }
    inline const_iterator rend() const{ return --const_iterator(NextNode); }

    class TypeWrapper :
            public dlist_p::type_wrapper,
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

    /** Builds an empty list. */
    inline DList()
        :dlist_p(new TypeWrapper)
    {}

    /** Constructs a list with the item in it. */
    inline DList(const T &i)
        :dlist_p(new TypeWrapper)
    {
        PushBack(i);
    }

    /** Conducts a deep copy of the list.
        \note O(N)
    */
    inline DList(const DList<T> &o){ *this = o; }

    /** Conducts a deep copy of the list.
        \note O(N)
    */
    inline DList<T> &operator =(const DList<T> &o){ return o.CloneTo(*this); }

    /** This is useful for chaining push commands together.  Ex: q << 1 << 2 << 3*/
    inline DList<T> &operator <<(const T &i){ PushBack(i); return *this; }

    /** Conducts a deep copy of the list.  Satisfies the IClonable abstract interface.
        \note O(N)
    */
    virtual DList<T> &CloneTo(DList<T> &o) const{
        o.Clear();
        return o;
    }

};


GUTIL_END_CORE_NAMESPACE;

#endif // QUEUE_H
