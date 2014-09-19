/*Copyright 2010-2013 George Karagoulis

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

#include "gutil_exception.h"
#include "gutil_icollections.h"
#include "gutil_globals.h"
#include "gutil_flexibletypecomparer.h"
NAMESPACE_GUTIL;


/** Implements a singly-linked list.

    A singly-linked list has the advantage of good memory performance while allowing
    one-directional access to the list items.  It keeps a cache of the last item in the list,
    so that you can push items on the back, but you can't use this cache to access list items.
    In this way, it is flexible enough to represent a queue.
*/
template<class T>class SListImp
{
    struct node;

    node *m_first;
    node *m_last;
    GUINT32 m_count;

    /** One node of data. */
    struct node
    {
        node *NextNode;
        T Data;

        explicit node(const T &i, node *next = NULL)
            :NextNode(next),
              Data(i)
        {}
    };

public:
    class iterator;
    class const_iterator;

    /** Creates an empty slist. */
    SListImp(): m_first(0), m_last(0), m_count(0){}

    /** Creates a new slist with the item at the front of the list. */
    explicit SListImp(const T &item): m_first(0), m_last(0), m_count(0)
    { iterator i(begin()); Insert(item, i); }

    /** Conducts a deep copy of the slist */
    SListImp(const SListImp<T> &o)
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
    SListImp<T> &operator =(const SListImp<T> &o){
        Clear();
        new(this) SListImp<T>(o);
        return *this;
    }

    ~SListImp(){ Clear(); }

    /** Removes the item pointed to by the iterator.

        This is the function that's called whenever an item is removed from the list,
        except when you call Clear().

        \note The iterator will remain valid after the removal.  It then points to the next
        element which replaced the one we removed on the stack.
        \note O(N) in the worst case.  It's O(1) if you remove the begin() iterator
        If you are removing a lot from within the stack (aka not the top) then you should
        think about using another class like a linked list.
    */
    void Remove(iterator &iter){ _remove(iter); }

    /** Insert an item into the list, and returns a reference to the new item.

        The iterator stays valid after insertion.  It will point to the same item as before, which
        has been shifted 1 place in the list, due to the insertion.

        This is the function that's called whenever an item is added to the list.  You can optionally
        override it to provide custom insertion behavior.
        \note O(1)
    */
    T &Insert(const T &i, iterator &iter){
        // We use malloc so we can handle a bad allocation event ourselves
        node *new_node( (node *)malloc(sizeof(node)) );
        if(new_node == NULL)
            throw BadAllocationException<>();
        new(new_node) node(i, iter.current);

        if(iter.parent)
            iter.parent->NextNode = new_node;
        else
            m_first = new_node;
        if(!iter.current)
            m_last = new_node;
        ++m_count;

        iter.parent = new_node;

        return new_node->Data;
    }

    /** Prepends an item on the list. */
    T &PushFront(const T &i){ iterator iter(begin()); return Insert(i, iter); }

    /** Appends an item on the list. */
    T &PushBack(const T &i){ iterator iter(end()); return Insert(i, iter); }

    /** Removes the front of the list. */
    void PopFront(){ iterator iter(begin()); Remove(iter); }

    /** Pushes an item on a logical stack, with appealing syntax. */
    SListImp<T> &operator << (const T &item){ PushFront(item); return *this; }

    /** Pops the top item from a logical stack and copies it into the given variable */
    SListImp<T> &operator >> (T &cpy){ cpy = *begin(); PopFront(); return *this; }

    /** Returns a reference to the first item in the list. */
    T &Front(){ return m_first->Data; }

    /** Returns a reference to the first item in the list. */
    const T &Front() const{ return m_first->Data; }

    /** Returns a reference to the last item in the list. */
    T &Back(){ return m_last->Data; }

    /** Returns a reference to the last item in the list. */
    const T &Back() const{ return m_last->Data; }

    /** Empties the slist and clears all memory.
        \note O(N)
    */
    void Clear(){ iterator iter(begin()); while(iter) _remove(iter); }

    /** Is the container empty? */
    bool IsEmpty() const{ return !m_count; }


    /** How many items in the SListImp. */
    GUINT32 Length() const{ return m_count; }


    /** Sorts the list with the given sorting algorithm. */
    void Sort(bool ascending = true,
                     GUtil::SortTypeEnum e = GUtil::MergeSort,
                     const IComparer<T> &comparer = DefaultComparer<T>())
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
            throw NotImplementedException<>();
            break;
        }
    }

    class iterator
    {
        friend class SListImp;
    public:
        iterator(node *n, node *p) :current(n), parent(p) {}
        explicit iterator(node *n) :current(n), parent(0) {}
        iterator() :current(0), parent(0) {}

        /** Return a reference to the data. */
        T &operator*() const{ return current->Data; }

        /** Return a pointer to the data. */
        T *operator->() const{ return current->Data; }

        /** Advances the iterator */
        iterator &operator++(){ advance(); return *this; }

        /** Advances the iterator */
        iterator operator++(int){iterator ret(*this); advance(); return ret; }

        /** Advances the iterator the specified number of items */
        iterator &operator+=(int n){ while(n-- > 0) advance(); return *this; }

        /** Returns a copy of the iterator advanced the specified number of times. */
        iterator operator+(int n){ iterator r(*this); while(n-- > 0) r.advance(); return r; }

        bool operator == (const iterator &o) const{ return current == o.current; }
        bool operator != (const iterator &o) const{ return current != o.current; }

        /** Returns if the iterator is valid. */
        operator bool() const{ return current; }


    private:

        /** The current node to which the iterator is pointing. */
        node *current;

        /** The parent of the current node.  This allows us to have constant time insertions. */
        node *parent;

        void advance(){ if(current){ parent = current; current = current->NextNode; } }

    };

    /** An iterator that won't modify the list, but it can still modify the values in the list. */
    class const_iterator
    {
        friend class SListImp;
    public:
        const_iterator(node *n, node *p) :current(n), parent(p) {}
        explicit const_iterator(node *n) :current(n), parent(0) {}
        const_iterator() :current(0), parent(0) {}

        /** Return a const reference to the data. */
        const T &operator*() const { return current->Data; }

        /** Return a const pointer to the data. */
        const T *operator->() const { return current->Data; }

        /** Advances the iterator */
        const_iterator &operator++(){ advance(); return *this; }

        /** Advances the iterator */
        const_iterator operator++(int){const_iterator ret(*this); advance(); return ret; }

        /** Advances the iterator the specified number of items */
        const_iterator &operator+=(int n){ while(n-- > 0) advance(); return *this; }

        /** Returns a copy of the iterator advanced the specified number of times. */
        const_iterator operator+(int n){ const_iterator r(*this); while(n-- > 0) r.advance(); return r; }

        bool operator == (const const_iterator &o) const{ return current == o.current; }
        bool operator != (const const_iterator &o) const{ return current != o.current; }

        /** Returns if the iterator is valid. */
        operator bool() const{ return current; }


    private:

        /** The current node to which the iterator is pointing. */
        node *current;

        /** The parent of the current node.  This allows us to have constant time insertions. */
        node *parent;

        void advance(){ if(current){ parent = current; current = current->NextNode; } }

    };

    /** Returns an iterator starting at the top of the stack. */
    iterator begin(){ return iterator(m_first); }

    /** Returns an iterator starting at the top of the stack. */
    const_iterator begin() const{ return const_iterator(m_first); }

    /** Returns an invalid iterator that you hit when you iterate to the end of the stack. */
    const_iterator end() const{ return const_iterator(0, m_last); }

    /** Returns an invalid iterator that you hit when you iterate to the end of the stack. */
    iterator end(){ return iterator(0, m_last); }


