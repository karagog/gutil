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
#include "Core/exception.h"
#include "gutil_globals.h"
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** The DList provides a doubly-linked list.

    Doubly-linked lists are optimized for lots of insertions/removals, but do not allow random-access
    to the elements.  The only way to access items inside the list (other than the front and back)
    is with an iterator.

    If you need random access, you can use the Vector or List.

    \sa List, Vector, SList
*/
template<class T>class DList
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
    void Insert(const T &i, iterator &iter)
    {
        node *new_node( reinterpret_cast<node *>(malloc(sizeof(node))) );
        if(new_node == NULL)
            THROW_NEW_GUTIL_EXCEPTION(BadAllocationException);
        new(new_node) node(i, iter.current);

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
    void Remove(iterator &iter)
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

    inline void PushFront(const T &i)
    {
        iterator b(begin());
        Insert(i, b);
    }
    inline void PushBack(const T &i)
    {
        iterator e(end());
        Insert(i, e);
    }
    inline void PopFront()
    {
        iterator b(begin());
        Remove(b);
    }
    inline void PopBack()
    {
        iterator e(rbegin());
        Remove(e);
    }

    /** How many items are in the dlist. */
    inline GUINT32 Length() const{ return m_size; }
    inline GUINT32 Count() const{ return m_size; }

    /** Clears all items and reclaims all memory. */
    inline void Clear(){
        iterator iter(begin());
        while(iter)
            Remove(iter);
    }

    /** A bidirectional iterator through the list. */
    class iterator
    {
        friend class DList;
    public:
        inline iterator(node *n = 0)
            :current(n){}

        /** Advances the iterator */
        inline iterator &operator++(){ advance(); return *this; }

        /** Advances the iterator */
        inline iterator operator++(int){ const_iterator ret(*this); advance(); return ret; }

        /** Advances the iterator the specified number of items */
        inline iterator &operator += (GUINT32 n){
            while(n-- > 0) advance();
            return *this;
        }

        /** Returns a copy of the iterator advanced the specified number of times. */
        inline iterator operator + (GUINT32 n){
            iterator ret(*this);
            while(n-- > 0) ret.advance();
            return ret;
        }

        /** Retreats the iterator */
        inline iterator &operator--(){ retreat(); return *this; }

        /** Retreats the iterator */
        inline iterator operator--(int){ const_iterator ret(*this); retreat(); return ret;}

        /** Retreats the iterator the specified number of items */
        inline iterator &operator -= (GUINT32 n){
            while(n-- > 0) retreat();
            return *this;
        }

        /** Returns a copy of the iterator retreated the specified number of times. */
        inline iterator operator - (GUINT32 n){
            iterator ret(*this);
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
        friend class DList;
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
    inline DList()
        :m_size(0),
          m_first(0),
          m_last(0)
    {}

    /** Constructs a list with the item in it. */
    inline DList(const T &i)
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
    inline DList(const DList<T> &o)
        :m_size(0),
          m_first(0),
          m_last(0)
    {
        DList<T>::const_iterator iter(o.begin());
        DList<T>::iterator e(end());
        while(iter)
        {
            Insert(*iter, e);
            ++iter;
        }
    }

    inline ~DList(){ Clear(); }

    /** Conducts a deep copy of the list.
        \note O(N)
    */
    inline DList<T> &operator =(const DList<T> &o){ new(this) DList<T>(o); return *this; }

    /** This is useful for chaining push commands together.  Ex: q << 1 << 2 << 3*/
    inline DList<T> &operator <<(const T &i){ PushBack(i); return *this; }


    void Sort(bool ascending = true,
              GUtil::SortTypeEnum e = GUtil::MergeSort,
              const Interfaces::IComparer<T> &comparer = DefaultComparer<T>())
    {
        switch(e)
        {
        case MergeSort:
        {
            iterator b(begin()), e(end());
            _merge_sort(b, e, ascending, comparer);
        }
            break;
        default:
            THROW_NEW_GUTIL_EXCEPTION(NotImplementedException);
        }
    }


private:

    GUINT32 m_size;
    node *m_first;
    node *m_last;

    void _merge_sort(iterator &b, iterator &e, bool ascending, const Interfaces::IComparer<T> &cmp)
    {
        GUINT32 diff(0);
        iterator m(b);
        {
            iterator c(b);
            bool even(false);
            while(c != e)
            {
                ++c, ++diff;
                even = !even;
                if(even) ++m;
            }
        }

        if(diff == 2)
        {
            node *cur( b.current );
            node *last( b.current->NextNode );
            if((ascending && 0 < cmp(cur->Data, last->Data)) ||
               (!ascending && 0 > cmp(cur->Data, last->Data)))
            {
                cur->NextNode = last->NextNode;
                last->PrevNode = cur->PrevNode;
                last->NextNode = cur;
                cur->PrevNode = last;

                b.current = last;
                if(b.current->PrevNode)
                    b.current->PrevNode->NextNode = last;
                else
                    m_first = last;

                if(e.current)
                    e.current->PrevNode = cur;
                else
                    m_last = cur;
            }
        }
        else if(diff > 2)
        {
            // Sort the left and right halves of the list
            _merge_sort(b, m, ascending, cmp);
            _merge_sort(m, e, ascending, cmp);

            // Terminate the list at the middle and the end
            m.current->PrevNode->NextNode = NULL;
            if(e.current)
                e.current->PrevNode->NextNode = NULL;

            // Join the two halves, which are already sorted
            iterator i1(b), i2(m);
            node **new_list(b.current->PrevNode ? &b.current->PrevNode->NextNode : &m_first);
            node **prev_new_list(&b.current->PrevNode);
            b.current = NULL;
            while(i1.current && i2.current)
            {
                if((ascending && 0 < cmp(*i1, *i2)) ||
                   (!ascending && 0 > cmp(*i1, *i2)))
                {
                    i2.current->PrevNode = prev_new_list == NULL ? NULL : *prev_new_list;
                    *new_list = i2.current;

                    prev_new_list = new_list;
                    new_list = &(*new_list)->NextNode;
                    if(b.current == NULL)
                    {
                        b.current = i2.current;
                        if(b.current->PrevNode) b.current->PrevNode->NextNode = b.current;
                        else m_first = b.current;
                    }
                    ++i2;
                }
                else
                {
                    i1.current->PrevNode = prev_new_list == NULL ? NULL : *prev_new_list;
                    *new_list = i1.current;

                    prev_new_list = new_list;
                    new_list = &(*new_list)->NextNode;
                    if(b.current == NULL)
                    {
                        b.current = i1.current;
                        if(b.current->PrevNode) b.current->PrevNode->NextNode = b.current;
                        else m_first = b.current;
                    }
                    ++i1;
                }
            }
            while(i1.current)
            {
                i1.current->PrevNode = *prev_new_list;
                *new_list = i1.current;
                prev_new_list = new_list;
                new_list = &(*new_list)->NextNode;
                ++i1;
            }
            while(i2.current)
            {
                i2.current->PrevNode = *prev_new_list;
                *new_list = i2.current;
                prev_new_list = new_list;
                new_list = &(*new_list)->NextNode;
                ++i2;
            }

            if(e.current)
                e.current->PrevNode = *prev_new_list;
            else
                m_last = *prev_new_list;
            (*prev_new_list)->NextNode = e.current;
        }
    }

};



template<class T>class DListStack : public Stack<T>
{
    GUTIL_DISABLE_COPY(DListStack<T>)
public:

    inline DListStack(DList<T> *lst) :m_list(lst), m_delete(false){}
    inline DListStack() :m_list(new DList<T>), m_delete(true){}
    inline ~DListStack(){ if(m_delete) delete m_list; }

    /** Satisfies the Stack abstract interface. */
    void Push(const T &i){ m_list->PushBack(i); }

    /** Satisfies the Stack abstract interface. */
    void Pop(){ m_list->PopBack(); }

    /** Satisfies the Stack abstract interface. */
    const T &Top() const{ return *m_list->rbegin(); }

    /** Satisfies the Stack abstract interface. */
    T &Top(){ return *m_list->rbegin(); }

    /** Satisfies the Stack abstract interface. */
    void FlushStack(){ m_list->Clear(); }

    /** Satisfies the Stack abstract interface. */
    GUINT32 CountStackItems() const{ return m_list->Count(); }


private:

    DList<T> *m_list;
    bool m_delete;

};


template<class T>class DListQueue : public Queue<T>
{
    GUTIL_DISABLE_COPY(DListQueue<T>)
public:

    inline DListQueue(DList<T> *lst) :m_list(lst), m_delete(false){}
    inline DListQueue() :m_list(new DList<T>), m_delete(true){}
    inline ~DListQueue(){ if(m_delete) delete m_list; }

    /** Satisfies the Queue abstract interface. */
    void Enqueue(const T &i){ m_list->PushBack(i); }

    /** Satisfies the Queue abstract interface. */
    void Dequeue(){ m_list->PopFront(); }

    /** Satisfies the Queue abstract interface. */
    T &Front(){ return *m_list->begin(); }

    /** Satisfies the Queue abstract interface. */
    const T &Front() const{ return *m_list->begin(); }

    /** Satisfies the Queue abstract interface. */
    void FlushQueue(){ m_list->Clear(); }

    /** Satisfies the Queue abstract interface. */
    GUINT32 CountQueueItems() const{ return m_list->Count(); }


private:

    DList<T> *m_list;
    bool m_delete;

};


template<class T>class DListDeque : public Deque<T>
{
    GUTIL_DISABLE_COPY(DListDeque<T>)
public:

    inline DListDeque(DList<T> *lst) :m_list(lst), m_delete(false){}
    inline DListDeque() :m_list(new DList<T>), m_delete(true){}
    inline ~DListDeque(){ if(m_delete) delete m_list; }

    /** Satisfies the Deque abstract interface. */
    void PushFront(const T &i){ m_list->PushFront(i); }

    /** Satisfies the Deque abstract interface. */
    void PushBack(const T &i){ m_list->PushBack(i); }

    /** Satisfies the Deque abstract interface. */
    void PopFront(){ m_list->PopFront(); }

    /** Satisfies the Deque abstract interface. */
    void PopBack(){ m_list->PopBack(); }

    /** Satisfies the Deque abstract interface. */
    const T &Front() const{ return *m_list->begin(); }

    /** Satisfies the Deque abstract interface. */
    T &Front(){ return *m_list->begin(); }

    /** Satisfies the Deque abstract interface. */
    const T &Back() const{ return *m_list->rbegin(); }

    /** Satisfies the Deque abstract interface. */
    T &Back(){ return *m_list->rbegin(); }

    /** Satisfies the Deque abstract interface. */
    void FlushDeque(){ m_list->Clear(); }

    /** Satisfies the Deque abstract interface. */
    GUINT32 CountDequeItems() const{ return m_list->Count(); }


private:

    DList<T> *m_list;
    bool m_delete;

};


GUTIL_END_CORE_NAMESPACE;


namespace GUtil
{

// Both DList types can be binary-moved
template<class T>struct IsMovableType< Core::DataObjects::DList<T> >{ enum{ Value = 1 }; };
template<class T>struct IsMovableType< Core::DataObjects::DListStack<T> >{ enum{ Value = 1 }; };
template<class T>struct IsMovableType< Core::DataObjects::DListQueue<T> >{ enum{ Value = 1 }; };
template<class T>struct IsMovableType< Core::DataObjects::DListDeque<T> >{ enum{ Value = 1 }; };

}


#endif // GUTIL_DLIST_H
