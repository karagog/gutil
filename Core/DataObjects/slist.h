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

#ifndef GUTIL_SLIST_H
#define GUTIL_SLIST_H

#include "Core/DataObjects/private/slist_p.h"
#include "Core/exception.h"
#include "Core/DataObjects/interfaces.h"
#include "Core/Interfaces/iclonable.h"
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** Implements a singly-linked list.

    A singly-linked list has the advantage of good memory performance while allowing
    one-directional access to the list items.  It keeps a cache of the last item in the list,
    so that you can push items on the back, but you can't use this cache to access list items.
    In this way, it is flexible enough to represent a queue.

    This has a Stack and a Queue interface, so you can have those properties with a
    singly-linked list implementation.
*/
template<class T>class SList :
        public slist_p,
        public Stack<T>,
        public Queue<T>,
        public Interfaces::IClonable< SList<T> >
{
    GUTIL_DISABLE_COPY(SList<T>);
public:
    class iterator;
    class const_iterator;

    /** Removes the item pointed to by the iterator.

        This is the function that's called whenever an item is removed from the list.  You can
        optionally override it to provide custom removal behavior.

        \note The iterator will remain valid after the removal.  It then points to the next
        element which replaced the one we removed on the stack.
        \note O(N) in the worst case.  It's O(1) if you remove the begin() iterator
        If you are removing a lot from within the stack (aka not the top) then you should
        think about using another class like a linked list.
    */
    virtual void Remove(iterator &iter){ remove(iter); }

    /** Insert an item into the list.

        The iterator stays valid after insertion.  It will point to the same item as before, which
        has been shifted 1 place in the list, due to the insertion.

        This is the function that's called whenever an item is added to the list.  You can optionally
        override it to provide custom insertion behavior.
        \note O(1)
    */
    virtual void Insert(const T &i, iterator &iter){ insert(reinterpret_cast<const void* const>(&i), iter);}

    /** Push an item onto the front of the list.
        \note O(1)
    */
    void PushFront(const T &i){ iterator b(begin()); Insert(i, b); }

    /** Push an item at the back of the SList.
        \note O(1)
    */
    void PushBack(const T &i){ iterator e(end()); Insert(i, e); }

    /** Pop an item from the front of the list.
        \note O(1)
    */
    void PopFront(){ if(!IsEmpty()){ iterator b(begin()); Remove(b); } }

    /** The default memory allocator for the slist.  Normally you don't deal with this class*/
    class TypeWrapper :
            public slist_p::type_wrapper
    {
    public:
        virtual T* Copy(const T &t) const{
            return new T(t);
        }
        virtual void Delete(T *t) const{
            delete t;
        }
    private:
        void *CopyVoid(const void *const v) const{
            return reinterpret_cast<void *>( Copy(*reinterpret_cast<const T* const>(v)) );
        }
        void DeleteVoid(void *v) const{
            Delete( reinterpret_cast<T*>(v) );
        }
    };

    /** Creates a new slist with the specified type wrapper. */
    inline explicit SList(TypeWrapper *w = new TypeWrapper)
        :slist_p(w)
    {}

    /** Creates a new slist with the item */
    inline SList(const T &item, TypeWrapper *w = new TypeWrapper)
        :slist_p(w){
        PushFront(item);
    }

    /** Satisfies the Stack abstract interface. */
    void Push(const T &item){ PushFront(item); }

    /** Satisfies the Stack abstract interface. */
    void Pop(){ PopFront(); }

    /** Satisfies the Stack abstract interface. */
    const T &Top() const{ return Front(); }

    /** Satisfies the Stack abstract interface. */
    T &Top(){ return Front(); }

    /** Satisfies the Stack abstract interface. */
    void FlushStack(){ return Clear(); }

    /** How many items in the SList. */
    inline long Count() const{ return _count(); }
    long CountStackItems() const{ return _count(); }
    long CountQueueItems() const{ return _count(); }

    /** Satisfies the Queue abstract interface. */
    void Enqueue(const T &i){ PushBack(i); }

    /** Satisfies the Queue abstract interface. */
    void Dequeue(){ PopFront(); }

    /** Satisfies the Queue abstract interface. */
    const T &Front() const{ return *reinterpret_cast<T const *>(NextNode ? NextNode->Data : 0); }

    /** Satisfies the Queue abstract interface. */
    T &Front(){ return *reinterpret_cast<T *>(NextNode ? NextNode->Data : 0); }

    /** Satisfies the Queue abstract interface. */
    void FlushQueue(){ return Clear(); }


    class iterator :
            public forward_node_iterator
    {
    public:
        typedef T value_type;
        typedef const T *pointer;
        typedef const T &reference;

        inline iterator(node_t *n = 0, node_link *parent = 0)
            :forward_node_iterator(n, parent)
        {}
        inline iterator(const forward_node_iterator &o)
            :forward_node_iterator(o.current, o.parent)
        {}

        /** Dereference the iterator and return a reference to the data. */
        inline T &operator*() { return *(reinterpret_cast<T *>(current->Data)); }
        /** Dereference the iterator and return a pointer to the data. */
        inline T *operator->() { return reinterpret_cast<T *>(current->Data); }

        /** Advances the iterator */
        inline iterator &operator++(){ advance(); return *this; }

        /** Advances the iterator */
        inline iterator operator++(int){iterator ret(*this); advance(); return ret; }

        /** Advances the iterator the specified number of items */
        inline iterator &operator+=(int n){ while(n-- > 0) advance(); return *this; }

        /** Returns a copy of the iterator advanced the specified number of times. */
        inline iterator operator+(int n){ iterator r(*this); while(n-- > 0) r.advance(); return r; }

    };

    class const_iterator :
            public forward_node_iterator
    {
    public:
        typedef T value_type;
        typedef const T *pointer;
        typedef const T &reference;

        inline const_iterator(node_t *const n = 0, const node_link *const parent = 0)
            // Alert: Yes I am casting away the constness, but I am stricly allowing only const
            //  access to it, so I say that's ok, 'cause it means I can still use the forward_node_iterator
            //  for a const_iterator
            :forward_node_iterator(const_cast<node_t *>(n), const_cast<node_link *>(parent))
        {}

        /** Dereference the iterator and return a reference to the data. */
        inline const T &operator*() const { return *(reinterpret_cast<const T *const>(current->Data)); }
        /** Dereference the iterator and return a pointer to the data. */
        inline const T *operator->() const { return reinterpret_cast<const T *const>(current->Data); }

        /** Advances the iterator */
        inline const_iterator &operator++(){ advance(); return *this; }

        /** Advances the iterator */
        inline const_iterator operator++(int){const_iterator ret(*this); advance(); return ret; }

        /** Advances the iterator the specified number of items */
        inline const_iterator &operator+=(int n){ while(n-- > 0) advance(); return *this; }

        /** Returns a copy of the iterator advanced the specified number of times. */
        inline const_iterator operator+(int n){ const_iterator r(*this); while(n-- > 0) r.advance(); return r; }

    };

    /** Returns an iterator starting at the top of the stack. */
    inline iterator begin(){ return iterator(NextNode, this); }

    /** Returns an iterator starting at the top of the stack. */
    inline const_iterator begin() const{ return const_iterator(NextNode, this); }

    /** Returns an invalid iterator that you hit when you iterate to the end of the stack. */
    inline const_iterator end() const{ return m_end == this ? const_iterator(0, this) :
                                                              ++const_iterator(static_cast<node_t*>(m_end)); }

    /** Returns an invalid iterator that you hit when you iterate to the end of the stack. */
    inline iterator end(){ return m_end == this ? iterator(0, this) :
                                                  ++iterator(static_cast<node_t*>(m_end)); }

    /** Conducts a deep copy of the stack.  Overridden from IClonable.
        \note O(N)
    */
    virtual SList<T> &CloneTo(SList<T> &s) const{
        s.Clear();
        _clone_helper(s, NextNode);
        return s;
    }


private:

    /** Recursive function to help clone the slist. */
    static void _clone_helper(SList<T> &s, node_t *n){
        if(n)
        {
            _clone_helper(s, n->NextNode);
            s.Push(*reinterpret_cast<const T *const>(n->Data));
        }
    }

};


GUTIL_END_CORE_NAMESPACE;

#endif // GUTIL_SLIST_H
