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

#ifndef GUTIL_SET_H
#define GUTIL_SET_H

#include "Core/DataObjects/queue.h"
#include "Core/DataObjects/binarysearchtree.h"
GUTIL_BEGIN_CORE_NAMESPACE(DataObjects);


/** Implements an associative container of objects.

    It organizes them efficiently by using a BinarySearchTree.  Can be
    treated as a unique set or a multi-set, depending on if you use
    Insert() or InsertMulti().
*/
template<class T> class Set :
        public Interfaces::IClonable< Set<T> >
{
public:

    class TypeWrapper :
            public BinarySearchTree< DList<T> * >::TypeWrapper,
            public FlexibleTypeComparer<T>
    {
    public:
        inline TypeWrapper(){}
        inline TypeWrapper(int (*cmp)(const T &, const T &))
            :FlexibleTypeComparer<T>(cmp)
        {}

        virtual void Delete(DList<T> **s) const{
            delete *s;
            BinarySearchTree< DList<T> * >::TypeWrapper::Delete(s);
        }
    private:
        int CompareVoid(const void *const lhs, const void *const rhs) const{
            return FlexibleTypeComparer<T>::Compare((*reinterpret_cast< DList<T> const *const *>(lhs))->Front(),
                                                    (*reinterpret_cast< DList<T> const *const *>(rhs))->Front());
        }
    };

    /** Constructs an empty set. */
    inline Set()
        :data(new TypeWrapper),
          m_size(0)
    {}
    /** Conducts a deep copy of the set.
        \note O(N log(N))
    */
    inline Set(const Set<T> &o){
        *this = o;
    }
    /** Constructs a set with your own compare function. */
    inline Set(int (*compare)(const T &, const T &))
        :data(new TypeWrapper(compare)),
          _T_comparer(compare),
          m_size(0)
    {}

    /** Conducts a deep copy of the set.
        \note O(N log(N))
    */
    inline Set<T> &operator = (const Set<T> &o){ return o.CloneTo(*this); }

    /** Conducts a deep copy of the set.  Satisfies the IClonable interface.
        \note O(N log(N))
    */
    virtual Set<T> &CloneTo(Set<T> &o) const{
        GDEBUG("Warning: Copying a Set is not efficient.  Use references and pointers when you can.");
        o.data.Clear();
        o._T_comparer = _T_comparer;
        *static_cast<FlexibleTypeComparer< DList<T> *> *>(o.data.GetTypeWrapper()) = *data.GetTypeWrapper();

        DList<T> const*last_stack(0);
        for(Set<T>::const_iterator iter(data.begin()); iter; ++iter)
        {
            DList<T> const*s(iter.stack());
            if(s != last_stack)
            {
                last_stack = s;

                DList<T> *n = new DList<T>;
                last_stack->CloneTo(*n);
                o.data.Add(n);
            }
        }
        o.m_size = m_size;
        return o;
    }

    /** Inserts the item into the set.  If the item is already present it will be overwritten.
        \note O(log(N))
    */
    inline void Insert(const T &i){ _insert(i, false); }

    /** Inserts the item into the set.  You can have duplicate items.
        \note O(log(N))
    */
    inline void InsertMulti(const T &i){ _insert(i, true);}

    /** Removes the last one of these that you inserted.
        \note O(log(N))
    */
    inline void RemoveOne(const T &i){ _remove(i, false); }

    /** Removes and deletes all of those items from the set, in the reverse order you inserted them (LIFO).
        \note O(log(N) + M), where M is the number of items that will be removed from the set.  If you do
        not have duplicate items then this is O(log(N)).
    */
    inline void RemoveAll(const T &i){ _remove(i, true); }

    /** Does the set contain at least 1 of these?
        \note O(log(N))
    */
    inline bool Contains(const T &i){ return data.Search(i, &_T_comparer); }

    /** Empties the set and cleans up all memory. */
    void Clear(){ data.Clear(); m_size = 0;}

    /** How many items in the set.
        \note O(1)
    */
    inline long Size() const{ return m_size; }

    /** Returns the number of times this item occurs in the set.
        \note O(log(N))
    */
    inline int Count(const T &i){
        const_iterator iter(data.Search(i, &_T_comparer));
        return iter ? iter.stack()->Count() : 0;
    }


    /** Iterates through the set.  Items are traversed in order.
        Duplicate items will be traversed in the order you inserted them.
    */
    class iterator :
            public BinarySearchTree< DList<T> *>::iterator
    {
        friend class Set;
    public:
        inline iterator(){}
        inline iterator(const typename BinarySearchTree< DList<T> * >::iterator &iter)
            :BinarySearchTree< DList<T> *>::iterator(iter)
        {
            // Initialize our stack iterator
            if(*this)
                siter = queue()->begin();
        }
        inline iterator(const iterator &iter)
            :BinarySearchTree< DList<T> *>::iterator(iter),
              siter(iter.siter)
        {}

        inline T &operator *(){ return *siter; }
        inline T *operator ->(){ return &(*siter); }

    protected:

        inline DList<T> *queue(){ return *reinterpret_cast<DList<T> **>(BinarySearchTree< DList<T> *>::iterator::current->Data); }

    private:
        typename DList<T>::iterator siter;

        // Overridden from bst_p
        void advance(){
            if(siter){
                if(!++siter){
                    BinarySearchTree< DList<T> *>::iterator::advance();
                    if(*this)
                        siter = queue()->begin();
                }
            }
        }
    };

    /** Iterates through the set.  Items are traversed in order.
        Duplicate items will be traversed in the order you inserted them.
    */
    class const_iterator :
            public BinarySearchTree< DList<T> *>::iterator
    {
        friend class Set;
    public:
        inline iterator(){}
        inline iterator(const typename BinarySearchTree< DList<T> *>::iterator &iter)
            :BinarySearchTree< DList<T> *>::iterator(iter)
        {
            // Initialize our stack iterator
            if(*this)
                siter = stack()->begin();
        }
        inline iterator(const iterator &iter)
            :BinarySearchTree< DList<T> *>::iterator(iter),
              siter(iter.siter)
        {}

        const T &operator *() const{ return *siter; }
        const T *operator ->() const{ return &(*siter); }

    protected:

        inline DList<T> const*stack(){ return *reinterpret_cast<const DList<T> *const*>(BinarySearchTree< DList<T> *>::iterator::current->Data); }

    private:
        typename DList<T>::const_iterator siter;

        // Overridden from bst_p
        void advance(){
            if(siter){
                if(!++siter){
                    BinarySearchTree< DList<T> *>::iterator::advance();
                    if(*this)
                        siter = stack()->begin();
                }
            }
        }
    };

    /** Returns an iterator at the beginning of the set.  Items are traversed in order.
        \note O(1)
    */
    inline iterator begin(){ return data.begin(); }
    /** Returns an iterator at the beginning of the set.  Items are traversed in order.
        \note O(1)
    */
    inline const_iterator begin() const{ return data.begin(); }
    /** Returns an iterator at the end of the set.
        \note O(1)
    */
    inline iterator end(){ return data.end(); }
    /** Returns an iterator at the end of the set.
        \note O(1)
    */
    inline const_iterator end() const{ return data.end(); }


private:

    BinarySearchTree< DList<T> *> data;

    class KeySearcher :
            public Interfaces::IVoidComparer,
            public FlexibleTypeComparer<T>
    {
    public:
        inline KeySearcher(){}
        inline KeySearcher(int (*cmp)(const T &, const T &))
            :FlexibleTypeComparer<T>(cmp) {}

    private:
        int CompareVoid(const void *const lhs, const void *const rhs) const{
            // Treat the rhs as a type T, lhs as a stack of T
            return Compare((*reinterpret_cast< const DList<T> *const* >(lhs))->Front(),
                           *reinterpret_cast< T const * >(rhs));
        }
    } _T_comparer;

    long m_size;

    void _insert(const T &, bool);
    void _remove(const T &, bool);

};





/** Otherwise a normal set, but by default it allows duplicates. */
template<class T>class MultiSet :
        public Set<T>
{
public:

    /** Overridden to call InsertMulti. */
    inline void Insert(const T &i){ Set<T>::InsertMulti(i); }

};









template<class T>void Set<T>::_insert(const T &i, bool allow_multiples)
{
    Set<T>::iterator iter( data.Search(i, &_T_comparer) );
    if(iter)
    {
        DList<T> *s(iter.queue());
        if(!allow_multiples)
        {
            m_size -= s->Count();
            s->Clear();
        }
        s->Enqueue(i);
    }
    else
    {
        data.Add(new DList<T>(i));
    }
    ++m_size;
}

template<class T>void Set<T>::_remove(const T &i, bool all)
{
    Set<T>::iterator iter( data.Search(i, &_T_comparer) );
    if(iter)
    {
        iter.queue()->Dequeue();
        --m_size;
        int cnt( iter.queue()->Count() );
        if(all || cnt == 0)
        {
            m_size -= cnt;
            data.Remove(iter);
        }
    }
}

GUTIL_END_CORE_NAMESPACE;

#endif // GUTIL_SET_H
