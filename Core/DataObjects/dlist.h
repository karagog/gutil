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

#ifndef GUTIL_DLIST_H
#define GUTIL_DLIST_H

#include "Core/DataObjects/interfaces.h"
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** The SimpleDList provides a doubly-linked list.

    Doubly-linked lists are optimized for lots of insertions/removals, but do not allow random-access
    to the elements.  The only way to access items inside the list (other than the front and back)
    is with an iterator.

    If you need random access, you can use the Vector or List.

    \sa List, Vector, SList
*/
template<class T>class SimpleDList
{

    /** Describes a node of the dlist. */
    class node
    {
    public:
        inline node(const T &i, node *next = 0, node *prev = 0)
            :NextNode(next),
              PrevNode(prev),
              Data(i)
        {}

        node *NextNode;
        node *PrevNode;
        T Data;
    };

public:
    class iterator;
    class const_iterator;

    /** Inserts the item into the list.

        This function is called any time a new item is added to the list.

        The iterator stays valid after the insertion, and it points to the same item it did before
        the insertion, only now it has been shifted one spot in the list.
        \note O(1)
    */
    virtual void Insert(const T &i, iterator &iter)
    {
        node *new_node( new node(i, iter.current) );

        if(iter.current)
        {
            new_node->PrevNode = iter.current->PrevNode;
            if(iter.current == m_first)
                m_first = new_node;
            else
                iter.current->PrevNode->NextNode = new_node;
            iter.current->PrevNode = new_node;
        }
        else
        {
            // Pushing onto the end of the list.
            if(m_last)
            {
                // The list already has items in it.
                m_last->NextNode = new_node;
                new_node->PrevNode = m_last;
            }
            else
            {
                // Pushing onto empty list
                m_first = new_node;
            }
            m_last = new_node;
        }

        m_size++;
    }

    /** Remove an item from the list.

        This function is called any time an item is removed from the list.

        The iterator stays valid after the removal, and it points to the next item in the list.
        \note O(1)
    */
    virtual void Remove(iterator &iter){ _remove(iter); }

    /** How many items are in the dlist. */
    inline GUINT32 Length() const{ return m_size; }
    inline GUINT32 Count() const{ return m_size; }

    /** Clears all items and reclaims all memory. */
    inline void Clear(){
        iterator iter(begin());
        while(iter)
            _remove(iter);
    }

    /** A bidirectional iterator through the list. */
    class iterator
    {
        friend class SimpleDList;
    public:
        inline iterator(node *n = 0)
            :current(n){}

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

        inline const T &operator *() const{ return current->Data; }
        inline T &operator *(){ return current->Data; }
        inline const T *operator ->() const{ return current->Data; }
        inline T *operator ->(){ return current->Data; }

        inline bool operator == (const iterator &o) const{ return current == o.current; }
        inline bool operator != (const iterator &o) const{ return current != o.current; }

        /** Returns if the iterator is valid. */
        inline operator bool() const{ return current; }

    protected:

        node *current;

        inline void advance(){ if(current) current = current->NextNode; }
        inline void retreat(){ if(current) current = current->PrevNode; }

    };

    /** A bidirectional iterator through the list. */
    class const_iterator
    {
        friend class SimpleDList;
    public:
        inline const_iterator(node *n = 0)
            :current(n){}

        inline const_iterator(const const_iterator &o)
            :current(o.current){}
        inline const_iterator(const iterator &o)
            :current(o.current){}

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

        inline const T &operator *() const{ return current->Data; }
        inline const T *operator ->() const{ &current->Data; }

        inline bool operator == (const const_iterator &o) const{ return current == o.current; }
        inline bool operator != (const const_iterator &o) const{ return current != o.current; }

        /** Returns if the iterator is valid. */
        inline operator bool() const{ return current; }

    protected:

        node *current;

        inline void advance(){ if(current) current = current->NextNode; }
        inline void retreat(){ if(current) current = current->PrevNode; }

    };

    inline iterator begin(){ return iterator(m_first); }
    inline const_iterator begin() const{ return const_iterator(m_first); }
    inline iterator end(){ return ++iterator(m_last); }
    inline const_iterator end() const{ return ++const_iterator(m_last); }
    inline iterator rbegin(){ return iterator(m_last); }
    inline const_iterator rbegin() const{ return const_iterator(m_last); }
    inline iterator rend(){ return --iterator(m_first); }
    inline const_iterator rend() const{ return --const_iterator(m_first); }

    /** Builds an empty list. */
    inline SimpleDList()
        :m_size(0),
          m_first(0),
          m_last(0)
    {}

    /** Constructs a list with the item in it. */
    inline SimpleDList(const T &i)
        :m_size(0),
          m_first(0),
          m_last(0)
    {
        iterator iter(begin());
        Insert(i, iter);
    }

    /** Conducts a deep copy of the list.
        \note O(N)
    */
    inline SimpleDList(const SimpleDList<T> &o)
        :m_size(0),
          m_first(0),
          m_last(0)
    {
        SimpleDList<T>::const_iterator iter(o.begin());
        SimpleDList<T>::iterator e(end());
        while(iter)
        {
            Insert(*iter, e);
            ++iter;
        }
    }

    inline ~SimpleDList(){ Clear(); }

    /** Conducts a deep copy of the list.
        \note O(N)
    */
    inline SimpleDList<T> &operator =(const SimpleDList<T> &o){ ::new(this) SimpleDList<T>(o); return *this; }

    /** This is useful for chaining push commands together.  Ex: q << 1 << 2 << 3*/
    inline SimpleDList<T> &operator <<(const T &i){ PushBack(i); return *this; }


private:

    GUINT32 m_size;
    node *m_first;
    node *m_last;

    void _remove(iterator &iter)
    {
        if(!iter)
            return;

        node *n(iter.current);
        if(m_first == n)
            m_first = n->NextNode;
        else
            n->PrevNode->NextNode = n->NextNode;

        // So the iterator is still valid after the removal.
        iter.current = n->NextNode;

        if(n->NextNode)
            n->NextNode->PrevNode = n->PrevNode;
        else
            m_last = n->PrevNode;

        delete n;
        m_size--;
    }

};



