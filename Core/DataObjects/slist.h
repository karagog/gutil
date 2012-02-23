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

#ifndef GUTIL_SLIST_H
#define GUTIL_SLIST_H

#include "Core/exception.h"
#include "Core/DataObjects/interfaces.h"
#include "Core/globals.h"
#include "Core/Utils/flexibletypecomparer.h"
NAMESPACE_GUTIL1(DataObjects);


/** Implements a singly-linked list.

    A singly-linked list has the advantage of good memory performance while allowing
    one-directional access to the list items.  It keeps a cache of the last item in the list,
    so that you can push items on the back, but you can't use this cache to access list items.
    In this way, it is flexible enough to represent a queue.
*/
template<class T>class SList
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

    /** Creates an empty slist. */
    inline SList(): m_first(0), m_last(0), m_count(0){}

    /** Creates a new slist with the item at the front of the list. */
    inline explicit SList(const T &item): m_first(0), m_last(0), m_count(0)
    { iterator i(begin()); Insert(item, i); }

    /** Conducts a deep copy of the slist */
    inline SList(const SList<T> &o)
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
    /** Conducts a deep copy of the slist */
    SList<T> &operator =(const SList<T> &o){
        Clear();
        new(this) SList<T>(o);
        return *this;
    }
    inline ~SList(){ Clear(); }

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
        node *new_node( reinterpret_cast<node *>(malloc(sizeof(node))) );
        if(new_node == NULL)
            THROW_NEW_GUTIL_EXCEPTION(BadAllocationException);
        new(new_node) node(i, iter.current);

        if(iter.parent)
            iter.parent->NextNode = new_node;
        else
            m_first = new_node;
        if(!iter.current)
            m_last = new_node;
        ++m_count;

        iter.parent = new_node;
    }

    /** Prepends an item on the list. */
    inline void PushFront(const T &i){ iterator iter(begin()); Insert(i, iter); }

    /** Appends an item on the list. */
    inline void PushBack(const T &i){ iterator iter(end()); Insert(i, iter); }

    /** Removes the front of the list. */
    inline void PopFront(){ iterator iter(begin()); Remove(iter); }

    /** Pushes an item on a logical stack, with appealing syntax. */
    inline SList<T> &operator << (const T &item){ PushFront(item); return *this; }

    /** Pops the top item from a logical stack and copies it into the given variable */
    inline SList<T> &operator >> (T &cpy){ cpy = *begin(); PopFront(); return *this; }

    /** Empties the slist and clears all memory.
        \note O(N)
    */
    inline void Clear(){ iterator iter(begin()); while(iter) _remove(iter); }

    /** Is the container empty? */
    inline bool IsEmpty() const{ return !m_count; }


    /** How many items in the SList. */
    inline GUINT32 Count() const{ return m_count; }
    /** How many items in the SList. */
    inline GUINT32 Length() const{ return m_count; }


    /** Sorts the list with the given sorting algorithm. */
    inline void Sort(bool ascending = true,
                     GUtil::SortTypeEnum e = GUtil::MergeSort,
                     const Interfaces::IComparer<T> &comparer = DefaultComparer<T>())
    {
        switch(e)
        {
        case GUtil::MergeSort:
        {
            iterator b(begin()), e(end());
            _merge_sort(b, e, ascending, comparer);
        }
            break;
        default:
            THROW_NEW_GUTIL_EXCEPTION(NotImplementedException);
            break;
        }
    }

    class iterator
    {
        friend class SList;
    public:
        inline iterator(node *n, node *p) :current(n), parent(p) {}
        inline explicit iterator(node *n) :current(n), parent(0) {}
        inline iterator() :current(0), parent(0) {}

        /** Return a reference to the data. */
        inline T &operator*() const{ return current->Data; }

        /** Return a pointer to the data. */
        inline T *operator->() const{ return current->Data; }

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


    private:

        /** The current node to which the iterator is pointing. */
        node *current;

        /** The parent of the current node.  This allows us to have constant time insertions. */
        node *parent;

        inline void advance(){ if(current){ parent = current; current = current->NextNode; } }

    };

    /** An iterator that won't modify the list, but it can still modify the values in the list. */
    class const_iterator
    {
        friend class SList;
    public:
        inline const_iterator(node *n, node *p) :current(n), parent(p) {}
        inline explicit const_iterator(node *n) :current(n), parent(0) {}
        inline const_iterator() :current(0), parent(0) {}

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


    private:

        /** The current node to which the iterator is pointing. */
        node *current;

        /** The parent of the current node.  This allows us to have constant time insertions. */
        node *parent;

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
                last->NextNode = cur;

                b.current = last;
                if(b.parent)
                    b.parent->NextNode = last;
                else
                    m_first = last;

                e.parent = cur;
                if(!e.current)
                    m_last = cur;
            }
        }
        else if(diff > 2)
        {
            // Sort the left and right halves of the list
            _merge_sort(b, m, ascending, cmp);
            _merge_sort(m, e, ascending, cmp);

            // Terminate the list at the middle and the end
            m.parent->NextNode = NULL;
            e.parent->NextNode = NULL;

            // Join the two halves, which are already sorted
            iterator i1(b), i2(m);
            node **new_list(b.parent ? &b.parent->NextNode : &m_first);
            node **prev_new_list(NULL);
            b.current = NULL;
            while(i1.current && i2.current)
            {
                if((ascending && 0 < cmp(*i1, *i2)) ||
                   (!ascending && 0 > cmp(*i1, *i2)))
                {
                    *new_list = i2.current;
                    prev_new_list = new_list;
                    new_list = &(*new_list)->NextNode;
                    if(b.current == NULL)
                    {
                        b.current = i2.current;
                        if(b.parent) b.parent->NextNode = b.current;
                        else m_first = b.current;
                    }
                    ++i2;
                }
                else
                {
                    *new_list = i1.current;
                    prev_new_list = new_list;
                    new_list = &(*new_list)->NextNode;
                    if(b.current == NULL)
                    {
                        b.current = i1.current;
                        if(b.parent) b.parent->NextNode = b.current;
                        else m_first = b.current;
                    }
                    ++i1;
                }
            }
            while(i1.current)
            {
                *new_list = i1.current;
                prev_new_list = new_list;
                new_list = &(*new_list)->NextNode;
                ++i1;
            }
            while(i2.current)
            {
                *new_list = i2.current;
                prev_new_list = new_list;
                new_list = &(*new_list)->NextNode;
                ++i2;
            }

            e.parent = *prev_new_list;
            if(!e.current)
                m_last = *prev_new_list;

            (*prev_new_list)->NextNode = e.current;
        }
    }
};


