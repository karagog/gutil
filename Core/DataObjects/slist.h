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

#include "Core/exception.h"
#include "Core/DataObjects/interfaces.h"
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** Implements a singly-linked list.

    A singly-linked list has the advantage of good memory performance while allowing
    one-directional access to the list items.  It keeps a cache of the last item in the list,
    so that you can push items on the back, but you can't use this cache to access list items.
    In this way, it is flexible enough to represent a queue.
*/
template<class T>class SimpleSList
{

    /** One node of data. */
    class node
    {
    public:
        inline node(const T &i, node *next = 0)
            :NextNode(next),
              Data(i)
        {}

        node *NextNode;
        T Data;
    };

public:
    class iterator;
    class const_iterator;

    /** Removes the item pointed to by the iterator.

        This is the function that's called whenever an item is removed from the list,
        except when you call Clear().

        \note The iterator will remain valid after the removal.  It then points to the next
        element which replaced the one we removed on the stack.
        \note O(N) in the worst case.  It's O(1) if you remove the begin() iterator
        If you are removing a lot from within the stack (aka not the top) then you should
        think about using another class like a linked list.
    */
    inline void Remove(iterator &iter){ _remove(iter); }

    /** Insert an item into the list.

        The iterator stays valid after insertion.  It will point to the same item as before, which
        has been shifted 1 place in the list, due to the insertion.

        This is the function that's called whenever an item is added to the list.  You can optionally
        override it to provide custom insertion behavior.
        \note O(1)
    */
    inline void Insert(const T &i, iterator &iter){
        node *new_node( new node(i, iter.current) );
        if(iter.parent)
            iter.parent->NextNode = new_node;
        else
            m_first = new_node;
        if(!iter.current)
            m_last = new_node;
        ++m_count;

        iter.parent = new_node;
    }

    /** Empties the slist and clears all memory.
        \note O(N)
    */
    inline void Clear(){
        iterator iter(begin());
        while(iter)
            _remove(iter);
    }

    /** Is the container empty? */
    inline bool IsEmpty() const{ return !m_count; }


    /** Creates an empty slist. */
    inline SimpleSList(): m_first(0), m_last(0), m_count(0){}

    /** Creates a new slist with the item at the front of the list. */
    inline SimpleSList(const T &item): m_first(0), m_last(0), m_count(0)
    { iterator i(begin()); Insert(item, i); }

    inline SimpleSList(const SimpleSList<T> &o)
        :m_first(0), m_last(0), m_count(0)
    {
        node *n(o.m_first);
        iterator e(end());
        while(n)
        {
            Insert(n->Data, e);
            n = n->NextNode;
        }
    }
    SimpleSList<T> &operator =(const SimpleSList<T> &o){
        Clear();
        new(this) SimpleSList<T>(o);
    }
    inline ~SimpleSList(){ Clear(); }

    /** How many items in the SList. */
    inline GUINT32 Count() const{ return m_count; }

    class iterator
    {
        friend class SimpleSList;
    public:
        inline iterator(node *n = 0, node *p = 0)
            :current(n),
              parent(p)
        {}

        /** Return a reference to the data. */
        inline T &operator*() { return current->Data; }
        /** Return a const reference to the data. */
        inline const T &operator*() const{ return current->Data; }

        /** Return a pointer to the data. */
        inline T *operator->() { return current->Data; }
        /** Return a const pointer to the data. */
        inline const T *operator->() const{ return &current->Data; }

        /** Advances the iterator */
        inline iterator &operator++(){ advance(); return *this; }

        /** Advances the iterator */
        inline iterator operator++(int){iterator ret(*this); advance(); return ret; }

        /** Advances the iterator the specified number of items */
        inline iterator &operator+=(int n){ while(n-- > 0) advance(); return *this; }

        /** Returns a copy of the iterator advanced the specified number of times. */
        inline iterator operator+(int n){ iterator r(*this); while(n-- > 0) r.advance(); return r; }

        inline bool operator == (const iterator &o) const{ return current == o.current; }
        inline bool operator != (const iterator &o) const{ return current != o.current; }

        /** Returns if the iterator is valid. */
        inline operator bool() const{ return current; }

    protected:

        /** The current node to which the iterator is pointing. */
        node *current;

        /** The parent of the current node.  This allows us to have constant time insertions. */
        node *parent;

    private:

        inline void advance(){ if(current){ parent = current; current = current->NextNode; } }

    };

    /** An iterator that won't modify the list, but it can still modify the values in the list. */
    class const_iterator
    {
        friend class SimpleSList;
    public:

        inline const_iterator(node *n = 0, node *p = 0)
            :current(n),
              parent(p)
        {}

        /** Return a const reference to the data. */
        inline const T &operator*() const { return current->Data; }

        /** Return a const pointer to the data. */
        inline const T *operator->() const { return current->Data; }

        /** Advances the iterator */
        inline const_iterator &operator++(){ advance(); return *this; }

        /** Advances the iterator */
        inline const_iterator operator++(int){const_iterator ret(*this); advance(); return ret; }

        /** Advances the iterator the specified number of items */
        inline const_iterator &operator+=(int n){ while(n-- > 0) advance(); return *this; }

        /** Returns a copy of the iterator advanced the specified number of times. */
        inline const_iterator operator+(int n){ const_iterator r(*this); while(n-- > 0) r.advance(); return r; }

        inline bool operator == (const const_iterator &o) const{ return current == o.current; }
        inline bool operator != (const const_iterator &o) const{ return current != o.current; }

        /** Returns if the iterator is valid. */
        inline operator bool() const{ return current; }

    protected:

        /** The current node to which the iterator is pointing. */
        node *current;

        /** The parent of the current node.  This allows us to have constant time insertions. */
        node *parent;

    private:

        inline void advance(){ if(current){ parent = current; current = current->NextNode; } }

    };

    /** Returns an iterator starting at the top of the stack. */
    inline iterator begin(){ return iterator(m_first); }

    /** Returns an iterator starting at the top of the stack. */
    inline const_iterator begin() const{ return const_iterator(m_first); }

    /** Returns an invalid iterator that you hit when you iterate to the end of the stack. */
    inline const_iterator end() const{ return const_iterator(0, m_last); }

    /** Returns an invalid iterator that you hit when you iterate to the end of the stack. */
    inline iterator end(){ return iterator(0, m_last); }


private:

    node *m_first;
    node *m_last;
    GUINT32 m_count;

    inline void _remove(iterator &iter){
        if(!iter.current)
            return;

        node *n( iter.current->NextNode );
        if(m_last == iter.current)
            m_last = iter.parent;

        if(iter.parent)
            iter.parent->NextNode = n;
        else
            m_first = n;

        delete iter.current;
        iter.current = n;
        --m_count;
    }

};


