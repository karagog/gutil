/*Copyright 2010-2015 George Karagoulis

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

#include <gutil/icollections.h>
#include <gutil/flexibletypecomparer.h>
#include <gutil/exception.h>
#include <gutil/globals.h>
#include <malloc.h>
NAMESPACE_GUTIL;


/** The DListImp provides a doubly-linked list.

    Doubly-linked lists are optimized for lots of insertions/removals, but do not allow random-access
    to the elements.  The only way to access items inside the list (other than the front and back)
    is with an iterator.

    If you need random access, you can use the Vector or List.

    \sa List, Vector, SList
*/
template<class T>class DListImp
{
    /** Describes a node of the dlist. */
    class node
    {
    public:
        node(const T &i, node *next = 0, node *prev = 0)
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
    DListImp()
        :m_size(0),
          m_first(0),
          m_last(0)
    {}

    /** Constructs a list with the item in it. */
    explicit DListImp(const T &i)
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
    DListImp(const DListImp<T> &o)
        :m_size(0),
          m_first(0),
          m_last(0)
    {
        for(const auto &item : o)
            PushBack(item);
    }

    ~DListImp(){ Clear(); }

    /** Conducts a deep copy of the list.
        \note O(N)
    */
    DListImp<T> &operator = (const DListImp<T> &o){
        Clear();
        new(this) DListImp<T>(o); return *this;
    }

    /** Inserts the item into the list.

        The iterator stays valid after the insertion, and it points to the item
        which has just been inserted into the list (so its overall position did not move)
        \note O(1)
    */
    template<class ITERATOR_TYPE>
    T &Insert(const T &item, ITERATOR_TYPE &iter){
        // The iterator must be valid, except in the case when we insert on the end
        if(0 < Count() && !iter.m_current && !iter.m_prev_node)
            throw Exception<>();

        // Insert, and then fix the iterator so it points to the correct node
        T &ret( _insert(item, iter) );

        // If we inserted somewhere in the middle of the list
        if(iter.m_current)
            iter.set_current_node(iter.m_current->PrevNode, 0, 0);

        // If we inserted on the end of the list
        else
            iter.set_current_node(0, 0, m_last);

        return ret;
    }

    /** Inserts the item into the list.
        \note The iterator is not changed in this version of the function
    */
    template<class ITERATOR_TYPE>
    T &Insert(const T &item, const ITERATOR_TYPE &iter){
        // The iterator must be valid, except in the case when we insert on the end
        if(0 < Count() && !iter.m_current && !iter.m_prev_node)
            throw Exception<>();
        return _insert(item, iter);
    }

    /** Remove an item from the list.
        The iterator stays valid after the removal, and it points to the next item in the list.
    */
    template<class ITERATOR_TYPE>
    void Remove(ITERATOR_TYPE &iter){
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
    void Remove(const ITERATOR_TYPE &iter){ _remove(iter); }

    /** Remove all items starting from iter_first and ending just before iter_last.
        The iterators may no longer be valid after removal (don't count on it)
    */
    template<class ITERATOR_TYPE>
    void Remove(const ITERATOR_TYPE &iter_first, const ITERATOR_TYPE &iter_last){
        ITERATOR_TYPE i( iter_first );
        while(0 < Count() && i != iter_last) Remove(i);
    }

    /** Remove N items starting from iter_first.
        The iterator may no longer be valid after removal (don't count on it)
    */
    template<class ITERATOR_TYPE>
    void Remove(const ITERATOR_TYPE &iter_first, GUINT32 N){
        GUINT32 cnt(0); ITERATOR_TYPE i( iter_first );
        while(0 < Count() && cnt++ < N) Remove(i);
    }

    /** Returns the item at the front of the list, or throws an exception if
        there is no such item
    */
    T &Front(){
        GASSERT(m_first);
        if(!m_first) throw IndexOutOfRangeException<>();
        return m_first->Data;
    }

    /** Returns the item at the front of the list, or throws an exception if
        there is no such item
    */
    const T &Front() const{
        GASSERT(m_first);
        if(!m_first) throw IndexOutOfRangeException<>();
        return m_first->Data;
    }

    /** Returns the item at the back of the list, or throws an exception if
        there is no such item
    */
    T &Back(){
        GASSERT(m_last);
        if(!m_last) throw IndexOutOfRangeException<>();
        return m_last->Data;
    }

    /** Returns the item at the back of the list, or throws an exception if
        there is no such item
    */
    const T &Back() const{
        GASSERT(m_last);
        if(!m_last) throw IndexOutOfRangeException<>();
        return m_last->Data;
    }

    /** Pushes the item on the front of the list */
    T &PushFront(const T &i){
        iterator b(begin());
        return Insert(i, b);
    }

    /** Pushes the item on the back of the list */
    T &PushBack(const T &i){
        iterator e(end());
        return Insert(i, e);
    }

    /** Removes the item on the front of the list */
    void PopFront(){
        iterator b(begin());
        Remove(b);
    }

    /** Removes the item on the back of the list */
    void PopBack(){
        iterator e(rbegin());
        Remove(e);
    }

    /** Pushes an item on a logical stack, with appealing syntax. */
    DListImp<T> &operator << (const T &item){ PushBack(item); return *this; }

    /** Pops the top item from a logical stack and copies it into the given variable */
    DListImp<T> &operator >> (T &cpy){ cpy = *rbegin(); PopBack(); return *this; }

    /** How many items are in the dlist */
    GUINT32 Length() const{ return m_size; }

    /** How many items are in the dlist */
    GUINT32 Count() const{ return m_size; }

    /** How many items are in the dlist */
    GUINT32 Size() const{ return m_size; }

    /** Clears all items and reclaims all memory. */
    void Clear(){ Remove(begin(), Count()); }

    /** A bidirectional iterator through the list.

        \note Once you iterate onto the end or rend of the list, you cannot get back
        onto the list.  You would have to manually set the iterator to begin() or rbegin()
    */
    class iterator
    {
        friend class DListImp;
    public:

        /** Creates a null iterator */
        iterator(){ set_current_node(0, 0, 0); }

        /** Advances the iterator */
        iterator &operator++(){ _advance(); return *this; }

        /** Advances the iterator */
        iterator operator++(int){ const_iterator ret(*this); _advance(); return ret; }

        /** Advances the iterator the specified number of items */
        iterator &operator += (GUINT32 n){
            while(n-- > 0) _advance();
            return *this;
        }

        /** Returns a copy of the iterator advanced the specified number of times. */
        iterator operator + (GUINT32 n) const{
            iterator ret(*this);
            while(n-- > 0) ret._advance();
            return ret;
        }
        /** Returns a copy of the iterator advanced the specified number of times. */
        iterator operator + (GINT32 n) const{
            if(n < 0)
                return this->operator - (-n);
            else
                return this->operator + ((GUINT32)n);
        }

        /** Retreats the iterator */
        iterator &operator--(){ _retreat(); return *this; }

        /** Retreats the iterator */
        iterator operator--(int){ const_iterator ret(*this); _retreat(); return ret;}

        /** Retreats the iterator the specified number of items */
        iterator &operator -= (GUINT32 n){
            while(n-- > 0) _retreat();
            return *this;
        }

        /** Returns a copy of the iterator retreated the specified number of times. */
        iterator operator - (GUINT32 n) const{
            iterator ret(*this);
            while(n-- > 0) ret._retreat();
            return ret;
        }
        /** Returns a copy of the iterator retreated the specified number of times. */
        iterator operator - (GINT32 n) const{
            if(n < 0)
                return this->operator + (-n);
            else
                return this->operator - ((GUINT32)n);
        }

        const T &operator *() const{ return m_current->Data; }
        T &operator *(){ return m_current->Data; }
        const T *operator ->() const{ return &m_current->Data; }
        T *operator ->(){ return &m_current->Data; }

        bool operator == (const iterator &o) const{ return m_current == o.m_current; }
        bool operator != (const iterator &o) const{ return m_current != o.m_current; }

        /** Returns if the iterator is valid. */
        operator bool() const{ return m_current; }


    private:

        node *m_current;
        node *m_next_node;
        node *m_prev_node;

        iterator(node *n, node *next, node *prev){
            set_current_node(n, next, prev);
        }

        explicit iterator(node *n){
            set_current_node(n, 0, 0);
        }

        /** Sets the current, next and previous node pointers to the given ones.
            This is just a helper, to make sure you set all three variables
        */
        void set_current_node(node *nd, node *next, node *prev){
            m_current = nd;
            m_next_node = next;
            m_prev_node = prev;
        }

        void _advance(){
            if(m_next_node)
                set_current_node(m_next_node, 0, 0);
            else if(m_current)
                set_current_node(m_current->NextNode, 0, m_current->NextNode ? 0 : m_current);
        }
        void _retreat(){
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
        friend class DListImp;
    public:

        /** Creates a null iterator */
        const_iterator(){ set_current_node(0, 0, 0); }

        const_iterator(const const_iterator &o){
            set_current_node(o.m_current, o.m_next_node, o.m_prev_node);
        }

        const_iterator(const iterator &o){
            set_current_node(o.m_current, o.m_next_node, o.m_prev_node);
        }

        const_iterator &operator = (const const_iterator &i){
            new(this) const_iterator(i);
            return *this;
        }
        const_iterator &operator = (const iterator &i){
            new(this) const_iterator(i);
            return *this;
        }

        /** Advances the iterator */
        const_iterator &operator++(){ _advance(); return *this; }

        /** Advances the iterator */
        const_iterator operator++(int){ const_iterator ret(*this); _advance(); return ret; }

        /** Advances the iterator the specified number of items */
        const_iterator &operator+=(int n){
            while(n-- > 0) _advance();
            return *this;
        }

        /** Returns a copy of the iterator advanced the specified number of times. */
        const_iterator operator + (GUINT32 n) const{
            const_iterator ret(*this);
            while(n-- > 0) ret._advance();
            return ret;
        }
        /** Returns a copy of the iterator advanced the specified number of times. */
        const_iterator operator + (GINT32 n) const{
            if(n < 0)
                return this->operator - (-n);
            else
                return this->operator + ((GUINT32)n);
        }

        /** Retreats the iterator */
        const_iterator &operator--(){ _retreat(); return *this; }

        /** Retreats the iterator */
        const_iterator operator--(int){ const_iterator ret(*this); _retreat(); return ret;}

        /** Retreats the iterator the specified number of items */
        const_iterator &operator-=(int n){
            while(n-- > 0) _retreat();
            return *this;
        }

        /** Returns a copy of the iterator retreated the specified number of times. */
        const_iterator operator - (GUINT32 n) const{
            const_iterator ret(*this);
            while(n-- > 0) ret._retreat();
            return ret;
        }
        /** Returns a copy of the iterator retreated the specified number of times. */
        const_iterator operator - (GINT32 n) const{
            if(n < 0)
                return this->operator + (-n);
            else
                return this->operator - ((GUINT32)n);
        }

        const T &operator *() const{ return m_current->Data; }
        const T *operator ->() const{ return &m_current->Data; }

        bool operator == (const const_iterator &o) const{ return m_current == o.m_current; }
        bool operator != (const const_iterator &o) const{ return m_current != o.m_current; }

        /** Returns if the iterator is valid. */
        operator bool() const{ return m_current; }


    private:

        node *m_current;
        node *m_next_node;
        node *m_prev_node;

        const_iterator(node *n, node *next, node *prev){ set_current_node(n, next, prev); }

        explicit const_iterator(node *n){ set_current_node(n, 0, 0); }

        /** Sets the current, next and previous node pointers to the given ones.
            This is just a helper, to make sure you set all three variables
        */
        void set_current_node(node *nd, node *next, node *prev){
            m_current = nd;
            m_next_node = next;
            m_prev_node = prev;
        }

        void _advance(){
            if(m_next_node)
                set_current_node(m_next_node, 0, 0);
            else if(m_current)
                set_current_node(m_current->NextNode, 0, m_current->NextNode ? 0 : m_current);
        }
        void _retreat(){
            if(m_prev_node)
                set_current_node(m_prev_node, 0, 0);
            else if(m_current)
                set_current_node(m_current->PrevNode, 0, m_current->PrevNode ? 0 : m_current);
        }

    };


    /** Returns an iterator to the beginning of the list */
    iterator begin(){ return iterator(m_first, m_first ? m_first->NextNode : 0, 0); }

    /** Returns an iterator to the beginning of the list */
    const_iterator begin() const{ return const_iterator(m_first, m_first ? m_first->NextNode : 0, 0); }

    /** Returns an iterator to the end of the list */
    iterator end(){ return iterator(0, 0, m_last); }

    /** Returns an iterator to the end of the list */
    const_iterator end() const{ return const_iterator(0, 0, m_last); }

    /** Returns an iterator to the reverse-beginning of the list */
    iterator rbegin(){ return iterator(m_last, 0, m_last ? m_last->PrevNode : 0); }

    /** Returns an iterator to the reverse-beginning of the list */
    const_iterator rbegin() const{ return const_iterator(m_last, 0, m_last ? m_last->PrevNode : 0); }

    /** Returns an iterator to the reverse-end of the list */
    iterator rend(){ return iterator(0, m_first, 0); }

    /** Returns an iterator to the reverse-end of the list */
    const_iterator rend() const{ return const_iterator(0, m_first, 0); }


    /** Sorts the list using the given sort type
        \note Not all sorting algorithms have been implemented
    */
    void Sort(bool ascending = true,
              GUtil::SortTypeEnum e = GUtil::MergeSort,
              const GUtil::IComparer<T> &comparer = GUtil::IComparer<T>())
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
            throw NotImplementedException<>();
        }
    }


private:

    GUINT32 m_size;
    node *m_first;
    node *m_last;

    void _merge_sort(iterator &b, iterator &e, bool ascending, const IComparer<T> &cmp)
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
                else{
                    GASSERT(false);
                }

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

    template<class ITERATOR_TYPE>T &_insert(const T &i, const ITERATOR_TYPE &iter){
        // The iterator must be valid, except in the case when we insert on the end
        GASSERT(0 == Count() || iter.m_current || iter.m_prev_node);

        node *cur( iter.m_current );
        node *new_node( (node*)malloc(sizeof(node)) );
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
        return new_node->Data;
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

        free(cur);
        --m_size;
    }

};



/** This is used to define all the same constructors as the base class so you don't have to
 *  repeat so much code.
 *
 *  \param class_name The name of top-level class for which to define constructors.
 *  \param base_class The base class to whose constructors you will forward parameters.
*/
#define GUTIL_DLIST_CONSTRUCTORS(class_name, base_class) \
    public: \
    class_name(){} \
    explicit class_name(const T &item):base_class(item){} \
    class_name(const base_class &o) :base_class(o){} \


/** Defines the basic DList type without any interfaces.
 *
 *  Use this as the most memory efficient version.
 *
 *  \tparam T The type contained.
 *  \tparam IFace The interface class, or void if no interface.
*/
template<class T, typename IFace = void>class DList : public DListImp<T>
{ GUTIL_DLIST_CONSTRUCTORS(DList, DListImp<T>) };


template<class T>class DList<T, IStack<T> > : public DList<T>, public IStack<T>
{
    GUTIL_DLIST_CONSTRUCTORS(DList, DList<T>)

    virtual T &Push(const T &i){ return DListImp<T>::PushBack(i); }
    virtual void Pop(){ DListImp<T>::PopBack(); }
    virtual const T &Top() const{ return DListImp<T>::Back(); }
    virtual T &Top(){ return DListImp<T>::Back(); }
    virtual void Clear(){ DListImp<T>::Clear(); }
    virtual GUINT32 Size() const{ return DListImp<T>::Count(); }
};


template<class T>class DList<T, IQueue<T> > : public DList<T>, public IQueue<T>
{
    GUTIL_DLIST_CONSTRUCTORS(DList, DList<T>)

    virtual T &Enqueue(const T &i){ return DListImp<T>::PushBack(i); }
    virtual void Dequeue(){ DListImp<T>::PopFront(); }
    virtual T &Front(){ return DListImp<T>::Front(); }
    virtual const T &Front() const{ return DListImp<T>::Front(); }
    virtual void Clear(){ DListImp<T>::Clear(); }
    virtual GUINT32 Size() const{ return DListImp<T>::Count(); }
};


template<class T>class DList<T, IDeque<T> > : public DList<T>, public IDeque<T>
{
    GUTIL_DLIST_CONSTRUCTORS(DList, DList<T>)

    virtual void PushFront(const T &i){ DListImp<T>::PushFront(i); }
    virtual void PushBack(const T &i){ DListImp<T>::PushBack(i); }
    virtual void PopFront(){ DListImp<T>::PopFront(); }
    virtual void PopBack(){ DListImp<T>::PopBack(); }
    virtual const T &Front() const{ return DListImp<T>::Front(); }
    virtual T &Front(){ return DListImp<T>::Front(); }
    virtual const T &Back() const{ return DListImp<T>::Back(); }
    virtual T &Back(){ return DListImp<T>::Back(); }
    virtual void FlushDeque(){ DListImp<T>::Clear(); }
    virtual GUINT32 Size() const{ return DListImp<T>::Count(); }
};


// Both DListImp types can be binary-moved
template<class T>struct IsMovableType< DList<T> >{ enum{ Value = 1 }; };
template<class T>struct IsMovableType< DList<T, IStack<T> > >{ enum{ Value = 1 }; };
template<class T>struct IsMovableType< DList<T, IQueue<T> > >{ enum{ Value = 1 }; };
template<class T>struct IsMovableType< DList<T, IDeque<T> > >{ enum{ Value = 1 }; };


END_NAMESPACE_GUTIL;

#endif // GUTIL_DLIST_H