private:

    void _remove(iterator &iter){
        if(!iter.current)
            return;

        node *n( iter.current->NextNode );
        if(m_last == iter.current)
            m_last = iter.parent;

        if(iter.parent)
            iter.parent->NextNode = n;
        else
            m_first = n;

        iter.current->~node();
        free(iter.current);

        iter.current = n;
        --m_count;
    }

    void _merge_sort(iterator &b, iterator &e, bool ascending, const IComparer<T> &cmp)
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



/** This is used to define all the same constructors as the base class so you don't have to
 *  repeat so much code.
 *
 *  \param class_name The name of top-level class for which to define constructors.
 *  \param base_class The base class to whose constructors you will forward parameters.
*/
#define GUTIL_SLIST_CONSTRUCTORS(class_name, base_class) \
    public: \
    class_name(){} \
    explicit class_name(const T &item):base_class(item){} \
    class_name(const base_class &o) :base_class(o){} \


/** Defines the basic SList type without any interfaces.
 *
 *  Use this as the most memory efficient version.
 *
 *  \tparam T The type contained.
 *  \tparam IFace The interface class, or void if no interface.
*/
template<class T, typename IFace = void>class SList : public SListImp<T>
{ GUTIL_SLIST_CONSTRUCTORS(SList, SListImp<T>) };



/** Defines a template specialization of the SList class, with a Stack interface. */
template<class T>class SList<T, IStack<T> > :
        public SList<T>,
        public IStack<T>
{
    GUTIL_SLIST_CONSTRUCTORS(SList, SList<T>)

    virtual T &Push(const T &item){ return SListImp<T>::PushFront(item); }
    virtual void Pop(){ SListImp<T>::PopFront(); }
    virtual T &Top(){ return SListImp<T>::Front(); }
    virtual T const &Top() const{ return SListImp<T>::Front(); }
    virtual GUINT32 Size() const{ return SListImp<T>::Length(); }
    virtual void Clear(){ SListImp<T>::Clear(); }

};


/** Defines a template specialization of the SList class, with a Queue interface. */
template<class T>class SList<T, IQueue<T> > :
        public SList<T>,
        public IQueue<T>
{
    GUTIL_SLIST_CONSTRUCTORS(SList, SList<T>)

    virtual T &Enqueue(const T &item){ return SListImp<T>::PushBack(item); }
    virtual void Dequeue(){ SListImp<T>::PopFront(); }
    virtual T &Front(){ return SListImp<T>::Front(); }
    virtual T const &Front() const{ return SListImp<T>::Front(); }
    virtual GUINT32 Size() const{ return SListImp<T>::Length(); }
    virtual void Clear(){ SListImp<T>::Clear(); }

};


template<class T>struct IsMovableType< SListImp<T> >{ enum{ Value = 1 }; };
template<class T, class U>struct IsMovableType< SList<T, U> >{ enum{ Value = 1 }; };


END_NAMESPACE_GUTIL;

#endif // GUTIL_SLIST_H