template<class T>class SListStack : public Stack<T>
{
    GUTIL_DISABLE_COPY(SListStack<T>)
public:

    inline SListStack(SList<T> *lst) :m_list(lst), m_delete(false){}

    inline SListStack() :m_list(new SList<T>), m_delete(true){}

    inline ~SListStack(){ if(m_delete) delete m_list; }

    /** Satisfies the Stack abstract interface. */
    void Push(const T &i){ m_list->PushFront(i); }

    /** Satisfies the Stack abstract interface. */
    void Pop(){ m_list->PopFront(); }

    /** Satisfies the Stack abstract interface. */
    const T &Top() const{ return *m_list->begin(); }

    /** Satisfies the Stack abstract interface. */
    T &Top(){ return *m_list->begin(); }

    /** Satisfies the Stack abstract interface. */
    void FlushStack(){ return m_list->Clear(); }

    /** Satisfies the Stack abstract interface. */
    GUINT32 CountStackItems() const{ return m_list->Count(); }


private:

    SList<T> *m_list;
    bool m_delete;

};


template<class T>class SListQueue : public Queue<T>
{
    GUTIL_DISABLE_COPY(SListQueue<T>)
public:

    inline SListQueue(SList<T> *lst) :m_list(lst), m_delete(false){}

    inline SListQueue() :m_list(new SList<T>), m_delete(true){}

    inline ~SListQueue(){ if(m_delete) delete m_list; }

    /** Satisfies the Queue abstract interface. */
    void Enqueue(const T &i){ m_list->PushBack(i); }

    /** Satisfies the Queue abstract interface. */
    void Dequeue(){ m_list->PopFront(); }

    /** Satisfies the Queue abstract interface. */
    const T &Front() const{ return *m_list->begin(); }

    /** Satisfies the Queue abstract interface. */
    T &Front(){ return *m_list->begin(); }

    /** Satisfies the Queue abstract interface. */
    void FlushQueue(){ return m_list->Clear(); }

    /** Satisfies the Queue abstract interface. */
    GUINT32 CountQueueItems() const{ return m_list->Count(); }


private:

    SList<T> *m_list;
    bool m_delete;

};


END_NAMESPACE_GUTIL1;


namespace GUtil
{

// Both SList types can be binary-moved
template<class T>struct IsMovableType< DataObjects::SList<T> >{ enum{ Value = 1 }; };
template<class T>struct IsMovableType< DataObjects::SListStack<T> >{ enum{ Value = 1 }; };
template<class T>struct IsMovableType< DataObjects::SListQueue<T> >{ enum{ Value = 1 }; };

}

#endif // GUTIL_SLIST_H