template<class T>class DList :
        public SimpleDList<T>,
        public Stack<T>,
        public Deque<T>
{
public:

    inline DList(){}
    inline DList(const SimpleDList<T> &o) :SimpleDList<T>(o){}

    /** Satisfies the Dequeue abstract interface. */
    void PushFront(const T &i){ typename SimpleDList<T>::iterator b(DList<T>::begin()); this->Insert(i, b); }

    /** Satisfies the Dequeue abstract interface. */
    void PushBack(const T &i){ typename SimpleDList<T>::iterator e(DList<T>::end()); this->Insert(i, e); }

    /** Satisfies the Dequeue abstract interface. */
    void PopFront(){ typename SimpleDList<T>::iterator b(DList<T>::begin()); this->Remove(b); }

    /** Satisfies the Dequeue abstract interface. */
    void PopBack(){ typename SimpleDList<T>::iterator e(DList<T>::rbegin()); this->Remove(e); }

    /** Satisfies the Dequeue abstract interface. */
    const T &Front() const{ return *DList<T>::begin(); }

    /** Satisfies the Dequeue abstract interface. */
    T &Front(){ return *DList<T>::begin(); }

    /** Satisfies the Dequeue abstract interface. */
    const T &Back() const{ return *DList<T>::rbegin(); }

    /** Satisfies the Dequeue abstract interface. */
    T &Back(){ return *DList<T>::rbegin(); }

    /** Satisfies the Dequeue abstract interface. */
    void FlushDeque(){ DList<T>::Clear(); }

    /** Satisfies the Dequeue abstract interface. */
    GUINT32 CountDequeItems() const{ return DList<T>::Count(); }



    /** Satisfies the Stack abstract interface. */
    void Push(const T &i){ typename SimpleDList<T>::iterator e(DList<T>::end()); this->Insert(i, e); }

    /** Satisfies the Stack abstract interface. */
    void Pop(){ typename SimpleDList<T>::iterator e(DList<T>::rbegin()); this->Remove(e); }

    /** Satisfies the Stack abstract interface. */
    const T &Top() const{ return *DList<T>::rbegin(); }

    /** Satisfies the Stack abstract interface. */
    T &Top(){ return *DList<T>::rbegin(); }

    /** Satisfies the Stack abstract interface. */
    void FlushStack(){ DList<T>::Clear(); }

    /** Satisfies the Stack abstract interface. */
    GUINT32 CountStackItems() const{ return DList<T>::Count(); }

};


GUTIL_END_CORE_NAMESPACE;

#endif // GUTIL_DLIST_H