template<class T>class SList :
        public SimpleSList<T>,
        public Stack<T>,
        public Queue<T>
{
public:

    inline SList(){}
    inline SList(const SimpleSList<T> &o) :SimpleSList<T>(o){}


    /** Satisfies the Stack abstract interface. */
    void Push(const T &i){ typename SimpleSList<T>::iterator b(SList<T>::begin()); this->Insert(i, b); }

    /** Satisfies the Stack abstract interface. */
    void Pop(){ typename SimpleSList<T>::iterator b(SList<T>::begin()); this->Remove(b); }

    /** Satisfies the Stack abstract interface. */
    const T &Top() const{ return *SList<T>::begin(); }

    /** Satisfies the Stack abstract interface. */
    T &Top(){ return *SList<T>::begin(); }

    /** Satisfies the Stack abstract interface. */
    void FlushStack(){ return SList<T>::Clear(); }

    GUINT32 CountStackItems() const{ return SList<T>::Count(); }



    /** Satisfies the Queue abstract interface. */
    void Enqueue(const T &i){ typename SimpleSList<T>::iterator e(SList<T>::end()); this->Insert(i, e); }

    /** Satisfies the Queue abstract interface. */
    void Dequeue(){ typename SimpleSList<T>::iterator b(SList<T>::begin()); this->Remove(b); }

    /** Satisfies the Queue abstract interface. */
    const T &Front() const{ return *SList<T>::begin(); }

    /** Satisfies the Queue abstract interface. */
    T &Front(){ return *SList<T>::begin(); }

    /** Satisfies the Queue abstract interface. */
    void FlushQueue(){ return SList<T>::Clear(); }

    GUINT32 CountQueueItems() const{ return SList<T>::Count(); }

};


GUTIL_END_CORE_NAMESPACE;


namespace GUtil
{

// Both SList types can be binary-moved
template<class T>struct IsMovableType< Core::DataObjects::SimpleSList<T> >{ enum{ Value = 1 }; };
template<class T>struct IsMovableType< Core::DataObjects::SList<T> >{ enum{ Value = 1 }; };

}

#endif // GUTIL_SLIST_H
