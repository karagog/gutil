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

#ifndef GUTIL_DLIST_H
#define GUTIL_DLIST_H

#include "Core/DataObjects/interfaces.h"
#include "Core/Utils/flexibletypecomparer.h"
#include "Core/exception.h"
#include "Core/globals.h"
NAMESPACE_GUTIL1(DataObjects);


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

    /** Builds an empty list. */
    inline DList()
        :m_size(0),
          m_first(0),
          m_last(0)
    {}

    /** Constructs a list with the item in it. */
    inline explicit DList(const T &i)
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
        G_FOREACH_CONST(const T &item, o)
            PushBack(item);
    }

    inline ~DList(){ Clear(); }

    /** Conducts a deep copy of the list.
        \note O(N)
    */
    inline DList<T> &operator = (const DList<T> &o){
        Clear();
        new(this) DList<T>(o); return *this;
    }

    /** Inserts the item into the list.

        The iterator stays valid after the insertion, and it points to the item
        which has just been inserted into the list (so its overall position did not move)
        \note O(1)
    */
    template<class ITERATOR_TYPE>
    inline void Insert(const T &item, ITERATOR_TYPE &iter){
        // The iterator must be valid, except in the case when we insert on the end
        if(0 < Count() && !iter.m_current && !iter.m_prev_node) return;

        // Insert, and then fix the iterator so it points to the correct node
        _insert(item, iter);

        // If we inserted somewhere in the middle of the list
        if(iter.m_current)
            iter.set_current_node(iter.m_current->PrevNode, 0, 0);

        // If we inserted on the end of the list
        else
            iter.set_current_node(0, 0, m_last);
    }

    /** Inserts the item into the list.
        \note The iterator is not changed in this version of the function
    */
    template<class ITERATOR_TYPE>
    inline void Insert(const T &item, const ITERATOR_TYPE &iter){
        // The iterator must be valid, except in the case when we insert on the end
        if(0 < Count() && !iter.m_current && !iter.m_prev_node) return;
        _insert(item, iter);
    }

    /** Remove an item from the list.
        The iterator stays valid after the removal, and it points to the next item in the list.
    */
    template<class ITERATOR_TYPE>
    inline void Remove(ITERATOR_TYPE &iter){
        if(!iter.m_current) return;
        node *nxt(iter.m_current->NextNode);
        node *prev(iter.m_current->PrevNode);

        _remove(iter);

        // Adjust the iterator so it still points to a valid value
        iter.set_current_node(nxt, 0, prev);
    }

    /** Remove an item from the list.
        The iterator is no longer valid after removal
    */
    template<class ITERATOR_TYPE>
    inline void Remove(const ITERATOR_TYPE &iter){ _remove(iter); }

    /** Remove all items starting from iter_first and ending just before iter_last.
        The iterators may no longer be valid after removal (don't count on it)
    */
    template<class ITERATOR_TYPE>
    inline void Remove(const ITERATOR_TYPE &iter_first, const ITERATOR_TYPE &iter_last){
        ITERATOR_TYPE i( iter_first );
        while(0 < Count() && i != iter_last) Remove(i);
    }

    /** Remove N items starting from iter_first.
        The iterator may no longer be valid after removal (don't count on it)
    */
    template<class ITERATOR_TYPE>
    inline void Remove(const ITERATOR_TYPE &iter_first, GUINT32 N){
        GUINT32 cnt(0); ITERATOR_TYPE i( iter_first );
        while(0 < Count() && cnt++ < N) Remove(i);
    }

    /** Returns the item at the front of the list, or throws an exception if
        there is no such item
    */
    inline T &Front(){
        GASSERT(m_first);
        if(!m_first) THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);
        return m_first->Data;
    }

    /** Returns the item at the front of the list, or throws an exception if
        there is no such item
    */
    inline const T &Front() const{
        GASSERT(m_first);
        if(!m_first) THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);
        return m_first->Data;
    }

    /** Returns the item at the back of the list, or throws an exception if
        there is no such item
    */
    inline T &Back(){
        GASSERT(m_last);
        if(!m_last) THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);
        return m_last->Data;
    }

    /** Returns the item at the back of the list, or throws an exception if
        there is no such item
    */
    inline const T &Back() const{
        GASSERT(m_last);
        if(!m_last) THROW_NEW_GUTIL_EXCEPTION(IndexOutOfRangeException);
        return m_last->Data;
    }

    /** Pushes the item on the front of the list */
    inline void PushFront(const T &i){
        iterator b(begin());
        Insert(i, b);
    }

    /** Pushes the item on the back of the list */
    inline void PushBack(const T &i){
        iterator e(end());
        Insert(i, e);
    }

    /** Removes the item on the front of the list */
    inline void PopFront(){
        iterator b(begin());
        Remove(b);
    }

    /** Removes the item on the back of the list */
    inline void PopBack(){
        iterator e(rbegin());
        Remove(e);
    }

    /** Pushes an item on a logical stack, with appealing syntax. */
    inline DList<T> &operator << (const T &item){ PushBack(item); return *this; }

    /** Pops the top item from a logical stack and copies it into the given variable */
    inline DList<T> &operator >> (T &cpy){ cpy = *rbegin(); PopBack(); return *this; }

    /** How many items are in the dlist */
    inline GUINT32 Length() const{ return m_size; }

    /** How many items are in the dlist */
    inline GUINT32 Count() const{ return m_size; }

    /** How many items are in the dlist */
    inline GUINT32 Size() const{ return m_size; }

    /** Clears all items and reclaims all memory. */
    inline void Clear(){ Remove(begin(), Count()); }

    /** A bidirectional iterator through the list.

        \note Once you iterate onto the end or rend of the list, you cannot get back
        onto the list.  You would have to manually set the iterator to begin() or rbegin()
    */
    class iterator
    {
        friend class DList;
    public:

        /** Creates a null iterator */
        inline iterator(){ set_current_node(0, 0, 0); }

        /** Advances the iterator */
        inline iterator &operator++(){ _advance(); return *this; }

        /** Advances the iterator */
        inline iterator operator++(int){ const_iterator ret(*this); _advance(); return ret; }

        /** Advances the iterator the specified number of items */
        inline iterator &operator += (GUINT32 n){
            while(n-- > 0) _advance();
            return *this;
        }

        /** Returns a copy of the iterator advanced the specified number of times. */
        inline iterator operator + (GUINT32 n) const{
            iterator ret(*this);
            while(n-- > 0) ret._advance();
            return ret;
        }
        /** Returns a copy of the iterator advanced the specified number of times. */
        inline iterator operator + (GINT32 n) const{
            if(n < 0) THROW_NEW_GUTIL_EXCEPTION2(Exception, "Cannot use negative values");
            return this->operator + ((GUINT32)n);
        }

        /** Retreats the iterator */
        inline iterator &operator--(){ _retreat(); return *this; }

        /** Retreats the iterator */
        inline iterator operator--(int){ const_iterator ret(*this); _retreat(); return ret;}

        /** Retreats the iterator the specified number of items */
        inline iterator &operator -= (GUINT32 n){
            while(n-- > 0) _retreat();
            return *this;
        }

        /** Returns a copy of the iterator retreated the specified number of times. */
        inline iterator operator - (GUINT32 n) const{
            iterator ret(*this);
            while(n-- > 0) ret._retreat();
            return ret;
        }
        /** Returns a copy of the iterator retreated the specified number of times. */
        inline iterator operator - (GINT32 n) const{
            if(n < 0) THROW_NEW_GUTIL_EXCEPTION2(Exception, "Cannot use negative values");
            return this->operator - ((GUINT32)n);
        }

        inline const T &operator *() const{ return m_current->Data; }
        inline T &operator *(){ return m_current->Data; }
        inline const T *operator ->() const{ return &m_current->Data; }
        inline T *operator ->(){ return &m_current->Data; }

        inline bool operator == (const iterator &o) const{ return m_current == o.m_current; }
        inline bool operator != (const iterator &o) const{ return m_current != o.m_current; }

        /** Returns if the iterator is valid. */
        inline operator bool() const{ return m_current; }


    private:

        node *m_current;
        node *m_next_node;
        node *m_prev_node;

        inline iterator(node *n, node *next, node *prev){
            set_current_node(n, next, prev);
        }

        inline explicit iterator(node *n){
            set_current_node(n, 0, 0);
        }

        /** Sets the current, next and previous node pointers to the given ones.
            This is just a helper, to make sure you set all three variables
        */
        inline void set_current_node(node *nd, node *next, node *prev){
            m_current = nd;
            m_next_node = next;
            m_prev_node = prev;
        }

        inline void _advance(){
            if(m_next_node)
                set_current_node(m_next_node, 0, 0);
            else if(m_current)
                set_current_node(m_current->NextNode, 0, m_current->NextNode ? 0 : m_current);
        }
        inline void _retreat(){
            if(m_prev_node)
                set_current_node(m_prev_node, 0, 0);
            else if(m_current)
                set_current_node(m_current->PrevNode, 0, m_current->PrevNode ? 0 : m_current);
        }

    };

    /** A bidirectional iterator through the list

        \note Once you iterate onto the end or rend of the list, you cannot get back
        onto the list.  You would have to manually set the iterator to begin() or rbegin()
    */
    class const_iterator
    {
        friend class DList;
    public:

        /** Creates a null iterator */
        inline const_iterator(){ set_current_node(0, 0, 0); }

        inline const_iterator(const const_iterator &o){
            set_current_node(o.m_current, o.m_next_node, o.m_prev_node);
        }

        inline const_iterator(const iterator &o){
            set_current_node(o.m_current, o.m_next_node, o.m_prev_node);
        }

        inline const_iterator &operator = (const const_iterator &i){
            new(this) const_iterator(i);
            return *this;
        }
        inline const_iterator &operator = (const iterator &i){
            new(this) const_iterator(i);
            return *this;
        }

        /** Advances the iterator */
        inline const_iterator &operator++(){ _advance(); return *this; }

        /** Advances the iterator */
        inline const_iterator operator++(int){ const_iterator ret(*this); _advance(); return ret; }

        /** Advances the iterator the specified number of items */
        inline const_iterator &operator+=(int n){
            while(n-- > 0) _advance();
            return *this;
        }

        /** Returns a copy of the iterator advanced the specified number of times. */
        inline const_iterator operator + (GUINT32 n) const{
            const_iterator ret(*this);
            while(n-- > 0) ret._advance();
            return ret;
        }
        /** Returns a copy of the iterator advanced the specified number of times. */
        inline const_iterator operator + (GINT32 n) const{
            if(n < 0) THROW_NEW_GUTIL_EXCEPTION2(Exception, "Cannot use negative values");
            return this->operator + ((GUINT32)n);
        }

        /** Retreats the iterator */
        inline const_iterator &operator--(){ _retreat(); return *this; }

        /** Retreats the iterator */
        inline const_iterator operator--(int){ const_iterator ret(*this); _retreat(); return ret;}

        /** Retreats the iterator the specified number of items */
        inline const_iterator &operator-=(int n){
            while(n-- > 0) _retreat();
            return *this;
        }

        /** Returns a copy of the iterator retreated the specified number of times. */
        inline const_iterator operator - (GUINT32 n) const{
            const_iterator ret(*this);
            while(n-- > 0) ret._retreat();
            return ret;
        }
        /** Returns a copy of the iterator retreated the specified number of times. */
        inline const_iterator operator - (int n) const{
            if(n < 0) THROW_NEW_GUTIL_EXCEPTION2(Exception, "Cannot use negative values");
            return this->operator - ((GUINT32)n);
        }

        inline const T &operator *() const{ return m_current->Data; }
        inline const T *operator ->() const{ return &m_current->Data; }

        inline bool operator == (const const_iterator &o) const{ return m_current == o.m_current; }
        inline bool operator != (const const_iterator &o) const{ return m_current != o.m_current; }

        /** Returns if the iterator is valid. */
        inline operator bool() const{ return m_current; }


    private:

        node *m_current;
        node *m_next_node;
        node *m_prev_node;

        inline const_iterator(node *n, node *next, node *prev){ set_current_node(n, next, prev); }

        inline explicit const_iterator(node *n){ set_current_node(n, 0, 0); }

        /** Sets the current, next and previous node pointers to the given ones.
            This is just a helper, to make sure you set all three variables
        */
        inline void set_current_node(node *nd, node *next, node *prev){
            m_current = nd;
            m_next_node = next;
            m_prev_node = prev;
        }

        inline void _advance(){
            if(m_next_node)
                set_current_node(m_next_node, 0, 0);
            else if(m_current)
                set_current_node(m_current->NextNode, 0, m_current->NextNode ? 0 : m_current);
        }
        inline void _retreat(){
            if(m_prev_node)
                set_current_node(m_prev_node, 0, 0);
            else if(m_current)
                set_current_node(m_current->PrevNode, 0, m_current->PrevNode ? 0 : m_current);
        }

    };


    /** Returns an iterator to the beginning of the list */
    inline iterator begin(){ return iterator(m_first, m_first ? m_first->NextNode : 0, 0); }

    /** Returns an iterator to the beginning of the list */
    inline const_iterator begin() const{ return const_iterator(m_first, m_first ? m_first->NextNode : 0, 0); }

    /** Returns an iterator to the end of the list */
    inline iterator end(){ return iterator(0, 0, m_last); }

    /** Returns an iterator to the end of the list */
    inline const_iterator end() const{ return const_iterator(0, 0, m_last); }

    /** Returns an iterator to the reverse-beginning of the list */
    inline iterator rbegin(){ return iterator(m_last, 0, m_last ? m_last->PrevNode : 0); }

    /** Returns an iterator to the reverse-beginning of the list */
    inline const_iterator rbegin() const{ return const_iterator(m_last, 0, m_last ? m_last->PrevNode : 0); }

    /** Returns an iterator to the reverse-end of the list */
    inline iterator rend(){ return iterator(0, m_first, 0); }

    /** Returns an iterator to the reverse-end of the list */
    inline const_iterator rend() const{ return const_iterator(0, m_first, 0); }


    /** Sorts the list using the given sort type
        \note Not all sorting algorithms have been implemented
    */
    void Sort(bool ascending = true,
              GUtil::SortTypeEnum e = GUtil::MergeSort,
              const GUtil::Interfaces::IComparer<T> &comparer = GUtil::DefaultComparer<T>())
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
        // Find the midpoint of the list
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

        // If there are only two items, then sort them
        if(diff == 2)
        {
            node *cur( b.m_current );
            node *last( b.m_current->NextNode );
            if((ascending && 0 < cmp(cur->Data, last->Data)) ||
               (!ascending && 0 > cmp(cur->Data, last->Data)))
            {
                cur->NextNode = last->NextNode;
                last->PrevNode = cur->PrevNode;
                last->NextNode = cur;
                cur->PrevNode = last;

                b.set_current_node(last, last->NextNode, last->PrevNode);
                if(b.m_current->PrevNode)
                    b.m_current->PrevNode->NextNode = last;
                else
                    m_first = last;

                if(e.m_current)
                    e.m_current->PrevNode = cur;
                else
                    m_last = cur;
                e.m_prev_node = cur;
            }
        }

        // If there are more than 2 items then split them again, recursively
        //  merge-sort them, and then reassemble back into a sorted list
        else if(diff > 2)
        {
            // Sort the left and right halves of the list
            _merge_sort(b, m, ascending, cmp);
            _merge_sort(m, e, ascending, cmp);

            // Terminate the list at the middle and the end
            m.m_current->PrevNode->NextNode = NULL;
            if(e.m_current){
                e.m_current->PrevNode->NextNode = NULL;
                if(m.m_current == e.m_current->PrevNode)
                    m.m_next_node = NULL;
            }

            // Join the two halves, which are already sorted
            iterator i1(b), i2(m);
            node **list_start( b.m_current->PrevNode ? &b.m_current->PrevNode->NextNode : &m_first );
            node **start_prev( &b.m_current->PrevNode );
            b.set_current_node(NULL, NULL, NULL);
            while(i1.m_current || i2.m_current)
            {
                bool item1_gt_item2( i1.m_current && i2.m_current &&
                                     ((ascending && 0 < cmp(*i1, *i2)) ||
                                     (!ascending && 0 > cmp(*i1, *i2))) );

                // We grab an item from list 2
                if(!i1.m_current || item1_gt_item2)
                {
                    i2.m_current->PrevNode = *start_prev;
                    *list_start = i2.m_current;

                    ++i2;
                }

                // We grab an item from list 1
                else if(!i2.m_current || !item1_gt_item2)
                {
                    i1.m_current->PrevNode = *start_prev;
                    *list_start = i1.m_current;

                    ++i1;
                }
                else GASSERT(false);

                if(NULL == b.m_current)
                    b.m_current = *list_start;

                start_prev = list_start;
                list_start = &(*list_start)->NextNode;
            }

            if(e.m_current)
                e.m_current->PrevNode = *start_prev;
            else
                m_last = *start_prev;
            (*start_prev)->NextNode = e.m_current;

            e.m_prev_node = *start_prev;
            b.m_next_node = b.m_current->NextNode;
            b.m_prev_node = b.m_current->PrevNode;
        }
    }

    template<class ITERATOR_TYPE>void _insert(const T &i, const ITERATOR_TYPE &iter){
        // The iterator must be valid, except in the case when we insert on the end
        GASSERT(0 == Count() || iter.m_current || iter.m_prev_node);

        node *cur( iter.m_current );
        node *new_node( GMALLOC(node) );
        new(new_node) node(i, cur);

        if(cur){
            new_node->PrevNode = cur->PrevNode;
            if(cur == m_first)
                m_first = new_node;
            else
                cur->PrevNode->NextNode = new_node;
            cur->PrevNode = new_node;
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

    template<class ITERATOR_TYPE>void _remove(const ITERATOR_TYPE &iter){
        node *cur( iter.m_current );
        if(!cur) return;

        // If we're removing somewhere other than the begin/end
        if(cur != m_first && cur != m_last){
            cur->PrevNode->NextNode = cur->NextNode;
            cur->NextNode->PrevNode = cur->PrevNode;
        }

        // If we're removing from either the beginning or end
        else{
            if(cur == m_first){
                if((m_first = cur->NextNode))
                    m_first->PrevNode = 0;
            }
            if(cur == m_last){
                if((m_last = cur->PrevNode))
                    m_last->NextNode = 0;
            }
        }

        GFREE(cur);
        --m_size;
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
    const T &Top() const{ return m_list->Back(); }

    /** Satisfies the Stack abstract interface. */
    T &Top(){ return m_list->Back(); }

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
    T &Front(){ return m_list->Front(); }

    /** Satisfies the Queue abstract interface. */
    const T &Front() const{ return m_list->Front(); }

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
    const T &Front() const{ return m_list->Front(); }

    /** Satisfies the Deque abstract interface. */
    T &Front(){ return m_list->Front(); }

    /** Satisfies the Deque abstract interface. */
    const T &Back() const{ return m_list->Back(); }

    /** Satisfies the Deque abstract interface. */
    T &Back(){ return m_list->Back(); }

    /** Satisfies the Deque abstract interface. */
    void FlushDeque(){ m_list->Clear(); }

    /** Satisfies the Deque abstract interface. */
    GUINT32 CountDequeItems() const{ return m_list->Count(); }


private:

    DList<T> *m_list;
    bool m_delete;

};


END_NAMESPACE_GUTIL1;


namespace GUtil
{

// Both DList types can be binary-moved
template<class T>struct IsMovableType< DataObjects::DList<T> >{ enum{ Value = 1 }; };
template<class T>struct IsMovableType< DataObjects::DListStack<T> >{ enum{ Value = 1 }; };
template<class T>struct IsMovableType< DataObjects::DListQueue<T> >{ enum{ Value = 1 }; };
template<class T>struct IsMovableType< DataObjects::DListDeque<T> >{ enum{ Value = 1 }; };

}


#endif // GUTIL_DLIST_H
